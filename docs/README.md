# Documentation

Start with the guide for what you want to do. Commands run from the repository
root unless a guide says otherwise.

| Goal | Guide |
| --- | --- |
| Learn the controls and gameplay changes | [Gameplay](gameplay.md) |
| Understand statistics and records | [Player statistics](player_stats.md) |
| Build a ROM | [Building](building.md) |
| Change the source or contribute a fix | [Contributing](contributing.md) |
| Run the relevant tests | [Testing](../tests/README.md) |
| Identify optional configurations | [Optional components](optional-components.md) |
| Prepare a source archive | [Source releases](source-release.md) |
| Find contributors and asset origins | [Credits](../credits.md), [asset sources](asset-sources.md) |

## Developer references

- [Coding standards and quality checks](source-quality-standards.md).
- [Build dependency versions and provenance](build-dependencies.md).
- [Nintendo 64 performance and measurement limits](n64-performance.md).
- [Recording performance captures](performance-capture.md).
- [GCC execution and branch profiling](gcc-profiling.md).
- [Statistics storage and event tracking](reference/player-stats.md).
- [Physics Lab geometry and fixtures](../levels/physics_lab/README.md), with its source maps.
- [Detailed gameplay rules](../gameplay_rules.md), for exact timing and edge cases.

These references describe the shipped implementation and tools. Agent operating
instructions, plans, old proposals and experiment history live in the optional
`llm/` directory. They are not prerequisites for using these guides.
