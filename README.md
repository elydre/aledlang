# AledLang

Keywords:

| Keyword | Internal    | Description               | Stack before, KW, Stack after           |
|---------|-------------|---------------------------|-----------------------------------------|
| `PRINT` | `KW_PRINT`  | print like `%d\n`         | `1 2` => `PRINT` => `1`     (2 printed) |
| `CPUT`  | `KW_CPUT`   | print like `%c`           | `1 65` => `CPUT` => `1`     (A printed) |
| `JIF`   | `KW_JIF`    | jump to label if true     | `1 3 123` => `JIF` => `1` (jump to 123) |
| `GOTO`  | `KW_GOTO`   | jump to label             | `1 123` => `GOTO` => `1`  (jump to 123) |
| `SET`   | `KW_SET`    | set memory                | `1 2 3` => `SET` => `1`   (mem[3] <- 2) |
| `GET`   | `KW_GET`    | get memory and push       | `1 3` => `GET` => `1 2`   (mem[3] -> 2) |
| `POP`   | `KW_POP`    | pop from stack            | `1 2` => `POP` => `1`        (2 popped) |
| `ROT`   | `KW_ROT`    | rotate last two elements  | `1 2 3` => `ROT` => `1 3 2`             |
| `DUP`   | `KW_DUP`    | duplicate last element    | `1` => `DUP` => `1 1`                   |
| `DUP2`  | `KW_DUP2`   | duplicate last two        | `1 2` => `DUP2` => `1 2 1 2`            |
| `+`     | `OP_ADD`    | add last two elements     | `1 2` => `+` => `3`                     |
| `-`     | `OP_SUB`    | subtract last two elements| `3 2` => `-` => `1`                     |
| `*`     | `OP_MUL`    | multiply last two elements| `2 3` => `*` => `6`                     |
| `/`     | `OP_DIV`    | divide last two elements  | `6 2` => `/` => `3`                     |
| `%`     | `OP_MOD`    | modulo last two elements  | `6 4` => `%` => `2`                     |
| `==`    | `OP_EQ`     | compare last two elements | `1 1` => `==` => `1`                    |
| `!=`    | `OP_NEQ`    | compare last two elements | `1 2` => `!=` => `1`                    |
| `>`     | `OP_GT`     | compare last two elements | `2 1` => `>` => `1`                     |
| `<`     | `OP_LT`     | compare last two elements | `1 2` => `<` => `1`                     |
| `>=`    | `OP_GTE`    | compare last two elements | `2 1` => `>=` => `1`                    |
| `<=`    | `OP_LTE`    | compare last two elements | `1 2` => `<=` => `1`                    |
