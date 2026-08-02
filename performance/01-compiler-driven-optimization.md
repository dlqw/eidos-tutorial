# 性能与编译器理念

性能章节不讲手工微优化（手动管理内存、选择容器、做微操作）。Eidos 的理念是**优化负担下沉到编译器**，程序员保留表达自由。

## 编译器承担什么

Eidos 的核心承诺（产品理念）：

- **表达自由**：递归/迭代、命令式/函数式、高阶类型、模式匹配及自然的数据结构写法，都应获得可靠且可预测的编译器优化；
- **内存由编译器管理**：内存分配、释放、栈提升、标量化和唯一所有权复用由编译器承担，程序员不需要 Box/Arc 之类的手工内存策略；
- **表示选择下沉**：容器选择（布局、增长策略、专用化、性能策略）由编译器决定，标准库只提供最少且最正交的用户抽象；
- **唯一所有权复用**：编译器推导所有权契约（`ByValue` / `SharedBorrow` / `MutableBorrow`），并在此基础上做复用与提升。

因此，在 Eidos 里"性能优化"章节回答的是两个问题：**编译器已经做了什么**，以及**怎么写代码才能让编译器做出可预测的优化**。

## 已实现的编译期机制

当前编译器已验证的机制（全部有对应教程章节与示例）：

| 机制 | 位置 |
| --- | --- |
| MIR CFG lowering 与 slot 语义（entry `alloca` + `load/store`，避免循环回边读到初始 alias） | [集合类型](../basics/10-collections.md) 列表推导式 |
| 列表推导式静态/动态来源统一 CFG + runtime `array_*` API 落地 | 同上 |
| 预编译 Prelude Core Image（Prelude 自动 open，核心契约不重复实例化） | [集合类型](../basics/10-collections.md) |
| 泛型特化、trait coherence 与增量缓存键包含值参数（const generics 进入 identity/layout/mangling） | [值域泛型与 const generics](../advanced/04-const-generics.md) |
| BuildGraph 能力约束构建：输入/环境/工具 hash 进入 cache key，output hash 验证复用 | [Build host 与 BuildGraph](../advanced/07-build-host.md) |
| 跨模块摘要、增量指纹与编译缓存状态参与 effect 行与借用契约版本化 | [效果系统](../advanced/02-effects.md)、[所有权与借用](../advanced/01-ownership-and-borrowing.md) |
| Native smoke 的 clang-only 链路（自动注入 `main -> eidos_main` 入口桥接） | [集合类型](../basics/10-collections.md) |

## 如何写出可预测性能的代码

1. **优先写可直接推断的简单表达式**，复杂组合拆成多个 `let`；
2. **用语义选抽象，不用布局选抽象**：`Option` / `Result` / `Seq` / `Either` 按语义选，实现选择交给编译器；
3. **优先使用函数式读法**（`|>`、`>>>`、链式 `.map(...).filter(...)`），风格约定本身就是编译器最容易优化与特化的形态（见 [函数式编程](../advanced/05-functional-programming.md)）；
4. **显式冻结契约**：导出 API 显式写 `need`、instance 用显式 trait 实参、借用靠 typed signature——契约越明确，编译器可做的特化/缓存越多；
5. **让常量在编译期可见**：值参数（const generics）、`comptime` 绑定、`@[expand(...)]` 生成——这些进入 identity 与缓存键，编译器可以提前展开（见 [编译期元编程](../advanced/06-metaprogramming.md)）。

## 不要做什么

- 不要手工内存布局、手工 allocator、`unsafe` 式微操——Eidos 没有把它们作为性能入口；
- 不要在同类容器之间挑选——那是编译器的工作；
- 不要为了性能牺牲可读性——如果性能确实成为问题，正确方向是报告编译器（增强编译器优化），而不是改写你的代码结构。

## 未来方向

栈提升、标量化、唯一所有权复用的完整实现仍在收口（当前还不是最终版内存模型，见 [所有权与借用](../advanced/01-ownership-and-borrowing.md) 的边界说明）；教程只描述已验证行为，编译器优化进展会随版本同步更新到对应章节。
