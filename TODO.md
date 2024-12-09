## TODO



## Problem

lang.c中

1. **缺少 `break` 语句：** 在 `simplify_regexp` 和 `add_one_regexp` 函数的 `switch` 语句中，缺少 `break` 语句。在 C 语言中，`switch` 语句在没有 `break` 的情况下会执行“贯穿”（fall-through）行为，意味着每个 case 分支执行完后会继续执行下一个 case。这可能会导致逻辑错误，因为它没有完全按照每种正则表达式类型的逻辑来处理。

   **解决方法**： 每个 `case` 后面都应该加上 `break;`，以确保每个正则表达式类型只被处理一次。

2. **内存管理：** 代码使用了 `malloc` 来分配内存，但没有对应的 `free` 操作。这可能导致内存泄漏。在实际应用中，应该确保在不再需要这些结构时，及时释放内存。

3. **字符集的处理：** 在 `T_FR_STRING` 和 `T_FR_SINGLE_CHAR` 类型的处理中，`strncpy` 和 `malloc` 可能需要进行额外的错误检查，以确保内存操作的安全。