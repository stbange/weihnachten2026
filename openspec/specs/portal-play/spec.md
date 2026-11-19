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
Each eligible arch SHALL admit the party to its dated expedition. Each expedition SHALL provide five reachable sectors, three shared tool objectives per sector, a navigable exit, and a distinct keepsake. An explicitly selected playtest SHALL allow the same route on any date.

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
