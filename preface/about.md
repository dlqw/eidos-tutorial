# 关于本书

本书是 Eidos 语言的官方教程，当前以 **Eidos 0.8.0-alpha.1** 为语言基线。新代码使用 `name :: Type { ... }`、`name :: expr;`、局部 `name := expr;` / `mut name := expr;`、点号 Namespace 与逗号分隔的 ADT 构造器；旧源码只通过显式迁移命令处理。

## 教程范围与验证基线

本教程只描述**当前仓库中已经实现并可复现的功能**。所有可运行的示例均在 [`examples/`](../examples/) 目录下，按主题组织：

| 示例目录 | 主题 | 对应章节 |
| --- | --- | --- |
| `examples/getting_started/` | 字面量与绑定 | 入门准备 |
| `examples/basics/` | 基础语法：函数/表达式/ADT/模块/错误处理 | 基础 1-6、11-13 章 |
| `examples/pattern/` | 模式匹配与覆盖分析 | 基础 7 章 |
| `examples/traits/` | Trait 与 named instance | 基础 9 章 |
| `examples/generics/` | 泛型、kind、const generics | 基础 8 章 / 进阶 3-4 章 |
| `examples/ownership/` | 借用与所有权契约 | 进阶 1 章 |
| `examples/effects/` | 效果系统 | 进阶 2 章 |
| `examples/functional/` | 函数式组合子与风格 | 进阶 5 章 |
| `examples/meta/` | 编译期元编程与 derive | 进阶 6 章 |
| `examples/ffi/` | FFI 与 C 互操作 | 进阶 8 章 |
| `examples/stdlib/` | 标准库使用 | 开发实践 |
| `examples/advanced/` | 重载与自定义运算符 | 进阶 9 章 |
| `examples/build_host/` | BuildGraph 项目 | 进阶 7 章（独立验证） |

示例文件的旧编号到新路径的完整映射见 [`EXAMPLES-MAP.md`](../EXAMPLES-MAP.md)。

语法权威来源为 `src/Eidosc/Eidosc/Grammar/GrammarDefine.cs`，验证基线为 2026-03-19 的本地执行结果。核心 BNF 摘要见附录 [语法参考](../appendix/grammar.md)，英文版见 [`BNF.en.md`](../BNF.en.md)。

## 如何阅读

教程按 rust-course 式的学习路径组织，分为七个部分：

1. **序言与入门准备**（本书 + [开始前的准备](mindset.md) + 环境安装与第一个程序）
2. **基础语法**（`basics/`，13 章，按依赖顺序组织）
3. **进阶主题**（`advanced/`，9 章：借用、效果、高阶类型、const generics、函数式编程、元编程、BuildGraph、FFI、重载）
4. **工具链与测试**（`toolchain/`：eidosc CLI、manifest、示例验证）
5. **开发实践与标准库**（`practices/`）
6. **攻克编译错误**（`errors/`：按错误码域组织的诊断指南）
7. **性能与编译器理念**（`performance/`）与**附录**（`appendix/`：BNF、错误码总表、Std 导出面）

每章正文中内嵌的代码与 `examples/` 下的可执行示例一一对应；运行全量验证：

```powershell
powershell -ExecutionPolicy Bypass -File verify-examples.ps1
```

> 说明：英文版暂未同步，后续会补上。
