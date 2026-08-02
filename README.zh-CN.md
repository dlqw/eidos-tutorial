# Eidos 教程（中文）

> 语言基线：本教程以 Eidos 0.9.0-alpha.1 为准。新代码使用 `name :: Type { ... }`、`name :: expr;`、局部 `name := expr;` / `mut name := expr;`、点号 Namespace 与逗号分隔的 ADT 构造器；旧源码只通过显式迁移命令处理。

本页是教程入口与学习路径导航；正文按主题拆分为独立章节。

## 学习路径

### 序言
- [关于本书](preface/about.md)（教程范围与验证基线、如何阅读）
- [开始前的准备：心智与常见误区](preface/mindset.md)

### 入门准备
1. [安装与验证环境](getting_started/01-installation.md)
2. [Hello World 与项目配置](getting_started/02-hello-world.md)
3. [第一个程序：字面量与绑定](getting_started/03-first-program.md)

### 基础语法（按依赖顺序）
1. [变量绑定与解构](basics/01-variables-and-bindings.md)
2. [基本类型](basics/02-primitive-types.md)
3. [语句与表达式](basics/03-statements-and-expressions.md)
4. [函数](basics/04-functions.md)
5. [复合类型：ADT、类型别名与记录](basics/05-compound-types.md)
6. [流程控制](basics/06-flow-control.md)
7. [模式匹配](basics/07-pattern-matching.md)
8. [泛型](basics/08-generics.md)
9. [Trait 与 named instance](basics/09-traits.md)
10. [集合类型](basics/10-collections.md)
11. [错误处理](basics/11-error-handling.md)
12. [模块与包](basics/12-modules-and-packages.md)
13. [格式化输出](basics/13-formatting-output.md)

### 基础实战
- [文本统计工具（逐步增强）](basic-practice/01-text-stats-tool.md)

### 进阶
1. [所有权与借用](advanced/01-ownership-and-borrowing.md)
2. [效果系统](advanced/02-effects.md)
3. [高阶类型与 kind](advanced/03-higher-kinded-types.md)
4. [值域泛型与 const generics](advanced/04-const-generics.md)
5. [函数式编程](advanced/05-functional-programming.md)
6. [编译期元编程](advanced/06-metaprogramming.md)
7. [Build host 与 BuildGraph](advanced/07-build-host.md)
8. [FFI 与 C 互操作](advanced/08-ffi.md)
9. [重载与自定义运算符](advanced/09-overloads-and-operators.md)

### 进阶实战
- [Snake（FFI + raylib）](advanced-practice/01-snake-raylib.md)
- [BuildGraph 生成项目](advanced-practice/02-buildgraph-project.md)

### 工具链与开发实践
- [eidosc CLI](toolchain/01-eidosc-cli.md)
- [示例验证与测试](toolchain/02-verification-and-testing.md)
- [风格约定](practices/01-style-conventions.md)
- [标准库使用](practices/02-standard-library.md)

### 攻克编译错误
- [错误码体系总览](errors/01-error-code-overview.md)
- [模式覆盖告警：W4200 / W4201](errors/02-pattern-coverage.md)
- [常见陷阱](errors/03-common-pitfalls.md)

### 性能与编译器理念
- [性能与编译器理念](performance/01-compiler-driven-optimization.md)

### 附录
- [语法参考（BNF 摘要）](appendix/grammar.md)
- [关键字与运算符](appendix/keywords-and-operators.md)
- [错误码与验证基线](appendix/verification-baseline.md)
- [Std 导出面索引](appendix/stdlib-index.md)

## 快速开始

```powershell
dotnet build src/Eidosc/Eidosc.sln
powershell -ExecutionPolicy Bypass -File docs/tutorial/verify-examples.ps1
```

详细步骤见 [安装与验证环境](getting_started/01-installation.md)。

## 参考资料

- BNF（中文）：[`BNF.zh-CN.md`](BNF.zh-CN.md) ｜ (English): [`BNF.en.md`](BNF.en.md)
- FFI 详细教程：[`FFI.zh-CN.md`](FFI.zh-CN.md)
- 可执行示例：[`examples/`](examples/)（旧路径映射见 [`EXAMPLES-MAP.md`](EXAMPLES-MAP.md)）
- 版本变更记录：[`changelogs/`](changelogs/)
- 贡献指南：[`CONTRIBUTING.md`](CONTRIBUTING.md)

> 双语说明：中文版为当前维护主线。英文版目前仅有 `README.en.md`（旧版结构的英文镜像，已标注"待同步"）；章节级英文镜像将在结构定稿后逐章翻译补充。
