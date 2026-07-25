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
- **调用 C 函数**：通过 `@[extern(c, ...)]` typed tag 和 `need ffi` 声明外部 C 函数，在 Eidos 代码中直接调用
- **操作 C 内存**：显式 `import std.Ffi`，通过 `RawPtr` / `Ptr[T]` 和 `Ffi` 的安全公开封装操作原始内存
- **C 回调**：将 Eidos 函数转换为 C 函数指针，传给接受回调的 C API
- **间接调用**：通过 `Ffi.cfn_call` 从 Eidos 调用 C 函数指针
- **生成绑定包**：用 `eidosc pkg bind` 从 C header 和声明式规则生成独立
  Eidos package，应用项目只通过普通 `[dependencies]` 引用

### 编译命令

```powershell
# 编译单个文件
eidosc build src/main.eidos

# 带详细输出
eidosc build src/main.eidos -v
```

编译器自动处理链接——`@[extern(c, ...)]` 声明的函数会生成 LLVM `declare`，最终由 `clang` 链接为可执行文件。

---

## 2. 声明 C 函数

### 基本声明

使用结构化 `@[extern(c, ...)]` typed tag 声明外部 C 函数。声明**不能有函数体**，并显式声明 `need ffi`：

```eidos
@[extern(c, name: "puts")]
puts :: Ptr[Char] -> Int need ffi;
```

编译器使用 `name` 字段作为 C 符号名。

### 自定义符号名

如果 Eidos 函数名和 C 符号名不同，可以在 `extern` 中指定：

```eidos
@[extern(c, name: "malloc")]
alloc :: Int -> RawPtr need ffi;

@[extern(c, name: "free")]
dealloc :: RawPtr -> Unit need ffi;
```

### 链接外部库

如果需要链接非默认库，在 `eidos.toml` 中声明允许并参与链接的库：

```toml
[ffi]
libraries = ["curl"]
```

外部声明用相同的 `library` 身份关联该库：

```eidos
@[extern(c, library: "curl", name: "curl_easy_init")]
curl_init :: Unit -> RawPtr need ffi;
```

编译器会在分析阶段检查 `library` 是否已由项目或依赖包声明，并在链接阶段把对应库传给平台链接器。复杂的头文件、shim、库路径和平台差异应封装在生成的绑定包中，而不是写入 Eidos 源文件。

### 多参数函数

Eidos 使用柯里化语法，但 FFI 声明支持多参数：

```eidos
import std.Ffi

@[extern(c, name: "qsort")]
qsort :: RawPtr -> Int -> Int -> Cfn[Int, Int, Int] -> Unit need ffi;
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

编译器在类型推断后运行 FFI 类型校验，确保 ``extern` typed tag` 函数的参数和返回值类型是 **FFI 安全类型**。

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

函数类型参数只适合传给明确理解 Eidos closure ABI 的 native 函数；它不是 C 函数指针。C 回调槽位仍应使用 `Cfn` / `Ffi.cfn_from`。`String`、自定义 ADT、列表等托管类型**不能**直接用于 FFI 参数或返回值。必须先转换：

```eidos
// 错误：String 不是 FFI 安全类型
`extern` typed tag
bad :: String -> Int          // E3051 错误

// 正确：先转换为 RawPtr
import std.Ffi
cstr := Ffi.to_c_string("hello");
puts(cstr);                      // RawPtr 是安全类型
```

### 错误代码

| 代码 | 级别 | 描述 |
|------|------|------|
| E3050 | Error | ``extern` typed tag` 函数带函数体 |
| E3051 | Error | ``extern` typed tag` 参数或返回值类型不安全 |
| W3050 | Warn | `link` 指令但无 ``extern` typed tag` 函数 |

---

## 4. 字符串互转

Eidos 的 `String` 是托管类型（带引用计数），不能直接传给 C。需要通过转换函数：

```eidos
import std.Ffi

// String → char*（C 字符串）
cstr: RawPtr  := Ffi.to_c_string("hello");

// char* → String（Eidos 字符串）
eidos_str: String  := Ffi.from_c_string(cstr);
```

`Ffi.to_c_string` 返回的 `RawPtr` 指向 C 兼容的 null-terminated 字符串，可直接传给 `puts`、`printf` 等 C 函数。

---

## 5. 指针操作

### RawPtr 与 Ptr[T]

- `RawPtr`：不透明裸指针，等价于 C 的 `void*`
- `Ptr[T]`：泛型指针，LLVM 表示相同（都是 `ptr`），但类型签名更清晰

两者都是非托管类型——**不参与引用计数**。

### 空指针

```eidos
null := Ffi.null_pointer();       // 创建空指针
yes := Ffi.is_null(null);         // true
no := Ffi.is_null(Ffi.malloc(8)); // false
```

### 指针算术

```eidos
Ffi.offset_bytes(ptr, byte_offset) -> RawPtr
```

`byte_offset` 以**字节**为单位：

```eidos
// offset_bytes 按 8 字节偏移（一个 i64 的大小）
second := Ffi.offset_bytes(arr, 8);
third := Ffi.offset_bytes(arr, 16);
```

### 内存读写

```eidos
Ffi.store[Int](ptr, value) -> Unit  // 按类型写入
Ffi.load[Int](ptr) -> Int           // 按类型读取
```

> 示例文件：[`examples/56_ffi_pointer_ops.eidos`](examples/56_ffi_pointer_ops.eidos)

`Ffi.load[T]` / `Ffi.store[T]` 使用统一泛型 API；用户代码不直接调用按类型后缀拆分的编译器内部 intrinsic。

---

## 6. 函数指针与回调

### Ffi.cfn_from：Eidos → C 函数指针

```eidos
Ffi.cfn_from(func) -> Cfn[A..., R]
```

将 Eidos 零捕获函数转换为 C 函数指针。带捕获闭包不是 C 函数指针；`Ffi.cfn_from` 会以 `E3053` 拒绝。需要把带捕获函数传给 native runtime 时，使用明确接受 Eidos closure 对象指针的 ``extern` typed tag` 参数，而不是 C 回调槽位。

```eidos
add_one :: Int -> Int { x => x + 1 }

fp: Cfn[Int, Int] := Ffi.cfn_from(add_one);
```

### Ffi.cfn_call：通过 C 函数指针调用

```eidos
Ffi.cfn_call(fn_ptr, args...) -> R
```

类型参数从 `Cfn[A, R]` 推导输入和返回类型；多参数 C 函数使用对应的 `Cfn[A..., R]` 类型。

```eidos
result := Ffi.cfn_call(fp, 41);  // 通过 fp 调用，返回 42
```

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
import std.Ffi

@[extern(c, name: "qsort")]
qsort :: RawPtr -> Int -> Int -> Cfn[Int, Int, Int] -> Unit need ffi;

my_cmp :: Int -> Int -> Int
{
    a => b => a - b
}

main :: Int -> Int need ffi
{
    _ => {
        arr := Ffi.malloc(40);

        Ffi.store[Int](arr, 5);
        Ffi.store[Int](Ffi.offset_bytes(arr, 8), 3);
        Ffi.store[Int](Ffi.offset_bytes(arr, 16), 1);
        Ffi.store[Int](Ffi.offset_bytes(arr, 24), 4);
        Ffi.store[Int](Ffi.offset_bytes(arr, 32), 2);

        qsort(arr, 5, 8, Ffi.cfn_from(my_cmp));

        // 排序后: [1, 2, 3, 4, 5]
        sum := Ffi.load[Int](arr)
                + Ffi.load[Int](Ffi.offset_bytes(arr, 8))
                + Ffi.load[Int](Ffi.offset_bytes(arr, 16))
                + Ffi.load[Int](Ffi.offset_bytes(arr, 24))
                + Ffi.load[Int](Ffi.offset_bytes(arr, 32));

        println(sum);  // 输出: 15
        0
    }
}
```

> 示例文件：[`examples/58_ffi_qsort.eidos`](examples/58_ffi_qsort.eidos)

---

## 8. 标准库公开 API 参考

### 字符串转换

| 函数 | 签名 | 说明 |
|------|------|------|
| `Ffi.to_c_string` | `String -> RawPtr` | Eidos String → C `char*` |
| `Ffi.from_c_string` | `RawPtr -> String` | C `char*` → Eidos String |

### 指针操作

| 函数 | 签名 | 说明 |
|------|------|------|
| `Ffi.null_pointer` | `Unit -> RawPtr` | 创建空指针 |
| `Ffi.is_null` | `RawPtr -> Bool` | 空指针检测 |
| `Ffi.pointer_eq` | `RawPtr -> RawPtr -> Bool` | 指针相等比较 |
| `Ffi.offset_bytes` | `RawPtr -> Int -> RawPtr` | 指针字节偏移 |
| `Ffi.load[T]` | `RawPtr -> T` | 按 `T` 加载值 |
| `Ffi.store[T]` | `RawPtr -> T -> Unit` | 按 `T` 存储值 |

### 标准库内存管理

`std.Ffi` 提供 C 标准内存管理绑定和作用域式辅助函数：

| 函数 | 签名 | 说明 |
|------|------|------|
| `Ffi.malloc` | `Int -> RawPtr` | 调用 C `malloc` |
| `Ffi.free` | `RawPtr -> Unit` | 调用 C `free` |
| `Ffi.calloc` | `Int -> Int -> RawPtr` | 调用 C `calloc` |
| `Ffi.realloc` | `RawPtr -> Int -> RawPtr` | 调用 C `realloc` |
| `Ffi.with_malloc[A]` | `Int -> (RawPtr -> A) -> A` | 分配后执行回调，返回前自动 `free` |
| `Ffi.with_malloc_zeroed[A]` | `Int -> (RawPtr -> A) -> A` | 使用 `calloc` 分配零初始化内存，返回前自动 `free` |

示例：

```eidos
import std.Ffi

value := Ffi.with_malloc(8, ptr => {
    stored := Ffi.store[Int](ptr, 7);
    Ffi.load[Int](ptr)
});
```

`with_malloc` / `with_malloc_zeroed` 只适合指针不逃逸回调的场景；如果把 `ptr` 保存到全局状态、返回给外部或交给异步任务，仍需要自己保证生命周期正确。

### 函数指针

| 函数 | 签名 | 说明 |
|------|------|------|
| `Ffi.cfn_from` | `(A... -> R) -> Cfn[A..., R]` | Eidos 零捕获函数 → 类型化 C 函数指针 |
| `Ffi.cfn_call` | `Cfn[A..., R] -> A... -> R` | 按完整 C 函数签名间接调用 |

---

## 9. LLVM 映射参考

| Eidos 构造 | LLVM IR |
|-----------|---------|
| ``extern` typed tag puts` | `declare i64 @puts(ptr)` |
| `RawPtr` / `Ptr[T]` / `Cfn` | `ptr`（不参与 RC） |
| `Ffi.to_c_string(s)` | `call ptr @eidos_string_to_cstr(ptr %s)` |
| `Ffi.from_c_string(p)` | `call ptr @eidos_string_from_cstr_raw(ptr %p)` |
| `Ffi.cfn_from(f)` | `bitcast ptr @f to ptr` |
| `Ffi.cfn_call(fp, x)` | 按 `Cfn` 签名生成间接 `call` |
| `Ffi.offset_bytes(p, n)` | `getelementptr i8, ptr %p, i64 %n` |
| `Ffi.load[T](p)` | 按 `T` 表示生成类型化 `load` |
| `Ffi.store[T](p, v)` | 按 `T` 表示生成类型化 `store` |

---

## 10. 绑定包生成

复杂 C 库不要把 include path、native source、linker flags 和大量 ``extern` typed tag`
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
import std.Ffi

main :: Int -> Int need ffi {
    _ => {
        Window.init(640, 400, Ffi.to_c_string("Eidos"));
        0
    }
}
```

限定名结构使用点分路径，例如 `Demo.Graphics.Color.red`；`::` 只用于声明绑定，不用于限定名。

## 11. 已知限制

1. **闭包回调 ABI 仍有边界**
   - `Ffi.cfn_from` 只支持零捕获函数；带捕获函数只能作为 Eidos closure 对象指针传给理解该 ABI 的 native API

2. **C 结构体未支持**
   - 无法直接映射 C `struct`，需手动通过 `Ffi.offset_bytes` + `Ffi.load[T]` 操作字段

3. **RawPtr 生命周期仍需人工约束**
   - 标准库已提供 `Ffi.free`、`Ffi.with_malloc` 和 `Ffi.with_malloc_zeroed`
   - `with_malloc` 系列不会让类型系统自动证明指针未逃逸；复杂 ownership 仍应使用项目本地封装或 C shim 明确约束
