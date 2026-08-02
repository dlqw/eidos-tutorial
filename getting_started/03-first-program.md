# 第一个程序：字面量与绑定

示例文件：`examples/getting_started/01_literals_bindings.eidos`

Eidos 程序从字面量与绑定开始：

```eidos
answer :: 42;
pi :: 3.14;
greeting :: "hello";
ok :: true;
initial :: 'a';
let mut counter = 1;
```

## 字面量

| 字面量 | 类型 | 示例 |
| --- | --- | --- |
| 整型 | `Int` | `42` |
| 浮点 | `Float` | `3.14` |
| 字符串 | `String` | `"hello"` |
| 布尔 | `Bool` | `true` / `false` |
| 字符 | `Char` | `'a'` |

字符串 `==` / `!=`（包括模式字面量比较）统一走 runtime `string_equals`，语义是"按内容比较"，不是指针身份比较。

## 绑定

Eidos 0.8 有两条绑定通道：

1. **顶层声明**：`name :: expr;` 声明编译期可解析的常量；`name :: Type { ... }` 声明类型。
2. **局部绑定**：`name := expr;` 绑定一个不可变局部值；`mut name := expr;` 绑定一个可重赋值的局部值。只有 `let mut` / `mut` 绑定可以作为 `:=` 赋值目标。

块级 `let <pattern> = <expr>;` 用于解构式绑定（见 [变量绑定与解构](../basics/01-variables-and-bindings.md)），当前要求"不可反驳模式"；可反驳场景请使用 `match`。

## 命名分层

Eidos 的命名分两层：

- **运行时值**：小写开头标识符（`answer`、`greeting`），推荐 `lower_snake_case`；
- **编译期值**：大写开头标识符（`Point`、`Show`、`Int`）。类型是一等编译期值，因此类型、trait、effect、构造器、模块路径和表示类型的泛型参数属于大写命名空间。

构造器调用会产生运行时值，但构造器符号本身仍是编译期值。依赖 alias 可以保持 `lower_snake_case`；其下一段必须是大写 module，例如 `crypto_a.Hash.Sha256.digest(bytes)`。

小写根后继续小写段表示普通运行时成员链（`user.profile.display_name`）；大写根，或"小写 package alias + 大写 module"，开始一条 Namespace 路径。

## 下一步

- 类型细节见 [基本类型](../basics/02-primitive-types.md)；
- 解构与绑定模式见 [变量绑定与解构](../basics/01-variables-and-bindings.md)。
