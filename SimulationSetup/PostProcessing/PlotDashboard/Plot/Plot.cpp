#include "SimulationSetup/PostProcessing/PlotDashboard/Plot/Plot.hpp"

Plot::Plot()
    : LiveWindow(
          "Solver Convergence",
          "Iteration",
          "Rate of Field Change",
          true), // Logarithmic Y axis
      PlotWindow(
          "Temperature Comparison",
          "y",
          "Temperature",
          false, // Linear x-axis
          false  // Linear y-axis
      ),
      ContourPlotWindow([] {
          ContourPlot::WindowOptions options;
          options.Title = "Temperature Contours";
          options.XLabel = "x";
          options.YLabel = "y";
          options.ColorLabel = "Temperature";
          return options;
      }())
{
    LiveWindow.AddSeries(
        "relative field change",
        "Relative Field Change");

}
