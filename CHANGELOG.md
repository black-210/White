# White Language CHANGELOG

## [0.1.0] - 2026-09-11

### Added
- Initial compiler implementation in C11
- Complete lexer with token scanning
- Recursive descent parser
- Abstract Syntax Tree (AST) implementation
- Type system with RF/DSP domain-specific types
- Semantic analysis and type checking
- C code generation backend
- Runtime library for signal processing operations
- FFT implementation (simple DFT-based)
- Spectrum analysis functions
- Complex number arithmetic
- IQ sample support
- Command-line interface (white compiler)
- Comprehensive error diagnostics
- Memory management utilities
- String manipulation utilities

### Features
- Variables, constants, and type declarations
- Functions with parameters and return types
- Control flow: if/else, while, for loops
- Operators: arithmetic, comparison, logical, bitwise
- Comments (// style)
- Print statements for output
- Break and continue statements
- Array and pointer types
- Structure types
- RF/DSP types: frequency, sample_rate, bandwidth, amplitude, phase, power, iq, signal, spectrum
- Complex number type with proper arithmetic

### Compiler Modes
- `white file.white` - Compile and run
- `white build file.white` - Compile to executable
- `white check file.white` - Check syntax only
- `white emit-c file.white` - Generate C source code
- `white version` - Print version
- `white help` - Show help

### Known Limitations
- FFT is simple DFT implementation (not optimized Cooley-Tukey)
- Limited standard library
- Basic type inference only
- No generics or template system
- No module system yet
- Limited optimization passes

### Future Roadmap
- Version 0.2: Optimized FFT, improved error messages, module system
- Version 0.3: LLVM backend, native code generation
- Version 0.4: WebAssembly support, performance profiling
- Version 0.5+: Extended physics library, hardware interfaces, SDR bindings
