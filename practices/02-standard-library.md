# 标准库使用

示例文件：`examples/stdlib/29_precompiled_stdlib.eidos`、`examples/stdlib/42_stdlib_safe_and_traits.eidos`、`examples/stdlib/65_game_math_vectors.eidos`

## Prelude 与 `std` 的分工

- **Prelude Core Image**：不是 package，自动 open，承载语言 elaboration 所需的核心函数式契约与类型（`Display`、`Option`、`Result`、`Either`、`Ordering`、`Seq`、`Functor`、`Applicative`、`Monad`、`Foldable`、`Traversable`、`Semigroup`、`Monoid`、`Alternative`）。见基础 [集合类型](../basics/10-collections.md)。
- **显式 `std` package**：非核心能力，需要 `[dependencies] std = "0.1.0-alpha.1"`。

## 能力分组

| 能力 | 显式模块 | 代表接口 |
| --- | --- | --- |
| 数学与几何 | `std.Math`、`std.FloatMath`、`std.GameMath` | 重载的 `Math.abs`、`GameMath.add`、`GameMath.scale` |
| 控制台 IO | `std.Console` | 泛型 `Console.write`、`Console.write_line`、`Console.read_line`、`Console.write_char_code` |
| 文本与文件 | `std.Text`、`std.File` | `Text.trim`、`File.read_text`、`File.write_text` |
| 容器 | `std.SeqBuilder`、map、set、queue、stack | builder 与专用容器操作 |
| 网络与序列化 | `std.Network`、`std.Binary`、`std.Json`、`std.JsonParser`、`std.JsonValue` | HTTP、二进制 codec、JSON 构造与解析 |

声明默认私有，只有 `export` 才形成跨模块 API（见基础 [模块与包](../basics/12-modules-and-packages.md)）。

## 游戏数学示例

```eidos
import std.FloatMath
import std.GameMath
import std.Math

main :: Unit -> Int need ffi
{
    _ => {
        board := GameMath.ivec2(20)(10);
        point := GameMath.ivec2(-1)(12);
        wrapped := GameMath.wrap(point)(board.x)(board.y);
        velocity := GameMath.vec2(3.0)(4.0);
        rotated := GameMath.rotate_degrees(GameMath.unit_x)(90.0);
        moved := GameMath.move_toward(GameMath.zero)(GameMath.vec2(10.0)(0.0))(2.5);

        ints_ok :=
            Math.wrap(-1)(20) == 19 &&
            Math.align_up(33)(8) == 40 &&
            wrapped == GameMath.ivec2(19)(2);

        floats_ok :=
            FloatMath.smoothstep(0.0)(10.0)(5.0) == 0.5 &&
            FloatMath.angle_delta_degrees(350.0)(10.0) == 20.0 &&
            FloatMath.approx_eq(GameMath.length(velocity))(5.0)(0.000001) &&
            GameMath.approx_eq(rotated)(GameMath.vec2(0.0)(1.0))(0.000001) &&
            FloatMath.approx_eq(moved.x)(2.5)(0.000001);

        if ints_ok && floats_ok then { 1 } else { 0 }
    }
}
```

完整可运行版本见 `examples/stdlib/65_game_math_vectors.eidos`。

## 命名原则

参数类型可区分的接口统一使用重载而非类型后缀（`Ordering.compare`、`Hash.hash`、`Hash.mix_value`、`Console.write`）。只靠返回类型区分、转换、带单位、字符码点以及 raw/safe FFI 边界继续使用语义名，避免歧义和安全含义丢失。

## 查看导出面

```powershell
dotnet run --project Eidosc/src/Eidosc.Cli -- info --stdlib
```

输出显式 package 的实时导出面，是标准库 API 的权威索引。
