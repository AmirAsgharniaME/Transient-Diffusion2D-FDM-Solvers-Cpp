#pragma once
#include "IO/Visualization/gnuplot/LinePlot/LinePlot.hpp"
#include "IO/Visualization/gnuplot/LivePointPlotter/LivePointPlotter.hpp"
#include "IO/Visualization/gnuplot/ContourPlot/ContourPlot.hpp"

class Plot
{
public:
    LivePointPlotter LiveWindow;
    LinePlot PlotWindow;
    ContourPlot  ContourPlotWindow;

    explicit Plot();
};
