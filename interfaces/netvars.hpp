#ifndef NETVARS_HPP
#define NETVARS_HPP

#include <cstdint>
#include <cstring>
#include <string>
#include <unordered_map>

#include "client.hpp"
#include "../print.hpp"

struct RecvTable;

struct RecvProp {
  const char* m_pVarName;             // 0x00
  int m_RecvType;                     // 0x08
  int m_Flags;                        // 0x0C
  int m_StringBufferSize;             // 0x10
  bool m_bInsideArray;                // 0x14
  uint8_t pad0[3];                    // 0x15
  const void* m_pExtraData;           // 0x18
  RecvProp* m_pArrayProp;             // 0x20
  void* m_ArrayLengthProxy;           // 0x28
  void* m_ProxyFn;                    // 0x30
  void* m_DataTableProxyFn;           // 0x38
  RecvTable* m_pDataTable;            // 0x40
  int m_Offset;                       // 0x48
  int m_ElementStride;                // 0x4C
  int m_nElements;                    // 0x50
  uint8_t pad1[4];                    // 0x54
  const char* m_pParentArrayPropName; // 0x58
};

struct RecvTable {
  RecvProp* m_pProps;                 // 0x00
  int m_nProps;                       // 0x08
  uint8_t pad0[4];                    // 0x0C
  void* m_pDecoder;                   // 0x10
  const char* m_pNetTableName;        // 0x18
  bool m_bInitialized;                // 0x20
  bool m_bInMainList;                 // 0x21
};

struct ClientClass {
  void* m_pCreateFn;                  // 0x00
  void* m_pCreateEventFn;             // 0x08
  const char* m_pNetworkName;         // 0x10
  RecvTable* m_pRecvTable;            // 0x18
  ClientClass* m_pNext;               // 0x20
  int m_ClassID;                      // 0x28
};

class NetVarManager {
public:
  std::unordered_map<std::string, std::unordered_map<std::string, int>> offsets;
  std::unordered_map<std::string, int> class_ids;
  bool initialized = false;

  void dump_table(const char* root_table, RecvTable* table, int base_offset, int depth = 0) {
    if (table == nullptr || table->m_pProps == nullptr || table->m_nProps <= 0 || depth > 16) {
      return;
    }

    const char* current_table = table->m_pNetTableName ? table->m_pNetTableName : root_table;

    for (int i = 0; i < table->m_nProps; ++i) {
      const RecvProp& prop = table->m_pProps[i];
      if (prop.m_pVarName == nullptr) {
        continue;
      }

      // Skip numeric array index sub-properties ("000", "001", ...) except when nested table
      if (prop.m_pVarName[0] >= '0' && prop.m_pVarName[0] <= '9') {
        continue;
      }

      int total_offset = base_offset + prop.m_Offset;

      if (strcmp(prop.m_pVarName, "baseclass") != 0) {
        // Store under both the root table and the immediate table
        if (root_table != nullptr) {
          auto& root_map = offsets[root_table];
          if (root_map.find(prop.m_pVarName) == root_map.end()) {
            root_map[prop.m_pVarName] = total_offset;
          }
        }
        if (current_table != nullptr) {
          auto& cur_map = offsets[current_table];
          if (cur_map.find(prop.m_pVarName) == cur_map.end()) {
            cur_map[prop.m_pVarName] = prop.m_Offset;
          }
        }
      }

      if (prop.m_pDataTable != nullptr) {
        dump_table(root_table, prop.m_pDataTable, total_offset, depth + 1);
      }
    }
  }

  void init(Client* client_interface) {
    if (initialized || client_interface == nullptr) {
      return;
    }

    ClientClass* cc = client_interface->get_all_classes();
    int table_count = 0;
    for (int guard = 0; cc != nullptr && guard < 2048; cc = cc->m_pNext, ++guard) {
      if (cc->m_pNetworkName != nullptr) {
        class_ids[cc->m_pNetworkName] = cc->m_ClassID;
      }
      if (cc->m_pRecvTable != nullptr && cc->m_pRecvTable->m_pNetTableName != nullptr) {
        dump_table(cc->m_pRecvTable->m_pNetTableName, cc->m_pRecvTable, 0);
        ++table_count;
      }
    }

    initialized = true;
    print("[NetVar] Initialized %d RecvTables (%zu unique tables indexed)\n", table_count, offsets.size());
  }

  int get_offset(const char* table_name, const char* prop_name, int fallback) {
    auto table_it = offsets.find(table_name);
    if (table_it != offsets.end()) {
      auto prop_it = table_it->second.find(prop_name);
      if (prop_it != table_it->second.end() && prop_it->second > 0) {
        int resolved = prop_it->second;
        if (resolved != fallback) {
          print("[NetVar] %s::%s updated: 0x%X (was 0x%X)\n", table_name, prop_name, resolved, fallback);
        } else {
          print("[NetVar] %s::%s verified: 0x%X\n", table_name, prop_name, resolved);
        }
        return resolved;
      }
    }

    print("[NetVar] WARNING: %s::%s not found in RecvTables, using fallback 0x%X\n", table_name, prop_name, fallback);
    return fallback;
  }
};

// Function-local static so it is constructed on first use. A plain global would not
// yet be constructed when main.cpp's __attribute__((constructor)) entry() runs, and
// touching its unordered_maps (bucket_count == 0) raises SIGFPE.
inline NetVarManager& get_netvars() {
  static NetVarManager instance;
  return instance;
}

#define NETVAR_OFFSET(table, prop, fallback) \
  ([]() -> int { \
    static int _offset = get_netvars().get_offset(table, prop, fallback); \
    return _offset; \
  }())

#endif
