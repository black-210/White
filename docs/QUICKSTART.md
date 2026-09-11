# White Compiler - Quick Start Guide

## Installation

### Prerequisites
- GCC or Clang with C11 support
- GNU Make
- POSIX-compatible system (Linux, macOS, BSD)

### Building from Source

```bash
git clone https://github.com/black-210/White.git
cd White
make clean
make
```

This produces `build/white` - the White compiler executable.

## First Program

Create `hello.white`:
```white
print("Hello from White!");
```

Compile and run:
```bash
./build/white hello.white
```

## Language Basics

### Variables
```white
let x = 42;              // Integer
let y: f64 = 3.14;      // Floating point with type
const PI = 3.14159;     // Constant
```

### Functions
```white
fn add(a: i32, b: i32) -> i32 {
    return a + b;
}

fn greet(name: string) {
    print("Hello, ", name);
}
```

### Control Flow
```white
// If/else
if (x > 10) {
    print("x is large");
} else if (x > 5) {
    print("x is medium");
} else {
    print("x is small");
}

// Loops
while (condition) {
    // ...
}

for (i = 0; i < 10; i = i + 1) {
    print(i);
}
```

### RF/DSP Types
```white
frequency f = 2400000000;     // 2.4 GHz
sample_rate sr = 10000000;     // 10 MHz
bandwidth bw = 20000000;        // 20 MHz
signal s = iq("data.iq", sr);  // Load IQ data
spectrum sp = fft(s, 4096);     // Compute FFT
```

## Compiler Commands

```bash
# Compile and run
./build/white program.white

# Check syntax only
./build/white check program.white

# Generate C code
./build/white emit-c program.white

# Build with polymorphic code generation (defensive)
./build/white build program.white --polymorphic

# Build with reproducible seed
./build/white build program.white --polymorphic-seed 0x12345678

# Show version
./build/white version

# Show help
./build/white help
```

## Examples

See `examples/` directory for complete examples:

- `hello.white` - Simple print statement
- `fib.white` - Fibonacci function and loop
- `physics.white` - Physics calculations
- `filter.white` - Signal filtering
- `circuits.white` - RF circuit analysis
- `fft.white` - FFT analysis
- `iq.white` - IQ data processing
- `spectrum.white` - Spectrum analysis

## Standard Library

### Math (`math.white`)
- Constants: PI, E, TAU, SQRT2
- Functions: abs(), min(), max(), clamp()

### Signals (`signal.white`)
- Windows: Hann, Hamming, Blackman
- Resampling: decimate(), interpolate()
- Windowing: apply_window()

### Spectrum (`spectrum.white`)
- Analysis: welch_psd(), periodogram()
- Detection: find_peaks(), bandwidth_3db()
- Metrics: spectral_flatness(), center_frequency()

### Physics (`physics.white`)
- Constants: SPEED_OF_LIGHT, PLANCK_CONSTANT, etc.
- Functions: wavelength_from_frequency(), impedance_free_space()
- Calculations: friss_path_loss(), vswr_from_reflection()

## Common Patterns

### Reading IQ Data
```white
fn process_iq_file(string filename, sample_rate sr) {
    signal s = iq(filename, sr);
    spectrum sp = fft(s, 4096);
    
    frequency peak = sp.peak();
    f64 snr = sp.snr();
    
    print("Peak: ", peak);
    print("SNR: ", snr);
}
```

### RF Calculations
```white
fn calculate_link_budget(frequency f, f64 distance) {
    f64 wavelength = SPEED_OF_LIGHT / f;
    f64 path_loss = 20.0 * log10(4 * PI * distance / wavelength);
    
    print("Path Loss: ", path_loss, " dB");
}
```

### Defensive Code with Polymorphism
```bash
# Build with maximum obfuscation
./build/white build critical.white --polymorphic --obfuscation-level 3

# Build reproducible for testing
./build/white build critical.white --polymorphic-seed 0xDEADBEEF
```

## Troubleshooting

### Compilation Errors

**Error: file not found**
- Ensure file path is correct
- Check current directory with `pwd`

**Error: expected type**
- Check variable type declarations
- RF types must use proper syntax: `frequency`, not `freq`

**Error: unknown function**
- Ensure function is declared before use
- Check standard library imports

### Runtime Issues

**Signal not found**
- Verify IQ file exists and is readable
- Check file path is absolute or relative to working directory

**Spectrum analysis gives incorrect results**
- Ensure FFT size is power of 2
- Check sample rate is correct
- Verify signal length

## Performance Tips

1. **Use appropriate FFT sizes** (powers of 2: 256, 512, 1024, 4096, 8192)
2. **Minimize dead code insertion** for performance-critical code
3. **Use constant folding** for compile-time computations
4. **Profile before optimizing** - measure actual bottlenecks

## Further Reading

- `LANGUAGE.md` - Complete language specification
- `docs/architecture.md` - Compiler architecture
- `docs/POLYMORPHIC.md` - Polymorphic code generation
- `examples/` - More example programs
- `tests/` - Test suite for reference implementations

## Getting Help

- Check README.md for overview
- Review examples for common patterns
- Inspect LANGUAGE.md for syntax reference
- Read compiler error messages carefully

## License

White is released under Apache License 2.0. See LICENSE file.
