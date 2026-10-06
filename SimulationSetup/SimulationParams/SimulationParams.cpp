#include "SimulationSetup/SimulationParams/SimulationParams.hpp"

SimulationParams::SimulationParams(const Mesh& grid, const Geometry& Geometry_Obj)
    : alpha(SolverInputs::Physics::alpha),
      NumTimeLevels(SolverInputs::Solver::NumTimeLevels),
      dt(SolverInputs::Solver::dt),
      Tolerance(SolverInputs::Solver::Tolerance),
      DeltaX(grid.Get_DeltaX()),
      DeltaY(grid.Get_DeltaY()),
      DiffNumberX(alpha.GetValue() * dt.GetValue() / (DeltaX.GetValue() * DeltaX.GetValue())),
      DiffNumberY(alpha.GetValue() * dt.GetValue() / (DeltaY.GetValue() * DeltaY.GetValue())),
      t_Scale(TimeScale::Return(Geometry_Obj, alpha.GetValue()))
{}
