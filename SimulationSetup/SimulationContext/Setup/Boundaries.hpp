#pragma once
#include "Core/Boundary/Boundary.hpp"
#include "Config/SolverInputs.hpp"
#include "Core/Setup/Meshes.hpp"

struct Boundaries
{
    Boundary TopWall;
    Boundary BottomWall;
    Boundary RightWall;    
    Boundary LeftWall;


    explicit Boundaries(Meshes& Meshes_Obj)
        :TopWall(
        Meshes_Obj.Grid,
        SolverInputs::BoundaryCondition::TopWallValue,
        BoundaryType::Dirichlet,
        BoundaryOrientation::Horizontal,
        BoundaryLocation::Top),
         BottomWall(
        Meshes_Obj.Grid,
        SolverInputs::BoundaryCondition::BottomWallValue,
        BoundaryType::Dirichlet,
        BoundaryOrientation::Horizontal,
        BoundaryLocation::Bottom),
         RightWall(
        Meshes_Obj.Grid,
        SolverInputs::BoundaryCondition::RightWallValue,
        BoundaryType::Dirichlet,
        BoundaryOrientation::Vertical,
        BoundaryLocation::Right),
         LeftWall(
        Meshes_Obj.Grid,
        SolverInputs::BoundaryCondition::LeftWallValue,
        BoundaryType::Dirichlet,
        BoundaryOrientation::Vertical,
        BoundaryLocation::Left)

    {
        
    }
};