# minpy - a mini-language for educational purposes

```MINPY Is Not Python Yet.```

`minpy` is a Pythonic-ish expression evaluation programming language built for teaching language/grammar design concepts.

When built, the resulting program acts as a console calculator and an interpreter: `usage: minpy [<filename>]`

## Grammar
See `minpy.lang` for the syntax specifications.

## Semantics
The semantics are pretty much the same as when evaluating Python 3.x expressions, except for the following:
- Only one statement per line
- The only statements are prints and assignments. 
    - A print statement is just an expression by itself.
    - An assignment statement is an identifier followed by a "=" symbol.
    - Because of this, the recursive descent parser uses a token buffer, which takes at most 2 tokens. Any more would result in an error.
- No compound assignment statements (i.e. no "+=", or "/=", etc.)
- `@` has the semantic as a unary operator. When applied to numeric values (or compatible), it computes the square root. We don't take into account complex numbers, so by default, you'll get `-nan` from something like `@-1`.

## To build
```g++ minpy.cpp minpy_lex.cpp minpy_parser.cpp -o minpy```