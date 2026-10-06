#pragma once
#include "Core/Boundary/Boundary.hpp"
#include "Config/SolverInputs.hpp"
#include "Core/Mesh/Mesh.hpp"
class SimulationBoundaries
{
public:
    Boundary TopWall;
    Boundary BottomWall;
    Boundary RightWall;
    Boundary LeftWall;


    BoundaryType GetType() const noexcept
    {
        return Type;
    }

    explicit SimulationBoundaries(Mesh& Mesh_Obj);

    private:
    BoundaryType Type;
};
