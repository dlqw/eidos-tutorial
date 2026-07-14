# Eidos 教程（中文）

> 语言基线：本教程以 Eidos 0.5.0-alpha.1 为准。新代码使用 `name :: Type { ... }`、`name :: expr;`、局部 `name := expr;` / `mut name := expr;`；旧源码只通过显式迁移命令处理。

## 1. 教程范围与验证基线
本教程只描述当前仓库中已经实现并可复现的功能。所有可运行的示例均在 [`docs/tutorial/examples/`](examples/) 目录下。
语法权威来源为 `src/Eidosc/Eidosc/Grammar/GrammarDefine.cs`，验证基线为 2026-03-19 的本地执行结果。

## 2. 快速开始
先确认编译器可运行：

```powershell
dotnet build src/Eidosc/Eidosc.sln
dotnet run --project src/Eidosc/Eidosc.Cli -- --help
```

再运行教程示例验证脚本：

```powershell
powershell -ExecutionPolicy Bypass -File docs/tutorial/verify-examples.ps1
```

项目建议使用下面这种最小 `eidos.toml`：

```toml
manifestSchema = 3

[language]
version = "0.5.0-alpha.1"

[package]
name = "dev.eidos.app"
version = "0.1.0"
```

其中：
1. `language.version` 是用户应主动设置的 Eidos 语言 SemVer；
2. `manifestSchema` 是 manifest schema 版本，当前纳入版本控制的 manifest 应显式声明 `3`。

## 3. 语言主线（已验证示例）
### 3.1 字面量与绑定
示例文件：`examples/01_literals_bindings.eidos`  
覆盖：`let`、`let mut`、整型/浮点/字符串/布尔/字符字面量。

```eidos
answer :: 42;
pi :: 3.14;
greeting :: "hello";
ok :: true;
initial :: 'a';
let mut counter = 1;
```

命名分层：运行时值使用小写开头标识符；编译期值使用大写开头标识符。类型是一等编译期值，因此类型、trait、effect、构造器、模块路径和表示类型的泛型参数属于大写命名空间。构造器调用会产生运行时值，但构造器符号本身仍是编译期值。

补充（2026-03-16）：内置 I/O 新增 `print_char: Int -> Unit`，按字符码输出单字符（例如 `34` 为 `"`，`39` 为 `'`）。

### 3.1.1 `let` 模式绑定（块级）
示例文件：`examples/12_let_pattern.eidos`、`examples/15_pattern_binding_modes.eidos`  
当前支持块级 `let <pattern> = <expr>;`，并在 NameResolver/Types/HIR/MIR 贯通。

```eidos
sum_pair :: (Int, Int) -> Int
{
    pair => {
        let (a, b) = pair;
        a + b
    }
}
```

当前约束（2026-03-18）：`let` 暂要求“不可反驳模式”（irrefutable pattern）；可反驳场景请使用 `match`。

补充（2026-04-04 / 2026-06-19）：模式绑定 mode 与可变绑定已贯通到语义与 MIR：
1. 正式写法支持 `let ref x = ...` 与 `let mref x = ...`；
2. `p as ref x` / `p as mref x` 会保留绑定 mode；
3. 可重赋值绑定使用 `let mut x = ...`；只有 `let mut` 绑定可以作为 `:=` 赋值目标；
4. `or-pattern` 中同名绑定必须使用一致的绑定 mode，否则会报 `E3000`。

### 3.1.2 `if let` 模式分支
示例文件：`examples/13_if_let_pattern.eidos`  
当前支持 `if let <pattern> = <expr> then <expr> else <expr>`，块分支仍然只是普通表达式的一种。语义上会降低为 `match`：

```text
if p := e then { t } else { f }
==> match e { p => { t }, _ => { f } }
```

短分支也可以直接写成 `if p := e then t else f`。

绑定作用域规则：
1. `pattern` 中的绑定变量仅在 `then` 分支可见；
2. `else` 分支与后续外层作用域不会看到这些绑定；
3. 没有 `else` 时，默认回退分支值为 `Unit`。

### 3.1.3 `while let` 模式循环
示例文件：`examples/14_while_let_pattern.eidos`  
当前支持 `while let <pattern> = <expr> then <block>`。语义上按循环匹配降低：

```text
while p := e then { body }
==> loop { match e { p => { body; () }, _ => break } }
```

当前行为：
1. `pattern` 绑定仅在循环体块中可见；
2. 匹配失败时自动退出循环（内部使用 `break` 语义）；
3. 表达式类型固定为 `Unit`，用于流程控制而不是值计算。
4. 块内在 `if/if let/while let` 语句后仍可继续写无分号尾表达式作为块值（例如 `...; total`）。

### 3.1.4 `let?` Option / Result 绑定
示例文件：`examples/63_let_question_option_result.eidos`
当前支持块内 `let? <pattern> = <expr>;`，用于展开 `Option` / `Result` 成功值，并在失败分支提前返回当前函数或 lambda。

```eidos
option_pipeline :: Int -> Option[Int]
{
    value => {
        let? first = maybe_positive(value);
        let? second = maybe_positive(first);
        Some(second + 1)
    }
}

result_pipeline :: Int -> Result::ResultWith[String, Int]
{
    value => {
        let? first = parse_positive(value);
        let? second = parse_positive(first);
        Ok(second + 1)
    }
}
```

约束：
1. `let?` 不是顶层声明，只能出现在有返回上下文的函数或 lambda 块内；
2. 右侧必须是 `Option[A]` 或 `Result[A, E]`；
3. `Option[A]` 只能用于返回 `Option[R]` 的上下文，失败分支返回 `None()`；
4. `Result[A, E]` 只能用于返回 `Result[R, E]` 或 canonical 等价 alias 的上下文，失败分支返回原 `Err(e)`；
5. 当前不做 `Option -> Result` 转换，也不做错误类型映射；
6. 左侧模式当前要求不可反驳，可反驳解构仍使用 `match` / `if let`。

降低规则：`let?` 只存在于 Parser/AST/NameResolver/Types 阶段；HIR 构建时会消除为普通 `match` + `return`。HIR/MIR/LLVM 中没有 `let?` 专用节点。

### 3.2 函数、泛型与普通调用
示例文件：`examples/02_functions_calls.eidos`  
覆盖：函数签名、泛型参数、普通调用。

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

对柯里化函数，逗号分隔的调用参数会从左到右逐个应用：`add(1, 2)` 等价于 `add(1)(2)`，`sum3(1, 2)` 仍可返回等待最后一个参数的函数。反引号中缀调用沿用同一规则：``left `add` right`` 等价于 `add(left)(right)`。

#### 值级 const generic（0.5.0-alpha.1）

`comptime N: Int` 声明值域泛型参数；`comptime T: Type` 仍声明类型域参数。参数顺序就是应用顺序，编译器在 AST、symbol、HIR/MIR、缓存和 IDE 中保留 type/value/effect-row 三种 domain，不再把值参数伪装成类型参数。

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

值实参必须在实例化点可编译期求值，并且类型与参数注解一致。未被普通参数类型约束的值参数必须显式提供；能从 `Buffer[N, T]` 这类参数/结果类型推出时可以省略。值会进入 nominal type identity、layout、name mangling、generic specialization、trait coherence 和增量缓存键，因此 `Buffer[4, Int]` 与 `Buffer[5, Int]` 是不同类型。浮点值在 alpha.1 中不能作为 specialization key；引用、指针、closure 和其他带运行期资源身份的值也不能越过 comptime/type identity 边界。

ADT、type alias、函数、trait、`@impl(...)` 和 named instance trait 引用均使用同一套有序 generic argument 规则。例如 `Sized[comptime N: Int] :: trait { ... }` 的实现必须显式写成 `@impl(Sized[4])`；trait 方法签名中的 `N` 会按该值替换，coherence 也会比较结构化值 key。完整示例见 `examples/68_const_generics.eidos`。

#### 只读类型反射、用户 derive 与结构化生成（0.5.0-alpha.3）

`Meta` 是编译器内建域，不属于 `Std`，也不需要 `import`。反射只返回已经完成且可稳定序列化的编译器事实，不暴露可变 AST 或内部 `SymbolId`：

```eidos
Info :: comptime Meta::typeInfo(User);
Kind :: comptime Meta::typeKind(Info);
HasName :: comptime Meta::hasField(User, "name");
NameType :: comptime Meta::fieldType(User, "name");
IntLayout :: comptime Meta::layoutOf(Int, "x86_64-pc-windows-msvc");
```

`Meta::typeInfo` 覆盖 primitive、tuple、function、reference、ADT 与 trait；构造器、字段、关联声明和属性保持稳定源码顺序。`Meta::layoutOf` 是独立的 target-dependent 查询，必须显式给出受支持的 target triple；尚未完成布局的类型会产生 phase diagnostic，不会退回 host layout。

用户 derive 必须是 `comptime Meta::DeriveInput -> Meta::Expansion`：

```eidos
Marker :: trait {
    marker :: Self -> String
}

deriveMarker :: comptime Meta::DeriveInput -> Meta::Expansion {
    input => {
        target := Meta::target(input);
        parameter := Meta::parameter("value", target);
        method := Meta::function(
            "marker",
            [parameter],
            String,
            Meta::exprString(Meta::typeName(target))
        );
        Meta::expansion([
            Meta::implementation(Meta::decl(Marker), target, [method])
        ])
    }
}

@derive(deriveMarker)
User :: type {
    name: String,
    age: Int
}
```

`Meta::Expansion` 可结构化生成 implementation、function、comptime value、attribute attachment、test、diagnostic 与 module member。函数体通过 `Meta::Expr` builder 构造，pattern branch 通过 `Meta::Pattern` / `Meta::Branch` builder 构造；声明引用使用 `Meta::Decl`，生成器局部引用使用 `Meta::Parameter` / `Meta::Binding` handle，以保持卫生。生成声明进入普通名称解析、类型检查、trait coherence、completion、hover、definition 与 references，并带稳定 `eidos-generated://` origin。

当前边界：不支持字符串源码插入、任意 AST 替换或删除；`@impl` 等语义属性不能通过 attribute attachment 回写，必须生成对应 structured declaration；pure comptime 不能读取文件、环境、进程、网络或 FFI。扩展按依赖与源码顺序执行到固定点，cycle、重复 stable identity、非法协议签名、预算超限和不完整反射都会产生确定 diagnostic。完整示例见 `examples/69_meta_reflection_derive.eidos`。

可用 CLI：

```powershell
eidosc meta expand source.eidos --format json
eidosc meta expand source.eidos --emit-generated generated
eidosc meta expand source.eidos --trace-comptime --comptime-budget 200000
```

普通同一作用域函数可以同名，只要参数签名不同。调用点会根据实参类型选择重载，
覆盖普通调用、限定路径调用、链式方法调用、中缀调用和管道调用。裸重载函数引用
必须有期望函数类型；否则编译器会报告歧义。

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

重复重载只看参数签名：返回类型、能力需求、声明顺序和函数体形态都不能让两个
重载变成不同声明。例如 `parse :: String -> Int` 与
`parse :: String -> String` 会被拒绝为重复重载。泛型签名会做 alpha 归一化，
所以 `id[T] :: T -> T` 与 `id[U] :: U -> U` 也是重复重载。

符号函数可以用带括号的运算符名声明，并直接作为中缀运算符使用：

```eidos
(|+|) :: Int -> Int -> Int
{
    left => right => left + right
}

asInfix :: 1 |+| 2;
asFunction :: (|+|)(3, 4);
```

自定义符号运算符默认左结合，优先级位于加减/拼接之后、`::`/函数组合之前。内建复杂运算符如 `|>`、`>>=`、`>>>`、`<<<`、`<$>`、`<*>`、`<>` 保持各自固定优先级与标准库 lowering。

风格约定（2026-05-28）：新的教程与标准库示例应优先展示 Eidos 的函数式读法。线性数据流优先写 `value |> f |> g`；函数组合优先写 `f >>> g` 或 `g <<< f`；`Functor` / `Applicative` / `Monad` 场景优先展示 `f <$> value`、`mf <*> mx`、`mx >>= f`；连续容器变换优先写链式调用，例如 `xs.map(f).filter(p).fold_left(seed)(step)`。当限定路径能显著降低歧义时，仍可保留 `Module::function(value)(arg)` 写法。普通分组调用 `function(value)` / `function(value, arg)` 也是稳定的默认调用风格，不会仅因为可写成链式或中缀而提示。CLI/IDE/LSP 会把可机械转换的连续柯里化前缀调用作为 help/hint 级风格建议给出 Quick Fix：`Seq::append(a)(b)` 可改为链式 `a.append(b)`，也可改为分组调用 `Seq::append(a, b)`。

补充（2026-03-27）：函数签名现支持高阶类型参数 kind 注解。
1. 一元构造器 kind：`F: kind2`
2. 多参数构造器 kind：`F: kind3`
3. 支持带括号的高阶 kind：`F: kind2 -> kind1`
4. kind 兼容检查同时覆盖“类型参数构造器应用”（如 `F[G]`）与“ADT 类型实参”（如 `Lift[F: kind2]` 下的 `Lift[Int]` 会报错）。
5. 具名类型构造器在高阶位置支持部分应用（例如 `Lift[F: kind2]` 下可写 `Lift[Either[String]]`）。
6. 未显式标注 kind 时会根据类型用法自动推断（例如 `F[A]` 会推断 `F: kind2`）。
7. 未显式标注的 ADT 类型参数 kind 也会根据构造器/别名中的类型使用自动推断（例如 `type Lift[F] { Lift(F[Int]) }` 推断 `F: kind2`，`type UseK[K] { UseK(K[Box]) }` 推断 `K: kind2 -> kind1`，因此在 `type ApplyToInt[F: kind2] { ... }` 存在时可合法写 `UseK[ApplyToInt]`）。
8. Effect 行参数必须显式使用 `effects` kind（例如 `E: effects`）；值类型的高阶 kind 继续使用 `kind1`、`kind2` 或 `kind3`。
9. kind 约束采用累积统一（unification）：同一类型变量的多条 kind 约束共享同一推断状态，因此像同时要求 `kind2` 与 `kind1` 这类不兼容约束会被稳定报错，而不是彼此独立放过。
10. trait 声明已支持一等 type params 语法（如 `Functor[F: kind2] :: trait { ... }`），且当 trait 类型参数未显式标注时会根据方法签名自动推断 kind（例如 `HK[K] :: trait { run :: K[Box] -> Self }` 推断 `K: kind2 -> kind1`），并在 symbol/HIR 层保留元数据。
11. 类型参数 trait 约束已支持模块限定与类型实参（如 `T: Core::Functor[Box]`）；Types 阶段会校验 trait 实参数量与 kind。
12. 类型参数 trait 约束位置只接受 trait；effect 授权通过函数 `need` 从句表达（例如 `String -> Unit need Writer`）。
13. 泛型约束支持轻量 `where` 从句，可把复杂 kind/trait 约束移出参数列表，例如 `lift[A, G] :: A -> G[A] where G: kind2, G: Applicative[G]`。
示例文件：`examples/31_hkt_parenthesized_kind.eidos`、`examples/32_hkt_adt_inferred_kind.eidos`、`examples/33_hkt_effect_polymorphism.eidos`、`examples/34_hkt_trait_inferred_kind.eidos`、`examples/35_hkt_trait_constraint_type_args.eidos`、`examples/36_hkt_trait_constraint_kind_mismatch.eidos`。

补充（2026-03-22）：`return <expr>` 现在会把 `<expr>` 与当前函数/匿名函数声明返回类型做统一（不再退化成 `Unit`），可消除合法早返回路径上的 `Int` vs `()` 假冲突。

补充（2026-04-04）：源码一元解引用 `*expr` 与引用构造 `ref expr` / `mref expr` 已进入第一阶段显式引用语义。  
当前语义：
1. `ref expr` 在 Types 阶段返回 `Ref[T]`；
2. `mref expr` 在 Types 阶段返回 `MRef[T]`；
3. `*expr` 要求操作数是 `Ref[T]` 或 `MRef[T]`，结果类型为内部的 `T`；
4. `MRef[T]` 是正式可写引用名；旧 `MutRef[T]` 当前仍作为兼容别名接受；
5. `&expr` 当前仍兼容，但已降为过渡写法，不是长期表面模型；
6. `ref expr` / `mref expr` / `&expr` 现在都只允许作用在“稳定位置”上：例如局部变量、字段、索引、`*r`；像 `ref (x + 1)`、`mref make()` 这种对临时值取引用会直接在 Types 阶段报错；
7. 只读读取路径开始支持小范围自动穿透：例如 `Ref[Seq[Int]]` / `MRef[Seq[Int]]` 现在可直接写 `xs[0]`，不必先写 `(*xs)[0]`；
8. 只读参数位置现在接受 `MRef[T] -> Ref[T]` 的自动退化：例如 `read: Ref[Int] -> Int` 可以直接写 `read(mref x)`；
9. 裸点访问 `a.b` 现在会优先在 record 风格 ADT 上按字段读取解释；`a.b()` 仍是方法调用。因此 `Ref[Range]` / `MRef[Range]` 现在可直接写 `r.start`，不必先写 `(*r).start`；
10. record 风格 ADT 的字段类型现在也可以直接写 `Ref[T]` / `MRef[T]`；因此像 `box.reader.read`、`box.writer.read` 这种“字段后继续点只读方法”的写法也已打通；
11. 返回 `Ref[T]` / `MRef[T]` 时，当前第一阶段已要求它必须可追溯到输入参数；直接返回参数、返回参数 reference 的局部别名都允许，但像 `ref local`、先从局部/临时值借一层再返回、或先 alias 参数后又被局部来源覆盖再返回，这类写法现在会在 borrow 阶段报 `E1004`；  
12. 更完整的值类别语义仍在后续收口中，当前还不是最终版内存模型。  
示例文件：`examples/39_unary_deref.eidos`、`examples/40_unary_ref.eidos`、`examples/41_adt_ref_fields.eidos`、`examples/54_return_borrow_param_alias.eidos`。

### 3.2.1 函数体模式分支的两种一等写法
示例文件：`examples/30_curried_pattern_branch.eidos`

1. 元组头写法：`(p1, p2) => expr`
2. 柯里化头写法：`p1 => p2 => expr`

两者语义等价，编译器会把柯里化头归一为同一分支模式表示；`_` 可用于柯里化段（例如 `NoneString() => _ => NoneString()`）。
补充（2026-04-10）：
1. 柯里化写法里的后续参数现在也能直接带 `when`，例如 `n => i when i > 0 => i`。
2. 后续参数里的构造器模式会按整段保留；`Some(v) => Some(w) => ...` 不会再被错认成普通变量绑定。
3. 函数体模式分支继续和 `match` 共用同一套覆盖分析；像 `Some(v) => f => ...` 这类写法会按 ADT 构造器稳定参与 `W4200/W4201` 推理，stdlib 可直接统一成该风格。

### 3.3 链式调用（自动降级为普通调用）
示例文件：`examples/03_chain_method_calls.eidos`  
链式调用已支持，并在编译阶段自动降级。语义等价规则：

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

### 3.4 ADT 与类型别名
示例文件：`examples/04_adt_type_alias.eidos`

```eidos
type Option[T] { Some(T), None }
type UserId = Int;

maybe :: Some(1);
uid: UserId  := 7;
```

GADT 构造器可以显式声明返回当前 ADT 的特化实例；没有 `->` 的普通 ADT 构造器仍等价于返回声明 head 的完整实例：

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

`TypeEq[A, B]` 是内建擦除类型；首版只提供 `Refl`，用于同一类型或 GADT 模式分支局部精化产生的类型等式证据。

构造器也可以声明局部类型参数。局部类型参数只在该构造器的参数和返回类型中可见；如果没有出现在返回类型中，它就是 existential，构造后会隐藏，只能通过模式匹配在分支内重新打开：

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

构造器命名块只描述运行时字段。`name: Type` 是运行时字段，会进入构造器参数、layout 和 ABI；`name = expr` 不再允许写在 ADT 构造器里。用户自定义 trait 的构造器事实应写在 named instance bridge 中：

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

bridge fact 输入支持字面量、元组、列表、受限一元/二元表达式、值名或路径引用，以及构造器/普通函数调用表达式。生成的 impl 会把这些表达式复制到对应构造器分支中并继续接受正常类型检查，因此可以表达 `delta = GameMath::up_i` 和 `opposite = South()` 这类构造器事实。bridge 支持 `Self -> R` 和 `Self -> A -> R` 这类额外参数方法；返回类型和额外参数中的 `Self` 会替换为目标类型。

当 constructor instance bridge 与 GADT 组合时，生成的泛型 impl 会在每个构造器分支内使用局部类型精化；例如 `North` 分支中只局部知道 `A = Vertical`，该等式不会泄漏到其他分支或分支外。

补充（2026-05-29）：record 风格 ADT 值现在支持短 record update 语法：

```eidos
reset :: state.{ tick: 0 };
```

`.{...}` 前面的 base 表达式提供要复制的 ADT record 值。未显式填写的字段会从 base 值按字段投影复制，后续显式字段覆盖这些复制值。对于拥有多个 record 构造器的 ADT，短更新会保留运行时构造器，并且只在每个 record 构造器都拥有被更新字段时接受。显式构造器形式仍可写成 `GameState { ..state, tick: 0 }`；这种形式中 `..base` 必须在命名构造字段列表开头出现一次。Eidos 不使用 Koka 风格调用式更新，例如 `state(tick = 0)`。

补充（2026-03-26）：泛型构造器参数类型现在会在表达式与模式两条路径同时校验。
1. 在期望 `Option[Int]` 的位置，`Some("x")` 会报错。
2. 当 `value: Option[Int]` 时，`match value { Some("x") => ... }` 会报错。
3. 相关不匹配在 Types 阶段以 `E4000` 报告。

补充（2026-03-15）：函数分支中的构造器模式参数绑定已修复，`TokCons(tok, tail)` 会稳定保留 `ConstructorName=TokCons` 与两个位置绑定参数（不再错误覆盖为最后一个参数名）。

补充（2026-03-16）：构造器模式匹配在后端已进入“真实判别 + 字段绑定”路径：
1. `match` 构造器分支会使用 runtime `type_id` + 稳定构造器 tag 判别；
2. 模式字段变量会在 MIR 中绑定为真实字段读取（不再只是语法保留）；
3. 位置字段 `_0/_1/...` 当前按固定 `8-byte slot` 布局读取。
4. 命名字段会在 MIR 层归一化到 `_index` 槽位后再进入 LLVM lowering（用于收敛字段偏移语义）。
5. ADT 命名字段访问现在要求“在该 ADT 内可唯一定位且对所有构造器总是可用”：
   - 跨构造器索引不一致会报 `E3205`；
   - 仅部分构造器拥有该字段会报 `E3204`；
   - 字段不存在会报 `E3203`；
   - LLVM 侧遇到未解析字段偏移会报 `E3301`（不再静默回落）。

### 3.5 Trait 与 `@impl`
示例文件：`examples/05_trait_impl_declaration.eidos`  
推荐使用 name-first `instance` 声明 trait evidence；旧的 `@impl(Trait)` 函数形式仍可用于与既有代码兼容，但新示例优先展示 `instance`。
`instance` 的成员名与签名必须匹配目标 Trait 方法，并且定义在同模块内。
对于泛型 Trait，`instance` 支持显式 trait 类型实参（例如 `FunctorBox :: instance Functor[Box]`），并会在命名阶段检查实参数量是否匹配。
约定式 impl 注册不会推断泛型 Trait 的类型实参；对泛型 Trait 必须使用显式 `Trait[...]`。
`instance` 会先做 alias canonicalization，再比较 impl 头的结构特化关系。若一个头严格比另一个更具体（例如 `Option[Int]` 相对于 `Option[T]`），两者可以共存；但若两个头 canonicalize 后等价，或彼此不可比较，命名阶段仍会以 `E3004`（`overlapping impl registration`）拒绝。仅靠 alias 改写不会产生“更具体”的关系，所以 trait 实参侧或实现类型侧的 alias-only 等价重叠仍会报错。
`expr given InstanceName` 可显式选择某个命名 evidence。
旧 `@impl(Trait)` 形式要求函数名与签名必须与 Trait 方法匹配，并且定义在同模块内。
对于泛型 Trait，`@impl` 现支持显式 trait 类型实参（例如 `@impl(Functor[Box])`），并会在命名阶段检查实参数量是否匹配。
约定式 impl 注册不会推断泛型 Trait 的类型实参；对泛型 Trait 必须使用显式 `@impl(Trait[...])`。
`@impl` 会先做 alias canonicalization，再比较 impl 头的结构特化关系。若一个头严格比另一个更具体（例如 `Option[Int]` 相对于 `Option[T]`），两者可以共存；但若两个头 canonicalize 后等价，或彼此不可比较，命名阶段仍会以 `E3004`（`overlapping impl registration`）拒绝。仅靠 alias 改写不会产生“更具体”的关系，所以 trait 实参侧或实现类型侧的 alias-only 等价重叠仍会报错。
作为高阶 trait 实参使用的开放别名，现在也会参与类型推断与 MIR 特化时的反向匹配。例如 `@impl(Applicative[KeepEdges[String, Bool]])` 现在可以在外层期望 `Triple[String, A, Bool]` 时满足 `G[A]`；可参考 `examples/43_open_alias_trait_impl.eidos`。
同样的反向匹配现在也能穿过预编译标准库的泛型组合子：`Result::traverse(Ok(2))(produce_keep_edges)` 即使回调返回的是底层 `Triple[String, Int, Bool]`，也能反推出 `G = KeepEdges[String, Bool]`；而 `DeepBoxedResult[String]` 这类深别名链在 direct/helper 两条遍历路径上也都能继续正确特化；可参考 `examples/44_std_traversable_alias_applicative.eidos`。
这一点现在也适用于递归 traversable：`Seq::traverse([1, 2])(produce_keep_edges)` 会在重复特化 `map2_applicative(cons)(...)` 的过程中持续保留用户自定义 open alias / deep alias `Applicative` impl；可参考 `examples/45_std_list_traversable_alias_applicative.eidos`。
这同样适用于短路/空输入分支：`Option::traverse(None())(...)` 与 `Seq::traverse([])(...)` 现在也会通过用户自定义 alias-backed `Applicative` 的 `pure` 正确完成特化，而不再只在回调被真正调用时才可达；可参考 `examples/46_traversable_alias_applicative_empty_cases.eidos`。
`Option`、`Seq`、`Result` 现在也分别提供公开的 `sequence` helper，可把 `Option[G[A]]`、`Seq[G[A]]`、`Result[G[A], E]` 稳定翻转成 `G[Option[A]]`、`G[Seq[A]]`、`G[Result[A, E]]`；当前 alias-backed 覆盖已通过 `KeepEdges[String, Bool]` 这类开放别名锁定，而且内建 `ResultWith[E]` 的同构嵌套现在也能稳定通过 `Option::sequence(Some(Ok(...)))`、`Seq::sequence([Ok(...), ...])`、`Result::sequence(Ok(Ok(...)))`；可参考 `examples/47_traversable_sequence_alias_applicative.eidos` 与 `examples/48_sequence_result_applicative.eidos`。
`Std::Traversable` 现在也公开提供泛型 `Traversable::sequence`，因此调用方不必再先选定外层容器专用 helper。这个泛型入口现在也能在 `Option`、`List`、`Result` 上同时穿过用户自定义 alias-backed applicative 和内建 `ResultWith[E]` 同构嵌套；可参考 `examples/49_generic_traversable_sequence.eidos`。
限定 trait 方法路径现在也已经成为一等可调用值路径。泛型代码里既可以通过导入模块别名直接写 `Applicative::pure`、`Traversable::traverse`，也可以写导入模块下的嵌套 owner 路径（如 `Trait::Eq::eq`），还可以写完整根路径 `Std::Applicative::pure`、`Std::Traversable::traverse`，以及在当前模块内部直接写同名 trait 的相对路径，例如 `Demo.Show :: module { ... }` 内部直接写 `Show::show`；预编译 `Std::Traversable` helper 现在内部就依赖这类模块内相对路径，而 `examples/50_qualified_trait_method_paths.eidos` 现在同时覆盖 alias/root/nested-import 这几种外部调用形态。
限定 effect 路径与普通函数路径独立解析。在 `need` 中使用 `Logger::Logger`、`Io::Writer` 或 `Cap.Io::Writer`；普通函数调用写作 `Logger::log(...)`、`Io::write(...)` 或 `Cap.Io::write(...)`。参见 `examples/51_qualified_effect_paths.eidos` 与 `examples/52_nested_qualified_effect_paths.eidos`。

说明（2026-06-18）：当前分支不再把 `proof` / lawful 相关内容作为教程基线的一部分；相关实验内容移至单独的 proof 分支维护。

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

Trait 约束求解当前行为（2026-03-15）：
1. `Tuple` 类型按结构化规则检查 trait：当且仅当每个元素类型都满足该 trait 时，元组满足该 trait。
2. `TyCon`（构造类型）优先走内置 trait 映射，其次走符号表中的 `impl` 查找。
3. 约束中的 `traitId` 缺失时，会回退按 `traitName` 在符号表中查找 trait 再做 `impl` 匹配。
4. 泛型函数的 trait bound 会在调用点实例化后强制检查（例如 `id[T: Marker] :: T -> T` 在 `Int` 未实现 `Marker` 时调用 `id(1)` 会报错）。
5. Trait 实参匹配采用精确语义：无实参 bound（例如 `T: Functor`）不会匹配仅有特化实参的 impl（如 `@impl(Functor[Box])`）。
6. `@impl` 注册前会先做 alias canonicalization 再检测重叠。严格更具体的头可以与更宽泛的头并存，但等价或不可比较的 canonical 形状仍会报 `E3004`。例如 `@impl(Show)` on `Option[Int]` 可以与 `@impl(Show)` on `Option[T]` 共存；但 `ResultWith[E, T] = Result[T, E]` 与 `AlsoResult[E, T] = Result[T, E]` 若同时用于声明 `@impl(Applicative[...])`，仍会因底层折叠到同一形状而冲突。
7. 泛型 trait bound 中的部分应用 alias 现在可以从具体底层返回类型反推出开放槽位。例如 `lift[A, G: kind2 : Applicative[G]] :: A -> G[A]` 现在可以通过 `KeepEdges[String, Bool] = Triple[String, _, Bool]` 在调用点特化出 `Triple[String, Int, Bool]`。
8. 同样的 alias-backed 反推能力现在也能与预编译标准库 helper（如 `Result::traverse`）组合，所以即使回调签名暴露的是底层构造器类型，用户自定义 open alias / deep alias `Applicative` impl 仍然可达。
9. 对 `Seq::traverse` 这类递归 traversable 也是如此：即使内部会反复经过 `map2_applicative` / `cons` 的特化链，alias-backed `Applicative` impl 仍能保持可用。
10. 对空/短路 traversable 分支也是如此：`Option::traverse(None())` 与 `Seq::traverse([])` 现在也会稳定走到用户 impl 的 `pure`，而不是只覆盖回调生效的那半条路径。
11. 现在 `Option::sequence`、`Seq::sequence`、`Result::sequence` 已作为容器专用 helper 对外可用，而 `Traversable::sequence` 也已作为泛型外层容器版本开放；它们在稳定场景下继承同样的特化行为，因此像 `Option[KeepEdges[String, Bool, A]]`、`Seq[KeepEdges[String, Bool, A]]`、`Result[KeepEdges[String, Bool, A], E]`、`Option[Result[A, E]]`、`Seq[Result[A, E]]`、`Result[Result[A, E], E]` 这类嵌套值都能直接通过标准库翻转。

### 3.5.1 模块 `export` 与 `re-export`
示例文件：`examples/53_module_exports_and_reexports.eidos`  
当前行为：
1. 模块内只要出现任意 `export` 声明，外部可见性就切换到“显式导出模式”；未标记 `export` 的声明继续可在模块内部使用，但不会再自动暴露给外部。
2. 若模块内完全没有 `export`，当前仍保持兼容性的“隐式全部导出”行为。
3. `export` 现在可前缀普通声明与 `import`：例如 `export let`、`export let mut`、`export func`、`export effect`、`export trait`、`export type`、`export import`。
4. `export BaseApi :: import Demo.Base;` 会把模块别名 `BaseApi` 放进当前模块的公开导出表；外部随后可以沿着这个 re-export 的模块别名继续访问公开成员。0.4.0-alpha.1 中模块别名导入使用 name-first 绑定形态 `Alias :: import Module.Path;`；旧 keyword 形式 `import Module.Path as Alias` 只在 `legacy` 语法模式中接受。
5. `export import Demo.Base::{Writer as W}` 会把 selective import 的 alias 一并公开；`export import Demo.Base::*` 则会把 imported module 当前可见的公开绑定整体转发出去。
6. `export import` 现在走真实模块路径和导出表，而不是旧的字符串启发式，因此 alias re-export、qualified path、IDE completion 与预编译导出表都共享同一套可见性语义。

```eidos
Demo.Facade :: module
{
    export BaseApi :: import Demo.Base;
    export import Demo.Base::{Writer as W}
}
```

上面的 `Facade` 会公开模块别名 `BaseApi` 与 effect alias `W`；普通函数需要单独 re-export。完整可运行示例见 `examples/53_module_exports_and_reexports.eidos`。

### 3.6 列表推导式（当前支持子集）
示例文件：`examples/08_list_comprehension.eidos`  
当前状态：
1. AST/NameResolver/Types 已支持多限定符（多 generator + guard）的作用域与类型检查。
2. HIR 已保留专用 `ListComprehension` 结构（不再在 HIR 早期展开为常量列表）。
3. MIR 已支持 CFG lowering（循环 + guard），并通过 runtime `array_new/array_push` 构建结果列表。
4. MIR 已支持“长度运行时求值”的生成器循环（使用 runtime `array_length` 调用）。
5. LLVM lowering 会把索引读写映射为 runtime `array_get/array_set`。
6. CFG join/backedge 的局部值更新已修复为 slot 语义（entry `alloca` + `load/store`），避免循环回边读到初始 alias。
7. Native smoke 已支持 clang-only 链路：编译器会自动注入 `main -> eidos_main` 入口桥接，不再强依赖 `llc`。
8. 非 `VarPattern` 的生成器模式当前被明确禁止，并在 MIR 阶段报告 `error[E5101]`；生成器请使用简单变量或通配绑定。

```eidos
doubled :: [x * 2 | x <- [1, 2, 3]];
```

### 3.7 借用签名推断（CFG 合流）
示例文件：`examples/38_borrow_effect_decoupling.eidos`
当前状态（2026-03-15）：
1. `LoanSignatureInferer` 已升级为 CFG/dataflow 程序点推断，不再仅依赖线性启发式别名。
2. 返回借用约束会聚合所有 `return` 站点，并在分支合流后对参数来源取并集。
3. 这意味着“不同分支返回绑定到不同参数”的函数会得到更准确的 `BoundToParams`。
4. `LoanConstraintVerifier` 的冲突诊断会携带 `alias trace id`，`debug` 状态输出中的 trace 也会带同一 ID，便于快速定位“诊断引用了哪条 alias 链”。
5. `BorrowChecker` 已对齐同样的 `alias trace id` 机制，`FormatBorrowAliasStates` 与借用错误输出可直接互相定位。
6. borrow/loan 冲突诊断现在会额外给出 `alias state lookup` 提示，可直接按 `id=...` 到 `*_borrow_aliases` 或 `*_loan_constraint_states` 调试输出反查状态。
7. 借用目标已升级为路径敏感模型（`BorrowTarget = BaseLocal + PathKey`），字段/常量索引可区分不同子路径：例如借 `x._0` 后写 `x._1` 不再误报冲突。
8. 对不确定索引（如 `x[i]`）仍采用保守冲突策略，避免不安全放宽；对常量索引（如 `x[0]`/`x[1]`）保持精确区分。
9. `base` 与 `base/deref` 已按不同内存域处理，减少”指针变量本身写入”与”pointee 借用”之间的误报冲突。
10. borrow 阶段新增 capability gate：共享 `MirLoad` 需要 `read`；可变借用 `MirLoad` 与 `MirStore` 需要 `write`；`MirMove` 需要 `move`。
11. 能力不足诊断与借用冲突诊断已分离，新增错误码：`E1011`（read 能力不足）、`E1012`（write 能力不足）、`E1013`（move 能力不足）。
12. Borrow 授权与 effect 行独立；effect 声明即使带有 `@borrow(...)` 也不会授予 `read/write/move`。
13. Borrow 权限由 borrow/ownership 流水线推导和约束，而不是由 `need` 提供。

### 3.8 Effect Tag、授权与多态
Effect 声明是 nominal 编译期标记，不包含 operation，也没有运行时表示：

```eidos
Writer :: effect;

write :: String -> Unit need Writer
{
    _ => ()
}
```

1. 调用方必须在 `need` 中声明所调用函数需要的全部 effect；缺少授权会报告 `E3003`。
2. Effect 不拥有函数。`Io::Writer` 等限定 effect 路径只用于 `need`；普通函数按模块路径调用，例如 `Io::write(text)`。
3. 高阶 API 使用 `E: effects` 行参数：`apply[A, B, E: effects] :: (A -> B need E) -> A -> B need E`。
4. 固定行与多态行可组合，例如 `need FFI, E`。Effect 变量会参与泛化，并保存在跨模块摘要和编译缓存状态中。
5. Effect 在运行前擦除；语言不提供 handler、`with`、`resume`、CPS 重写或运行时 effect dispatch。
6. Borrow 检查与 effect 授权独立；在 effect 上标注 `@borrow(...)` 不会授予 read、write 或 move 权限。

### 3.10 `match when` 分支守卫（已打通到 MIR）
示例文件：`examples/10_match_guard.eidos`、`examples/27_pattern_guard_binding.eidos`  
补充（2026-03-16）：`pattern when guard => expr` 现在不再只停留在类型阶段，已在 HIR/MIR 贯通：
1. HIR 分支会保留 `Guard` 表达式；
2. MIR 会按分支顺序生成可执行条件链，`guard=false` 会继续尝试后续分支；
3. 构造器/元组子模式会生成真实匹配条件，不再被统一降级为默认分支。
4. `when` 后可直接使用简单标识符表达式（例如 `v when v => ...`），不会再丢失 guard。
5. 现已支持 Haskell 风格 pattern guard：`when pat <- expr`。其中 `pat` 绑定仅在当前分支体内可见，不会泄漏到其它分支。
6. 该语义在 `match` 分支和“函数体模式分支”两种入口都可用，MIR 会生成独立 guard 流并在 guard 失败时继续尝试后续分支。
7. `when pat <- expr` 里的 `pat` 现在是完整模式，不再只限构造器/列表/`_`；元组、`or/and/not`、`view` 等模式都可放进去。
8. 守卫和普通表达式里的 `&&` / `||` 现在按短路求值；左边已经决定结果时，右边不会再继续执行。

### 3.11 ViewPattern（标准语法 `(expr -> pattern)`）
示例文件：`examples/11_advanced_patterns.eidos`

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

当固定参数选择一个分类器、被匹配值提供剩余参数时，view pattern 也很适合：

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

当前实现状态（2026-03-17）：
1. `view-pattern` 标准写法为 `(expr -> pattern)`，语义是“先求值 `expr` 得到可调用值，再调用 `expr(scrutinee)`，并用 `pattern` 匹配返回值”。
2. 仅支持标准写法 `(expr -> pattern)`；`view(...)` / `View(...)` 已移除。
3. `or-pattern`（如 `1 | 2`）、`and-pattern`（如 `(p1) & (p2)`）、`not-pattern`（如 `!0`）与 `range-pattern`（如 `3..5`）已接入 parser/hir/mir 主链路。
4. `or-pattern` 支持同名绑定传播到分支体（例如 `(1 as n) | (2 as n) => n` 与 `TokA(n) | TokB(n) => n`）。
   若绑定名不一致，诊断会指出具体 alternative 的 `missing/extra` 细节，便于快速定位。
5. `and-pattern` 支持跨 conjunct 合并“不同变量名”的绑定（例如 `Pair(a, _) & Pair(_, b) => a + b`）；同名重复绑定会报错，避免绑定来源歧义。
6. `not-pattern` 不允许引入绑定；`!(1 as n)` 会报错，避免“否定分支里变量来源不确定”。
7. 同一分支内会复用 view 调用结果，避免“判定阶段一次 + 绑定阶段一次”的重复调用。
8. HIR/MIR 已补齐 `ViewPattern`、`OrPattern`、`AndPattern`、`NotPattern`、`RangePattern` 对应 lowering 路径。
9. 同一模式作用域中重复绑定同名变量会报错（例如 `Pair(n, n)`），避免“后绑定覆盖前绑定”的隐式歧义。
10. `or/and/not` 相关模式诊断会附带 `context` 路径（例如 `branch#1 > and-pattern`）以及 `alternative#/conjunct#` 细节；构造器、tuple、`as`、`view` 等递归路径会附带 `positional#N` / `field#name` / `element#N` / `as-inner` / `view-inner` 子路径（`range` 已预留 `start/end` 钩子）。
11. `or-pattern` / `and-pattern` 在 MIR 阶段已按短路语义 lowering，右侧模式不会被无条件求值。
12. 字符串 `==/!=`（包括模式字面量比较）统一走 runtime `string_equals`，语义是“按内容比较”，不是指针身份比较。
13. 构造器形状校验现在默认严格执行：会检查位置参数个数、命名字段合法性，以及 named/positional 形态是否匹配（包括零参数构造器）。
14. `range-pattern` 会在类型阶段做边界有序性校验；如 `5..3` 或 `'z'..'a'` 会报错（`start <= end` 约束），并在诊断中分别标注 `start` 与 `end` 边界位置（`E4011`）。若 scrutinee 不是 `Int/Char`，会报 `E4012`。
15. `as-pattern` 现在会继承并校验被匹配值类型；例如在 `Int` scrutinee 上写 `("a" as s)` 会稳定报 `E4013`（as-pattern 类型不匹配），并附带 `as-pattern binding` / `as-pattern inner pattern` 标签与 scrutinee 类型 note。
16. `view-pattern` 左侧表达式支持一般表达式形态（如 `if ... then ... else ...`），但语义要求该表达式在当前类型上下文下可调用为一元函数 `expr(scrutinee)`；若不可调用，会给出专用诊断 `E4014`（view expression invalid），并附带 scrutinee/view-expression 类型说明。
17. 部分应用调用是表达固定参数 view 的推荐方式，例如 `(key_pressed(81) -> true)`。对 FFI/输入轮询，优先先读取或分类一次事件，再匹配稳定值；不要把多次有副作用的轮询分散到许多 pattern 里。
18. 名称解析阶段新增模式覆盖告警（不阻断编译）：
   - `W4200`：模式匹配不穷尽（例如只匹配 `true`，或 ADT 漏掉构造器）；
   - `W4201`：不可达分支（前面已有“无 guard 的不可反驳分支”时，后续分支不可达）。
   - `W4200` 在 `Bool` 场景会直接给出缺失分支明细（如 `missing bool cases: false`）。
   - `W4200` 现在会额外附带 `Missing-case witnesses: ...` note，给出最小缺失路径示例（Bool/ADT 先行覆盖）。
   - `W4200` 还会附带 `Missing-case traces: ...`，以 `display [stable-key]` 形式输出结构化 witness（例如 `false [bool:false]`、`None [ctor:123]`）。
   - 同时会附带 `Missing-case trace groups: ...`，按 witness 空间分组（`bool` / `tuple-bool` / `ctor` / `wildcard`），并采用“稳定键优先、展示文本可选”格式（如 `bool=bool:false`、`ctor=ctor:123 (None)`）。
   - 另附 `Missing-case trace kv: ...` 机器可解析键值段，单项格式为 `kind=<...>;key=<...>;display=<...>`，多项以 ` || ` 分隔（保留转义）。
   - 对 `tuple` 的 `Bool` 组合（如 `(Bool, Bool)`）也会给出缺失组合诊断（`missing tuple bool cases`）与对应 witness note。
   - `tuple Bool` 的覆盖分析现在支持顶层组合模式枚举（`or/and/not/as` 组合），例如 `(true, true) | (true, false)`、`!(false, false)` 这类写法也会进入精确缺失/覆盖推理。
   - 若存在无法静态证明的 `when` guard 且触发了 `W4200`，会附带 note 说明“guard 分支不会被当作穷尽覆盖”。
   - 覆盖分析对常量 guard 有折叠：`when true` 按无 guard 处理并计入覆盖；`when false` 不计入覆盖。
   - `when false`（含可折叠为 false 的布尔组合，如 `true && false`）会额外触发 `W4201` 不可达分支告警。
   - 对可精确枚举的有限模式空间（当前包含 `Bool`、`tuple Bool` 与顶层 ADT 构造器集合），若某分支不再新增覆盖（例如重复 `true`、`(true, _)` 之后再写 `(true, false)`、或重复 `Some(...)` 构造器分支），会触发 `W4201`，并附带 `Covered-case witnesses: ...` 说明该分支被哪些前序分支覆盖。
   - 上述 `W4201` 现在还会追加 `Covered-case traces: ...`，给出 `witness <- #branch` 的覆盖来源映射，便于直接定位是哪条前序分支造成遮蔽。
   - 若某条 covered trace 来源于“guarded unresolved 分支的已证明下界”，`W4201` 会额外追加 `Covered-case lower-bound traces: ...`，直接标注该 trace 的 lower-bound provenance。
   - 对上述有限空间，若组合模式本身可证明“匹配集为空”（例如 `true & false`、`(true, false) & (false, true)`、`Some(v) & !Some(_)`），也会触发 `W4201`（`pattern is unsatisfiable in finite coverage space`）。
   - ADT 的 `Covered-case witnesses` 会优先保留分支模式形状提示（如 `Some(...)`、`Node{...}`），定位“被覆盖的是哪个构造器形态”更直观。
   - ADT 顶层组合模式（`or/and/not/as`）现在也会进入精确覆盖特化：例如 `!None`、`Some(x) & !None` 这类写法可参与缺失构造器与覆盖不可达推理。
   - 对变量模式的简单布尔等式 guard（`x == true/false`、`x != true/false`），覆盖分析会计入对应 bool 分支。
   - 对布尔逻辑短路也会做三值推理：例如 `x || true` 视为恒真（按无 guard 覆盖），`x && false` 视为恒假（触发 `W4201` 不可达）。
   - 变量模式谓词 guard（如 `v when v => ...`）会被识别为可证明覆盖，不会再误报非穷尽。
   - 在 `match x` 中，若分支是不可反驳模式（如 `_`）且 guard 直接引用 `x`（例如 `_ when x`），覆盖分析也会把它识别为可证明覆盖。
   - 在 `match x` 中，若分支绑定了变量但 guard 用的是 subject 名（例如 `v when x`），也会按同一布尔来源推理覆盖。
   - 在 `match x` 中，若分支是单一布尔字面量模式（如 `true`）且 guard 直接引用 `x`（例如 `true when x`），也会按该字面量分支计入可证明覆盖。
   - `when !x` 这类一元 guard 表达式现在会稳定保留为 `UnaryExpr(Not, x)` 进入语义分析，不再被压扁为未知 guard。
   - 在布尔字面量分支上会做 pattern-aware guard 恒值判定：例如 `true when !x` 可判为恒假并触发 `W4201`（同时 `W4200` 会提示缺失 `true` 分支）。
   - 在 `match x` 中，若分支模式可精确枚举布尔取值（例如 `true | false`），覆盖分析会按每个 case 分别评估 guard；例如 `true | false when x` 仅计入 `true` 覆盖。
   - 对上述“可精确枚举”的布尔模式，若 guard 在所有可匹配 case 上都恒假（例如 `true | false when !x && x`），会触发 `W4201`（guard 恒假不可达）。
   - 对可精确枚举的 `tuple Bool` 模式，覆盖分析也会按“每个 tuple case + 模式绑定布尔变量”的组合逐 case 评估 guard：例如 `(a, b) when a` 可只计入首元素为 `true` 的组合；`(a, b) when a && !a` 会判定为空匹配并触发 `W4201`。
   - 对 `tuple Bool` 的 guarded unresolved 分支，若部分 tuple case 已可证明 guard 为真（其余 case 不可证明），这些“已证明下界”现在也会参与 `covered` 诊断；不可证明部分仍保持保守，不会放大为误报。
   - 对“可精确枚举构造器集合”的顶层 ADT guarded 分支，覆盖分析会按“每个构造器 + 可证明布尔绑定”评估 guard：例如 `Some((true as f)) when f` 可计入 `Some` 覆盖；`Some((false as f)) when f` 会判定为该构造器空覆盖并触发 `W4201`。
   - 上述 ADT guarded 分支现在也会利用可证明的整型绑定约束（含 `as` / `view-inner` / 有限集合）：例如 `Some((normalize -> (1 as n))) when n == 1` 可计入 `Some` 覆盖；`Some((normalize -> ((1 as n) | (2 as n)))) when n == 3` 可判定为空匹配并触发 `W4201`。
   - `Char` 字面量也会进入同一 finite 约束路径（按离散值处理）：例如 `Some((normalizeChar -> ('a' as c))) when c != 'a'` 会被识别为空匹配并触发 `W4201`。
   - 对 `as` 绑定包裹 `not` 组合的整型模式（如 `((!2) as n)`、`((!2 & !3) as n)`），分析会把 `n` 视为 `other(exclude {...})`：`when n != 2` 可被证明恒真，`when n == 2` 可被证明恒假并触发 `W4201`。
   - 在 ADT 场景下，外层嵌套 `as` 绑定也会继承内层 `other(exclude {...})` 约束（例如 `Some(((normalize -> ((!2 & !3) as n)) as m)) when m == 2`），可直接触发可证明空匹配 `W4201`。
   - 在 ADT 的“按构造器有限域”覆盖分析中，若覆盖来源分支是 guarded 且依赖可反驳（refutable）的 `view` 负载约束，后续构造器/字面量分支的 `covered` 型 `W4201` 会保守抑制，避免因构造器粒度过粗导致误报（`irrefutable view` 仍保持精确 covered 诊断）。
   - 上述 ADT 抑制策略进一步收窄：若 guarded refutable-view 来源分支本身在 `or` 中含有“不可反驳且不含 view 的兜底备选”（例如 `Some(((normalize -> 1) | _)) when true`），则保持精确 covered 诊断（不再抑制对应 `W4201`）。
   - 该“兜底备选”必须对整个 payload 模式形成结构性支配。若仅出现在受限 `and` 子式内部（例如 `Some(((normalize -> 1) & (_ | 1)))`），不会关闭保守抑制。
   - 该“兜底备选”还必须与目标构造器相关：在 `Some(refutable-view) | None` 中，`None` 备选只会放开 `None` 目标的 covered 诊断，不会再错误放开 `Some` 目标的保守抑制。
   - 对 `Some((<uncertain-view>) | <non-view>)` 这类 mixed uncertain-view 场景，若 `<non-view>` 能对“目标分支字面量”形成可证明命中（例如目标是 `Some(3)` 且备选含字面量 `3`），则会恢复该目标分支的精确 `W4201 covered`；若无法证明命中（例如目标 `Some(4)` 仅有备选 `3`），继续保持保守抑制。
   - 上述“可证明命中”已扩展到有限整型集合目标（如 `Some(3..4)` 或 `Some(3 | 4)`）：只有当 non-view 备选可覆盖该目标集合中的每个值时才恢复 `W4201 covered`；否则仍保守抑制，避免部分命中误判为全覆盖。
   - deterministic 目标约束现在同时支持 `Int` 与 `Bool` 字段：例如 `Pair{flag: true | false, value: 3..4}` 这类跨类型 finite target，也可参与 covered 恢复判定。
   - 上述恢复判定同样适用于命名字段与深层组合子场景：例如 `Some{value: ...}` 的字段约束，以及 `Some(_) & Some(3..4)` 这类 `and` 中顺序交换的目标模式，都会按“有限集合全覆盖”规则判定是否恢复 `W4201 covered`。
   - 对 `or` covering 分支，恢复判定支持“多备选联合覆盖同一 finite target”：当单个备选无法独立覆盖目标集合（如一个命中 `3`、另一个命中 `4`），但它们在目标约束赋值空间上的并集可全覆盖时，也会恢复 `W4201 covered`；若仅部分联合覆盖则继续保守。
   - 上述 finite target 约束域现已扩展到 `Char` 字面量（按有限离散值处理）：例如目标 `('a' | 'b')` 可在多备选并集全覆盖时恢复 `W4201 covered`，仅部分命中时继续保守，避免误报。
   - 对 ADT `guarded refutable-view` 且含 `as` 绑定的场景，现在会按“目标分支 finite profile + 绑定收敛后的 guard 真值”补充 covered 判定：例如 `((normalizeChar -> ('a'..'b')) | 'b') as t when t == 'b'` 可恢复后续 `tag: 'b'` 分支的 `W4201 covered`；若 guard 在该 profile 下不可证明为真，仍保持保守。
   - 上述 ADT covered 恢复路径现已扩展到 `top-level or/and` 组合，并补齐命名字段简写绑定（`Pair{right}`）在 target profile 下的约束收敛：`((CtorA & CtorB) | CtorC) when ...` 这类分支只要能证明“命中目标 profile 的备选”下 guard 恒真，也可稳定恢复后续分支 `W4201 covered`；无法证明命中的备选仍保持保守。
   - 对有限目标 profile（如 `tag: ('a' | 'b')`），guard 证明现在会按 assignment 逐项求值再汇总：`t == 'a' || t == 'b'` 这类“并集恒真”约束可恢复 `W4201 covered`；仅对部分 assignment 成立（如 `t == 'a'`）时仍保持保守，不会误报 covered。
   - 在 `or` 备选中，若某个备选可被证明“对当前目标 assignment 不可命中”（例如 `!((normalizeChar -> 'b'))` 对目标 `'b'`），它不会再阻断 guard 绑定收敛；其他可确定命中的 non-view 备选仍可恢复 `W4201 covered`，无确定命中时继续保守。
   - 上述“不可命中备选跳过”规则同样适用于 Bool 域：例如目标为 `flag=true` 时，`flag=false` 或与该 assignment 不兼容的 `!((normalizeBool -> true))` 备选不会阻断可命中备选的 covered 恢复；若不存在可命中备选，仍保持保守不报 covered。
   - 针对 ADT mixed uncertain-view 的恢复路径，covering 分支现在按组合子结构递归校验：`and` 必须逐 conjunct 同时满足目标约束；若 covering 在目标未约束字段上仍有额外可反驳限制（非 irrefutable non-view fallback），不会恢复 `W4201 covered`，以避免 split-conjunct / extra-field 造成的误报。
   - 当发生上述保守抑制且最终触发 `W4200` 时，`W4200` 现在会追加 `Conservatively suppressed covered warnings: ... (reason: adt-guarded-refutable-view)`，用于追踪哪些 covered 冲突被保守吞掉。
   - suppression trace 现在会在分支映射后附加细粒度标签（`{reason=...}`），例如 `adt:refutable-view`、`guard:not-provable`、`adt:view-inner-nonfinite@...`、`adt:deterministic-assignment-overflow`，便于快速定位是“guard 不可证”、“view-inner 非有限”，还是“deterministic 赋值枚举超限”导致的保守抑制。
   - 对 list 的 uncertain view-only 覆盖来源，`W4200` 也会追加 suppression trace note，作用域 reason 为 `list-guarded-uncertain-view`，并携带分支标签（例如 `list:no-deterministic-nonview-hit`）。
   - 上述 list 抑制判定现在同时覆盖 `Int/Char token-vector` 与 `Bool vector` finite case：例如 `[((normalizeBool -> true)) | ((normalizeBool -> false))]` 这类 bool-view-only guarded 覆盖来源，也会按同一保守路径避免误报 `W4201 covered`。
   - 在 bool mixed uncertain-view 场景下，若同层 non-view 备选可对目标 token 做确定命中（例如 `(... | false)` 对 `[false]`），会恢复精确 `W4201 covered`；若 non-view 备选无法命中目标 token（例如 `(... | true)` 对 `[false]`），则继续保守抑制。
   - 对纯 `Bool` 目标也新增了同一条 deterministic 恢复路径：例如 `((!(normalize -> true)) | true) when true` 可稳定覆盖后续 `true` 分支并触发 `W4201 covered`；若 guard 不可证明（如 `when probe(x)`），则保持保守，不会误报 covered。
   - 对纯 `Bool` 的 deep `not` mixed uncertain-view 边界也已收敛：当 `not` 内层可由 non-view 子约束证明 `no-match`（如 `!((normalize -> true) & false)`）时会恢复 `W4201 covered`；若内层仍为 `Unknown`（如 `... & true`），则继续保守，不误报 covered。
   - 对 `tuple-bool` 目标同样补齐了 mixed uncertain-view 的 deterministic 恢复：例如 `(((normalize -> true) | true), false) when true` 可覆盖后续 `(true, false)` 分支并触发 `W4201 covered`；guard 不可证明（如 `when probe(t)`）时仍保持保守，不误报 covered。
   - 对 `tuple-bool` 的 deep `not` mixed uncertain-view 边界也已收敛：当 `not` 内层可由 non-view 子约束证明 `no-match`（如 `!((normalize -> true) & false)`）时会恢复 `W4201 covered`；若内层仍为 `Unknown`（如 `... & true`），则继续保守，不误报 covered。
   - suppression reason 现在新增目标域与 uncertain 路径标签：`list:target-domain-int` / `list:target-domain-bool` 与 `list:view-inner-uncertain@...`，用于区分“哪个 finite 域触发保守路径”以及“哪个 view-inner 子路径不可确定命中”。
   - 当 list 因 deterministic non-view 未命中而进入保守抑制时，现在还会追加按 case 的标签（如 `list:no-deterministic-nonview-hit-case1`），可直接定位“哪个 finite target case 没有找到可证明命中的 non-view 路径”。
   - 同时，list case 标签会附带稳定 key（如 `list:no-deterministic-nonview-hit-case1-key:int:3`、`...-key:bool:true`），便于在多 case 场景中快速对齐具体目标 case。
   - 当目标域是 `Char` 时，list case key 现在会优先使用字符字面量（如 `list:no-deterministic-nonview-hit-case1-key:char:'a'`），而不是整数编码，便于直接阅读与定位。
   - ADT suppression 现在也会输出同类标签：`adt:target-domain-int` / `adt:target-domain-bool` 与 `adt:view-inner-uncertain@...`，用于快速区分目标构造器约束域与不可确定的 view-inner 子路径。
   - ADT 在 deterministic non-view 恢复失败（且非 assignment-overflow）时，现在会额外输出 `adt:no-deterministic-nonview-hit`，便于区分“求解上限溢出”与“确实无 deterministic 命中”的保守回退来源。
   - 在该场景下还会追加构造器粒度标签：`adt:no-deterministic-nonview-hit-ctor<id>` 与 `adt:no-deterministic-nonview-hit-ctor-name:<CtorName>`，可直接定位是哪些构造器未形成 deterministic non-view 命中。
   - 现在还会继续细化到构造器 case 级标签：`adt:no-deterministic-nonview-hit-ctor-name:<CtorName>-caseN` 与 `...-caseN-key:...`，用于在同一构造器下定位“哪个 finite profile case”未形成 deterministic non-view 命中。
   - 当目标分支是 `Char` 有限域时，unresolved/suppression note 现在也会补充 `list:target-domain-char` / `adt:target-domain-char`（并保留既有 `*-target-domain-int` 兼容标签），便于在 Int/Bool/Char 混合场景中快速分辨保守路径落在哪个域。
   - 对 `Char` 场景若 finite-int 域提取失败（例如较宽的字符区间），unresolved reason 现在仍会保留 `list:target-domain-char` / `adt:target-domain-char`，避免 explain note 因域提取退化而丢失 `char` 语义可观测性。
   - ADT 的 deterministic non-view 命中恢复已扩展到 `and` 深层组合子：当某个 conjunct 内部虽然含 uncertain view，但同层仍存在可确定命中的 non-view 子路径（例如 `(((normalize -> 1) | 2) & _)` 对目标 `2`），现在会恢复精确 `W4201 covered`；若 non-view 子路径无法命中目标值，则继续保守抑制。
   - deterministic non-view 恢复路径现在也覆盖 deep `not` mixed uncertain-view：当 `not` 内层可由 non-view 子约束证明 `no-match`（例如 `!((normalize -> (2..3)) & 2)` 对目标 `3`）时，会恢复精确 `W4201 covered`；若内层仍含不可消除的不确定命中（同例对目标 `2`），则保持保守，不误报 covered。
   - 针对 `guarded + as` 的 deterministic 绑定收敛，`not` 现在也支持“inner 含 view 但可证明 `no-match`”的子集：例如 `((!((normalizeBool -> true) & false)) as f) when f` 或 `((!((normalizeChar -> 'a') & 'c')) as t) when t == 'b'` 可恢复后续分支 `W4201 covered`；若 inner 仍是 `Unknown`（如 `... & true` / `... & 'b'`），继续保守不恢复 covered。
   - 对 list guarded + refutable-view 分支，现新增 deterministic 补充 covered 恢复路径：即使主覆盖分析因 split-domain 下界不足未产出 `W4201`，只要覆盖分支可在目标 finite case 上做 deterministic non-view 命中且 guard 可证明为真，仍会补发精确 `covered` 告警。
   - 对 list 的 `not(refutable-view)` 覆盖抑制入口也已收敛：当目标分支无法通过 deterministic non-view 路径命中时会继续保守抑制，避免 `!((normalizeChar -> 'a') & 'b')` 这类 uncertain inner 场景误报 `W4201 covered`。
   - 上述补充恢复路径现在也支持 **多分支并集覆盖**（list + ADT）：当不同 guarded refutable-view 分支分别命中目标 finite case 的不同子集时，只要并集可覆盖全部目标赋值，后续目标分支也会恢复 `W4201 covered`。
   - 对这类“并集恢复”的 covered 分支，`W4201` 现在也会补充 `Covered-case witnesses` / `Covered-case traces`，可直接看到多来源覆盖（例如同时出现 `... <- #2` 与 `... <- #3`）。
   - 上述并集/部分并集边界现已在 `Char` 有限域（list + ADT）做对称锁定：并集全覆盖会恢复 covered 并附多来源 trace，部分并集仍保持保守（不误报 covered）。
   - `W4200` 还会追加机器可解析 note：`Suppressed-covered trace kv: kind=...;branch=...;covering=...;reason=...`（多条记录用 ` || ` 分隔，字段分隔符按 `\` 转义；`reason` 中仍用 `,` 分隔标签，若标签内自带逗号会转义为 `\,`），便于 IDE/工具侧做结构化解析与聚合。
   - 若上述 finite case 的 guard 里仍存在不可证明项（例如调用/外部谓词），分析会保守降级为“非精确覆盖”，不会把该分支误判为 `W4201` 空匹配；若最终触发 `W4200`，会附带 guarded note 解释该保守处理。
   - 上述保守路径在 `W4200` 中会额外标注具体分支号（`#N`），方便直接定位是哪些 guarded 分支被按“不可证明”处理。
   - `W4200` 还会追加 `Unresolved-guard branch hints: ...`，给出 `#N@line:col` 与可推导的 lower-bound case（未知时显示 `?`），用于快速定位 guard 证据不足的分支。
   - 对 ADT 场景，若模式可确定构造器集合，即使 guard 不可证明，lower-bound 也会尽量给出构造器提示（如 `[Some]`），减少 `?`。
   - 对 `Bool` / tuple-bool 场景，若模式本身可推导有限 case（即使 guard 仍不可证明），lower-bound hint 也会优先显示这些 case（如 `[true]`、`[(true, false)]`），不再一律退化为 `?`。
   - 对 list 场景，lower-bound hint 现在优先使用分支内已证明的 list case；当仅有长度形状（如 `[_]`）时，会尝试基于 deterministic non-view 路径细化出可证明的 bool 向量（如 `[[true]]`），进一步减少“有信息但只显示形状”的提示退化。
   - 对 `Char` 列表 finite case，unresolved lower-bound hint 现在会优先以字符字面量展示（如 `['a']`、`['b']`），不再退化为数值编码（如 `[97]`、`[98]`），便于直接定位分支语义。
   - 对 ADT guarded unresolved 分支，若部分构造器已可证明 guard 为真（其余构造器仍不可证明），这些“已证明下界”现在也会参与 `covered` 诊断：例如可稳定把后续同构造器分支判定为已覆盖，同时未证明部分仍保持保守。
   - 对列表与 ADT 中 `view` 相关的保守降级，`reason=` 里会附带可定位子路径（如 `list:view-inner-nonfinite@pattern/list-element#1/view-inner`、`adt:view-inner-nonfinite@pattern/positional#1/view-inner`），便于快速追踪是哪个子模式导致 finite 求解退化。
   - list unresolved hint 现在在 bool 域也支持模式回退兜底：当列表 case hint 无法直接给出 bool 向量（例如 mixed alternative 退化为 `[_]`）时，仍会递归从模式提取 bool 候选并输出 `list:target-domain-bool` 与 `list:view-inner-uncertain@...` 路径标签。
   - 对 ADT deep `not` + uncertain-view 的 unresolved 分支，`reason=` 现在也会稳定输出 `adt:view-inner-uncertain@...` 路径标签（如 `adt:view-inner-uncertain@pattern/positional#1/not-inner/conjunct#1/view-inner`）；并会按可推导域补充 `adt:target-domain-int` 或 `adt:target-domain-bool`，同时保持 `guard:not-provable` 优先，兼容既有诊断断言与工具解析契约。
   - 对 tuple-bool 的 unresolved 分支也已对齐同类可观测性：`reason=` 会补充 `tuple-bool:target-domain-bool` 与 `tuple-bool:view-inner-uncertain@...`（例如 `tuple-bool:view-inner-uncertain@pattern/tuple#1/not-inner/conjunct#1/view-inner`），用于定位 tuple 元素内的 deep `not` uncertain-view 降级来源。
   - 对 Bool 的 unresolved 分支也已对齐：`reason=` 现可包含 `bool:target-domain-bool`，以及 `bool:view-inner-nonfinite@...` / `bool:view-inner-uncertain@...` 路径标签（例如 `bool:view-inner-uncertain@pattern/not-inner/conjunct#1/view-inner`），用于定位布尔域中的 view-inner 保守降级来源。
   - 现在已新增“同一 `W4200` 同时包含 unresolved-guard hint 与 suppression trace”的锁定回归（list/ADT），用于防止后续重构把两类 explain note 的其中一类意外丢失。

### 3.12 List/Rest Pattern（列表模式）
示例文件：`examples/16_list_rest_pattern.eidos`、`examples/17_list_guarded_coverage.eidos`、`examples/18_view_guard_unsat.eidos`、`examples/19_view_guard_finite_set.eidos`、`examples/20_guard_algebra_unsat.eidos`、`examples/23_view_guard_other_and_not_unsat.eidos`、`examples/24_view_guard_nested_as_other_unsat.eidos`、`examples/25_view_guard_mixed_view_nonview_conservative.eidos`  
当前支持以下模式形态（已贯通 Parser/Ast/HIR/NameResolver/Types/MIR）：

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

语义补充（2026-03-19）：
1. MIR lowering 会先执行长度判定，再执行元素匹配，避免索引越界读取；
2. `..rest` 在 MIR 中会绑定为“后缀列表切片”，并通过运行时列表 API 物化；
3. `[..]` 表示“仅剩余匹配标记，不绑定变量”，常用于兜底列表分支。
4. 模式覆盖分析已支持列表长度形状与顶层组合模式：会对 `[]` / 固定前缀 / `[..]` 以及 `or/and/not/as` 组合给出 `W4200/W4201`（`missing list cases`、空匹配分支与覆盖分支追踪）。
5. 在可证明 Bool 元素模式下，列表覆盖会进一步细化到元素级 witness（例如 `[true]` / `[false]`），否则会保守回退到长度形状分析。
6. 对 guarded 列表分支，若可在有限 case 上逐项证明 guard（例如 `[x] when x` 与 `[x] when !x`），覆盖分析会把它们计入精确覆盖；若某分支在 finite case 上为空匹配，则会触发 `W4201`（`pattern is unsatisfiable in finite coverage space`）。
7. 列表 `Int` 元素覆盖已从“仅长度 1”扩展到“固定多元素长度（当前上限 2）”：`[1, 2]`、`[1..2, 3..4]`、`[1 as a, 2 as b] when a == 1 && b == 2` 这类分支会进入元素级 finite-case 推理，并稳定产出覆盖/不可达诊断。
8. `Int + rest` 组合现在也可进入精确推理：如 `[1, ..]` 不会再错误吞掉 `[2, 1]`；`[1 as x, ..] when x != 1` 可稳定识别为空匹配并触发 `W4201`。
9. 对 `view` 元素模式，当前已支持“不可反驳内层模式”的有限推理（如 `[(normalize -> _)]`）：这类分支可参与列表 finite-case 覆盖判定，并可触发后续字面量分支的 `W4201` 覆盖不可达；refutable 的 view 内层模式仍按保守策略处理（不误报覆盖）。
10. 对 `view` 内层模式，已新增“可证明代数边界”子集：若内层在有限域上可证明“全匹配”或“空匹配”（如 `(1 | !1)`、`(1 & !1)`），也可进入 list finite-case 推理并稳定触发覆盖/不可达（`W4201`）。
11. guarded 列表分支现在会把 `view-inner` 的可证明绑定继续传播到 guard：例如 `[(normalize -> (true as b))] when !b`、`[(normalize -> (1 as x))] when x != 1` 会识别为空匹配并触发 `W4201`；若 `view-inner` 约束无法静态证明，仍保守降级避免误报。
12. 对 `view-inner` 的 `as` 绑定，guard 求值已支持有限整数集合约束（例如 `((1 | 2) as x)` 与 `((1 as x) | (2 as x))`）：`when x == 3`、`when x < 0` 这类可证明矛盾条件会被识别为空匹配并触发 `W4201`。
13. guard 代数推理新增两类保守可证明规则：  
   - 有限整数集合上的算术比较（例如 `x + 1 == 4`，当 `x ∈ {1,2}` 时可判恒假）；  
   - 布尔互补恒值（例如 `b && !b` 判恒假、`b || !b` 判恒真），即使 `b` 不是模式绑定变量，也可用于 guarded 分支可达性判断。
14. `view-inner` 的有限集合提取已扩展到 `and/not` 混合组合：例如 `((1 | 2) & !2)` 会被收敛为 `{1}`，因此 `[(normalize -> ((((1 | 2) & !2) as x)))] when x != 1` 可稳定识别为空匹配并触发 `W4201`。
15. 列表元素模式中的“组合子内部 view”现在支持 `Always/Never` 子集推理：例如 `[((normalize -> (1 | !1)) | 2)]` 中的 `view` 子模式可被识别为恒匹配，从而参与覆盖/不可达分析；无法证明为恒真/恒假的 nested view 仍保持保守降级，避免误报。
16. Parser 对 `..` 的边界错误提示已统一：`..` 仅允许出现在列表模式中，且 `..rest` 必须位于列表模式尾部。像 `(a, ..rest)`、`Some(..rest)`、`[a, ..rest, b]` 这类写法会给出带修复建议的 `E4000` 提示（合法示例：`[a, ..rest]`、`[..rest]`、`[..]`）。
17. 对“同层组合子里同时存在 refutable view 与非 view 子模式”的场景（例如 `((normalize -> (1 as x)) | 3..4)`），finite-case 覆盖推理默认保持保守：避免把后续字面量分支错误判定为“已覆盖”，优先消除 `W4201` 误报。
18. 列表/view 的 Int 有限域 token 匹配现在改为三值求值（`match` / `no-match` / `unknown`）并接入 `or/and/not`：在 `or` 场景中，除了不可反驳的非 view 备选（如 `((normalize -> (1..2)) | _)`）外，若存在“对当前 token 可确定命中”的非 view 备选（如 `(((normalize -> (1..2)) | 2) as x) when x == 2`），也可恢复精确 covered 诊断；对仍不可判定的 mixed 路径继续保守降级。
19. mixed uncertain-view 的识别现在也覆盖嵌套包裹形式（如 `!((normalize -> p))`、`as((normalize -> p))` 以及更深层 `or/and` 组合）：若不存在可证明命中的 non-view 备选，分支将保持保守，不会误触发 covered 型 `W4201`。
20. 列表 finite-split 的离散字面量域现在同时覆盖 `Char`：`['a']` / `['b']` 这类分支不再被仅按长度形状粗化；`['a'..'c']` 也可稳定覆盖后续 `['b']` 分支并触发对应 `W4201 covered`。
21. 对“仅由 uncertain view 备选组成”的 guarded 列表覆盖来源（缺少可证明命中的 non-view 路径），covered 型 `W4201` 现在会保守抑制，避免 `[((normalize -> (1..2)) | (normalize -> 3))]` 这类分支错误吞掉后续 `[3]`。若同一来源存在可证明命中的 deterministic non-view 路径，或 `view-inner` 在有限域可证明 always/never，则仍保留精确 covered 诊断（见 `examples/26_view_guard_uncertain_only_conservative.eidos`）。
22. 对 deep `not` mixed uncertain-view（如 `[!((normalize -> (2..3)) & 2)]`）的 covered 诊断边界已与 ADT 对齐：目标 token 可由内层 non-view 子约束证明 `no-match` 时恢复 `W4201 covered`（如目标 `[3]`），否则保持保守（如目标 `[2]`）。

### 3.13 预编译标准库（正式可用）
示例文件：`examples/29_precompiled_stdlib.eidos`、`examples/42_stdlib_safe_and_traits.eidos`

编译器现在以内嵌资源形式提供预编译标准库模块，并且 CLI 会按“能力”而不是按源码目录平铺显示：

| 能力分类 | 模块 | 这类模块能做什么 | 代表接口 |
| --- | --- | --- | --- |
| 函数式能力 | `Std::Fn`、`Std::Prelude`、`Std::Functor`、`Std::Applicative`、`Std::Foldable`、`Std::Traversable`、`Std::Monad`、`Std::Option`、`Std::Result`、`Std::Ordering`、`Std::Trait`、`Std::TraitInvoke` | `Std::Fn` 提供函数工具，`Std::Prelude` 提供常用 Text 安全 helper 与基础 File 文本 I/O fallback，再配合 `Option/Result`、trait 与 `Functor/Applicative/Foldable/Traversable/Monad` 抽象；当前 `Option/Result/Seq` 都已具备 `fmap/pure/apply/traverse/bind` 使用面，并共享 `fold_left/fold_right` 折叠入口；`Option/Result/Ordering` 也都已具备 `Eq` / `Ord` / `Show` 这组基础值类型能力；`T?` 可作为 `Std::Option::Option[T]` 类型糖，`??` 可作为 `Option::unwrap_or` fallback 运算符，`let?` 可作为 `Option/Result` 早返回绑定语法；教程风格优先展示 `|>`、`>>>`、`<<<`、`<$>`、`<*>`、`>>=`、`<>`、`??`、`let?` 与链式调用 | `value |> f |> g`、`f >>> g`、`inc <$> Some(1)`、`Some(f) <*> Some(x)`、`Some(x) >>= f`、`maybe_count ?? 0`、`let? value = maybe_value`、`xs.map(f).filter(p)`、`Fn::compose`、`Option::traverse`、`Seq::traverse`、`Result::and_then` |
| 数学能力 | `Std::Math`、`Std::FloatMath`、`Std::GameMath` | 标量数学、角度/插值辅助，以及 `IVec2/Vec2/IRect/Rect` 这类游戏数学类型与网格/几何 helper | `Math::wrap`、`FloatMath::smoothstep`、`FloatMath::angle_delta_degrees`、`GameMath::ivec2`、`GameMath::grid_cell_rect` |
| 容器能力 | `Std::Seq`、`Std::SeqBuilder` | 只读序列的查询、变换、过滤、折叠、拼接、拉链组合，以及显式 builder 阶段的 push/set/swap/freeze 工作流 | `Seq::head`、`Seq::tail`、`Seq::find`、`Seq::map`、`Seq::filter`、`Seq::fold_left`、`SeqBuilder::push`、`SeqBuilder::freeze` |
| 文件 IO 能力 | `Std::File` | 判断文件是否存在、整文件读写、fallback 读取与最后一次 IO 状态 | `File::exists`、`File::read_text_or`、`File::last_error`、`File::write_text` |
| 控制台 IO 能力 | `Std::Console` | 输出文本/整数/浮点/字符/布尔值，支持前缀+值的单行输出，读取一行输入 | `Console::write_line`、`Console::write_int`、`Console::write_text_int_line`、`Console::read_line_text` |
| 网络能力 | `Std::Network` | 发起最基础的 HTTP GET 文本请求 | `Network::http_get_text_or_empty` |
| 序列化能力 | `Std::Binary`、`Std::Json` | 做基础二进制编码/解码，以及 JSON 字符串、数组、对象拼装 | `Binary::encode_u32_le`、`Binary::bytes_to_string`、`Json::array`、`Json::object` |
| 其他基础能力 | `Std::Text` | 文本长度、判空、基础值转文本、裁剪空白、切片、字符/码点安全读取、子串查询；安全读取/查询现在同时提供 `*_opt` 与 `*_or` 两类入口 | `Text::len`、`Text::from_int`、`Text::from_bool`、`Text::trim`、`Text::slice`、`Text::char_code_at_opt`、`Text::char_code_at_or`、`Text::char_at_opt`、`Text::index_of_or` |

如果只想看“现在到底有哪些模块、每个模块导出了什么”，直接用 CLI：

可通过 CLI 查看当前可用模块及导出函数：

```powershell
dotnet run --project src/Eidosc/Eidosc.Cli -- info --stdlib
```

当前验证口径（2026-04-11）：
1. `examples/29_precompiled_stdlib.eidos` 继续覆盖核心函数式模块，并已通过 LLVM 阶段验证；当前样例已显式使用 `Option/Result/Seq/Text/Ordering` 的组合路径、Prelude wildcard 暴露的 Text/File helper、Text 空白裁剪、safe fallback helper，以及一部分 trait 驱动接口。
2. `examples/42_stdlib_safe_and_traits.eidos` 提供一个更短的综合入口，现已同时演示 `Option/Result/Seq` 的 `Functor/Applicative/Foldable/Traversable/Monad` 使用面、`Option/Result/Ordering` 的 `Eq/Ord/Show`、以及 `Text` 的 `*_opt` / `*_or` helper；当前基线也已包含 `Result::traverse` 对 `ResultWith[E]` applicative 的自动推断。
3. `Std::Option`、`Std::Result`、`Std::Ordering`、`Std::Seq`、`Std::Text` 均已有独立导入样例与 LLVM 集成断言；其中 `Option/Result/Seq` 现已覆盖 `fmap/pure/apply/bind`，并补齐 `fold_left/fold_right` 折叠路径；`let?` 的 `Option/Result` 绑定糖另由 `examples/63_let_question_option_result.eidos` 与 `projects/test/src/stdlib/std_let_question_binding.eidos` 覆盖。
4. `Std::Math`、`Std::FloatMath`、`Std::GameMath`、`Std::Console`、`Std::File`、`Std::Network`、`Std::Binary`、`Std::Json` 已有独立导入样例与定向测试；`Std::Prelude::*` 另有 `projects/test/src/stdlib/std_prelude_core_import.eidos` 覆盖 Text/File 核心 helper 的直接导入；`Std::Functor`、`Std::Applicative`、`Std::Foldable`、`Std::Traversable`、`Std::Monad`、`Std::TraitInvoke` 目前通过导出表与 CLI 分组可见性校验覆盖。
5. 当前 `Std::Network` 仍是最小实现：底层为 best-effort 文本抓取；失败时返回空字符串，不做完整 HTTP 协议抽象。
6. 当前 `Std::Json` 偏向“拼 JSON 文本”，还不是完整 JSON 解析器；`Std::Binary` 目前也只覆盖基础布尔、整数和字符串编码工具。

### 3.14 FFI：与 C 的互操作（已验证 2026-05-01）

详细教程见 [`FFI.zh-CN.md`](FFI.zh-CN.md)。

已验证能力：
1. `@ffi` 声明外部 C 函数（支持自定义符号名）。  
   样例：`examples/55_ffi_basic.eidos`。
2. 指针操作：`ptr_null`、`ptr_is_null`、`ptr_add`、`ptr_load_int`、`ptr_store_int`。  
   样例：`examples/56_ffi_pointer_ops.eidos`。
3. 函数指针回调：`cfn_from` 将 Eidos 函数转为 C 函数指针，`cfn_call` 通过指针间接调用。  
   样例：`examples/57_ffi_callback.eidos`。
4. 端到端 `qsort` 回调集成：Eidos 比较函数通过 `cfn_from` 传给 C `qsort`，排序结果正确。  
   样例：`examples/58_ffi_qsort.eidos`。

FFI 安全类型集合：`Int`、`Int32`、`Float`、`Bool`、`Unit`、`RawPtr`、`Ptr[T]`、`Cfn`；函数类型参数可作为 Eidos closure 对象指针传给理解该 ABI 的 native 函数。
错误码：E3050（@ffi 带函数体）、E3051（非安全类型）、W3050（无用 link 指令）。

## 4. BNF 入口
核心 BNF 摘要请看 [`BNF.zh-CN.md`](BNF.zh-CN.md)。
英文版在 [`BNF.en.md`](BNF.en.md)。

## 5. 已验证示例基线
以下能力已通过 `verify-examples.ps1` 自动校验：

1. 嵌套调用在柯里化签名下可通过 Parser + Types。  
   样例：`examples/06_nested_call_parser_only.eidos`（`add(1)(2)`）。
2. 块表达式的尾表达式可正确作为块值参与返回类型统一。  
   样例：`examples/07_block_result_known_issue.eidos`。
3. 列表推导式可在 HIR 阶段保留完整结构；MIR 对静态与动态来源都可生成 CFG，并通过 runtime 列表 API 落地核心读写语义；LLVM 阶段完成 `array_get/array_set` 映射与回边局部值 slot 化；native smoke 可在 clang-only 环境执行；非 `VarPattern` 生成器会明确报错 `E5101`。  
   样例：`examples/08_list_comprehension.eidos`。
4. Marker effect 调用可通过 `types` 与 `llvm`；授权在编译期检查，并在 MIR 运行时语义前擦除。
   样例：`examples/09_effect_tag_call.eidos`。
5. `(expr -> pattern)` 可通过 `hir` 与 `mir`。  
   样例：`examples/11_advanced_patterns.eidos`。
6. 模式入口 `if let` 与 `while let` 已贯通到 HIR/MIR：`if let` 降为双分支 `match`，`while let` 降为 `loop + match + break`。  
   样例：`examples/13_if_let_pattern.eidos`、`examples/14_while_let_pattern.eidos`。
7. `let?` 绑定糖已贯通 Parser/NameResolver/Types，并在 HIR 构建时消除为普通 `match` + `return`；HIR/MIR/LLVM 没有 `let?` 专用节点。
   样例：`examples/63_let_question_option_result.eidos`、`projects/test/src/stdlib/std_let_question_binding.eidos`。
8. 列表模式与剩余模式（`[]` / `[head, ..tail]` / `[..]`）已贯通到 HIR/MIR，并包含长度保护与 tail 物化语义；guarded 列表分支支持 finite-case 覆盖推理。
   样例：`examples/16_list_rest_pattern.eidos`、`examples/17_list_guarded_coverage.eidos`、`examples/18_view_guard_unsat.eidos`、`examples/19_view_guard_finite_set.eidos`、`examples/20_guard_algebra_unsat.eidos`、`examples/21_view_guard_mixed_and_not_unsat.eidos`、`examples/22_nested_view_combinator_coverage.eidos`、`examples/23_view_guard_other_and_not_unsat.eidos`、`examples/24_view_guard_nested_as_other_unsat.eidos`、`examples/25_view_guard_mixed_view_nonview_conservative.eidos`。
9. `when pat <- expr` 的 pattern guard 绑定已贯通 Parser/NameResolver/Types/HIR/MIR，并覆盖函数体模式分支优先写法。
   样例：`examples/27_pattern_guard_binding.eidos`。
10. 早返回 `return` 的类型与 lowering 链路已贯通到 Types/HIR/MIR/LLVM：返回值会按函数返回类型校验，HIR 保留专用 return 节点，MIR/LLVM 可稳定生成返回终止指令且不走 fallback 诊断。
   样例：`examples/28_early_return.eidos`。
11. 内嵌预编译标准库现已按能力重整：核心函数式模块在 `examples/29_precompiled_stdlib.eidos` 中完成宽覆盖，在 `examples/42_stdlib_safe_and_traits.eidos` 中完成短路径综合演示；数学、游戏数学、IO、网络、序列化模块另有独立导入样例与定向测试覆盖；能力分组本身由 CLI 与导出表测试校验。
   样例：`examples/29_precompiled_stdlib.eidos`、`examples/42_stdlib_safe_and_traits.eidos`、`examples/55_functional_infix_chain_style.eidos`、`examples/62_option_suffix_coalesce.eidos`、`examples/63_let_question_option_result.eidos`、`examples/65_game_math_vectors.eidos`、`projects/test/src/stdlib/std_*_import.eidos`。
12. 返回借用来源规则当前已锁定到教程基线：参数 reference 可先流经局部 alias 再返回，但返回来源仍必须能追到输入参数。
   样例：`examples/54_return_borrow_param_alias.eidos`。

## 6. 使用建议
1. 优先写可直接推断的简单表达式，复杂组合可拆成多个 `let`。
2. 新示例优先使用中缀和链式函数式风格：线性值流用 `|>`，trait 组合用 `<$>` / `<*>` / `>>=`，容器流水线用 `.map(...).filter(...).fold_left(...)`。每一段函数签名仍需能独立成立，再进行拼接。
3. 修改语法后，必须同时更新：
   `docs/tutorial/*.md`、`docs/tutorial/examples/*`、`tools/editor/*`。
4. 需要排查类型推断时，建议使用 `debug --debug-level diagnostic` 查看 `substitution` 输出；当前已包含类型变量 `raw/resolved`、绑定链（chain）与 AST 上下文位置。

## 7. 当前 MIR 状态与限制
- Effect 行只属于编译期元数据，不会生成 handler、continuation、CPS 或运行时 dispatch MIR 节点。
- `MirBuilder` 现在会在调用结果类型缺失时做类型落盘收敛（函数签名优先，effect 场景回退当前函数返回类型），用于降低后端 coercion 兜底压力。
- ADT 构造器调用现在有专用 MIR 目标解析：`CallConvention.Constructor` 会直接降为 `MirFunctionRef`（例如 `call @A(1)`），不再退化成“调用未初始化局部”。
- `list comprehension` 在 MIR 阶段已支持静态/动态来源 CFG lowering；非 `VarPattern` 生成器模式明确报错 `E5101`，不会作为 warning 回退。
