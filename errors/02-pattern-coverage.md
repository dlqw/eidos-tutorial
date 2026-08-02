# 模式覆盖告警：W4200 / W4201

示例文件：`examples/pattern/17_list_guarded_coverage.eidos`、`examples/pattern/18_view_guard_unsat.eidos`、`examples/pattern/19_view_guard_finite_set.eidos`、`examples/pattern/20_guard_algebra_unsat.eidos`、`examples/pattern/21_view_guard_mixed_and_not_unsat.eidos`、`examples/pattern/22_nested_view_combinator_coverage.eidos`、`examples/pattern/23_view_guard_other_and_not_unsat.eidos`、`examples/pattern/24_view_guard_nested_as_other_unsat.eidos`、`examples/pattern/25_view_guard_mixed_view_nonview_conservative.eidos`、`examples/pattern/26_view_guard_uncertain_only_conservative.eidos`

编译器对模式分支做覆盖分析（不阻断编译，但建议消除）：

- `W4200`：**模式匹配不穷尽**。例如只匹配 `true`，或 ADT 漏掉构造器。
- `W4201`：**不可达分支**。前面已有"无 guard 的不可反驳分支"时，后续分支不可达；或组合模式可证明"匹配集为空"（如 `true & false`）。

## 诊断输出结构

`W4200` 会附带机器可解析的缺失与覆盖信息：

| note | 内容 |
| --- | --- |
| `Missing-case witnesses` | 最小缺失路径示例（Bool/ADT 先行覆盖） |
| `Missing-case traces` | 结构化 witness，`display [stable-key]` 格式（如 `false [bool:false]`、`None [ctor:123]`） |
| `Missing-case trace groups` | 按 witness 空间分组（`bool` / `tuple-bool` / `ctor` / `wildcard`） |
| `Missing-case trace kv` | 机器可解析键值段：`kind=<...>;key=<...>;display=<...>`，多条用 `\|\|` 分隔 |

`W4201` 会附带 `Covered-case witnesses` / `Covered-case traces`（`witness <- #branch` 来源映射），以及 `Suppressed-covered trace kv` 键值段。

## 精确推理域

覆盖分析对以下有限空间做精确推理：

- `Bool`、`tuple Bool`（含顶层 `or/and/not/as` 组合，如 `(true, true) | (true, false)`、`!(false, false)`）；
- 顶层 ADT 构造器集合（`!None`、`Some(x) & !None` 可参与缺失构造器与覆盖不可达推理）；
- 列表长度形状与元素级 witness（`[]` / 固定前缀 / `[..]`，Bool 元素可到元素级）；
- `Char` 字面量（按有限离散值处理）；
- 可证明的整型绑定约束（含 `as` / `view-inner` / 有限集合）；
- 常量 guard 折叠：`when true` 按无 guard 处理并计入覆盖；`when false` 不计入覆盖并触发 `W4201`；
- 布尔逻辑短路三值推理：`x \|\| true` 恒真，`x && false` 恒假；
- 变量模式谓词 guard（`v when v => ...`）识别为可证明覆盖；
- guard 代数：有限整数集合上的算术比较（`x + 1 == 4` 当 `x ∈ {1,2}` 可判恒假）、布尔互补恒值（`b && !b` 恒假、`b \|\| !b` 恒真）。

## 保守策略（不误报优先）

对无法静态证明的场景，分析保持保守，绝不把分支误判为已覆盖：

- 存在无法静态证明的 `when` guard 时，`W4200` 会附带 note 说明"guard 分支不会被当作穷尽覆盖"，并标注具体分支号（`#N`）与 `Unresolved-guard branch hints`（`#N@line:col` 与可推导的 lower-bound case，未知时显示 `?`）；
- guarded refutable-view 来源分支的 covered 判定会保守抑制（`Conservatively suppressed covered warnings`），并附带细粒度 reason 标签（如 `adt:refutable-view`、`guard:not-provable`、`list:view-inner-uncertain@...`）；
- 但当 non-view 备选可对目标做"可证明命中"时恢复精确 `W4201 covered`（如 `((normalize -> 1) | 2) & _` 对目标 `2`）；
- `or` 备选中"对当前目标 assignment 不可命中"的备选不会阻断 guard 绑定收敛。

## 常见误报排查

1. 看到 `W4200` 但认为已穷尽：检查 guarded 分支是否带不可证明 guard（看 `Unresolved-guard branch hints`）；
2. 看到 `W4201` 但认为分支可达：看 `Covered-case traces` 的覆盖来源（`witness <- #branch`），并确认前序分支是否无 guard；
3. 涉及 view 的组合模式：默认按保守处理，需要 deterministic non-view 命中才会精确。

完整规则与锁定回归见 README 基线第 3.11 节与编译器测试；示例覆盖见上方文件列表。
