# D3D12 Voxel Engine — Learning Roadmap & Progress Tracker

**Goal:** Learn D3D12 and C++ game architecture by building a cross-platform-ready engine and recreating Minecraft on it.

**How to use this:** Work through the phases in order. Each phase has a *milestone* — a concrete, visible result. Don't move on until the milestone works. Check boxes as you go. Time estimates assume part-time work (evenings/weekends); double them if you're new to C++.

---

## Architecture target (keep this picture in mind)

```
┌─────────────────────────────────────────────┐
│  GAME LAYER        voxels, world gen,       │
│                    player, blocks, mobs     │
├─────────────────────────────────────────────┤
│  ENGINE LAYER      renderer (scene-level),  │
│                    asset system, job system │
├─────────────────────────────────────────────┤
│  RHI LAYER         device, buffers, PSOs,   │
│                    command lists (abstract) │
│                    └─ backend: D3D12 (now), │
│                       Vulkan/Metal (later)  │
├─────────────────────────────────────────────┤
│  PLATFORM LAYER    window, input, file I/O, │
│                    threads, timers (SDL3)   │
├─────────────────────────────────────────────┤
│  CORE LAYER        math, logging, asserts,  │
│                    allocators, containers   │
└─────────────────────────────────────────────┘
```

**Golden rules:**
- Dependencies point downward only. Game may include Engine; Engine may never include Game.
- No D3D12 header (`d3d12.h`) is ever included above the RHI layer.
- No OS header (`windows.h`) is ever included above the Platform layer.

---

## Phase 0 — Tooling & Project Skeleton  *(~1 week)*

**Learning goals:** CMake, dependency management, repo hygiene.

- [x] Install Visual Studio 2022 (Desktop C++ + Game dev workloads)
- [x] Install CMake + Ninja, verify a hello-world builds from command line
- [x] Set up vcpkg in manifest mode (`vcpkg.json` committed to repo)
- [x] Create repo structure:
  ```
  /engine
    /core        (math, log, assert)
    /platform    (window, input)
    /rhi         (graphics abstraction)
    /renderer    (scene-level rendering)
  /game          (voxel game code)
  /shaders
  /assets
  /third_party or vcpkg.json
  /tools
  CMakeLists.txt
  ```
- [x] Each folder = one CMake static library target with explicit dependencies (enforces layering!)
- [x] Git + .gitignore + Git LFS for /assets
- [x] GitHub Actions CI: builds Debug + Release on every push
- [ ] Install PIX, RenderDoc, Tracy
- [x] Decide conventions and write them in `CONVENTIONS.md`: C++20, exceptions on/off, naming style, Y-up right/left-handed coordinates, units (1.0f = 1 meter = 1 block)

**Milestone:** `cmake --build` produces an exe from the game target that calls a function in engine/core and prints via your own `LOG_INFO` macro.

---

## Phase 1 — Platform Layer & Window  *(~1 week)*

**Learning goals:** layering discipline, main loop structure, input handling.

- [x] Add SDL3 via vcpkg (or raw Win32 if you want that learning experience)
- [x] `Platform::Window` class — create/destroy, poll events, expose native handle (HWND) *only* through an opaque getter used by RHI
- [ ] Input state: keyboard + mouse (current/previous frame state, pressed/held/released queries)
- [x] Fixed-timestep main loop skeleton: `while (running) { poll(); update(dt); render(); }`
- [x] High-resolution timer in core (QueryPerformanceCounter behind an interface)
- [ ] Assert macro that breaks in debugger; log to console + file

**Milestone:** A window opens, ESC closes it, WASD press/release events print to your log at a stable tick rate.

---

## Phase 2 — D3D12 Initialization: Clear Screen  *(~1–2 weeks)*

**Learning goals:** device/adapter/swapchain, command queue/allocator/list, fences, the debug layer. This is where D3D12 starts.

- [ ] Enable D3D12 Debug Layer (+ GPU-Based Validation in a config flag) — *before* device creation
- [ ] Enumerate adapters via DXGI, pick the discrete GPU, log its name/VRAM
- [ ] Create `ID3D12Device`, direct command queue
- [ ] Create swapchain (flip model, 2–3 buffers), handle window resize
- [ ] Create RTV descriptor heap; RTVs for each backbuffer
- [ ] Command allocator + graphics command list
- [ ] Fence-based CPU/GPU sync: understand `Signal`/`Wait`, write a `FlushGPU()` helper
- [ ] Record: barrier (PRESENT→RENDER_TARGET) → clear → barrier back → present
- [ ] Read every debug-layer message; run clean with zero warnings

**Study alongside:** the "Hello Window/Triangle" samples in Microsoft's DirectX-Graphics-Samples repo; understand every line before copying any.

**Milestone:** Window clears to a color that animates over time (proves your frame loop and sync work). Zero debug layer warnings. You can explain to a rubber duck why the fence is needed.

---

## Phase 3 — First Triangle  *(~1–2 weeks)*

**Learning goals:** the D3D12 drawing pipeline end-to-end — this is the hardest conceptual hump of the entire project.

- [ ] Write HLSL vertex + pixel shader; compile offline with DXC as a build step
- [ ] Root signature (empty at first)
- [ ] Pipeline State Object: input layout, shaders, rasterizer/blend/depth state, RT format
- [ ] Vertex buffer: create in an UPLOAD heap first (simple), draw 3 vertices
- [ ] Then do it "properly": DEFAULT heap + upload via staging buffer + `CopyBufferRegion` + barrier
- [ ] Viewport/scissor, `DrawInstanced`
- [ ] Capture the frame in PIX and in RenderDoc; walk through every event
- [ ] Shader hot-reload: press F5 → recompile with DXC API → rebuild PSO → see change live

**Milestone:** A colored triangle renders. You can hot-reload its pixel shader without restarting. You've inspected the draw in PIX.

---

## Phase 4 — Frames In Flight, Constant Buffers, 3D Camera  *(~2 weeks)*

**Learning goals:** the frame-resource ring — the architectural heart of every D3D12 engine — plus 3D math in practice.

- [ ] `FrameContext[N]` (N=2 or 3): per-frame command allocator, fence value, upload arena
- [ ] Frame loop that waits only on the frame N-behind (not full flush) — understand *why*
- [ ] Per-frame linear upload allocator (bump pointer into a persistently-mapped UPLOAD buffer, reset each frame)
- [ ] Constant buffer with view/projection matrix, bound via root CBV
- [ ] Math: `Vec3/Mat4` (your own or GLM) — perspective projection, lookAt
- [ ] Depth buffer (DSV heap, depth texture, clear + depth test in PSO)
- [ ] Fly camera: WASD + mouse look
- [ ] Draw an indexed cube; then 100 cubes with per-object transforms
- [ ] Integrate Tracy; add CPU zones to update/record/present; add GPU timestamps

**Milestone:** You fly around a field of spinning cubes with correct depth, at a Tracy-verified stable frame rate, with no full-GPU-flush per frame.

---

## Phase 5 — Textures & Descriptor Management  *(~1–2 weeks)*

**Learning goals:** descriptor heaps for real — the most confusing D3D12 topic after sync.

- [ ] Load an image with stb_image
- [ ] Create texture in DEFAULT heap; upload via staging buffer with correct row pitch alignment (256B) — this *will* bite you once
- [ ] Shader-visible SRV descriptor heap; a simple descriptor allocator (free-list or ring)
- [ ] Root signature with descriptor table; sampler (static sampler is fine)
- [ ] Textured cube; mipmaps (generate offline or compute-shader them later)
- [ ] Dear ImGui integration (it will exercise your descriptor heap properly)
- [ ] ImGui debug panel: frame time, camera position, draw count

**Milestone:** Textured, mipmapped cubes + an ImGui overlay showing live stats. You can explain descriptor heap vs. descriptor table vs. root parameter.

---

## Phase 6 — Extract the RHI Layer  *(~2 weeks)*

**Learning goals:** API abstraction design. Do this *now*, while the D3D12 code is small — this is the "learn architecture" payoff phase.

- [ ] Define RHI interfaces shaped like explicit APIs: `Device`, `CommandList`, `Buffer`, `Texture`, `Pipeline`, `DescriptorSet`-equivalent
- [ ] Handle-based resources (opaque `BufferHandle` etc.) rather than raw pointers — enables pooling, validation, hot swap
- [ ] Move all `d3d12.h` includes into `engine/rhi/d3d12/`; enforce with a CI grep check
- [ ] Renderer layer on top: `Mesh`, `Material`, `RenderPass` concepts — game code never touches RHI directly
- [ ] Everything from Phases 2–5 still works, now through the abstraction

**Milestone:** The cube scene renders identically, but `game/` and `engine/renderer/` compile without any D3D12 headers. A second backend is now *possible* even if you never write it yet.

---

## Phase 7 — Core Systems: Jobs, Memory, Assets  *(~2–3 weeks)*

**Learning goals:** the systems that make the voxel world feasible.

- [ ] Job system: thread pool, job queue, `JobHandle` with dependencies/counters (study: "Parallelizing the Naughty Dog Engine" GDC talk)
- [ ] Verify with Tracy: jobs spread across worker threads
- [ ] Allocators in core: linear/arena, pool; hook allocation stats into ImGui
- [ ] Asset system v1: hashed asset IDs, async load-from-disk on job system, refcounted handles
- [ ] Move texture + shader loading onto it
- [ ] World save groundwork: simple binary serialization utilities

**Milestone:** Textures load asynchronously via the job system (with a visible placeholder-then-swap), and Tracy shows multi-core utilization.

---

## Phase 8 — First Voxel Chunk  *(~1–2 weeks)*

**Learning goals:** voxel data layout and meshing fundamentals. The game begins!

- [ ] `Chunk`: 16×16×16 (or ×256 column) flat `std::uint16_t` array of block IDs
- [ ] Block registry: ID → properties (solid, texture indices per face)
- [ ] Culled mesher: emit a face only if the neighbor block is air; per-face texture UVs from a texture atlas
- [ ] Chunk mesh → vertex/index buffer via your RHI
- [ ] Texture atlas (or texture array — arrays avoid mip bleeding; recommended)
- [ ] Simple directional light + ambient in shader; face-based shading like Minecraft

**Milestone:** One chunk of grass/dirt/stone renders correctly from every angle with no interior faces (verify in RenderDoc: face count ≪ block count × 6).

---

## Phase 9 — Chunk Streaming World  *(~3–4 weeks, the boss fight)*

**Learning goals:** everything converges — jobs, memory pools, upload pipeline, state machines.

- [ ] Chunk manager keyed by chunk coordinates; load radius around player
- [ ] Chunk state machine: `Missing → Generating → Generated → Meshing → Ready → Uploaded` (atomic state, one-way transitions)
- [ ] World gen on job system: heightmap terrain via noise (FastNoise2 or your own Perlin)
- [ ] Meshing on job system; neighbor-aware (needs adjacent chunk data — handle borders!)
- [ ] Pool allocators for chunk voxel data and mesh scratch memory
- [ ] Ring-buffer GPU upload path for finished meshes; budget uploads per frame (no hitches)
- [ ] Unload far chunks; recycle memory to pools
- [ ] Frustum culling (AABB vs. camera frustum per chunk)
- [ ] ImGui: chunks loaded/meshing/uploaded per frame, pool usage, gen/mesh timings

**Milestone:** Fly across an infinite procedurally generated terrain with no frame hitches (Tracy-verified: no main-thread stall > 2 ms from streaming).

---

## Phase 10 — It Becomes a Game  *(~2–3 weeks)*

**Learning goals:** gameplay systems on top of a clean engine.

- [ ] AABB player physics: gravity, jumping, swept collision vs. voxel grid (no physics engine needed)
- [ ] Walk mode vs. fly mode toggle
- [ ] Raycast into voxel grid (DDA algorithm) for block targeting; highlight targeted block
- [ ] Break/place blocks → mark chunk dirty → remesh via job system (fast path: < 1 frame visible delay)
- [ ] Basic hotbar UI (ImGui first, real UI later)
- [ ] Skybox / sky gradient, fog matching view distance
- [ ] Save/load: serialize modified chunks to region files

**Milestone:** You can walk, jump, break and place blocks, quit, relaunch, and your changes persist. **This is playable Minecraft-lite on your own engine.**

---

## Phase 11 — Scale & Polish (ongoing)

Pick by interest — each is a deep-dive:

- [ ] Greedy meshing (big vertex-count win; compare stats before/after)
- [ ] Per-vertex lighting: sunlight + block light propagation (BFS flood fill), smooth lighting/AO
- [ ] Transparent blocks: water pass with sorting, leaves with alpha-test
- [ ] Palette-compressed chunk storage (memory ×4–10 savings)
- [ ] Bindless descriptors (SM 6.6 `ResourceDescriptorHeap`) — modern D3D12 idiom
- [ ] Second RHI backend (Vulkan) — the true test of Phase 6
- [ ] Entities: EnTT-based mobs/items, simple animation
- [ ] Multiplayer groundwork: separate sim tick from render, deterministic world edits

---

## Study resources (in order of usefulness for this path)

1. **Microsoft DirectX-Graphics-Samples** (GitHub) — canonical D3D12 patterns
2. **"D3D12 Do's and Don'ts"** (NVIDIA) + **AMD's D3D12 performance guides** — read after Phase 4
3. **learnopengl.com** — API is GL, but the *graphics concepts* chapters (lighting, cameras, framebuffers) transfer directly
4. **Game Engine Architecture** (Jason Gregory) — layering, core systems, memory; your Phase 0/6/7 companion
5. **GDC talk: "Parallelizing the Naughty Dog Engine Using Fibers"** — job system design
6. **0fps.net greedy meshing articles** — the classic voxel meshing reference
7. **Handmade Hero** (early episodes) — platform layer philosophy, if you want to go deeper than SDL

---

## Progress log

| Date | Phase | Notes / blockers / wins |
|------|-------|-------------------------|
| 2026-07-05 | 0 | Repo structure + layered CMake targets created (ve_core, ve_platform, ve_rhi, ve_renderer, game), linked downward-only. Global CONVENTIONS §1 flags applied (C++20, /W4 /WX /permissive- /EHs-c- /GR-, _HAS_EXCEPTIONS=0). Added fmt via vcpkg, switched triplet to x64-windows-static-md. Milestone met: game/main.cpp calls VE_LOG_INFO (engine/core/log.h/.cpp), builds and runs clean with zero warnings. Hit/fixed: missing `find_package(fmt CONFIG REQUIRED)`, typos (`fmr::string_view`, `fmStr`, `_HAS_EXEPTIONS`), and fmt v12's deprecated implicit `format_string`→`string_view` conversion (fixed via explicit `fmtStr.str`, caught by /WX). |
| 2026-07-05 | 0 | Initialized git repo, added .gitignore (build dirs, vcpkg_installed, CMakePresets.json), configured Git LFS for assets/. |
| 2026-07-05 | 0 | git init + .gitignore (build/, .vs/, hello/) + LFS tracking for asset types (.gitattributes). Pushed to GitHub (BrziGolub/voxel-world). GitHub Actions CI: Debug + Release matrix on windows-latest. Three CI bugs fixed in sequence: (1) `fail_fast` -> `fail-fast` (schema rejects unknown keys before any VM starts); (2) matrix key typo `present` vs `${{ matrix.preset }}` - GH expressions expand silently to empty string, no error; (3) vcpkg baseline commit missing from runner's preinstalled clone -> added `git fetch` step. Verified CI catches real code errors: pushed unused variable, C4101->C2220 via /WX, both jobs red, then `git revert` -> green. Lesson: read the topmost error only; the rest is cascade. |
| 2026-07-11 | 1 | Implemented platform Window class that utilizes SDL under the hood. Learned about forward declaration for including SDL_Window struct type, C++ lets me declare that a type exists without defining it. Starting convention decision forbids constructors that can fail. Window owns a resource, so copying it must be illegal, so copy constructor must be deleted, this is a first time meeting with *Rule of Five*. Implemented window.cpp with header defined functions containing constructors, destructors, Create, Destroy, PollEvents, and HWND opaque getter. Pattern worth knowing: **RAII** (Resource Acquisition Is Initialization), this means tieing a resource's lifetime to an object's lifetime (Destructor calls Destroy). Implemented poll events drain loop. Learned that Destroy functions is bad to be called twice as that means pointer to already freed window will be sent to SDL handle, so used standard idempotent-release idiom. |
| 2026-07-11 | 1 | time header that has NowSeconds() function getter that fetches high-resolution current time in seconds. Internally uses QueryPerformanceFrequency and QueryPerformanceCounter and stores in private struct ClockState initial values. clock.frequency uses inverted frequency as this is per-frame function and division is the slowest arithmetic op. Time is calculated: NowSeconds = `(now - clock.base) * clock.inverse_frequency`. Main loop now updates elapsed time (dt). Learned about *spiral-of-death* and how to break it by clamping slower frames and accumulating frame times. Rate counter that prints steady ticks/second |

*Rule of thumb: if stuck > 3 days on the same bug, capture it in PIX/RenderDoc, reduce to minimal repro, and re-read the relevant sample. Sync bugs and descriptor bugs cause 90% of early D3D12 pain.*
