# Transient Diffusion 2D — Finite Difference Solvers in C++

<p align="center">
  <img alt="C++20" src="https://img.shields.io/badge/C%2B%2B-20-00599C?logo=c%2B%2B&logoColor=white">
  <img alt="License not specified" src="https://img.shields.io/badge/license-not%20specified-lightgrey">
  <img alt="Build status not configured" src="https://img.shields.io/badge/build-status%20not%20configured-lightgrey">
</p>

**A finite-difference solver suite for two-dimensional, time-dependent heat and diffusion problems.**

This project is an advanced mechanical engineering and computational fluid dynamics (CFD) portfolio implementation. It brings together spatial discretization, time integration, boundary-condition handling, linear-system solution, analytical verification, and field visualization in a modular C++ codebase.

> **Repository status:** No license file or continuous-integration workflow is present in this checkout. The corresponding badges report that status; replace them with the project’s selected license and CI workflow once those are established.

## Overview

The code advances a scalar field \(T(x,y,t)\), such as temperature or concentration, over a two-dimensional mesh. The implementation includes explicit and implicit finite-difference approaches:

| Scheme | Formulation | Notes |
| --- | --- | --- |
| FTCS | Explicit, forward time / centered space | Includes a diffusion-number stability check and time-step stabilization. |
| DuFort–Frankel | Explicit, three-time-level | Uses an FTCS start-up step before the DuFort–Frankel recurrence. |
| Laasonen | Implicit, backward Euler | Advanced with alternating-direction line sweeps and tridiagonal solves. |
| Crank–Nicolson | Implicit, trapezoidal time integration | Implemented with alternating-direction sweeps. |

The project also contains shared mesh, field, geometry, boundary, and simulation setup components; Thomas and Gaussian-elimination linear solvers; analytical reference-solution support; output writers; and gnuplot-based live and contour visualization. The primary executables are defined in `main/` and built by CMake.

## Mathematical formulation

For a homogeneous, isotropic medium with constant diffusivity \(\alpha\), the two-dimensional transient diffusion equation is

\[
\frac{\partial T}{\partial t}
= \alpha\left(
\frac{\partial^2 T}{\partial x^2}
+\frac{\partial^2 T}{\partial y^2}
\right) + S(x,y,t),
\qquad (x,y)\in\Omega,\quad t>0,
\]

where \(T\) is temperature (or a diffusing scalar), \(\alpha\) is the thermal diffusivity (or diffusivity), and \(S\) is an optional volumetric source term. The source-free model corresponds to \(S=0\). An initial field closes the transient problem:

\[
T(x,y,0)=T_0(x,y),\qquad (x,y)\in\Omega.
\]

Boundary data may be specified in Dirichlet or Neumann form on the boundary \(\partial\Omega\):

\[
T = T_b(x,y,t) \quad\text{on }\Gamma_D,
\qquad
\frac{\partial T}{\partial n}=g(x,y,t) \quad\text{on }\Gamma_N,
\qquad
\partial\Omega=\Gamma_D\cup\Gamma_N,
\]

where \(\partial/\partial n\) denotes the outward normal derivative. The current `Config/SolverInputs.hpp` setup supplies wall values and therefore represents a Dirichlet case. The boundary components also provide the framework used to enforce boundary data during the solver sweeps.

On a uniform Cartesian grid, the interior Laplacian is approximated with centered second differences:

\[
\left.\nabla^2T\right|_{i,j}^{n}
\approx
\frac{T_{i+1,j}^{n}-2T_{i,j}^{n}+T_{i-1,j}^{n}}{\Delta x^2}
+\frac{T_{i,j+1}^{n}-2T_{i,j}^{n}+T_{i,j-1}^{n}}{\Delta y^2}.
\]

## Project structure

Headers are kept alongside their implementations; this repository does not currently use a separate `include/` tree. There are no top-level `tests/` or `examples/` directories in this checkout.

```text
Transient_Diffusion2D_FDM_Solvers_Cpp/
├── ADI/                          # ADI fields, sweep assemblers, sweep boundaries
├── Config/                       # Solver input parameters and configuration notes
├── Core/                         # Geometry, mesh, field, and boundary abstractions
├── Discretization/
│   ├── Explicit/                  # FTCS and DuFort–Frankel implementations
│   ├── Implicit/                  # Laasonen, Crank–Nicolson, boundary enforcement
│   └── SolverScheme/              # Solver scheme identifiers
├── IO/
│   ├── OutputWriter/              # Field output and naming
│   ├── PathManager/               # Result path handling
│   └── Visualization/gnuplot/     # Live plots and contour plots
├── LinearSolvers/                 # Thomas algorithm, Gaussian elimination, matrices
├── main/                          # One entry point for each solver executable
├── Results/                       # Initial, numerical, and analytical field outputs
├── SimulationSetup/               # Parameters, boundaries, orchestration, post-processing
├── Utilities/                     # Parameters, convergence, status, keyboard handling
├── Verification/                  # Analytical reference solution
├── CMakeLists.txt
└── README.md
```

## Dependencies and prerequisites

- A C++20-capable compiler: GCC 10+ or Clang 10+ are reasonable baselines.
- CMake 3.20 or newer.
- Make (for the Makefile-based build commands below).
- `gnuplot` for the live and contour visualization components.
- The local sibling `cfd-toolkit` repository is part of the surrounding workspace context. **This project’s current `CMakeLists.txt` does not discover, include, or link it**; the components needed by the configured targets are built here as internal `cfd_*` libraries. No separate toolkit installation step is currently needed to build this checkout.

No third-party numerical package is declared by the CMake configuration. To use visualization, install gnuplot and ensure the `gnuplot` executable is on `PATH`; headless runs can omit the plotting interaction only if the corresponding application code is adapted/configured for that use.

## Build and run

From the repository root, configure and build all four solvers:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

The configured CMake generator defaults to the platform’s available generator. To explicitly use Make:

```bash
cmake -S . -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
make -C build -j"$(nproc)"
```

Executables are written to the repository’s `bin/` directory. Run one from the project root so its relative output paths resolve as expected:

```bash
./bin/Explicit_FTCS
./bin/Explicit_DUFORT_FRANKEL
./bin/Implicit_Laasonen
./bin/Implicit_CrankNicolson
```

The solver inputs—including domain dimensions, diffusivity, time step, number of time levels, mesh resolution, initial field, and wall values—are currently set in `Config/SolverInputs.hpp`. Results are written under `Results/`; plotting features invoke gnuplot at runtime.

## Author and academic context

This repository is presented as an advanced mechanical engineering / CFD computational portfolio project. It demonstrates the translation of transient diffusion models into finite-difference algorithms, comparison of explicit and implicit time-marching strategies, ADI-based line solves, treatment of boundary conditions, and comparison with analytical reference fields. The modular organization is intended to support numerical-method study, code review, and future extension to additional discretizations and physical models.
