# voxel-world

[![CI](https://github.com/BrziGolub/voxel-world/actions/workflows/ci.yml/badge.svg)](https://github.com/BrziGolub/voxel-world/actions/workflows/ci.yml)

A voxel game engine and game, built from scratch in **C++20** and **Direct3D 12** — as a learning project. Every line is typed and understood before it's committed; nothing is copied that can't be explained to a rubber duck.

The project follows a phased roadmap ([`d3d12-engine-roadmap.md`](d3d12-engine-roadmap.md)) from empty repo to a playable voxel world. The progress log at the bottom of that file doubles as an engineering notebook — bugs, root causes, and war stories included.

## Architecture

Strictly layered; dependencies point downward only.

```
game        voxel game code
renderer    scene-level rendering: meshes, materials, passes
rhi         graphics abstraction — D3D12 backend now, shaped so Vulkan/Metal fit later
platform    window, input, timers (SDL3)
core        logging, asserts, time, math
```

**Golden rules:** no `d3d12.h` above the RHI layer, no `windows.h` above the platform layer, engine never includes game. Full conventions in [`CONVENTIONS.md`](CONVENTIONS.md).

## Status

**Phase 1 complete (v0.1.0)** — platform layer:

- Window + event pump (SDL3 under an opaque handle)
- Keyboard input with per-tick edge detection (pressed/held/released)
- Fixed-timestep main loop (60 Hz, spiral-of-death clamp)
- High-resolution timer (QPC behind an interface)
- `VE_ASSERT` — breaks in the debugger at the call site, compiles to a no-op in Release
- Logger with console + file sinks, crash-safe flushing

**Next: Phase 2** — D3D12 device, swapchain, fences, and a window that clears to an animating color.

## Building

Requirements: Visual Studio 2022 (MSVC), CMake 3.28+, Ninja, [vcpkg](https://github.com/microsoft/vcpkg) with `VCPKG_ROOT` set.

```
cmake --preset debug
cmake --build --preset debug
```

Presets: `debug` / `release`. Builds with `/W4 /WX`, no exceptions, no RTTI. Dependencies (fmt, SDL3) come from the vcpkg manifest automatically. CI builds both configurations on every push.
