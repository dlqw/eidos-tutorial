# 模式匹配

示例文件：`examples/pattern/`（`10_match_guard`、`11_advanced_patterns`、`13_if_let_pattern`、`14_while_let_pattern`、`15_pattern_binding_modes`、`16_list_rest_pattern`、`17_list_guarded_coverage`、`18-26` view/guard 覆盖分析系列、`27_pattern_guard_binding`、`70_decision_table`）

模式匹配是 Eidos 最核心的控制结构之一。`match` 按分支顺序尝试模式，命中后执行对应分支体：

```eidos
classify :: Int -> Int
{
    x => match x
    {
        n when n > 0 => 1,
        _ => 0
    }
}
```

## 模式种类

```eidos
normalize :: Int -> Int { x => x }

classify :: Int -> Int
{
    x => match x
    {
        !0 => 99,                    // not-pattern
        1 | 2 => 12,                 // or-pattern
        3..5 => 35,                  // range-pattern
        (normalize -> 9) => 90,      // view-pattern
        _ => 0
    }
}
```

| 模式 | 写法 | 说明 |
| --- | --- | --- |
| 字面量 | `1`、`'a'`、`true` | 值相等匹配 |
| 通配 | `_` | 匹配任意值，不绑定 |
| 变量 | `n` | 绑定被匹配值 |
| or | `p1 \| p2` | 任选其一 |
| and | `(p1) & (p2)` | 同时满足 |
| not | `!p` | 否定（不允许引入绑定） |
| range | `3..5` | 区间（`start <= end`，类型阶段校验 `E4011`） |
| as | `(p as x)` | 匹配 p 并绑定 x |
| view | `(expr -> pattern)` | 先求值 expr 得到可调用值，调用 `expr(scrutinee)` 后用 pattern 匹配返回值 |
| 构造器 | `Some(v)` | 按构造器判别 + 字段绑定 |
| tuple | `(a, b)` | 结构化匹配 |
| list | `[]`、`[p1, p2]`、`[head, ..tail]` | 列表形状匹配 |

### view pattern

标准写法为 `(expr -> pattern)`，语义是"先求值 `expr` 得到可调用值，再调用 `expr(scrutinee)`，并用 `pattern` 匹配返回值"；`view(...)` / `View(...)` 已移除。当固定参数选择一个分类器、被匹配值提供剩余参数时，部分应用调用是表达固定参数 view 的推荐方式：

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

view-pattern 左侧表达式支持一般表达式形态（如 `if ... then ... else ...`），但语义要求该表达式在当前类型上下文下可调用为一元函数；若不可调用，会给出专用诊断 `E4014`。对 FFI/输入轮询，优先先读取或分类一次事件，再匹配稳定值；不要把多次有副作用的轮询分散到许多 pattern 里。

### 组合模式细节

1. `or-pattern` 支持同名绑定传播到分支体（如 `(1 as n) | (2 as n) => n`）；绑定名不一致时诊断会指出具体 alternative 的 `missing/extra` 细节；
2. `and-pattern` 支持跨 conjunct 合并"不同变量名"的绑定（如 `Pair(a, _) & Pair(_, b) => a + b`）；同名重复绑定会报错；
3. `not-pattern` 不允许引入绑定（`!(1 as n)` 会报错）；
4. 同一分支内会复用 view 调用结果，避免"判定阶段一次 + 绑定阶段一次"的重复调用；
5. 同一模式作用域中重复绑定同名变量会报错（如 `Pair(n, n)`）；
6. `or-pattern` / `and-pattern` 在 MIR 阶段按短路语义 lowering，右侧模式不会被无条件求值；
7. 构造器形状校验默认严格执行：检查位置参数个数、命名字段合法性，以及 named/positional 形态是否匹配（包括零参数构造器）；
8. `range-pattern` 在类型阶段做边界有序性校验：`5..3` 或 `'z'..'a'` 报 `E4011`（`start <= end`）；scrutinee 不是 `Int/Char` 报 `E4012`；
9. `as-pattern` 继承并校验被匹配值类型（类型不匹配报 `E4013`）。

## 分支守卫 `when`

`pattern when guard => expr` 中 guard 为 `false` 时继续尝试后续分支（示例：`10_match_guard`）：

- `when` 后可直接使用简单标识符表达式（如 `v when v => ...`）；
- 支持 Haskell 风格 pattern guard：`when pat <- expr`，其中 `pat` 绑定仅在当前分支体内可见，不会泄漏到其它分支；
- `when pat <- expr` 里的 `pat` 是完整模式（元组、`or/and/not`、`view` 等都可放进去）；
- 守卫和普通表达式里的 `&&` / `||` 按短路求值，左边已决定结果时右边不再执行。

## Decision table 表达式

当多个分支重复调用同一个 predicate、只改变少量参数时，使用 `decide fallback { template: rows }`：

```eidos
choose :: Int -> Int
{
    fallback => decide fallback {
        is_even(_):
            2 | 4 => 20,
            6 when fallback > 0 => 60
    }
}
```

表头中的 `_` 是 row key 的代入位置；row 按源码顺序、key 按从左到右顺序测试，命中第一项后短路返回，否则只在最后求值 fallback。多 hole 表头使用 tuple key，`when` 可追加 row guard。同一表头组内的重复字面量 key 会产生 `W4301`（后一个 key 永远不可达）。

## 列表模式

支持 `[]`、`[p1, p2]`、`[p1, ..rest]`、`[..rest]`、`[..]`：

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

- MIR lowering 先执行长度判定再执行元素匹配，避免索引越界读取；
- `..rest` 在 MIR 中绑定为"后缀列表切片"，通过运行时列表 API 物化；
- `[..]` 表示"仅剩余匹配标记，不绑定变量"；
- `..` 仅允许出现在列表模式中，且 `..rest` 必须位于列表模式尾部；`(a, ..rest)`、`Some(..rest)`、`[a, ..rest, b]` 会给出带修复建议的 `E4000`。

## 覆盖分析（W4200 / W4201）

编译器对模式分支做覆盖分析：

- `W4200`：模式匹配不穷尽（例如只匹配 `true`，或 ADT 漏掉构造器），会附带缺失分支明细与 witness；
- `W4201`：不可达分支（前面已有"无 guard 的不可反驳分支"时，后续分支不可达），会附带被哪些前序分支覆盖的 trace。

覆盖分析支持 Bool、tuple-Bool、顶层 ADT 构造器集合、列表长度/元素形状与 `Char` 有限域的精确推理，并对 guarded / view 分支采用保守策略避免误报。完整规则与机器可解析输出见 [攻克编译错误：模式覆盖告警](../errors/02-pattern-coverage.md)。

## `if let` / `while let` / `let?`

- `if let` 与 `while let`：见 [流程控制](06-flow-control.md)；
- `let?`：Option/Result 成功值展开与早返回，见 [错误处理](11-error-handling.md)；
- 块级解构绑定 `(a, b) := pair`：见 [变量绑定与解构](01-variables-and-bindings.md)。
