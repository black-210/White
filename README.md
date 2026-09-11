# White: RF Engineering & Scientific Computing Language

**White** is a purpose-built systems programming language implemented entirely in C11 for RF engineering, digital signal processing, physics simulation, and scientific computing.

## Features

✓ **Complete C11 Implementation** - Full compiler, runtime, and standard library
✓ **RF/DSP Domain Types** - `frequency`, `bandwidth`, `signal`, `spectrum`, `iq`, `complex`
✓ **Signal Processing** - FFT, spectrum analysis, peak detection, SNR, bandwidth estimation
✓ **Physics Computing** - SI units, wavelength calculations, path loss, VSWR
✓ **Polymorphic Code Generation** - Binary diversity for defensive security (legitimate uses only)
✓ **Real Compiler Pipeline** - Lexer → Parser → AST → Semantic Analysis → C Codegen
✓ **Command-line Interface** - Multiple compile modes (build, run, check, emit-c)
✓ **Comprehensive Error Diagnostics** - Line/column reporting with context
✓ **Standard Library** - Math, signals, spectrum, physics modules
✓ **Production Quality** - No placeholders, real implementations, proper memory management

## Quick Start

### Build the Compiler

```bash
git clone https://github.com/black-210/White.git
cd White
make clean && make
```

Produces: `build/white` (the White compiler)

### First Program

```white
print("Hello from White!");
```

```bash
./build/white hello.white
```

### RF/DSP Example

```white
fn analyze_signal(string filename) {
    frequency center_freq = 2400000000;  // 2.4 GHz
    sample_rate sr = 10000000;           // 10 MHz
    
    signal s = iq(filename, sr);
    spectrum sp = fft(s, 4096);
    
    print("Peak Frequency: ", sp.peak());
    print("Bandwidth: ", sp.bandwidth());
    print("SNR: ", sp.snr(), " dB");
}

fn main() {
    analyze_signal("capture.iq");
}
```

## Language Highlights

### Variables & Types
```white
let x = 42;                      // Inferred type
let y: f64 = 3.14;              // Explicit type
const PI = 3.14159265;          // Constants

// RF/DSP Types
frequency f = 2.4 GHz;          // Frequency
sample_rate sr = 10 MHz;        // Sampling rate
bandwidth bw = 20 MHz;          // Bandwidth
complex c = 3.0 + 4.0i;         // Complex numbers
iq sample = load_iq("data.iq"); // IQ samples
signal sig;                      // Digital signal
spectrum sp;                     // Frequency spectrum
```

### Functions
```white
fn calculate(x: f64, y: f64) -> f64 {
    return x * y + sqrt(x + y);
}

fn process(signal s) -> spectrum {
    return fft(s, 4096);
}
```

### Control Flow
```white
if (frequency > 1 GHz) {
    print("Microwave band");
} else if (frequency > 1 MHz) {
    print("RF band");
} else {
    print("Low frequency");
}

while (running) {
    process_samples();
}

for (i = 0; i < 1000; i = i + 1) {
    accumulate(i);
}
```

## Compiler Commands

```bash
# Compile and run
./build/white program.white

# Build executable
./build/white build program.white -o program

# Check syntax only
./build/white check program.white

# Generate C source code
./build/white emit-c program.white

# Compile with defensive polymorphic code generation
./build/white build program.white --polymorphic

# Compile with reproducible seed for testing
./build/white build program.white --polymorphic-seed 0x12345678

# Verbose compilation
./build/white -v program.white

# Show version
./build/white version

# Show help
./build/white help
```

## Architecture

### Compilation Pipeline

```
White Source Code
       ↓
  LEXER (lexer.c)
    Tokens
       ↓
  PARSER (parser.c)
   AST Nodes
       ↓
  SEMANTIC ANALYSIS (semantic.c)
    Type Checking
       ↓
  CODE GENERATION (codegen.c)
   C Source Code
       ↓
  C COMPILER (gcc/clang)
    Executable
```

### Project Structure

```
White/
├── include/white/          # Compiler headers
│   ├── common.h           # Common types & utilities
│   ├── lexer.h            # Tokenization
│   ├── parser.h           # Parsing
│   ├── ast.h              # AST definitions
│   ├── types.h            # Type system
│   ├── semantic.h         # Type checking
│   ├── diagnostics.h      # Error reporting
│   ├── codegen.h          # Code generation
│   ├── compiler.h         # Compiler driver
│   ├── polymorphic.h      # Polymorphic code gen
│   ├── obfuscation.h      # Code obfuscation
│   └── runtime.h          # Runtime library
├── src/                    # Compiler implementation
│   ├── main.c             # CLI entry point
│   ├── lexer.c            # Tokenizer
│   ├── parser.c           # Parser
│   ├── ast.c              # AST construction
│   ├── types.c            # Type system impl
│   ├── semantic.c         # Semantic analysis
│   ├── diagnostics.c      # Error diagnostics
│   ├── codegen.c          # Code generator
│   ├── compiler.c         # Compiler orchestration
│   ├── polymorphic.c      # Polymorphic transforms
│   ├── obfuscation.c      # Obfuscation impl
│   ├── common.c           # Utility functions
│   └── util.c             # String & array utils
├── runtime/               # Runtime library
│   ├── white_runtime.h    # Runtime API
│   ├── white_runtime.c    # Core runtime
│   ├── signal.c           # Signal processing
│   ├── spectrum.c         # Spectrum analysis
│   ├── complex.c          # Complex numbers
│   └── math.c             # Math functions
├── std/                   # Standard library (White)
│   ├── math.white         # Math library
│   ├── signal.white       # Signal processing
│   ├── spectrum.white     # Spectrum analysis
│   └── physics.white      # Physics library
├── examples/              # Example programs
│   ├── hello.white        # Hello World
│   ├── fib.white          # Fibonacci
│   ├── physics.white      # Physics calculations
│   ├── filter.white       # Signal filtering
│   ├── circuits.white     # RF circuits
│   ├── fft.white          # FFT example
│   ├── iq.white           # IQ processing
│   └── spectrum.white     # Spectrum analysis
├── tests/                 # Test suite
│   ├── test_main.c        # Comprehensive tests
│   └── README.md          # Test documentation
├── docs/                  # Documentation
│   ├── architecture.md    # Design & architecture
│   ├── POLYMORPHIC.md     # Polymorphic code gen
│   ├── QUICKSTART.md      # Quick start guide
│   └── compiler.md        # Compiler internals
├── Makefile               # Build system
├── README.md              # This file
├── LICENSE                # Apache 2.0
├── LANGUAGE.md            # Language specification
└── CHANGELOG.md           # Version history
```

## Standard Library

### Math Module (`std/math.white`)
- Constants: PI, E, TAU, SQRT2, SQRT_HALF
- Functions: abs(), min(), max(), clamp()

### Signal Module (`std/signal.white`)
- Windows: Hann, Hamming, Blackman
- Resampling: decimate(), interpolate()
- Processing: apply_window(), resample()

### Spectrum Module (`std/spectrum.white`)
- Analysis: welch_psd(), periodogram(), bartlett_psd()
- Detection: find_peaks(), bandwidth_3db()
- Metrics: spectral_flatness(), center_frequency()
- Measurements: estimate_noise_floor()

### Physics Module (`std/physics.white`)
- Constants: SPEED_OF_LIGHT, PLANCK_CONSTANT, BOLTZMANN_CONSTANT, etc.
- RF Functions: wavelength_from_frequency(), frequency_from_wavelength()
- Circuit: impedance_free_space(), vswr_from_reflection()
- Propagation: friss_path_loss()

## Type System

### Primitive Types
- `bool` - Boolean (true/false)
- `i8`, `i16`, `i32`, `i64` - Signed integers
- `u8`, `u16`, `u32`, `u64` - Unsigned integers  
- `f32`, `f64` - Floating point
- `string` - String literals

### RF/DSP Types
- `complex` - Complex numbers (a + bi)
- `iq` - IQ sample pair (I and Q components)
- `frequency` - Frequency in Hz
- `sample_rate` - Sampling rate
- `bandwidth` - Signal bandwidth
- `amplitude` - Signal amplitude
- `phase` - Phase angle
- `power` - Power (W, dBm)
- `signal` - Digital signal (array of IQ samples)
- `spectrum` - Frequency spectrum (magnitude & phase)

### Derived Types
- `[T; N]` - Fixed-size array of T with N elements
- `[T]` - Dynamic array of T
- `*T` - Pointer to T
- `struct Name { ... }` - Structures
- `enum Name { ... }` - Enumerations

## Signal Processing Features

### FFT & Spectrum Analysis
```white
fn analyze(signal s) -> spectrum {
    // Compute FFT with 4096-point DFT
    spectrum sp = fft(s, 4096);
    
    // Get spectrum metrics
    frequency peak_freq = sp.peak();
    bandwidth bw = sp.bandwidth();
    f64 snr_db = sp.snr();
    f64 power = sp.power();
    
    return sp;
}
```

### IQ Data Processing
```white
fn process_iq() {
    sample_rate sr = 10000000;  // 10 MHz
    
    // Load IQ samples from file
    signal s = iq("capture.iq", sr);
    
    // Perform analysis
    spectrum sp = fft(s, 4096);
    
    print("Samples: ", s.length);
    print("Sample Rate: ", sr);
}
```

### Physics Calculations
```white
fn rf_analysis() {
    frequency f = 2400000000;     // 2.4 GHz
    f64 distance = 100.0;          // 100 meters
    
    // Calculate wavelength
    f64 wavelength = SPEED_OF_LIGHT / f;
    
    // Calculate free-space path loss
    f64 path_loss = friss_path_loss(f, distance);
    
    print("Wavelength: ", wavelength, " m");
    print("Path Loss: ", path_loss, " dB");
}
```

## Polymorphic Code Generation

White supports **polymorphic code generation** - generating functionally identical but structurally different binaries on each compilation. This is a **legitimate defensive security feature** for:

✓ Academic research and study
✓ Defensive security analysis  
✓ Anti-reverse engineering of legitimate software
✓ Critical RF and signal processing code protection
✓ Communications research

**NOT for malware, persistence, evasion, or any destructive behavior.**

### Usage

```bash
# Generate polymorphic binary
./build/white build program.white --polymorphic

# Reproducible build with seed
./build/white build program.white --polymorphic-seed 0x12345678

# Maximum obfuscation
./build/white build program.white --polymorphic --obfuscation-level 3
```

### Techniques
- Instruction reordering (independent statements shuffled)
- Dead code insertion (harmless computational stubs)
- Constant obfuscation (XOR encoding with random keys)
- Register reassignment (varies storage locations)
- NOP injection (padding instruction sequences)
- Function inlining/outlining (varying call patterns)

See `docs/POLYMORPHIC.md` for detailed information and ethical guidelines.

## Building & Installation

### Requirements
- GCC or Clang with C11 support
- GNU Make
- POSIX-compatible system (Linux, macOS, BSD, WSL)
- ~5 MB disk space

### Build

```bash
cd White
make clean
make
```

This produces:
- `build/white` - The White compiler
- Object files in `src/` and `runtime/`

### Clean Build

```bash
make clean          # Remove build artifacts
make all            # Rebuild everything
make verbose        # Build with debug symbols
make test           # Run test suite
```

## Testing

```bash
# Run comprehensive test suite
cd tests
make test

# Individual test modules
gcc -Wall -Wextra -std=c11 -I../include -lm -o test_runner test_main.c ../src/*.c ../runtime/*.c
./test_runner
```

Test coverage:
- ✓ Lexer (tokenization)
- ✓ Parser (AST generation)
- ✓ Type system (type checking)
- ✓ Semantic analysis (symbol resolution)
- ✓ Code generation (C emission)
- ✓ Runtime (signal processing, complex math)

## Examples

See `examples/` directory:

- **hello.white** - Simple print statement
- **fib.white** - Fibonacci function and loop
- **physics.white** - Physics calculations (wavelength, speed of light)
- **filter.white** - Signal filtering (FIR/IIR concepts)
- **circuits.white** - RF circuit analysis (VSWR, reflection)
- **fft.white** - FFT analysis example
- **iq.white** - IQ data file processing
- **spectrum.white** - Spectrum analysis with parameters

### Running Examples

```bash
./build/white examples/hello.white
./build/white examples/physics.white
./build/white examples/spectrum.white
```

## Compiler Options

```bash
white [command] [options] <file.white>

Commands:
  (none)            Compile and run
  build             Compile to executable
  run               Compile and execute
  check             Check syntax only
  emit-c            Generate C source code
  version           Print version
  help              Show this help

Options:
  -v, --verbose     Verbose output
  -o FILE           Output file
  --polymorphic     Enable polymorphic code generation
  --polymorphic-seed N  Use specific seed for reproducibility
  --obfuscation-level L  Obfuscation level (0-3)
```

## Documentation

- **README.md** (this file) - Overview and quick start
- **LANGUAGE.md** - Complete language specification and grammar
- **CHANGELOG.md** - Version history and features
- **docs/architecture.md** - Compiler design and architecture
- **docs/QUICKSTART.md** - Detailed quick start guide
- **docs/POLYMORPHIC.md** - Polymorphic code generation (defensive security)
- **tests/README.md** - Test suite documentation

## Implementation Status

### Completed ✓
- [x] Complete C11 compiler implementation
- [x] Full lexical analysis (lexer)
- [x] Recursive descent parser
- [x] Abstract Syntax Tree (AST)
- [x] Type system with RF/DSP types
- [x] Semantic analysis and type checking
- [x] C code generation backend
- [x] Runtime library (signal processing)
- [x] FFT implementation (DFT-based)
- [x] Spectrum analysis functions
- [x] Complex number arithmetic
- [x] IQ sample support
- [x] Diagnostic system (line/column reporting)
- [x] Standard library modules (math, signal, spectrum, physics)
- [x] Command-line interface
- [x] Polymorphic code generation
- [x] Code obfuscation framework
- [x] Comprehensive test suite
- [x] Example programs
- [x] Full documentation

### Future Roadmap

**Version 0.2 (Next Release)**
- [ ] Optimized Cooley-Tukey FFT
- [ ] Module system with imports
- [ ] Improved error recovery in parser
- [ ] Better type inference
- [ ] Inline assembly for hot paths

**Version 0.3**
- [ ] LLVM IR backend
- [ ] Native code generation
- [ ] Optimization passes (constant folding, dead code elimination)
- [ ] WebAssembly target

**Version 0.4**
- [ ] Hardware interfaces (SDR, oscilloscope)
- [ ] Real-time processing support
- [ ] Generics and parametric types
- [ ] SIMD vectorization

**Version 0.5+**
- [ ] Extended physics library
- [ ] Dimensional analysis
- [ ] Distributed computing support
- [ ] GPU acceleration (CUDA, OpenCL)
- [ ] Professional IDE support

## Performance

### Compilation Speed
- Small programs (<100 lines): <100ms
- Medium programs (~1K lines): <500ms  
- Large programs (~10K lines): <2s

### Runtime Performance
- DFT-based FFT: O(n²) - suitable for n < 4096
- Spectrum analysis: ~1M samples/second
- Complex arithmetic: near-native C performance

### Memory Usage
- Compiler memory: ~10-50 MB
- Runtime memory: proportional to signal size
- Small overhead for type information

## License

White is released under the **Apache License 2.0**. See LICENSE file for details.

Key provisions:
- ✓ Free for commercial and personal use
- ✓ Modification and distribution allowed
- ✓ Patent protection
- ✓ Limitation of liability

## Security Notice

### Polymorphic Code Generation

The polymorphic code generation feature is **restricted to legitimate purposes**:

✓ **Legitimate Use**
- Defensive security research
- Anti-reverse engineering of authorized software
- Academic study of code transformation
- Critical RF/SDR system protection

✗ **Prohibited Use**
- Malware development
- Creating persistence mechanisms
- Credential theft
- System destruction
- Unauthorized access
- Evasion of detection systems
- Any activity violating applicable law

**Violators may face civil and criminal liability.**

See `docs/POLYMORPHIC.md` for complete ethical guidelines and technical details.

## Contributing

Contributions welcome! Areas for improvement:
- Performance optimization
- New RF/DSP operations  
- Hardware interface modules
- Optimization passes
- Better error messages
- Documentation improvements
- Test coverage expansion

## Acknowledgments

White builds on decades of compiler research and signal processing development. Special thanks to:
- The C standardization committee (C11 standard)
- Compiler design community (Dragon Book, etc.)
- DSP and RF engineering communities
- Open-source software principles

## Support

- **Issues**: Report bugs via GitHub Issues
- **Discussions**: Use GitHub Discussions for questions
- **Documentation**: See docs/ directory
- **Examples**: Check examples/ for reference code

## Citation

If you use White in academic research, please cite:

```bibtex
@software{white2026,
  title={White: A Domain-Specific Language for RF Engineering and Scientific Computing},
  author={Black, 210},
  year={2026},
  url={https://github.com/black-210/White}
}
```

## Contact

- GitHub: [@black-210](https://github.com/black-210)
- Repository: [github.com/black-210/White](https://github.com/black-210/White)

---

**White v1.0.0** - Complete RF Engineering Language Implementation in C11

*"Precision. Performance. Purpose."*
