# Eidos Tutorial / Eidos 教程

Eidos 语言官方教程（语言基线 `0.8.0-alpha.1`）。

- 中文版：[`README.zh-CN.md`](README.zh-CN.md)
- English: [`README.en.md`](README.en.md)

## 教程结构

| 部分 | 章节 | 内容 |
| --- | --- | --- |
| 序言 | [`preface/`](preface/about.md) | 关于本书、心智与常见误区 |
| 入门准备 | [`getting_started/`](getting_started/01-installation.md) | 环境安装、Hello World、字面量与绑定 |
| 基础 | [`basics/`](basics/01-variables-and-bindings.md) | 绑定/类型/表达式/函数/复合类型/流程/模式匹配/泛型/trait/集合/错误/模块/输出（13 章） |
| 基础实战 | [`basic-practice/`](basic-practice/01-text-stats-tool.md) | 文本统计工具（逐步增强） |
| 进阶 | [`advanced/`](advanced/01-ownership-and-borrowing.md) | 借用/效果/高阶类型/const generics/函数式/元编程/BuildGraph/FFI/重载（9 章） |
| 进阶实战 | [`advanced-practice/`](advanced-practice/01-snake-raylib.md) | Snake（FFI+raylib）、BuildGraph 项目 |
| 工具链 | [`toolchain/`](toolchain/01-eidosc-cli.md) | eidosc CLI、示例验证与测试 |
| 开发实践 | [`practices/`](practices/01-style-conventions.md) | 风格约定、标准库使用 |
| 攻克编译错误 | [`errors/`](errors/01-error-code-overview.md) | 错误码总览、模式覆盖告警、常见陷阱 |
| 性能 | [`performance/`](performance/01-compiler-driven-optimization.md) | 性能与编译器理念 |
| 附录 | [`appendix/`](appendix/grammar.md) | 语法参考、关键字与运算符、错误码与验证基线、Std 导出面 |

## 参考资料

- BNF（中文）：[`BNF.zh-CN.md`](BNF.zh-CN.md) ｜ (English): [`BNF.en.md`](BNF.en.md)
- FFI 详细教程：[`FFI.zh-CN.md`](FFI.zh-CN.md)
- 可执行示例：[`examples/`](examples/)（旧路径映射见 [`EXAMPLES-MAP.md`](EXAMPLES-MAP.md)）
- 验证脚本：[`verify-examples.ps1`](verify-examples.ps1)
- 版本变更记录：[`changelogs/`](changelogs/)

## 仓库

- 贡献指南：[`CONTRIBUTING.md`](CONTRIBUTING.md)
- 安全策略：[`SECURITY.md`](SECURITY.md)
- License：[`LICENSE`](LICENSE)
