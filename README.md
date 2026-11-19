# Lantern Hall

Twenty-five years after a snowbound LAN party, the lights come on in the old municipal hall. A printer in the cloakroom spits out a ticket. On its back someone has written, **please leave one place free**.

**[Play in your browser](https://stbange.github.io/weihnachten2026/)** · **[Read the campaign concept](design/lantern-hall.md)** · **[Explore all 24 missions and their sources](design/portals.md)**

![The playable great hall, with its tree, rose window, long table and numbered arches](design/hall.jpg)

## The calendar

One new arch opens at midnight **Europe/Warsaw**, December 1–24, 2026. Earlier dates stay available, including after Christmas. A new player can join on any evening. The full campaign is designed for about twenty minutes per door, or eight hours across December.

The concept preserves the supplied order, from Duke Nukem 3D to the first Red Alert. Each treatment identifies its reference edition, explains its gameplay and graphics, translates three signature mechanics into first person, and gives the mission a timed route, enemies, a solo/co-op solution and a keepsake for the hall. For the ambiguous titles, day 6 uses the original 1989 PC MechWarrior, day 9 uses Starsiege: Tribes, day 15 uses UFO: Enemy Unknown and day 17 uses Jedi Knight: Dark Forces II.

## What you can play today

Choose **Playtest Door 01** on the title screen for *House Lights*. Start at the tram terminus, collect and punch tickets, fold paper ushers, and lift reels to the raised booths. The **Cinema tutorial** button opens the original short auditorium exercise.

**Portal almanac → a date → Playtest mission** opens any of the 24 expeditions before December. Each currently has five sectors, shared objectives, a route guide on the floor and a star gate. Field stations ask the party to recover and answer a transmission using two of the door’s tools. Their recordings explain why the hall’s clock stopped. The browser's **Map** shows the destination and both players. Hold Pulse and release for charged tools. For escorts, stay within twelve metres and bring the follower to the dock. Returning to the same door resumes at the start of the last sector reached.

Walk through eligible arches in the hall to earn the 24 individual keepsakes. Practice and calendar checkpoints are separate. Read a returned keepsake's story in the almanac. Completing the final date opens the front doors even for someone playing their first evening.

All 24 missions have five individually drawn floor plans. Open **View floor plans** in any mission briefing to inspect the playable terrain, entrances, exits, encounter sites and commissioned routes. The levels use three signature encounters per door in different combinations across the sectors. Sector checkpoints from earlier builds still resume at the corresponding sector entrance. [Explore the mission atlas](https://stbange.github.io/weihnachten2026/?atlas=1)

Normal-input solo replays measured 19.3–20.6 simulated minutes across the 24 doors. The replay follows the walking guide and takes one active seat at a time. Players can use ferries, slipgates and other shortcuts, split up or spend time exploring. First-time human pacing remains to be measured. The playtest log keeps earlier layout editions and calculates its first-clear medians from the current edition.

| Mode | Controls |
| --- | --- |
| Desktop, seat one | WASD moves, mouse or arrows look, F or left click pulses, E uses, Space boosts |
| Second local seat | IJKL moves, U/O turns, Y pulses, H uses, N boosts |
| Mobile browser | Left stick moves, drag on the right to look, tap Pulse or Use, hold Lift |
| WebXR preview | Left stick moves, right stick snap-turns, gaze aims, trigger pulses, grip uses, primary face button lifts |

Choose **Play together** for split screen or a two-browser session. For a direct WebRTC game, the host sends an invitation, the guest sends a reply and the host connects. Both browsers share one authoritative mission. A WebSocket relay option supports networks that block direct connections. Friends can supply their own relay endpoint and use an eight-character room code. The static Pages site serves the game files. [Relay instructions and platform design](design/engineering.md)

## Human playtests

Open **Portal almanac → Playtest log**, choose a door and party, and mark a first visit only when everyone is new to it. The timed trial starts in sector one and preserves the device’s regular progress. For an online party, the browser host can select a door and **Record a network playtest** under **Play together**. [Time House Lights](https://stbange.github.io/weihnachten2026/?playtest=1) · [Time The Glass Convoy](https://stbange.github.io/weihnachten2026/?playtest=3)

For the first round, have a fresh solo player and a fresh two-person party try each of these two doors. Let them discover the shutter and weighbridge on their own. After extraction, note the device, which interaction took longest to understand, and where anyone needed help. The log records active time in each sector. Solo menus, hidden tabs and lost XR tracking pause the timer. A network host’s menu leaves the party simulation running. Early exits remain visible as stopped sessions. Export finished sessions as JSON and import friends’ files in the same panel to see observed first-clear medians by door and mode.

On a physical headset, open a trial link in its browser and choose **WebXR**. Use the right stick to snap-turn and the secondary face button to toggle the stereo guide. Check that the lettering aligns in both eyes, the trigger pulses, grip uses, lift works, and the floor height feels right while stationary. Record the headset, comfort rating and any alignment or locomotion issues in the result after exiting VR. The export includes reference space, observed eye count, controller actions, tracking interruptions and mean frame-callback rate. Simulated headset runs exercise the software path. Physical comfort and first-time human results require participant sessions.

## Build and test

Native builds require CMake 3.20+, a C11 compiler, and an installed SDL3 development package. On Windows, make the SDL3 CMake package discoverable through `CMAKE_PREFIX_PATH`. The build copies SDL3.dll beside the executable.

```sh
cmake -S . -B build/native -DCMAKE_BUILD_TYPE=Release
cmake --build build/native --config Release
ctest --test-dir build/native -C Release --output-on-failure
```

Run `lantern-hall` for the calendar, `--door 1` through `--door 24` for a campaign playtest, or `--rehearsal` for the cinema tutorial. Add `--split` for a second local player.

For network play on Windows or Linux, install Python 3.10+ and `websockets>=14,<17`. The native build places `native_peer.py` beside the game library. Point it at the same relay used by the web game. The host gets an eight-character room code. The browser can join from **Play together → Connect browsers → Connect through a WebSocket relay**. A native player can also join a browser host.

```sh
python native_peer.py wss://your-relay.example --door 3
python native_peer.py wss://your-relay.example --join ROOMCODE
```

To run a local relay, use `python scripts/relay.py` and enter `ws://127.0.0.1:8765` on the same machine. Public browser sessions use a TLS relay. [Relay setup](design/engineering.md)

With Emscripten **4.0.20** active, build and serve the web version. It uses the SDK’s SDL3 port.

```sh
bash scripts/build-web.sh
python3 -m http.server 8571 --directory dist
```

Verification uses OpenSpec 2.1.0, GCC/gcov, SDL3, an offscreen Mesa OpenGL driver, Python and Playwright. The coverage gate counts **all five hand-written production C translation units** and fails below 95% executed lines. The separate input replays and browser tests do not inflate this number.

```sh
npx --yes @fission-ai/openspec@2.1.0 validate --all --strict --no-interactive
bash scripts/test-c.sh
python3 -m pip install -r tests/requirements.txt
python3 -m playwright install chromium
python3 tests/web_game.py http://127.0.0.1:8571/
```

Set `CHROMIUM` to use an existing Chromium executable. Add `--direct` to the browser test on a network that permits WebRTC ICE candidates. The ordinary suite starts a loopback WebSocket relay, connects two browser contexts through the visible room UI, folds an usher from the guest and checks the shared result. It also checks all 24 briefings, calendar rollover, split views, mobile controls and a simulated stereo XR session. The browser replays cover the short tutorial extraction and entry through the live December arch.

GCC/gcov measured **98.03% native C unit line coverage (2,240 of 2,285 lines)**. Separate normal-input replays cleared all 24 expeditions in solo calendar and split-screen practice. A native relay integration test exercises both host and guest. The browser suite also covers an actual two-peer relay session and a simulated stereo XR session. Physical phone and headset testing and first-time human pacing sessions remain release criteria. [Continuous verification](https://github.com/stbange/weihnachten2026/actions/workflows/verify.yml) · [OpenSpec capabilities](openspec/specs) · [Remaining campaign work](openspec/changes/complete-campaign/tasks.md)

## Publishing and credits

Develop on `main`. Publish the generated `dist/` tree to `gh-pages`, the repository’s Pages source. From a clean `main` checkout with the tested SDK active, `bash scripts/publish-pages.sh` rebuilds and publishes a fast-forward commit. It requires a GitHub account with push access to this repository.

Code uses the repository’s **MIT License**. Original art and generated sound use **CC BY 4.0**. The classic games are reference works, and the web fonts and runtime dependencies carry their own notices. [Code license](LICENSE) · [Art attribution](ART-CREDITS.md)
