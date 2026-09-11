# White Language Specification

## Syntax Overview

### Comments
```white
// Single-line comment
```

### Variables
```white
let x = 10;
let y: i32 = 20;
let z: f64 = 3.14;
```

### Constants
```white
const PI = 3.14159;
const SPEED_OF_LIGHT = 3e8;
```

### Types

#### Primitive Types
- `bool` - Boolean
- `i8`, `i16`, `i32`, `i64` - Signed integers
- `u8`, `u16`, `u32`, `u64` - Unsigned integers
- `f32`, `f64` - Floating point
- `string` - String

#### RF/DSP Types
- `frequency` - Frequency (Hz, kHz, MHz, GHz)
- `sample_rate` - Sampling rate
- `bandwidth` - Signal bandwidth
- `amplitude` - Signal amplitude
- `phase` - Phase angle
- `power` - Power (W, dBm)
- `complex` - Complex number
- `iq` - IQ sample
- `signal` - Digital signal
- `spectrum` - Frequency spectrum

#### Derived Types
- `[T; N]` - Array of fixed size
- `[T]` - Dynamic array
- `*T` - Pointer to T
- `struct Name { ... }` - Structure
- `enum Name { ... }` - Enumeration

### Functions
```white
fn add(x: i32, y: i32) -> i32 {
    return x + y;
}

fn process(signal s) -> spectrum {
    return fft(s, 4096);
}
```

### Control Flow

#### If/Else
```white
if (x > 10) {
    // ...
} else if (x > 5) {
    // ...
} else {
    // ...
}
```

#### While Loop
```white
while (condition) {
    // ...
}
```

#### For Loop
```white
for (let i = 0; i < 10; i = i + 1) {
    // ...
}
```

### Operators

#### Arithmetic
- `+` - Addition
- `-` - Subtraction
- `*` - Multiplication
- `/` - Division
- `%` - Modulo

#### Comparison
- `==` - Equal
- `!=` - Not equal
- `<` - Less than
- `<=` - Less than or equal
- `>` - Greater than
- `>=` - Greater than or equal

#### Logical
- `&&` - AND
- `||` - OR
- `!` - NOT

#### Bitwise
- `&` - AND
- `|` - OR
- `^` - XOR
- `<<` - Left shift
- `>>` - Right shift

### Print Statement
```white
print("Hello", 42, 3.14);
```

### RF/DSP Example
```white
fn analyze() {
    frequency f = 2.4 GHz;
    sample_rate sr = 10 MHz;
    bandwidth bw = 20 MHz;
    
    signal s = iq("capture.iq", sr);
    spectrum sp = fft(s, 4096);
    
    print("Peak: ", sp.peak());
    print("Bandwidth: ", sp.bandwidth());
    print("SNR: ", sp.snr());
}
```

## Compiler

### Compilation Pipeline
1. Lexical Analysis (Tokenization)
2. Parsing (AST Generation)
3. Semantic Analysis (Type Checking)
4. Code Generation (C Emission)
5. C Compilation (GCC/Clang)
6. Linking

### Error Reporting
Diagnostics include:
- Filename, line number, column
- Error/warning/note severity
- Clear error messages
- Context lines where applicable

## Standard Library

### Signal Operations
- `fft(signal s, u32 size) -> spectrum`
- `ifft(spectrum sp) -> signal`
- `psd(signal s) -> spectrum`
- `spectrogram(signal s, u32 window_size) -> spectrum[]`

### Analysis
- `peak_frequency(spectrum sp) -> frequency`
- `bandwidth(spectrum sp) -> bandwidth`
- `snr(spectrum sp) -> f64`
- `power(spectrum sp) -> f64`
- `noise_floor(spectrum sp) -> f64`

### Math
- `abs(x) -> f64`
- `sqrt(x) -> f64`
- `sin(x) -> f64`
- `cos(x) -> f64`
- `tan(x) -> f64`
- `log(x) -> f64`
- `exp(x) -> f64`
- `pow(x, y) -> f64`

### Complex Arithmetic
- `magnitude(complex c) -> f64`
- `phase(complex c) -> f64`
- `conj(complex c) -> complex`
- `real(complex c) -> f64`
- `imag(complex c) -> f64`

### Physics
- `wavelength(frequency f) -> f64`
- `frequency_from_wavelength(f64 w) -> frequency`

## Grammar (Simplified EBNF)

```
program = statement*

statement = declaration
          | expression_stmt
          | block
          | if_stmt
          | while_stmt
          | for_stmt
          | return_stmt
          | print_stmt

declaration = ("let" | "const") IDENT (":" type)? "=" expression ";"
            | "fn" IDENT "(" parameters? ")" ("->" type)? block

expression = assignment
assignment = logical_or (("=" | "+=" | "-=" | "*=" | "/=") assignment)?
logical_or = logical_and ("||" logical_and)*
logical_and = equality ("&&" equality)*
equality = comparison (("==" | "!=") comparison)*
comparison = shift (("<" | "<=" | ">" | ">=") shift)*
shift = additive (("<<" | ">>") additive)*
additive = multiplicative ((("+" | "-") multiplicative)*
multiplicative = unary (("*" | "/" | "%") unary)*
unary = ("!" | "-" | "+" | "&" | "*") unary
       | postfix
postfix = primary (call | index | member)*
primary = NUMBER
        | STRING
        | IDENT
        | "(" expression ")"
        | "true" | "false"

type = IDENT
     | "[" type ";" NUMBER "]"
     | "[" type "]"
     | "*" type
```
