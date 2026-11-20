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

### Requirement: Measured first-clear sessions
The web playtest SHALL provide a fresh timed session, durable per-sector active times, independent trial and campaign progress, and locally exportable results.

#### Scenario: A returning playtester joins a new party
- **GIVEN** one party member has already played this door
- **WHEN** the host records a repeat visit or stops before extraction
- **THEN** the log preserves the run and excludes it from first-clear medians

#### Scenario: A friend imports recorded results
- **WHEN** a tester imports the same JSON export twice
- **THEN** each party session appears once, with its original door, play mode, five sector times and comfort notes. First-clear medians include the current layout edition only

### Requirement: Headset guidance and field evidence
Immersive play SHALL render a readable objective panel in each eye, support snap turns, and allow a tester to record tracking and control observations.

#### Scenario: Seated device with interrupted tracking
- **GIVEN** a timed WebXR session uses local reference space because floor space is unavailable
- **WHEN** tracking is lost and returns
- **THEN** play pauses during loss, resumes with the stereo guide, and records the interruption and reference space

### Requirement: Low-resolution world raster
The flat-screen renderer SHALL cap the scene at 320 pixels along its long edge, preserve its aspect ratio and present with nearest-neighbour scaling. Browser controls and text SHALL use display resolution. WebXR SHALL request a 0.7 framebuffer scale with eye-specific projections.

#### Scenario: Rotate during local play
- **WHEN** the browser stage changes from landscape to portrait
- **THEN** the world canvas changes from at most 320 pixels wide to at most 320 pixels tall and the touch overlay continues to use the full stage

### Requirement: Browser mouse look
The web game SHALL request pointer lock when a player uses a mouse to start or resume play, or clicks the active game view. Mouse movement SHALL turn the active seat without a held button while captured. The game SHALL choose touch controls when the player touches the screen and support drag look if the browser declines capture.

#### Scenario: Switch input on a touchscreen laptop
- **WHEN** a player enters by touch and then clicks the touch look area with a mouse
- **THEN** the touch overlay gives way and the same click requests capture for the canvas

#### Scenario: Return to play from the map or menu
- **WHEN** the player opens the map or menu, then clicks Resume with a mouse
- **THEN** the panel releases capture while open and the resume gesture requests it again
