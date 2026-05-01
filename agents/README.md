# Agent Index

This directory contains local expert-context files for `Rift Squad`.
These are not external plugin agents and do not depend on any global `.agents` registry.
Each file is a self-contained specialist role used by `../MAIN_AGENT.md` for task dispatch.

## Expert Files

- `01-story-world-agent.md`
- `02-gameplay-systems-agent.md`
- `03-character-systems-agent.md`
- `04-weapon-systems-agent.md`
- `05-enemy-boss-agent.md`
- `06-level-procgen-agent.md`
- `07-items-progression-agent.md`
- `08-networking-agent.md`
- `09-technical-architecture-agent.md`
- `10-art-visual-agent.md`
- `11-music-audio-agent.md`
- `12-ui-ux-agent.md`
- `13-balance-economy-agent.md`
- `14-production-schedule-agent.md`
- `15-testing-release-agent.md`
- `16-risk-management-agent.md`
- `17-milestone-acceptance-agent.md`

## Usage Rule

- single-domain task: load one expert file
- cross-domain task: load one lead expert plus the required support experts
- project-level task: start with `../MAIN_AGENT.md`

## Project Source Of Truth

The main project design reference is:

- `../裂隙小队_游戏开发计划书.md`
