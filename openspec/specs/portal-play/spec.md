# Portal Play Specification

## Purpose

Define the observable behaviour of the hall and cinema vertical slice.

## Requirements

### Requirement: Physical hall and almanac
The game SHALL provide 24 numbered arches in a first-person hall and a readable research-backed briefing for each date.

#### Scenario: A guest visits a sealed door
- **WHEN** a player uses a future arch
- **THEN** its date is visible and the player remains in the hall

### Requirement: Cinema objectives
The cinema SHALL require folding both paper ushers, reaching the raised projector and returning the ticket to the entrance.

#### Scenario: Early projector use
- **WHEN** a player uses the projector with an upright usher still present
- **THEN** the film remains stopped

#### Scenario: Shared extraction
- **GIVEN** two players in the cinema whose team has started the projector
- **WHEN** either player uses the entrance punch
- **THEN** both return to the hall with one completion event

### Requirement: Playable expedition routes
Each eligible arch SHALL admit the party to its dated expedition. Each expedition SHALL provide five reachable sectors, two or three scheduled tool objectives and a staged encounter in each sector, a navigable exit, and a distinct keepsake. An explicitly selected playtest SHALL allow the same route on any date.

#### Scenario: Recover a keepsake without earlier dates
- **GIVEN** door 24 is eligible and the hall ledger is empty
- **WHEN** the party completes the fifth sector of door 24
- **THEN** the hall receives the blank place card, shows the open front door, and leaves every other keepsake available to earn

#### Scenario: Resume a sector
- **GIVEN** the party reached the second sector of a door in playtest mode
- **WHEN** the same device starts that playtest again
- **THEN** it resumes at the sector entrance with its tools reset and its calendar checkpoint independent

#### Scenario: Shared navigation
- **WHEN** either seat opens a sector map in the browser or follows the in-world floor guide
- **THEN** the guidance identifies the active sector objective and a traversable next passage


### Requirement: Field transmissions
Selected sectors of each expedition SHALL include a two-part encounter tied to the source game's tools and the hall's story. The party SHALL share progress between its two stations.

#### Scenario: Complete an interrupted dispatch
- **GIVEN** the party has uncovered the service timetable in a night-shift transmission
- **WHEN** either player sets the points cabinet to the indicated setting and confirms the signal
- **THEN** the chapter recording plays as a caption and the sector exit becomes available

### Requirement: Authored mission chambers
Each dated mission SHALL have a navigable authored chamber and a shared shortcut that opens after the first objective in each sector.

#### Scenario: Cinema shutter and harvester weighbridge
- **WHEN** one player pulses the booth shutter or the weighbridge relay
- **THEN** the lit apparatus allows either player to load the reel at booth height or unload the harvester when it reaches the scale


### Requirement: Individual mission floor plans
Each of the 120 sectors SHALL specify its own entrance, passages, courts, objective sites, encounter sequence and exit. The map, collision and path guidance SHALL use that sector's plan. Terrain elevation SHALL apply to the players, objectives and stereo views.

#### Scenario: Review a sector before playing
- **WHEN** a player selects a door and sector in the atlas and switches between arrival and commissioned routes
- **THEN** the atlas displays the corresponding production floor plan, source-game reference and encounter locations without altering campaign progress

#### Scenario: Use a location-specific mechanic
- **GIVEN** the final mission's coil feed is powered and the mail van is following within boarding distance
- **WHEN** its driver uses an available chrono pad
- **THEN** the driver and van arrive at the matching pad and the other seat can stay behind

#### Scenario: First-pass pacing
- **WHEN** normal-input solo and split replays take each expedition from arrival through extraction
- **THEN** every clear takes 17 to 23 simulated minutes and the test reports each measured duration separately from human first-clear observations

### Requirement: Field practice and optional return rooms
Each of the 24 expeditions SHALL use an authored two-approach ward room in each sector. The party SHALL earn the return to the entrance from the ward room. Field actions SHALL provide a benefit in transit, with guidance for missed compulsory skills. Each final sector SHALL include a safe rest and three signature encounters before extraction.

#### Scenario: Scout a return path together
- **GIVEN** a party at a fresh sector entrance
- **WHEN** one seat performs the indicated field action in the side room
- **THEN** both seats gain access to a two-way return line and share protection against one sentry contact

#### Scenario: Reach a missed skill and close the mission
- **WHEN** a runner reaches the end of their tools without completing the sector's field practice
- **THEN** the compass leads them to an appropriate reachable mark, and they can finish without restarting or spending a finite resource

#### Scenario: Bring the van home
- **WHEN** the party boards its loaded van for the final chrono shift
- **THEN** the driver and the van arrive at the matching station and the party may rest before the final three actions

### Requirement: Validate authored maps and play styles
The validator SHALL check reachability on arrival and after commissioning, search the ward lock in both states, report optional rooms, cycles, dead ends, reward spacing and critical travel, and print any requested sector as an ASCII map.

#### Scenario: Validate the December campaign
- **WHEN** the complete route audit runs
- **THEN** all 120 sectors are reachable and each mission clears through guided solo, guided split, rush, exploration and cautious input policies, with the final demonstration beginning in the last quarter
