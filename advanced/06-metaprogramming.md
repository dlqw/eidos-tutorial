# 编译期元编程

示例文件：`examples/meta/64_constructor_constant_derive.eidos`、`examples/meta/69_meta_reflection_derive.eidos`

## `comptime` 与 `meta` 域

`meta` 是编译器内建域，不属于 `Std`，也不需要 `import`。反射只返回已经完成且可稳定序列化的编译器事实，不暴露可变 AST 或内部 `SymbolId`：

```eidos
USER_INFO :: comptime meta.shape_of(User);
USER_KIND :: comptime meta.kind_of(USER_INFO);
HAS_NAME :: comptime meta.has_field(User, "name");
NAME_TYPE :: comptime find_field_type(meta.find_field(User, "name"));
INT_LAYOUT :: comptime meta.layout_of(Int, "x86_64-pc-windows-msvc");
```

- `meta.shape_of` 覆盖 primitive、tuple、function、reference、ADT 与 trait；构造器、字段、关联声明和属性保持稳定源码顺序；
- `meta.layout_of` 是独立的 target-dependent 查询，必须显式给出受支持的 target triple；尚未完成布局的类型会产生 phase diagnostic，不会退回 host layout。

编译期值使用大写开头标识符（`USER_INFO`、`NAME_TYPE`），与运行时小写命名分层（见入门 [第一个程序](../getting_started/03-first-program.md)）。

## 用户 derive 与结构化生成

自定义生成器使用编译器管理的 `comptime meta.Type -> meta.Items` 协议，并通过 typed `@[expand(...)]` 标签附着：

```eidos
Marker :: trait {
    marker :: Self -> String
}

derive_marker :: comptime meta.Type -> meta.Items {
    target => {
        parameter := meta.parameter("value", target);
        method := meta.function(
            "marker",
            [parameter],
            String,
            meta.expr_string(meta.name_of(target))
        );
        [meta.instance(meta.declaration_of(Marker), target, [method])]
    }
}

@[expand(derive_marker)]
User :: type
{
    name:: String,
    age:: Int
}
```

`meta.Items` 包含结构化生成声明。`meta.Function -> meta.Function` 用于函数体变换，`meta.Syntax[K] -> meta.Syntax[K]` 用于 syntax-site expansion。生成声明进入普通名称解析、类型检查、trait coherence、completion、hover、definition 与 references，并带稳定 `eidos-generated://` origin。

当前边界：不支持字符串源码插入、任意 AST 替换、公开调度 clause 或公开 ownership attribute；pure comptime 不能读取文件、环境、进程、网络或 FFI。编译器根据 typed protocol 与 declaration tag 自动推导依赖顺序、fixed point、identity、cache、diagnostic 和 provenance。

## 可用 CLI

```powershell
eidosc meta expand source.eidos --format json
eidosc meta expand source.eidos --emit-generated generated
eidosc meta expand source.eidos --trace-comptime --comptime-budget 200000
```

## 派生 trait

`@[derive(Eq), derive(Show)]` 为类型生成派生实现（见基础 [复合类型](../basics/05-compound-types.md)）。构造器常量派生示例见 `examples/meta/64_constructor_constant_derive.eidos`。
