# 重载与自定义运算符

示例文件：`examples/advanced/67_function_overloads.eidos`、`examples/advanced/59_custom_symbolic_operators.eidos`

## 函数重载

普通同一作用域函数可以同名，只要参数签名不同。调用点会根据实参类型选择重载，覆盖普通调用、限定路径调用、链式方法调用、中缀调用和管道调用：

```eidos
format :: Int -> String
{
    _ => "int"
}

format :: String -> String
{
    text => text
}

format[T] :: T -> String
{
    _ => "generic"
}

format_int :: Int -> String = format;

join :: Int -> Int -> String
{
    _ => _ => "ints"
}

join :: String -> String -> String
{
    left => right => left ++ right
}

main :: Unit -> String
{
    _ => format(1) ++ " " ++ "ok".format() ++ " " ++ (1 |> format) ++ " " ++ (1 `join` 2) ++ " " ++ format_int(2)
}
```

规则：

1. 裸重载函数引用必须有期望函数类型；否则编译器会报告歧义（`format_int :: Int -> String = format;` 通过期望类型消除歧义）；
2. 重复重载只看参数签名：返回类型、能力需求、声明顺序和函数体形态都不能让两个重载变成不同声明（`parse :: String -> Int` 与 `parse :: String -> String` 会被拒绝为重复重载）；
3. 泛型签名会做 alpha 归一化，所以 `id[T] :: T -> T` 与 `id[U] :: U -> U` 也是重复重载。

## 自定义符号运算符

符号函数可以用带括号的运算符名声明，并直接作为中缀运算符使用：

```eidos
(|+|) :: Int -> Int -> Int
{
    left => right => left + right + 10
}

main :: Unit -> Int
{
    _ => {
        infixed := 1 |+| 2;
        prefixed := (|+|)(3, 4);
        infixed + prefixed
    }
}
```

- 自定义符号运算符默认左结合，优先级位于加减/拼接之后、函数组合之前；
- `::` 是声明绑定 token，不是表达式运算符；
- 内建复杂运算符如 `|>`、`>>=`、`>>>`、`<<<`、`<$>`、`<*>`、`<>` 保持各自固定优先级与标准库 lowering。

## 字符串拼接

`++` 是字符串拼接运算符（`left ++ right`），`format_int(2) ++ " "` 这类组合在 [函数式编程](05-functional-programming.md) 与标准库章节中广泛使用。
