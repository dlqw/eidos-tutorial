# FFI 教程：Eidos 与 C 的互操作

> 本教程描述当前已实现并验证的 FFI 能力。所有示例位于 [`examples/`](examples/) 目录。
> 验证基线：2026-05-01，Windows 11，`eidosc build`。
> 支持范围总表见 [`docs/reference/ffi-support-matrix.md`](../reference/ffi-support-matrix.md)。

---

## 目录

1. [概述](#1-概述)
2. [声明 C 函数](#2-声明-c-函数)
3. [FFI 类型安全](#3-ffi-类型安全)
4. [字符串互转](#4-字符串互转)
5. [指针操作](#5-指针操作)
6. [函数指针与回调](#6-函数指针与回调)
7. [完整示例：qsort](#7-完整示例qsort)
8. [内置函数参考](#8-内置函数参考)
9. [LLVM 映射参考](#9-llvm-映射参考)
10. [绑定包生成](#10-绑定包生成)
11. [已知限制](#11-已知限制)

---

## 1. 概述

Eidos FFI 允许：
- **调用 C 函数**：通过 `@ffi` 声明外部 C 函数，在 Eidos 代码中直接调用
- **操作 C 内存**：通过 `RawPtr` / `Ptr[T]` 类型和指针工具函数操作原始内存
- **C 回调**：将 Eidos 函数转换为 C 函数指针，传给接受回调的 C API
- **间接调用**：通过 `cfn_call` 从 Eidos 调用 C 函数指针
- **生成绑定包**：用 `eidosc pkg bind` 从 C header 和声明式规则生成独立
  Eidos package，应用项目只通过普通 `[dependencies]` 引用

### 编译命令

```powershell
# 编译单个文件
eidosc build src/main.eidos

# 带详细输出
eidosc build src/main.eidos -v
```

编译器自动处理链接——`@ffi` 声明的函数会生成 LLVM `declare`，最终由 `clang` 链接为可执行文件。

---

## 2. 声明 C 函数

### 基本声明

使用 `@ffi` 属性声明外部 C 函数。声明**不能有函数体**：

```eidos
@ffi
puts :: Ptr[Char] -> Int
```

编译器使用函数名 `"puts"` 作为 C 符号名。

### 自定义符号名

如果 Eidos 函数名和 C 符号名不同，可以在 `@ffi` 中指定：

```eidos
@ffi("malloc")
alloc :: Int -> RawPtr

@ffi("free")
dealloc :: RawPtr -> Unit
```

### 链接外部库

如果需要链接非默认库，使用 `link` 指令：

```eidos
link "curl"

@ffi("curl_easy_init")
curl_init :: () -> RawPtr
```

`link "curl"` 会传递 `-lcurl` 给链接器。

### 多参数函数

Eidos 使用柯里化语法，但 FFI 声明支持多参数：

```eidos
@ffi("qsort")
qsort :: RawPtr -> Int -> Int -> RawPtr -> Unit
```

调用时逐参数应用或使用元组语法：

```eidos
qsort(arr)(5)(8)(cmp_fn)
// 或
qsort(arr, 5, 8, cmp_fn)
```

> 示例文件：[`examples/55_ffi_basic.eidos`](examples/55_ffi_basic.eidos)

---

## 3. FFI 类型安全

编译器在类型推断后运行 FFI 类型校验，确保 `@ffi` 函数的参数和返回值类型是 **FFI 安全类型**。

### FFI 安全类型

| Eidos 类型 | C 等价 | LLVM 表示 |
|-----------|--------|----------|
| `Int` / `Int64` | `int64_t` | `i64` |
| `Int32` | `int32_t` | `i32` |
| `Float` / `Float64` | `double` | `f64` |
| `Bool` | `_Bool` | `i1` |
| `Unit` | `void` | `void` |
| `RawPtr` / `Ptr[T]` | `void*` / `T*` | `ptr` |
| `Cfn` | 函数指针 | `ptr` |
| 函数类型参数 | Eidos closure 对象指针 | `ptr` |

### 不安全类型

函数类型参数只适合传给明确理解 Eidos closure ABI 的 native 函数；它不是 C 函数指针。C 回调槽位仍应使用 `Cfn` / `cfn_from`。`String`、自定义 ADT、列表等托管类型**不能**直接用于 FFI 参数或返回值。必须先转换：

```eidos
// 错误：String 不是 FFI 安全类型
@ffi
bad :: String -> Int          // E3051 错误

// 正确：先转换为 RawPtr
cstr := string_to_cstr("hello");
puts(cstr);                      // RawPtr 是安全类型
```

### 错误代码

| 代码 | 级别 | 描述 |
|------|------|------|
| E3050 | Error | `@ffi` 函数带函数体 |
| E3051 | Error | `@ffi` 参数或返回值类型不安全 |
| W3050 | Warn | `link` 指令但无 `@ffi` 函数 |

---

## 4. 字符串互转

Eidos 的 `String` 是托管类型（带引用计数），不能直接传给 C。需要通过转换函数：

```eidos
// String → char*（C 字符串）
cstr: RawPtr  := string_to_cstr("hello");

// char* → String（Eidos 字符串）
eidos_str: String  := cstr_to_string(cstr);
```

`string_to_cstr` 返回的 `RawPtr` 指向 C 兼容的 null-terminated 字符串，可直接传给 `puts`、`printf` 等 C 函数。

---

## 5. 指针操作

### RawPtr 与 Ptr[T]

- `RawPtr`：不透明裸指针，等价于 C 的 `void*`
- `Ptr[T]`：泛型指针，LLVM 表示相同（都是 `ptr`），但类型签名更清晰

两者都是非托管类型——**不参与引用计数**。

### 空指针

```eidos
null := ptr_null();              // 创建空指针
yes := ptr_is_null(null);        // true
no := ptr_is_null(malloc(8));   // false
```

### 指针算术

```eidos
ptr_add(ptr, byte_offset) -> RawPtr
```

`byte_offset` 以**字节**为单位：

```eidos
// ptr_add 按 8 字节偏移（一个 i64 的大小）
second := ptr_add(arr, 8);
third := ptr_add(arr, 16);
```

### 内存读写

```eidos
ptr_store_int(ptr, value) -> Unit   // 向 ptr 写入 i64
ptr_load_int(ptr) -> Int            // 从 ptr 读取 i64
```

> 示例文件：[`examples/56_ffi_pointer_ops.eidos`](examples/56_ffi_pointer_ops.eidos)

> **限制**：`ptr_load_int` / `ptr_store_int` 仅支持 i64。对于其他类型，使用下方的类型化变体。

---

## 6. 函数指针与回调

### cfn_from：Eidos → C 函数指针

```eidos
cfn_from(func) -> RawPtr
```

将 Eidos 零捕获函数转换为 C 函数指针。带捕获闭包不是 C 函数指针；`cfn_from` 会以 `E3053` 拒绝。需要把带捕获函数传给 native runtime 时，使用明确接受 Eidos closure 对象指针的 `@ffi` 参数，而不是 C 回调槽位。

```eidos
add_one :: Int -> Int { x => x + 1 }

fp: RawPtr  := cfn_from(add_one);
```

### cfn_call：通过 C 函数指针调用

```eidos
cfn_call(fn_ptr, arg1, arg2, ...) -> Int
```

接受可变参数——第一个参数是函数指针，后续参数直接传递给目标函数。

```eidos
result := cfn_call(fp, 41);  // 通过 fp 调用，返回 42
```

> 返回类型固定为 `Int`（i64）。对于返回指针的 C 函数，需要将结果 `inttoptr`（尚未提供内置函数）。

### 多参数回调

对于接受多个参数的 C 回调（如 `qsort` 的比较函数），使用嵌套 lambda 语法：

```eidos
// C 签名: int cmp(const void* a, const void* b)
// Eidos 签名: Int -> Int -> Int
my_cmp :: Int -> Int -> Int
{
    a => b => a - b
}
```

也可以使用元组参数语法：

```eidos
my_cmp :: Int -> Int -> Int
{
    (a, b) => a - b
}
```

> 示例文件：[`examples/57_ffi_callback.eidos`](examples/57_ffi_callback.eidos)

---

## 7. 完整示例：qsort

端到端演示：分配内存 → 填充数组 → 转换比较函数 → 调用 `qsort` → 读取排序结果。

```eidos
@ffi("qsort")
qsort :: RawPtr -> Int -> Int -> RawPtr -> Unit

@ffi("malloc")
malloc :: Int -> RawPtr

my_cmp :: Int -> Int -> Int
{
    a => b => a - b
}

main :: Int -> Int need FFI
{
    _ => {
        arr := malloc(40);

        ptr_store_int(arr, 5);
        ptr_store_int(ptr_add(arr, 8), 3);
        ptr_store_int(ptr_add(arr, 16), 1);
        ptr_store_int(ptr_add(arr, 24), 4);
        ptr_store_int(ptr_add(arr, 32), 2);

        qsort(arr, 5, 8, cfn_from(my_cmp));

        // 排序后: [1, 2, 3, 4, 5]
        sum := ptr_load_int(arr)
                + ptr_load_int(ptr_add(arr, 8))
                + ptr_load_int(ptr_add(arr, 16))
                + ptr_load_int(ptr_add(arr, 24))
                + ptr_load_int(ptr_add(arr, 32));

        print_int(sum);  // 输出: 15
        print_newline();
        0
    }
}
```

> 示例文件：[`examples/58_ffi_qsort.eidos`](examples/58_ffi_qsort.eidos)

---

## 8. 内置函数参考

### 字符串转换

| 函数 | 签名 | 说明 |
|------|------|------|
| `string_to_cstr` | `String -> RawPtr` | Eidos String → C `char*` |
| `cstr_to_string` | `RawPtr -> String` | C `char*` → Eidos String |

### 指针操作

| 函数 | 签名 | 说明 |
|------|------|------|
| `ptr_null` | `() -> RawPtr` | 创建空指针 |
| `ptr_is_null` | `RawPtr -> Bool` | 空指针检测 |
| `ptr_add` | `(RawPtr, Int) -> RawPtr` | 指针字节偏移 |
| `ptr_load_int` | `RawPtr -> Int` | 加载 `i64` |
| `ptr_store_int` | `(RawPtr, Int) -> Unit` | 存储 `i64` |
| `ptr_load_float` | `RawPtr -> Float` | 加载 `f64` |
| `ptr_store_float` | `(RawPtr, Float) -> Unit` | 存储 `f64` |
| `ptr_load_ptr` | `RawPtr -> RawPtr` | 加载 `ptr` |
| `ptr_store_ptr` | `(RawPtr, RawPtr) -> Unit` | 存储 `ptr` |
| `ptr_load_i32` | `RawPtr -> Int` | 加载 `i32`（自动扩展为 Int） |
| `ptr_store_i32` | `(RawPtr, Int) -> Unit` | 存储 `i32`（自动截断） |
| `ptr_load_i8` | `RawPtr -> Int` | 加载 `i8`（自动扩展为 Int） |
| `ptr_store_i8` | `(RawPtr, Int) -> Unit` | 存储 `i8`（自动截断） |
| `ptr_load_bool` | `RawPtr -> Bool` | 加载 `i1` |

### 标准库内存管理

`Std.FFI` 提供 C 标准内存管理绑定和两个作用域式辅助函数：

| 函数 | 签名 | 说明 |
|------|------|------|
| `FFI.malloc` | `Int -> RawPtr` | 调用 C `malloc` |
| `FFI.free` | `RawPtr -> Unit` | 调用 C `free` |
| `FFI.calloc` | `Int -> Int -> RawPtr` | 调用 C `calloc` |
| `FFI.realloc` | `RawPtr -> Int -> RawPtr` | 调用 C `realloc` |
| `FFI.with_malloc[A]` | `Int -> (RawPtr -> A) -> A` | 分配后执行回调，返回前自动 `free` |
| `FFI.with_malloc_zeroed[A]` | `Int -> (RawPtr -> A) -> A` | 使用 `calloc` 分配零初始化内存，返回前自动 `free` |

示例：

```eidos
import Std.FFI

value := FFI.with_malloc[Int](8)({ ptr => {
    stored := ptr_store_as[Int](ptr, 7);
    ptr_load_as[Int](ptr)
}});
```

`with_malloc` / `with_malloc_zeroed` 只适合指针不逃逸回调的场景；如果把 `ptr` 保存到全局状态、返回给外部或交给异步任务，仍需要自己保证生命周期正确。

### 函数指针

| 函数 | 签名 | 说明 |
|------|------|------|
| `cfn_from` | `func -> RawPtr` | Eidos 函数 → C 函数指针 |
| `cfn_call` | `(RawPtr, ...) -> Int` | 通过 C 函数指针间接调用 |

---

## 9. LLVM 映射参考

| Eidos 构造 | LLVM IR |
|-----------|---------|
| `@ffi puts` | `declare i64 @puts(ptr)` |
| `RawPtr` / `Ptr[T]` / `Cfn` | `ptr`（不参与 RC） |
| `string_to_cstr(s)` | `call ptr @eidos_string_to_cstr(ptr %s)` |
| `cfn_from(f)` | `bitcast ptr @f to ptr` |
| `cfn_call(fp, x)` | `call i64 %fp(i64 %x)` |
| `ptr_add(p, n)` | `getelementptr i8, ptr %p, i64 %n` |
| `ptr_load_int(p)` | `load i64, ptr %p` |
| `ptr_store_int(p, v)` | `store i64 %v, ptr %p` |
| `ptr_load_float(p)` | `load double, ptr %p` |
| `ptr_store_float(p, v)` | `store double %v, ptr %p` |
| `ptr_load_ptr(p)` | `load ptr, ptr %p` |
| `ptr_store_ptr(p, v)` | `store ptr %v, ptr %p` |
| `ptr_load_i32(p)` | `load i32, ptr %p` + `zext i32→i64` |
| `ptr_store_i32(p, v)` | `trunc i64→i32` + `store i32` |
| `ptr_load_i8(p)` | `load i8, ptr %p` + `zext i8→i64` |
| `ptr_store_i8(p, v)` | `trunc i64→i8` + `store i8` |

---

## 10. 绑定包生成

复杂 C 库不要把 include path、native source、linker flags 和大量 `@ffi`
声明直接写进应用项目。推荐先生成独立绑定包：

```powershell
dotnet run --project Eidosc/src/Eidosc.Cli -- pkg bind init projects/bindings/demo `
  --package dev.eidos.demo `
  --library demo `
  --header native/demo.h `
  --include native `
  --native-source native/demo.c

dotnet run --project Eidosc/src/Eidosc.Cli -- pkg bind generate projects/bindings/demo
```

应用侧只写普通依赖：

```toml
[dependencies]
Demo = { path = "../bindings/demo", target = "lib" }
```

代码里通过包别名导入生成模块：

```eidos
import Demo.Window

main :: Int -> Int need FFI {
    _ => {
        Window.init(640, 400, "Eidos".string_to_cstr());
        0
    }
}
```

限定名结构是 `包名::模块名::符号名`；0.4.0-alpha.1 文档中模块名内部使用 dot-separated segments，例如
`Demo.Graphics.Color.red`；旧 slash-separated 写法只作为显式 migration 输入，不属于 Eidos 0.6 编译语法。

## 11. 已知限制

1. **`cfn_call` 返回类型固定为 `Int`**
   - 未实现从 `Cfn[..., Ret]` 推导返回类型
   - 返回指针时需要额外处理

2. **闭包回调 ABI 仍有边界**
   - `cfn_from` 只支持零捕获函数；带捕获函数只能作为 Eidos closure 对象指针传给理解该 ABI 的 native API

4. **C 结构体未支持**
   - 无法直接映射 C `struct`，需手动通过 `ptr_add` + `ptr_load_int` 操作字段

5. **RawPtr 生命周期仍需人工约束**
   - 标准库已提供 `Std.FFI.free`、`with_malloc` 和 `with_malloc_zeroed`
   - `with_malloc` 系列不会让类型系统自动证明指针未逃逸；复杂 ownership 仍应使用项目本地封装或 C shim 明确约束
