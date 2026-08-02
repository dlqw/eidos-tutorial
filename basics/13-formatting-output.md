# 格式化输出

示例文件：`examples/stdlib/29_precompiled_stdlib.eidos`、`examples/stdlib/42_stdlib_safe_and_traits.eidos`

## `print` / `println` 与 `Display`

Eidos 0.8 使用 `Display` 驱动的 `print` / `println`。它们是普通的 `Display` trait 约束重载，并非编译器特判：

```eidos
main :: Unit -> Int need io {
    print(Some(42));
    println();
    0
}
```

基础类型和核心 ADT 都提供 `Display` instance，因此 `Int`、`Float`、`String`、`Bool`、`Char`、`Option`、`Result`、`Ordering` 等都可以直接打印：

```eidos
option_shown := show(Some(8));          // "Some(8)"
result_shown := show(Ok(7));            // "Ok(7)"
ordering_shown := Ordering.show(compare(1)(2));  // "Less"
```

`show(...)` 返回 `String`；需要 `String` 形式的文本（拼接、比较、断言）时优先用 `show` 而不是打印。

## 字符码输出边界 API

按字符码输出使用显式边界 API `Console.write_char_code`（例如 `34` 为 `"`，`39` 为 `'`）。底层带类型后缀的输出 intrinsic 不属于用户 API。

## 输出与 effect

`print` / `println` 需要 `io` effect 授权（`main :: Unit -> Int need io`）；effect 系统见进阶 [效果系统](../advanced/02-effects.md)。
