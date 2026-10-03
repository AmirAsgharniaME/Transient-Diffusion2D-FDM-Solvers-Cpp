cmake_minimum_required(VERSION 3.20)

project(Transient_Diffusion2D_FDM_Solvers_Cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CheckIPOSupported)
check_ipo_supported(RESULT CFD_IPO_SUPPORTED OUTPUT CFD_IPO_ERROR LANGUAGES CXX)
if(CFD_IPO_SUPPORTED)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
else()
    message(STATUS "IPO/LTO is unavailable: ${CFD_IPO_ERROR}")
endif()

function(configure_cfd_library target_name)
    target_include_directories(${target_name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -O3)
    elseif(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /O2)
    endif()
endfunction()

function(configure_cfd_executable target_name)
    target_include_directories(${target_name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -O3)
    elseif(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /O2)
    endif()
endfunction()

# Core data structures and mesh implementation.
add_library(cfd_core STATIC
    Core/Mesh/Mesh.cpp
    Core/Field/Field.cpp
)
configure_cfd_library(cfd_core)

# Explicit finite-difference schemes.
add_library(cfd_explicit STATIC
    Discretization/Explicit/FTCS/FTCS.cpp
    Discretization/Explicit/DuFortFrankel/DuFortFrankel.cpp
)
configure_cfd_library(cfd_explicit)
target_link_libraries(cfd_explicit PUBLIC cfd_core)

# Linear system solvers.
add_library(cfd_linear_solvers STATIC
    LinearSolvers/GaussianElimination/GaussianElimination.cpp
    LinearSolvers/ThomasAlgorithm/ThomasAlgorithm.cpp
)
configure_cfd_library(cfd_linear_solvers)
target_link_libraries(cfd_linear_solvers PUBLIC cfd_core)

# Implicit schemes and boundary enforcement.
add_library(cfd_implicit STATIC
    Discretization/Implicit/Boundary/EnforceBoundary.cpp
    Discretization/Implicit/CrankNicolson/CrankNicolsonAssembler.cpp
    Discretization/Implicit/Laasonen/LaasonenAssembler.cpp
)
configure_cfd_library(cfd_implicit)
target_link_libraries(cfd_implicit PUBLIC cfd_core cfd_linear_solvers)

# File output and visualization.
add_library(cfd_io STATIC
    IO/OutputWriter/FileWriter/FileWriter.cpp
    IO/Visualization/LivePointPlotter/LivePointPlotter.cpp
    IO/Visualization/Plotter/Plotter.cpp
)
configure_cfd_library(cfd_io)
target_link_libraries(cfd_io PUBLIC cfd_core)

# Convergence and runtime utilities.
add_library(cfd_utilities STATIC
    Utilities/Convergence/RelativeFieldChange/FieldChange.cpp
)
configure_cfd_library(cfd_utilities)
target_link_libraries(cfd_utilities PUBLIC cfd_core)

# Analytical reference solutions.
add_library(cfd_verification STATIC
    Verification/AnalyticalSolution/AnalyticalSolution.cpp
)
configure_cfd_library(cfd_verification)
target_link_libraries(cfd_verification PUBLIC cfd_core)

# Shared simulation setup and orchestration.
add_library(cfd_simulation_setup STATIC
    SimulationSetup/SimulationContext/SimulationContext.cpp
)
configure_cfd_library(cfd_simulation_setup)
target_link_libraries(cfd_simulation_setup
    PUBLIC
        cfd_core
        cfd_io
        cfd_utilities
        cfd_verification
)

# Solver executables.
add_executable(Explicit_FTCS
    main/Explicit_FTCS.cpp
)
configure_cfd_executable(Explicit_FTCS)
target_link_libraries(Explicit_FTCS PRIVATE cfd_explicit cfd_simulation_setup)

add_executable(Explicit_DUFORT_FRANKEL
    main/Explicit_DUFORT_FRANKEL.cpp
)
configure_cfd_executable(Explicit_DUFORT_FRANKEL)
target_link_libraries(Explicit_DUFORT_FRANKEL PRIVATE cfd_explicit cfd_simulation_setup)

add_executable(Implicit_Laasonen
    main/Implicit_Laasonen.cpp
)
configure_cfd_executable(Implicit_Laasonen)
target_link_libraries(Implicit_Laasonen
    PRIVATE
        cfd_implicit
        cfd_linear_solvers
        cfd_simulation_setup
)

add_executable(Implicit_CrankNicolson
    main/Implicit_CrankNicolson.cpp
)
configure_cfd_executable(Implicit_CrankNicolson)
target_link_libraries(Implicit_CrankNicolson
    PRIVATE
        cfd_implicit
        cfd_linear_solvers
        cfd_simulation_setup
)