# 基本类型

示例文件：`examples/getting_started/01_literals_bindings.eidos`

## 类型一览

| 类型 | 字面量示例 | 说明 |
| --- | --- | --- |
| `Int` | `42` | 整型 |
| `Float` | `3.14` | 浮点 |
| `String` | `"hello"` | 字符串，内容相等语义 |
| `Bool` | `true` | 布尔 |
| `Char` | `'a'` | 字符 |
| `Unit` | `()` | 单元类型 |

## 单元类型 `Unit`

`Unit` 是"无值"类型，对应其他语言的 `void`/`unit`：

- 没有副作用的纯流程函数常以 `Unit` 为参数或返回类型；
- 首个运行时参数规范化为 `Unit` 时，普通 block 隐式等价于唯一的 `_ => block` 分支（见 [语句与表达式](03-statements-and-expressions.md)）；
- 函数/分支没有尾表达式时，块值就是 `Unit`。

## 字符串语义

`String` 的 `==` / `!=`（包括模式字面量比较）统一走 runtime `string_equals`，语义是"按内容比较"，不是指针身份比较。输出相关约定见 [格式化输出](13-formatting-output.md)。

## 元组

`(Int, String)` 是元组类型。元组按结构化规则参与 trait 检查：当且仅当每个元素类型都满足该 trait 时，元组满足该 trait（详见 [Trait 与 named instance](09-traits.md)）。
