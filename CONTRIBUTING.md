# Contributing to the Eidos tutorial

Create a short-lived branch from the latest `main` and open a pull request back
to `main`. Use a Conventional Commits subject and keep language, grammar, and
examples synchronized.

Before submitting changes, run:

```powershell
powershell -ExecutionPolicy Bypass -File verify-examples.ps1
powershell -ExecutionPolicy Bypass -File scripts/verify-release-metadata.ps1
```

Grammar or semantics changes must be coordinated with a compatible Eidosc
release. Examples should use the current documented syntax and include their
expected verification status. Do not commit compiled native libraries,
executables, editor state, logs, credentials, or private development records.
