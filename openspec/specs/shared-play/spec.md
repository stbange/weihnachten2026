# Shared Play Specification

## Purpose

Provide an observable baseline for one-device and browser-to-browser co-op in the public playtest.

## Requirements

### Requirement: Split-screen seats
The game SHALL render two independent first-person cameras with shared objectives in local two-seat play.

#### Scenario: Independent movement
- **WHEN** only the second player moves
- **THEN** the first camera stays in place and the second player's position changes

### Requirement: Browser host authority
The browser host SHALL simulate both seats, select the calendar count and publish validated versioned snapshots to the guest.

#### Scenario: First peer connection
- **GIVEN** two browsers with an open data channel after invitation and reply
- **WHEN** the guest moves or folds an usher
- **THEN** the host simulates the input and both browsers display the resulting state

#### Scenario: Invalid snapshot
- **WHEN** a guest receives a snapshot with an invalid protocol version, non-finite coordinate or out-of-range seat count
- **THEN** it retains the previous world state

### Requirement: WebSocket relay alternative
Two browsers SHALL be able to run the same host-authoritative game through a user-supplied WebSocket relay using a private room code.

#### Scenario: Guest performs a shared action
- **GIVEN** a host creates a relay room and a guest joins with its code
- **WHEN** the guest pulses an upright usher
- **THEN** both players see the same incremented folded-usher count
