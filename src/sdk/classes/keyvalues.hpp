export module sdk.classes.keyvalues;
export import std.compat;
export import core.sys;
export import core.memory;

export extern "C++" {



enum types_t {
  TYPE_NONE = 0,
  TYPE_STRING,
  TYPE_INT,
  TYPE_FLOAT,
  TYPE_PTR,
  TYPE_WSTRING,
  TYPE_COLOR,
  TYPE_UINT64,
  TYPE_NUMTYPES,
};

class KeyValues;
inline KeyValues* (*key_values_constructor_original)(void*, const char*);
inline void (*key_values_set_int_original)(void*, const char*, int);
inline bool (*key_values_load_from_buffer_original)(void*, const char*, const char*, void*, const char*);

inline void* get_key_values_system() {
  static void* (*fn)() = nullptr;
  if (fn == nullptr) {
    void* handle = dlopen("./bin/linux64/libvstdlib.so", RTLD_NOLOAD | RTLD_NOW);
    if (handle != nullptr) {
      fn = reinterpret_cast<void* (*)()>(dlsym(handle, "KeyValuesSystem"));
      dlclose(handle);
    }
    if (fn == nullptr) {
      fn = reinterpret_cast<void* (*)()>(dlsym(RTLD_DEFAULT, "KeyValuesSystem"));
    }
  }
  return fn ? fn() : nullptr;
}

inline void key_values_set_int_hook(void* me, const char* key, int value) {

  /*
  if (strstr(key, "class")) {
    value = 3;
    }*/
  
  key_values_set_int_original(me, key, value);
}

class KeyValues {
public:
  void* operator new(std::size_t size) {
    void* kvs = get_key_values_system();
    if (kvs != nullptr) {
      return mem::call_vfunc<void*(int)>(kvs, 1, static_cast<int>(size));
    }
    return ::operator new(size);
  }

  void operator delete(void* mem) {
    if (mem == nullptr) return;
    void* kvs = get_key_values_system();
    if (kvs != nullptr) {
      mem::call_vfunc<void(void*)>(kvs, 2, mem);
      return;
    }
    ::operator delete(mem);
  }

  KeyValues(const char* name) {
    key_values_constructor_original(this, name);
  }

  KeyValues() {
    
  }

  void set_int(const char* key_name, int value) {
    key_values_set_int_original(this, key_name, value);
  }

  bool load_from_buffer(const char* resource_name, const char* buffer) {
    return key_values_load_from_buffer_original(this, resource_name, buffer, nullptr, nullptr);
  }
  
  
private:
  int m_iKeyName;
  char* m_sValue;
  wchar_t* m_wsValue;

  union {
    int m_iValue;
    float m_flValue;
    void* m_pValue;
    unsigned char m_Color[4];
  };

  char m_iDataType;
  char m_bHasEscapeSequences;
  char m_bEvaluateConditionals;
  char unused[1];

  KeyValues* m_pPeer;
  KeyValues* m_pSub;
  KeyValues* m_pChain;
  void* m_pExpressionGetSymbolProc;
};

static_assert(sizeof(KeyValues) == 0x48, "KeyValues must be 72 (0x48) bytes on 64-bit Linux");


}
