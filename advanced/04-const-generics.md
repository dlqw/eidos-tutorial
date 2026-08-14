# 值域泛型与 const generics

示例文件：`examples/generics/68_const_generics.eidos`

## `comptime` 参数域

`comptime N: Int` 声明值域泛型参数；`comptime T: Type` 仍声明类型域参数。参数顺序就是应用顺序，编译器在 AST、symbol、HIR/MIR、缓存和 IDE 中保留 type/value/effect-row 三种 domain，不再把值参数伪装成类型参数：

```eidos
Buffer[comptime N: Int, comptime T: Type] :: type {
    Buffer(T)
}

FixedBuffer[comptime N: Int, comptime T: Type] :: type = Buffer[N, T];

constant[comptime N: Int] :: Unit -> Int
{
    _ => N
}

use :: Unit -> Int
{
    _ => constant[4](()) + constant[5](())
}
```

## 实例化规则

1. 值实参必须在实例化点可编译期求值，并且类型与参数注解一致；
2. 未被普通参数类型约束的值参数必须显式提供；能从 `Buffer[N, T]` 这类参数/结果类型推出时可以省略；
3. 值会进入 nominal type identity、layout、name mangling、generic specialization、trait coherence 和增量缓存键，因此 `Buffer[4, Int]` 与 `Buffer[5, Int]` 是不同类型；
4. 浮点值在 alpha.1 中不能作为 specialization key；引用、指针、closure 和其他带运行期资源身份的值也不能越过 comptime/type identity 边界。

## 与 trait 组合

ADT、type alias、函数、trait 和 named instance trait 引用均使用同一套有序 generic argument 规则。例如 `Sized[comptime N: Int] :: trait { ... }` 的实现应显式写成 `SizedHolder :: instance Sized[4]`；trait 方法签名中的 `N` 会按该值替换，coherence 也会比较结构化值 key。
