# Platform and production design

## What the public build contains

The same C11 simulation, geometry builder and OpenGL renderer run in native SDL3 and in the Emscripten browser build. Eligible arches lead to 24 five-sector missions, each with its own authored terrain and encounters. The short cinema tutorial remains available separately. The browser atlas samples the C collision map in its arrival and commissioned states, so reviewers can inspect all 120 sectors before entering a door. [Mission atlas](https://stbange.github.io/weihnachten2026/?atlas=1)

| Capability | Public rehearsal | Campaign release target |
| --- | --- | --- |
| Calendar and saves | Warsaw-date gates, device-local ticket, isolated rehearsal | A versioned 24-bit ledger, keepsakes, optional guestbook text and checkpoint per door |
| Local play | One or two views on desktop or web. Touch on seat one; keyboard for the second seat | Two gamepads, hot-plug, remapping, independent prompts and mixed couch/online sessions |
| Network | Two browsers, direct WebRTC or a user-hosted WebSocket relay, host-authoritative objectives and guest input | Up to four seats, hosted room discovery, TURN fallback, native cross-play and rejoin |
| Rendering | C-built triangle meshes, paper sprite actors, voxel props, low-resolution world, shader lights and projected floor shadows | Per-door palette packs, more shadow casters, content streaming and graphics presets |
| Immersive play | Experimental immersive-vr entry on supported browsers, stereo views, controller inputs and snap turning | Tested headset matrix, seated height calibration, room-scale bounds, comfort presets and complete headset menus |
| Platforms | Native SDL3 builds for Windows/Linux and a Wasm build for browsers. Verification status is in the README | Signed or checksummed native distributions, measured touch and headset frame budgets |

## The calendar contract

The shared time zone is **Europe/Warsaw**. In December 2026 each boundary is midnight CET, or 23:00 UTC on the preceding date. Before the first boundary zero doors are eligible. At boundary N, doors 1 through N are eligible, even for a new player. At and after the last boundary all 24 stay eligible. Missing a night or leaving a mission unfinished never blocks the next one.

Use integer epoch seconds, clamp before subtracting extreme values and evaluate time independently of a render frame. A solo device reads its clock. A network host supplies a single authoritative count to both seats. A sleeping browser refreshes after resuming. In this friendly calendar, changing the host clock can reveal a door early. The production save merges earned stamps monotonically and treats clock changes independently from achievements. A rehearsal can run before December and cannot write a stamp.

Persist completion at extraction, after the keepsake has been recovered. Save the campaign version, door index, checkpoint ID and content revision at benches. Migrate known revisions to the nearest safe bench and keep completed doors on unknown revisions. Browser persistence uses origin-local storage, with an export/import option planned for the campaign. Save guestbook names locally. A joiner introduces their own public display alias.

## Runtime and rendering

* **Core.** Use a fixed 60 Hz C simulation. Movement, doors, inventory, enemy state, objective interactions and calendar rules have no SDL or GPU dependency. Each input frame names a seat and a tick. The renderer can draw the same state once, twice, or once per XR eye.
* **Content.** Design each mission as a set of authored rooms and objective transitions. The release schema names spawn points, trigger volumes, encounter seeds, tool definitions, time budgets, save IDs and accessible puzzle alternatives. Use stable content IDs for saves and the network. The prototype’s web almanac is generated directly from the design Markdown.
* **Graphics.** Use SDL3 to create the GL context. The rehearsal uses desktop OpenGL 2.1 and GLES 2.0/WebGL 1 shaders. Static mesh buffers are uploaded when a room or its visible state changes. Pixel actors and remote players use a separate small dynamic buffer. Per-face normals, palette lighting, grain, materials and cheap projected shadows keep lights readable on a low-resolution target. World pixels upscale with nearest-neighbour filtering. The native HUD uses the window title in this slice. Browser controls and text remain display-resolution.
* **Budgets to measure.** Start at a 480×270 world buffer on the low preset and scale up to a long edge of 840 pixels on desktop. Target 60 fps solo desktop and 30 fps minimum on representative mobile and split-screen devices. Limit the first door’s compressed download to 3 MiB. Stream one full door pack at a time in the campaign. GPU residency and memory-growth budgets must cover one extra party member and one additional eye pass.
* **WebXR.** Request an immersive session after user input on HTTPS, then use XR frame timing, headset projection and eye transforms with the shared scene. Retain the headset’s native render target for silhouettes and text. Use pixelated materials for the retro look. A headset that exposes no immersive session continues with the normal browser view. Design terrain flight with horizon lock, snap turns and landing anchors.
* **Assets and licensing.** Code uses MIT. Original art and composed sounds use CC BY 4.0, including displayed art produced by code. Keep provenance, creator, source, license and modifications for each imported asset. Fonts and toolchain runtime notices ship alongside the game. [Art credits](../ART-CREDITS.md) · [SDL3 Emscripten notes](https://wiki.libsdl.org/SDL3/README-emscripten) · [WebXR specification](https://immersive-web.github.io/webxr/)

## How a web party works

Direct rehearsal sessions use one ordered WebRTC data channel. A host sends versioned full-state messages at approximately 20 Hz. A guest sends movement, view deltas and action buttons. The host steps both players against the same world and validates remote input. Each browser draws its own view. Reliable transport keeps the tiny prototype deterministic on a quiet connection. The two browsers exchange offer/answer codes manually, with a public STUN server to discover direct routes. If the peer link breaks, both return to the title and earned tickets survive. The website and Wasm are static files served by GitHub Pages.

**WebSocket relay for two seats.** The included relay forwards the same game messages over a web connection and uses eight-character private room codes. It holds rooms in memory and expires idle connections. Install the Python dependencies from the repository root and run the server.

```sh
python3 -m pip install -r tests/requirements.txt
python3 scripts/relay.py --bind 127.0.0.1 --port 8765
```

For two local browsers, enter `ws://127.0.0.1:8765` in **Play together → Connect browsers → Connect through a WebSocket relay**. For friends on different networks, run the service at a reachable HTTPS reverse proxy with WebSocket upgrades, or supply `--bind 0.0.0.0 --cert fullchain.pem --key privkey.pem` with a valid certificate for the relay hostname. Both friends enter the same `wss://` address. One creates a room and shares its code, and the other joins. The public Pages deployment contains the client. The relay process runs on the host chosen by the party.

For release, a small HTTPS signalling service issues private room codes and exchanges ICE candidates. Provide time-limited TURN credentials for networks that cannot connect directly. The native adapter can use a maintained WebRTC library with a C API. Carry the same versioned message schema across all platforms. Use an ordered channel for joins, date changes, objective events and saves, and an unordered channel for sequenced motion snapshots. Predict local movement and reconcile to the host. Each input names a connection and a local seat to support two friends on one browser.

Mid-mission joins receive a version check, encounter seed, baseline and last acknowledged tick. Replay reliable events after that baseline and let the guest enter at the current bench. On host loss the first release should preserve the last shared checkpoint and offer to reopen the room. A stale or malformed message cannot award a keepsake, move a portal date or create a new seat. Actual internet NAT coverage, latency up to 200 ms and mixed-device sessions require device and network testing. [Emscripten networking](https://emscripten.org/docs/porting/networking.html) · [GitHub Pages hosting model](https://docs.github.com/en/pages/getting-started-with-github-pages/what-is-github-pages)

## OpenSpec and verification

Current requirements live in `openspec/specs/`. Each uses SHALL statements and GIVEN/WHEN/THEN scenarios. Future work begins with a proposed change, a design and a task list under `openspec/changes/`. Incorporate accepted deltas into the capability after verification. The campaign expansion proposal records outstanding work independently of implemented behaviour. Validate with `openspec validate --all --strict --no-interactive`. [OpenSpec](https://github.com/Fission-AI/OpenSpec)

The unit gate counts executed lines across the project’s hand-written C files, including drawing and platform glue in a native test build. Generated SDL, Emscripten, fonts and test code are outside the denominator. Run C rule/geometry tests, platform adapter tests with a real offscreen GL context, and a scripted 60 Hz gameplay replay. Report unit coverage separately from the replay and browser tests. Cover the exact midnight boundaries, December 25 and later, a backward clock change, locked interactions, solo and split play, rehearsal isolation, packet round trips and malformed data, extraction and one-time credit.

Browser automation drives ordinary UI entry points over HTTP. Exercise all 24 lore cards, one pre-December session, December boundaries, a local split session, a mobile viewport and two browsers that clear an objective together. The optional direct-ICE test also exercises the invite/reply path. In the full campaign, give each mission a seeded solo completion script, a two-seat completion script and recovery from its riskiest failure. Add save migration, reload during an objective, disconnect/rejoin and packet loss before marking a door shippable. Device acceptance also measures frame time, text readability and VR comfort. A software-rendered browser test cannot establish headset comfort.

## Production sequence

| Gate | Evidence required before moving on |
| --- | --- |
| Hall and first playable room | Walk to a numbered arch, complete the rehearsal, and exchange a live two-browser invitation. Measure the time to understand the lift and projector. |
| Two contrasting vertical slices | Deliver the full 20-minute cinema and the harvester convoy. Verify solo, split and web co-op using the same geometry and objective rules. Estimate the remaining doors from actual authoring and testing time. |
| Four batches of six | Lock each chapter’s mechanics, multisensory cues, keepsakes, lighting card, timed mission and co-op roles. Playtest with someone unfamiliar with the original. |
| Release candidate | Exercise all dates with injected clocks, all 24 return paths, four seats, save versions and a real headset. Publish tagged content manifests through the tested main-to-pages build. |

All development work goes to `main`. The `gh-pages` branch holds built static files. See the repository README for reproducible commands and the exact boundary of this playtest.


## Campaign playtest implementation

The current numeric protocol is version 4. Each host snapshot contains 32 numeric words covering both poses, the 24-bit completion ledger, sector, task, phase, dial, escort position, local encounter clock, stun and two charge values. The receiver validates the full frame before applying it. Guest input uses seven numbers. Update both clients and the relay together when moving from an earlier playtest. The optional native Python transport drives the same compiled SDL3/OpenGL runtime used by the executable.

The five-sector routes use deterministic layouts with connected passages, mapped landmarks and shared tool state. Native preferences and browser local storage preserve sector entrances independently by day and mode. A completed calendar mission adds its bit to the keepsake ledger. The old single-ticket save maps to Door 01. The playtest does not yet provide join-in-progress recovery or mixed local and online seats.
