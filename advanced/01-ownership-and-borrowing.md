# 所有权与借用

示例文件：`examples/ownership/38_borrow_effect_decoupling.eidos`、`examples/ownership/39_unary_deref.eidos`、`examples/ownership/40_unary_ref.eidos`、`examples/ownership/41_adt_ref_fields.eidos`、`examples/ownership/54_return_borrow_param_alias.eidos`

Eidos 的借用模型由编译器推导与约束：你写出意图（`ref` / `mref` / `*`），所有权契约（`OwnershipContract`）由 typed callable signature 生成并版本化。

## 引用表面

```eidos
read :: MRef[Int] -> Int
{
    r => *r
}

id :: Int -> Ref[Int]
{
    x => ref x
}
```

1. `ref expr` 在 Types 阶段返回 `Ref[T]`；
2. `mref expr` 返回 `MRef[T]`；
3. `*expr` 要求操作数是 `Ref[T]` 或 `MRef[T]`，结果类型为内部的 `T`；
4. `MRef[T]` 是正式可写引用名；旧 `MutRef[T]` 当前仍作为兼容别名接受；
5. `&expr` 当前仍兼容，但已降为过渡写法，不是长期表面模型；
6. `ref expr` / `mref expr` / `&expr` 都只允许作用在"稳定位置"上：例如局部变量、字段、索引、`*r`；对临时值取引用（如 `ref (x + 1)`、`mref make()`）会直接在 Types 阶段报错；
7. 只读读取路径支持小范围自动穿透：`Ref[Seq[Int]]` / `MRef[Seq[Int]]` 可直接写 `xs[0]`，不必先写 `(*xs)[0]`；
8. 只读参数位置接受 `MRef[T] -> Ref[T]` 的自动退化：`read: Ref[Int] -> Int` 可以直接写 `read(mref x)`；
9. 裸点访问 `a.b` 优先在 record 风格 ADT 上按字段读取解释；`a.b()` 仍是方法调用。因此 `Ref[Range]` / `MRef[Range]` 可直接写 `r.start`；
10. record 风格 ADT 的字段类型可以直接写 `Ref[T]` / `MRef[T]`，并支持"字段后继续点只读方法"：

```eidos
Box[T] :: type {
    reader:: Ref[T], writer:: MRef[T], tag:: Int
}

read :: Ref[Int] -> Int
{
    r => r
}

inspect :: Ref[Box[Int]] -> Int
{
    box => box.reader.read + box.writer.read
}
```

11. 返回 `Ref[T]` / `MRef[T]` 时要求它必须可追溯到输入参数；直接返回参数、返回参数 reference 的局部别名都允许，但像 `ref local`、先从局部/临时值借一层再返回、或先 alias 参数后又被局部来源覆盖再返回，这类写法会在 borrow 阶段报 `E1004`：

```eidos
keep_alias :: Ref[Int] -> Ref[Int]
{
    r => {
        alias := r;
        alias
    }
}
```

12. 更完整的值类别语义仍在后续收口中，当前还不是最终版内存模型。

## OwnershipContract（0.7 基线）

Eidos 0.7 由 typed callable signature 生成版本化 `OwnershipContract`：

1. 普通 `T` 是 `ByValue`，`Ref[T]` 是 `SharedBorrow`，`MRef[T]` 是 `MutableBorrow`；
2. 函数体不能把公开参数模式改成 borrow，也不能用 compiler tag 修补签名；
3. `Copy` 不是第四种参数模式。传入 `ByValue` 参数时，compiler 根据结构化 `Copy` verifier 与当前 place 状态选择 Copy 或 Move；`Ref[T]` 固定为 Copy，`MRef[T]` 固定为 non-Copy；
4. `Clone` 是普通显式 trait 调用，receiver 为 `Ref[Self]`；compiler 不会在调用、赋值或迁移过程中自动插入 `clone`，也不会把 `Clone` 当作 `Copy` fallback；
5. non-Copy aggregate 支持字段/索引级 partial move 与显式重新初始化；控制流合流会保守合并已移动路径。对已 move/drop 的值再次 drop 报 `E1001`，每条路径只释放一次；
6. reborrow 保留 place 与来源：`MRef[T] -> Ref[T]` 只产生共享 reborrow，不复制 pointee；返回 reference 必须保持可证明的输入 provenance 与 lifetime。

## 借用签名推断（CFG 合流）

`LoanSignatureInferer` 使用 CFG/dataflow 程序点推断，不再仅依赖线性启发式别名：

1. 返回借用约束会聚合所有 `return` 站点，并在分支合流后对参数来源取并集；
2. "不同分支返回绑定到不同参数"的函数会得到更准确的 `BoundToParams`；
3. `LoanConstraintVerifier` 的冲突诊断携带 `alias trace id`，`debug` 状态输出中的 trace 带同一 ID；`BorrowChecker` 对齐同一机制，借用错误输出可直接互相定位；
4. 借用目标已升级为路径敏感模型（`BorrowTarget = BaseLocal + PathKey`）：借 `x._0` 后写 `x._1` 不再误报冲突；
5. 对不确定索引（如 `x[i]`）采用保守冲突策略；对常量索引（如 `x[0]` / `x[1]`）保持精确区分；
6. `base` 与 `base/deref` 按不同内存域处理，减少"指针变量本身写入"与"pointee 借用"之间的误报冲突。

## 能力门禁与 effect 解耦

1. borrow 阶段有 capability gate：共享 `MirLoad` 需要 `read`；可变借用 `MirLoad` 与 `MirStore` 需要 `write`；`MirMove` 需要 `move`；
2. 能力不足诊断与借用冲突诊断分离：`E1011`（read 能力不足）、`E1012`（write 能力不足）、`E1013`（move 能力不足）；
3. Borrow 授权与 effect 行独立；effect 声明即使带有 `@borrow(...)` 也不会授予 `read/write/move`；
4. Borrow 权限由 borrow/ownership 流水线推导和约束，而不是由 `need` 提供；
5. body analysis 只生成独立 loan summary（provenance、lifetime/outlives、reborrow、escape 与验证事实），不会改写 `OwnershipContract`；两者分别版本化并参与 HIR/MIR、跨模块 cache 与增量失效；
6. Meta reflection、IDE semantic snapshot 与 LSP hover 只能读取结构化 ownership slots，不能授权或覆盖所有权语义；
7. 旧 `@borrow(read/write/move)` 只作为迁移输入看待；它无法唯一决定 `Ref`/`MRef`。0.7 migrator 遇到这类歧义会在写文件前原子停止，要求同时修改定义与 typed call sites，并且绝不自动插入 `clone`。

## 绑定 mode 回顾

局部引用绑定使用 `ref x := ...` / `mref x := ...`，模式内用 `p as ref x` / `p as mref x`（见基础 [变量绑定与解构](../basics/01-variables-and-bindings.md)）。
