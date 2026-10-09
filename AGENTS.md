# AGENTS.md

- Use plain, natural language when replying and writting. Avoid complex or hard-to-follow wording. Aim for an “explain it like I’m 5” approach while keeping it professional.
- Treat the prompt as transcribed text from a speech-to-text app, so some terms may be inaccurate or misleading. If it's confusing, ask the user for clarification.
- Write commit messages using Conventional Commits (e.g. `feat: ...`). Keep them to 1 or 2 short lines. Avoid additional descriptions unless strictly necessary, and never add anyone as a co-author.
- When creating new branches, use conventional branch names (e.g. `feat/calendar`, `fix/button`, `chore/tooling...`).
- When fixing a bug, first reproduce it in an end-to-end scenario that closely matches how an end user experiences it. Base the fix on the reproduced behavior.
- Avoid long comments on functions, constants, classes, etc. Keep comments concise or omit them when the code is self-explanatory.
- Point out incorrect assumptions, mistakes, or misunderstandings when they affect the solution. Do not validate incorrect conclusions.
- When making technical decisions, prioritize correctness, simplicity, robustness, scalability, and long-term maintainability over implementation effort, unless the user explicitly asks for the quickest or lowest-cost solution.
- Preserve the user's existing coding style unless there is a clear reason to change it.

## Development

- Work in the `main` branch unless told otherwise
- When you need to test the app, close the current app if its running and replace it with the new one to test
- Avoid duplicating logic, component structure, or styles. Extract shared code when duplication is intentional and likely to be maintained together, but do not introduce unnecessary abstractions for one-off cases.
- Keep changes focused. Avoid unrelated refactors or drive-by improvements unless they are necessary to implement the requested change.
