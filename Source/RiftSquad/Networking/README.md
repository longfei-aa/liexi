# Networking Module

Owns session wrappers and multiplayer flow glue.

Milestone 1 only reserves server-authoritative combat paths. Session creation starts in Milestone 3.

## Current Authority Paths

- `URiftWeaponComponent::ServerRequestFire`: client requests basic fire; server spawns projectile and owns damage.
- `URiftAbilityComponent::ServerRequestShockwave`: client requests active skill; server performs overlap query and damage.
- `URiftAbilityComponent::ServerRequestDash`: client requests dash; server moves the character.
- `ARiftPlayerController::ServerSelectReward`: client submits reward choice; server applies reward and advances room flow.

## Milestone 3 Priority

Before adding Steam/EOS:

1. Validate two-player PIE Listen Server.
2. Confirm host and client can both move, fire, dash, shockwave, clear rooms, choose rewards, and see the same victory/defeat state.
3. Only after this passes, add local session wrappers around `OnlineSubsystemNull`.
