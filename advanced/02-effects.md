# 效果系统

示例文件：`examples/effects/09_effect_tag_call.eidos`、`examples/effects/51_qualified_effect_paths.eidos`、`examples/effects/52_nested_qualified_effect_paths.eidos`

## effect 声明

Effect 声明是 nominal 编译期标记，不包含 operation，也没有运行时表示：

```eidos
Logger :: effect;

log :: String -> Int need Logger
{
    _ => 0
}
```

## `need` 授权

1. 有函数体且省略 `need` 时，编译器从函数体和递归调用图推断精确 effect row，并把它作为函数的有效 effect 类型；调用方不需要重复手写 `need`；
2. 显式 `need` 是受检查的公开上界。建议在导出 API 和封装 effect 的底层包装函数上显式书写，以冻结契约并阻止实现意外扩大 effect；trait 方法、external/FFI 等无函数体声明无法从实现推断，因此必须显式书写；
3. Effect 不拥有函数。`Io.Writer` 等限定 effect 路径只用于 `need`；普通函数按模块路径调用，例如 `Io.write(text)`。自定义 effect 用于区分 `io` 之上的领域能力，例如把键盘读取包装为 `need io, Input`；只声明而不把它附着到任何边界函数没有语义作用；
4. LSP 会在省略处虚拟显示灰色 `need ...`，并提供 materialize code action；源码仍保持不变，直到用户选择落盘。

## 多态 effect 行

高阶 API 使用 `E: effects` 行参数：

```eidos
apply[A, B, E: effects] :: (A -> B need E) -> A -> B need E
```

固定行与多态行可组合，例如 `need ffi, E`。

## 限定 effect 路径

限定 effect 路径与普通函数路径独立解析。在 `need` 中使用 `Logger.Logger`、`Io.Writer` 或 `Cap.Io.Writer`；普通函数调用写作 `Logger.log(...)`、`Io.write(...)` 或 `Cap.Io.write(...)`：

```eidos
Demo.Logger :: module {
    Logger :: effect;

    log :: String -> Unit need Logger
    {
        _ => ()
    }

    helper :: Int -> Unit need Logger.Logger
    {
        _ => Logger.log("hello")
    }

    main :: Unit -> Unit need Logger.Logger
    {
        _ => helper(0)
    }
}
```

嵌套限定路径（如 `Cap.Io.Writer`）见 `examples/effects/52_nested_qualified_effect_paths.eidos`。

## 运行时语义

1. Effect 变量和推断行会参与类型、跨模块摘要、增量指纹与编译缓存状态；
2. Effect 在运行前擦除；语言不提供 handler、`with`、`resume`、CPS 重写或运行时 effect dispatch；
3. Borrow 检查与 effect 授权独立；旧 `@borrow(...)` 即使仍作为迁移输入被识别，也不会授予 read、write 或 move 权限（见 [所有权与借用](01-ownership-and-borrowing.md)）。

## 常见用法

- `main :: Unit -> Int need io`：入口函数声明 IO 授权；
- `need ffi`：FFI 边界（见 [FFI 与 C 互操作](08-ffi.md)）；
- 在模块内 re-export effect alias 供外部使用（见基础 [模块与包](../basics/12-modules-and-packages.md)）。
