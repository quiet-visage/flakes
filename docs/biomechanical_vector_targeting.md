# Biomechanical Kinematics, Control Theory & Signal Filtering in Vector Targeting

This document details the mathematical theory, biomechanical models, and signal processing architecture implemented in **Flakes** for **non-silent vector targeting**.

In real-time 3D environments, standard vector targeting (aimbots) typically relies on instantaneous view-angle overwrite (silent aim) or rudimentary linear/exponential interpolation. While silent aim operates purely at the packet level, visible (non-silent) aiming that uses naïve smoothing looks synthetic, robotic, and easily detectable due to constant angular velocity plateaus, infinite jerk acceleration spikes, and unnatural straight-line trajectories.

To replicate organic human neuromuscular control, Flakes implements a multi-stage biomechanical motor control pipeline grounded in peer-reviewed neuroscience, human-computer interaction (HCI), and optimal control literature.

---

## Architecture Pipeline

```
  ┌──────────────────────────────────────────────────────────┐
  │         Target Entity & Bone Selection (Raycast)         │
  └────────────────────────────┬─────────────────────────────┘
                               │ Raw Position [x, y, z]
                               ▼
  ┌──────────────────────────────────────────────────────────┐
  │    Constant Acceleration Kalman Filter (3D Kinematics)   │
  │    • State: [p, v, a]^T                                  │
  │    • Process Noise (Q) & Measurement Noise (R)           │
  │    • Latency Lead Projection: p + v*τ_delay + 0.5*a*τ^2  │
  └────────────────────────────┬─────────────────────────────┘
                               │ Latency-Compensated Position
                               ▼
  ┌──────────────────────────────────────────────────────────┐
  │   Angle Solver & Recoil Compensation (- punch_angles)    │
  └────────────────────────────┬─────────────────────────────┘
                               │ Target Angles [Pitch, Yaw]
                               ▼
  ┌──────────────────────────────────────────────────────────┐
  │            Biomechanical Actuation Controller            │
  │  ┌───────────────────────┐   ┌────────────────────────┐  │
  │  │ Spring-Damper (2nd-O) │   │ Minimum-Jerk Traj.     │  │
  │  │ Exact Matrix Expo.    │OR │ Flash & Hogan (1985)   │  │
  │  │ (ω_n, ζ overshoot)    │   │ Dynamic Re-planning    │  │
  │  └───────────────────────┘   └────────────────────────┘  │
  └────────────────────────────┬─────────────────────────────┘
                               │ Commanded Angle & Angular Velocity [θ, θ_dot]
                               ▼
  ┌──────────────────────────────────────────────────────────┐
  │     Signal-Dependent Noise (Harris & Wolpert, 1998)      │
  │     • Variance scales proportionally with |θ_dot|        │
  │     • Ornstein-Uhlenbeck (OU) Mean-Reverting Drift       │
  │     • Orthogonal Lateral Drift during Ballistic Flicks   │
  └────────────────────────────┬─────────────────────────────┘
                               │ Perturbed Angles
                               ▼
  ┌──────────────────────────────────────────────────────────┐
  │      One-Euro Adaptive Filter (Casiez et al., 2012)      │
  │      • Speed-adaptive cutoff: fc = fc_min + β*|dx|       │
  │      • Sub-pixel jitter elimination at low speed         │
  │      • Zero phase-lag during high-speed flicks           │
  └────────────────────────────┬─────────────────────────────┘
                               │ Final View Angles [Pitch, Yaw]
                               ▼
  ┌──────────────────────────────────────────────────────────┐
  │  CUserCmd Write & Client Engine Sync (SetViewAngles)     │
  │  Autonomous Fire Dispatch (Convergence Gating ≤ 2.0°)    │
  └──────────────────────────────────────────────────────────┘
```

---

## 1. Second-Order Damped Harmonic Oscillator (Spring-Damper)

### Theoretical Model
Aiming movements possess mass, inertia, and muscular stiffness/damping. The angular trajectory towards a target angle $\theta_{\text{target}}$ is modeled by a second-order linear differential equation:

$$\ddot{\theta}(t) + 2 \zeta \omega_n \dot{\theta}(t) + \omega_n^2 (\theta(t) - \theta_{\text{target}}) = 0$$

Defining error $e(t) = \theta(t) - \theta_{\text{target}}$, the homogeneous equation is:

$$\ddot{e}(t) + 2 \zeta \omega_n \dot{e}(t) + \omega_n^2 e(t) = 0$$

- **Natural Frequency ($\omega_n$)**: Controls responsiveness and snap speed (radians/sec). Higher $\omega_n$ accelerates the initial flick.
- **Damping Ratio ($\zeta$)**: Dictates qualitative convergence and overshoot dynamics:
  - **Underdamped ($\zeta < 1$)**: Aim rapidly snaps, slightly overshoots the target plane, and oscillates back to settle. This faithfully replicates high-speed competitive flick shots where human muscle contraction momentarily overshoots before agonist-antagonist deceleration catches it.
  - **Critically Damped ($\zeta = 1$)**: Mathematically fastest convergence without exceeding the target plane.
  - **Overdamped ($\zeta > 1$)**: Sluggish, asymptotic approach without overshoot, mimicking cautious tracking.

### Unconditionally Stable Exact Analytical Integration
Standard explicit Euler integration ($v_{k+1} = v_k + a_k \Delta t$) easily becomes unstable or suffers from energy blowup when $\omega_n \Delta t > 2$ or under variable tick rates.

To guarantee absolute mathematical stability without artificial damping, Flakes evaluates the **exact analytical state-transition matrix** $\mathbf{\Phi}(\Delta t)$ derived from the continuous-time ODE solution:

$$\begin{bmatrix} e(t + \Delta t) \\ \dot{e}(t + \Delta t) \end{bmatrix} = \begin{bmatrix} \Phi_{11} & \Phi_{12} \\ \Phi_{21} & \Phi_{22} \end{bmatrix} \begin{bmatrix} e(t) \\ \dot{e}(t) \end{bmatrix}$$

#### Underdamped Case ($\zeta < 1$):
Let $\omega_d = \omega_n \sqrt{1 - \zeta^2}$ and $\alpha = \zeta \omega_n$:

$$\Phi_{11} = e^{-\alpha \Delta t} \left( \cos(\omega_d \Delta t) + \frac{\alpha}{\omega_d} \sin(\omega_d \Delta t) \right)$$
$$\Phi_{12} = e^{-\alpha \Delta t} \left( \frac{1}{\omega_d} \sin(\omega_d \Delta t) \right)$$
$$\Phi_{21} = -e^{-\alpha \Delta t} \left( \frac{\omega_n^2}{\omega_d} \sin(\omega_d \Delta t) \right)$$
$$\Phi_{22} = e^{-\alpha \Delta t} \left( \cos(\omega_d \Delta t) - \frac{\alpha}{\omega_d} \sin(\omega_d \Delta t) \right)$$

#### Overdamped Case ($\zeta > 1$):
Let $\omega_d = \omega_n \sqrt{\zeta^2 - 1}$, $\alpha = \zeta \omega_n$, $r_1 = -\alpha + \omega_d$, $r_2 = -\alpha - \omega_d$:

$$\Phi_{11} = \frac{r_2 e^{r_1 \Delta t} - r_1 e^{r_2 \Delta t}}{r_2 - r_1}, \quad \Phi_{12} = \frac{e^{r_1 \Delta t} - e^{r_2 \Delta t}}{r_1 - r_2}$$
$$\Phi_{21} = \frac{r_1 r_2 (e^{r_2 \Delta t} - e^{r_1 \Delta t})}{r_2 - r_1}, \quad \Phi_{22} = \frac{r_2 e^{r_2 \Delta t} - r_1 e^{r_1 \Delta t}}{r_2 - r_1}$$

#### Critically Damped Case ($\zeta = 1$):
$$\Phi_{11} = e^{-\omega_n \Delta t} (1 + \omega_n \Delta t), \quad \Phi_{12} = e^{-\omega_n \Delta t} \Delta t$$
$$\Phi_{21} = -e^{-\omega_n \Delta t} \omega_n^2 \Delta t, \quad \Phi_{22} = e^{-\omega_n \Delta t} (1 - \omega_n \Delta t)$$

Because $\det(\mathbf{\Phi}) = e^{-2 \zeta \omega_n \Delta t} < 1$ for all $\Delta t > 0$, this integration is **unconditionally stable**, preserves the exact continuous physics, and is immune to numerical degradation.

---

## 2. Minimum-Jerk Trajectory Planning (Flash & Hogan, 1985)

### Biomechanical Foundation
In human motor control literature, voluntary point-to-point arm and head movements minimize the integral of squared jerk (the third time derivative of position, $\dddot{\theta}$) over total movement duration $T$:

$$J = \int_{0}^{T} \left( \frac{d^3\theta}{dt^3} \right)^2 dt$$

Solving the Euler-Lagrange variational problem $\frac{d^6\theta}{dt^6} = 0$ yields a 5th-order polynomial:

$$\theta(t) = c_0 + c_1 t + c_2 t^2 + c_3 t^3 + c_4 t^4 + c_5 t^5$$

For resting boundary conditions ($\dot{\theta}(0) = \ddot{\theta}(0) = \dot{\theta}(T) = \ddot{\theta}(T) = 0$) at normalized time $\tau = \frac{t}{T} \in [0, 1]$:

$$\theta(\tau) = \theta_0 + (\theta_f - \theta_0) \left( 10\tau^3 - 15\tau^4 + 6\tau^5 \right)$$

### Bell-Shaped Velocity Profile
Differentiating with respect to $\tau$ produces the classic unimodal bell-shaped velocity curve observed in human reaching kinematics:

$$\dot{\theta}(\tau) = \frac{\theta_f - \theta_0}{T} \left( 30\tau^2 - 60\tau^3 + 30\tau^4 \right)$$

The movement begins with zero acceleration, smoothly achieves peak velocity exactly mid-flight ($\tau = 0.5$), and decelerates smoothly to zero at the target.

### Dynamic Re-planning with Non-Zero Boundary Conditions
Because targets in real-time environments continuously accelerate and strafe, the trajectory cannot assume zero initial velocity or acceleration mid-flight. When target acceleration exceeds the re-planning threshold ($a_{\text{target}} > a_{\text{replan}}$), the target switches, or $t > T$, Flakes dynamically recalculates the 5th-order polynomial with arbitrary initial state $[\theta_0, v_0, a_0]$ and terminal state $[\theta_f, v_f = 0, a_f = 0]$:

$$c_0 = \theta_0, \quad c_1 = v_0, \quad c_2 = \frac{1}{2} a_0$$

Letting $\Delta = \theta_f - (c_0 + c_1 T + c_2 T^2)$, $\Delta v = -(c_1 + 2 c_2 T)$, and $\Delta a = -2 c_2$:

$$c_3 = \frac{10\Delta - 4 \Delta v T + \frac{1}{2}\Delta a T^2}{T^3}$$
$$c_4 = \frac{-15\Delta + 7 \Delta v T - \Delta a T^2}{T^4}$$
$$c_5 = \frac{6\Delta - 3 \Delta v T + \frac{1}{2}\Delta a T^2}{T^5}$$

This guarantees $C^2$ continuity: crosshair position, velocity, and acceleration transition seamlessly without jerky discontinuities.

---

## 3. Signal-Dependent Noise & Fitts’s Law (Harris & Wolpert, 1998)

### Neuromuscular Noise Variance Scaling
Human motor output is fundamentally corrupted by signal-dependent noise: the standard deviation of motor noise is proportional to the control signal magnitude (velocity $|\dot{\theta}|$ and acceleration $|\ddot{\theta}|$):

$$\sigma_{\text{neuromuscular}} \propto |\dot{\theta}|$$

This principle explains Fitts's Law: higher velocity commands induce wider trajectory variance, necessitating a terminal corrective deceleration phase.

- **High-Velocity Phase (Ballistic)**: Large angular velocities generate high noise variance. Crucially, human flicks exhibit lateral drift orthogonal to the primary movement axis rather than uniform isotropic noise.
- **Low-Velocity Phase (Corrective)**: As velocity drops near the target, noise variance collapses, allowing sub-pixel micro-corrections and rock-steady holding.

### Ornstein-Uhlenbeck (OU) Mean-Reverting Process
Uncorrelated white Gaussian noise looks like high-frequency artificial jitter. Human physiological micro-tremor is temporally correlated and mean-reverting, described by the Ornstein-Uhlenbeck stochastic differential equation:

$$dx_t = -\theta_{\text{ou}} x_t \, dt + \sigma_{\text{ou}} \, dW_t$$

Where:
- $\theta_{\text{ou}}$ is the mean-reversion rate (restoring force towards zero).
- $\sigma_{\text{ou}}$ is the volatility parameter.
- $W_t$ is a Wiener process (Brownian motion).

The exact discrete-time distribution for step $\Delta t$ is:

$$x_{t+\Delta t} = x_t e^{-\theta_{\text{ou}} \Delta t} + \sigma_{\text{ou}} \sqrt{\frac{1 - e^{-2\theta_{\text{ou}}\Delta t}}{2\theta_{\text{ou}}}} \, \xi, \quad \xi \sim \mathcal{N}(0, 1)$$

### Directional Lateral Drift Decomposition
Flakes decomposes the velocity vector $\mathbf{v} = [\dot{\theta}_{\text{pitch}}, \dot{\theta}_{\text{yaw}}]^T$ into normalized parallel ($\hat{\mathbf{u}}$) and orthogonal ($\hat{\mathbf{u}}_{\perp}$) components:

$$\hat{\mathbf{u}} = \frac{\mathbf{v}}{\|\mathbf{v}\|}, \quad \hat{\mathbf{u}}_{\perp} = \begin{bmatrix} -\hat{u}_y \\ \hat{u}_x \end{bmatrix}$$

The resulting noise perturbation vector added to the aim angle is:

$$\mathbf{w} = \text{scale} \cdot \|\mathbf{v}\| \cdot \left( 1.2 \, x_{\text{drift}} \, \hat{\mathbf{u}}_{\perp} + 0.4 \, x_{\text{tremor}} \, \hat{\mathbf{u}} \right)$$

This creates natural, organic arc deviations during high-speed flicking that smoothly vanish as the crosshair locks onto the target.

---

## 4. Constant Acceleration Kalman Filter (Target Prediction & Lead)

Raw entity coordinate updates from server ticks suffer from network latency, tick-rate quantization (66.67 Hz), packet jitter, and human reaction latency ($\approx 150\text{–}250\text{ ms}$).

### Kinematic State Formulation
Flakes employs a 3D Constant Acceleration Kalman Filter tracking the state vector $\mathbf{x} = [p, v, a]^T \in \mathbb{R}^3$ along each spatial coordinate axis:

$$\mathbf{x}_{k|k-1} = \mathbf{F} \mathbf{x}_{k-1|k-1}, \quad \mathbf{F} = \begin{bmatrix} 1 & \Delta t & \frac{1}{2}\Delta t^2 \\ 0 & 1 & \Delta t \\ 0 & 0 & 1 \end{bmatrix}$$

### Process Noise Covariance ($\mathbf{Q}$)
Assuming a continuous white noise jerk model with spectral density $q$:

$$\mathbf{Q} = q \begin{bmatrix} \frac{\Delta t^5}{20} & \frac{\Delta t^4}{8} & \frac{\Delta t^3}{6} \\ \frac{\Delta t^4}{8} & \frac{\Delta t^3}{3} & \frac{\Delta t^2}{2} \\ \frac{\Delta t^3}{6} & \frac{\Delta t^2}{2} & \Delta t \end{bmatrix}$$

- **Tuning $Q$**: Higher $Q$ allows the filter to rapidly track sudden evasive strafes (AD-spamming) at the cost of slight velocity noise; lower $Q$ produces smoother, inertia-filtered trajectories.
- **Measurement Noise ($R$)**: Represents bone positioning jitter and network packet variance.

### Latency Compensation Lead
To ensure the controller aims where the target will be when the command reaches the server, the filtered state is projected forward by reaction and network delay $\tau_{\text{delay}}$:

$$\mathbf{p}_{\text{lead}} = p + v \tau_{\text{delay}} + \frac{1}{2} a \tau_{\text{delay}}^2$$

---

## 5. One-Euro Filter (Adaptive Low-Pass Smoothing, Casiez et al., 2012)

The One-Euro Filter is an adaptive first-order low-pass filter widely utilized in virtual reality tracking and human-computer interfaces to resolve the fundamental trade-off between jitter elimination and lag.

### Dynamic Cutoff Frequency
The cutoff frequency $f_c$ scales linearly with the magnitude of the filtered derivative (speed) $|\dot{\hat{X}}_i|$:

$$f_c = f_{c,\min} + \beta |\dot{\hat{X}}_i|$$

$$\alpha = \frac{1}{1 + \frac{\tau}{T_e}}, \quad \tau = \frac{1}{2\pi f_c}$$

$$\hat{X}_i = \alpha X_i + (1 - \alpha) \hat{X}_{i-1}$$

- **Low Speed ($|\dot{\hat{X}}| \to 0$)**: Cutoff frequency drops to $f_{c,\min}$, heavily smoothing sensor and sub-pixel jitter.
- **High Speed ($|\dot{\hat{X}}| \gg 0$)**: Cutoff frequency increases proportionally via slope parameter $\beta$, reducing filter lag to near zero during rapid flick phases.

### Circular Angle Arithmetic
For yaw angles spanning $[-180^\circ, 180^\circ]$, the derivative and smoothing steps use shortest circular differences:

$$\Delta X = \text{remainderf}(X_i - \hat{X}_{i-1}, 360.0^\circ)$$
$$\hat{X}_i = \text{remainderf}(\hat{X}_{i-1} + \alpha \Delta X, 360.0^\circ)$$

This prevents 360-degree wrap-around filter anomalies when crossing the $\pm 180^\circ$ boundary.

---

## Summary of Models & Parameter Guide

| Model | Primary Behavior | Best Used For | Key Tuning Parameters |
| :--- | :--- | :--- | :--- |
| **Spring-Damper (2nd-Order)** | Inertia, physical momentum, tunable overshoot | Flicks & organic deceleration | Natural frequency ($\omega_n \in [1, 50]$ rad/s), damping ratio ($\zeta \in [0.1, 2.5]$) |
| **Minimum-Jerk** | Bell-shaped velocity curve, zero-jerk profile | Point-to-point ballistic transitions | Movement duration ($T \in [0.05, 1.0]$ s), re-plan threshold ($a_{\text{thresh}} \in [10, 500]$ deg/s²) |
| **Ornstein-Uhlenbeck** | Temporally correlated drift and tremor | Breaking artificial straight-line paths | Mean-reversion rate ($\theta_{\text{ou}} \in [1, 30]$), volatility ($\sigma \in [0, 5]$), noise gain |
| **Kalman Filter** | State estimation & latency lead prediction | Moving/strafing target prediction | Process noise ($Q \in [1, 500]$), measurement noise ($R \in [0.1, 20]$), delay ($\tau_{\text{delay}}$) |
| **One-Euro Filter** | Speed-adaptive low-pass smoothing | Post-processing angles without flick lag | Min cutoff ($f_{c,\min} \in [0.1, 10]$ Hz), speed coefficient ($\beta \in [0.001, 0.5]$) |

---

## Code Reference

- **Subsystem Implementation:** [`src/subsystems/vector_targeting/vector_targeting.cpp`](file:///var/home/anon/projects/Flakes/src/subsystems/vector_targeting/vector_targeting.cpp)
- **Configuration Schema:** [`src/gui/config.hpp`](file:///var/home/anon/projects/Flakes/src/gui/config.hpp)
- **Serialization & Deserialization:** [`src/gui/config_io.hpp`](file:///var/home/anon/projects/Flakes/src/gui/config_io.hpp)
- **ImGui Control Panel:** [`src/gui/menu.hpp`](file:///var/home/anon/projects/Flakes/src/gui/menu.hpp)
- **Engine View Angle Interface:** [`src/sdk/interfaces/engine.hpp`](file:///var/home/anon/projects/Flakes/src/sdk/interfaces/engine.hpp)
- **CreateMove Detour:** [`src/hooks/client_mode_create_move.cpp`](file:///var/home/anon/projects/Flakes/src/hooks/client_mode_create_move.cpp)
