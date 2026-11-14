# Flakes — Source Engine Dynamic Binary Instrumentation & Autonomous Agent Research Framework

**Flakes** (`tf2.so`) is an experimental C++23 shared-object (`ELF64` dynamic library) research framework designed for **in-process runtime introspection, real-time telemetry visualization, deterministic physics simulation, and autonomous navigation research** within the Linux 64-bit build of the [Valve Source Engine (SDK 2013 Multiplayer)](https://github.com/ValveSoftware/source-sdk-2013) (`tf_linux64`).

This project explores low-level systems programming concepts on Linux, including dynamic symbol resolution, virtual method table (VMT) interposition, inline function detouring, network variable (`RecvTable` / `RecvProp`) schema reflection, Vulkan/SDL2 render-pipeline overlay composition, and A* navmesh pathfinding.

> [!NOTE]
> **Academic & Educational Scope**  
> This repository serves as a technical case study in reverse engineering, game engine architecture, runtime instrumentation, and autonomous behavioral modeling in complex 3D environments.

---

## System Architecture

Flakes operates as an internal instrumentation module loaded into the target process address space via POSIX dynamic linking (`dlopen` / `dlclose`) orchestrated through `gdb` ptrace attachment.

```
┌─────────────────────────────────────────────────────────────────────────┐
│                     Target Process (tf_linux64)                         │
│                                                                         │
│  ┌──────────────────────┐    CreateInterface    ┌────────────────────┐  │
│  │ Engine Shared Libs   │◄─────────────────────►│ Interface Bindings │  │
│  │ client.so / engine.so│    & AOB Sigscans     │ (src/sdk/interfaces│  │
│  └──────────┬───────────┘                       └─────────┬──────────┘  │
│             │                                             │             │
│             ▼                                             ▼             │
│  ┌──────────────────────┐                       ┌────────────────────┐  │
│  │ VMT & Inline Hooks   │────── Detours ───────►│ Research Modules   │  │
│  │ (funchook + VTables) │                       │  (src/subsystems/) │  │
│  └──────────┬───────────┘                       └─────────┬──────────┘  │
│             │                                             │             │
│             ▼                                             ▼             │
│  ┌──────────────────────┐                       ┌────────────────────┐  │
│  │ Vulkan / SDL2 / VGUI │◄───── Telemetry ──────│ Entity Cache &     │  │
│  │ ImGui Render Layer   │       Overlays        │ Netvar Reflection  │  │
│  └──────────────────────┘                       └────────────────────┘  │
└─────────────────────────────────────────────────────────────────────────┘
```

### Core Mechanisms

1. **Factory Interface Discovery & Reflection (`src/sdk/interfaces/`, `src/sdk/classes/`)**
   - Queries exported `CreateInterface` symbol factories across loaded engine modules (`client.so`, `engine.so`, `vgui2.so`, `vguimatsurface.so`, `materialsystem.so`, `steamclient.so`).
   - Traverses the engine's `ClientClass` linked list and `RecvTable` trees at initialization to dynamically resolve network property offsets (`netvars.hpp`) without brittle hardcoded struct padding.
2. **Byte-Pattern Signature Scanning (`vendor/libsigscan/`)**
   - Parses ELF program headers in memory to locate non-exported internal engine routines and RIP-relative global singletons (`CInput`, `CMoveHelper`, `CClientState`) via wildcard pattern matching.
3. **Hybrid Execution Interposition (`src/hooks/`, `vendor/funchook/`)**
   - **Virtual Method Table (VMT) Swapping:** Non-destructively replaces pointers in C++ virtual tables (`IBaseClientDLL`, `IClientMode`, `IPanel`, `IVModelRender`, `IVRenderView`, `IGameEventManager2`) to observe and augment per-frame and per-tick callbacks.
   - **Inline Prologue Detouring:** Uses `funchook` and `diStorm3` to place trampolines on non-virtual engine routines (`CL_Move`, Vulkan queue presentation, and SDL2 event polling).
4. **Unity Build Compilation Model (`src/main.cpp`)**
   - Employs a single translation unit ([Unity Build](https://en.wikipedia.org/wiki/Unity_build)) for the core instrumentation logic, maximizing link-time optimization opportunities, simplifying header dependency chains, and accelerating incremental rebuilds.

---

## Research Subsystems

| Subsystem | Directory | Technical Description |
| :--- | :--- | :--- |
| **Spatial Telemetry & HUD Diagnostics** | `src/subsystems/spatial_telemetry/`, `src/gui/` | Projects 3D world-space entity bounding volumes (`IVDebugOverlay`), skeletal kinematics, status flags, and resource states onto a 2D viewport via `VGUI_Surface030` and an interactive Dear ImGui control panel rendered over Vulkan/SDL2. |
| **Autonomous Kinematics & Vector Targeting** | `src/subsystems/vector_targeting/` | Computes ballistic trajectories, hitscan ray-intersection checks (`IEngineTrace`), and angular orientation vectors within `IClientMode::CreateMove` user-command ticks. |
| **Active Deflection Routine** | `src/subsystems/active_deflection/` | Evaluates incoming projectile trajectories and triggers compressed-air deflection commands within bounding-sphere thresholds. |
| **Autonomous Graph Navigation (`Navmesh`)** | `src/subsystems/graph_navigation/` | Parses Source Engine `.nav` navigation meshes and integrates the `MicroPather` A* solver to autonomously compute and traverse waypoint graphs across map objectives. |
| **Deterministic Client-Side Prediction** | `src/subsystems/state_prediction/` | Reconstructs client-side movement state (`IPrediction`, `IGameMovement`, `IMoveHelper`) prior to server command dispatch to compensate for network latency and tick quantization. |
| **Tick-Rate & Command Queue Analysis** | `src/subsystems/tick_scheduler/`, `src/hooks/cl_move.cpp` | Evaluates command batching, packet choking, and `CL_Move` simulation step scheduling within the Source networking stack. |
| **Material Pipeline & Shader Override** | `src/subsystems/material_override/`, `src/hooks/draw_model_execute.cpp` | Interfaces with `IMaterialSystem` and `IVModelRender` (`KeyValues`-backed custom materials) to inspect depth-buffer occlusion and entity rendering states. |
| **Locomotion, Camera Frustum & Observer Telemetry** | `src/subsystems/locomotion/`, `src/subsystems/camera_frustum/`, `src/subsystems/observer_telemetry/` | Studies ground-contact state transitions in `CUserCmd` movement flags, camera frustum/FOV overrides (`OverrideView`), and observer target tracking. |
| **External Hardware Telemetry Bridge** | `src/subsystems/hardware_bridge/` | Asynchronous `libcurl` HTTP client integration demonstrating event-driven webhook dispatch triggered by in-game `IGameEventManager2` callbacks. |

---

## Repository Layout

```text
.
├── src/
│   ├── main.cpp             # Unity build entry point (__attribute__((constructor)) / destructor)
│   ├── core/                # Foundational primitives (memory, math, vec, entity_cache, lookup, offsets, print)
│   ├── sdk/
│   │   ├── classes/         # C++ wrapper abstractions for Source Engine entities & materials
│   │   └── interfaces/      # Pure virtual interface definitions matching Source SDK vtables
│   ├── subsystems/          # Modular R&D subsystems (vector_targeting, spatial_telemetry, graph_navigation, etc.)
│   ├── hooks/               # VMT and inline detour implementations (Vulkan, SDL2, ClientMode, etc.)
│   └── gui/                 # Dear ImGui configuration menu, state serialization, and HUD indicators
├── vendor/
│   ├── funchook/            # Inline prologue detouring library (funchook + diStorm3)
│   ├── imgui/               # Bundled Dear ImGui library with SDL2, OpenGL3, and Vulkan backends
│   ├── libsigscan/          # Pattern scanning library for ELF memory segments
│   ├── md5/                 # Source Engine MD5 pseudo-random number generator
│   └── micropather/         # A* graph solver used by graph_navigation
├── scripts/
│   ├── build.sh             # Standalone bootstrap script (builds funchook + tf2.so)
│   ├── inject.sh            # GDB-based dlopen/dlclose runtime loader and log follower
│   ├── debug.sh             # Interactive GDB attachment helper for crash diagnostics
│   └── packages/            # Distribution-aware dependency installation scripts
├── Makefile                 # Parallel GNU Make build configuration
└── Taskfile.yml             # Host/Container workflow automation (Task)
```

---

## Prerequisites & Dependencies

Building and loading `tf2.so` requires a 64-bit Linux environment with a C++23-capable compiler (`g++`), `cmake`, `gdb`, `execstack`, and development headers for `SDL2`, `GLEW`, `Vulkan`, and `libcurl`.

### Automated Dependency Installation
Run the distribution detection script (supports Arch, Debian/Ubuntu, Fedora, Gentoo, and Void Linux):

```bash
./scripts/packages/packages.sh
```

---

## Building & Runtime Deployment

### Option 1: Standard Build & Load Scripts

1. **Bootstrap & Compile** (clones/builds `funchook` into `vendor/funchook` on first run, compiles `tf2.so`, and clears the ELF executable stack flag):
   ```bash
   ./scripts/build.sh
   ```
   For subsequent incremental builds, you can invoke GNU Make directly:
   ```bash
   make -j$(nproc)
   ```

2. **Load into Target Process** (requires `tf_linux64` running):
   ```bash
   sudo ./scripts/inject.sh
   ```
   `scripts/inject.sh` attaches `gdb` in batch mode to invoke `dlopen` inside `tf_linux64`, prints the allocated base address handle, and streams `/tmp/tf2.log`. Press `Ctrl+C` to cleanly invoke `dlclose` and detach the module from memory.

### Option 2: Container / Flatpak Workflow (`Taskfile.yml` & `Makefile`)

When developing inside a rootless container (e.g., Fedora `toolbox`) while running Steam inside a Flatpak sandbox, use the provided `Makefile` or [Task](https://taskfile.dev/) targets from the host/container environment:

| Command | Action |
| :--- | :--- |
| `task build` / `make` | Compiles `tf2.so`, clears the `execstack` flag, and synchronizes artifacts to `STEAM_DIR`. |
| `task ptrace` | Sets `kernel.yama.ptrace_scope=0` via `sysctl` to permit cross-process debugging. |
| `task sync` / `make sync` | Copies `tf2.so` and `scripts/inject.sh` into the Steam Flatpak accessible directory. |
| `task inject` / `make inject` | Enters the container mount namespace via `nsenter` and executes `inject.sh`. |
| `task logs` / `make logs` | Streams live diagnostic output from `/proc/<pid>/root/tmp/tf2.log`. |

### Debugging
To attach an interactive `gdb` session directly to `tf_linux64` for symbol inspection or backtrace analysis:
```bash
sudo ./scripts/debug.sh
```

---

## Engineering & Contribution Guidelines

Contributions that improve stability, expand engine interface coverage, or refine pathfinding and simulation accuracy are welcome. Please adhere to the following architectural conventions:

- **Naming Convention:** Strictly `snake_case` for variables, functions, and filenames.
- **Compilation Model:** Maintain the [Unity Build](https://en.wikipedia.org/wiki/Unity_build) architecture—new hooks and subsystems are included directly into the primary translation unit (`src/main.cpp`), while standalone third-party libraries (`imgui`, `libsigscan`, `micropather`, `md5`) compile to separate object files in `obj/`.
- **Code Style:** Favor straightforward, deterministic, C-style C++23 with minimal abstraction overhead and explicit memory layout control.

---

## License

This project is distributed under the terms of the [GNU General Public License v3.0](LICENSE).
