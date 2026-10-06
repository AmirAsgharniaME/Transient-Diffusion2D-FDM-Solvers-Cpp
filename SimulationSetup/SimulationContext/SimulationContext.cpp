#include "SimulationSetup/SimulationContext/SimulationContext.hpp"

#include "IO/OutputWriter/FileWriter/FileWriter.hpp"
#include "IO/OutputWriter/FileName/FileName.hpp"
#include "IO/PathManager/PathManager/PathManager.hpp"
#include "IO/OutputWriter/Legend/Legend.hpp"
#include "Verification/AnalyticalSolution/AnalyticalSolution.hpp"
#include "Config/SolverInputs.hpp"
#include "Utilities/Convergence/RelativeFieldChange/FieldChange.hpp"
#include "Utilities/StatusPrinter/StatusPrinter.hpp"
#include "Verification/AnalyticalSolution/AnalyticalSolution.hpp"

SimulationContext::SimulationContext(SolverScheme scheme_)
    : geometry(SolverInputs::Geometry::Width, SolverInputs::Geometry::Height),
      grid(geometry, SolverInputs::Mesh::Nx, SolverInputs::Mesh::Ny),
      params(grid,geometry),
      boundaries(grid),
      T_0(grid),
      T_1(grid),
      T_nMinus1(grid),
      T_n(grid),
      T_nPlus1(grid),
      T_SteadyState(grid),
      T_Analytical_n(grid),
      scheme(scheme_),
      RelativeFieldChange(0.0),
      FinalTime(0.0),
      BreakCondition(false)
{
    T_0.SetIntitialProfile(SolverInputs::InitialCondition::InitialProfile);
    T_0.ApplyBoundaryCondition(boundaries.TopWall);
    T_0.ApplyBoundaryCondition(boundaries.BottomWall);
    T_0.ApplyBoundaryCondition(boundaries.RightWall);
    T_0.ApplyBoundaryCondition(boundaries.LeftWall);

    DiffusionEQ::AnalyticalSolution::Pass(
        T_SteadyState,
        geometry,
        grid,
        boundaries
    );
}





void SimulationContext::Common(std::size_t TimeLevel_n)
{

    ApplyBC();
    double Value = CalculateRelativeFieldChange();
    SetRelativeFieldChange(Value);
    PrintStatus(TimeLevel_n);
    UpdadeField();

    LivePlot(TimeLevel_n);
    // WriteToFileTimeLevel(TimeLevel_n);
    // PlotContourTimeLevel(TimeLevel_n);

    if(IsSteadyState(TimeLevel_n))
    {
        return;
    }

    
}
 bool SimulationContext::GetBreakCondition()
 {
    return BreakCondition;
 }

 void SimulationContext::WriteInitialOutputs()
{
    FileWriter::WriteField(
        FileName::Create(FieldName::T, 0.0),
        T_0,
        grid,
        Path::Create(OutputCategory::Initial)
    );

    FileWriter::WriteField(
        FileName::Create(FieldName::TAnalytical, Label::Steady_State),
        T_SteadyState,
        grid,
        Path::Create(OutputCategory::Analytical)
    );

    plot.ContourPlotWindow.AddContour(
        Path::Create(OutputCategory::Initial), 
        FileName::Create(FieldName::T,0.0),
          Legend::Create(FieldName::T,0.0));

    plot.ContourPlotWindow.AddContour(
        Path::Create(OutputCategory::Analytical),
        FileName::Create(FieldName::TAnalytical,Label::Steady_State),
        Legend::Create(FieldName::TAnalytical,Label::Steady_State));



}

//Private Mthods
void SimulationContext::SetRelativeFieldChange(double value)
{
    RelativeFieldChange = value;
}
void SimulationContext::SetFinalTime(double value)
{
    FinalTime = value;
}
void SimulationContext::SetBreakCondition(bool value)
{
    BreakCondition = value;
}

void SimulationContext::StartUp()
{
    if (scheme == SolverScheme::FTCS 
        || scheme == SolverScheme::Laasonen 
        || scheme == SolverScheme::CrankNicolson)
    {
            T_n.Swap(T_0);
    }
    else if (scheme == SolverScheme::DUFORT_FRANKEL)
    {
        return;
    }
}

void SimulationContext::ApplyBC()
 {
    //Apply Boundary Conditions To Field
    T_nPlus1.ApplyBoundaryCondition(boundaries.TopWall);
    T_nPlus1.ApplyBoundaryCondition(boundaries.BottomWall);
    T_nPlus1.ApplyBoundaryCondition(boundaries.RightWall);
    T_nPlus1.ApplyBoundaryCondition(boundaries.LeftWall);
 }

 double SimulationContext::CalculateRelativeFieldChange()
 {
    //Calculate The Relative Field Change from T_n to T_nPlus1
    double value = RelativeFieldChange::Return(
        T_n,
        T_nPlus1,
        params.dt.GetValue(),
        params.t_Scale.GetValue()
    );
    return value;
 }

 void SimulationContext::PrintStatus(std::size_t TimeLevel_n)
 {
        StatusPrinter::Print(
        "Relative Field Change",
        TimeLevel_n,
        RelativeFieldChange,
        params.dt.GetValue());
 }

  void SimulationContext::UpdadeField()
  {
    if (scheme == SolverScheme::FTCS || scheme == SolverScheme::Laasonen || scheme == SolverScheme::CrankNicolson)
    {     //Update Field
        T_n.Swap(T_nPlus1);
    }
    else if (scheme == SolverScheme::DUFORT_FRANKEL)
    {
        T_nMinus1.Swap(T_n);
        T_n.Swap(T_nPlus1);
    }
  }

void SimulationContext::LivePlot(std::size_t TimeLevel_n)
{
if (TimeLevel_n % 20 == 0)
    {
        plot.LiveWindow.AddPoint(
            "relative field change",
            TimeLevel_n,
            RelativeFieldChange);
    }
}

 void SimulationContext::WriteToFileTimeLevel(std::size_t TimeLevel_n)
 {
if(TimeLevel_n % 200 == 0)
    {
        FileWriter::WriteField
        (
        FileName::Create(FieldName::T,static_cast<double>(TimeLevel_n)*params.dt.GetValue()),
        T_n,
        grid,
        Path::Create(OutputCategory::Numerical, scheme)
        );

    }

 }

 void SimulationContext::PlotContourTimeLevel(std::size_t TimeLevel_n)
 {
    if (TimeLevel_n % 200 == 0)
    {
            plot.ContourPlotWindow.AddContour(
            Path::Create(OutputCategory::Numerical, scheme), 
            FileName::Create(FieldName::T,static_cast<double>(TimeLevel_n)*params.dt.GetValue()), 
            Legend::Create(FieldName::T,static_cast<double>(TimeLevel_n)*params.dt.GetValue())
        );
    }
 }

 bool SimulationContext::IsSteadyState(std::size_t TimeLevel_n)
 {
    if(RelativeFieldChange < params.Tolerance.GetValue())
    {
        StatusPrinter::Print(
            "Relative Field Change",
            TimeLevel_n,
            RelativeFieldChange,
            params.dt.GetValue(),true);

        double finaltime = static_cast<double>(TimeLevel_n) * params.dt.GetValue();
        SetFinalTime(finaltime);

        FileWriter::WriteField(
            FileName::Create(FieldName::T,FinalTime),
            T_n,
            grid,
            Path::Create(OutputCategory::Numerical, scheme)
            );

       plot.ContourPlotWindow.AddContour(
            Path::Create(OutputCategory::Numerical, scheme),
            FileName::Create(FieldName::T,FinalTime),
            Legend::Create(FieldName::T,FinalTime)
            );
       
        DiffusionEQ::AnalyticalSolution::Pass(
            T_Analytical_n,
            FinalTime,
            params.alpha.GetValue(),
            geometry,
            grid,
            boundaries,
            T_n);


        FileWriter::WriteField(
            FileName::Create(FieldName::TAnalytical,FinalTime),
            T_Analytical_n,
            grid,
            Path::Create(OutputCategory::Analytical));

        plot.ContourPlotWindow.AddContour(
            Path::Create(OutputCategory::Analytical),
            FileName::Create(FieldName::TAnalytical,FinalTime),
            Legend::Create(FieldName::TAnalytical,FinalTime)
        );
        std::cout << "Calculations Completed Successfully for " << std::string(To_String(scheme)) << '\n';
        SetBreakCondition(true);
        return true;
    }
    else
    {
        SetBreakCondition(false);
        return false;
    }
 }
