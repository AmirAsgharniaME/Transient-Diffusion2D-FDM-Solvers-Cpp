// ===========
// C++ Libraries
// ===========
#include <string>
#include <iostream>




// Config
#include "Config/SolverScheme.hpp"


// Utilities
#include "Utilities/KeyboardHandler/KeyboardHandler.hpp"
#include "Utilities/StatusPrinter/StatusPrinter.hpp"


// Core
#include "Core/Convergence/Convergence.hpp"


// Stability
#include "Stability/FTCS/Stabilization.hpp"



// Explicit Solvers
#include "ExplicitSolvers/ExplicitSolvers.hpp"


//Setup
#include "Core/Setup/Geometries.hpp"
#include "Core/Setup/Meshes.hpp"
#include "Core/Setup/Params.hpp"
#include "Core/Setup/Boundaries.hpp"
#include "Core/Setup/Fields.hpp"
#include "Core/Setup/Contours.hpp"

//Verification
#include "Verification/AnalyticalSolution/AnalyticalSolution.hpp"


int main() 
{
//-------------------------------------------------------------------------------------------------
//Setup
 Geometries geometries;
 Meshes meshes(geometries);
 Params params(meshes,geometries);
 Boundaries boundaries(meshes);
 Fields fields(meshes,boundaries,geometries);
 Contours contours;
//-----------------------------------------------------------------------------------------------
//Stabilization
Stabilization(meshes.Grid, params);
//-------------------------------------------------------------------------------------------------
//Start Up
fields.T_n.Swap(fields.T_0);
//-------------------------------------------------------------------------------------------------
//============================================================================
// Fields Status: T_n = T_0 and T_nPlus1 is empty
//============================================================================
//-------------------------------------------------------------------------------------------------
//Defining Loop Parameters
std::size_t TotalTimeLevel = params.NumTimeLevels.GetValue();
double dt_ = params.dt.GetValue();
double t_Scale = params.t_Scale.GetValue();
double DiffNumberX_ = params.DiffNumberX.GetValue();
double DiffNumberY_ = params.DiffNumberY.GetValue();

//------------------------------------------------------------------------------
for(std::size_t TimeLevel_n = 1; TimeLevel_n <= TotalTimeLevel; TimeLevel_n++){
//------------------------------------------------------------------------------Start Solver Loop

//This Condition Stops The Loop Solver Iterations by Pressing ESC
if (isEscPressed())
{
    std::cout << "\nESC pressed. Exiting program now..." << std::endl;
    return 0;
}

//ExplicitSolver For FTCS
DiffusionEQ::FTCS::Solve_nPlus1(
    fields.T_n,
    fields.T_nPlus1,
    DiffNumberX_,
    DiffNumberY_);

//Apply Boundary Conditions To Field
fields.T_nPlus1.ApplyBoundaryCondition(boundaries.TopWall);
fields.T_nPlus1.ApplyBoundaryCondition(boundaries.BottomWall);
fields.T_nPlus1.ApplyBoundaryCondition(boundaries.RightWall);
fields.T_nPlus1.ApplyBoundaryCondition(boundaries.LeftWall);

//Calculate The Relative Field Change from T_n to T_nPlus1
double RelativeFieldChange = RelativeFieldChange::Return(
    fields.T_n,
    fields.T_nPlus1,
     dt_,
     t_Scale);

// double RelativeChange = RelativeFieldChange1D::ReturnFieldChange(Setup.T_n,Setup.T_nPlus1,dt_,t_scale_);
StatusPrinter::Print("Relative Field Change",TimeLevel_n,RelativeFieldChange,dt_);

//Update
fields.T_n.Swap(fields.T_nPlus1);

//Live Plotter
if (TimeLevel_n % 20 == 0) 
{
contours.LiveWindow.AddPoints(
static_cast<double>(TimeLevel_n),
{
    {"rate_of_field_change", RelativeFieldChange}
});

}

if(TimeLevel_n % 200 == 0)
{
    FileWriter::WriteField
    (
        FileName::Create(FieldName::T,static_cast<double>(TimeLevel_n)*dt_),
        fields.T_n,
        meshes.Grid,
        Path::Create(OutputCategory::Numerical, SolverScheme::FTCS)
    );

    // contours.Window.AddContour
    // (
    //     Path::Create(OutputCategory::Numerical, SolverScheme::FTCS),
    //     FileName::Create(FieldName::T,static_cast<double>(TimeLevel_n)*dt_),
    //     Legend::Create(FieldName::T,static_cast<double>(TimeLevel_n)*dt_)
    // );
   
}

//Codition For Convergence To stady state Solution
if(RelativeFieldChange < params.Tolerance.GetValue())
{
StatusPrinter::Print("Relative Field Change",TimeLevel_n,RelativeFieldChange,dt_,true);
double FinalTime = static_cast<double>(TimeLevel_n) * dt_;

FileWriter::WriteField
    (
    FileName::Create(FieldName::T,FinalTime),
    fields.T_n,
    meshes.Grid,
    Path::Create(OutputCategory::Numerical, SolverScheme::FTCS)
    );

contours.Window.AddContour
    (
    Path::Create(OutputCategory::Numerical, SolverScheme::FTCS),
    FileName::Create(FieldName::T,FinalTime),
    Legend::Create(FieldName::T,FinalTime)
    );
   
AnalyticalSolution::Pass(
    fields.T_Analytical_n,
    FinalTime,
    params.alpha.GetValue(),
    geometries.G1,
    meshes.Grid,
    boundaries,
    fields.T_n);

FileWriter::WriteField(
    FileName::Create(FieldName::TAnalytical,FinalTime),
    fields.T_Analytical_n,
    meshes.Grid,
    Path::Create(OutputCategory::Analytical));

contours.Window.AddContour(
    Path::Create(OutputCategory::Analytical),
    FileName::Create(FieldName::TAnalytical,FinalTime),
    Legend::Create(FieldName::TAnalytical,FinalTime)
);

    break;
}
//----------------------------------------------------------------------------End Solver Loop
}
 contours.LiveWindow.Close();
//----------------------------------------------------------------------------



//-----------------------------------------------------------------------------------------------------------------------------
    std::cout << "Calculations Completed Successfully for " << std::string(To_String(SolverScheme::FTCS)) << std::endl;
    std::cin.get();
    return 0;
}