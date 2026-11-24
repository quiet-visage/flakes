export module hooks.hooks;
export import std.compat;
export import core.sys;
export import core.print;

export extern "C++" {


inline void* get_interface(const char* lib_path, const char* version) {
  void* lib_handle = dlopen(lib_path, RTLD_NOLOAD | RTLD_NOW);
  if (!lib_handle) {
    print("Failed to load %s\n", lib_path);
    return nullptr;
  }

  print("%s loaded at %p\n", lib_path, lib_handle);
  
  typedef void* (*CreateInterface)(const char*, int*);

  CreateInterface create_interface = (CreateInterface)dlsym(lib_handle, "CreateInterface");
  dlclose(lib_handle);
  
  if (!create_interface) {
    print("Failed to get CreateInterface\n");
    return nullptr;
  }

  print("%s factory found at %p\n", lib_path, create_interface);

  void* interface =  create_interface(version, nullptr);
    
  if (!interface) {
    print("Failed to get %s interface\n", version);
    return nullptr;
  }

  print("%s interface found at %p\n", version, interface);
    
  return interface;
}

inline bool write_to_table(void** vtable, int index, void* func) {
  if (vtable == nullptr || index < 0) {
    print("write_to_table: invalid vtable %p or index %d\n", vtable, index);
    return false;
  }

  const long page_size = sysconf(_SC_PAGESIZE);

  // Compute the page containing the specific vtable[index] entry
  void** entry_addr = &vtable[index];
  void* entry_page = (void*)((uint64_t)entry_addr & ~(page_size - 1));
  print("vfunc table entry %d page found at %p\n", index, entry_page);

  if (mprotect(entry_page, page_size, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
    if (mprotect(entry_page, page_size, PROT_READ | PROT_WRITE) != 0) {
      print("mprotect failed to change page protection\n");
      return false;
    }
  }

  *entry_addr = func;

  // Restore page protection without stripping PROT_EXEC (stripping PROT_EXEC causes SIGSEGV on code pages)
  if (mprotect(entry_page, page_size, PROT_READ | PROT_EXEC) != 0) {
    mprotect(entry_page, page_size, PROT_READ);
  }

  return true;
}

inline bool sdl_hook(void* lib_handle, const char* func_name, void* hook, void** original) {
  if (!lib_handle || !func_name || !hook) {
    return false;
  }

  void* func = dlsym(lib_handle, func_name);

  if (!func) {
    print("Failed to get %s\n", func_name);
    return false;
  }

  print("%s wrapper found at %p\n", func_name, func);

  /* The symbols resolve to a wrapper function. Verify it starts with
   * `jmp *GOT[rip+offset]` (0xFF 0x25) before attempting to calculate
   * the GOT address. Otherwise we would write out-of-bounds to arbitrary memory. */
  unsigned char* b = (unsigned char*)func;
  if (b[0] != 0xFF || b[1] != 0x25) {
    print("WARNING: %s does not begin with jmp *GOT[rip+offset] (bytes: %02X %02X), skipping to avoid crash\n",
          func_name, b[0], b[1]);
    return false;
  }

  int32_t offset = *(int32_t *)((unsigned long)func + 2);
  void** ptr_to_func = (void **)((unsigned long)func + 6 + (intptr_t)offset);
  *original = *ptr_to_func;

  print("Original %s at %p\n", func_name, *original);

  // GOT page may be read-only under Full RELRO, ensure write permissions
  const long page_size = sysconf(_SC_PAGESIZE);
  void* ptr_page = (void*)((uint64_t)ptr_to_func & ~(page_size - 1));
  mprotect(ptr_page, page_size, PROT_READ | PROT_WRITE);

  *ptr_to_func = hook;

  return true;
}

inline bool restore_sdl_hook(void* lib_handle, const char* func_name, void* original) {
  if (!lib_handle || !func_name || !original) {
    return false;
  }

  void *func = dlsym(lib_handle, func_name);

  if (!func) {
    print("Failed to get %s\n", func_name);
    return false;
  }

  unsigned char* b = (unsigned char*)func;
  if (b[0] != 0xFF || b[1] != 0x25) {
    return false;
  }

  int32_t offset = *(int32_t*)((unsigned long)func + 2);
  void** ptr_to_func = (void **)((unsigned long)func + 6 + (intptr_t)offset);

  const long page_size = sysconf(_SC_PAGESIZE);
  void* ptr_page = (void*)((uint64_t)ptr_to_func & ~(page_size - 1));
  mprotect(ptr_page, page_size, PROT_READ | PROT_WRITE);

  *ptr_to_func = original;

  return true;
}

}
