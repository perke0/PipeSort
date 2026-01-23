# Contributing to PiedPiperSort

Thanks for wanting to contribute — big or small, it all helps.

## Ways to contribute
- Fix bugs
- Improve performance or readability
- Add tests or benchmarks
- Add a new algorithm (with explanation + tests)
- Improve documentation

## Before you start
- Check existing issues and pull requests to avoid duplicates
- If you want to add a major feature, open an issue first so we can align

## Branching
- `main` = stable
- `test/*` or `experiment/*` = new ideas / risky changes
- Use descriptive branch names, like `fix/overflow-bug` or `experiment/branchless-merge`

## Pull Request checklist
- [ ] Clear description of what changed and why
- [ ] Tests added/updated (if needed)
- [ ] Benchmarks included for performance changes
- [ ] No unrelated formatting-only changes unless necessary

## Style & quality
- Prefer clarity first, micro-optimizations second (unless the goal is performance)
- Comment tricky code paths
- If you change performance-critical code, include benchmark results if possible

## Commit messages
Keep them simple:
- `fix: handle empty input`
- `perf: reduce branch mispredicts in partition`
- `docs: update benchmark instructions`
