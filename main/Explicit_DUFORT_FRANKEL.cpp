// C++ Libraries
#include <iostream>
#include <cmath>
//Local Files
#include "Discretization/Explicit/FTCS/FTCS.hpp"
#include "Discretization/SolverScheme/SolverScheme.hpp"
#include "SimulationSetup/SimulationContext/SimulationContext.hpp"
#include "Utilities/KeyboardHandler/KeyboardHandler.hpp"
#include "Discretization/Explicit/FTCS/FTCS.hpp"
#include "Discretization/Explicit/FTCS/FTCS_Stability.hpp"
#include "Discretization/Explicit/DuFortFrankel/DuFortFrankel.hpp"


int main() 
{
//-------------------------------------------------------------------------------------------------
//Simulation Setup
SimulationContext ctx(SolverScheme::DUFORT_FRANKEL);
//-----------------------------------------------------------------------------------------------


//Stabilization and Time Step

ctx.StartUp(); //T_n = T_1 and T_nMinus1 = T_0 //not working

//Start Up
if (ctx.params.DiffNumberX.GetValue()+ctx.params.DiffNumberY.GetValue() < 0.5)
{
std::cout<<"FTCS Scheme is Stable For StartUp"<<'\n';
Discretization::Explicit::FTCS::Step(
ctx.T_0, 
ctx.T_1, 
ctx.params.DiffNumberX.GetValue(),
ctx.params.DiffNumberY.GetValue());

//Apply Boundary Conditions To New Field
ctx.T_1.ApplyBoundaryCondition(ctx.boundaries.TopWall);
ctx.T_1.ApplyBoundaryCondition(ctx.boundaries.BottomWall);
ctx.T_1.ApplyBoundaryCondition(ctx.boundaries.RightWall);
ctx.T_1.ApplyBoundaryCondition(ctx.boundaries.LeftWall);
ctx.T_nMinus1.Swap(ctx.T_0);
ctx.T_n.Swap(ctx.T_1);
}


if (ctx.params.DiffNumberX.GetValue()+ctx.params.DiffNumberY.GetValue() >= 0.5)
{
    double Original_DiffNumberX = ctx.params.DiffNumberX.GetValue();
    double Original_DiffNumberY = ctx.params.DiffNumberY.GetValue();
    double Original_dt = ctx.params.dt.GetValue(); 
    //Stabilization
    Discretization::Explicit::FTCS::Stabilize(ctx.grid,ctx.params);
    double Lower_DiffNumberX = ctx.params.DiffNumberX.GetValue();
    double Lower_DiffNumberY = ctx.params.DiffNumberY.GetValue();
    double Lower_dt = ctx.params.dt.GetValue();
    //Calculating the substeps to get from U0 to U1
    std::size_t n = static_cast<std::size_t>(std::llround(Original_dt/ Lower_dt));
    //Restore the Original Values to Params
    ctx.params.dt.SetValue(Original_dt);
    ctx.params.DiffNumberX.SetValue(Original_DiffNumberX);
    ctx.params.DiffNumberY.SetValue(Original_DiffNumberY);

    //Saving T_n-1 = T_0
    ctx.T_nMinus1 = ctx.T_0;
    for (size_t i = 1; i <= n; i++)
    {
        Discretization::Explicit::FTCS::Step(
        ctx.T_0, 
        ctx.T_1, 
        Lower_DiffNumberX,
        Lower_DiffNumberY);
        //Apply Boundary Conditions To New Field
        ctx.T_1.ApplyBoundaryCondition(ctx.boundaries.TopWall);
        ctx.T_1.ApplyBoundaryCondition(ctx.boundaries.BottomWall);
        ctx.T_1.ApplyBoundaryCondition(ctx.boundaries.RightWall);
        ctx.T_1.ApplyBoundaryCondition(ctx.boundaries.LeftWall);
        if (i==n)
        {
            //T_nPlus1 = T_1
            ctx.T_n.Swap(ctx.T_1);
            break;
        }
        //update
        ctx.T_0.Swap(ctx.T_1);
    }
}


ctx.WriteInitialOutputs();

auto Coeffs = Discretization::Explicit::DuFortFrankel::CalculateCoefficients(
    ctx.params.DiffNumberX.GetValue(),ctx.params.DiffNumberY.GetValue());

std::size_t TotalTimeLevel = ctx.params.NumTimeLevels.GetValue();

for(std::size_t TimeLevel_n = 1; TimeLevel_n <= TotalTimeLevel; TimeLevel_n++)
{

//This Condition Stops The Loop Solver Iterations by Pressing ESC
if (isEscPressed())
{
    std::cout << "\nESC pressed. Exiting program now..." << '\n';
    return 0;
}

Discretization::Explicit::DuFortFrankel::Step(
ctx.T_nMinus1,
ctx.T_n,
ctx.T_nPlus1,
Coeffs);

ctx.Common(TimeLevel_n);

if(ctx.GetBreakCondition())
{
    break;
}

}//End of Solver Loop


ctx.plot.LiveWindow.Close();

std::cout << "Done" << '\n';
std::cin.get();
return 0;
}//End of main