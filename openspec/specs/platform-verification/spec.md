# Platform Verification Specification

## Purpose

Keep the native and web builds traceable to one shared C simulation and exercise the supported playtest inputs.

## Requirements

### Requirement: Shared portable world
The public rehearsal SHALL use the C/SDL3/OpenGL world for native and Emscripten builds and render stereo eye views when an immersive session is available.

#### Scenario: Immersive session
- **WHEN** an XR frame supplies two eye views
- **THEN** both views render from the same simulation tick with their supplied eye transforms

### Requirement: Touch input
The web build SHALL expose movement, look, pulse, lift and use on touch devices.

#### Scenario: A player enters on a phone
- **WHEN** the player starts the rehearsal on a touch-capable phone
- **THEN** the touch controls and a touch-specific objective prompt become visible

### Requirement: Unit coverage
Unit verification SHALL cover at least 95 percent of the instrumented production C lines across all hand-written C translation units in the native build.

#### Scenario: Coverage gate
- **WHEN** the measured percentage is below 95 or a production C translation unit is missing
- **THEN** the verification command exits unsuccessfully before separate game replays
