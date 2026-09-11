# White Compiler Tests

This directory contains the test suite for the White compiler.

## Test Categories

### Lexer Tests
- Number tokenization (integers, floats, scientific notation)
- Keyword recognition
- Operator tokenization
- String literals
- Comment handling
- Whitespace handling

### Parser Tests
- Literal expressions
- Binary operations
- Unary operations
- Function definitions
- Variable declarations
- Control flow (if/while/for)
- Blocks and scope

### Type System Tests
- Primitive types
- RF/DSP types
- Array types
- Pointer types
- Struct types
- Type equality and compatibility

### Semantic Analysis Tests
- Symbol resolution
- Type checking
- Scope management
- Function resolution

### Codegen Tests
- Expression compilation
- Statement compilation
- Function compilation
- C code output correctness

### Runtime Tests
- Complex number arithmetic
- IQ sample handling
- Signal creation and manipulation
- Spectrum analysis
- FFT computation
- Peak detection
- SNR calculation

## Building Tests

```bash
cd tests
gcc -Wall -Wextra -std=c11 -I../include -lm -o test_runner test_main.c ../src/*.c ../runtime/*.c
./test_runner
```

## Adding New Tests

1. Create test function with `TEST_START()` and `TEST_ASSERT()` macros
2. Call function in `main()`
3. Follow naming convention: `test_category_feature()`
4. Keep tests focused and independent
