# White: A Systems Programming Language for RF Engineering and Scientific Computing

## Project Architecture

The White compiler is organized into modular components with clear separation of concerns:

### Source Layout

```
include/white/          Compiler headers (public API)
src/                    Compiler implementation
runtime/                Runtime library (C)
std/                    Standard library (White)
examples/               Example programs
tests/                  Test suites
docs/                   Documentation
```

### Compilation Stages

1. **Lexer** (`lexer.c`)
   - Tokenizes input source
   - Handles strings, numbers, keywords
   - Tracks line/column for diagnostics
   - Supports comments

2. **Parser** (`parser.c`)
   - Builds Abstract Syntax Tree (AST)
   - Recursive descent parser
   - Expression precedence handling
   - Error recovery (minimal)

3. **AST** (`ast.c`)
   - Node types for all language constructs
   - Tree manipulation functions
   - Memory management

4. **Type System** (`types.c`)
   - Type definitions and checking
   - Primitive and compound types
   - RF/DSP domain-specific types
   - Type compatibility checking

5. **Semantic Analysis** (`semantic.c`)
   - Symbol resolution
   - Type inference
   - Scope management
   - Function resolution

6. **Code Generation** (`codegen.c`)
   - Emits C source code from AST
   - Handles expressions, statements, functions
   - Type conversion to C types

7. **Compiler Driver** (`compiler.c`)
   - Orchestrates compilation pipeline
   - File I/O
   - Options handling

8. **CLI** (`main.c`)
   - Command-line interface
   - Mode selection (build, run, check, emit-c)
   - Help and version information

### Runtime Library

**white_runtime.c/h**
- Signal processing: FFT, IFFT, PSD
- Spectrum analysis: peak detection, bandwidth, SNR
- Complex arithmetic
- IQ sample handling
- Print utilities

### Type System Design

The type system distinguishes between:
- **Primitive types**: bool, integers, floats
- **Aggregate types**: arrays, structures
- **RF/DSP types**: frequency, bandwidth, signal, spectrum
- **Derived types**: pointers, functions

This enables:
- Type safety
- Dimensional analysis (future)
- Compile-time checking
- Better error messages

### Extensibility Points

#### Adding New Primitives
1. Add `TypeKind` enum value in `types.h`
2. Add case to `type_to_c_type()` in `types.c`
3. Add parsing in `parser.c`
4. Add code generation in `codegen.c`

#### Adding New Operators
1. Add `TokenType` enum value in `lexer.h`
2. Add lexer recognition in `lexer.c`
3. Add parser precedence in `parser.c`
4. Add code generation in `codegen.c`

#### Adding New Keywords
1. Add to keyword table in `lexer.c`
2. Add `TokenType` in `lexer.h`
3. Add parser handling in `parser.c`
4. Add code generation as needed

#### Adding New AST Nodes
1. Add `ASTNodeType` in `ast.h`
2. Add data structure in `ast.h`
3. Add node creation function in `ast.c`
4. Add semantic analysis in `semantic.c`
5. Add code generation in `codegen.c`

### Memory Management

All allocations use:
- `xmalloc()`, `xcalloc()`, `xrealloc()` - malloc wrappers with error checking
- `xfree()` - safe free wrapper
- Proper cleanup in `_free()` functions

No garbage collection; explicit cleanup required.

### Error Handling

Diagnostics subsystem provides:
- Errors, warnings, notes
- Source location tracking
- Context lines
- Clean formatting

### Performance Considerations

- Dynamic arrays for variable-length collections
- String interning via hashing (future)
- AST walk-based code generation
- No intermediate representation optimization (future)

### Future Improvements

1. **Intermediate Representation (IR)**
   - Lower AST to simpler IR
   - Enable optimization passes
   - Support multiple backends

2. **LLVM Backend**
   - Emit LLVM IR instead of C
   - Better optimization
   - Native code generation

3. **Module System**
   - File-based modules
   - Import/export mechanism
   - Dependency tracking

4. **Optimization Passes**
   - Constant folding
   - Dead code elimination
   - Inlining
   - Loop unrolling

5. **Better Diagnostics**
   - Colored output
   - Suggestion engine
   - Error recovery

## Building

```bash
make          # Build compiler
make clean    # Remove build artifacts
make test     # Run basic tests
```

## Testing Strategy

Test organization:
- `tests/lexer/` - Tokenization tests
- `tests/parser/` - AST generation tests
- `tests/semantic/` - Type checking tests
- `tests/codegen/` - Code generation tests
- `tests/integration/` - End-to-end tests

## Contributing

To extend White:
1. Add feature headers to `include/white/`
2. Implement in corresponding `src/` file
3. Add tests
4. Update documentation
5. Ensure `make` builds cleanly
