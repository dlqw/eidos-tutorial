# Trait 与 named instance

示例文件：`examples/traits/05_trait_impl_declaration.eidos`、`examples/traits/50_qualified_trait_method_paths.eidos`

## 声明 trait

```eidos
Show :: trait {
    show :: Self -> String
}
```

`Self` 表示"实现该 trait 的类型"。trait 方法声明只有签名，没有函数体。

## named instance

trait evidence 使用 name-first `instance` 声明。函数级 `@impl(Trait)` 已从 0.7 authoring surface 删除，只作为显式迁移输入识别：

```eidos
ShowPoint :: instance Show {
    show :: Point -> String
    {
        p => "Point"
    }
}
```

规则：

1. `instance` 的成员名与签名必须匹配目标 Trait 方法，并且定义在同模块内；
2. 对于泛型 Trait，`instance` 支持显式 trait 类型实参（例如 `FunctorBox :: instance Functor[Box]`），并会在命名阶段检查实参数量是否匹配；
3. 约定式 impl 注册不会推断泛型 Trait 的类型实参；对泛型 Trait 必须使用显式 `Trait[...]`；
4. `expr given InstanceName` 可显式选择某个命名 evidence。

## 重叠与特化

`instance` 会先做 alias canonicalization，再比较 impl 头的结构特化关系：

- 若一个头严格比另一个更具体（例如 `Option[Int]` 相对于 `Option[T]`），两者可以共存；
- 若两个头 canonicalize 后等价，或彼此不可比较，命名阶段会以 `E3004`（`overlapping impl registration`）拒绝；
- 仅靠 alias 改写不会产生"更具体"的关系，所以 trait 实参侧或实现类型侧的 alias-only 等价重叠仍会报错。

## 约束求解行为

1. `Tuple` 类型按结构化规则检查 trait：当且仅当每个元素类型都满足该 trait 时，元组满足该 trait；
2. `TyCon`（构造类型）优先走内置 trait 映射，其次走符号表中的 `impl` 查找；
3. 约束中的 `traitId` 缺失时，会回退按 `traitName` 在符号表中查找 trait 再做 `impl` 匹配；
4. 泛型函数的 trait bound 会在调用点实例化后强制检查（见 [泛型](08-generics.md)）；
5. Trait 实参匹配采用精确语义：无实参 bound（例如 `T: Functor`）不会匹配仅有特化实参的 named instance（如 `FunctorBox :: instance Functor[Box]`）；
6. 泛型 trait bound 中的部分应用 alias 可以从具体底层返回类型反推出开放槽位（详见进阶 [函数式编程](../advanced/05-functional-programming.md)）。

## 构造器事实 bridge

用户自定义 trait 的构造器事实写在 named instance bridge 中。`name: Type` 是运行时字段，会进入构造器参数、layout 和 ABI；`name = expr` 不再允许写在 ADT 构造器里：

```eidos
DirectionVector :: trait {
    dx :: Self -> Int
    dy :: Self -> Int
}

type Direction[A]
{
    North -> Direction[Vertical],
    South -> Direction[Vertical],
    East -> Direction[Horizontal]
}

DirectionVectorDirection[A] :: instance DirectionVector for Direction[A] {
    North => { dx = 0, dy = -1 } |
    South => { dx = 0, dy = 1 } |
    East => { dx = 1, dy = 0 }
}
```

- bridge fact 输入支持字面量、元组、列表、受限一元/二元表达式、值名或路径引用，以及构造器/普通函数调用表达式；生成的 impl 会把这些表达式复制到对应构造器分支中并继续接受正常类型检查；
- bridge 支持 `Self -> R` 和 `Self -> A -> R` 这类额外参数方法；返回类型和额外参数中的 `Self` 会替换为目标类型；
- 与 GADT 组合时，生成的泛型 impl 会在每个构造器分支内使用局部类型精化（例如 `North` 分支中只局部知道 `A = Vertical`，该等式不会泄漏到其他分支或分支外）。

## 限定 trait 方法路径

限定 trait 方法路径是一等可调用值路径。泛型代码里既可以通过导入模块别名直接写 `Applicative.pure`、`Traversable.traverse`，也可以写导入模块下的嵌套 owner 路径（如 `Trait.Eq.eq`），还可以写完整根路径 `std.Applicative.pure`、`std.Traversable.traverse`：

```eidos
lift_alias[A, G: kind2 : Applicative.Applicative[G]] :: A -> G[A]
{
    value => Applicative.pure(value)
}

eq_via_module_trait[T: Traits.Eq] :: T -> T -> Bool
{
    left => right => Traits.Eq.eq(left)(right)
}
```

在当前模块内部也可以写同名 trait 的相对路径，例如 `Demo.Show :: module { ... }` 内部直接写 `Show.show`。
