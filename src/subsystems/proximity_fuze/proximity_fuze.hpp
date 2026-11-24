export module subsystems.proximity_fuze.proximity_fuze_hpp;
export import std.compat;
export import core.sys;
export import sdk.interfaces.client;
export import core.vec;

export extern "C++" {

void run_proximity_fuze(user_cmd* user_cmd, Vec3 original_view_angles);

}
