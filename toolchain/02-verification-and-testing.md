# 示例验证与测试

## 验证脚本机制

`verify-examples.ps1` 是教程的自动验证门禁：

1. 递归扫描 `examples/` 下所有 `*.eidos`（`examples/build_host/` 除外，作为独立 BuildGraph 项目单独验证）；
2. 每个示例默认执行 `analyze <file> --phase hir --deny style`；
3. 特殊预期按文件名识别：
   - `06_nested_call_parser_only.eidos` → 只要求到 `parser` 阶段；
   - `36_hkt_trait_constraint_kind_mismatch.eidos` → 预期失败，并校验错误文本包含 `kind`；
4. `examples/build_host` 执行 `build --target typed --no-cache --emit-build-graph`，校验产物存在。

```powershell
powershell -ExecutionPolicy Bypass -File verify-examples.ps1          # 默认根目录
powershell -ExecutionPolicy Bypass -File verify-examples.ps1 -NoBuild  # 复用已构建 CLI
powershell -ExecutionPolicy Bypass -File verify-examples.ps1 -CliProject <path> -Root <path>
```

## 新增或修改示例

1. 示例使用当前基线语法（0.8：`name :: expr;`、`name := expr;`、`Ctor:: type(...)`）；
2. 在提交前跑通 `verify-examples.ps1`；
3. 语法或语义变化必须同步：教程正文（`preface/`、`getting_started/`、`basics/`、`advanced/` 等）、`examples/`、编辑器插件（`tools/editor/*`）、fixture（`projects/test/src`）与 BNF 附录；
4. 示例重命名或移动后更新 [`EXAMPLES-MAP.md`](../EXAMPLES-MAP.md)。

## 发布元数据验证

```powershell
powershell -ExecutionPolicy Bypass -File scripts/verify-release-metadata.ps1
```

校验版本权威源（`eidos-language.toml`）、changelog 与 release metadata 一致性。

## 贡献流程

教程仓库采用短生命周期分支 + PR（squash merge）流程，commit subject 遵循 Conventional Commits：

```text
docs: 描述性标题
docs!: 破坏性文档变更（语法/语义变化必须使用 ! 或 BREAKING CHANGE）
```

语法或语义变更必须与兼容的 Eidosc 发布协调（`eidos-language.toml` 的 `[compatibility]` 段声明已验证的 Eidosc 范围）。不要提交编译后的 native 库、可执行文件、编辑器状态、日志、凭据或私有开发记录。
