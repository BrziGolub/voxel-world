# CONVENTIONS.md

Project-wide rules. If code and this document disagree, one of them is wrong — fix whichever it is.
These are starting decisions; change them consciously (edit this file in the same commit), never silently.

---

## 1. Language & Toolchain

- **C++20.** No compiler extensions (`/permissive-` on MSVC).
- Primary compiler: **MSVC**, CI also builds with **Clang** to keep us honest and portable.
- **Exceptions: OFF** (`/EHs-c-`, `-fno-exceptions`). Errors are returned, not thrown (see §6).
- **RTTI: OFF** (`/GR-`, `-fno-rtti`). No `dynamic_cast`, no `typeid`.
- Warnings: `/W4` (MSVC), `-Wall -Wextra` (Clang). **Warnings are errors** in CI.
- Standard library is allowed, but be deliberate in hot paths: prefer our allocators/containers where they exist. No `std::shared_ptr` in engine code without a written justification in a comment.

## 2. Layering (the most important rule in this file)

```
game  →  engine/renderer  →  engine/rhi  →  engine/platform  →  engine/core
```

- **Includes/dependencies point downward only.** A layer never includes headers from a layer above it.
- `d3d12.h`, `dxgi*.h` may only be included inside `engine/rhi/d3d12/`.
- `windows.h`, SDL headers may only be included inside `engine/platform/` (and `rhi/d3d12` for HWND plumbing).
- `game/` never includes anything from `engine/rhi/` directly — it talks to `engine/renderer`.
- Each layer is its own CMake static library; CMake `target_link_libraries` visibility enforces this. Don't cheat with include paths.

## 3. Project & File Layout

```
/engine/core        math, log, assert, allocators, containers, time
/engine/platform    window, input, file I/O, threads
/engine/rhi         graphics abstraction (API-agnostic headers)
/engine/rhi/d3d12   D3D12 backend (the only place D3D12 exists)
/engine/renderer    meshes, materials, passes, camera
/game               all voxel/gameplay code
/shaders            HLSL source (compiled by build, never committed compiled)
/assets             textures etc. (Git LFS)
/tools              offline tools, scripts
```

- One class/system per header where sensible. Header `foo.h`, source `foo.cpp`, same folder.
- File names: `snake_case.h` / `snake_case.cpp`.
- Every header starts with `#pragma once`.
- Include order in a .cpp: own header first, then same-layer, then lower layers, then third-party, then std. Alphabetical within groups. (clang-format enforces this.)

## 4. Naming

| Thing | Style | Example |
|---|---|---|
| Types (class/struct/enum) | `PascalCase` | `ChunkMesher`, `BufferHandle` |
| Functions & methods | `PascalCase` | `CreateBuffer()`, `GetBlock()` |
| Local variables & parameters | `camelCase` | `chunkCoord`, `vertexCount` |
| Member variables | `m_camelCase` | `m_device`, `m_fenceValue` |
| Static/global variables | `s_` / `g_` prefix | `s_instance`, `g_log` |
| Constants & enum values | `PascalCase` | `MaxFramesInFlight`, `BlockType::Stone` |
| Macros (rare!) | `SCREAMING_SNAKE` | `VE_ASSERT`, `VE_LOG_INFO` |
| Namespaces | `snake_case`, shallow | `ve::rhi`, `ve::core` |

- Engine namespace prefix: `ve::` (rename when the project gets its name — one find/replace).
- No abbreviations that save under 3 characters. `index` not `idx` is fine either way, but never `mgr`, `hndl`, `tex2dDescHpAllocr`.
- Enums: always `enum class`.

## 5. Types & Ownership

- Fixed-width integers from `<cstdint>`: `u8/u16/u32/u64`, `i8/...`, `f32/f64` (aliased in `core/types.h`). No bare `int` in serialized or GPU-facing data.
- **Handles, not pointers, across layer boundaries.** RHI resources are opaque 32/64-bit handles (`BufferHandle`, `TextureHandle`) with generation counters. Raw pointers never cross the RHI boundary.
- Ownership is explicit and documented per system:
  - `std::unique_ptr` for clear single ownership.
  - Raw pointer / reference = non-owning, never null unless documented.
  - Pools own bulk objects (chunks, meshes); systems hand out handles.
- No `new`/`delete` in game or renderer code — use allocators or containers.

## 6. Error Handling

- Recoverable errors: return `Result<T>` / `std::expected<T, Error>` (core provides the alias). Check it. `[[nodiscard]]` everywhere it matters.
- Programmer errors: `VE_ASSERT(cond, "msg")` — active in Debug and internal Release ("Dev") builds, compiled out in Ship builds.
- Unrecoverable runtime errors (device lost, out of memory at boot): `VE_FATAL("msg")` → log, message box, clean exit.
- Every D3D12 `HRESULT` is checked via `VE_CHECK_HR(expr)`. No naked calls.
- Debug layer warnings are treated as bugs. The project runs clean or it isn't done.

## 7. Math, Coordinates & Units

- **Right-handed, Y-up.** +X right, +Y up, **−Z forward** (camera looks down −Z).
- **1.0f = 1 meter = 1 block.**
- Row-vector vs column-vector: **column vectors, column-major storage** (GLM default). Transform composition reads right-to-left: `proj * view * model`.
- Depth range 0..1 (D3D convention). Reverse-Z later; note it here when adopted.
- Angles in **radians** everywhere in code. Degrees only at UI/tool boundaries.
- World voxel coordinates: `i32` per axis. Chunk coordinate = `worldCoord >> 4` (chunk size 16). Never use floats to identify a block.
- Rendering is **camera-relative** (translate world by −cameraPos before building matrices) to survive far-from-origin precision loss. Adopt no later than Phase 9.

## 8. Threading

- Main thread owns: game tick, render command submission, present.
- All other work (chunk gen, meshing, asset I/O) goes through the **job system**. No ad-hoc `std::thread` in feature code.
- Shared data rules: prefer message passing / double buffering over locks. Any mutex must have a one-line comment stating what it protects.
- Chunk data handed to mesher jobs is **immutable snapshot** for the duration of the job.

## 9. Shaders

- HLSL 2021, compiled with DXC to SM 6.6, as a build step. Compiled shaders are build artifacts, never committed.
- One `.hlsl` file may contain VS+PS pairs; entry points named `VSMain` / `PSMain` / `CSMain`.
- Shared CPU/GPU struct layouts live in `shaders/shared/*.h`, included from both sides; static-assert sizes on the C++ side.
- All shaders must survive hot-reload (F5) without restarting.

## 10. Formatting & Commits

- `clang-format` file is law; format-on-save. CI rejects unformatted code. (Base: LLVM style, 4-space indent, 120 columns, braces on same line — adjust once, then freeze.)
- `clang-tidy` runs in CI with a small curated check set; grow it slowly.
- Commits: imperative mood, scoped prefix — `rhi: add fence wrapper`, `core: fix arena alignment`. Small commits; the repo must build at every commit.
- Branch `main` is always green. Feature branches merge via PR-to-self if solo (forces you to re-read your diff).

## 11. Third-Party Policy

- Dependencies via vcpkg manifest (`vcpkg.json`) only. No copy-pasted source dumps, except single-header libs which live in `/third_party` with version noted at top.
- Every new dependency gets one sentence of justification in this file:
  - **SDL3** — platform layer (window/input) so we focus on rendering & engine.
  - **GLM** — math, until/unless we replace it for learning.
  - **D3D12MA** — GPU memory sub-allocation; industry standard.
  - **Dear ImGui** — debug UI.
  - **stb_image** — image loading.
  - **Tracy** — profiling.
  - **EnTT** — (Phase 11) entities.
  - **fmt** — formatting for `VE_LOG_INFO` and friends; used until/unless `std::format` fully replaces it.
- vcpkg triplet: **x64-windows-static-md** (static libs, dynamic CRT) — set via `VCPKG_TARGET_TRIPLET` in `CMakePresets.json`.

## 12. Definition of Done (per phase)

A phase milestone counts only when:
1. It runs with the D3D12 debug layer **and** GPU-based validation enabled with zero warnings.
2. Tracy shows no unexplained main-thread stalls.
3. Code is formatted, warning-free on both compilers, committed, CI green.
4. Any convention change made along the way is reflected in this file.

---

*Changelog*
- 2026-07-02 — initial version.
- 2026-07-05 — added fmt as a dependency (§11); recorded vcpkg triplet decision (x64-windows-static-md, §11).
