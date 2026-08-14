# 常见陷阱

按真实开发中容易踩的坑整理，每条都对应已实现的诊断或边界行为。

## 借用相关

1. **返回局部引用**：`return ref local`、先从局部/临时值借一层再返回、先 alias 参数后又被局部来源覆盖再返回 → borrow 阶段 `E1004`。返回引用必须可追溯到输入参数（`ref expr` 只允许作用在稳定位置上）。见 [所有权与借用](../advanced/01-ownership-and-borrowing.md)。
2. **对临时值取引用**：`ref (x + 1)`、`mref make()` → Types 阶段直接报错。
3. **重复 drop**：non-Copy aggregate 的 partial move 后整值再次使用/释放 → `E1001`。每条路径只释放一次。

## 声明与命名

4. **`::` 不是运算符**：`::` 是声明绑定 token；想写限定名或链式调用用 `.`（点号 Namespace）。见入门 [第一个程序](../getting_started/03-first-program.md)。
5. **裸重载引用歧义**：`f :: render;` 在 `render` 有多个重载且无期望类型时报歧义；用 `show_int :: Int -> String = render;` 显式给类型。
6. **别名不产生特化**：`ResultWith[E, T] = Result[T, E]` 与 `AlsoResult[E, T] = Result[T, E]` 同时声明同一 `Applicative[...]` evidence → canonical 折叠冲突 `E3004`。alias-only 等价重叠不会因"名字不同"而共存。
7. **泛型 trait 实参推断**：约定式 impl 注册不会推断泛型 Trait 的类型实参；对泛型 Trait 必须显式 `Trait[...]`（`FunctorBox :: instance Functor[Box]`）。

## 类型与模式

8. **生成器模式**：列表推导式生成器必须是简单变量或通配绑定（`[x * 2 | x <- ...]`）；非 `VarPattern` 生成器 → MIR 阶段 `E5101`，不会作为 warning 回退。
9. **`..` 位置**：`..rest` 只允许在列表模式尾部；`(a, ..rest)`、`Some(..rest)`、`[a, ..rest, b]` 会收到带修复建议的 `E4000`。
10. **range 边界**：`5..3`、`'z'..'a'` → `E4011`；对非 `Int/Char` 的 scrutinee → `E4012`。
11. **view 表达式必须可调用**：`(expr -> pattern)` 中的 `expr` 必须可调用为一元函数 `expr(scrutinee)`，否则 `E4014`。
12. **FFI 轮询副作用**：不要把多次有副作用的轮询分散到许多 pattern 里；先读取/分类一次事件，再匹配稳定值。

## 效果与能力

13. **effect 只声明不附着**：自定义 effect 只声明而不把它附着到任何边界函数没有语义作用；`need` 才是授权入口。
14. **`noImplicitStdlib` 已移除**：关闭 Prelude 使用 `[language].noImplicitPrelude = true`；旧配置名直接报错。
15. **pure comptime 能力边界**：pure comptime 不能读取文件、环境、进程、网络或 FFI；只有 `[build].program` 指定的构建程序能取得能力。见 [编译期元编程](../advanced/06-metaprogramming.md) 与 [Build host 与 BuildGraph](../advanced/07-build-host.md)。
16. **`let?` 类型约束**：`Option[A]` 只能用于返回 `Option[R]` 的上下文，`Result[A, E]` 只能用于返回 `Result[R, E]` 上下文；当前不做 `Option -> Result` 转换。见 [错误处理](../basics/11-error-handling.md)。

## 风格

17. **过度柯里化前缀调用**：`Seq.append(a)(b)` 可机械转换为链式 `a.append(b)` 或分组 `Seq.append(a, b)`；CLI/IDE 会给出 help/hint 级风格建议，`--deny style` 会把这类告警视为失败。
18. **在 `:=` 上写 `let` 旧形态**：旧 `let` 绑定只通过显式迁移命令处理；0.8 使用 `name := expr;` / `mut name := expr;` 与块级 `let <pattern> = <expr>;`。
