# 附录 B：关键字与运算符

> 本表只收录本教程正文出现并已验证的关键字与运算符；完整语法以 [BNF 摘要](grammar.md) 与 BNF 全文件为准。

## 关键字

| 关键字 | 用途 | 章节 |
| --- | --- | --- |
| `let` / `let mut` | 块级模式绑定 / 可重赋值绑定 | [变量绑定与解构](../basics/01-variables-and-bindings.md) |
| `mut` | 局部可重赋值绑定 | 同上 |
| `type` | 类型 / 别名 / 构造器声明 | [复合类型](../basics/05-compound-types.md) |
| `trait` / `instance` | trait 与 named instance | [Trait 与 named instance](../basics/09-traits.md) |
| `effect` | effect 声明 | [效果系统](../advanced/02-effects.md) |
| `module` | 模块声明 | [模块与包](../basics/12-modules-and-packages.md) |
| `import` / `export` | 导入 / 导出 | 同上 |
| `need` | effect 授权 | [效果系统](../advanced/02-effects.md) |
| `match` / `when` | 模式匹配 / 分支守卫 | [模式匹配](../basics/07-pattern-matching.md) |
| `if` / `then` / `else` | 分支与选择器 | [流程控制](../basics/06-flow-control.md) |
| `while` | 循环（含 `while let`） | 同上 |
| `return` | 早返回 | [语句与表达式](../basics/03-statements-and-expressions.md) |
| `decide` | decision table | [模式匹配](../basics/07-pattern-matching.md) |
| `comptime` | 编译期值/参数域 | [值域泛型与 const generics](../advanced/04-const-generics.md) |
| `where` | 泛型约束从句 | [泛型](../basics/08-generics.md) |
| `given` | 显式选择 instance | [Trait 与 named instance](../basics/09-traits.md) |
| `case` | 构造器特化声明（`Ctor :: type case Type`） | [复合类型](../basics/05-compound-types.md) |
| `as` | 模式绑定（含 `as ref` / `as mref`） | [模式匹配](../basics/07-pattern-matching.md) |
| `do` | operator/`do` elaboration 语义角色（Prelude 注册） | [集合类型](../basics/10-collections.md) |

## 运算符与符号

| 类别 | 符号 | 说明 |
| --- | --- | --- |
| 声明 | `::`、`:=`、`=` | `::` 声明绑定；`:=` 局部绑定；`=` 字段/构造赋值与别名 |
| 类型 | `->`、`=>`、`[...]` | 函数箭头、分支箭头、泛型参数 |
| 管道与组合 | `\|>`、`>>>`、`<<<` | 数据流、正向/反向组合（[函数式编程](../advanced/05-functional-programming.md)） |
| 组合子 | `>>=`、`<$>`、`<*>`、`<>` | Prelude lowering 的固定优先级运算符（同上） |
| 字符串 | `++` | 拼接 |
| Option | `?`（后缀类型）、`??` | `Int?`、`value ?? 42`（[错误处理](../basics/11-error-handling.md)） |
| 布尔 | `&&`、`\|\|`、`!` | 短路与 / 或 / 非 |
| 比较 | `==`、`!=`、`<`、`<=`、`>`、`>=` | 字符串按内容比较 |
| 算术 | `+`、`-`、`*`、`/`、`%` | 数值 |
| 模式 | `\|`、`&`、`!`、`..`、`<-` | or / and / not / range / pattern guard（[模式匹配](../basics/07-pattern-matching.md)） |
| 引用 | `ref`、`mref`、`*` | 共享/可变引用、解引用（[所有权与借用](../advanced/01-ownership-and-borrowing.md)） |
| 反引号 | `` `f` `` | 中缀调用（[函数](../basics/04-functions.md)） |
| 属性 | `@[...]` | 编译器属性（`derive`、`expand`、`extern`） |
| 字面占位 | `_` | 通配 / 位置参数 |

自定义符号运算符默认左结合，优先级位于加减/拼接之后、函数组合之前（[重载与自定义运算符](../advanced/09-overloads-and-operators.md)）。
