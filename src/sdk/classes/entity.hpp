#ifndef ENTITY_HPP
#define ENTITY_HPP



#include "core/offsets.hpp"
#include "core/memory.hpp"
#include <string>
#include <string.h>
#include <cstdint>

#include "core/vec.hpp"

#include "sdk/interfaces/entity_list.hpp"
#include "sdk/interfaces/netvars.hpp"

// https://github.com/rei-2/Amalgam/blob/c1c6bf64d739538b48a301ddc5e1a988cb9b479c/Amalgam/src/SDK/Definitions/Definitions.h#L1032
enum class_id {  
  AMMO_OR_HEALTH_PACK = 1,
  DISPENSER = 86,
  SENTRY = 88,
  TELEPORTER = 89,
  ARROW = 122,
  BALL_ORNAMENT = 183,
  PLAYER = 247,
  ARROW_PROJECTILE = 252,
  BALL_OF_FIRE = 253,
  CLEAVER = 254,
  ENERGY_BALL = 255,
  ROCKET = 264,
  SENTRY_ROCKET = 265,
  SPELL_FIREBALL = 266,
  THROWABLE = 276,
  STUN_BALL = 295,
  PILL_OR_STICKY = 217,
  FLARE = 257,
  GRAPPLING_HOOK = 258,
  CROSSBOW_BOLT = 259,
  JAR = 260,
  JAR_GAS = 261,
  JAR_MILK = 262,
  SNIPER_DOT = 118,
  CAPTURE_FLAG = 26,
  OBJECTIVE_RESOURCE = 235,
  WEARABLE_VM = 343,
  WEARABLE = 336,
  WEARABLE_ITEM = 339,
  WEARABLE_ECON = 35,
  WEARABLE_ROBOT_ARM = 342,
  WEARABLE_RAZORBACK = 341,
  WEARABLE_DEMO_SHIELD = 338,
  WEARABLE_LEVELABLE_ITEM = 340,
  WEARABLE_CAMPAIGN_ITEM = 337,
  RESPAWN_ROOM_VISUALIZER = 64,
  BASE_DOOR = 6,
  BASE_PROP_DOOR = 15,
  
  // Negative values are reserved by the hack for entity_cache.hpp
  // Entity::get_class_id() will not return any of these.
  PROJECTILE = -3,
  AMMO = -2,
  HEALTH_PACK = -1,
};

enum class projectile_type {
  NONE = 0,
  ROCKET,
  GRENADE,
  STICKY,
  ARROW,
  FLARE,
  THROWABLE,
};

enum class tf_team {
  UNKNOWN = 0,
  SPECTATOR,
  RED,
  BLU
};

enum pickup_type {
  UNKNOWN = 0,
  MEDKIT,
  AMMOPACK,
};

enum entity_flags {
    FL_ONGROUND = (1 << 0),
    FL_DUCKING = (1 << 1),
    FL_WATERJUMP = (1 << 2),
    FL_ONTRAIN = (1 << 3),
    FL_INRAIN = (1 << 4),
    FL_FROZEN = (1 << 5),
    FL_ATCONTROLS = (1 << 6),
    FL_CLIENT = (1 << 7),
    FL_FAKECLIENT = (1 << 8),
    FL_INWATER = (1 << 9),
    FL_FLY = (1 << 10),
    FL_SWIM = (1 << 11),
    FL_CONVEYOR = (1 << 12),
    FL_NPC = (1 << 13),
    FL_GODMODE = (1 << 14),
    FL_NOTARGET = (1 << 15),
    FL_AIMTARGET = (1 << 16),
    FL_PARTIALGROUND = (1 << 17),
    FL_STATICPROP = (1 << 18),
    FL_GRAPHED = (1 << 19),
    FL_GRENADE = (1 << 20),
    FL_STEPMOVEMENT = (1 << 21),
    FL_DONTTOUCH = (1 << 22),
    FL_BASEVELOCITY = (1 << 23),
    FL_WORLDBRUSH = (1 << 24),
    FL_OBJECT = (1 << 25),
    FL_KILLME = (1 << 26),
    FL_ONFIRE = (1 << 27),
    FL_DISSOLVING = (1 << 28),
    FL_TRANSRAGDOLL = (1 << 29),
    FL_UNBLOCKABLE_BY_PLAYER = (1 << 30)
};

#define STUDIO_NONE 0x00000000
#define STUDIO_RENDER 0x00000001
#define STUDIO_VIEWXFORMATTACHMENTS 0x00000002
#define STUDIO_DRAWTRANSLUCENTSUBMODELS 0x00000004
#define STUDIO_TWOPASS 0x00000008
#define STUDIO_STATIC_LIGHTING 0x00000010
#define STUDIO_WIREFRAME 0x00000020
#define STUDIO_ITEM_BLINK 0x00000040
#define STUDIO_NOSHADOWS 0x00000080
#define STUDIO_WIREFRAME_VCOLLIDE 0x00000100
#define STUDIO_NO_OVERRIDE_FOR_ATTACH 0x00000200
#define STUDIO_GENERATE_STATS 0x01000000
#define STUDIO_SSAODEPTHTEXTURE 0x08000000
#define STUDIO_SHADOWDEPTHTEXTURE 0x40000000
#define STUDIO_TRANSPARENCY 0x80000000

class Entity {
public:  
  int get_owner_entity_handle(void) {
    return mem::read<int>(this, NETVAR_OFFSET("DT_BaseEntity", "m_hOwnerEntity", 0x754));
  }

  Entity* get_owner_entity(void) {
    return entity_list->entity_from_handle(this->get_owner_entity_handle());
  }
  
  Vec3 get_origin(void) {
    // Hardcoded on purpose, do not resolve via NETVAR_OFFSET (see offsets.hpp).
    return mem::read<Vec3>(this, offsets::entity::origin);
  }

  int get_ent_flags(void) {
    return mem::read<int>(this, NETVAR_OFFSET("DT_BasePlayer", "m_fFlags", 0x460));
  }
  
  // TODO: Substitute "void*" with real class type
  void* get_networkable(void) {
    return mem::field_address(this, offsets::entity::networkable);
  }

  void* get_renderable(void) {
    return mem::field_address(this, offsets::entity::renderable);
  }

  enum tf_team get_team(void)  {
    return (enum tf_team)mem::read<int>(this, NETVAR_OFFSET("DT_BaseEntity", "m_iTeamNum", 0xDC));
  }
  
  int get_index(void) {
    void* networkable = get_networkable();
    return mem::call_vfunc<int()>(networkable, 9);
  }
  
  const char* get_model_name(void) {
    const void* model = mem::read<const void*>(this, offsets::entity::model);
    if (model == nullptr) {
      return "";
    }
    const char* name = mem::read<const char*>(model, offsets::entity::model_name);
    return name != nullptr ? name : "";
  }
  
  int draw_model(int flags) {
    void* renderable = this->get_renderable();
    return mem::call_vfunc<int(int)>(renderable, 10, flags);
  }
  
  bool is_dormant(void) {
    void* networkable = get_networkable();
    return mem::call_vfunc<bool()>(networkable, 8);
  }  
  
  void* get_client_class(void) {
    void* networkable = get_networkable();
    if (networkable == nullptr || *(void**)networkable == nullptr) {
      return nullptr;
    }
    return mem::call_vfunc<void*()>(networkable, 2);
  }
  
  class_id get_class_id(void) {
    void* client_class = get_client_class();
    if (client_class == nullptr || (uintptr_t)client_class < 0x10000) {
      return (class_id)0;
    }
    return mem::read<class_id>(client_class, offsets::client_class::class_id);
  }

  const char* get_network_name(void) {
    ClientClass* client_class = (ClientClass*)get_client_class();
    if (client_class == nullptr || (uintptr_t)client_class < 0x10000 || client_class->m_pNetworkName == nullptr) {
      return "";
    }
    return client_class->m_pNetworkName;
  }

  int get_pipebomb_type(void) {
    return mem::read<int>(this, NETVAR_OFFSET("DT_TFProjectile_Pipebomb", "m_iType", 0x8FC));
  }

  projectile_type get_projectile_type(void) {
    switch (this->get_class_id()) {
    case class_id::ROCKET:
    case class_id::SENTRY_ROCKET:
    case class_id::ENERGY_BALL:
      return projectile_type::ROCKET;

    case class_id::PILL_OR_STICKY:
      {
        int pipe_type = this->get_pipebomb_type();
        if (pipe_type == 1 || pipe_type == 2) {
          return projectile_type::STICKY;
        }
        return projectile_type::GRENADE;
      }

    case class_id::ARROW:
    case class_id::ARROW_PROJECTILE:
    case class_id::CROSSBOW_BOLT:
    case class_id::GRAPPLING_HOOK:
      return projectile_type::ARROW;

    case class_id::FLARE:
    case class_id::BALL_OF_FIRE:
    case class_id::SPELL_FIREBALL:
      return projectile_type::FLARE;

    case class_id::JAR:
    case class_id::JAR_GAS:
    case class_id::JAR_MILK:
    case class_id::CLEAVER:
    case class_id::STUN_BALL:
    case class_id::BALL_ORNAMENT:
    case class_id::THROWABLE:
      return projectile_type::THROWABLE;

    default:
      break;
    }

    const char* net_name = this->get_network_name();
    if (net_name[0] == 'C' && net_name[1] == 'T' && net_name[2] == 'F') {
      if (strcmp(net_name, "CTFProjectile_Rocket") == 0 ||
          strcmp(net_name, "CTFProjectile_SentryRocket") == 0 ||
          strcmp(net_name, "CTFProjectile_EnergyBall") == 0) {
        return projectile_type::ROCKET;
      }
      if (strcmp(net_name, "CTFGrenadePipebombProjectile") == 0) {
        int pipe_type = this->get_pipebomb_type();
        return (pipe_type == 1 || pipe_type == 2) ? projectile_type::STICKY : projectile_type::GRENADE;
      }
      if (strcmp(net_name, "CTFProjectile_Arrow") == 0 ||
          strcmp(net_name, "CTFProjectile_HealingBolt") == 0 ||
          strcmp(net_name, "CTFProjectile_GrapplingHook") == 0) {
        return projectile_type::ARROW;
      }
      if (strcmp(net_name, "CTFProjectile_Flare") == 0 ||
          strcmp(net_name, "CTFProjectile_BallOfFire") == 0 ||
          strcmp(net_name, "CTFProjectile_SpellFireball") == 0) {
        return projectile_type::FLARE;
      }
      if (strcmp(net_name, "CTFProjectile_Jar") == 0 ||
          strcmp(net_name, "CTFProjectile_JarMilk") == 0 ||
          strcmp(net_name, "CTFProjectile_JarGas") == 0 ||
          strcmp(net_name, "CTFProjectile_Cleaver") == 0 ||
          strcmp(net_name, "CTFStunBall") == 0 ||
          strcmp(net_name, "CTFBall_Ornament") == 0 ||
          strcmp(net_name, "CTFProjectile_Throwable") == 0) {
        return projectile_type::THROWABLE;
      }
    }

    return projectile_type::NONE;
  }

  int get_tickbase(void) {
    return mem::read<int>(this, NETVAR_OFFSET("DT_BasePlayer", "m_nTickBase", 0x1718));
  }

  void set_tickbase(int tickbase) {
    mem::write<int>(this, NETVAR_OFFSET("DT_BasePlayer", "m_nTickBase", 0x1718), tickbase);
  }

  
  float get_simulation_time(void) {
    return mem::read<float>(this, NETVAR_OFFSET("DT_BaseEntity", "m_flSimulationTime", 0x98));
  }
  
  bool is_building(void) {
    switch (this->get_class_id()) {
    case class_id::SENTRY:
    case class_id::DISPENSER:
    case class_id::TELEPORTER:
      return true;
    }

    return false;
  }

  bool is_wearable(void) {
    switch (this->get_class_id()) {
    case class_id::WEARABLE:
    case class_id::WEARABLE_CAMPAIGN_ITEM:
    case class_id::WEARABLE_DEMO_SHIELD:
    case class_id::WEARABLE_ECON:
    case class_id::WEARABLE_ITEM:
    case class_id::WEARABLE_RAZORBACK:
    case class_id::WEARABLE_VM:
    case class_id::WEARABLE_LEVELABLE_ITEM:
      return true;
    }

    return false;
  }
  
  enum pickup_type get_pickup_type(void) {
    const char* model_name = get_model_name();
    
    if (strstr(model_name, "models/items/ammopack")) {
      return pickup_type::AMMOPACK;
    }

    if (strstr(model_name, "models/items/medkit")                     ||
	strstr(model_name, "models/props_medieval/medieval_meat.mdl") ||
	strstr(model_name, "models/props_halloween/halloween_medkit")
	)
      {
	return pickup_type::MEDKIT;
      }

    return pickup_type::UNKNOWN;
  }

  bool is_base_combat_weapon(void) {
    return mem::call_vfunc<bool()>(this, 191);
  }
  
};

#endif
