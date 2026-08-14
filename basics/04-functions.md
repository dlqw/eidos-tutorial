# 函数

示例文件：`examples/basics/02_functions_calls.eidos`、`examples/basics/30_curried_pattern_branch.eidos`、`examples/basics/03_chain_method_calls.eidos`、`examples/basics/06_nested_call_parser_only.eidos`、`examples/basics/71_implicit_unit_selection.eidos`

## 声明与调用

```eidos
inc :: Int -> Int
{
    x => x + 1
}

a :: inc(1);
```

`inc :: Int -> Int` 声明签名（`::` 是声明绑定 token，不是表达式运算符），`{ x => x + 1 }` 是函数体。`a :: inc(1);` 是顶层绑定：声明 `a` 并求值 `inc(1)`。

## 柯里化与 binder list

Eidos 函数是柯里化的：多参数函数返回等待剩余参数的函数。规范写法是 **binder list**：

```eidos
add :: Int -> Int -> Int
{
    left, right => left + right
}

sum3 :: Int -> Int -> Int -> Int
{
    a, b, c => a + b + c
}

c :: add(1, 2);        // 分组调用：等价于 add(1)(2)
d :: sum3(1, 2)(3);    // 柯里化应用
e :: 1 `add` 2;        // 反引号中缀调用：等价于 add(1)(2)
```

- 规范柯里化 binder list `p1, p2 => expr` 与等价的右结合链 `p1 => p2 => expr` 语义等价；
- 逗号分隔的调用参数会从左到右逐个应用：`add(1, 2)` 等价于 `add(1)(2)`，`sum3(1, 2)` 仍可返回等待最后一个参数的函数；
- 反引号中缀调用沿用同一规则：``left `add` right`` 等价于 `add(left)(right)`；
- 嵌套调用 `add(1)(2)` 在柯里化签名下可通过 Parser + Types。

## 带模式的 binder 与 guard

binder list 可以使用 `_` 和构造器 pattern，也可用于花括号 lambda。后续参数也能直接带 `when` guard：

```eidos
pick_positive :: Int -> Int -> Int
{
    n, i when i > 0 => i,
    _, _             => 0
}
```

`p1, p2` 后的 guard 可以引用两个绑定，并在两者均匹配后执行；`p1 when guard1 => p2 => expr` 这类分阶段写法会保留箭头链，因为展平会移动 guard 的求值阶段。函数体模式分支与 `match` 共用同一套覆盖分析（`W4200` / `W4201`，见 [模式匹配](07-pattern-matching.md)）。

## 函数体模式分支

函数体可以直接写分支序列，按参数逐一匹配：

```eidos
option_string_map_list :: OptionString -> (String -> String) -> OptionString
{
    SomeString(value), mapper => SomeString(mapper(value)),
    NoneString(), _           => NoneString()
}
```

- 单个 tuple 参数：`(left, right) => expr`；
- 两个柯里化 tuple 参数：`(a1, a2), (b1, b2) => expr`；
- 带外层括号的 tuple 写法始终只绑定一个 tuple 值，与柯里化 binder list 语义不同。

## 泛型函数

```eidos
identity[T] :: T -> T
{
    x => x
}

b :: identity(42);
```

`[T]` 是类型参数列表，`identity[Int]` 实例化后应用于参数。泛型细节（kind、where 从句）见 [泛型](08-generics.md)。

## 链式调用

链式调用在编译阶段自动降级为普通调用：

```text
obj.m(a, b)  =>  m(obj, a, b)
obj.m        =>  m(obj)
obj.m().n()  =>  n(m(obj))
obj.m.n      =>  n(m(obj))
```

```eidos
id :: Int -> Int { x => x }
inc :: Int -> Int { x => x + 1 }
double :: Int -> Int { x => x + x }

chain :: id(1).inc().double();
chain2 :: id(1).inc.double;
chain3 :: 3.inc.double;
```

## 隐式 `Unit` 参数

首个运行时参数规范化为 `Unit` 时，普通 block 隐式等价于唯一的 `_ => block` 分支（见 [语句与表达式](03-statements-and-expressions.md)）。

## 风格约定

新代码优先展示 Eidos 的函数式读法：线性数据流优先写 `value |> f |> g`；函数组合优先写 `f >>> g`；连续容器变换优先写链式调用 `xs.map(f).filter(p).fold_left(seed)(step)`。完整约定见 [开发实践与风格](../practices/01-style-conventions.md)。
