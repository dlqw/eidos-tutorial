# 风格约定

## 命名分层

Eidos 的命名分两层（入门见 [第一个程序](../getting_started/03-first-program.md)）：

- **运行时值**：小写开头标识符，推荐 `lower_snake_case`（函数、值、参数、字段）；
- **编译期值**：大写开头标识符（类型、trait、effect、构造器、模块路径、表示类型的泛型参数、`comptime` 绑定）。

规则要点：

1. 小写根后继续小写段表示普通运行时成员链（`user.profile.display_name`）；
2. 大写根，或"小写 package alias + 大写 module"，开始一条 Namespace 路径（`crypto_a.Hash.Sha256.digest(bytes)`）；
3. 依赖 alias 可以保持 `lower_snake_case`；其下一段必须是大写 module；
4. 构造器调用会产生运行时值，但构造器符号本身仍是编译期值。

## 函数式读法

风格约定基线（2026-05-28）：新的教程与标准库示例应优先展示 Eidos 的函数式读法。

| 场景 | 优先写法 | 示例 |
| --- | --- | --- |
| 线性数据流 | 管道 `\|>` | `value \|> f \|> g` |
| 函数组合 | `>>>` / `<<<` | `f >>> g` 或 `g <<< f` |
| Functor / Applicative / Monad | `<$>` / `<*>` / `>>=` | `f <$> value`、`mf <*> mx`、`mx >>= f` |
| 连续容器变换 | 链式调用 | `xs.map(f).filter(p).fold_left(seed)(step)` |

补充约定：

- 当限定路径能显著降低歧义时，仍可保留 `Module.function(value)(arg)` 写法；
- 普通分组调用 `function(value)` / `function(value, arg)` 是稳定的默认调用风格，不会仅因为可写成链式或中缀而提示；
- CLI/IDE/LSP 会把可机械转换的连续柯里化前缀调用作为 help/hint 级风格建议给出 Quick Fix：`Seq.append(a)(b)` 可改为链式 `a.append(b)`，也可改为分组调用 `Seq.append(a, b)`；
- 使用建议：优先写可直接推断的简单表达式，复杂组合可拆成多个 `let`；新示例优先使用中缀和链式函数式风格，每一段函数签名仍需能独立成立，再进行拼接。

## 示例代码要求

1. 示例正文与 `examples/` 文件一一对应，可被 `verify-examples.ps1` 验证；
2. 参数类型可区分的接口统一使用重载而非类型后缀（`Ordering.compare`、`Hash.hash`、`Console.write`）；
3. 只靠返回类型区分、转换、带单位、字符码点以及 raw/safe FFI 边界继续使用语义名（见 [标准库使用](02-standard-library.md)）。

## 文档同步义务

修改语法后，必须同时更新：教程正文（全部章节）、`examples/*`、`tools/editor/*`（三个编辑器插件）、fixture 与 BNF 附录。见 [示例验证与测试](../toolchain/02-verification-and-testing.md)。
