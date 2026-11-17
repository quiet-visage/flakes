export module subsystems.vector_targeting.vector_targeting;
export import std.compat;
export import core.sys;
export import subsystems.vector_targeting.vector_targeting_hpp;
export import subsystems.observer_telemetry.observer_telemetry;
export import core.math;
export import sdk.interfaces.client;
export import sdk.interfaces.entity_list;
export import sdk.interfaces.engine;
export import sdk.interfaces.engine_trace;
export import sdk.interfaces.convar_system;
export import sdk.interfaces.global_vars;
export import gui.config;
export import sdk.classes.player;
export import core.entity_cache;
export import core.print;

export extern "C++" {

// -----------------------------------------------------------------------------
// Random Gaussian Generator for Ornstein-Uhlenbeck Stochastic Process
// -----------------------------------------------------------------------------
inline float sample_gaussian() {
  static std::mt19937 rng(424242);
  static std::normal_distribution<float> dist(0.0f, 1.0f);
  return dist(rng);
}

// -----------------------------------------------------------------------------
// 1. Second-Order Damped Harmonic Oscillator
//    \ddot{\theta} + 2 \zeta \omega_n \dot{\theta} + \omega_n^2 (\theta - \theta_{target}) = 0
//    Exact analytical state transition integration guarantees absolute stability,
//    zero numerical drift, and physically faithful overshoot / damping behavior.
// -----------------------------------------------------------------------------
struct SpringDamperState {
  float pos = 0.0f;
  float vel = 0.0f;

  void reset(float initial_pos) {
    pos = initial_pos;
    vel = 0.0f;
  }

  void step(float target, float omega_n, float zeta, float dt, bool is_angle = false) {
    if (dt <= 0.0f) return;
    dt = std::min(dt, 0.05f);
    omega_n = std::max(omega_n, 0.1f);

    float err = is_angle ? remainderf(pos - target, 360.0f) : (pos - target);
    float v = vel;

    float new_err = 0.0f;
    float new_vel = 0.0f;

    if (zeta < 0.999f) {
      // Underdamped (\zeta < 1): The aim slightly overshoots the target and corrects back (flicks)
      float omega_d = omega_n * std::sqrt(1.0f - zeta * zeta);
      float alpha = zeta * omega_n;
      float exp_term = std::exp(-alpha * dt);
      float cos_term = std::cos(omega_d * dt);
      float sin_term = std::sin(omega_d * dt);

      float phi11 = exp_term * (cos_term + (alpha / omega_d) * sin_term);
      float phi12 = exp_term * (sin_term / omega_d);
      float phi21 = -exp_term * ((omega_n * omega_n / omega_d) * sin_term);
      float phi22 = exp_term * (cos_term - (alpha / omega_d) * sin_term);

      new_err = phi11 * err + phi12 * v;
      new_vel = phi21 * err + phi22 * v;
    } else if (zeta > 1.001f) {
      // Overdamped (\zeta > 1): Sluggish, cautious approach without crossing target plane
      float omega_d = omega_n * std::sqrt(zeta * zeta - 1.0f);
      float alpha = zeta * omega_n;
      float r1 = -alpha + omega_d;
      float r2 = -alpha - omega_d;
      float exp1 = std::exp(r1 * dt);
      float exp2 = std::exp(r2 * dt);

      float denom = r2 - r1; // -2 * omega_d
      float phi11 = (r2 * exp1 - r1 * exp2) / denom;
      float phi12 = (exp1 - exp2) / (r1 - r2);
      float phi21 = (r1 * r2 * (exp2 - exp1)) / denom;
      float phi22 = (r2 * exp2 - r1 * exp1) / denom;

      new_err = phi11 * err + phi12 * v;
      new_vel = phi21 * err + phi22 * v;
    } else {
      // Critically Damped (\zeta = 1): Fastest convergence without overshoot
      float exp_term = std::exp(-omega_n * dt);
      float phi11 = exp_term * (1.0f + omega_n * dt);
      float phi12 = exp_term * dt;
      float phi21 = -exp_term * (omega_n * omega_n * dt);
      float phi22 = exp_term * (1.0f - omega_n * dt);

      new_err = phi11 * err + phi12 * v;
      new_vel = phi21 * err + phi22 * v;
    }

    if (is_angle) {
      pos = remainderf(target + new_err, 360.0f);
    } else {
      pos = target + new_err;
    }
    vel = new_vel;
  }
};

// -----------------------------------------------------------------------------
// 2. Minimum-Jerk Trajectory Planning (Flash & Hogan, 1985)
//    Minimizes \int_0^T (\dddot{\theta})^2 dt yielding a 5th-order polynomial:
//    \theta(\tau) = \theta_0 + (\theta_f - \theta_0)(10\tau^3 - 15\tau^4 + 6\tau^5)
//    Bell-shaped velocity profile: \dot{\theta}(\tau) \propto 30\tau^2 - 60\tau^3 + 30\tau^4
//    Dynamic re-planning uses current [pos, vel, accel] as boundary conditions.
// -----------------------------------------------------------------------------
struct MinimumJerkAxis {
  float c0 = 0.0f;
  float c1 = 0.0f;
  float c2 = 0.0f;
  float c3 = 0.0f;
  float c4 = 0.0f;
  float c5 = 0.0f;

  float t_elapsed = 0.0f;
  float duration = 0.22f;
  float current_pos = 0.0f;
  float current_vel = 0.0f;
  float current_accel = 0.0f;
  float target_pos = 0.0f;
  bool is_active = false;

  void plan(float start_pos, float start_vel, float start_accel, float target, float T, bool is_angle = false) {
    target_pos = target;
    duration = std::max(T, 0.02f);
    t_elapsed = 0.0f;
    is_active = true;

    float theta_0 = start_pos;
    float theta_f = target;
    if (is_angle) {
      float d_ang = remainderf(target - start_pos, 360.0f);
      theta_f = start_pos + d_ang;
    }

    float v0 = start_vel;
    float a0 = start_accel;
    float vf = 0.0f;
    float af = 0.0f;

    c0 = theta_0;
    c1 = v0;
    c2 = 0.5f * a0;

    float T2 = duration * duration;
    float T3 = T2 * duration;
    float T4 = T3 * duration;
    float T5 = T4 * duration;

    float D = theta_f - (c0 + c1 * duration + c2 * T2);
    float V = vf - (c1 + 2.0f * c2 * duration);
    float A = af - (2.0f * c2);

    c3 = (10.0f * D - 4.0f * V * duration + 0.5f * A * T2) / T3;
    c4 = (-15.0f * D + 7.0f * V * duration - A * T2) / T4;
    c5 = (6.0f * D - 3.0f * V * duration + 0.5f * A * T2) / T5;

    current_pos = start_pos;
    current_vel = start_vel;
    current_accel = start_accel;
  }

  void step(float dt, bool is_angle = false) {
    if (!is_active) return;
    t_elapsed += dt;

    if (t_elapsed >= duration) {
      current_pos = target_pos;
      if (is_angle) current_pos = remainderf(current_pos, 360.0f);
      current_vel = 0.0f;
      current_accel = 0.0f;
      is_active = false;
      return;
    }

    float t = t_elapsed;
    float t2 = t * t;
    float t3 = t2 * t;
    float t4 = t3 * t;
    float t5 = t4 * t;

    current_pos = c0 + c1 * t + c2 * t2 + c3 * t3 + c4 * t4 + c5 * t5;
    current_vel = c1 + 2.0f * c2 * t + 3.0f * c3 * t2 + 4.0f * c4 * t3 + 5.0f * c5 * t4;
    current_accel = 2.0f * c2 + 6.0f * c3 * t + 12.0f * c4 * t2 + 20.0f * c5 * t3;

    if (is_angle) {
      current_pos = remainderf(current_pos, 360.0f);
    }
  }

  void reset(float pos) {
    current_pos = pos;
    current_vel = 0.0f;
    current_accel = 0.0f;
    is_active = false;
    t_elapsed = 0.0f;
  }
};

// -----------------------------------------------------------------------------
// 3. Signal-Dependent Noise & Fitts's Law (Harris & Wolpert, 1998)
//    + Ornstein-Uhlenbeck (OU) Mean-Reverting Process
//    dx_t = -\theta_{ou} x_t dt + \sigma dW_t
//    High-Velocity Phase (Ballistic): High variance orthogonal to movement vector
//    Low-Velocity Phase (Corrective): Shrinks near target, allowing micro-corrections
// -----------------------------------------------------------------------------
struct OrnsteinUhlenbeckProcess {
  float state = 0.0f;

  void reset() {
    state = 0.0f;
  }

  float step(float theta_ou, float sigma, float dt) {
    if (dt <= 0.0f) return state;
    dt = std::min(dt, 0.05f);
    theta_ou = std::max(theta_ou, 0.1f);

    float exp_term = std::exp(-theta_ou * dt);
    float variance = (sigma * sigma / (2.0f * theta_ou)) * (1.0f - exp_term * exp_term);
    float std_dev = std::sqrt(std::max(variance, 0.0f));

    state = state * exp_term + std_dev * sample_gaussian();
    return state;
  }
};

// -----------------------------------------------------------------------------
// 4. Constant Acceleration Kalman Filter (Target Prediction & Lead)
//    x = [p, v, a]^T
//    x_{k|k-1} = [1  \Delta t  0.5 \Delta t^2]
//                [0     1         \Delta t   ] x_{k-1|k-1}
//                [0     0            1       ]
//    Latency compensation forward projection: p + v*\tau_{delay} + 0.5*a*\tau_{delay}^2
// -----------------------------------------------------------------------------
struct KalmanAxis {
  float p = 0.0f;
  float v = 0.0f;
  float a = 0.0f;

  float P[3][3] = {
    {1.0f, 0.0f, 0.0f},
    {0.0f, 10.0f, 0.0f},
    {0.0f, 0.0f, 100.0f}
  };

  void reset(float initial_pos, float initial_vel = 0.0f) {
    p = initial_pos;
    v = initial_vel;
    a = 0.0f;
    P[0][0] = 1.0f;  P[0][1] = 0.0f;   P[0][2] = 0.0f;
    P[1][0] = 0.0f;  P[1][1] = 10.0f;  P[1][2] = 0.0f;
    P[2][0] = 0.0f;  P[2][1] = 0.0f;   P[2][2] = 100.0f;
  }

  void update(float z, float dt, float q_spectral, float r_meas) {
    if (dt <= 0.0001f) return;
    dt = std::min(dt, 0.05f);

    float dt2 = dt * dt;
    float dt3 = dt2 * dt;
    float dt4 = dt3 * dt;
    float dt5 = dt4 * dt;

    float p_pred = p + v * dt + 0.5f * a * dt2;
    float v_pred = v + a * dt;
    float a_pred = a;

    float Q00 = q_spectral * (dt5 / 20.0f);
    float Q01 = q_spectral * (dt4 / 8.0f);
    float Q02 = q_spectral * (dt3 / 6.0f);
    float Q11 = q_spectral * (dt3 / 3.0f);
    float Q12 = q_spectral * (dt2 / 2.0f);
    float Q22 = q_spectral * dt;

    float M[3][3];
    for (int i = 0; i < 3; ++i) {
      M[0][i] = P[0][i] + dt * P[1][i] + 0.5f * dt2 * P[2][i];
      M[1][i] = P[1][i] + dt * P[2][i];
      M[2][i] = P[2][i];
    }

    float P_pred[3][3];
    for (int j = 0; j < 3; ++j) {
      P_pred[j][0] = M[j][0] + dt * M[j][1] + 0.5f * dt2 * M[j][2];
      P_pred[j][1] = M[j][1] + dt * M[j][2];
      P_pred[j][2] = M[j][2];
    }

    P_pred[0][0] += Q00;
    P_pred[0][1] += Q01;  P_pred[1][0] += Q01;
    P_pred[0][2] += Q02;  P_pred[2][0] += Q02;
    P_pred[1][1] += Q11;
    P_pred[1][2] += Q12;  P_pred[2][1] += Q12;
    P_pred[2][2] += Q22;

    float y = z - p_pred;
    float S = P_pred[0][0] + r_meas;
    if (S < 1e-6f) S = 1e-6f;

    float K0 = P_pred[0][0] / S;
    float K1 = P_pred[1][0] / S;
    float K2 = P_pred[2][0] / S;

    p = p_pred + K0 * y;
    v = v_pred + K1 * y;
    a = a_pred + K2 * y;

    for (int j = 0; j < 3; ++j) {
      P[0][j] = (1.0f - K0) * P_pred[0][j];
      P[1][j] = -K1 * P_pred[0][j] + P_pred[1][j];
      P[2][j] = -K2 * P_pred[0][j] + P_pred[2][j];
    }
  }

  float predict_lead(float tau_delay) const {
    return p + v * tau_delay + 0.5f * a * tau_delay * tau_delay;
  }
};

struct KalmanFilter3D {
  KalmanAxis x;
  KalmanAxis y;
  KalmanAxis z;
  bool initialized = false;

  void reset(Vec3 pos, Vec3 vel = {0, 0, 0}) {
    x.reset(pos.x, vel.x);
    y.reset(pos.y, vel.y);
    z.reset(pos.z, vel.z);
    initialized = true;
  }

  void update(Vec3 pos, float dt, float q_spectral, float r_meas) {
    if (!initialized) {
      reset(pos);
      return;
    }
    x.update(pos.x, dt, q_spectral, r_meas);
    y.update(pos.y, dt, q_spectral, r_meas);
    z.update(pos.z, dt, q_spectral, r_meas);
  }

  Vec3 predict_lead(float tau_delay) const {
    return Vec3{
      x.predict_lead(tau_delay),
      y.predict_lead(tau_delay),
      z.predict_lead(tau_delay)
    };
  }

  float get_accel_magnitude() const {
    return std::sqrt(x.a * x.a + y.a * y.a + z.a * z.a);
  }
};

// -----------------------------------------------------------------------------
// 5. One-Euro Filter (Adaptive Low-Pass Smoothing, Casiez et al., 2012)
//    fc = fc_min + beta * |\dot{\hat{X}}_i|
//    \alpha = 1 / (1 + \tau / T_e), \tau = 1 / (2 \pi fc)
//    Low speed: drops to fc_min, heavily smoothing micro-jitter
//    High speed: cutoff increases via beta, reducing phase lag to near zero
// -----------------------------------------------------------------------------
struct OneEuroFilterAxis {
  bool initialized = false;
  float x_prev = 0.0f;
  float dx_prev = 0.0f;

  float filter(float x, float dt, float min_cutoff, float beta, float d_cutoff = 1.0f, bool is_angle = false) {
    if (!initialized) {
      initialized = true;
      x_prev = x;
      dx_prev = 0.0f;
      return x;
    }
    if (dt <= 0.0001f) return x_prev;
    dt = std::min(dt, 0.05f);

    float diff = is_angle ? remainderf(x - x_prev, 360.0f) : (x - x_prev);
    float dx = diff / dt;

    float alpha_d = compute_alpha(d_cutoff, dt);
    float dx_hat = alpha_d * dx + (1.0f - alpha_d) * dx_prev;
    dx_prev = dx_hat;

    float speed = std::abs(dx_hat);
    float cutoff = min_cutoff + beta * speed;

    float a = compute_alpha(cutoff, dt);
    float result = is_angle ? remainderf(x_prev + a * diff, 360.0f) : (a * x + (1.0f - a) * x_prev);
    x_prev = result;
    return result;
  }

  static float compute_alpha(float cutoff, float dt) {
    cutoff = std::max(cutoff, 0.001f);
    float tau = 1.0f / (2.0f * (float)M_PI * cutoff);
    return 1.0f / (1.0f + tau / dt);
  }

  void reset() {
    initialized = false;
  }
};

// -----------------------------------------------------------------------------
// Biomechanical Aim Controller Context
// -----------------------------------------------------------------------------
struct BiomechanicalAimContext {
  SpringDamperState spring_pitch;
  SpringDamperState spring_yaw;

  MinimumJerkAxis jerk_pitch;
  MinimumJerkAxis jerk_yaw;

  OrnsteinUhlenbeckProcess ou_drift;
  OrnsteinUhlenbeckProcess ou_tremor;

  KalmanFilter3D kalman;

  OneEuroFilterAxis one_euro_pitch;
  OneEuroFilterAxis one_euro_yaw;

  Player* last_target_player = nullptr;
  int last_target_bone = -1;
  Vec3 current_aim_angles = {0.0f, 0.0f, 0.0f};
  bool aiming_active = false;

  void reset(Vec3 view_angles) {
    spring_pitch.reset(view_angles.x);
    spring_yaw.reset(view_angles.y);

    jerk_pitch.reset(view_angles.x);
    jerk_yaw.reset(view_angles.y);

    ou_drift.reset();
    ou_tremor.reset();

    kalman.initialized = false;
    one_euro_pitch.reset();
    one_euro_yaw.reset();

    last_target_player = nullptr;
    last_target_bone = -1;
    current_aim_angles = view_angles;
    aiming_active = false;
  }
};

inline BiomechanicalAimContext bio_aim;

bool is_player_visible(Player* localplayer, Player* entity, int bone) {
  Vec3 target_pos = entity->get_bone_pos(bone);
  Vec3 start_pos  = localplayer->get_shoot_pos();
  
  struct ray_t ray = engine_trace->init_ray(&start_pos, &target_pos);
  struct trace_filter filter;
  engine_trace->init_trace_filter(&filter, localplayer);

  struct trace_t trace_world;
  engine_trace->trace_ray(&ray, MASK_SHOT | CONTENTS_GRATE, &filter, &trace_world);

  wchar_t target_wname[32];
  entity->get_player_name(target_wname);
  char target_name[32];
  std::wcstombs(target_name, target_wname, 32);
  
  if (trace_world.entity == entity || trace_world.fraction > 0.97f /* magic number */) {
    return true;
  }

  return false;
}

void run_vector_targeting(user_cmd* user_cmd, Vec3 original_view_angles) {  
  if (config.targeting.master == false || (config.targeting.ignore_spectated == true && spectated_1st == true)) {
    target_player = nullptr;
    bio_aim.reset(original_view_angles);
    return;
  }

  std::lock_guard<std::recursive_mutex> lock(cache_mutex);

  static int dbg_last_aim = 0;
  bool dbg = debug_every(&dbg_last_aim);

  if (entity_cache[class_id::PLAYER].empty()) {
    if (dbg) print("[DBG targeting] bail: entity_cache[PLAYER] empty\n");
    target_player = nullptr;
    bio_aim.reset(original_view_angles);
    return;
  }
  
  Player* localplayer = entity_list->get_localplayer();
  if (localplayer == nullptr || localplayer->get_lifestate() != 1) {
    if (dbg) print("[DBG targeting] bail: localplayer lifestate invalid\n");
    target_player = nullptr;
    bio_aim.reset(original_view_angles);
    return;
  }

  Weapon* weapon = localplayer->get_weapon();
  if (weapon == nullptr) {
    if (dbg) print("[DBG targeting] bail: weapon == nullptr\n");
    target_player = nullptr;
    bio_aim.reset(original_view_angles);
    return;    
  }

  // Delta time for simulation
  float dt = (float)TICK_INTERVAL;
  if (global_vars != nullptr && global_vars->interval_per_tick > 0.001f) {
    dt = global_vars->interval_per_tick;
  }

  bool friendlyfire = false;
  static Convar* friendlyfirevar = convar_system->find_var("mp_friendlyfire");
  if (friendlyfirevar != nullptr) {
    if (friendlyfirevar->get_int() != 0) {
      friendlyfire = true;
    }
  }

  float smallest_fov_angle = __FLT_MAX__;
  float smallest_distance  = __FLT_MAX__;
  int   smallest_health    = INT_MAX;
  int   largest_health     = INT_MIN;

  Player* best_player = nullptr;
  int     best_bone   = 2;
  Vec3    best_raw_angles = {0, 0, 0};
  Vec3    best_bone_pos = {0, 0, 0};

  for (unsigned int i = 0; i < entity_cache[class_id::PLAYER].size(); ++i) {
    Player* player = (Player*)entity_cache[class_id::PLAYER].at(i);
    
    if (player == nullptr                                                        ||
        player == localplayer                                                    ||
        player->is_dormant() == true                                             || 
        (player->get_team() == localplayer->get_team() && friendlyfire == false) ||
        player->get_lifestate() != 1                                             ||
        player->is_invulnerable() == true                                        ||
        (config.targeting.ignore_friends == true && player->is_friend()))
      {
        continue;
      }
    
    int bone = player->get_tf_class() == tf_class::ENGINEER ? 5 : 2; // Target body by default

    // Target head if it yields higher damage
    if (localplayer->get_tf_class() == tf_class::SNIPER) {
      if (localplayer->is_scoped() && player->get_health() > 50)
        bone = player->get_head_bone();
    } else if (localplayer->get_tf_class() == tf_class::SPY) {
      if (weapon->is_headshot_weapon())
        bone = player->get_head_bone();
    }

    Vec3 bone_pos = player->get_bone_pos(bone);
    Vec3 diff = {
      bone_pos.x - localplayer->get_shoot_pos().x,
      bone_pos.y - localplayer->get_shoot_pos().y,
      bone_pos.z - localplayer->get_shoot_pos().z
    };
      
    float yaw_hyp = std::sqrt((diff.x * diff.x) + (diff.y * diff.y));
    float pitch_angle = std::atan2(diff.z, yaw_hyp) * radpi;
    float yaw_angle = std::atan2(diff.y, diff.x) * radpi;

    Vec3 candidate_angles = {
      .x = -pitch_angle,
      .y = yaw_angle,
      .z = 0
    };
      
    float x_diff = candidate_angles.x - original_view_angles.x;
    float y_diff = candidate_angles.y - original_view_angles.y;

    float x = remainderf(x_diff, 360.0f);
    float y = remainderf(y_diff, 360.0f);

    float clamped_x = x > 89.0f ? 89.0f : x < -89.0f ? -89.0f : x;
    float clamped_y = y > 180.0f ? 180.0f : y < -180.0f ? -180.0f : y;

    bool visible = is_player_visible(localplayer, player, bone);
    float fov = std::hypot(clamped_x, clamped_y);
    float distance = distance_3d(localplayer->get_origin(), player->get_origin());
    int   health = player->get_health();

    if (config.targeting.target_type == VectorTargetingConfig::TargetType::FOV) {
      if (visible == true && fov <= config.targeting.fov && fov < smallest_fov_angle) {
        best_player = player;
        best_bone = bone;
        best_raw_angles = candidate_angles;
        best_bone_pos = bone_pos;
        smallest_fov_angle = fov;
      }
    } else if (config.targeting.target_type == VectorTargetingConfig::TargetType::DISTANCE) {
      if (visible == true && fov <= config.targeting.fov && distance < smallest_distance) {
        best_player = player;
        best_bone = bone;
        best_raw_angles = candidate_angles;
        best_bone_pos = bone_pos;
        smallest_fov_angle = fov;
        smallest_distance = distance;
      }
    } else if (config.targeting.target_type == VectorTargetingConfig::TargetType::LEAST_HEALTH) {
      if (visible == true && fov <= config.targeting.fov && health < smallest_health) {
        best_player = player;
        best_bone = bone;
        best_raw_angles = candidate_angles;
        best_bone_pos = bone_pos;
        smallest_fov_angle = fov;
        smallest_health = health;
      }
    } else if (config.targeting.target_type == VectorTargetingConfig::TargetType::MOST_HEALTH) {
      if (visible == true && fov <= config.targeting.fov && health > largest_health) {
        best_player = player;
        best_bone = bone;
        best_raw_angles = candidate_angles;
        best_bone_pos = bone_pos;
        smallest_fov_angle = fov;
        largest_health = health;
      }    
    }
  }

  target_player = best_player;

  bool scoped_only = ((config.targeting.scoped_only == true && weapon->is_sniper_rifle() && localplayer->is_scoped()) ||
                      config.targeting.scoped_only == false ||
                      !weapon->is_sniper_rifle());
  bool use_key = ((is_button_down(config.targeting.key) && config.targeting.use_key == true) ||
                  config.targeting.use_key == false);

  // Optics auto-unscope
  if (config.targeting.auto_unscope == true &&
      (localplayer->get_tickbase() * (float)TICK_INTERVAL) - localplayer->get_fov_time() >= 1 &&
      localplayer->get_tf_class() == tf_class::SNIPER &&
      localplayer->is_scoped()) {
    user_cmd->buttons |= IN_ATTACK2;
  }

  if (target_player == nullptr || !use_key || !scoped_only) {
    bio_aim.reset(original_view_angles);
    return;
  }

  // Optics auto-scope
  if (config.targeting.auto_scope == true &&
      localplayer->get_tf_class() == tf_class::SNIPER &&
      weapon->is_sniper_rifle() &&
      !localplayer->is_scoped() &&
      weapon->can_primary_attack() &&
      localplayer->get_ground_entity() != nullptr) {
    user_cmd->buttons |= IN_ATTACK2;
  }

  // ---------------------------------------------------------------------------
  // SILENT VECTOR TARGETING
  // Snaps angles instantaneously on attack tick without moving screen
  // ---------------------------------------------------------------------------
  if (config.targeting.silent == true) {
    if (weapon->can_primary_attack() && target_player == best_player) {
      user_cmd->view_angles = (best_raw_angles - localplayer->get_punch_angles());
    }

    if (config.targeting.auto_shoot == true && target_player == best_player && localplayer->can_shoot(target_player)) {
      if (!(user_cmd->buttons & IN_ATTACK2) && scoped_only == true)
        user_cmd->buttons |= IN_ATTACK;
    }
    return;
  }

  // ---------------------------------------------------------------------------
  // NON-SILENT BIOMECHANICAL VECTOR TARGETING
  // Implements:
  // 1. Second-Order Damped Harmonic Oscillator (Spring-Damper)
  // 2. Minimum-Jerk Trajectory Planning (Flash & Hogan 1985)
  // 3. Signal-Dependent Noise & Fitts's Law (Harris & Wolpert 1998) + OU Process
  // 4. Constant Acceleration Kalman Filter with Latency Lead
  // 5. One-Euro Adaptive Low-Pass Filter (Casiez et al., 2012)
  // ---------------------------------------------------------------------------

  // A. Kalman Filter Target Prediction & Latency Compensation
  Vec3 target_bone_world = best_bone_pos;
  float target_accel_mag = 0.0f;

  if (config.targeting.kalman_enabled) {
    bool target_changed = (bio_aim.last_target_player != best_player || bio_aim.last_target_bone != best_bone);
    if (target_changed || !bio_aim.kalman.initialized) {
      bio_aim.kalman.reset(target_bone_world);
    } else {
      bio_aim.kalman.update(target_bone_world, dt, config.targeting.kalman_q, config.targeting.kalman_r);
    }
    target_bone_world = bio_aim.kalman.predict_lead(config.targeting.kalman_latency);
    target_accel_mag = bio_aim.kalman.get_accel_magnitude();
  }

  bio_aim.last_target_player = best_player;
  bio_aim.last_target_bone = best_bone;

  // B. Target Angle & Punch Compensation
  Vec3 shoot_pos = localplayer->get_shoot_pos();
  Vec3 lead_diff = {
    target_bone_world.x - shoot_pos.x,
    target_bone_world.y - shoot_pos.y,
    target_bone_world.z - shoot_pos.z
  };

  float lead_yaw_hyp = std::sqrt((lead_diff.x * lead_diff.x) + (lead_diff.y * lead_diff.y));
  float raw_target_pitch = -std::atan2(lead_diff.z, lead_yaw_hyp) * radpi;
  float raw_target_yaw   = std::atan2(lead_diff.y, lead_diff.x) * radpi;

  Vec3 punch = localplayer->get_punch_angles();
  float target_pitch = clampf(raw_target_pitch - punch.x, -89.0f, 89.0f);
  float target_yaw   = remainderf(raw_target_yaw - punch.y, 360.0f);

  if (!bio_aim.aiming_active) {
    bio_aim.reset(original_view_angles);
    bio_aim.aiming_active = true;
  }

  float aim_pitch = bio_aim.current_aim_angles.x;
  float aim_yaw   = bio_aim.current_aim_angles.y;
  float vel_p = 0.0f;
  float vel_y = 0.0f;

  // C. Aim Motion Generation
  if (config.targeting.bio_model == VectorTargetingConfig::BiomechanicalModel::SPRING_DAMPER) {
    // 1. Second-Order Damped Harmonic Oscillator
    bio_aim.spring_pitch.step(target_pitch, config.targeting.spring_omega_n, config.targeting.spring_zeta, dt, false);
    bio_aim.spring_yaw.step(target_yaw, config.targeting.spring_omega_n, config.targeting.spring_zeta, dt, true);

    aim_pitch = bio_aim.spring_pitch.pos;
    aim_yaw   = bio_aim.spring_yaw.pos;
    vel_p     = bio_aim.spring_pitch.vel;
    vel_y     = bio_aim.spring_yaw.vel;

  } else if (config.targeting.bio_model == VectorTargetingConfig::BiomechanicalModel::MINIMUM_JERK) {
    // 2. Minimum-Jerk Trajectory Planning with Dynamic Re-planning
    bool replan = !bio_aim.jerk_pitch.is_active ||
                  !bio_aim.jerk_yaw.is_active ||
                  (target_accel_mag > config.targeting.jerk_replan_accel);

    if (replan) {
      bio_aim.jerk_pitch.plan(bio_aim.jerk_pitch.current_pos, bio_aim.jerk_pitch.current_vel, bio_aim.jerk_pitch.current_accel,
                              target_pitch, config.targeting.jerk_duration, false);
      bio_aim.jerk_yaw.plan(bio_aim.jerk_yaw.current_pos, bio_aim.jerk_yaw.current_vel, bio_aim.jerk_yaw.current_accel,
                            target_yaw, config.targeting.jerk_duration, true);
    }

    bio_aim.jerk_pitch.step(dt, false);
    bio_aim.jerk_yaw.step(dt, true);

    aim_pitch = bio_aim.jerk_pitch.current_pos;
    aim_yaw   = bio_aim.jerk_yaw.current_pos;
    vel_p     = bio_aim.jerk_pitch.current_vel;
    vel_y     = bio_aim.jerk_yaw.current_vel;

  } else if (config.targeting.bio_model == VectorTargetingConfig::BiomechanicalModel::HYBRID) {
    // Hybrid: Minimum-Jerk feeds smooth reference trajectory to Spring-Damper
    bool replan = !bio_aim.jerk_pitch.is_active ||
                  !bio_aim.jerk_yaw.is_active ||
                  (target_accel_mag > config.targeting.jerk_replan_accel);

    if (replan) {
      bio_aim.jerk_pitch.plan(bio_aim.jerk_pitch.current_pos, bio_aim.jerk_pitch.current_vel, bio_aim.jerk_pitch.current_accel,
                              target_pitch, config.targeting.jerk_duration, false);
      bio_aim.jerk_yaw.plan(bio_aim.jerk_yaw.current_pos, bio_aim.jerk_yaw.current_vel, bio_aim.jerk_yaw.current_accel,
                            target_yaw, config.targeting.jerk_duration, true);
    }

    bio_aim.jerk_pitch.step(dt, false);
    bio_aim.jerk_yaw.step(dt, true);

    bio_aim.spring_pitch.step(bio_aim.jerk_pitch.current_pos, config.targeting.spring_omega_n, config.targeting.spring_zeta, dt, false);
    bio_aim.spring_yaw.step(bio_aim.jerk_yaw.current_pos, config.targeting.spring_omega_n, config.targeting.spring_zeta, dt, true);

    aim_pitch = bio_aim.spring_pitch.pos;
    aim_yaw   = bio_aim.spring_yaw.pos;
    vel_p     = bio_aim.spring_pitch.vel;
    vel_y     = bio_aim.spring_yaw.vel;
  }

  // D. Signal-Dependent Noise & Fitts's Law + Ornstein-Uhlenbeck Process
  if (config.targeting.ou_noise_enabled) {
    float speed = std::hypot(vel_p, vel_y);
    float ou1 = bio_aim.ou_drift.step(config.targeting.ou_theta, config.targeting.ou_sigma, dt);
    float ou2 = bio_aim.ou_tremor.step(config.targeting.ou_theta, config.targeting.ou_sigma, dt);

    float noise_p = 0.0f;
    float noise_y = 0.0f;

    if (speed > 0.05f) {
      float u_p = vel_p / speed;
      float u_y = vel_y / speed;
      float perp_p = -u_y;
      float perp_y = u_p;

      // High-velocity ballistic phase: large lateral drift orthogonal to trajectory
      // Low-velocity corrective phase: shrinks as speed -> 0
      noise_p = config.targeting.ou_scale * speed * (1.2f * ou1 * perp_p + 0.4f * ou2 * u_p);
      noise_y = config.targeting.ou_scale * speed * (1.2f * ou1 * perp_y + 0.4f * ou2 * u_y);
    } else {
      // Resting micro-tremor
      noise_p = config.targeting.ou_scale * 0.05f * ou1 * 0.2f;
      noise_y = config.targeting.ou_scale * 0.05f * ou2 * 0.2f;
    }

    aim_pitch += noise_p;
    aim_yaw = remainderf(aim_yaw + noise_y, 360.0f);
  }

  // E. One-Euro Adaptive Low-Pass Filter
  if (config.targeting.one_euro_enabled) {
    aim_pitch = bio_aim.one_euro_pitch.filter(aim_pitch, dt,
                                              config.targeting.one_euro_min_cutoff,
                                              config.targeting.one_euro_beta,
                                              1.0f, false);
    aim_yaw   = bio_aim.one_euro_yaw.filter(aim_yaw, dt,
                                            config.targeting.one_euro_min_cutoff,
                                            config.targeting.one_euro_beta,
                                            1.0f, true);
  }

  // Clamping to valid Source Engine view angle ranges
  aim_pitch = clampf(aim_pitch, -89.0f, 89.0f);
  aim_yaw   = remainderf(aim_yaw, 360.0f);

  bio_aim.current_aim_angles = { aim_pitch, aim_yaw, 0.0f };

  user_cmd->view_angles.x = aim_pitch;
  user_cmd->view_angles.y = aim_yaw;
  user_cmd->view_angles.z = 0.0f;

  engine->set_view_angles(user_cmd->view_angles);

  // F. Autonomous Fire Dispatch
  // Only dispatches when the crosshair has converged onto the target within angular tolerance
  float err_pitch = std::abs(aim_pitch - target_pitch);
  float err_yaw   = std::abs(remainderf(aim_yaw - target_yaw, 360.0f));
  float total_angular_error = std::hypot(err_pitch, err_yaw);

  if (config.targeting.auto_shoot == true &&
      total_angular_error <= 2.0f &&
      weapon->can_primary_attack() &&
      localplayer->can_shoot(best_player)) {
    if (!(user_cmd->buttons & IN_ATTACK2) && scoped_only == true) {
      user_cmd->buttons |= IN_ATTACK;
    }
  }
}

}
