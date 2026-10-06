#pragma once
#include "Config/SolverInputs.hpp"
#include "Utilities/Parameter/Parameter.hpp"
#include "Core/Setup/Meshes.hpp"
#include "Config/TimeScale.hpp"
#include "Core/Setup/Geometries.hpp"

struct Params
{
    Parameter<double> alpha;
    Parameter<std::size_t> NumTimeLevels;
    Parameter<double> dt;
    Parameter<double> t_Scale;
    Parameter<double> Tolerance;
    Parameter<double> DiffNumberX;
    Parameter<double> DiffNumberY;

    explicit Params(const Meshes& Meshes_Obj,const Geometries& Geometries_Obj)

    {
        alpha.SetValue(SolverInputs::Physics::alpha);
        NumTimeLevels.SetValue(SolverInputs::Solver::NumTimeLevels);
        dt.SetValue(SolverInputs::Solver::dt);
        Tolerance.SetValue(SolverInputs::Solver::Tolerance);
        DiffNumberX.SetValue((alpha.GetValue() * dt.GetValue()) / (Meshes_Obj.Grid.Get_DeltaX() * Meshes_Obj.Grid.Get_DeltaX()));
        DiffNumberY.SetValue((alpha.GetValue() * dt.GetValue()) / (Meshes_Obj.Grid.Get_DeltaY() * Meshes_Obj.Grid.Get_DeltaY()));
        t_Scale.SetValue(TimeScale::Return(Geometries_Obj.G1,alpha.GetValue()));

    }
};