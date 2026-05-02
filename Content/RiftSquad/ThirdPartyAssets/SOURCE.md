# Third Party Asset Register

Record every imported external asset before it is used in a playable build.

| Asset | Source URL | License | Imported To | Replacement Priority | Notes |
| --- | --- | --- | --- | --- | --- |
| UE Basic Shapes | Engine install: `/Engine/BasicShapes/*` | Unreal Engine Licensed Technology / Starter Content terms | Runtime C++ fallback arena, player, enemy, projectile | High | Used only as temporary demo placeholders. Replace with authored assets before public branding. |

## Approved Free Asset Candidates

These are approved candidates, not yet imported. Verify the exact asset page license before importing any file.

| Candidate | Source URL | Preferred License | Intended Use | Notes |
| --- | --- | --- | --- | --- |
| Kenney UI / Input Prompts / Low Poly Kits | https://kenney.nl/assets | CC0 where listed on the asset page | UI icons, keyboard prompts, simple props | Good first source for clean placeholder art. |
| OpenGameArt CC0 / CC-BY assets | https://opengameart.org | CC0 preferred; CC-BY only with credits | SFX, icons, simple textures | Must record author, asset URL, and license per imported asset. |

## Open Source Reference Projects

These are code/design references only. Do not copy assets or code blindly.

| Project | Source URL | Use As | Notes |
| --- | --- | --- | --- |
| Tom Looman Action Roguelike | https://github.com/tomlooman/ActionRoguelike | Unreal C++ architecture reference | Useful for ability/action patterns, multiplayer, UI, data-driven enemies. Check project license and asset notes before reuse. |
