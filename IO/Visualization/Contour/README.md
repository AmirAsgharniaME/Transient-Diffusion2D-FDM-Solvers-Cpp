# Contour Class

Each `Contour` object owns an independent gnuplot session and a comparison window. The constructor accepts window settings; the first `AddContour` call displays the initial plot. Each subsequent call adds a contour in a separate panel within the same window and redraws all panels. The number of contours is not limited to three.

## Usage Example

```cpp
#include "Core/PostProcessing/Contour.hpp"

int main()
{
    Contour::WindowOptions options;
    options.Title = "Diffusion: numerical vs analytical";
    options.Width = 1500;
    options.Height = 800;
    options.XLabel = "x (m)";
    options.YLabel = "y (m)";
    options.ColorLabel = "Temperature";
    options.Columns = 2;
    // Optional: a fixed color range shared by all panels.
    options.ColorRange = std::make_pair(0.0, 100.0);
    // Optional: specify the project root if automatic discovery fails.
    // options.ProjectRoot = "/absolute/path/to/project";

    Contour comparison(options);
    comparison.AddContour("Results/Numerical_Field/FTCS",
                          "U.dat", "FTCS");
    comparison.AddContour("Results/Analytical_Field",
                          "U_Analytical.dat", "Analytical");

    return 0;
}
```

To use the default settings, pass only a window title:

```cpp
Contour comparison("Solution comparison");
comparison.AddContour("Results/Initial_Field", "U.dat", "Initial field");
```

## Inputs and Behavior

- The first `AddContour` argument is a directory relative to the **project root**. Use `"."` for a file located directly in the root.
- The second argument is the filename only. The third argument is the `legend`, displayed above its contour panel.
- Each data file contains three numeric columns in the order `x y value`, separated by spaces or tabs. Comments starting with `#` and blank lines are allowed.
- Coordinates must form a complete rectangular grid with at least two nodes in each direction, matching the grids used by `Mesh`. Node spacing may be nonuniform. File rows may appear in any order; blank lines between grid rows are not required.
- Grid dimensions are extracted from the file. The constructor does not depend on `Mesh` or `Field`, and different files may have different grid dimensions.
- Data is read when `AddContour` is called, and the coordinates and field values are retained for plotting. To add an updated version of a file, call `AddContour` again.
- All panels share the same spatial axis ranges and color range. Without an explicit `ColorRange`, the color range is calculated from all added fields. Constant fields also receive a valid color range.
- `Columns = 0` selects an automatic layout. A positive value specifies the desired number of columns.
- Missing files, invalid data, duplicate or incomplete coordinates, and invalid settings are reported through exceptions. Adding invalid data does not remove existing contours.
- The project root is discovered by searching the current working directory and its parents for the `Core` and `Config` directories. The source location is also checked if its path was absolute at compile time. Set `ProjectRoot` explicitly when running from another location if automatic discovery fails.
- Copying and moving objects are disabled; each object owns its session. Destroying the object closes the session, while `-persist` keeps the plotted window visible.

## Building

The class requires C++17 and gnuplot with the `qt` terminal. Compile and link `Contour.cpp` with your program. For the example above saved as `example.cpp`, run the following command from the project root:

```bash
g++ -std=c++17 -Wall -Wextra -I. example.cpp \
    Core/PostProcessing/Contour.cpp -o /tmp/contour-example
```

The current Makefile defines `POSTPROCESSING_SRC`, but its entry in `CORE_SRCS` is commented out. Add that source to the files being compiled and linked for any program that uses this class.
