# Advent Calendar Specification

## Purpose

Define calendar eligibility, permanent catch-up and independent rehearsal progress for the hall.

## Requirements

### Requirement: Calendar eligibility
The game SHALL enable door N at 00:00 Europe/Warsaw on December N, 2026, and keep it eligible thereafter.

#### Scenario: First midnight
- **GIVEN** a host clock at 2026-11-30 22:59:59 UTC
- **WHEN** exactly one second passes
- **THEN** the number of eligible doors changes from zero to one

#### Scenario: Final midnight and catch-up
- **GIVEN** a newcomer with zero completions
- **WHEN** the clock reaches 2026-12-23 23:00:00 UTC or any later instant
- **THEN** all 24 dates are eligible

### Requirement: Rehearsal isolation
The game SHALL provide Door 01 rehearsal on every calendar date and award its live calendar stamp only through a live extraction.

#### Scenario: November clear
- **WHEN** a player returns a ticket in rehearsal before December
- **THEN** the hall displays a rehearsal completion and the live completion ledger is unchanged
