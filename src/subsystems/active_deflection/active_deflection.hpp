export module subsystems.active_deflection.active_deflection_hpp;
export import std.compat;
export import core.sys;
export import sdk.interfaces.client;
export import core.vec;

export extern "C++" {


void run_active_deflection(user_cmd* user_cmd, Vec3 original_view_angles);


}
