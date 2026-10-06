#include "SimulationSetup/SimulationBoundaries/SimulationBoundaries.hpp"

SimulationBoundaries::SimulationBoundaries(Mesh& Mesh_Obj)
    : TopWall(
          Mesh_Obj,
          SolverInputs::BoundaryCondition::TopWallValue,
          BoundaryType::Dirichlet,
          BoundaryOrientation::Horizontal,
          BoundaryLocation::Top),
      BottomWall(
          Mesh_Obj,
          SolverInputs::BoundaryCondition::BottomWallValue,
          BoundaryType::Dirichlet,
          BoundaryOrientation::Horizontal,
          BoundaryLocation::Bottom),
      RightWall(
          Mesh_Obj,
          SolverInputs::BoundaryCondition::RightWallValue,
          BoundaryType::Dirichlet,
          BoundaryOrientation::Vertical,
          BoundaryLocation::Right),
      LeftWall(
          Mesh_Obj,
          SolverInputs::BoundaryCondition::LeftWallValue,
          BoundaryType::Dirichlet,
          BoundaryOrientation::Vertical,
          BoundaryLocation::Left),
      Type(BoundaryType::Dirichlet)
{}
