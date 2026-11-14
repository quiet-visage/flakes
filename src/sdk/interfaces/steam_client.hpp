export module sdk.interfaces.steam_client;
export import std.compat;
export import core.sys;
export import core.memory;

export extern "C++" {


class SteamClient {
public:
  /* Steam Pipe Handle */ int create_steam_pipe(void) {
    return mem::call_vfunc<int()>(this, 0);
  }

  /* Steam User Handle */ int connect_to_global_user(int steam_pipe) {
    return mem::call_vfunc<int(int)>(this, 2, steam_pipe);
  }

  /* Steam Friends Interface */ void* get_steam_friends_interface(int steam_user, int steam_pipe, const char* version) {
    return mem::call_vfunc<void*(int, int, const char*)>(this, 8, steam_user, steam_pipe, version);
  }
};

inline SteamClient* steam_client;


}
