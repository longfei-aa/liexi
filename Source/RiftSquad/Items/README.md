# Items Module

Owns item definitions, runtime inventory, stat modifiers, and reward choices.

## Current Implementation

- `URiftItemDefinition`: DataAsset shape for future authored item assets.
- `URiftItemInventoryComponent`: runtime inventory on the player character.
- `URiftItemLibrary`: temporary C++ item library and reward-option generator.
- `FRiftRewardOption`: lightweight replicated reward payload used by `GameState` and HUD.
- `ARiftRoomManager`: asks `URiftItemLibrary` for three reward options when a room is cleared.

## Built-In MVP Item Pool

The temporary pool contains 12 item definitions:

- `ITEM_OverloadCapacitor`: weapon damage.
- `ITEM_RapidCoil`: fire rate.
- `ITEM_RiftPlate`: max health.
- `ITEM_FieldRepair`: heal.
- `ITEM_LightBoots`: movement speed.
- `ITEM_CooldownCore`: skill cooldown.
- `ITEM_ShockAmplifier`: shockwave damage.
- `ITEM_DiffusionModule`: shockwave radius.
- `ITEM_HeavyRounds`: large weapon damage.
- `ITEM_CombatInjector`: large movement speed.
- `ITEM_RiftBattery`: large fire rate.
- `ITEM_CoreResonator`: large shockwave damage.

## Next Step

Move the temporary C++ pool from `URiftItemLibrary::BuildDefaultItemPool` to authored `URiftItemDefinition` assets or a DataTable under `Content/RiftSquad/Data`.
