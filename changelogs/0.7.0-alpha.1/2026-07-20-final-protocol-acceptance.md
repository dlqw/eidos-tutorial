# Compiler-managed protocol acceptance

- Synchronized the tutorial baseline and manifest metadata with Eidos 0.7.0-alpha.1.
- Kept public examples on typed `meta.Type -> meta.Items` generation, typed declaration tags, and structured extern declarations.
- Use the final `meta.instance(trait, target, methods)` constructor without retaining the interim `meta.implementation` spelling.
- Teach name-first instances instead of function-level `@impl(...)`, and document clean build environments, deterministic provenance/SBOM metadata, and release rejection of volatile capabilities.
