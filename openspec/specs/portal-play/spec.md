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
