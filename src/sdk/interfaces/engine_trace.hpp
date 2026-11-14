export module sdk.interfaces.engine_trace;
export import std.compat;
export import core.sys;
export import core.memory;
export import core.vec;
export import sdk.classes.entity;

export extern "C++" {




enum trace_type_t {
  TRACE_EVERYTHING = 0,
  TRACE_WORLD_ONLY,
  TRACE_ENTITIES_ONLY,
  TRACE_EVERYTHING_FILTER_PROPS,
};

struct ray_t
{
  struct Vec3_aligned start;	    // starting point, centered within the extents
  struct Vec3_aligned delta;	    // direction + length of the ray
  struct Vec3_aligned start_offset;	// Add this to m_Start to get the actual ray start
  struct Vec3_aligned extents;	    // Describes an axis aligned box extruded along a ray
  bool is_ray;	                    // are the extents zero?
  bool is_swept;	                    // is delta != 0?
};

struct trace_filter {
  void** vtable;
  void* skip;
};

struct cplane_t {
  Vec3 normal;
  float dist;
  unsigned char type;
  unsigned char signbits;
  unsigned char pad[2];
};

struct csurface_t {
  const char* name;
  short surface_props;
  unsigned short flags;
};

struct trace_t {
  Vec3 startpos;
  Vec3 endpos;
  struct cplane_t plane;
  float fraction;
  int contents;
  unsigned short disp_flags;
  bool all_solid;
  bool start_solid;
  float fraction_left_solid;
  struct csurface_t surface;
  int hit_group;
  short physics_bone;
  void* entity;
  int hitbox;

  // bool did_hit() const { return m_fraction < 1.f || m_allsolid || m_start_solid; }
};

inline constexpr auto CONTENTS_EMPTY = 0; // No contents

inline constexpr auto CONTENTS_SOLID = 0x1; // an eye is never valid in a solid
inline constexpr auto CONTENTS_WINDOW = 0x2; // translucent, but not watery (glass)
inline constexpr auto CONTENTS_AUX = 0x4;
inline constexpr auto CONTENTS_GRATE = 0x8; // alpha-tested "grate" textures.  Bullets/sight pass through, but solids don't
inline constexpr auto CONTENTS_SLIME = 0x10;
inline constexpr auto CONTENTS_WATER = 0x20;
inline constexpr auto CONTENTS_BLOCKLOS = 0x40; // block AI line of sight
inline constexpr auto CONTENTS_OPAQUE = 0x80; // things that cannot be seen through (may be non-solid though)
inline constexpr auto LAST_VISIBLE_CONTENTS = 0x80;

inline constexpr auto ALL_VISIBLE_CONTENTS = (LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1));

inline constexpr auto CONTENTS_TESTFOGVOLUME = 0x100;
inline constexpr auto CONTENTS_UNUSED = 0x200;

// unused 
// NOTE: If it's visible, grab from the top + update LAST_VISIBLE_CONTENTS
// if not visible, then grab from the bottom.
inline constexpr auto CONTENTS_UNUSED6 = 0x400;

inline constexpr auto CONTENTS_TEAM1 = 0x800; // per team contents used to differentiate collisions
inline constexpr auto CONTENTS_TEAM2 = 0x1000; // between players and objects on different teams
inline constexpr auto CONTENTS_REDTEAM = CONTENTS_TEAM1;
inline constexpr auto CONTENTS_BLUETEAM = CONTENTS_TEAM2;

// ignore CONTENTS_OPAQUE on surfaces that have SURF_NODRAW
inline constexpr auto CONTENTS_IGNORE_NODRAW_OPAQUE = 0x2000;

// hits entities which are MOVETYPE_PUSH (doors, plats, etc.)
inline constexpr auto CONTENTS_MOVEABLE = 0x4000;

// remaining contents are non-visible, and don't eat brushes
inline constexpr auto CONTENTS_AREAPORTAL = 0x8000;

inline constexpr auto CONTENTS_PLAYERCLIP = 0x10000;
inline constexpr auto CONTENTS_MONSTERCLIP = 0x20000;

// currents can be added to any other contents, and may be mixed
inline constexpr auto CONTENTS_CURRENT_0 = 0x40000;
inline constexpr auto CONTENTS_CURRENT_90 = 0x80000;
inline constexpr auto CONTENTS_CURRENT_180 = 0x100000;
inline constexpr auto CONTENTS_CURRENT_270 = 0x200000;
inline constexpr auto CONTENTS_CURRENT_UP = 0x400000;
inline constexpr auto CONTENTS_CURRENT_DOWN = 0x800000;

inline constexpr auto CONTENTS_ORIGIN = 0x1000000; // removed before bsping an entity

inline constexpr auto CONTENTS_MONSTER = 0x2000000; // should never be on a brush, only in game
inline constexpr auto CONTENTS_DEBRIS = 0x4000000;
inline constexpr auto CONTENTS_DETAIL = 0x8000000; // brushes to be added after vis leafs
inline constexpr auto CONTENTS_TRANSLUCENT = 0x10000000; // auto set if any surface has trans
inline constexpr auto CONTENTS_LADDER = 0x20000000;
inline constexpr auto CONTENTS_HITBOX = 0x40000000; // use accurate hitboxes on trace
inline constexpr auto CONTENTS_NOSTARTSOLID = 0x80000000; // don't skip entities or displacements when starting in solid


inline constexpr auto MASK_ALL = (0xFFFFFFFF);
// everything that is normally solid
inline constexpr auto MASK_SOLID = (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE);
// everything that blocks player movement
inline constexpr auto MASK_PLAYERSOLID = (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE);
// blocks npc movement
inline constexpr auto MASK_NPCSOLID = (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE);
// water physics in these contents
inline constexpr auto MASK_WATER = (CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME);
// everything that blocks lighting
inline constexpr auto MASK_OPAQUE = (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE);
// everything that blocks lighting, but with monsters added.
inline constexpr auto MASK_OPAQUE_AND_NPCS = (MASK_OPAQUE|CONTENTS_MONSTER);
// everything that blocks line of sight for AI
inline constexpr auto MASK_BLOCKLOS = (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS);
// everything that blocks line of sight for AI plus NPCs
inline constexpr auto MASK_BLOCKLOS_AND_NPCS = (MASK_BLOCKLOS|CONTENTS_MONSTER);
// everything that blocks line of sight for players
inline constexpr auto MASK_VISIBLE = (MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE);
// everything that blocks line of sight for players, but with monsters added.
inline constexpr auto MASK_VISIBLE_AND_NPCS = (MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE);
// bullets see these as solid
inline constexpr auto MASK_SHOT = (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX);



//add a namespace or class for these functions and vars
inline bool should_hit_entity(struct trace_filter* interface, Entity* handle_entity, int contents_mask) {
  if (handle_entity == nullptr) return false;
  // IClientUnknown::GetBaseEntity() is at vfunc index 8.
  // For CStaticProp (from engine.so), vfunc[8] returns nullptr; static props should block traces.
  Entity* entity = mem::call_vfunc<Entity*()>(handle_entity, 8);
  if (entity == nullptr) return true;
  if (entity == interface->skip) return false;
  if (entity->get_class_id() == class_id::RESPAWN_ROOM_VISUALIZER) return false;  
  return true;
}

inline enum trace_type_t get_type(struct trace_filter* interface) {
  return TRACE_EVERYTHING;
}

inline void* trace_filter_vtable[2] = { (void*)should_hit_entity, (void*)get_type };

class EngineTrace {
public:
  struct Vec3_aligned Vec3_aligned_subtract(Vec3* a, Vec3* b) {
    struct Vec3_aligned result = {
      .x = a->x - b->x,
      .y = a->y - b->y,
      .z = a->z - b->z
    };

    return result;
  }

  struct Vec3_aligned Vec3_aligned_add(Vec3* a, Vec3* b) {
    struct Vec3_aligned result = {
      .x = a->x + b->x,
      .y = a->y + b->y,
      .z = a->z + b->z
    };

    return result;
  }

  struct Vec3_aligned Vec3_aligned_add(Vec3* a, Vec3_aligned* b) {
    struct Vec3_aligned result = {
      .x = a->x + b->x,
      .y = a->y + b->y,
      .z = a->z + b->z
    };

    return result;
  }

  
  struct ray_t init_ray(Vec3* start, Vec3* end) {
    struct Vec3_aligned delta = Vec3_aligned_subtract(end, start);
    bool is_swept = (delta.x != 0.0f || delta.y != 0.0f || delta.z != 0.0f);

    struct ray_t ray = {
      .start = { start->x, start->y, start->z },
      .delta = { delta.x, delta.y, delta.z },
      .start_offset = { 0.0f, 0.0f, 0.0f },
      .extents = { 0.0f, 0.0f, 0.0f },
      .is_ray = true,
      .is_swept = is_swept
    };

    return ray;
  }
  
  struct ray_t init_ray(Vec3* start, Vec3* end, Vec3* mins, Vec3* maxs) {
    struct ray_t ray;

    ray.delta = Vec3_aligned_subtract(end, start);
    ray.is_swept = (ray.delta.x * ray.delta.x + ray.delta.y * ray.delta.y) != 0;
    ray.extents = Vec3_aligned_subtract(maxs, mins);
    ray.extents.x *= 0.5f;
    ray.extents.y *= 0.5f;
    ray.extents.z *= 0.5f;

    ray.is_ray = (ray.extents.x * ray.extents.x + ray.extents.y * ray.extents.y) == 0;
    ray.start_offset = Vec3_aligned_add(mins, maxs);
    ray.start_offset.x *= 0.5f;
    ray.start_offset.y *= 0.5f;
    ray.start_offset.z *= 0.5f;
    
    ray.start = Vec3_aligned_add(start, &ray.start_offset);
    ray.start_offset.x *= -1.0f;
    ray.start_offset.y *= -1.0f;
    ray.start_offset.z *= -1.0f;
    
    return ray;
  }
  
  void init_trace_filter(struct trace_filter* filter, void* skip) {
    filter->vtable = trace_filter_vtable;
    filter->skip = skip;
  }

  void trace_ray(struct ray_t* ray, unsigned int f_mask, struct trace_filter* p_trace_filter, struct trace_t* p_trace) {
    mem::call_vfunc<void(struct ray_t*, unsigned int, struct trace_filter*, struct trace_t*)>(this, 4, ray, f_mask, p_trace_filter, p_trace);
  }

  void trace_hull(Vec3* start, Vec3* end, Vec3* hull_min, Vec3* hull_max, unsigned int mask, struct trace_t* trace) {
    struct ray_t ray = this->init_ray(start, end, hull_min, hull_max);
    struct trace_filter filter;
    Player* localplayer = entity_list->get_localplayer();
    this->init_trace_filter(&filter, localplayer);

    
    this->trace_ray(&ray, mask, &filter, trace);
  }

};

inline EngineTrace* engine_trace;


}
