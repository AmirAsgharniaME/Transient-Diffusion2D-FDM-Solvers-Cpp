#pragma once

#include "Utilities/Parameter/Parameter.hpp"
#include "Core/Mesh/Mesh.hpp"
#include "Core/Geometry/Geometry.hpp"
#include "Config/SolverInputs.hpp"
#include "Utilities/Convergence/RelativeFieldChange/TimeScale/TimeScale.hpp"
class SimulationParams
{
public:
    Parameter<double> alpha;
    Parameter<std::size_t> NumTimeLevels;
    Parameter<double> dt;
    Parameter<double> Tolerance;
    Parameter<double> DeltaX;
    Parameter<double> DeltaY;
    Parameter<double> DiffNumberX;
    Parameter<double> DiffNumberY;
    Parameter<double> t_Scale;
   
   
    explicit SimulationParams(const Mesh& grid,const Geometry& Geometry_Obj);
};
