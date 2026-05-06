# Water-Slosh

Water-Slosh is a C++20 fluid-simulation sandbox focused on building the core data structures for a grid-based/FLIP-style water solver.

The project currently contains lightweight math and grid primitives such as vectors, points, tensors, N-dimensional arrays, field layouts, and scalar fields with ghost-cell storage.

## Project Structure

- `Vec.h`, `Point.h`, `PointVecOps.h`: fixed-dimension math primitives and point/vector operations.
- `Tensor.h`: fixed-rank compile-time tensor storage.
- `ArrayND.h`: dynamic N-dimensional array storage with checked and unchecked indexing.
- `FieldTypes.h`: field layout tags such as `CellCentered` and `FaceCentered`.
- `ScalarField.h`: scalar grid field storage using a layout tag, physical shape, ghost cells, and world/field coordinate helpers.
- `FluidGrid.h`, `Particle.h`: early simulation-level types.
- `Tests/CoreTests.cpp`: dependency-free standalone tests for the core types.
- `Tests/run-core-tests.bat`: helper script that compiles and runs the tests with MSVC.

## Requirements

- Visual Studio 2026 with the C++ toolchain installed.
- C++20 support.
- Windows/MSVC for the provided batch test runner.

## Running Tests

From the repository root, run:

```powershell
.\Tests\run-core-tests.bat
```

The script loads the Visual Studio C++ environment, compiles `Tests/CoreTests.cpp`, and runs the generated executable.

Expected successful output includes:

```text
[PASS] Vec construction and access
[PASS] Vec arithmetic
[PASS] Vec floating-point helpers
[PASS] Point
[PASS] PointVecOps
[PASS] FieldTypes
[PASS] ScalarField index bounds
[PASS] Tensor construction, access, and arithmetic
[PASS] ArrayND construction, access, and fill
[PASS] ArrayND arithmetic and errors
All tests passed.
```

## Scalar Field Indexing Convention

`ScalarField` distinguishes between physical indices and stored data indices.

Physical indices describe the logical simulation field:

```text
0 <= index < physical_shape
```

Stored indices include ghost cells:

```text
-ghost_width <= index < physical_shape + ghost_width
```

Internally, physical/stored field indices are shifted into `ArrayND` data indices:

```text
data_index = field_index + ghost_width
```

This means a stencil index such as `(-1, 0)` is not physical, but it can still be stored if it lies inside the ghost layer.

## Field Layouts

Field layouts are defined in `FieldTypes.h`.

`CellCentered` represents one value per cell:

```text
offset = (0.5, 0.5, ...)
index_extend = (0, 0, ...)
```

`FaceCentered` represents one value per face along an axis. For example, a 2D x-face field has one extra value along x:

```text
offset = (0.0, 0.5)
index_extend = (1, 0)
```

These layout tags let `ScalarField` compute where values live and how large the physical field should be.

## Notes

The current tests use a small custom harness instead of GoogleTest or Catch2. This keeps the project dependency-free while the core architecture is still evolving.

As the simulation grows, likely next steps include:

- Implementing `ScalarField::sample()` and field-space interpolation.
- Separating raw field interpolation from simulation-aware sampling in `FluidGrid`.
- Adding validity masks or cell-type grids for solid/fluid/air handling.
- Moving to a fuller test framework if the test suite grows substantially.
