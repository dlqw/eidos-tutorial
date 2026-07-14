# Eidos 核心 BNF（中文摘要）

本文件是“教程级摘要”，用于快速定位语法结构。
权威定义仍是 `src/Eidosc/Eidosc/Grammar/GrammarDefine.cs`。

## 1. 程序与声明
```bnf
program      ::= module_def* declaration*
module_def   ::= "module" module_path "{" declaration* "}"
               | "module" module_path ";"

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

package_path ::= package_alias "::" module_path
package_alias ::= lower_identifier | upper_identifier
import_path  ::= module_path | package_path
item_path    ::= module_path "::" (lower_identifier | upper_identifier)
               | package_path "::" (lower_identifier | upper_identifier)

import_stmt  ::= "import" import_path
               | "import" import_path "::" "*"
               | "import" import_path "::" "{" import_item ("," import_item)* "}"
name_first_import_binding ::= attribute* "export"? upper_identifier "::" "import" import_path ";"
import_item  ::= (lower_identifier | upper_identifier)
                 ("as" (lower_identifier | upper_identifier))?
operator_identifier ::= 一个或多个符号运算符字符，排除保留 token
```
说明：模块内一旦出现任意 `export` 声明，外部可见性即切换到显式导出模式；`export import ...` 会把导入绑定作为真实模块公开成员继续转发。
说明：模块导入别名是编译期模块名，必须使用大写开头。Selective import 的成员 alias 按目标符号分层：运行时 value alias 小写，类型、构造器、trait、effect 等编译期符号 alias 大写。
说明：依赖包中的模块使用 `packageAlias::Module.Path` 导入，其中 `packageAlias` 是当前项目 `eidos.toml` 中的依赖键。标准库内置 package alias 为 `Std`，因此标准库序列模块写作 `Std::Seq`，其中 `Std` 是包名，`Seq` 是模块名；标准库序列类型全名写作 `Std::Seq::Seq[Int]`。类型、函数、trait、effect、构造器等最终符号可写成 `packageAlias::Module.Path::Item`；当前包或已导入模块仍可使用 `Module.Path::Item` 或模块别名形式。未写 package 的模块路径会同时从当前 package、`Std` 和依赖 package 中收集候选；唯一候选通过，多个候选报错并要求写 package-qualified 路径。显式 package-qualified 路径不会回退到其他 package。
说明：在 `0.4.0-alpha.1` 的声明/import 语境中，module path 可使用 dot-separated module segments，例如 `Demo.Main :: module`、`Parser :: import Compiler.Parser;` 和 `import PackageAlias::Module.Path::{item}`。`::` 仍是 package 与 item qualification 分隔符；migration 只重写 import declaration 中 module path 的 `/` 分隔符。
说明：标准库 `Std::Prelude::*` 现在会通过上述 re-export 语义直接暴露常用 Text 安全 helper（如 `trim`、`char_code_at_or`、`last_index_of_or`）与基础 File 文本 I/O fallback（如 `read_text_or`、`write_text_result`）；完整模块仍可通过 `Std::Text`、`Std::File` 显式导入。
说明：无别名的模块导入会同时引入模块别名和该模块公开 value / constructor 的裸名；例如 `import Std::Seq` 后可写 `Seq::append(xs)(ys)` 或 `append(xs)(ys)`。name-first 模块别名绑定 `M :: import A;` 仍只引入别名，effect 也不会通过模块导入变成裸名可见。旧 `import A as M` keyword 形式只在 `legacy` 语法模式中接受；在 `0.4.0-alpha.1` 中会被拒绝并给出迁移诊断。
说明：自定义符号运算符可使用 `! $ % & * + / < = > ? ^ | - ~ :` 这组字符；`->`、`=>`、`:`、`::`、`=`、`<-`、`<$>`、`<*>`、`|>`、`?`、`??` 等保留 token 仍保持内建语法含义。
说明：用户声明或绑定名不得以 `__` 开头，也不得包含 `__spec_`；这些形式属于编译器保留内部命名空间，违反时报告 `E3055`。
说明：Eidos 采用命名分层：运行时值使用小写开头标识符；编译期值使用大写开头标识符。类型是一等编译期值，因此类型、trait、effect、构造器、模块路径和表示类型的泛型参数属于大写命名空间。构造器调用会产生运行时值，但构造器符号本身仍是编译期值。
说明：语法版本 `0.4.0-alpha.1` 新增初始 name-first 声明形态；当前已实现子集包括 `Name :: module`、`name :: Type -> Type { ... }`、`name :: Type -> Type;`、`name :: comptime Type -> Type { ... }`、`name :: value;`、`name :: Type = value;`、`Name :: comptime expr;`、`Name :: type`、`Name :: trait`、`Name :: effect;`、`Name :: instance Trait[...] { ... }` 与 `Alias :: import Module.Path;`。旧 keyword-first 形态仍在本文记录，作为当前默认语法与迁移期间的兼容基线。

## 2. 绑定与语句
```bnf
let_decl     ::= attribute* "let" "mut"? pattern (":" type)? "=" expr ";"
name_first_local_binding ::= "comptime"? "mut"? pattern (":" type)? ":=" expr ";"
let_question_decl ::= "let" "?" pattern "=" expr ";"
assignment   ::= lower_identifier ":=" expr ";"
expr_stmt    ::= expr ";"

stmt_non_expr        ::= let_decl | name_first_local_binding | let_question_decl | assignment
block_expr           ::= "{" block_body
block_body           ::= "}"
                       | stmt_non_expr block_body
                       | expr block_expr_after_expr
block_expr_after_expr ::= ";" block_body | "}"
```
说明：块只有在最后一项是未带分号的表达式时才有显式 tail expression，该表达式是块值。没有显式 tail expression 的块值为 `Unit`，包括空块、只包含声明/赋值的块，或最后一个表达式带分号的块。

说明：在 `0.5.0-alpha.1` 中，`comptime name := expr;` 表示局部编译期绑定，`name :: comptime A -> B { ... }` 表示 comptime-only 函数。`comptime mut`、对 comptime binding 赋值、运行期引用 comptime 值和运行期调用 comptime-only 函数都会被拒绝。类型化 evaluator 接受标量与 String、无 spread 的 tuple/list、普通 ADT 构造器与字段、递归 comptime-only 调用、带局部绑定的纯 block、纯 unary/binary operator、编译期 `if`，以及带 literal/range/wildcard/or/tuple/list/constructor pattern 和纯 guard 的 `match`。named-argument call、partial comptime application、borrow、effect、FFI、`with`、文件/环境/进程访问和运行期资源身份不属于 pure CTFE。被重新具化的 comptime 值直接进入类型化 HIR，不生成运行期 getter 或 local。

说明：`let?` 只在函数或 lambda 的块内有效，不是合法顶层声明；parser 可接受顶层形态用于给出定向诊断。`let? pattern = expr;` 的右侧必须是 `Option[A]` 或 `Result[A, E]`，`pattern` 当前要求不可反驳。`Option` 形态要求当前返回上下文为 `Option[R]`，失败分支短路返回 `None()`；`Result` 形态要求当前返回上下文为 `Result[R, E]`（或 canonical 等价 alias），失败分支短路返回原 `Err(e)`。`let?` 是前端语法糖，只存在于 Parser/AST/NameResolver/Types 阶段；HIR 构建时会消除为普通 `match` + `return`，HIR/MIR/LLVM 不存在 `let?` 专用节点。

## 3. 函数与类型签名
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

effect_path  ::= (module_path "::")? upper_identifier
               | package_path "::" upper_identifier
trait_path   ::= (module_path "::")? upper_identifier
               | package_path "::" upper_identifier
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
说明：在 `0.4.0-alpha.1` 中，`@ffi("malloc") malloc :: Int -> RawPtr;` 这样的顶层 name-first 函数声明表示无函数体声明，主要用于 external/FFI 声明。name-first 入口函数不再获得 legacy 隐式根 `FFI`/`IO` 能力；从 `main` 或其他入口函数调用 FFI/native 声明时，必须由显式 `need FFI` 或等价 effect tag 覆盖。
说明：类型后缀 `?` 是 `Std::Option::Option[...]` 的语法糖，例如 `Int?` 等价于 `Std::Option::Option[Int]`；连续后缀可嵌套，`Int??` 等价于 `Option[Option[Int]]`。
说明：在 0.4.0-alpha.1 模式中，`Iterator[I].Item` 这类类型后缀投影表示目标 trait 声明的 associated type，并可在存在具体 named instance 时归约到对应实现类型。

## 4. Trait、ADT、Type Alias、Attribute
```bnf
effect_def   ::= attribute* upper_identifier "::" "effect" ";"
trait_def    ::= attribute* "trait" upper_identifier type_params? generic_where? "{" func_decl* "}"
name_first_trait_def ::= attribute* upper_identifier type_params? "::" "trait"
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

adt_def      ::= attribute* (("type") upper_identifier type_params? generic_where? "{" adt_body "}"
               | type_alias)
type_alias   ::= ("type") upper_identifier type_params? generic_where? "=" type ";"
adt_body     ::= constructor ("," constructor)* ","?
               | field ("," field)* ","?
constructor  ::= upper_identifier type_params? ctor_args? ("->" type)?
ctor_args    ::= "(" type ("," type)* ")"
               | "{" field ("," field)* "}"

attribute    ::= "@" lower_identifier ("(" arg_list? ")")?
```
说明：`by` 是 proof 语境关键词；在普通函数、模式和表达式语境中仍可作为 `lower_identifier` 使用。
说明：属性、derive trait、自定义 operator 与 unsupported value 的语义支持状态见 `docs/reference/semantic-capability-support-matrix.md`。`@derive(Eq, Show)` 会按参数顺序处理多个内建 derive trait。`0.5.0-alpha.3` 起，参数也可以解析为签名严格等于 `comptime Meta::DeriveInput -> Meta::Expansion` 的用户函数；内建 derive 与用户 generator 均按 attribute occurrence 和参数源码顺序执行到固定点。
说明：`Meta` 是无需 import 的编译器内建域。`Meta::typeInfo(Type)`、`typeName`、`hasField`、`fieldType` 与 `declarationInfo` 提供只读、target-independent 事实；`Meta::layoutOf(Type, target)` 必须显式给出 target。`Meta::Expansion` 只接受结构化 declaration / expression / pattern builder，不接受字符串源码插入或任意 AST 修改。
说明：effect 声明上的 `@borrow(...)` 仅作为可接受的元数据，不会授予借用权限；borrow 检查与 effect 授权相互独立。
说明：generic argument 按声明顺序和目标参数 domain 解析。类型参数消费 type argument，`comptime N: T` 消费编译期 value expression，`effects` 参数消费 effect-row argument。ADT、type alias、constructor、function、trait、`@impl(...)` 与 named instance 使用同一套有序 domain 模型。
说明：在 `0.5.0-alpha.1` 中，`comptime T: Type` 与 `T: comptime Type` 是类型域泛型参数，`comptime N: Int` 是值域 const generic。值实参必须可编译期求值且类型正确，并参与 type identity、layout、name mangling、specialization、trait coherence、cache fingerprint 和 IDE hover/completion。浮点值不能作为 specialization key；带运行期资源身份的值不能跨越该边界。
说明：`@impl(...)` 也会在命名阶段做语义校验：函数名/签名必须与目标 trait 方法匹配；泛型 trait 实参不会由约定式注册自动推断；const-generic trait 实参会替换进方法签名；impl 重叠会在 alias canonicalization 后比较：严格更具体的头可以与更宽泛的头共存，但若 canonical 形状等价或不可比较，仍会直接以 `E3004` 拒绝。
说明：named `instance` 声明是 0.4.0-alpha.1 已实现的 evidence 声明表面。`given` 表达式可用 `expr given InstanceName` 显式选择一个 named instance；被选择的 evidence 会通过普通 module/import 可见性规则解析，注册到结构化 trait impl 表，参与 coherence 检查，并在类型推断与 HIR/MIR dispatch 中映射到具体 trait 方法。`Name :: instance Trait for Type { Ctor => { method = expr } }` 是构造器 instance bridge：它从目标 ADT 每个构造器的同名 fact entry 生成对应 trait 方法。
说明：当前分支不再把 `proof` / lawful 语法作为教程基线的一部分；相关能力移至单独的 proof 分支维护。
说明：构造器名后的 `type_params?` 是构造器局部类型参数。没有出现在返回类型中的局部参数会在构造后隐藏，只能通过模式匹配分支重新打开，例如 `type AnyDirection { AnyDirection[A](Direction[A]) -> AnyDirection }`。
说明：GADT 构造器可写 `Ctor(args) -> CurrentAdt[Specialized]`，返回类型必须是当前 ADT head 的实例；无 `->` 的普通构造器仍按声明 head 糖化。内建擦除证据 `TypeEq[A, B]` 目前只有 `Refl`，用于同一类型或 GADT 分支局部精化产生的等式证据。
说明：ADT 构造器列表的新代码使用逗号分隔，例如 `Direction :: type { North, South }`。迁移期 parser 仍兼容旧 `|` 构造器分隔符，但同一个 ADT 构造器列表不能混用 `,` 和 `|`。
说明：构造器命名块中 `name: Type` 是运行时字段，会成为构造器参数、layout 项和 ABI 相关字段。ADT 构造器中不再接受 `name = expr`；构造器事实应写入 named instance bridge。bridge fact 输入支持字面量、元组、列表、受限一元/二元表达式、值名或路径引用，以及构造器/普通函数调用表达式；与 GADT 组合时，生成 impl 按构造器分支局部精化检查。
说明：裸积类型语法糖——当 `adt_body` 直接由 `field ("," field)*` 组成（无构造器变体）时，编译期在命名阶段自动合成一个与类型同名的默认构造子，等价于显式写出 `TypeName { field ("," field)* }`。例如 `Point :: type { x: Int, y: Int }` 等价于 `Point :: type { Point { x: Int, y: Int } }`，可直接用 `Point { x: 1, y: 2 }` 构造、`Point { x: a, y: b }` 模式匹配，并可叠加 `@derive`。`@cstruct` 行为不变。
说明：作为高阶 trait 实参使用的部分应用 alias，现在也会参与类型推断/特化时的反向匹配。例如 `@impl(Applicative[KeepEdges[String, Bool]])` 在具体上下文收敛为 `Triple[String, A, Bool]` 时，可以满足后续的 `G[A]` 需求。这种反向推断同样适用于 `Result::traverse`、`Seq::traverse`、通过 `lift_pure` / `pure` 走空输入或短路分支的 traversable 标准库组合子、公开的 `Option::sequence` / `Seq::sequence` / `Result::sequence` helper，以及泛型公开 helper `Traversable::sequence`；现在也覆盖了 `Option[Result[A, E]]`、`Seq[Result[A, E]]`、`Result[Result[A, E], E]` 这类内建 `ResultWith[E]` 同构嵌套。
说明：trait、effect tag 与普通函数通过限定路径独立解析。`Writer :: effect;` 在 `need` 中写作 `Io::Writer` 或 `Cap.Io::Writer`；普通函数 `write :: String -> Int need Writer` 则写作 `Io::write(...)` 或 `Cap.Io::write(...)`。effect 不拥有 operation。
说明：workspace `import` 解析现在支持 `eidos.toml` 项目模型。解析总是先搜索当前文件所在目录；若最近祖先存在 `eidos.toml`，则继续按 `sourceRoots`、`importRoots` 的声明顺序搜索，并把该配置视为工程边界。两类相对路径都按 `eidos.toml` 所在目录解析。通过这些工程根发现的 workspace 源文件，要求声明的 `module` 路径与相对文件路径一致；重复模块路径会作为工程模型错误拒绝。只有在没有项目配置且没有显式导入根（`CompilationOptions.ImportSearchRoots`，或重复传入 CLI `--import-root`）时，编译器才回退到“首选祖先导入根”规则（若祖先链上存在 `src`/`source` 则优先使用，否则回退到最近的 workspace 根，例如包含 `.git` 或 `.sln` 的目录）。显式 CLI 导入根只覆盖所选项目中的 `importRoots`，保留有效 `sourceRoots`；IDE 的 `AddImport` quick-fix 也使用同一套导入根选择规则。`eidos.toml` 还支持紧凑默认值：省略 `sourceRoots` 等价于 `["src"]`，省略 `targets` 时可由 `src/Main.eidos` 推断 `main` 或由 `src/Lib.eidos` 推断 `lib`，`[dependencies]` 可接受版本字符串或 inline dependency table。CLI 可直接接收项目目录或 `eidos.toml`，选择或推断入口 target 的 `entry` 作为编译入口，并在编译前验证重复 target、缺失入口、缺失依赖与依赖环。

## 5. 表达式优先级（从低到高）
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
                       // 注：当前 Types 语义下，`ref expr` 产出 `Ref[T]`，`mref expr` 产出 `MRef[T]`，`*expr` 需要 `Ref[T]` / `MRef[T]` 操作数；`ref/mref/&` 只能作用在稳定位置（局部、字段、索引、`*r`），不能对临时表达式取引用；`&expr` 与旧 `MutRef[T]` 仍兼容；只读参数位置允许 `MRef[T]` 自动退化成 `Ref[T]`；record 风格 ADT 字段类型也可直接写 `Ref[T]` / `MRef[T]`；返回 `Ref/MRef` 时来源必须能追到输入参数，参数 reference 的局部 alias 允许返回，但被局部来源覆盖后的 alias 不允许逃逸。
```
说明：`??` 是 `Option` fallback 运算符，`option ?? fallback` 按 `Std::Option::unwrap_or(option)(fallback)` 降低；左侧必须是 `Option[T]`，右侧是 `T`，表达式结果也是 `T`。`??` 右结合，且优先级高于 `|>` / `>>=`、低于 `||`。
说明：`+:` 是序列 prepend 运算符（Scala 风格），`:+` 是序列 append 运算符。`head +: tail` 降低为 `Std::Seq::cons(head)(tail)`（右结合：`a +: b +: xs` = `a +: (b +: xs)`）；`xs :+ last` 降低为 `Std::Seq::append(xs)(Std::Seq::singleton(last))`（左结合：`xs :+ a :+ b` = `(xs :+ a) :+ b`）。两者共享 cons 优先级。`::` 不再是运算符，仅用于静态绑定与路径限定。

## 6. 后缀表达式与链式调用
```bnf
postfix_expr         ::= primary_expr
                       | postfix_expr "." lower_identifier ("(" arg_list? ")")?
                       | postfix_expr "." upper_identifier
                       | postfix_expr "." record_update_body
                       | postfix_expr "(" arg_list? ")"
                       | postfix_expr "[" expr "]"
                       | postfix_expr "given" item_path
                       // 注：语法上裸 `a.b` 与 `a.b()` 共用这一条；当前语义里，`a.b()` 保持方法调用，而裸 `a.b` 会优先尝试 record-ADT 字段读取。
record_update_body   ::= "{" field_init ("," field_init)* "}"
                       // 短 record update：`state.{ tick: 0 }`
primary_expr         ::= ...
                       | "." "{" field_init ("," field_init)* "}"
                       // 上下文推断 record literal：`.{ x: 1, y: 2 }`
                       | "(" operator_identifier ")"
```
说明：在 0.4.0-alpha.1 模式中，表达式位置的 `Trait[Concrete].Member` 是 associated const 投影。`expr given InstanceName` 保留目标表达式形态，同时为 trait resolution 选择一个 named instance evidence 值。

这组规则支持：
1. 普通调用：`f(x)`
2. 柯里化逗号调用糖：`f(a, b)` 等价于 `f(a)(b)`，也可以提前停止形成部分应用
3. 反引号中缀调用：``a `f` b`` 等价于 `f(a)(b)`
4. 方法样式调用：`obj.m(a)`
5. 无括号点应用：`obj.m`
6. 链式调用：`obj.m().n()` 与 `obj.m.n`
7. 混合后缀：`arr[0].m()(x)`

空实参调用说明：`f()` 是零个显式实参的调用，不是 `Unit` 字面量；显式传递 `Unit` 仍写作 `f(())`。当目标是普通 Eidos `Unit -> T` 时，`f()` 降糖为 `f(())`，且只消耗一层 `Unit` 参数；`Unit -> Unit -> T` 的 `f()` 结果仍是 `Unit -> T`。当目标是 external/bodyless FFI C ABI 声明，且源码签名用 `Unit -> T` 表示 C 零参数函数时，`f()` 降低为真实 C 零参数调用。

## 7. primary_expr 与 pattern（常用）
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
                       // record update spread 必须在开头出现一次：`GameState { ..state, tick: 0 }`
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
                       | "ref" "mut" lower_identifier   // 兼容旧写法
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
                       // 仅可用于 list_pattern，且必须出现在列表模式尾部
view_pattern         ::= "(" expr "->" pattern ")"
                       // 语义约束：expr 需可作为一元函数调用（expr(scrutinee)），否则类型阶段报 E4014
                       // 可使用部分应用函数作为 view，如 (key_pressed(81) -> true)
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

省略 `else_clause` 时，缺失分支为 `Unit`。这让只为副作用执行的条件块可以自然写成 `if cond then { ... }`；返回非 `Unit` 值的条件表达式仍必须写出 `else`。

`decision_expr` 用于同一 predicate/template 只改变参数的表驱动条件。`decide fallback { Input::key_down(_) != 0: 87 | 265 => North() }` 按源码顺序测试每个 key，命中第一个 row 时返回 row result，否则返回 fallback。表头表达式必须包含 `_` hole；多 hole 表头使用 tuple key。

## 8. 教程建议
1. 需要稳定复现时，优先使用 `docs/tutorial/examples/*` 中已验证样例。
2. 语法调整后，先更新本文件，再更新中英文教程正文与插件高亮语法。
3. IDE/LSP 快照中的诊断严重级别覆盖 error / warning / info / help，编辑器映射为错误、警告、提示或建议；`suggestions` 中带 replacement 的条目会作为 Quick Fix。当前风格建议包括：前缀调用 `Seq::append(a)(b)` 可改为链式 `a.append(b)`，连续柯里化调用也可改为分组调用 `Seq::append(a, b)`。
