# 附录 C：错误码与验证基线

## 错误码总表

完整按域排查指南见 [错误码体系总览](../errors/01-error-code-overview.md)：

| 域 | 范围 | 主题 | 典型错误 |
| --- | --- | --- | --- |
| `E1xxx` | 借用与能力 | `E1001`（重复 drop）、`E1004`（返回借用）、`E1011`-`E1013`（read/write/move 能力不足） |
| `E3xxx` | 声明与命名 | `E3000`（绑定 mode）、`E3004`（重叠 impl）、`E3050`/`E3051`（FFI）、`E3061`（closed-case）、`E3203`-`E3205`/`E3301`（字段） |
| `E4xxx` | 类型 | `E4000`（构造器/`..` 边界）、`E4011`-`E4014`（range/as/view 模式） |
| `E5xxx` | MIR | `E5101`（列表推导生成器） |
| `W3xxx` | 警告 | `W3050`（无用 link 指令） |
| `W4xxx` | 覆盖告警 | `W4200`（非穷尽）、`W4201`（不可达）、`W4301`（重复 key） |

## 已验证示例基线

> 基线范围说明：当前分支不再把 `proof` / lawful 相关内容作为教程基线的一部分；相关实验内容移至单独的 proof 分支维护。

以下能力已通过 `verify-examples.ps1` 自动校验（示例文件在重组后的新路径，旧路径见 [`EXAMPLES-MAP.md`](../EXAMPLES-MAP.md)）：

1. 嵌套调用在柯里化签名下可通过 Parser + Types。样例：`examples/basics/06_nested_call_parser_only.eidos`。
2. 块表达式的尾表达式可正确作为块值参与返回类型统一。样例：`examples/basics/07_block_result_known_issue.eidos`。
3. 列表推导式：HIR 保留完整结构；MIR 对静态与动态来源生成 CFG，通过 runtime 列表 API 落地；LLVM 完成 `array_get/array_set` 映射与回边局部值 slot 化；native smoke 可在 clang-only 环境执行；非 `VarPattern` 生成器报 `E5101`。样例：`examples/basics/08_list_comprehension.eidos`。
4. Marker effect 调用可通过 `types` 与 `llvm`；授权在编译期检查，并在 MIR 运行时语义前擦除。样例：`examples/effects/09_effect_tag_call.eidos`。
5. `(expr -> pattern)` 可通过 `hir` 与 `mir`。样例：`examples/pattern/11_advanced_patterns.eidos`。
6. `if let` 与 `while let` 已贯通到 HIR/MIR（降为双分支 `match`、`loop + match + break`）。样例：`examples/pattern/13_if_let_pattern.eidos`、`examples/pattern/14_while_let_pattern.eidos`。
7. `let?` 绑定糖已贯通 Parser/NameResolver/Types，并在 HIR 构建时消除为普通 `match` + `return`。样例：`examples/basics/63_let_question_option_result.eidos`。
8. 列表模式与剩余模式（`[]` / `[head, ..tail]` / `[..]`）已贯通到 HIR/MIR，含长度保护与 tail 物化语义；guarded 列表分支支持 finite-case 覆盖推理。样例：`examples/pattern/16_list_rest_pattern.eidos`、`examples/pattern/17_list_guarded_coverage.eidos` 及 `18`-`25` 系列。
9. `when pat <- expr` 的 pattern guard 绑定已贯通 Parser/NameResolver/Types/HIR/MIR。样例：`examples/pattern/27_pattern_guard_binding.eidos`。
10. 早返回 `return` 的类型与 lowering 链路已贯通到 Types/HIR/MIR/LLVM。样例：`examples/basics/28_early_return.eidos`。
11. 内嵌预编译标准库按能力重整：核心函数式模块在 `examples/stdlib/29_precompiled_stdlib.eidos` 宽覆盖、`examples/stdlib/42_stdlib_safe_and_traits.eidos` 短路径综合演示；数学、游戏数学、IO、网络、序列化模块另有独立导入样例与定向测试覆盖。样例：`examples/stdlib/29_precompiled_stdlib.eidos`、`examples/stdlib/42_stdlib_safe_and_traits.eidos`、`examples/functional/55_functional_infix_chain_style.eidos`、`examples/basics/62_option_suffix_coalesce.eidos`、`examples/basics/63_let_question_option_result.eidos`、`examples/stdlib/65_game_math_vectors.eidos`。
12. 返回借用来源规则已锁定到教程基线：参数 reference 可先流经局部 alias 再返回，但返回来源仍必须能追到输入参数。样例：`examples/ownership/54_return_borrow_param_alias.eidos`。

## MIR 状态与限制（基线）

- Effect 行只属于编译期元数据，不会生成 handler、continuation、CPS 或运行时 dispatch MIR 节点；
- `MirBuilder` 在调用结果类型缺失时做类型落盘收敛（函数签名优先，effect 场景回退当前函数返回类型）；
- ADT 构造器调用有专用 MIR 目标解析：`CallConvention.Constructor` 直接降为 `MirFunctionRef`（如 `call @A(1)`），不再退化成"调用未初始化局部"；
- `list comprehension` 在 MIR 阶段支持静态/动态来源 CFG lowering；非 `VarPattern` 生成器模式报 `E5101`。

更多编译期机制与理念见 [性能与编译器理念](../performance/01-compiler-driven-optimization.md)。
