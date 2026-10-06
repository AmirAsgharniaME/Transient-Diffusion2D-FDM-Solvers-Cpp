#pragma once
#include "Utilities/LivePointPlotter/LivePointPlotter.hpp"
#include "Core/PostProcessing/Contour.hpp"
#include "Config/PathManager.hpp"
#include "Config/FileName.hpp"
#include "Config/Legend.hpp"
#include "Config/FieldName.hpp"

struct Contours
{

    //Live Plotter Initialization
    LivePointPlotter LiveWindow;
    Contour Window;


            Contours()
            :LiveWindow(
            "Solver Convergence",
            "Iteration",
            "Rate of Field Change",
            true) //Logarithmic Y axis
            {
                LiveWindow.AddSeries("rate_of_field_change", "Rate of Field Change");


                Contour::WindowOptions options;
                options.Title = "Solution Comparison";
                options.XLabel = "x (m)";
                options.YLabel = "y (m)";
                options.ColorLabel = "Temperature";
                options.Width = 1500;
                options.Height = 800;
                options.Columns = 3;

                Window.AddContour(
                    Path::Create(OutputCategory::Initial),
                    FileName::Create(FieldName::T,0.0),
                    Legend::Create(FieldName::T,0.0));

            }


};