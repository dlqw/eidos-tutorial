# 复合类型：ADT、类型别名与记录

示例文件：`examples/basics/04_adt_type_alias.eidos`、`examples/basics/66_default_ctor_sugar.eidos`

## ADT 声明（0.8 语法）

代数数据类型（ADT）由类型名与逗号分隔的构造器（case）组成：

```eidos
Option[T] :: type {
    Some:: type(T),
    None :: type {}
}
```

- `Some:: type(T)`：命名构造器 `Some`，载荷为 `T`；
- `None :: type {}`：无载荷构造器；
- 构造器调用 `Some(1)` 产生运行时值，但构造器符号本身是编译期值（大写命名空间）。

位置参数形态也支持（见 [Trait 与 named instance](09-traits.md) 中的 `type Point { Point(Int, Int) }` 历史形态），当前教程示例以 `Ctor:: type(...)` 命名载荷形态为准。

## 裸积类型与默认构造器糖

类型体直接写字段，编译期自动合成与类型同名的默认构造器：

```eidos
Point :: type {
    x:: Int,
    y:: Int
}
```

等价于 `Point :: type { Point { x: Int, y: Int } }`。使用：

```eidos
origin :: Point { x: 0, y: 0 };

make :: Int -> Int -> Point
{
    x => y => Point { x: x, y: y }
}

sum_coords :: Point -> Int
{
    p => p.x + p.y
}
```

`@[derive(Eq), derive(Show)]` 属性可以为类型自动生成派生 trait 实现：

```eidos
@[derive(Eq), derive(Show)]
Record :: type
{
    name:: Int,
    age:: Int
}
```

## 类型别名

```eidos
UserId :: type = Int;

uid :: UserId = 7;
```

别名是类型层面的编译期绑定；`UserId :: type = Int` 使 `UserId` 与 `Int` 在类型系统中等价。

## 记录更新

短记录更新语法 `base.{ field: value }` 从 base 值复制未显式填写的字段，后续显式字段覆盖：

```eidos
reset_tick :: GameState -> GameState
{
    state => { state.{ tick: 0 } }
}
```

- `.{...}` 前面的 base 表达式提供要复制的 ADT record 值；
- 对拥有多个 record 构造器的 ADT，短更新保留运行时构造器，并且只在每个 record 构造器都拥有被更新字段时接受；
- 显式构造器形式：`GameState { ..state, tick: 0 }`，`..base` 必须在命名构造字段列表开头出现一次；
- Eidos 不使用 Koka 风格调用式更新（例如 `state(tick = 0)`）。

## 记录字段访问约束

ADT 命名字段访问要求"在该 ADT 内可唯一定位且对所有构造器总是可用"：

- 跨构造器索引不一致会报 `E3205`；
- 仅部分构造器拥有该字段会报 `E3204`；
- 字段不存在会报 `E3203`；
- LLVM 侧遇到未解析字段偏移会报 `E3301`（不再静默回落）。

## 闭包 case hierarchy（0.7 基线）

嵌套的 `Case :: type` 构成词法封闭的 closed-case hierarchy，每个嵌套 case 是一等 exact nominal subtype：

```eidos
Message :: type {
    Text :: type {
        value :: String,
    },

    Control :: type {
        code :: Int,
    },
}
```

完整 hierarchy 使用原子可见性：导出 `Message` 会同时导出穷尽匹配所需的所有 descendant case type、constructor 与 case field；root 未导出或由编译器标记为 internal 时，整棵 hierarchy 保持私有。public root 不能包含 toolchain-internal descendant（`E3061`）。Eidos 0.7 不会根据 descendant visibility 隐式推导 non-exhaustive 或 opaque-case 模式。

## GADT 与构造器局部类型参数

GADT 构造器可以显式声明返回当前 ADT 的特化实例；没有 `->` 的普通 ADT 构造器仍等价于返回声明 head 的完整实例：

```eidos
type Expr[T]
{
    IntLit(Int) -> Expr[Int],
    BoolLit(Bool) -> Expr[Bool]
}

classify[T] :: Expr[T] -> Int
{
    expr => match expr
    {
        IntLit(value) => value,
        BoolLit(flag) => if flag then { 1 } else { 0 }
    }
}
```

`TypeEq[A, B]` 是内建擦除类型；首版只提供 `Refl`，用于同一类型或 GADT 模式分支局部精化产生的类型等式证据。

构造器也可以声明局部类型参数；如果没有出现在返回类型中，它就是 existential，构造后会隐藏，只能通过模式匹配在分支内重新打开：

```eidos
type AnyDirection
{
    AnyDirection[A](Direction[A]) -> AnyDirection
}

dx_any :: AnyDirection -> Int
{
    AnyDirection(dir) => dir.dx()
}
```

> 注：GADT 与 existential 部分以 README 基线文档为准，示例文件覆盖见 `examples/` 相关主题；教程只描述已实现且可复现的功能，验证状态以 `verify-examples.ps1` 为准。

## 下一步

- 模式匹配这些类型：见 [模式匹配](07-pattern-matching.md)；
- 为 ADT 声明 trait 实现（构造器事实 bridge）：见 [Trait 与 named instance](09-traits.md)。
