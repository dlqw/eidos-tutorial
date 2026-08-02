# FFI 与 C 互操作

示例文件：`examples/ffi/55_ffi_basic.eidos`、`examples/ffi/56_ffi_pointer_ops.eidos`、`examples/ffi/57_ffi_callback.eidos`、`examples/ffi/58_ffi_qsort.eidos`；C shim 源码见 [`ffi_c_interop/`](../ffi_c_interop/)

详细教程见 [`FFI.zh-CN.md`](../FFI.zh-CN.md)。本章为已验证能力概览。

## 外部函数声明

`@[extern(c, ...)]` 声明外部 C 函数（支持自定义符号名），无函数体，需要 `ffi` effect：

```eidos
import std.Ffi

@[extern(c, name: "puts")]
write_c_string :: RawPtr -> Int need ffi;

main :: Int -> Int need ffi
{
    _ => {
        msg := Ffi.to_c_string("Hello from Eidos!");
        write_c_string(msg);
        0
    }
}
```

已验证能力：

1. `@[extern(c, ...)]` 声明外部 C 函数（支持自定义符号名）。样例：`examples/ffi/55_ffi_basic.eidos`；
2. 指针操作：显式 `import std.Ffi`，使用 `Ffi.null_pointer`、`Ffi.is_null`、`Ffi.offset_bytes`、`Ffi.load[T]`、`Ffi.store[T]`。样例：`examples/ffi/56_ffi_pointer_ops.eidos`；
3. 函数指针回调：`Ffi.cfn_from` 将 Eidos 函数转为 C 函数指针，`Ffi.cfn_call` 通过指针间接调用。样例：`examples/ffi/57_ffi_callback.eidos`；
4. 端到端 `qsort` 回调集成：Eidos 比较函数通过 `Ffi.cfn_from` 传给 C `qsort`，排序结果正确。样例：`examples/ffi/58_ffi_qsort.eidos`。

## 安全类型集

FFI 安全类型集合：`Int`、`Int32`、`Float`、`Bool`、`Unit`、`RawPtr`、`Ptr[T]`、`Cfn`；函数类型参数可作为 Eidos closure 对象指针传给理解该 ABI 的 native 函数。

## 错误码

- `E3050`：`@[extern(...)]` 带函数体；
- `E3051`：非安全类型；
- `W3050`：无用 link 指令。

## 实践建议

对 FFI/输入轮询，优先先读取或分类一次事件，再匹配稳定值；不要把多次有副作用的轮询分散到许多 pattern 里（见基础 [模式匹配](../basics/07-pattern-matching.md)）。
