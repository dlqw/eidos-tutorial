# 模块与包

示例文件：`examples/basics/53_module_exports_and_reexports.eidos`

## 模块声明

模块用点号 Namespace 路径声明，块内是普通声明：

```eidos
Demo.Base :: module {
    export answer :: Int = 40;

    export add_two :: Int -> Int
    {
        x => x + 2
    }

    export Writer :: effect;

    export write :: String -> Int need Writer
    {
        _ => Demo.Base.add_two(Demo.Base.answer)
    }

    secret :: Int -> Int
    {
        x => x + 100
    }
}
```

## 可见性与 `export`

1. 模块内只要出现任意 `export` 声明，外部可见性就切换到"显式导出模式"；未标记 `export` 的声明继续可在模块内部使用，但不会再自动暴露给外部（上面的 `secret` 就是模块私有）；
2. 若模块内完全没有 `export`，当前仍保持兼容性的"隐式全部导出"行为；
3. `export` 现在可前缀普通声明与 `import`：例如 `export let`、`export let mut`、`export func`、`export effect`、`export trait`、`export type`、`export import`。

## import 与 re-export

```eidos
Demo.Facade :: module {
    export BaseApi :: import Demo.Base;
    export import Demo.Facade.{Writer as W, write}
}
```

1. `Alias :: import Module.Path;` 是 name-first 绑定形态（0.4.0-alpha.1）；旧 keyword 形式 `import Module.Path as Alias` 只在 `legacy` 语法模式中接受；
2. `export import Demo.Base.{Writer as W}` 会把 selective import 的 alias 一并公开；`export import Demo.Base.*` 则把 imported module 当前可见的公开绑定整体转发出去；
3. `export import` 走真实模块路径和导出表，而不是旧的字符串启发式，因此 alias re-export、qualified path、IDE completion 与预编译导出表都共享同一套可见性语义。

使用侧：

```eidos
Demo.Main :: module {
    import Demo.Facade
    import Demo.Facade.{W}

    run :: String -> Int need Facade.W
    {
        text => Facade.write(text)
    }
}
```

`import Demo.Facade` 后可以按模块路径调用 `Facade.write`；`need Facade.W` 引用 re-export 的 effect alias（效果授权见进阶 [效果系统](../advanced/02-effects.md)）。

## 标准库导入

`import std.Option` / `import std.Result` / `import std.Seq` 等把模块公开成员带入当前作用域；`Module.function` 限定路径始终可用：

```eidos
import std.Option

fallback :: Int? -> Int {
    value => value ?? 42
}
```

## 包与 manifest

`eidos.toml` 中的 `manifestSchema = 3`、`[language].version` 与 `[package]` 见 [Hello World 与项目配置](../getting_started/02-hello-world.md)；`[dependencies] std = "0.1.0-alpha.1"` 引入显式标准库。

## 模块导出表的实时查看

```powershell
dotnet run --project Eidosc/src/Eidosc.Cli -- info --stdlib
```

输出显式 package 的实时导出面。
