# 进阶实战 1：Snake（FFI + raylib）

示例项目：`ffi_c_interop/snake/snake.eidos`（C shim 见 `ffi_c_interop/snake/snake_shim.c`）

本项目把进阶章节的能力串成一个完整游戏：**FFI 声明**（[FFI 与 C 互操作](../advanced/08-ffi.md)）驱动 raylib，**游戏循环**用纯函数式状态流转，**输入事件**用布尔模式匹配分类。

## 项目结构

```
ffi_c_interop/snake/
├── snake.eidos       # Eidos 游戏逻辑
└── snake_shim.c      # C shim（把 raylib 调用封装成稳定 ABI 的 C 函数）
```

教程根 `eidos.toml` 的 `[ffi]` 段声明链接库：

```toml
[ffi]
libraries = ["raylib", "snake_shim"]
```

## 声明外部函数

```eidos
import std.Ffi

@[extern(c, library: "raylib", name: "InitWindow")]
init_window :: Int -> Int -> RawPtr -> Unit need ffi;

@[extern(c, library: "raylib", name: "WindowShouldClose")]
window_should_close :: Unit -> Bool need ffi;

@[extern(c, library: "snake_shim", name: "shim_draw_rectangle")]
draw_rectangle :: Int -> Int -> Int -> Int -> Int -> Int -> Int -> Int -> Unit need ffi;
```

要点：

- `@[extern(c, library: ..., name: ...)]` 指定 C 符号与宿主库；不写 `name` 时默认用函数名；
- 无函数体的 extern 声明**必须**显式写 `need ffi`（[效果系统](../advanced/02-effects.md)：无函数体声明无法从实现推断 effect）；
- 可变游戏状态通过 FFI 指针存储（shim 侧管理），Eidos 侧保持纯函数流转，避免所有权/借用问题（[所有权与借用](../advanced/01-ownership-and-borrowing.md)）。

## 游戏循环

主循环模式：`window_should_close()` 返回 `Bool`，用 `if ... then { ... } else { ... }` 或布尔守卫驱动每帧处理；键盘输入用 `is_key_pressed(code)` 的布尔结果分类事件（[模式匹配](../basics/07-pattern-matching.md) 的"先分类一次事件，再匹配稳定值"建议）。

完整游戏逻辑见 `snake.eidos`（含随机食物、蛇身移动、碰撞判定；`shim_get_random_value` 通过 shim 提供确定性随机源）。

## 构建与运行

raylib 与 shim 需要本地 C 工具链（clang）与 raylib 库：

```powershell
# 先编译 shim 并链接 raylib（见 snake_shim.c 头部注释），然后：
dotnet run --project src/Eidosc/Eidosc.Cli -- build --project ffi_c_interop/snake --target-name main --trace-build
```

> 说明：Snake 项目需要 raylib 环境，教程 CI 不执行构建；`verify-examples.ps1` 只验证 `examples/` 下的示例与 `examples/build_host`。

## 练习

1. 给 `draw_rectangle` 换一种配色逻辑，验证 FFI 参数按顺序传递；
2. 把 `is_key_pressed` 的四方向处理改写为 decision table（[模式匹配](../basics/07-pattern-matching.md)）；
3. 用 `Ffi.cfn_from` 把 Eidos 函数传给 shim 侧回调，替换 `shim_get_random_value`（[FFI 与 C 互操作](../advanced/08-ffi.md)）。
