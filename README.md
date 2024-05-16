# AledLang

AledLang is a stupid programming language, this repo contains what
is *- I hope -* the only tool to compile and debug it.

This language is so difficult to use that programming an AledLang
interpertor is simpler than making a hello world with it... Aled is
the contraction of "à l'aide" in French which means "help" / "help me".

Originally, it is interpreted but a compiler for x86 in 32bit is now
functional (it generates GNU assembly files and not directly .elf files).

## Syntax

The language was designed to be written on a single line, the number,
keyword and operator must be separated by any non-null quantity of any
whitespace.

### Pushs

The language is stack based, each written number is pushed into the stack,
we can also note charaters which will send their ascii value.

```
[ PUSHING NUMBERS ]

> 1 2 3 'A'

- 1 is pushed
- 2 is pushed
- 3 is pushed
- 65 is pushed (A ascii value)
```

To simplify the push of string, we can note a sequence of charatere between
double quote, which will push all the chars of the string with a \0 at the end.

```
[ PUSHING STRING ]

> "Hello"

- 72 is pushed (H ascii value)
- 101 is pushed (e ascii value)
- 108 is pushed (l ascii value)
- 108 is pushed (l ascii value)
- 111 is pushed (o ascii value)
- 0 is pushed (NULL terminator)
```

### Stack

The stack is a LIFO stack, the last element pushed is the first to be popped.
Numbers pushed are 32bits integers.

```
[ PRINT 42 AND 123 ]

> 123 42 PRINT PRINT

- 123 is pushed
- 42 is pushed
- 42 is popped and printed
- 123 is popped and printed
```

### Labels

A label is a number surrounded by parenthesis, it is used to represent a
position in the code, it is used by the `GOTO` and `JIF` keywords.

```
[ PRINT 123 UNTIL THE END OF TIME ]

> (0) 123 PRINT 0 GOTO

- 123 is pushed
- 123 is popped and printed
- 0 is pushed
- 0 is popped and the program jumps to the label 0
```

### Comments

comments are not recommended because they risk making the code less obscure,
however I am easily influenced and a friend asked me to implement them.

```
[ PRINT 42 WITH TOO MANY COMMENTS ]

> 42 [gosh, I love comments] PRINT // I love them so much

- 42 is pushed
- 42 is popped and printed
```

## Keywords

| Keyword | Internal    | Description               | Stack before, KW, Stack after             |
|---------|-------------|---------------------------|-------------------------------------------|
| `PRINT` | `KW_PRINT`  | print like `%u\n`         | `1 2    ` ` PRINT ` `1`       (2 printed) |
| `CPUT`  | `KW_CPUT`   | print like `%c`           | `1 65   ` ` CPUT  ` `1`       (A printed) |
| `GOTO`  | `KW_GOTO`   | jump to label             | `1 123  ` ` GOTO  ` `1`     (jump to 123) |
| `JIF`   | `KW_JIF`    | jump to label if true     | `1 3 123` ` JIF   ` `1`     (jump to 123) |
| `SET`   | `KW_SET`    | set memory                | `1 2 3  ` ` SET   ` `1`     (mem[3] <- 2) |
| `GET`   | `KW_GET`    | get memory and push       | `1 3    ` ` GET   ` `1 2`   (mem[3] -> 2) |
| `POP`   | `KW_POP`    | pop from stack            | `1 2    ` ` POP   ` `1`        (2 popped) |
| `SWAP`  | `KW_SWAP`   | swap last two elements    | `1 2 3  ` ` SWAP  ` `1 3 2`               |
| `SWAP3` | `KW_SWAP3`  | swap last three elements  | `1 2 3 4` ` SWAP3 ` `1 4 3 2`             |
| `ROT`   | `KW_ROT`    | rotate all elements       | `1 2 3 4` ` ROT   ` `4 1 2 3`             |
| `DUP`   | `KW_DUP`    | duplicate last element    | `1      ` ` DUP   ` `1 1`                 |
| `DUP2`  | `KW_DUP2`   | duplicate last two        | `1 2    ` ` DUP2  ` `1 2 1 2`             |
| `+`     | `OP_ADD`    | add last two elements     | `1 2    ` `  +    ` `3`                   |
| `-`     | `OP_SUB`    | subtract last two elements| `3 2    ` `  -    ` `1`                   |
| `*`     | `OP_MUL`    | multiply last two elements| `2 3    ` `  *    ` `6`                   |
| `/`     | `OP_DIV`    | divide last two elements  | `6 2    ` `  /    ` `3`                   |
| `%`     | `OP_MOD`    | modulo last two elements  | `6 4    ` `  %    ` `2`                   |
| `==`    | `OP_EQ`     | compare last two elements | `1 1    ` `  ==   ` `1`                   |
| `!=`    | `OP_NEQ`    | compare last two elements | `1 2    ` `  !=   ` `1`                   |
| `>`     | `OP_GT`     | compare last two elements | `2 1    ` `  >    ` `1`                   |
| `<`     | `OP_LT`     | compare last two elements | `1 2    ` `  <    ` `1`                   |
| `>=`    | `OP_GTE`    | compare last two elements | `2 1    ` `  >=   ` `1`                   |
| `<=`    | `OP_LTE`    | compare last two elements | `1 2    ` `  <=   ` `1`                   |
