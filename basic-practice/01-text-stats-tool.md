# 基础实战：文本统计工具

示例文件：`examples/practice/01_text_stats/text_stats.eidos`（已纳入 `verify-examples.ps1` 验证）

这个实战项目**边学边做、逐步增强**：每一步只用前面章节已经验证过的能力。

## 第 1 步：读取文本

用 `File.read_text_or(path, fallback)` 读取文件，文件不存在时回退到默认文本：

```eidos
import std.File
import std.Text

main :: Unit -> Int
{
    _ => {
        content := File.read_text_or("missing.txt", "a b c");
        Text.len(content)
    }
}
```

用到的知识：[标准库使用](../practices/02-standard-library.md)（`std.File` / `std.Text`）、[语句与表达式](../basics/03-statements-and-expressions.md)（块与尾表达式）。

## 第 2 步：统计非空白字符

Eidos 没有传统 for 循环语法，但递归是第一等表达方式（编译器从递归调用图推断 effect row，见 [效果系统](../advanced/02-effects.md)）。逐字符扫描：

```eidos
count_non_blank :: String -> Int -> Int
{
    text => index => {
        if index >= Text.len(text) then { 0 }
        else {
            c := text.char_at_or(index, ' ');
            extra := if c == ' ' then { 0 } else { 1 };
            extra + count_non_blank(text, index + 1)
        }
    }
}
```

要点：

- `text.char_at_or(index, ' ')`：按索引取字符，越界回退到 `' '`（链式读法，见 [风格约定](../practices/01-style-conventions.md)）；
- 递归基例：`index >= Text.len(text)` 时返回 `0`；
- 分支表达式 `if ... then { ... } else { ... }`（见 [流程控制](../basics/06-flow-control.md)）。

## 第 3 步：组合输出

把统计结果组合成一个可断言的程序：

```eidos
main :: Unit -> Int
{
    _ => {
        content := File.read_text_or("missing.txt", "a b c");
        total := Text.len(content);
        non_blank := count_non_blank(content, 0);
        if !Text.is_blank(content) && non_blank > 0 then { total + non_blank } else { 0 }
    }
}
```

- `Text.is_blank(content)`：判断是否全空白（[标准库使用](../practices/02-standard-library.md)）；
- `&&` 短路与 `!` 一元否定（[模式匹配](../basics/07-pattern-matching.md) 的守卫语义同样适用）。

## 验证

```powershell
dotnet run --project src/Eidosc/Eidosc.Cli -- analyze examples/practice/01_text_stats/text_stats.eidos --phase hir --deny style
```

## 练习

1. 把"非空白字符数"改为"非空格字符数"（`c == ' '` 改为其他判断），并验证 `\t` 的处理差异；
2. 增加一个 `count_char` 参数，统计指定字符的出现次数；
3. 用 `match` 改写第 3 步的判断，输出 `Some(total)` / `None()` 形式的 `Option[Int]`（提示：[错误处理](../basics/11-error-handling.md)）。
