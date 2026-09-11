# White Programming Language

A domain-specific systems programming language designed for RF engineering, digital signal processing (DSP), signal analysis, scientific computing, and communications research.

White is implemented entirely in C11, providing a high-performance compiler that translates White source code to C, which is then compiled to native executables.

## Overview

White combines the expressiveness needed for RF/DSP domains with the performance characteristics of systems programming. It features:

- **RF/DSP first-class types**: `complex`, `iq`, `signal`, `spectrum`, `frequency`, `sample_rate`, `bandwidth`
- **Physics-aware semantics**: SI units, dimensional analysis, physical constants
- **Modern syntax**: Clean grammar designed for scientific and engineering workloads
- **Real compiler architecture**: Complete pipeline from source through AST to C code generation
- **Extensible design**: Modular architecture supports new types, operations, and backends
- **Direct SDR/hardware integration**: Designed to interface with software-defined radios and measurement equipment

## Features

### Core Language
- Variables, constants, and type aliases
- Numeric types: integers, floats, complex numbers, booleans
- Structures, enums, and arrays
- Functions with parameters and return types
- Pattern matching and error handling
- Comments and compiler diagnostics

### RF/DSP Capabilities
- IQ data acquisition and manipulation
- Fast Fourier Transform (FFT/IFFT)
- Power Spectral Density (PSD)
- Spectrograms and waterfalls
- Peak detection and bandwidth estimation
- Signal filtering (FIR/IIR)
- Complex arithmetic and statistics
- Frequency and phase analysis
- Correlation and convolution

### Physics Support
- SI unit system
- Vector and matrix operations
- Complex number algebra
- Physical constants
- Electromagnetic quantities
- Wave calculations
- Dimensional analysis framework

## Building

### Prerequisites
- GCC or Clang with C11 support
- GNU Make
- Standard POSIX utilities

### Compilation

```bash
make
```

This produces `build/white`, the White compiler executable.

To clean build artifacts:

```bash
make clean
```

## Usage

### Compile a White program

```bash
./build/white program.white
```

This generates `program.c` and compiles it to `program` (or `program.exe` on Windows).

### Command-line Options

```bash
white <file.white>           # Compile and link
white build <file.white>     # Compile only
white run <file.white>       # Compile and execute
white check <file.white>     # Type-check only
white emit-c <file.white>    # Emit generated C
white version                # Show version
white help                   # Show help
```

## Language Examples

### Hello World

```white
fn main() {
    print("Hello, World!");
}
```

### IQ Signal Processing

```white
fn main() {
    // Load IQ data from file
    signal s = iq("capture.iq", 2.4 MHz);
    
    // Compute FFT
    spectrum sp = fft(s, 4096);
    
    // Analyze spectrum
    frequency peak = sp.peak();
    snr value = sp.snr();
    
    print(peak);
    print(value);
}
```

### Physics Calculations

```white
fn wavelength(frequency f) -> float {
    const float c = 3e8;  // speed of light in m/s
    return c / f;
}

fn main() {
    frequency f = 2.4 GHz;
    float lambda = wavelength(f);
    print(lambda);
}
```

### DSP with Filtering

```white
fn main() {
    signal s = iq("data.iq", 2.4 MHz);
    
    // Design FIR filter
    array[float] coeffs = fir_design(100, 10 MHz, 2.4 MHz);
    
    // Apply filter
    signal filtered = convolve(s, coeffs);
    
    // Analyze result
    spectrum sp = fft(filtered, 4096);
    print(sp.peak());
}
```

## Project Structure

```
White/
├── README.md              # This file
├── LICENSE                # MIT License
├── Makefile               # Build configuration
├── LANGUAGE.md            # Language specification
├── CHANGELOG.md           # Version history
│
├── include/white/         # Public headers
│   ├── common.h          # Common definitions
│   ├── lexer.h           # Lexical analysis
│   ├── parser.h          # Syntax analysis
│   ├── ast.h             # Abstract syntax tree
│   ├── types.h           # Type system
│   ├── semantic.h        # Semantic analysis
│   ├── diagnostics.h     # Compiler diagnostics
│   ├── codegen.h         # Code generation
│   ├── compiler.h        # Main compiler interface
│   └── runtime.h         # Runtime support
│
├── src/                   # Compiler implementation
│   ├── main.c            # Entry point
│   ├── lexer.c           # Tokenization
│   ├── parser.c          # Parsing
│   ├── ast.c             # AST construction
│   ├── types.c           # Type system
│   ├── semantic.c        # Semantic analysis
│   ├── diagnostics.c     # Diagnostics
│   ├── codegen.c         # C code generation
│   ├── compiler.c        # Compilation driver
│   └── util.c            # Utilities
│
├── runtime/               # Runtime library
│   ├── white_runtime.h   # Runtime interface
│   ├── white_runtime.c   # Runtime implementation
│   ├── signal.c          # Signal operations
│   ├── spectrum.c        # Spectral operations
│   ├── complex.c         # Complex arithmetic
│   └── math.c            # Math operations
│
├── std/                   # Standard library
│   ├── signal.white      # Signal processing
│   ├── spectrum.white    # Spectrum analysis
│   ├── physics.white     # Physics utilities
│   └── math.white        # Mathematical functions
│
├── examples/              # Example programs
│   ├── hello.white       # Hello World
│   ├── iq.white          # IQ data processing
│   ├── fft.white         # FFT example
│   ├── spectrum.white    # Spectrum analysis
│   └── physics.white     # Physics calculations
│
├── tests/                 # Test suite
│   ├── lexer/            # Lexer tests
│   ├── parser/           # Parser tests
│   ├── semantic/         # Semantic tests
│   ├── codegen/          # Code generation tests
│   └── integration/      # Integration tests
│
└── docs/                  # Documentation
    ├── architecture.md    # Compiler architecture
    ├── compiler.md        # Compiler internals
    ├── language.md        # Language specification
    └── roadmap.md         # Development roadmap
```

## Compiler Architecture

White uses a traditional multi-pass compiler pipeline:

```
White Source Code
        ↓
    Lexer (Tokenization)
        ↓
    Tokens
        ↓
    Parser (Syntax Analysis)
        ↓
    Abstract Syntax Tree (AST)
        ↓
    Semantic Analysis (Type Checking)
        ↓
    Intermediate Representation
        ↓
    Code Generation (to C)
        ↓
    Generated C Code
        ↓
    C Compiler (GCC/Clang)
        ↓
    Native Executable
```

The architecture is modular and designed to support:
- Additional compiler passes (optimization, verification)
- Alternative backends (LLVM, WebAssembly, native code)
- Extended type systems and operations
- Custom standard library modules

## Development

### Building with Debug Information

```bash
make DEBUG=1
```

### Running Tests

```bash
make test
```

### Code Style

The project uses:
- C11 standard
- POSIX compliance where practical
- Consistent naming conventions
- Modular design with clear interfaces
- Comprehensive error handling

## License

White is released under the MIT License. See LICENSE file for details.

## Contributing

White is open to contributions. Areas of active development:

- RF/DSP operation implementations (FFT, filtering, modulation analysis)
- Physics simulation framework
- Standard library expansion
- Compiler optimizations
- Backend implementations (LLVM, native)
- Documentation and examples
- Test coverage

## Roadmap

### Phase 1: Core Compiler (Current)
- ✓ Lexer and parser
- ✓ Type system
- ✓ Semantic analysis
- ✓ Basic code generation
- ✓ RF/DSP types
- ✓ Runtime library

### Phase 2: Optimization
- [ ] Constant folding
- [ ] Dead code elimination
- [ ] Inlining
- [ ] SIMD generation

### Phase 3: Extended Runtime
- [ ] Advanced DSP algorithms
- [ ] Hardware interfaces
- [ ] Graphics/visualization
- [ ] Parallel processing

### Phase 4: Alternative Backends
- [ ] LLVM backend
- [ ] WebAssembly support
- [ ] Native code generation
- [ ] JIT compilation

## Resources

- **Language Specification**: See `LANGUAGE.md`
- **Compiler Architecture**: See `docs/architecture.md`
- **Examples**: See `examples/` directory
- **Tests**: See `tests/` directory

## Support

For issues, questions, or contributions, please open an issue on GitHub.

---

**White**: A language for RF engineering, signal processing, and scientific computing.
