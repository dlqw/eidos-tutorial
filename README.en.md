# Eidos Tutorial (English)

> Language baseline: this tutorial targets Eidos 0.6.0-alpha.1. New code uses `name :: Type { ... }`, `name :: expr;`, local `name := expr;` / `mut name := expr;`, dot-qualified Namespaces, and comma-separated ADT constructors. Older source is handled only by the explicit migration command.

## 1. Scope and Validation Baseline
This tutorial only describes features that are implemented and reproducible in this repository today. All runnable snippets are under [`docs/tutorial/examples/`](examples/).  
The grammar source of truth is `src/Eidosc/Eidosc/Grammar/GrammarDefine.cs`, and the validation baseline is local execution on 2026-03-19.

## 2. Quick Start
Make sure the compiler works:

```powershell
dotnet build src/Eidosc/Eidosc.sln
dotnet run --project src/Eidosc/Eidosc.Cli -- --help
```

Run the tutorial verification script:

```powershell
powershell -ExecutionPolicy Bypass -File docs/tutorial/verify-examples.ps1
```

Recommended minimal `eidos.toml`:

```toml
manifestSchema = 3

[language]
version = "0.6.0-alpha.1"

[package]
name = "dev.eidos.app"
version = "0.1.0"
```

Meaning:
1. `language.version` is the user-controlled Eidos language SemVer.
2. `manifestSchema` is the manifest schema version; current checked-in manifests declare schema `3` explicitly.

## 3. Core Language Flow (Validated)
### 3.1 Literals and Bindings
Example file: `examples/01_literals_bindings.eidos`  
Covers `let`, `let mut`, and literal forms.

```eidos
answer :: 42;
pi :: 3.14;
greeting :: "hello";
ok :: true;
initial :: 'a';
let mut counter = 1;
```

Naming tiers: runtime values use lower-case-leading identifiers, while compile-time values use upper-case-leading identifiers. Types are first-class compile-time values, so types, traits, effects, constructors, module path segments, and generic parameters that denote types belong to the upper-case namespace. A constructor call produces a runtime value, but the constructor symbol itself is still a compile-time value.

#### Namespace and naming style

Eidos 0.6 uses one member-selection surface:

```eidos
import Std.Option

Info :: comptime Meta.typeInfo(User);
fallback := Std.Option.unwrap_or(input)(0);
name := user.profile.display_name;
```

1. `::` binds a declared name to its declaration and has no qualified-name meaning.
2. `.` selects through every Namespace: packages, modules, types, traits, effects, constructors, compile-time values, and their members.
3. Module segments, types, traits, effects, constructors, and compile-time values start upper-case. Runtime functions, values, parameters, and fields start lower-case and should prefer `lower_snake_case`.
4. A dependency alias may remain `lower_snake_case`; its next segment is an upper-case module, as in `crypto_a.Hash.Sha256.digest(bytes)`.
5. A lower-case root followed by lower-case segments is an ordinary runtime member chain, such as `user.profile.display_name`; an upper-case root, or a lower-case package alias followed by an upper-case module, starts a Namespace path.

Update (2026-03-16): builtin I/O now includes `print_char: Int -> Unit`, which prints one character by code point (for example `34` for `"`, `39` for `'`).

### 3.1.1 `let` Pattern Binding (Block Scope)
Example files: `examples/12_let_pattern.eidos`, `examples/15_pattern_binding_modes.eidos`  
`let <pattern> = <expr>;` is now supported in block scope and wired through NameResolver/Types/HIR/MIR.

```eidos
sum_pair :: (Int, Int) -> Int
{
    pair => {
        let (a, b) = pair;
        a + b
    }
}
```

Current constraint (2026-03-18): `let` currently requires an irrefutable pattern. For refutable cases, use `match`.

Update (2026-04-04 / 2026-06-19): pattern binding modes and mutable bindings are now wired through semantic analysis and MIR:
1. The primary spellings are `let ref x = ...` and `let mref x = ...`.
2. `p as ref x` / `p as mref x` preserve the binding mode through later phases.
3. Reassignable bindings use `let mut x = ...`; only `let mut` bindings can be assignment targets for `:=`.
4. In `or-pattern`, the same binding name must use the same binding mode across alternatives; otherwise `E3000` is emitted.

### 3.1.2 `if let` Pattern Branching
Example file: `examples/13_if_let_pattern.eidos`  
Supported form: `if let <pattern> = <expr> then <expr> else <expr>`; block branches are still valid because blocks are expressions. It is lowered with `match` semantics:

```text
if p := e then { t } else { f }
==> match e { p => { t }, _ => { f } }
```

Short branches may be written directly as `if p := e then t else f`.

Binding scope rules:
1. Pattern-bound variables are visible only in the `then` branch.
2. The `else` branch and outer scope do not see those bindings.
3. Without `else`, the fallback branch value defaults to `Unit`.

### 3.1.3 `while let` Pattern Looping
Example file: `examples/14_while_let_pattern.eidos`  
Supported form: `while let <pattern> = <expr> then <block>`. It is lowered as a looped pattern check:

```text
while p := e then { body }
==> loop { match e { p => { body; () }, _ => break } }
```

Current behavior:
1. Pattern-bound variables are visible only inside the loop body block.
2. Match failure exits the loop automatically (via internal `break` semantics).
3. The expression type is always `Unit` and is intended for control flow.
4. A block can still end with an unsuffixed tail expression after `if/if let/while let` statements (for example `...; total`).

### 3.1.4 `let?` Option / Result Binding
Example file: `examples/63_let_question_option_result.eidos`
Block-level `let? <pattern> = <expr>;` is supported for unwrapping the success value of `Option` / `Result` and returning early from the enclosing function or lambda on the failure branch.

```eidos
option_pipeline :: Int -> Option[Int]
{
    value => {
        let? first = maybe_positive(value);
        let? second = maybe_positive(first);
        Some(second + 1)
    }
}

result_pipeline :: Int -> Result.ResultWith[String, Int]
{
    value => {
        let? first = parse_positive(value);
        let? second = parse_positive(first);
        Ok(second + 1)
    }
}
```

Rules:
1. `let?` is not a top-level declaration; it is valid only inside a function or lambda block with a return context.
2. The right-hand side must be `Option[A]` or `Result[A, E]`.
3. `Option[A]` is valid only in an `Option[R]` return context and short-circuits with `None()`.
4. `Result[A, E]` is valid only in a `Result[R, E]` return context, or a canonical equivalent alias, and short-circuits with the original `Err(e)`.
5. There is no implicit `Option -> Result` conversion and no implicit error mapping.
6. The left-hand pattern must currently be irrefutable; use `match` / `if let` for refutable destructuring.

Lowering rule: `let?` exists only through Parser/AST/NameResolver/Types. HIR construction eliminates it into ordinary `match` + `return`; HIR/MIR/LLVM have no dedicated `let?` nodes.

### 3.2 Functions, Generics, and Direct Calls
Example file: `examples/02_functions_calls.eidos`  
Covers function signatures, type parameters, and direct calls.

```eidos
inc :: Int -> Int
{
    x => x + 1
}

identity[T] :: T -> T
{
    x => x
}

add :: Int -> Int -> Int
{
    left => right => left + right
}

sum3 :: Int -> Int -> Int -> Int
{
    a => b => c => a + b + c
}

a :: inc(1);
b :: identity(42);
c :: add(1, 2);
d :: sum3(1, 2)(3);
e :: 1 `add` 2;
```

For curried functions, comma-separated call arguments apply the function left-to-right: `add(1, 2)` is equivalent to `add(1)(2)`, and `sum3(1, 2)` can still return a function waiting for the final argument. Backtick infix calls use the same rule: ``left `add` right`` is equivalent to `add(left)(right)`.

#### Value-level const generics (0.5.0-alpha.1)

`comptime N: Int` declares a value-domain generic parameter, while `comptime T: Type` remains a type-domain parameter. Declaration order is application order. The compiler preserves type, value, and effect-row domains independently through AST, symbols, HIR/MIR, caches, and IDE output instead of representing values as fake type arguments.

```eidos
Buffer[comptime N: Int, comptime T: Type] :: type {
    Buffer(T)
}

FixedBuffer[comptime N: Int, comptime T: Type] :: type = Buffer[N, T];

constant[comptime N: Int] :: Unit -> Int
{
    _ => N
}

use :: Unit -> Int
{
    _ => constant[4](()) + constant[5](())
}
```

A value argument must be compile-time evaluable at the instantiation site and must match its declared type. A value parameter that is not constrained by ordinary parameter types must be supplied explicitly; it may be omitted when it can be inferred through a type such as `Buffer[N, T]`. Values participate in nominal type identity, layout, name mangling, generic specialization, trait coherence, and incremental cache keys, so `Buffer[4, Int]` and `Buffer[5, Int]` are distinct types. Floating-point values are not specialization keys in alpha.1. References, pointers, closures, and other values with runtime resource identity cannot cross the comptime/type-identity boundary.

ADTs, type aliases, functions, traits, `@impl(...)`, and named-instance trait references use the same ordered generic-argument rules. For example, an implementation of `Sized[comptime N: Int] :: trait { ... }` must explicitly name a value such as `@impl(Sized[4])`; `N` is substituted through the trait method signature and coherence compares the structured value key. See `examples/68_const_generics.eidos` for the complete example.

#### Read-only reflection, user derives, and structured generation (0.5.0-alpha.3)

`Meta` is a compiler-built-in domain. It is not part of `Std` and does not require an `import`. Reflection exposes only completed, stably serializable compiler facts; it does not expose mutable AST objects or internal `SymbolId` values:

```eidos
Info :: comptime Meta.typeInfo(User);
Kind :: comptime Meta.typeKind(Info);
HasName :: comptime Meta.hasField(User, "name");
NameType :: comptime Meta.fieldType(User, "name");
IntLayout :: comptime Meta.layoutOf(Int, "x86_64-pc-windows-msvc");
```

`Meta.typeInfo` covers primitives, tuples, functions, references, ADTs, and traits. Constructors, fields, associated declarations, and attributes retain stable source order. `Meta.layoutOf` is a separate target-dependent query and requires an explicit supported target triple. A type whose layout is not complete produces a phase diagnostic instead of falling back to host layout.

A user derive must have the type `comptime Meta.DeriveInput -> Meta.Expansion`:

```eidos
Marker :: trait {
    marker :: Self -> String
}

deriveMarker :: comptime Meta.DeriveInput -> Meta.Expansion {
    input => {
        target := Meta.target(input);
        parameter := Meta.parameter("value", target);
        method := Meta.function(
            "marker",
            [parameter],
            String,
            Meta.exprString(Meta.typeName(target))
        );
        Meta.expansion([
            Meta.implementation(Meta.decl(Marker), target, [method])
        ])
    }
}

@derive(deriveMarker)
User :: type {
    name: String,
    age: Int
}
```

`Meta.Expansion` can structurally generate implementations, functions, comptime values, attribute attachments, tests, diagnostics, and module members. Function bodies use `Meta.Expr` builders, while pattern branches use `Meta.Pattern` / `Meta.Branch` builders. Declaration references use `Meta.Decl`; generator-local references use `Meta.Parameter` / `Meta.Binding` handles for hygiene. Generated declarations participate in ordinary name resolution, type checking, trait coherence, completion, hover, definition, and references, with stable `eidos-generated://` origins.

Current boundaries: there is no string source insertion and no arbitrary AST replacement or deletion. Semantic attributes such as `@impl` cannot be written back through an attribute attachment; generate the corresponding structured declaration instead. Pure comptime cannot access files, environment variables, processes, networks, or FFI. Expansions execute to a fixed point in dependency and source order; cycles, duplicate stable identities, invalid protocol signatures, budget exhaustion, and incomplete reflection produce deterministic diagnostics. See `examples/69_meta_reflection_derive.eidos` for the complete example.

CLI surfaces:

```powershell
eidosc meta expand source.eidos --format json
eidosc meta expand source.eidos --emit-generated generated
eidosc meta expand source.eidos --trace-comptime --comptime-budget 200000
```

#### Capability-constrained Build host and BuildGraph (0.5.0-alpha.4)

Like `Meta`, `Build` is a compiler-built-in domain that needs no import, but capabilities are issued only while compiling the program named by `[build].program`. Ordinary pure comptime may resolve `Build.context()` but is rejected if it tries to acquire filesystem, environment, process, or artifact-emission capabilities.

```toml
[build]
program = "build.eidos"
fileInputs = ["schema/model.json", "assets"]
environment = ["SDK_ROOT"]
outputRoots = ["build/generated"]

[[build.tools]]
name = "generator"
path = "tools/generator.exe"
```

`fileInputs` may name files or directories. Directories expand recursively in stable project-relative order. Input contents, registered environment-variable presence and values, registered executable path/hash identity, the build program, and the host and target triples all participate in the Build host cache key. The build program, file inputs, and outputs must remain inside the project root; the program and file inputs must be disjoint from output roots. Tools use explicit registered paths rather than ambient `PATH` lookup.

A build program uses separate `Build.Fs`, `Build.Env`, `Build.Process`, and `Build.Emit` capabilities and returns one top-level `BuildGraph`:

```eidos
Context :: comptime Build.context();
Process :: comptime Build.process(Context);
Emit :: comptime Build.emit(Context);

Generate :: comptime Build.command(
    Process,
    "generate",
    "generator",
    ["schema/model.json", "build/generated/Model.eidos"],
    ["schema/model.json"],
    ["build/generated/Model.eidos"],
    []
);

Generated :: comptime Build.generatedSource(
    Emit,
    "build/generated/Model.eidos",
    "generate",
    "main"
);

BuildGraph :: comptime Build.graph(Emit, [Generate], [Generated]);
```

`Build.readText(Fs, path)` can read only files expanded from `fileInputs`; `Build.environment(Env, name)` can read only a registered variable that is present. `Build.command` describes a registered host tool, its arguments, inputs, outputs, and step dependencies; it does not perform hidden side effects during expression evaluation. The compiler checks cycles, duplicate outputs, undeclared inputs, missing dependency edges, output roots, producers, and target use, then executes steps in stable topological order and verifies every declared output. A `generatedSource` directory is added automatically to the selected target's import roots.

```powershell
eidosc build --project . --target-name main --trace-build
eidosc build --project . --emit-build-graph build/graph.json
```

`--trace-build` reports capability dependencies and accesses, graph hashes, step/cache decisions, and tool output. `--emit-build-graph` writes canonical JSON. Identical programs, declared inputs, environment, executable hashes, host/target pairs, and graphs reuse a BuildGraph cache whose output hashes are revalidated. See `examples/build_host/` for a minimal verified project.

Ordinary same-scope functions may share a name when their parameter signatures
are distinct. Calls resolve the overload from argument types, including direct
calls, qualified calls, method-call syntax, infix calls, and pipe calls. A bare
overloaded function reference needs an expected function type; otherwise the
compiler reports an ambiguity.

```eidos
render :: String -> String
{
    text => text
}

render :: Int -> String
{
    _ => "number"
}

show_int :: Int -> String = render;

direct :: render(1);
method :: "ok".render();
```

Duplicate overloads are checked by parameter signature only: return type,
effect requirements, source order, and body shape do not make two overloads
distinct. For example, `parse :: String -> Int` and
`parse :: String -> String` are rejected as duplicate overloads. Generic
signatures are alpha-normalized, so `id[T] :: T -> T` and `id[U] :: U -> U`
are also duplicates.

Symbolic functions can be declared with a parenthesized operator name and then used as infix operators:

```eidos
(|+|) :: Int -> Int -> Int
{
    left => right => left + right
}

asInfix :: 1 |+| 2;
asFunction :: (|+|)(3, 4);
```

Custom symbolic operators are left-associative at the default custom-infix precedence, between additive operators and function composition. `::` is a declaration binding token, not an expression operator. Built-in operators such as `|>`, `>>=`, `>>>`, `<<<`, `<$>`, `<*>`, and `<>` keep their standard precedence and lowering.

Style convention (2026-05-28): new tutorial and standard-library examples should prefer Eidos' functional reading style. Use `value |> f |> g` for linear data flow, `f >>> g` or `g <<< f` for function composition, `f <$> value`, `mf <*> mx`, and `mx >>= f` for `Functor` / `Applicative` / `Monad` code, and chained calls such as `xs.map(f).filter(p).fold_left(seed)(step)` for container pipelines. Keep `Module.function(value)(arg)` when the qualified path makes intent or dispatch clearer. Ordinary grouped calls such as `function(value)` / `function(value, arg)` are also stable default call style and are not reported only because they could be written as fluent or infix calls. CLI/IDE/LSP reports mechanically convertible consecutive curried prefix calls as help/hint-level style suggestions with Quick Fixes: `Seq.append(a)(b)` can become fluent `a.append(b)` or grouped as `Seq.append(a, b)`.

Update (2026-03-27): Higher-kinded type parameter annotations are supported in signatures.
1. Unary constructor kind: `F: kind2`
2. Multi-parameter constructor kind: `F: kind3`
3. Parenthesized higher-order kind is supported: `F: kind2 -> kind1`
4. Kind compatibility is checked both for constructor-parameter application (for example `F[G]`) and ADT type arguments (for example `Lift[Int]` is rejected when `Lift[F: kind2]`).
5. Named type constructors can be partially applied in higher-kinded positions (for example `Lift[Either[String]]` when `Lift[F: kind2]`).
6. Omitted kind annotations are inferred from type usage (for example `F[A]` infers `F: kind2`).
7. Omitted ADT type-parameter kinds are inferred from constructor/alias type usage (for example `type Lift[F] { Lift(F[Int]) }` infers `F: kind2`; `type UseK[K] { UseK(K[Box]) }` infers `K: kind2 -> kind1`, so `UseK[ApplyToInt]` is valid when `type ApplyToInt[F: kind2] { ... }`).
8. Effect-row parameters are explicit and use the `effects` kind (for example `E: effects`); higher-kinded value type parameters continue to use `kind1`, `kind2`, or `kind3`.
9. Kind constraints are solved by cumulative unification: constraints on the same type variable share one inferred kind state, so incompatible constraints (for example both `kind2` and `kind1`) are reported as errors instead of being treated independently.
10. Trait declarations support type parameters as first-class syntax (`Functor[F: kind2] :: trait { ... }`), and trait type-parameter kinds are also inferred from method signatures when omitted (for example `HK[K] :: trait { run :: K[Box] -> Self }` infers `K: kind2 -> kind1`), with metadata preserved into symbol/HIR layers.
11. Type-parameter trait constraints support module-qualified trait refs with type arguments (`T: Core.Functor[Box]`); trait-argument arity and kind are checked in Types phase.
12. Type-parameter trait constraints accept traits only. Effect authorization belongs in function `need` clauses instead (for example `String -> Unit need Writer`).
13. Generic constraints support lightweight `where` clauses, so complex kind/trait constraints can move out of the parameter list; for example, `lift[A, G] :: A -> G[A] where G: kind2, G: Applicative[G]`.
Example files: `examples/31_hkt_parenthesized_kind.eidos`, `examples/32_hkt_adt_inferred_kind.eidos`, `examples/33_hkt_effect_polymorphism.eidos`, `examples/34_hkt_trait_inferred_kind.eidos`, `examples/35_hkt_trait_constraint_type_args.eidos`, `examples/36_hkt_trait_constraint_kind_mismatch.eidos`.

Update (2026-03-22): `return <expr>` now unifies `<expr>` with the current function/lambda declared result type (instead of degrading to `Unit`). This removes false `Int` vs `()` mismatches in valid early-return code paths.

Update (2026-04-04): source unary deref `*expr` and reference constructors `ref expr` / `mref expr` have entered the first explicit-reference stage.  
Current semantics:
1. `ref expr` now yields `Ref[T]` in Types.
2. `mref expr` now yields `MRef[T]` in Types.
3. `*expr` now requires a `Ref[T]` or `MRef[T]` operand and yields the inner `T`.
4. `MRef[T]` is the preferred writable-reference spelling; legacy `MutRef[T]` is still accepted as a compatibility alias for now.
5. `&expr` is still accepted for compatibility, but is now a transitional spelling rather than the intended long-term surface syntax.
6. `ref expr` / `mref expr` / `&expr` now only work on stable places: locals, fields, indexes, or `*r`. Forms like `ref (x + 1)` or `mref make()` now fail in Types instead of borrowing from a temporary.
7. Read-only projection paths have started to auto-penetrate small cases: `Ref[Seq[Int]]` / `MRef[Seq[Int]]` can now be indexed directly as `xs[0]` without first writing `(*xs)[0]`.
8. Read-only parameter positions now accept an automatic `MRef[T] -> Ref[T]` downgrade: for example, `read: Ref[Int] -> Int` can be called directly as `read(mref x)`.
9. Bare dot access `a.b` now prefers record-style ADT field reads, while `a.b()` remains a method call. That means `Ref[Range]` / `MRef[Range]` can now read fields directly as `r.start` without first writing `(*r).start`.
10. Record-style ADT field types can now mention `Ref[T]` / `MRef[T]` directly, so chained read-only method style such as `box.reader.read` and `box.writer.read` is also supported.
11. Returning `Ref[T]` / `MRef[T]` now has a first-stage hard rule: the returned reference must be traceable to an input parameter; directly returning a parameter or returning a local alias of a parameter reference is allowed, but forms like `ref local`, borrowing from a local/temp and returning that borrow, or overwriting a parameter alias with a local borrow before returning it now fail in Borrow with `E1004`.  
12. Full value-category semantics are still being stabilized; this is not the final memory model yet.  
Example files: `examples/39_unary_deref.eidos`, `examples/40_unary_ref.eidos`, `examples/41_adt_ref_fields.eidos`, `examples/54_return_borrow_param_alias.eidos`.

### 3.2.1 Two First-Class Function-Body Branch Forms
Example file: `examples/30_curried_pattern_branch.eidos`

1. Tuple-head form: `(p1, p2) => expr`
2. Curried-head form: `p1 => p2 => expr`

Both forms are semantically equivalent. The compiler normalizes the curried head to the same branch-pattern representation; `_` is valid in curried segments (for example `NoneString() => _ => NoneString()`).
Update (2026-04-10):
1. Later curried segments can now carry their own `when` guard directly, for example `n => i when i > 0 => i`.
2. Constructor patterns in later curried segments are preserved as whole patterns; `Some(v) => Some(w) => ...` no longer degrades into a plain variable binding.
3. Function-body pattern branches still share the same coverage analysis as `match`, so forms such as `Some(v) => f => ...` keep participating in stable ADT-constructor `W4200/W4201` reasoning.

### 3.3 Chained Calls (Auto-desugared)
Example file: `examples/03_chain_method_calls.eidos`  
Chained calls are supported and desugared during compilation. Equivalent forms:

```text
obj.m(a, b)  =>  m(obj, a, b)
obj.m        =>  m(obj)
obj.m().n()  =>  n(m(obj))
obj.m.n      =>  n(m(obj))
```

```eidos
id :: Int -> Int { x => x }
inc :: Int -> Int { x => x + 1 }
double :: Int -> Int { x => x + x }

chain :: id(1).inc().double();
chain2 :: id(1).inc.double;
chain3 :: 3.inc.double;
```

### 3.4 ADT and Type Alias
Example file: `examples/04_adt_type_alias.eidos`

The Eidos 0.7 development line also supports lexically sealed closed-case hierarchies. Each nested `Case :: type` is both an exact nominal subtype and a constructor-bearing type:

```eidos
Message :: type {
    Text :: type {
        value :: String,
    },

    Control :: type {
        code :: Int,
    },
}
```

Visibility is atomic for the complete hierarchy. Exporting `Message` exports every descendant case type, constructor, and case field needed for exhaustive matching. If the root is not exported or is compiler-internal, the complete hierarchy stays private. A public root cannot contain a toolchain-internal descendant; the compiler reports `E3061` instead of silently creating a hidden case. Eidos 0.7 does not infer a non-exhaustive or opaque-case mode from descendant visibility.

```eidos
type Option[T] { Some(T), None }
type UserId = Int;

maybe :: Some(1);
uid: UserId  := 7;
```

GADT constructors can explicitly return a specialization of the current ADT head. Ordinary constructors without `->` still mean the full declared head:

```eidos
type Expr[T]
{
    IntLit(Int) -> Expr[Int],
    BoolLit(Bool) -> Expr[Bool]
}

classify[T] :: Expr[T] -> Int
{
    expr => match expr
    {
        IntLit(value) => value,
        BoolLit(flag) => if flag then { 1 } else { 0 }
    }
}

proof_ok :: Unit -> TypeEq[Int, Int]
{
    _ => Refl
}
```

`TypeEq[A, B]` is an erased built-in type. The first version only exposes `Refl`, for identical types or branch-local type equalities refined by GADT patterns.

Constructors may also declare local type parameters. A constructor-local type parameter is visible only in that constructor's arguments and return type; if it does not appear in the return type, it is existential, hidden after construction, and reopened only inside a pattern branch:

```eidos
type AnyDirection
{
    AnyDirection[A](Direction[A]) -> AnyDirection
}

dx_any :: AnyDirection -> Int
{
    AnyDirection(dir) => dir.dx()
}
```

Constructor named blocks only describe runtime fields. `name: Type` is a runtime field and enters constructor arguments, layout, and ABI. `name = expr` is no longer accepted inside ADT constructors; custom trait facts belong in a named instance bridge:

```eidos
DirectionVector :: trait {
    dx :: Self -> Int
    dy :: Self -> Int
}

DirectionLabel :: trait {
    label :: Self -> String
}

type Direction[A]
{
    North -> Direction[Vertical],
    South -> Direction[Vertical],
    East -> Direction[Horizontal]
}

DirectionVectorDirection[A] :: instance DirectionVector for Direction[A] {
    North => { dx = 0, dy = -1 } |
    South => { dx = 0, dy = 1 } |
    East => { dx = 1, dy = 0 }
}

DirectionLabelDirection[A] :: instance DirectionLabel for Direction[A] {
    North => { label = "north" } |
    South => { label = "south" } |
    East => { label = "east" }
}
```

Bridge fact input supports literals, tuples, lists, restricted unary/binary expressions, value names or path references, and constructor/function call expressions. The generated impl copies those expressions into the matching constructor branch and then type-checks them normally, so constructor facts such as `delta = GameMath.up_i` and `opposite = South()` are valid. Bridges support `Self -> R` and extra-argument shapes such as `Self -> A -> R`; `Self` in return types and extra parameters is substituted with the target type.

When constructor instance bridges are combined with GADTs, generated generic impls are checked with branch-local refinement. For example, the `North` branch locally knows `A = Vertical`, and that equality does not leak to other branches or outside the branch.

Update (2026-05-29): record-style ADT values now support short update syntax:

```eidos
reset :: state.{ tick: 0 };
```

The base expression before `.{...}` supplies the ADT record value, unspecified fields are copied through field projections from that base, and explicit fields override those copied values. For ADTs with multiple record constructors, short update preserves the runtime constructor and is accepted only when every updated field is present on every record constructor. The explicit constructor form is still available as `GameState { ..state, tick: 0 }`; in that form, `..base` must appear once at the start of the named constructor fields. Eidos does not use Koka-style call-like update syntax such as `state(tick = 0)`.

Update (2026-03-26): generic constructor argument types are now checked in both expressions and patterns.
1. `Some("x")` cannot be used where `Option[Int]` is expected.
2. `match value { Some("x") => ... }` is rejected when `value: Option[Int]`.
3. Mismatches are reported in Types phase with `E4000`.

Update (2026-03-15): constructor patterns in function branches now preserve both constructor name and positional bindings correctly. `TokCons(tok, tail)` is no longer mis-reconstructed as the last identifier name.

Update (2026-03-16): backend constructor-pattern lowering now takes the real matching path:
1. constructor branches are discriminated via runtime `type_id` + stable constructor tags;
2. pattern field variables are materialized as real MIR field loads;
3. positional fields `_0/_1/...` currently use a fixed `8-byte slot` payload layout.
4. named fields are normalized to `_index` slots in MIR before LLVM lowering to keep field-offset semantics consistent.
5. ADT named-field access now requires unique, total resolution within the target ADT:
   - inconsistent ordinals across constructors: `E3205`;
   - field only present on a subset of constructors: `E3204`;
   - unknown field: `E3203`;
   - unresolved LLVM field offset now reports `E3301` (no silent fallback).

### 3.5 Trait and `@impl`
Example file: `examples/05_trait_impl_declaration.eidos`  
Prefer name-first `instance` declarations for trait evidence. The legacy `@impl(Trait)` function form remains available for existing code, but new examples should use `instance`.
Instance member names and signatures must match the target trait methods and stay in the same module.
For generic traits, `instance` supports explicit trait type arguments (for example `FunctorBox :: instance Functor[Box]`), and arity mismatch is reported during naming.
Convention-based impl registration does not infer generic trait arguments; use explicit `Trait[...]` for generic traits.
`instance` registration canonicalizes aliases before overlap checking. Strictly more specific heads may coexist with broader heads, but equivalent or incomparable canonical shapes still fail with `E3004`.
`expr given InstanceName` can explicitly select a named evidence value.
The legacy `@impl(Trait)` form requires the function name/signature to match the trait method and stay in the same module.
For generic traits, `@impl` supports explicit trait type arguments (for example `@impl(Functor[Box])`), and arity mismatch is reported during naming.
Convention-based impl registration does not infer generic trait arguments; use explicit `@impl(Trait[...])` for generic traits.
Impl heads are compared after alias canonicalization. Strict specialization is allowed when one head is structurally narrower than the other (for example `Option[Int]` over `Option[T]`), but equivalent or incomparable overlaps are still rejected during naming with `E3004` (`overlapping impl registration`). Alias-only rewrites do not create specialization, so overlaps introduced purely through equivalent aliases on either the trait-argument side or the implementing-type side are still rejected.
Open alias heads used as higher-kinded trait arguments now also participate in reverse matching during type inference and MIR specialization. For example, `@impl(Applicative[KeepEdges[String, Bool]])` can satisfy `G[A]` when the surrounding context expects `Triple[String, A, Bool]`; see `examples/43_open_alias_trait_impl.eidos`.
The same reverse matching now survives precompiled stdlib generic combinators as well: `Result.traverse(Ok(2))(produce_keep_edges)` can infer `G = KeepEdges[String, Bool]` even when the callback returns the underlying `Triple[String, Int, Bool]`, and deep alias chains such as `DeepBoxedResult[String]` continue to specialize correctly through both direct and helper-wrapped traversal; see `examples/44_std_traversable_alias_applicative.eidos`.
This now also holds for recursive traversables: `Seq.traverse([1, 2])(produce_keep_edges)` can thread a user-defined alias-backed `Applicative` through repeated `map2_applicative(cons)(...)` specialization without losing the open-alias or deep-alias impl; see `examples/45_std_list_traversable_alias_applicative.eidos`.
Short-circuit traversable branches now behave the same way. `Option.traverse(None())(...)` and `Seq.traverse([])(...)` still specialize `lift_pure`/`pure` through the user-defined alias-backed `Applicative`, so empty inputs no longer depend on the callback path to keep the impl reachable; see `examples/46_traversable_alias_applicative_empty_cases.eidos`.
`Option`, `Seq`, and `Result` now also expose public `sequence` helpers built on top of their traversable implementations. This gives a stable stdlib path for flipping `Option[G[A]]`, `Seq[G[A]]`, or `Result[G[A], E]` into `G[Option[A]]`, `G[Seq[A]]`, or `G[Result[A, E]]`; the current alias-backed coverage is locked through open aliases such as `KeepEdges[String, Bool]`, and built-in `ResultWith[E]` nesting now also works through `Option.sequence(Some(Ok(...)))`, `Seq.sequence([Ok(...), ...])`, and `Result.sequence(Ok(Ok(...)))`; see `examples/47_traversable_sequence_alias_applicative.eidos` and `examples/48_sequence_result_applicative.eidos`.
`Std.Traversable` now also exposes a public generic `Traversable.sequence`, so callers no longer need to choose the outer container-specific helper up front. The generic form now specializes through both user-defined alias-backed applicatives and built-in `ResultWith[E]` nesting for `Option`, `List`, and `Result`; see `examples/49_generic_traversable_sequence.eidos`.
Qualified trait-method paths are now first-class callable value paths as well. In generic code you can call methods through the imported module alias (`Applicative.pure`, `Traversable.traverse`), nested imported-module owner paths (`Trait.Eq.eq`), the fully qualified stdlib root (`Std.Applicative.pure`, `Std.Traversable.traverse`), or the current module's own same-named trait path (for example `Show.show` inside `Demo.Show :: module { ... }`); the precompiled `Std.Traversable` helpers now rely on that module-relative form internally, while `examples/50_qualified_trait_method_paths.eidos` now covers alias/root/nested-import forms together.
Qualified effect paths and ordinary function paths resolve independently. Use `Logger.Logger`, `Io.Writer`, or `Cap.Io.Writer` in `need`, and call ordinary module functions as `Logger.log(...)`, `Io.write(...)`, or `Cap.Io.write(...)`. See `examples/51_qualified_effect_paths.eidos` and `examples/52_nested_qualified_effect_paths.eidos`.

Note (2026-06-18): this branch no longer treats `proof` / lawful material as part of the tutorial baseline; the experimental material lives on the dedicated proof branch.

```eidos
Show :: trait {
    show :: Self -> String
}

type Point {
    Point(Int, Int)
}

ShowPoint :: instance Show {
    show :: Point -> String {
        p => "Point"
    }
}
```

Current trait-constraint solving behavior (2026-03-15):
1. `Tuple` types are checked structurally: a tuple satisfies a trait only if every element type satisfies that trait.
2. `TyCon` (constructed types) uses builtin-trait mapping first, then symbol-table `impl` lookup.
3. If a constraint carries no valid `traitId`, solver falls back to `traitName` lookup before matching `impl`.
4. Generic function trait bounds are now enforced at call-site instantiation (for example `id[T: Marker] :: T -> T` rejects `id(1)` when `Int` has no `Marker` impl).
5. Trait-argument matching is exact: a no-arg bound (for example `T: Functor`) does not match only-specialized impls like `@impl(Functor[Box])`.
6. `@impl` registration canonicalizes aliases before overlap checking. Strictly more specific heads may coexist with broader heads, but equivalent or incomparable canonical shapes still fail with `E3004`. For example, `@impl(Show)` on `Option[Int]` may coexist with `@impl(Show)` on `Option[T]`, while `ResultWith[E, T] = Result[T, E]` and `AlsoResult[E, T] = Result[T, E]` still conflict because both heads collapse to the same underlying shape.
7. Partially applied aliases in generic trait bounds can now be inferred back from concrete underlying return types. A helper like `lift[A, G: kind2 : Applicative[G]] :: A -> G[A]` may specialize through `KeepEdges[String, Bool] = Triple[String, _, Bool]` and produce `Triple[String, Int, Bool]` at the call site.
8. The same alias-backed reverse inference now composes with precompiled stdlib helpers like `Result.traverse`, so user-defined `Applicative` impls over open aliases or deep alias chains remain reachable even when the callback surface uses the underlying constructor type.
9. Recursive traversables such as `Seq.traverse` now preserve the same behavior across repeated `map2_applicative` specialization, so alias-backed `Applicative` impls remain usable beyond single-step containers.
10. Empty/short-circuit traversable branches also preserve alias-backed specialization: `Option.traverse(None())` and `Seq.traverse([])` keep routing through the user impl's `pure`, not just through the callback-driven branch.
11. `Option.sequence`, `Seq.sequence`, and `Result.sequence` are available as public container-specific helpers, and `Traversable.sequence` is now available as the generic outer-container version. Together they inherit the same specialization behavior for stable cases, so nested values like `Option[KeepEdges[String, Bool, A]]`, `Seq[KeepEdges[String, Bool, A]]`, `Result[KeepEdges[String, Bool, A], E]`, `Option[Result[A, E]]`, `Seq[Result[A, E]]`, or `Result[Result[A, E], E]` can be flipped directly through stdlib code.

### 3.5.1 Module `export` and `re-export`
Example file: `examples/53_module_exports_and_reexports.eidos`  
Current behavior:
1. As soon as a module contains any `export` declaration, external visibility switches to explicit-export mode. Non-exported declarations still work inside the module, but they are no longer exposed automatically.
2. If a module contains no `export` at all, the current compatibility behavior remains implicit full export.
3. `export` can now prefix both regular declarations and `import`: for example `export let`, `export let mut`, `export func`, `export effect`, `export trait`, `export type`, and `export import`.
4. `export BaseApi :: import Demo.Base;` publishes the module alias `BaseApi` into the current module's public exports, so downstream code can keep traversing through that re-exported module alias. The 0.4.0-alpha.1 name-first module-alias binding form `Alias :: import Module.Path;` is the canonical import-as syntax; the legacy keyword form `import Module.Path as Alias` is accepted only in `legacy` syntax mode.
5. `export import Demo.Base.{Writer as W}` publishes the selective-import alias as part of the public API, while `export import Demo.Base.*` re-exports all currently visible public bindings from the imported module.
6. `export import` now flows through the real module path model instead of the old string heuristic, so alias re-exports, qualified paths, IDE completion, and the precompiled export table all share the same visibility semantics.

```eidos
Demo.Facade :: module
{
    export BaseApi :: import Demo.Base;
    export import Demo.Base.{Writer as W}
}
```

This `Facade` module publicly exposes a module alias `BaseApi` and an effect alias `W`; ordinary functions are re-exported separately. See `examples/53_module_exports_and_reexports.eidos` for the full runnable example.

### 3.6 List Comprehension (Current Status)
Example file: `examples/08_list_comprehension.eidos`  
Current implementation status:
1. AST/NameResolver/Types support multi-qualifier scope/type behavior (multiple generators + guards).
2. HIR now preserves a dedicated `ListComprehension` node (no early expansion into a constant list).
3. MIR now supports CFG lowering (loop + guard) and builds result lists via runtime `array_new/array_push`.
4. MIR now also supports runtime-length generator loops via runtime `array_length` call.
5. LLVM lowering now maps indexed reads/writes to runtime `array_get/array_set`.
6. CFG join/backedge local updates are now stabilized with slot-backed lowering (entry `alloca` + `load/store`) to avoid stale loop aliases.
7. Native smoke now works in clang-only environments via an auto-generated `main -> eidos_main` entry shim (no hard `llc` requirement).
8. Non-`VarPattern` generator patterns are explicitly unsupported and report `error[E5101]` during MIR lowering; use a simple variable or wildcard generator binding.

```eidos
doubled :: [x * 2 | x <- [1, 2, 3]];
```

### 3.7 Borrow Signature Inference (CFG Join-Aware)
Example file: `examples/38_borrow_effect_decoupling.eidos`
Current status (2026-03-15):
1. `LoanSignatureInferer` now uses CFG/dataflow program-point inference instead of only linear alias heuristics.
2. Return-borrow constraints are aggregated across all `return` sites, with parameter origins unioned after control-flow joins.
3. This improves `BoundToParams` precision for functions that return different borrowed parameters on different branches.
4. `LoanConstraintVerifier` conflict diagnostics now include an `alias trace id`, and debug state output prints the same ID on traces for direct cross-reference.
5. `BorrowChecker` now uses the same `alias trace id` linkage, so `FormatBorrowAliasStates` and borrow error output can be cross-referenced directly.
6. Borrow/loan conflict diagnostics now also include an `alias state lookup` hint, so you can jump to `*_borrow_aliases` or `*_loan_constraint_states` by searching `id=...`.
7. Borrow targets are now path-sensitive (`BorrowTarget = BaseLocal + PathKey`), so distinct field/constant-index subpaths are separated (for example, borrowing `x._0` no longer blocks writing `x._1`).
8. Unknown indices (for example `x[i]`) remain conservatively conflicting for soundness, while constant indices (for example `x[0]` vs `x[1]`) are tracked precisely.
9. `base` and `base/deref` are now treated as different memory domains, reducing false conflicts between pointer-variable writes and pointee borrows.
10. Borrow phase now applies capability gates: shared `MirLoad` requires `read`; mutable-borrow `MirLoad` and `MirStore` require `write`; `MirMove` requires `move`.
11. Capability-insufficient diagnostics are now separated from borrow-conflict roots with dedicated codes: `E1011` (missing read), `E1012` (missing write), and `E1013` (missing move).
12. Borrow authorization is independent from effect rows. Effect declarations, including effects annotated with `@borrow(...)`, do not grant `read/write/move` permissions.
13. Borrow permissions are derived and enforced by the borrow/ownership pipeline rather than by `need`.

### 3.8 Effect Tags, Authorization, and Polymorphism
Effect declarations are nominal compile-time markers with no operations or runtime representation:

```eidos
Writer :: effect;

write :: String -> Unit need Writer
{
    _ => ()
}
```

1. A caller must declare every effect required by the functions it invokes. Missing authorization is reported as `E3003`.
2. Effects do not own functions. Qualified effect paths such as `Io.Writer` are used only in `need`; ordinary functions are called through their module path, for example `Io.write(text)`.
3. Higher-order APIs use row parameters such as `E: effects`: `apply[A, B, E: effects] :: (A -> B need E) -> A -> B need E`.
4. Fixed and polymorphic rows can be combined, for example `need FFI, E`. Effect variables are generalized and preserved across module summaries and cached compilation state.
5. Effects are erased before runtime. There are no handlers, `with`, `resume`, CPS rewriting, or runtime effect dispatch.
6. Borrow checking is independent from effect authorization; annotating an effect with `@borrow(...)` does not grant read, write, or move permission.

### 3.10 `match when` Branch Guards (Lowered Through MIR)
Example files: `examples/10_match_guard.eidos`, `examples/27_pattern_guard_binding.eidos`  
Update (2026-03-16): `pattern when guard => expr` is now fully wired through lowering, not only type checking:
1. HIR match branches now preserve the `Guard` expression.
2. MIR lowers match branches as ordered executable checks; when `guard=false`, control falls through to later branches.
3. Constructor/tuple sub-pattern checks now emit real match conditions instead of always degrading to default-branch behavior.
4. Identifier-only guard expressions are preserved correctly as well (for example `v when v => ...`).
5. Haskell-style pattern guards are now supported: `when pat <- expr`. Bindings introduced by `pat` are branch-local and do not leak into sibling branches.
6. The same guard form is available in both `match` branches and function-body pattern branches, with dedicated MIR guard flow and fallback semantics.
7. The left-hand side of `when pat <- expr` is now the full pattern grammar, not only constructor/list/`_` forms. Tuple, `or/and/not`, and `view` patterns are accepted there as well.
8. `&&` and `||` now use real short-circuit lowering in guards and normal expressions. If the left side already determines the result, the right side is not evaluated.

### 3.11 ViewPattern (standard syntax `(expr -> pattern)`)
Example file: `examples/11_advanced_patterns.eidos`

```eidos
normalize :: Int -> Int { x => x }

classify :: Int -> Int
{
    x => match x
    {
        !0 => 99,
        1 | 2 => 12,
        3..5 => 35,
        (normalize -> 9) => 90,
        _ => 0
    }
}
```

View patterns are also useful when a fixed argument selects a classifier and
the scrutinee supplies the remaining argument:

```eidos
key_pressed :: Int -> Int -> Bool
{
    expected => key => key == expected
}

classify_key :: Int -> Int
{
    key => match key
    {
        (key_pressed(81) -> true) => 10,
        (key_pressed(82) -> true) => 20,
        _ => 0
    }
}
```

Current status (2026-03-17):
1. The standard `view-pattern` form is `(expr -> pattern)`, meaning “evaluate `expr`, call `expr(scrutinee)`, then match the returned value with `pattern`”.
2. Only the standard form `(expr -> pattern)` is accepted; `view(...)` / `View(...)` are removed.
3. `or-pattern` (for example `1 | 2`), `and-pattern` (for example `(p1) & (p2)`), `not-pattern` (for example `!0`), and `range-pattern` (for example `3..5`) are now wired through parser/hir/mir.
4. `or-pattern` supports shared binding propagation into branch bodies (for example `(1 as n) | (2 as n) => n` and `TokA(n) | TokB(n) => n`).
   When binding names differ, diagnostics now include per-alternative `missing/extra` details for faster pinpointing.
5. `and-pattern` supports merging disjoint bindings across conjuncts (for example `Pair(a, _) & Pair(_, b) => a + b`); duplicate binding names across conjuncts are rejected to avoid ambiguous binding sources.
6. `not-pattern` does not allow introducing bindings; `!(1 as n)` is rejected to avoid ambiguous variable provenance under negation.
7. The same branch reuses the view call result to avoid duplicate calls between the condition-check pass and the binding pass.
8. HIR/MIR include dedicated lowering paths for `ViewPattern`, `OrPattern`, `AndPattern`, `NotPattern`, and `RangePattern`.
9. Rebinding the same variable name in one pattern scope now reports an error (for example `Pair(n, n)`), preventing implicit last-wins ambiguity.
10. Pattern diagnostics around `or/and/not` now include a `context` path (for example `branch#1 > and-pattern`) plus `alternative#/conjunct#` details; constructor/tuple/`as`/`view` recursion adds `positional#N` / `field#name` / `element#N` / `as-inner` / `view-inner` subpaths (`range` also has reserved `start/end` hooks).
11. `or-pattern` / `and-pattern` are now lowered with short-circuit control flow in MIR, so right-hand pattern checks are no longer evaluated unconditionally.
12. String `==/!=` (including string literal pattern comparisons) is now unified through runtime `string_equals`, meaning content equality instead of pointer identity.
13. Constructor-pattern shape validation now runs in strict mode by default: positional arity, named-field legality, and named/positional form mismatches are all checked (including nullary constructors).
14. `range-pattern` now validates boundary ordering during type checking; reversed ranges such as `5..3` or `'z'..'a'` report `E4011` (`start <= end` constraint), and diagnostics label `start` and `end` boundaries separately. Non-`Int/Char` scrutinees report `E4012`.
15. `as-pattern` now inherits and validates the scrutinee type; for example `("a" as s)` on an `Int` scrutinee now reports `E4013` (as-pattern type mismatch), with `as-pattern binding` / `as-pattern inner pattern` labels plus scrutinee-type notes.
16. The left side of `view-pattern` supports general expression shapes (for example `if ... then ... else ...`), but it must be callable as a unary function `expr(scrutinee)` in the current type context. If not, the compiler reports dedicated `E4014` (`view expression invalid`) with scrutinee/view-expression type notes.
17. Partially applied calls are the intended way to express fixed-argument views such as `(key_pressed(81) -> true)`. For FFI/input polling, prefer reading or classifying the event once and then matching the stable value; do not scatter repeated side-effectful polling across many patterns.
18. Name resolution now emits pattern-coverage warnings (non-blocking):
   - `W4200`: non-exhaustive pattern matching (for example only `true`, or missing ADT constructors);
   - `W4201`: unreachable branch after a previous unguarded irrefutable branch.
   - In `Bool` cases, `W4200` now reports missing cases explicitly (for example `missing bool cases: false`).
   - `W4200` now also adds a `Missing-case witnesses: ...` note that shows minimal missing-path examples (currently for Bool/ADT first).
   - `W4200` also adds `Missing-case traces: ...`, using `display [stable-key]` structured witness form (for example `false [bool:false]`, `None [ctor:123]`).
   - It also adds `Missing-case trace groups: ...`, grouping witnesses by space (`bool` / `tuple-bool` / `ctor` / `wildcard`) in a “stable-key first, display optional” form (for example `bool=bool:false`, `ctor=ctor:123 (None)`).
   - A machine-parseable `Missing-case trace kv: ...` note is also emitted, with per-entry format `kind=<...>;key=<...>;display=<...>` and ` || ` as entry separator (escaped when needed).
   - Tuple bool spaces (for example `(Bool, Bool)`) now report missing combinations as `missing tuple bool cases` and include tuple witnesses in notes.
   - Tuple-bool coverage now supports top-level combinator specialization (`or/and/not/as` combinations), so forms like `(true, true) | (true, false)` and `!(false, false)` participate in precise missing/covered-case reasoning.
   - When unresolved guarded (`when`) branches contribute to `W4200`, the warning adds a note that guarded branches are not treated as exhaustive coverage.
   - Coverage analysis performs constant folding for guards: `when true` is treated as unguarded coverage, while `when false` contributes no coverage.
   - `when false` (including foldable boolean combos such as `true && false`) also triggers `W4201` as an unreachable branch warning.
   - For exactly-enumerable finite spaces (currently `Bool`, tuple-bool, and top-level ADT constructor sets), if a branch adds no new coverage (for example duplicate `true`, `(true, false)` after `(true, _)`, or a duplicate `Some(...)` constructor branch), `W4201` is emitted with a `Covered-case witnesses: ...` note to show why it is shadowed by earlier branches.
   - Those `W4201` diagnostics now also include `Covered-case traces: ...`, mapping `witness <- #branch` so you can pinpoint which earlier branch shadows each finite case.
   - When a covered trace is sourced from a guarded-unresolved proven lower-bound, `W4201` now appends `Covered-case lower-bound traces: ...` so that lower-bound provenance is explicit.
   - For the same finite spaces, `W4201` is also emitted when a combinator pattern is provably empty (for example `true & false`, `(true, false) & (false, true)`, or `Some(v) & !Some(_)`) with message `pattern is unsatisfiable in finite coverage space`.
   - For ADT covered-case witnesses, shape hints are now preserved when available (for example `Some(...)`, `Node{...}`), so the shadowed constructor form is easier to identify.
   - Top-level ADT combinator patterns (`or/and/not/as`) now also participate in exact coverage specialization; forms like `!None` and `Some(x) & !None` now contribute to missing-constructor and covered-branch reasoning.
   - For variable-pattern guards with simple boolean equality (`x == true/false`, `x != true/false`), coverage analysis now counts the corresponding bool case.
   - Guard analysis also applies tri-valued short-circuit reasoning: `x || true` is treated as always true, while `x && false` is treated as always false.
   - Variable-predicate guards such as `v when v => ...` are now recognized as provably covering the corresponding bool case.
   - In `match x`, an irrefutable branch guarded by the matched identifier (for example `_ when x`) is also recognized as provably covering its bool case.
   - In `match x`, variable-binding branches guarded by the matched identifier (for example `v when x`) are inferred as the same bool source for coverage reasoning.
   - In `match x`, a single bool-literal branch guarded by the matched identifier (for example `true when x`) is also recognized as provably covering that literal case.
   - Unary guards such as `when !x` are now preserved as `UnaryExpr(Not, x)` in AST, instead of collapsing into unknown guard forms.
   - Pattern-aware guard truth is applied on bool-literal branches: for example `true when !x` is recognized as always false and emits `W4201` (while `W4200` reports missing `true` coverage).
   - In `match x`, if a branch pattern can be exactly enumerated as bool cases (for example `true | false`), coverage reasoning evaluates the guard per case; for example `true | false when x` contributes only the `true` case.
   - For such exactly-enumerated bool patterns, if the guard is false on all matched cases (for example `true | false when !x && x`), the branch is reported as `W4201` (constant-false guard).
   - For exactly-enumerable tuple-bool patterns, guard reasoning now also runs per tuple case with pattern-bound bool variables: for example `(a, b) when a` contributes only cases with first element `true`, and `(a, b) when a && !a` is treated as empty-match and reported as `W4201`.
   - For guarded unresolved tuple-bool branches, if some tuple cases are still provably guard-true while others remain unresolved, that proven lower-bound now also participates in covered diagnostics; unresolved tuple cases still remain conservative.
   - For top-level ADT guarded branches with exactly-enumerable constructor sets, coverage now evaluates guards as `constructor + provable bool bindings`; for example `Some((true as f)) when f` contributes `Some`, while `Some((false as f)) when f` is treated as empty for that constructor and reported as `W4201`.
   - Those ADT guarded branches now also use provable integer-binding constraints (including `as` / `view-inner` / finite sets): for example `Some((normalize -> (1 as n))) when n == 1` contributes `Some` coverage, while `Some((normalize -> ((1 as n) | (2 as n)))) when n == 3` is recognized as empty-match (`W4201`).
   - `Char` literals now flow through the same finite constraint path (as discrete values): for example `Some((normalizeChar -> ('a' as c))) when c != 'a'` is recognized as an empty-match branch and reported as `W4201`.
   - For `as` bindings over negated int subpatterns (for example `((!2) as n)` and `((!2 & !3) as n)`), analysis now models `n` as `other(exclude {...})`: `when n != 2` is provably true, while `when n == 2` is provably false and reported as `W4201`.
   - In ADT matches, outer nested `as` bindings now inherit the same inner `other(exclude {...})` constraints (for example `Some(((normalize -> ((!2 & !3) as n)) as m)) when m == 2`), so provable empty-match branches can be reported as `W4201`.
   - For ADT constructor-level finite coverage, when a covered-source branch is guarded and depends on a refutable `view` payload constraint, covered-branch `W4201` for following constructor/literal branches is now suppressed conservatively to avoid false positives caused by constructor-only coarse domains (`irrefutable view` cases still report precise covered diagnostics).
   - The ADT suppression is now narrower: if that guarded refutable-view source branch contains an `or` arm with an irrefutable non-view fallback (for example `Some(((normalize -> 1) | _)) when true`), covered diagnostics stay precise (`W4201` is preserved) instead of being suppressed.
   - The fallback must be structurally dominant for the whole payload pattern. A nested fallback inside a restrictive `and` branch (for example `Some(((normalize -> 1) & (_ | 1)))`) no longer disables conservative suppression.
   - The fallback must also be target-constructor relevant. In `Some(refutable-view) | None`, the `None` arm only preserves precise covered diagnostics for `None` targets, and no longer disables suppression for `Some` targets.
   - For mixed uncertain-view forms like `Some((<uncertain-view>) | <non-view>)`, if `<non-view>` can be proven to hit the target literal branch (for example target `Some(3)` with a literal `3` non-view arm), the target branch keeps precise `W4201 covered`; if no deterministic hit can be proven (for example target `Some(4)` with only non-view arm `3`), suppression remains conservative.
   - That deterministic-hit recovery now also covers finite int-set targets (for example `Some(3..4)` or `Some(3 | 4)`): `W4201 covered` is restored only when the non-view arm can cover every value in the target finite set; partial hits remain conservatively suppressed.
   - Deterministic target constraints now cover both `Int` and `Bool` fields. Mixed finite targets such as `Pair{flag: true | false, value: 3..4}` can now participate in covered-restoration checks.
   - The same recovery rule now applies to named-field and deep combinator targets as well: for example `Some{value: ...}` field constraints and swapped-order `and` targets such as `Some(_) & Some(3..4)` are evaluated with the same full finite-set coverage requirement before restoring `W4201 covered`.
   - For `or` covering branches, recovery now supports union coverage across alternatives for the same finite target profile: even when no single alternative covers the full target set (for example one covers `3`, another covers `4`), `W4201 covered` is restored if their union covers all target assignments; partial union remains conservative.
   - The same finite-target profile path now also treats `Char` literals as finite discrete domains: targets such as `('a' | 'b')` can restore `W4201 covered` when alternative unions cover all target values, while partial unions remain conservative.
   - For ADT `guarded refutable-view` branches with `as` bindings, covered recovery now also checks guard truth after narrowing bindings by the target finite profile: for example `((normalizeChar -> ('a'..'b')) | 'b') as t when t == 'b'` can now restore `W4201 covered` for a later `tag: 'b'` branch; if guard truth cannot be proven under that profile, behavior stays conservative.
   - The same ADT covered-recovery path now also handles `top-level or/and` combinators and named-field shorthand bindings (`Pair{right}`): branches such as `((CtorA & CtorB) | CtorC) when ...` can now restore downstream `W4201 covered` when guard truth is provable on the target-profile-hitting alternatives, while non-provable alternatives still stay conservative.
   - For finite target profiles (for example `tag: ('a' | 'b')`), guard proving now evaluates per deterministic assignment and then aggregates: disjunctive guards like `t == 'a' || t == 'b'` can now be proven true for covered restoration, while partially-true guards (for example `t == 'a'`) remain conservative and do not trigger false covered diagnostics.
   - In `or` alternatives, branches that are deterministically incompatible with the current target assignment (for example `!((normalizeChar -> 'b'))` against target `'b'`) no longer block guard-binding convergence; deterministic hits from other non-view alternatives can still restore `W4201 covered`, and no-hit cases remain conservative.
   - The same incompatible-alternative skipping rule now holds for Bool targets as well: when the target assignment is `flag=true`, alternatives like `flag=false` (or `!((normalizeBool -> true))` alternatives incompatible with `true`) no longer block covered restoration from deterministic hits; if no deterministic hit exists, behavior remains conservative.
   - For ADT mixed uncertain-view recovery, the covering branch is now validated recursively by combinator structure: `and` paths must satisfy target constraints conjunct-by-conjunct, and `W4201 covered` is not restored when the covering branch still has extra refutable constraints on target-unconstrained fields (unless they are dominated by an irrefutable non-view fallback). This blocks split-conjunct / extra-field false positives.
   - When that conservative suppression happens and `W4200` is emitted, `W4200` now appends `Conservatively suppressed covered warnings: ... (reason: adt-guarded-refutable-view)` so suppressed branch coverage conflicts remain traceable.
   - Suppression traces now also include fine-grained `{reason=...}` tags after branch mapping, such as `adt:refutable-view`, `guard:not-provable`, `adt:view-inner-nonfinite@...`, and `adt:deterministic-assignment-overflow`, so you can quickly tell whether suppression came from unresolved guard evidence, non-finite view-inner constraints, or deterministic assignment enumeration overflow.
   - For list uncertain view-only coverage sources, `W4200` now also emits a suppression-trace note with scope reason `list-guarded-uncertain-view`, plus branch-level tags such as `list:no-deterministic-nonview-hit`.
   - The same list suppression path now covers both `Int/Char token-vector` and `Bool vector` finite cases: bool view-only guarded sources such as `[((normalizeBool -> true)) | ((normalizeBool -> false))]` are also handled conservatively to avoid false `W4201 covered` reports.
   - In bool mixed uncertain-view cases, precise `W4201 covered` is restored only when a same-level non-view alternative deterministically hits the target token (for example `(... | false)` against `[false]`); if the non-view alternative cannot hit the target token (for example `(... | true)` against `[false]`), conservative suppression remains.
   - The same deterministic restoration is now available for plain `Bool` targets as well: for example `((!(normalize -> true)) | true) when true` can now stably cover a following `true` branch and emit `W4201 covered`; when guard truth is unresolved (for example `when probe(x)`), behavior stays conservative and avoids false covered reports.
   - Deep `not` mixed uncertain-view boundaries for plain `Bool` are now aligned too: when the inner side of `not` is provably `no-match` via a non-view subconstraint (for example `!((normalize -> true) & false)`), `W4201 covered` can be restored; when inner truth remains `Unknown` (for example `... & true`), analysis stays conservative and avoids false covered diagnostics.
   - Tuple-bool targets now follow the same mixed uncertain-view deterministic recovery path: for example `(((normalize -> true) | true), false) when true` can cover a following `(true, false)` branch and emit `W4201 covered`; when guard truth is unresolved (for example `when probe(t)`), analysis remains conservative and avoids false covered reports.
   - Deep `not` mixed uncertain-view boundaries for tuple-bool are now aligned as well: when the inner side of `not` is provably `no-match` via a non-view subconstraint (for example `!((normalize -> true) & false)`), `W4201 covered` can be restored; when inner truth remains `Unknown` (for example `... & true`), analysis stays conservative and avoids false covered diagnostics.
   - Suppression reasons now also include explicit domain/path tags: `list:target-domain-int` / `list:target-domain-bool` and `list:view-inner-uncertain@...`, so diagnostics can show both the finite domain that triggered the conservative path and the nested view-inner location that stayed non-deterministic.
   - When list suppression is triggered by deterministic non-view misses, reasons now also include per-case tags such as `list:no-deterministic-nonview-hit-case1`, so you can distinguish “which finite target case failed to find a deterministic non-view hit” without reconstructing it manually.
   - Those list case tags now also include stable case keys (for example `list:no-deterministic-nonview-hit-case1-key:int:3`, `...-key:bool:true`) so multi-case suppression can be mapped to concrete finite target cases quickly.
   - For `Char` target domains, list case keys now prefer char-literal forms (for example `list:no-deterministic-nonview-hit-case1-key:char:'a'`) instead of numeric code points, improving readability during triage.
   - ADT suppression now emits the same kind of tags as well: `adt:target-domain-int` / `adt:target-domain-bool` and `adt:view-inner-uncertain@...`, so conservative fallback diagnostics can distinguish constructor-target domains from uncertain nested view-inner paths.
   - ADT suppression now also emits `adt:no-deterministic-nonview-hit` when deterministic non-view restoration fails (without assignment-overflow), making it easier to separate “solver-cap overflow” from genuine deterministic no-hit fallbacks.
   - In the same path, constructor-granular reason tags are also emitted: `adt:no-deterministic-nonview-hit-ctor<id>` and `adt:no-deterministic-nonview-hit-ctor-name:<CtorName>`, so you can see which constructors failed deterministic non-view matching.
   - The same ADT path is now further refined to constructor-case tags: `adt:no-deterministic-nonview-hit-ctor-name:<CtorName>-caseN` plus `...-caseN-key:...`, so you can pinpoint which finite-profile case under the same constructor failed deterministic non-view matching.
   - For `Char` finite targets, unresolved/suppression notes now also include `list:target-domain-char` / `adt:target-domain-char` (while retaining existing `*-target-domain-int` compatibility tags), so Int/Bool/Char mixed scenarios can be triaged by domain more directly.
   - Even when finite int-domain extraction fails in Char scenarios (for example wider character ranges), unresolved reasons now still retain `list:target-domain-char` / `adt:target-domain-char`, preventing char-domain observability from being dropped by domain-extraction fallback.
   - ADT deterministic non-view recovery now also covers deeper `and` combinators: when a conjunct contains uncertain view parts but still has a same-level non-view subpath that deterministically hits the target value (for example `(((normalize -> 1) | 2) & _)` for target `2`), precise `W4201 covered` is restored; if no non-view subpath can hit the target value, suppression remains conservative.
   - The same deterministic non-view path now also covers deep `not` mixed uncertain-view cases: when the inner side of `not` can be proven `no-match` by non-view constraints (for example `!((normalize -> (2..3)) & 2)` against target `3`), precise `W4201 covered` is restored; when inner uncertainty cannot be eliminated (same pattern against target `2`), behavior remains conservative.
   - Deterministic guarded-binding narrowing for `as` now also handles the subset where `not` wraps a view-containing inner pattern that is still provably `no-match` on the current token: examples like `((!((normalizeBool -> true) & false)) as f) when f` or `((!((normalizeChar -> 'a') & 'c')) as t) when t == 'b'` can now restore downstream `W4201 covered`; when the inner remains `Unknown` (for example `... & true` / `... & 'b'`), recovery stays conservative.
   - List guarded + refutable-view branches now also have a deterministic supplemental covered path: even when the main coverage pass misses `W4201` due split-domain lower-bound gaps, the resolver emits covered diagnostics if deterministic non-view matching proves target finite-case hits and the guard is provably true.
   - List suppression entry for `not(refutable-view)` is now aligned with deterministic re-checking: if target finite cases cannot be hit through deterministic non-view matching, covered warnings stay conservatively suppressed, preventing false `W4201 covered` in uncertain-inner shapes such as `!((normalizeChar -> 'a') & 'b')`.
   - The same supplemental recovery now supports **multi-branch union coverage** in both list and ADT spaces: if different guarded refutable-view branches deterministically cover different target finite cases, a later target branch can still be reported as `W4201 covered` once the union covers all target assignments.
   - For those union-recovered branches, `W4201` now carries `Covered-case witnesses` / `Covered-case traces` from the supplemental path too, so multi-source coverage (for example `... <- #2` and `... <- #3`) is directly visible in diagnostics.
   - The same union/partial-union boundaries are now symmetry-locked for `Char` finite domains in both list and ADT spaces: full union restores covered diagnostics with multi-source traces, while partial union remains conservative (no false covered).
   - `W4200` now also emits a machine-parseable note: `Suppressed-covered trace kv: kind=...;branch=...;covering=...;reason=...` (multiple entries are separated by ` || ` and field delimiters are escaped with `\`; `reason` still uses `,` between tags, and commas inside a tag are escaped as `\,`) so IDE/tooling layers can parse suppression traces structurally.
   - If guard evaluation over those finite cases still contains unresolved parts (for example external predicate calls), analysis conservatively degrades to non-exact coverage instead of emitting a false `W4201` unsatisfiable branch; when `W4200` is emitted, the guarded-note explains this conservative path.
   - That conservative-path note now also includes concrete guarded branch indices (`#N`) so the unresolved branches can be located directly.
   - `W4200` now also appends `Unresolved-guard branch hints: ...`, including `#N@line:col` plus inferred lower-bound finite cases (or `?` when no lower-bound case is provable).
   - For ADT targets, when the pattern still determines constructor candidates, those lower-bound hints now prefer constructor names (for example `[Some]`) instead of falling back to `?`.
   - For `Bool` / tuple-bool targets, when the pattern itself determines finite cases (even if the guard is still unresolved), lower-bound hints now prefer those cases (for example `[true]`, `[(true, false)]`) instead of always falling back to `?`.
   - For list targets, lower-bound hints now prioritize branch-proven list cases; when only length-shape hints are available (for example `[_]`), the resolver now tries deterministic non-view refinement to recover provable bool vectors (for example `[[true]]`) and reduce shape-only degradation.
   - For `Char` list finite cases, unresolved lower-bound hints now prefer character-literal rendering (for example `['a']`, `['b']`) instead of numeric code-point forms (for example `[97]`, `[98]`), making branch intent easier to read.
   - For guarded unresolved ADT branches, if some constructors are still provably guard-true while others remain unresolved, that proven lower-bound now also participates in covered diagnostics (for example later same-constructor branches can still be reported as covered), while unresolved parts remain conservative.
   - For list/ADT `view` conservative fallbacks, `reason=` now carries pinpoint subpaths (for example `list:view-inner-nonfinite@pattern/list-element#1/view-inner` and `adt:view-inner-nonfinite@pattern/positional#1/view-inner`) so you can locate which nested subpattern blocked finite solving.
   - List unresolved hints now also support bool-domain fallback extraction: when list case hints cannot directly recover bool vectors (for example mixed alternatives degrading to `[_]`), analysis still derives bool-domain candidates from the pattern and emits `list:target-domain-bool` plus `list:view-inner-uncertain@...` path tags.
   - For ADT deep `not` + uncertain-view unresolved branches, `reason=` now also reliably emits `adt:view-inner-uncertain@...` path tags (for example `adt:view-inner-uncertain@pattern/positional#1/not-inner/conjunct#1/view-inner`), and includes `adt:target-domain-int` or `adt:target-domain-bool` when derivable; reason ordering still keeps `guard:not-provable` first to preserve existing diagnostic/test/tooling contracts.
   - Tuple-bool unresolved branches now expose the same observability shape: `reason=` includes `tuple-bool:target-domain-bool` and `tuple-bool:view-inner-uncertain@...` (for example `tuple-bool:view-inner-uncertain@pattern/tuple#1/not-inner/conjunct#1/view-inner`) to pinpoint deep `not` uncertain-view fallback paths inside tuple elements.
   - Bool unresolved branches are now aligned as well: `reason=` can include `bool:target-domain-bool` plus `bool:view-inner-nonfinite@...` / `bool:view-inner-uncertain@...` path tags (for example `bool:view-inner-uncertain@pattern/not-inner/conjunct#1/view-inner`) to pinpoint conservative fallback sources over bool-domain view-inner constraints.
   - There are now dedicated regressions that lock cases where a single `W4200` must carry both unresolved-guard hints and suppression traces (list/ADT), preventing either explain-note family from silently regressing.

### 3.12 List/Rest Pattern (list matching)
Example files: `examples/16_list_rest_pattern.eidos`, `examples/17_list_guarded_coverage.eidos`, `examples/18_view_guard_unsat.eidos`, `examples/19_view_guard_finite_set.eidos`, `examples/20_guard_algebra_unsat.eidos`, `examples/23_view_guard_other_and_not_unsat.eidos`, `examples/24_view_guard_nested_as_other_unsat.eidos`, `examples/25_view_guard_mixed_view_nonview_conservative.eidos`  
Supported forms (wired through Parser/Ast/HIR/NameResolver/Types/MIR):

```text
[]
[p1, p2]
[p1, ..rest]
[..rest]
[..]
```

```eidos
head_or_zero :: Int -> Int
{
    _ => match [1, 2, 3]
    {
        [head, ..tail] => head,
        [] => 0,
        [..] => 0
    }
}
```

Semantics update (2026-03-19):
1. MIR lowering performs list-length guards before element loads to avoid out-of-bounds reads.
2. `..rest` is lowered as a suffix-list binding and materialized via runtime list APIs.
3. `[..]` is a rest-only marker (matches the remaining suffix without introducing a binding).
4. Pattern coverage analysis now supports list-shape coverage plus top-level combinators: `[]` / fixed-prefix / `[..]` and `or/and/not/as` combinations can emit `W4200/W4201` (`missing list cases`, unsatisfiable branches, and covered-branch traces).
5. For provable Bool-element list patterns, coverage is further refined to element-level witnesses (for example `[true]` / `[false]`); otherwise analysis conservatively falls back to length-shape coverage.
6. For guarded list branches, when guards can be proven per finite case (for example `[x] when x` and `[x] when !x`), those branches now contribute exact coverage; if a guarded branch is empty over the finite case set, it is reported as `W4201` (`pattern is unsatisfiable in finite coverage space`).
7. Int-element list coverage is now extended from “length = 1 only” to fixed multi-element lengths (current cap: 2): patterns such as `[1, 2]`, `[1..2, 3..4]`, and `[1 as a, 2 as b] when a == 1 && b == 2` now participate in element-level finite-case usefulness reasoning.
8. `Int + rest` combinations now also participate in precise reasoning: `[1, ..]` no longer falsely swallows `[2, 1]`, and `[1 as x, ..] when x != 1` is now recognized as an empty-match branch (`W4201`).
9. For list element `view` patterns, finite-case reasoning now includes the irrefutable-inner subset (for example `[(normalize -> _)]`): these branches can contribute exact list coverage and mark later literal branches as covered (`W4201`). Refutable view-inner patterns still use a conservative path to avoid false covered-branch reports.
10. For view-inner patterns, list finite-case reasoning now also accepts a provable algebraic subset: when the inner pattern is provably full-match or empty-match over the finite domain (for example `(1 | !1)` and `(1 & !1)`), the branch now participates in exact coverage/unreachable diagnostics (`W4201`).
11. Guarded list branches now also propagate provable bindings from `view-inner` into guard reasoning: for example `[(normalize -> (true as b))] when !b` and `[(normalize -> (1 as x))] when x != 1` are recognized as empty-match branches and reported as `W4201`; when `view-inner` constraints are not provable, analysis still degrades conservatively.
12. Guard evaluation now also tracks finite int-set constraints from view-inner `as` bindings (for example `((1 | 2) as x)` and `((1 as x) | (2 as x))`): contradictions such as `when x == 3` or `when x < 0` are now recognized as empty-match branches and reported as `W4201`.
13. Guard algebra now includes two additional conservative derivation rules:
   - finite int-set arithmetic comparisons (for example `x + 1 == 4` is provably false when `x ∈ {1,2}`);
   - boolean complement identities (`b && !b` => always false, `b || !b` => always true) even when `b` is not pattern-bound.
14. Finite set extraction for `view-inner` bindings now also supports mixed `and/not` combinators: for example `((1 | 2) & !2)` is narrowed to `{1}`, so `[(normalize -> ((((1 | 2) & !2) as x)))] when x != 1` is recognized as an empty-match branch and reported as `W4201`.
15. List-element patterns now also support `Always/Never` reasoning for nested `view` inside combinators: for example in `[((normalize -> (1 | !1)) | 2)]`, the nested view subpattern is recognized as always-match and participates in coverage/unreachable analysis; non-provable nested-view cases still degrade conservatively to avoid false positives.
16. Parser diagnostics now unify `..` boundary errors: `..` is only valid inside list patterns, and `..rest` must be the trailing item. Forms like `(a, ..rest)`, `Some(..rest)`, or `[a, ..rest, b]` now emit `E4000` with fix-oriented guidance (`[a, ..rest]`, `[..rest]`, `[..]`).
17. For same-level combinators that mix a refutable uncertain `view` subpattern with non-view alternatives (for example `((normalize -> (1 as x)) | 3..4)`), finite-case coverage remains conservative to avoid false covered-branch diagnostics (`W4201` false positives).
18. Int-domain token matching for list/view finite reasoning now uses three-valued evaluation (`match` / `no-match` / `unknown`) for `or/and/not`: for `or`, analysis now recovers precise covered diagnostics not only with an irrefutable non-view fallback (for example `((normalize -> (1..2)) | _)`), but also when a non-view alternative is deterministically matched for the current token (for example `(((normalize -> (1..2)) | 2) as x) when x == 2`); unresolved mixed paths still degrade conservatively.
19. Mixed uncertain-view detection now also recognizes nested wrappers (for example `!((normalize -> p))`, `as((normalize -> p))`, and nested `or/and` forms): if no deterministic non-view arm can prove the current token match, the branch remains conservative and no false covered warning is emitted.
20. List finite-split discrete literal domains now also cover `Char`: branches like `['a']` and `['b']` are no longer over-approximated by shape-only length coverage; char ranges such as `['a'..'c']` can now stably cover a later `['b']` branch and produce the expected `W4201 covered` diagnostic.
21. For guarded list coverage sources made only of uncertain-view alternatives (no provable deterministic non-view hit), covered-style `W4201` is now conservatively suppressed, avoiding false reports where a branch like `[((normalize -> (1..2)) | (normalize -> 3))]` incorrectly shadows a later `[3]`. If a deterministic non-view hit can be proven, or the `view-inner` is provably always/never over the finite domain, precise covered diagnostics are still preserved (see `examples/26_view_guard_uncertain_only_conservative.eidos`).
22. Deep `not` mixed uncertain-view list branches (for example `[!((normalize -> (2..3)) & 2)]`) now follow the same boundary as ADT: restore `W4201 covered` when the target token is provably excluded by inner non-view constraints (for example target `[3]`), and stay conservative when inner uncertainty is still unresolved (for example target `[2]`).

### 3.13 Precompiled Standard Library (Officially Available)
Example files: `examples/29_precompiled_stdlib.eidos`, `examples/42_stdlib_safe_and_traits.eidos`

The compiler now ships precompiled stdlib modules as embedded resources, and the CLI presents them by capability instead of as one flat list:

| Capability | Modules | What this group is for | Representative APIs |
| --- | --- | --- | --- |
| Functional | `Std.Fn`, `Std.Prelude`, `Std.Functor`, `Std.Applicative`, `Std.Foldable`, `Std.Traversable`, `Std.Monad`, `Std.Option`, `Std.Result`, `Std.Ordering`, `Std.Trait`, `Std.TraitInvoke` | `Std.Fn` is the home for function tools, `Std.Prelude` keeps common Text safe helpers plus basic File text-I/O fallback helpers, and the rest covers optional/error pipelines plus reusable `Functor`/`Applicative`/`Foldable`/`Traversable`/`Monad` abstractions; `Option/Result/Seq` now expose the full `fmap/pure/apply/traverse/bind` surface and share `fold_left/fold_right`, while `Option/Result/Ordering` also expose the basic value-type `Eq` / `Ord` / `Show` surface; `T?` is available as sugar for `Std.Option.Option[T]`, `??` is available as an `Option.unwrap_or` fallback operator, and `let?` is available for early-return `Option/Result` binding; tutorial style now prefers `|>`, `>>>`, `<<<`, `<$>`, `<*>`, `>>=`, `<>`, `??`, `let?`, and chained calls | `value |> f |> g`, `f >>> g`, `inc <$> Some(1)`, `Some(f) <*> Some(x)`, `Some(x) >>= f`, `maybe_count ?? 0`, `let? value = maybe_value`, `xs.map(f).filter(p)`, `Fn.compose`, `Option.traverse`, `Seq.traverse`, `Result.and_then` |
| Math | `Std.Math`, `Std.FloatMath`, `Std.GameMath` | Scalar math, angle/interpolation helpers, and game-oriented `IVec2`/`Vec2`/`IRect`/`Rect` geometry plus grid helpers | `Math.wrap`, `FloatMath.smoothstep`, `FloatMath.angle_delta_degrees`, `GameMath.ivec2`, `GameMath.grid_cell_rect` |
| Containers | `Std.Seq`, `Std.SeqBuilder` | Read-side sequence querying, mapping, filtering, folding, concatenation, and zipping plus explicit builder-side push/set/swap/freeze workflows | `Seq.head`, `Seq.tail`, `Seq.find`, `Seq.map`, `Seq.filter`, `Seq.fold_left`, `SeqBuilder.push`, `SeqBuilder.freeze` |
| File IO | `Std.File` | File existence checks, whole-file text reads/writes, fallback reads, and last IO status | `File.exists`, `File.read_text_or`, `File.last_error`, `File.write_text` |
| Console IO | `Std.Console` | Text/integer/float/char/bool output, prefix+value line output, plus single-line input | `Console.write_line`, `Console.write_int`, `Console.write_text_int_line`, `Console.read_line_text` |
| Network | `Std.Network` | Minimal text-oriented HTTP GET access | `Network.http_get_text_or_empty` |
| Serialization | `Std.Binary`, `Std.Json` | Basic binary encoding/decoding plus JSON text construction for strings, arrays, and objects | `Binary.encode_u32_le`, `Binary.bytes_to_string`, `Json.array`, `Json.object` |
| Other foundation | `Std.Text` | String length, emptiness, primitive-to-text construction, trimming, slicing, safe char/code-point reads, and substring search; safe lookup helpers now come in both `*_opt` and `*_or` forms | `Text.len`, `Text.from_int`, `Text.from_bool`, `Text.trim`, `Text.slice`, `Text.char_code_at_opt`, `Text.char_code_at_or`, `Text.char_at_opt`, `Text.index_of_or` |

If you want the live module/export list, use the CLI directly:

You can inspect available modules and exported functions via CLI:

```powershell
dotnet run --project src/Eidosc/Eidosc.Cli -- info --stdlib
```

Validation scope as of 2026-04-11:
1. `examples/29_precompiled_stdlib.eidos` still covers the core functional modules and passes LLVM-phase validation; the example now explicitly exercises composed `Option/Result/Seq/Text/Ordering` flows, Prelude wildcard Text/File helpers, Text trimming, safe fallback helpers, and a subset of trait-driven APIs.
2. `examples/42_stdlib_safe_and_traits.eidos` now provides a shorter combined entry point that simultaneously exercises `Option/Result/Seq` `Functor/Applicative/Foldable/Traversable/Monad` usage, `Option/Result/Ordering` `Eq/Ord/Show`, and `Text` `*_opt` / `*_or` helpers; the current baseline also includes `Result.traverse` inferring its applicative back to `ResultWith[E]`.
3. `Std.Option`, `Std.Result`, `Std.Ordering`, `Std.Seq`, and `Std.Text` all have dedicated import fixtures plus LLVM integration assertions; `Option/Result/Seq` now cover the full `fmap/pure/apply/bind` path plus `fold_left/fold_right`; `let?` `Option/Result` binding sugar is also covered by `examples/63_let_question_option_result.eidos` and `projects/test/src/stdlib/std_let_question_binding.eidos`.
4. `Std.Math`, `Std.FloatMath`, `Std.GameMath`, `Std.Console`, `Std.File`, `Std.Network`, `Std.Binary`, and `Std.Json` have dedicated import fixtures plus targeted tests; `Std.Prelude.*` is additionally covered by `projects/test/src/stdlib/std_prelude_core_import.eidos` for direct Text/File core-helper imports; `Std.Functor`, `Std.Applicative`, `Std.Foldable`, `Std.Traversable`, `Std.Monad`, and `Std.TraitInvoke` are currently covered through export-table checks and CLI grouping visibility.
5. `Std.Network` is still intentionally minimal: the current implementation is a best-effort text fetch and returns an empty string on failure instead of modeling a full HTTP client surface.
6. `Std.Json` is currently encode-oriented rather than a full JSON parser, and `Std.Binary` currently focuses on foundational bool/int/string helpers.

## 4. BNF Entry
See the compact grammar guide in [`BNF.en.md`](BNF.en.md).
Chinese version: [`BNF.zh-CN.md`](BNF.zh-CN.md).

## 5. Verified Baseline Examples
The following behaviors are validated by `verify-examples.ps1`:

1. Nested call type-checks with a curried signature (`add(1)(2)`), from Parser through Types.  
   Example: `examples/06_nested_call_parser_only.eidos`.
2. A block tail expression is correctly treated as the block value in return-type unification.  
   Example: `examples/07_block_result_known_issue.eidos`.
3. List comprehension preserves full structure at HIR; MIR lowers both static and dynamic sources and uses runtime list APIs for core read/write semantics; LLVM now includes `array_get/array_set` mapping plus slot-backed backedge local updates; native smoke runs in clang-only environments; non-`VarPattern` generator patterns now report `E5101`.  
   Example: `examples/08_list_comprehension.eidos`.
4. Marker effect calls pass `types` and `llvm`; authorization is checked statically and erased before MIR runtime semantics.
   Example: `examples/09_effect_tag_call.eidos`.
5. `(expr -> pattern)` passes `hir` and `mir`.  
   Example: `examples/11_advanced_patterns.eidos`.
6. Pattern-entry forms `if let` and `while let` now flow through HIR/MIR: `if let` lowers to two-branch `match`, and `while let` lowers to `loop + match + break`.  
   Examples: `examples/13_if_let_pattern.eidos`, `examples/14_while_let_pattern.eidos`.
7. `let?` binding sugar is wired through Parser/NameResolver/Types and eliminated during HIR construction into ordinary `match` + `return`; HIR/MIR/LLVM have no dedicated `let?` nodes.
   Examples: `examples/63_let_question_option_result.eidos`, `projects/test/src/stdlib/std_let_question_binding.eidos`.
8. List/rest pattern forms (`[]` / `[head, ..tail]` / `[..]`) now flow through HIR/MIR with guarded length checks and tail materialization semantics; guarded list branches also participate in finite-case coverage reasoning.
   Examples: `examples/16_list_rest_pattern.eidos`, `examples/17_list_guarded_coverage.eidos`, `examples/18_view_guard_unsat.eidos`, `examples/19_view_guard_finite_set.eidos`, `examples/20_guard_algebra_unsat.eidos`, `examples/21_view_guard_mixed_and_not_unsat.eidos`, `examples/22_nested_view_combinator_coverage.eidos`, `examples/23_view_guard_other_and_not_unsat.eidos`, `examples/24_view_guard_nested_as_other_unsat.eidos`, `examples/25_view_guard_mixed_view_nonview_conservative.eidos`.
9. Pattern-guard binding (`when pat <- expr`) is now wired through Parser/NameResolver/Types/HIR/MIR, including the preferred function-body pattern-branch style.
   Example: `examples/27_pattern_guard_binding.eidos`.
10. Early `return` now keeps payload type semantics across Types/HIR/MIR/LLVM: return payloads are type-checked against function result type, lowered as dedicated HIR return nodes, and compiled through MIR/LLVM return terminators without fallback diagnostics.
   Example: `examples/28_early_return.eidos`.
11. The embedded precompiled stdlib is now organized by capability: the core functional modules are LLVM-validated through the wider `examples/29_precompiled_stdlib.eidos` example and the shorter `examples/42_stdlib_safe_and_traits.eidos` showcase; the math/game-math/IO/network/serialization modules have dedicated import fixtures plus targeted tests; and the capability grouping itself is checked through CLI and export-table tests.
   Examples: `examples/29_precompiled_stdlib.eidos`, `examples/42_stdlib_safe_and_traits.eidos`, `examples/55_functional_infix_chain_style.eidos`, `examples/62_option_suffix_coalesce.eidos`, `examples/63_let_question_option_result.eidos`, `examples/65_game_math_vectors.eidos`, `projects/test/src/stdlib/std_*_import.eidos`.
12. Returned-borrow origin rules are now part of the tutorial baseline: a parameter reference may flow through a local alias and still be returned, but the returned origin must remain traceable to an input parameter.
   Example: `examples/54_return_borrow_param_alias.eidos`.

## 6. Practical Tips
1. Prefer simple expressions and split complex flows into multiple `let` bindings.
2. New examples should prefer infix and chained functional style: use `|>` for linear value flow, `<$>` / `<*>` / `>>=` for trait-driven composition, and `.map(...).filter(...).fold_left(...)` for container pipelines. Every segment's function signature must still be valid on its own before composing the chain.
3. When syntax changes, update together:
   `docs/tutorial/*.md`, `docs/tutorial/examples/*`, and `tools/editor/*`.
4. For type-inference debugging, use `debug --debug-level diagnostic` and inspect `substitution`; it now includes `raw/resolved` bindings, binding chains, and AST usage context.

## 7. Current MIR Status and Limits
- Effect rows are compile-time metadata and do not introduce handler, continuation, CPS, or runtime-dispatch MIR nodes.
- `MirBuilder` now converges missing call-result types early (function-signature first, effect fallback to current function return type) to reduce backend-only coercion reliance.
- ADT constructor calls now have a dedicated MIR target resolution path: `CallConvention.Constructor` is lowered directly to `MirFunctionRef` (for example `call @A(1)`), instead of degrading to an uninitialized local callee.
- `list comprehension` supports MIR CFG lowering for both static and dynamic sources; non-`VarPattern` generator patterns now report `E5101` instead of falling back as a warning.
