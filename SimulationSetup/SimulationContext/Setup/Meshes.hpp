#pragma once

#include "Config/SolverInputs.hpp"
#include "Core/Mesh/Mesh.hpp"
#include "Core/Setup/Geometries.hpp"

struct Meshes
{
    Mesh Grid; 

    explicit Meshes(const Geometries& Geometries_Obj)
        :Grid(Geometries_Obj.G1,SolverInputs::Mesh::Nx,SolverInputs::Mesh::Ny)
    {}
};