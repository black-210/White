# White 0.2

White is a portable C11 language and compiler for systems, RF engineering, DSP, and scientific computing.

## v0.2 highlights

- Fast radix-2 Cooley–Tukey FFT for power-of-two sizes.
- Correct zero-padding and bounds-safe FFT input handling.
- DFT fallback for arbitrary sizes.
- Stable version reporting: `White 0.2.0`.
- Existing White 0.1 syntax and runtime API remain compatible.

## Build

```bash
make clean
make
./build/white version
```

## Run an example

```bash
./build/white examples/hello.white
```

## Release scope

White 0.2 is a focused runtime/compiler-quality release, not a claim that the language already replaces Python. A credible general-purpose language needs a larger staged roadmap: modules and packages, complete type checking, a batteries-included standard library, tooling, documentation, benchmarks, and a stable ecosystem. White's RF/DSP focus remains its differentiator.

## Roadmap

- **0.3:** imports/modules, improved diagnostics, stronger inference, constant folding.
- **0.4:** generics, SIMD-friendly arrays, profiling, WebAssembly target.
- **1.0:** stable specification, package manager, formatter, language server, comprehensive test and benchmark suites.

See [LANGUAGE.md](LANGUAGE.md) and [CHANGELOG.md](CHANGELOG.md) for details.
