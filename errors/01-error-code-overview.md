# 错误码体系总览

编译器诊断按错误码域组织。遇到报错时先看错误码前缀，再按对应章节排查：

| 域 | 范围 | 主题 | 典型错误 |
| --- | --- | --- | --- |
| `E1xxx` | 借用与能力 | 所有权、借用冲突、能力不足 | `E1001`、`E1004`、`E1011`-`E1013` |
| `E3xxx` | 声明与命名 | trait 重叠、FFI 声明、closed-case、字段访问 | `E3000`、`E3004`、`E3050`、`E3051`、`E3061`、`E3203`-`E3205`、`E3301` |
| `E4xxx` | 类型 | 构造器类型、模式边界、as/view 模式 | `E4000`、`E4011`-`E4014` |
| `E5xxx` | MIR | lowering 与后端 | `E5101` |
| `W3xxx` | 警告 | FFI 等 | `W3050` |
| `W4xxx` | 覆盖告警 | 模式非穷尽 / 不可达 / 决策表 | `W4200`、`W4201`、`W4301` |

## `E1xxx`：借用与能力

| 错误 | 含义 | 详见 |
| --- | --- | --- |
| `E1001` | 对已 move/drop 的值再次 drop | [所有权与借用](../advanced/01-ownership-and-borrowing.md) |
| `E1004` | 返回的引用不能追溯到输入参数 | 同上 |
| `E1011` | read 能力不足 | 同上 |
| `E1012` | write 能力不足 | 同上 |
| `E1013` | move 能力不足 | 同上 |

排查提示：borrow/loan 冲突诊断携带 `alias trace id`，可到 `*_borrow_aliases` 或 `*_loan_constraint_states` 调试输出按 `id=...` 反查状态。

## `E3xxx`：声明与命名

| 错误 | 含义 | 详见 |
| --- | --- | --- |
| `E3000` | or-pattern 同名绑定 mode 不一致 | [变量绑定与解构](../basics/01-variables-and-bindings.md) |
| `E3004` | overlapping impl registration（canonical 等价或不可比较） | [Trait 与 named instance](../basics/09-traits.md) |
| `E3050` | `@[extern(...)]` 带函数体 | [FFI 与 C 互操作](../advanced/08-ffi.md) |
| `E3051` | FFI 非安全类型 | 同上 |
| `E3061` | public root 包含 toolchain-internal descendant | [复合类型](../basics/05-compound-types.md) |
| `E3203` | 命名字段不存在 | 同上 |
| `E3204` | 仅部分构造器拥有该字段 | 同上 |
| `E3205` | 跨构造器字段索引不一致 | 同上 |
| `E3301` | LLVM 侧未解析字段偏移 | 同上 |

## `E4xxx`：类型

| 错误 | 含义 | 详见 |
| --- | --- | --- |
| `E4000` | 构造器参数类型不匹配；`..` 越界用法提示 | [模式匹配](../basics/07-pattern-matching.md) |
| `E4011` | range 边界无序（`start <= end`） | 同上 |
| `E4012` | range scrutinee 不是 `Int/Char` | 同上 |
| `E4013` | as-pattern 类型不匹配 | 同上 |
| `E4014` | view expression invalid（不可调用为一元函数） | 同上 |

## `E5xxx`：MIR

| 错误 | 含义 | 详见 |
| --- | --- | --- |
| `E5101` | 列表推导式生成器不是 `VarPattern` | [集合类型](../basics/10-collections.md) |

## 告警域

| 错误 | 含义 | 详见 |
| --- | --- | --- |
| `W3050` | 无用 link 指令 | [FFI 与 C 互操作](../advanced/08-ffi.md) |
| `W4200` | 模式匹配不穷尽 | [模式覆盖告警](02-pattern-coverage.md) |
| `W4201` | 不可达分支 / 空匹配 | 同上 |
| `W4301` | decision table 重复字面量 key | [模式匹配](../basics/07-pattern-matching.md) |

## 已移除项

- `noImplicitStdlib` 配置已移除，直接报错（用 `noImplicitPrelude`）；
- `@impl(Trait)` 函数级 attribute 已从 authoring surface 删除，只作为迁移输入识别；
- `view(...)` / `View(...)` 模式写法已移除，使用 `(expr -> pattern)`。

## 调试工具

```powershell
# 查看类型推断 substitution 输出（raw/resolved、绑定链、AST 上下文位置）
eidosc debug projects/test/src/basic/literals.eidos --debug-output projects/test/debug --debug-level diagnostic
```
