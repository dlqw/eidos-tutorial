# Eidos Core BNF (English Summary)

This file is a tutorial-oriented summary for quick lookup.
The canonical grammar source is `src/Eidosc/Eidosc/Grammar/GrammarDefine.cs`.

## 1. Program and Declarations
```bnf
program      ::= declaration*
module_def   ::= attribute* module_path "::" "module" "{" declaration* "}"
               | attribute* module_path "::" "module" ";"

declaration  ::= let_decl
               | export_decl
               | func_def
               | effect_def
               | adt_def
               | trait_def
               | instance_def
               | name_first_decl
               | import_stmt

name_first_decl ::= attribute* (lower_identifier | upper_identifier) type_params? "::"
                    ("module" | "type" | "trait" | "effect" | "instance" | "import" import_path | "comptime" expr | type) ...

export_decl  ::= "export" (let_decl
               | func_def
               | effect_def
               | adt_def
               | trait_def
               | instance_def
               | name_first_decl
               | import_stmt)

namespace_path ::= module_path | package_path
package_path ::= package_alias "." module_path
package_alias ::= lower_identifier | upper_identifier
module_path  ::= upper_identifier ("." upper_identifier)*
import_path  ::= namespace_path
item_path    ::= namespace_path "." (lower_identifier | upper_identifier)

import_stmt  ::= "import" import_path
               | "import" import_path "." "*"
               | "import" import_path "." "{" import_item ("," import_item)* "}"
name_first_import_binding ::= attribute* "export"? upper_identifier "::" "import" import_path ";"
import_item  ::= (lower_identifier | upper_identifier)
                 ("as" (lower_identifier | upper_identifier))?
operator_identifier ::= one or more symbolic operator characters, excluding reserved tokens
```
Note: once a module contains any `export` declaration, external visibility switches to explicit-export mode. `export import ...` re-exports the imported bindings as real public module members.
Note: module import aliases are compile-time module names and must start with an upper-case letter. Selective import member aliases follow the imported symbol tier: runtime value aliases start lower-case, while aliases for types, constructors, traits, effects, and other compile-time symbols start upper-case.
Note: modules from dependency packages use `packageAlias.Module.Path`, where `packageAlias` is the dependency key in the current project's `eidos.toml`. The standard library has the built-in package alias `Std`, so the standard sequence module is `Std.Seq`: `Std` is the package name and `Seq` is the module name. The standard sequence type is `Std.Seq.Seq[Int]`. Final symbols such as types, functions, traits, effects, and constructors can use `packageAlias.Module.Path.Item`; current-package modules and imported module aliases can still use `Module.Path.Item` or alias-qualified forms. When a module path is written without a package, the compiler collects candidates from the current package, `Std`, and dependency packages. A unique candidate is accepted; multiple candidates are an error and require a package-qualified path. Explicit package-qualified paths never fall back to another package.
Note: Eidos 0.6 has one Namespace surface. Packages, modules, types, traits, effects, constructors, compile-time values, and their members are selected with `.`, for example `Std.Option.Option[Int]`, `Meta.typeInfo(User)`, and `Parser :: import Compiler.Parser;`. `::` is reserved exclusively for declaration binding. Explicit migration from Eidos 0.5 rewrites removed `::` qualifiers and slash module paths to dots.
Note: `Std.Prelude.*` now uses that re-export behavior to expose common Text safe helpers such as `trim`, `char_code_at_or`, and `last_index_of_or`, plus basic File text-I/O fallback helpers such as `read_text_or` and `write_text_result`; full modules remain available through explicit `Std.Text` and `Std.File` imports.
Note: an unaliased module import introduces both the module alias and bare names for that module's public values / constructors. For example, after `import Std.Seq`, both `Seq.append(xs)(ys)` and `append(xs)(ys)` are valid. A name-first module-alias binding `M :: import A;` introduces only the alias, and effects do not become bare-name-visible through module imports. The legacy `import A as M` keyword form is accepted only in `legacy` syntax mode; in `0.4.0-alpha.1` it is rejected with a migration diagnostic.
Note: symbolic custom operators use characters from `! $ % & * + / < = > ? ^ | - ~ :`; reserved built-in tokens such as `->`, `=>`, `:`, `::`, `=`, `<-`, `<$>`, `<*>`, `|>`, `?`, and `??` keep their built-in meaning.
Note: user declarations and bindings cannot use names beginning with `__` or names containing `__spec_`; that internal namespace is reserved for compiler-generated artifacts and reports `E3055`.
Note: Eidos uses a naming-tier split: runtime values use lower-case-leading identifiers, and compile-time values use upper-case-leading identifiers. Types are first-class compile-time values, so types, traits, effects, constructors, module path segments, and generic parameters that denote types belong to the upper-case namespace. A constructor call produces a runtime value, but the constructor symbol itself is still a compile-time value.
Note: name-first declarations cover `Name :: module`, `name :: Type -> Type { ... }`, `name :: Type -> Type;`, `name :: comptime Type -> Type { ... }`, `name :: value;`, `name :: Type = value;`, `Name :: comptime expr;`, `Name :: type`, `Name :: trait`, `Name :: effect;`, `Name :: instance Trait[...] { ... }`, and `Alias :: import Module.Path;`. Keyword-first forms are migration input, not Eidos 0.6 source syntax.

## 2. Bindings and Statements
```bnf
let_decl     ::= attribute* "let" "mut"? pattern (":" type)? "=" expr ";"
name_first_local_binding ::= "comptime"? "mut"? pattern (":" type)? ":=" expr ";"
let_question_decl ::= "let" "?" pattern "=" expr ";"
assignment   ::= lower_identifier ":=" expr ";"
expr_stmt    ::= expr ";"

stmt_non_expr         ::= let_decl | name_first_local_binding | let_question_decl | assignment
block_expr            ::= "{" block_body
block_body            ::= "}"
                        | stmt_non_expr block_body
                        | expr block_expr_after_expr
block_expr_after_expr ::= ";" block_body | "}"
```
Note: a block has an explicit tail expression only when its last item is an expression without a trailing semicolon; that expression is the block value. A block with no explicit tail expression evaluates to `Unit`, including an empty block, a block containing only declarations or assignments, or a block whose final expression has a trailing semicolon.

Note: in `0.5.0-alpha.1`, `comptime name := expr;` marks a local compile-time binding, and `name :: comptime A -> B { ... }` declares a comptime-only function. `comptime mut`, assignment to a comptime binding, runtime references to comptime values, and runtime calls to comptime-only functions are rejected. The typed evaluator accepts scalar and String values, tuples/lists without spread, ordinary ADT constructors and fields, recursive comptime-only calls, pure blocks with local bindings, pure unary/binary operators, compile-time `if`, and `match` with literal/range/wildcard/or/tuple/list/constructor patterns and pure guards. Named-argument calls, partial comptime application, borrow bindings, effects, FFI, `with`, filesystem/environment/process access, and runtime resource identity are outside pure CTFE. Reified comptime values enter typed HIR without runtime binding getters or locals.

Note: `let?` is valid only inside a function or lambda block; it is not a valid top-level declaration, though the parser may accept that shape to produce a targeted diagnostic. `let? pattern = expr;` requires an `Option[A]` or `Result[A, E]` right-hand side, and `pattern` currently must be irrefutable. The `Option` form requires an enclosing `Option[R]` return context and short-circuits with `None()`. The `Result` form requires an enclosing `Result[R, E]` return context, or a canonical equivalent alias, and short-circuits with the original `Err(e)`. `let?` is front-end sugar only: it exists through Parser/AST/NameResolver/Types and is eliminated by HIR construction into ordinary `match` + `return`; HIR/MIR/LLVM do not have dedicated `let?` nodes.

## 3. Functions and Signatures
```bnf
func_name    ::= lower_identifier | "(" operator_identifier ")"
func_def     ::= attribute* "func" func_name type_params? ":" signature need_clause? func_body
name_first_func_def  ::= attribute* func_name type_params? "::" signature need_clause? generic_where? func_body
name_first_func_decl ::= attribute* func_name type_params? "::" signature need_clause? generic_where? ";"
func_body    ::= "{" pattern_branch ("," pattern_branch)* "}"
pattern_branch ::= pattern ("when" (pattern "<-" expr | expr))? "=>" expr
                 | pattern "=>" curried_branch_rhs
curried_branch_rhs ::= expr
                     | "_"
                     | pattern ("when" (pattern "<-" expr | expr))? "=>" curried_branch_rhs

signature    ::= type ("->" type)*
need_clause  ::= "need" effect_path ("," effect_path)*

effect_path  ::= upper_identifier | namespace_path "." upper_identifier
trait_path   ::= upper_identifier | namespace_path "." upper_identifier
trait_ref    ::= trait_path generic_args?

type_params  ::= "[" type_param ("," type_param)* "]"
type_param   ::= upper_identifier (":" ("comptime"? "Type" | kind | "effects"))? trait_constraints?
               | "comptime" upper_identifier ":" type
generic_args ::= "[" generic_arg ("," generic_arg)* "]"
generic_arg  ::= type | expr
trait_constraints ::= ":" trait_ref ("+" trait_ref)*
generic_where ::= "where" where_constraint ("," where_constraint)*
where_constraint ::= upper_identifier ":" (kind | trait_ref ("+" trait_ref)*)
kind         ::= kind_atom ("->" kind_atom)*
kind_atom    ::= "kind" digits
               | "(" kind ")"
type         ::= function_type_head ("->" type)?
function_type_head ::= tuple_type | postfix_type
postfix_type ::= primary_type ("?" | "??" | "." upper_identifier)*
```
Note: in `0.4.0-alpha.1`, a top-level name-first function declaration such as `@ffi("malloc") malloc :: Int -> RawPtr;` declares a function without a body and is used for external/FFI declarations. Name-first entry functions do not receive the legacy implicit root `FFI`/`IO` capability; calls to FFI/native declarations from `main` or another entry function must be covered by an explicit `need FFI` or equivalent effect tag.
Note: postfix `?` on a type is sugar for `Std.Option.Option[...]`; for example, `Int?` is equivalent to `Std.Option.Option[Int]`. Repeated suffixes nest, so `Int??` is equivalent to `Option[Option[Int]]`.
Note: in 0.4.0-alpha.1 mode, type postfix projection such as `Iterator[I].Item` names an associated type declared by the target trait and reduced through a concrete named instance when available.

## 4. Trait, ADT, Type Alias, Attribute
```bnf
effect_def   ::= attribute* upper_identifier "::" "effect" ";"
trait_def    ::= attribute* upper_identifier type_params? "::" "trait"
                         trait_constraints? generic_where? trait_body generic_where? ";"?
trait_body   ::= "{" trait_member* "}"
trait_member ::= func_decl
               | name_first_func_member
               | associated_type_decl
               | associated_const_decl
func_decl    ::= "func" func_name type_params? generic_where? ":" signature need_clause? generic_where?
name_first_func_member ::= func_name type_params? "::" signature need_clause? generic_where? func_body?
associated_type_decl ::= upper_identifier type_params? "::" "type" ("=" type)?
associated_const_decl ::= upper_identifier "::" type ("=" expr)?
instance_def ::= attribute* upper_identifier "::" "instance" trait_ref "{" instance_member* "}"
               | attribute* upper_identifier "::" "instance" trait_ref "for" type (";" | constructor_bridge_body)
constructor_bridge_body ::= "{" constructor_bridge_fact (("|" | "," | ";") constructor_bridge_fact)* "}"
constructor_bridge_fact ::= upper_identifier "=>" "{" constructor_bridge_entry ("," constructor_bridge_entry)* "}"
constructor_bridge_entry ::= lower_identifier "=" expr
instance_member ::= name_first_func_member | associated_type_decl | associated_const_decl

adt_def      ::= attribute* upper_identifier type_params? "::" "type" generic_where? "{" adt_body "}"
               | type_alias
type_alias   ::= attribute* upper_identifier type_params? "::" "type" generic_where? "=" type ";"
adt_body     ::= constructor ("," constructor)* ","?
               | field ("," field)* ","?
constructor  ::= upper_identifier type_params? ctor_args? ("->" type)?
ctor_args    ::= "(" type ("," type)* ")"
               | "{" field ("," field)* "}"

attribute    ::= "@" lower_identifier ("(" arg_list? ")")?
```
Note: `by` is a proof-context keyword; in ordinary function, pattern, and expression contexts it remains available as a `lower_identifier`.
Note: the semantic support state for attributes, derive traits, custom operators, and unsupported values is tracked in `docs/reference/semantic-capability-support-matrix.md`. `@derive(Eq, Show)` processes built-in derives in argument order. Starting with `0.5.0-alpha.3`, an argument may also resolve to a user function whose exact protocol is `comptime Meta.DeriveInput -> Meta.Expansion`; built-in derives and user generators execute to a fixed point in attribute-occurrence and argument source order.
Note: `Meta` is a compiler-built-in domain that needs no import. `Meta.typeInfo(Type)`, `typeName`, `hasField`, `fieldType`, and `declarationInfo` expose read-only target-independent facts; `Meta.layoutOf(Type, target)` requires an explicit target. `Meta.Expansion` accepts only structured declaration/expression/pattern builders, not string source insertion or arbitrary AST mutation.
Note: `Build` is also a compiler-built-in domain and adds no grammar production. Only the program selected by `[build].program` can acquire `Build.Fs`, `Build.Env`, `Build.Process`, and `Build.Emit` capabilities and declare the single top-level `BuildGraph`; capability access from ordinary pure comptime is rejected. Build programs use the ordinary name-first comptime binding, list, and call grammar.
Note: legacy `@borrow(...)` is migration input only and cannot grant borrow or ownership permissions. Eidos 0.7 derives its ownership contract from typed-signature `T` / `Ref[T]` / `MRef[T]`; borrow checking remains independent from effect authorization. The migrator stops atomically when it cannot prove an equivalent definition-and-call-site rewrite and never inserts `clone`.
Note: generic arguments are resolved in declaration order against the target parameter domains. A declared type parameter consumes a type argument, `comptime N: T` consumes a compile-time value expression, and an `effects` parameter consumes an effect-row argument. The same ordered domain model applies to ADTs, type aliases, constructors, functions, traits, `@impl(...)`, and named instances.
Note: in `0.5.0-alpha.1`, `comptime T: Type` and `T: comptime Type` are type-domain generic parameters, while `comptime N: Int` declares a value-domain const generic. Value arguments must be compile-time evaluable and type-correct. They participate in type identity, layout, name mangling, specialization, trait coherence, cache fingerprints, and IDE hover/completion. Floating-point values are rejected as specialization keys; values with runtime resource identity cannot cross this boundary.
Note: `@impl(...)` is semantically validated during naming. The function name/signature must match the target trait method, generic trait arguments are not inferred by convention registration, const-generic trait arguments are substituted into method signatures, and impl overlap is checked after alias canonicalization: strictly more specific heads may coexist with broader heads, while equivalent or incomparable canonical shapes are rejected with `E3004`.
Note: named `instance` declarations are the 0.4.0-alpha.1 evidence declaration surface implemented by the parser and name resolver. A `given` expression can explicitly select one named instance with `expr given InstanceName`; the selected evidence is resolved through the normal module/import visibility rules, registered in the structured trait impl table, checked for coherence, and mapped to concrete trait methods during type inference and HIR/MIR dispatch. `Name :: instance Trait for Type { Ctor => { method = expr } }` is a constructor instance bridge: it generates trait methods from same-named fact entries for every constructor of the target ADT.
Note: this branch no longer treats `proof` / lawful syntax as part of the tutorial baseline; those experimental capabilities are maintained on a dedicated proof branch.
Note: the `type_params?` after a constructor name declares constructor-local type parameters. Local parameters that do not appear in the return type are hidden after construction and can only be reopened by pattern matching, for example `type AnyDirection { AnyDirection[A](Direction[A]) -> AnyDirection }`.
Note: GADT constructors may use `Ctor(args) -> CurrentAdt[Specialized]`; the return type must instantiate the current ADT head. Constructors without `->` keep the ordinary sugar of returning the declaration head. The erased built-in evidence type `TypeEq[A, B]` currently has only `Refl`, which proves identical types or branch-local equalities refined by GADT patterns.
Note: ADT constructor lists use commas, for example `Direction :: type { North, South }`. Eidos 0.6 rejects `|` in an ADT body with a migration diagnostic; `eidosc migrate syntax --from 0.5.0-alpha.1 --to 0.6.0-alpha.1` performs the explicit rewrite.
Note: named constructor entries with `name: Type` are runtime fields and become constructor arguments, layout entries, and ABI-relevant fields. Entries with `name = expr` are no longer accepted in ADT constructors; write constructor facts in a named instance bridge instead. Bridge fact input supports literals, tuples, lists, restricted unary/binary expressions, value names or path references, and constructor/function call expressions. When combined with GADTs, generated impls are checked with constructor branch-local refinement.
Note: bare product-type sugar — when `adt_body` consists directly of `field ("," field)*` (no constructor variants), the compiler synthesizes a default constructor named after the type during name resolution, equivalent to writing `TypeName { field ("," field)* }` explicitly. For example `Point :: type { x: Int, y: Int }` is equivalent to `Point :: type { Point { x: Int, y: Int } }`, and can be constructed with `Point { x: 1, y: 2 }`, matched with `Point { x: a, y: b }`, and combined with `@derive`. `@cstruct` behavior is unchanged.
Note: the Eidos 0.7 development line permits nested `Case :: type { ... }` declarations as lexically sealed closed cases. Visibility is a semantic property of the root declaration: exporting the root exports the complete descendant case/constructor/field graph, while a non-exported or compiler-internal root keeps the complete graph private. A public root with a toolchain-internal descendant is rejected with `E3061`; there is no implicit hidden-case or non-exhaustive mode.
Note: partially applied aliases used as higher-kinded trait arguments now participate in reverse matching during type inference and specialization. For example, `@impl(Applicative[KeepEdges[String, Bool]])` may satisfy a later `G[A]` expectation when the concrete context resolves to `Triple[String, A, Bool]`. The same reverse inference also applies through precompiled stdlib combinators such as `Result.traverse`, recursive traversals such as `Seq.traverse`, empty/short-circuit traversable branches that specialize through `lift_pure` / `pure`, the public container-specific helpers `Option.sequence`, `Seq.sequence`, and `Result.sequence`, and the generic public helper `Traversable.sequence`; built-in `ResultWith[E]` nesting like `Option[Result[A, E]]`, `Seq[Result[A, E]]`, and `Result[Result[A, E], E]` is now covered as well.
Note: traits, effect tags, and ordinary functions resolve independently through qualified paths. An effect such as `Writer :: effect;` is referenced by `Io.Writer` or `Cap.Io.Writer` in `need`, while an ordinary function such as `write :: String -> Int need Writer` is called as `Io.write(...)` or `Cap.Io.write(...)`. Effects do not own operations.
Note: workspace `import` resolution now supports the `eidos.toml` project model. Resolution always searches the current file directory first; if the nearest ancestor contains `eidos.toml`, the compiler then searches `sourceRoots` followed by `importRoots` in declaration order and treats that config as the project boundary. Relative paths in both lists are resolved against the directory containing `eidos.toml`. Workspace source files discovered through those roots must declare a `module` path that matches their relative file path, and duplicate module paths are rejected as project-model errors. Only when there is no project config and no explicit import root (`CompilationOptions.ImportSearchRoots`, or repeated CLI `--import-root`) does the compiler fall back to the preferred ancestor import root rule (`src`/`source` when present, otherwise the nearest workspace root containing `.git` or a `.sln`). Explicit CLI import roots override only the selected project's `importRoots` while preserving valid `sourceRoots`; IDE `AddImport` suggestions use the same root selection rules. `eidos.toml` also supports compact defaults: omitted `sourceRoots` means `["src"]`, omitted `targets` may infer `main` from `src/Main.eidos` or `lib` from `src/Lib.eidos`, and `[dependencies]` accepts version strings or inline dependency tables. The CLI may receive a project directory or `eidos.toml` directly, use the selected or inferred target's `entry` as the compilation entry, and validate duplicate targets, missing entries, missing dependencies, and dependency cycles before compilation starts.

## 5. Expression Precedence (low to high)
```bnf
expr                 ::= pipe_expr
pipe_expr            ::= coalesce_expr (("|>" | ">>=") coalesce_expr)*
coalesce_expr        ::= or_expr ("??" coalesce_expr)?
or_expr              ::= and_expr ("||" and_expr)*
and_expr             ::= comparison_expr ("&&" comparison_expr)*
comparison_expr      ::= cons_expr (("<" | ">" | "<=" | ">=" | "==" | "!=") cons_expr)?
cons_expr            ::= custom_infix_expr (("+:") custom_infix_expr)*
               | custom_infix_expr (":+" custom_infix_expr)*
               | custom_infix_expr ((">>>" | "<<<") custom_infix_expr)*
custom_infix_expr    ::= additive_expr (operator_identifier additive_expr)*
additive_expr        ::= infix_call_expr (("+" | "-" | "++") infix_call_expr)*
infix_call_expr      ::= multiplicative_expr ("`" lower_identifier "`" multiplicative_expr)*
multiplicative_expr  ::= unary_expr (("*" | "/" | "%" | "<$>" | "<*>") unary_expr)*
unary_expr           ::= postfix_expr | "-" unary_expr | "!" unary_expr | "*" unary_expr | "&" unary_expr | "ref" unary_expr | "mref" unary_expr
                       // Note: in the current Types semantics, `ref expr` yields `Ref[T]`, `mref expr` yields `MRef[T]`, and `*expr` requires `Ref[T]` / `MRef[T]`; `ref/mref/&` only work on stable places (locals, fields, indexes, or `*r`) and no longer borrow from temporaries; `&expr` and legacy `MutRef[T]` remain accepted for now; read-only parameter positions allow `MRef[T]` to downgrade to `Ref[T]`; record-style ADT fields may also use `Ref[T]` / `MRef[T]` directly; returned `Ref/MRef` values must still trace back to input parameters, so a local alias of a parameter reference may return, but an alias overwritten by a local origin may not escape.
```
Note: `??` is the `Option` fallback operator. `option ?? fallback` lowers to `Std.Option.unwrap_or(option)(fallback)`; the left side must be `Option[T]`, the right side is `T`, and the expression result is `T`. `??` is right-associative, binds tighter than `|>` / `>>=`, and looser than `||`.
Note: `+:` is the sequence prepend operator (Scala-style) and `:+` is the sequence append operator. `head +: tail` lowers to `Std.Seq.cons(head)(tail)` (right-associative: `a +: b +: xs` = `a +: (b +: xs)`); `xs :+ last` lowers to `Std.Seq.append(xs)(Std.Seq.singleton(last))` (left-associative: `xs :+ a :+ b` = `(xs :+ a) :+ b`). Both operands share the cons precedence level. `::` is not an expression operator and serves only declaration binding.

## 6. Postfix Expressions and Chaining
```bnf
postfix_expr         ::= primary_expr
                       | postfix_expr "." lower_identifier ("(" arg_list? ")")?
                       | postfix_expr "." upper_identifier
                       | postfix_expr "." record_update_body
                       | postfix_expr "(" arg_list? ")"
                       | postfix_expr "[" expr "]"
                       | postfix_expr "given" item_path
                       // Note: bare `a.b` and `a.b()` still share the same grammar entry; semantically, `a.b()` remains a method call while bare `a.b` now first tries record-ADT field access.
record_update_body   ::= "{" field_init ("," field_init)* "}"
                       // Short record update: `state.{ tick: 0 }`
primary_expr         ::= ...
                       | "." "{" field_init ("," field_init)* "}"
                       // Contextual record literal: `.{ x: 1, y: 2 }`
                       | "(" operator_identifier ")"
```
Note: in 0.4.0-alpha.1 mode, `Trait[Concrete].Member` in expression position is an associated const projection. `expr given InstanceName` keeps the target expression shape while selecting a named instance evidence value for trait resolution.

These rules support:
1. Direct call: `f(x)`
2. Curried comma-call sugar: `f(a, b)` is equivalent to `f(a)(b)` and may stop early for partial application
3. Backtick infix call: ``a `f` b`` is equivalent to `f(a)(b)`
4. Method-style call: `obj.m(a)`
5. Dot application without parens: `obj.m`
6. Chained call: `obj.m().n()` and `obj.m.n`
7. Mixed postfix: `arr[0].m()(x)`

Empty call note: `f()` is a call with zero explicit arguments, not the `Unit` literal; explicitly passing `Unit` is still written as `f(())`. For an ordinary Eidos `Unit -> T` callable, `f()` desugars to `f(())` and consumes one `Unit` layer; for `Unit -> Unit -> T`, `f()` still returns `Unit -> T`. For an external/bodyless FFI C ABI declaration whose source signature uses `Unit -> T` to represent a C zero-argument function, `f()` lowers as a true C zero-argument call.

## 7. Common `primary_expr` and `pattern`
```bnf
primary_expr         ::= literal
                       | lower_identifier
                       | path_expr
                       | ctor_expr
                       | "(" expr ")"
                       | list_expr
                       | tuple_expr
                       | list_comprehension
                       | block_expr
                       | if_expr
                       | if_let_expr
                       | decision_expr
                       | while_let_expr
                       | loop_expr
                       | match_expr
                       | lambda_expr
                       | return_expr
                       | break_expr
                       | continue_expr
                       | unreachable_expr

ctor_expr            ::= upper_identifier "(" arg_list? ")"
                       | upper_identifier "{" field_init ("," field_init)* "}"
                       | upper_identifier "{" ctor_update_base ("," field_init)* "}"
ctor_update_base     ::= ".." expr
                       // Record update spread must appear once at the start: `GameState { ..state, tick: 0 }`
field_init           ::= lower_identifier ":" expr

list_expr            ::= "[" "]"
                       | "[" expr ("," expr)* "]"
                       | "[" expr ("," expr)* "," list_expr_rest "]"
                       | "[" list_expr_rest "]"
list_expr_rest       ::= ".."
                       | ".." expr

return_expr          ::= "return" expr?
break_expr           ::= "break" expr?
continue_expr        ::= "continue"
unreachable_expr     ::= "unreachable"

pattern              ::= "_"
                       | literal
                       | binding_pattern
                       | ctor_pattern
                       | tuple_pattern
                       | list_pattern
                       | range_pattern
                       | or_pattern
                       | view_pattern
                       | as_pattern
                       | "(" pattern ")"

range_pattern        ::= literal ".." literal
not_pattern          ::= "!" pattern_term
and_pattern          ::= pattern_term ("&" pattern_term)*
or_pattern           ::= and_pattern ("|" and_pattern)*
binding_pattern      ::= lower_identifier
                       | "ref" lower_identifier
                       | "mref" lower_identifier
                       | "mut" lower_identifier
                       | "ref" "mut" lower_identifier  // legacy compatibility
field_pattern        ::= lower_identifier ":" pattern
                       | lower_identifier
ctor_pattern         ::= upper_identifier
                       | upper_identifier "(" pattern ("," pattern)* ")"
                       | upper_identifier "(" ")"
                       | upper_identifier "{" record_field_pattern_list? "}"
record_field_pattern_list ::= field_pattern ("," field_pattern)* ("," record_pattern_rest)?
                       | record_pattern_rest
record_pattern_rest  ::= ".."
list_pattern         ::= "[" "]"
                       | "[" pattern ("," pattern)* "]"
                       | "[" pattern ("," pattern)* "," list_rest_pattern "]"
                       | "[" list_rest_pattern "]"
list_rest_pattern    ::= ".."
                       | ".." binding_pattern
                       | ".." "_"
                       // Only valid inside list_pattern and only at trailing position
view_pattern         ::= "(" expr "->" pattern ")"
                       // Semantic constraint: expr must be callable as unary expr(scrutinee), otherwise type phase emits E4014
                       // Partially applied functions are valid views, e.g. (key_pressed(81) -> true)
as_pattern           ::= or_pattern "as" binding_pattern
pattern_term         ::= not_pattern | range_pattern | pattern_atom
pattern_atom         ::= "_" | literal | binding_pattern | ctor_pattern | tuple_pattern
                       | list_pattern | view_pattern | "(" pattern ")"

if_expr              ::= "if" expr "then" expr else_clause?
if_let_expr          ::= "if" "let" pattern "=" expr "then" expr else_clause?
while_let_expr       ::= "while" "let" pattern "=" expr "then" block_expr
else_clause          ::= "else" expr
decision_expr        ::= "decide" expr "{" decision_group ("," decision_group)* ","? "}"
decision_group       ::= expr ":" decision_row ("," decision_row)* ","?
decision_row         ::= expr ("|" expr)* ("when" expr)? "=>" expr
```

When `else_clause` is omitted, the missing branch has value `Unit`. This makes side-effect-only conditionals natural as `if cond then { ... }`; conditionals that return a non-`Unit` value still need an explicit `else`.

`decision_expr` is for table-driven conditionals where the same predicate/template changes only by argument. `decide fallback { Input.key_down(_) != 0: 87 | 265 => North() }` tests keys in source order, returns the first matching row result, and returns fallback when no row matches. The template expression must contain a `_` hole; multi-hole templates use tuple keys.

## 8. Tutorial Guidance
1. For reproducible behavior, start from `docs/tutorial/examples/*`.
2. After grammar updates, update this file, both tutorial files, and editor syntax definitions together.
3. IDE/LSP snapshot diagnostics cover error / warning / info / help severities, which editors map to errors, warnings, information, or hints; `suggestions` entries with replacements are exposed as Quick Fixes. Current style suggestions include rewriting prefix calls such as `Seq.append(a)(b)` to fluent `a.append(b)`, and rewriting consecutive curried calls to grouped calls such as `Seq.append(a, b)`.
