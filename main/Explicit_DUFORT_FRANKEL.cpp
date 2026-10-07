// ===========
// C++ Libraries
// ===========
#include <string>
#include <iostream>
#include <cmath>
#include <array>


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
//Define New Fields According to the DuFortFrankel Scheme
Field T_1(meshes.Grid);
Field T_nMinus1(meshes.Grid);
//-----------------------------------------------------------------------------------------------
//Start Up
//===========================================
//FTCS is a StartUp for DuFortFrankel Method
//T_0----->Explicit FTCS------> T_1
//=========================================== 

if ((params.DiffNumberX.GetValue() + params.DiffNumberY.GetValue()) <= 0.5)
{
    std::cout<<"FTCS Scheme is Stable For StartUp"<<'\n';
    DiffusionEQ::FTCS::Solve_nPlus1(
        fields.T_0,
        T_1,
        params.DiffNumberX.GetValue(),
        params.DiffNumberY.GetValue());
    //Apply Boundary Conditions To New Field
    T_1.ApplyBoundaryCondition(boundaries.TopWall);
    T_1.ApplyBoundaryCondition(boundaries.BottomWall);
    T_nMinus1.Swap(fields.T_0);

    fields.T_n.Swap(T_1);
}

if ((params.DiffNumberX.GetValue() + params.DiffNumberY.GetValue()) > 0.5)
{
    double Original_DiffNumberX = params.DiffNumberX.GetValue();
    double Original_DiffNumberY = params.DiffNumberY.GetValue();
    double Original_dt = params.dt.GetValue(); 

    //Stabilization
    Stabilization(meshes.Grid, params);
    double Lower_DiffNumberX = params.DiffNumberX.GetValue();
    double Lower_DiffNumberY = params.DiffNumberY.GetValue();
    double Lower_dt = params.dt.GetValue();
    // Choose enough substeps so each substep is no larger than the stable step.
    const std::size_t n = static_cast<std::size_t>(std::ceil(Original_dt / Lower_dt));
    const double Substep_dt = Original_dt / static_cast<double>(n);
    const double Last_Substep_dt =
        Original_dt - Substep_dt * static_cast<double>(n - 1);
    const double Substep_Scale = Substep_dt / Lower_dt;
    const double Last_Substep_Scale = Last_Substep_dt / Lower_dt;
    const double Substep_DiffNumberX = Lower_DiffNumberX * Substep_Scale;
    const double Substep_DiffNumberY = Lower_DiffNumberY * Substep_Scale;
    const double Last_Substep_DiffNumberX = Lower_DiffNumberX * Last_Substep_Scale;
    const double Last_Substep_DiffNumberY = Lower_DiffNumberY * Last_Substep_Scale;
    //Restore the Original Values to Params
    params.dt.SetValue(Original_dt);
    params.DiffNumberX.SetValue(Original_DiffNumberX);
    params.DiffNumberY.SetValue(Original_DiffNumberY);

    //Saving T_(n-1) = T_0
    T_nMinus1 = fields.T_0;
    for (size_t i = 1; i <= n; i++)
    {
        const double DiffNumberX =
            (i == n) ? Last_Substep_DiffNumberX : Substep_DiffNumberX;
        const double DiffNumberY =
            (i == n) ? Last_Substep_DiffNumberY : Substep_DiffNumberY;
        DiffusionEQ::FTCS::Solve_nPlus1(
            fields.T_0,
            T_1,
            DiffNumberX,
            DiffNumberY);
        //Apply Boundary Conditions To New Field
        T_1.ApplyBoundaryCondition(boundaries.TopWall);
        T_1.ApplyBoundaryCondition(boundaries.BottomWall);
        T_1.ApplyBoundaryCondition(boundaries.RightWall);
        T_1.ApplyBoundaryCondition(boundaries.LeftWall);
        if (i==n)
        {
            //T_(n) = T_1
            fields.T_n.Swap(T_1);
            break;
        }
        //update
        fields.T_0.Swap(T_1);
    }
}
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

std::array<double, 3> Coeffs =  DiffusionEQ::DUFORT_FRANKEL::ReturnCoeffs
(
params.DiffNumberX.GetValue(),
params.DiffNumberY.GetValue()
);
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
DiffusionEQ::DUFORT_FRANKEL::Solve_nPlus1(
    fields.T_n,
    T_nMinus1,
    fields.T_nPlus1,
    Coeffs);

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
T_nMinus1.Swap(fields.T_n);
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
        Path::Create(OutputCategory::Numerical, SolverScheme::DUFORT_FRANKEL)
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
    Path::Create(OutputCategory::Numerical, SolverScheme::DUFORT_FRANKEL)
    );

contours.Window.AddContour
    (
    Path::Create(OutputCategory::Numerical, SolverScheme::DUFORT_FRANKEL),
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
    std::cout << "Calculations Completed Successfully for " << std::string(To_String(SolverScheme::DUFORT_FRANKEL)) << std::endl;
    std::cin.get();
    return 0;
}
