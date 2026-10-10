#pragma once

#include <cstdio>
#include <map>
#include <string>
#include <vector>

class LivePointPlotter {
public:
    explicit LivePointPlotter(
        std::string plotTitle = "Convergence History",
        std::string xLabel = "Iteration",
        std::string yLabel = "Value",
        bool logarithmicY = true);

    ~LivePointPlotter();

    LivePointPlotter(const LivePointPlotter&) = delete;
    LivePointPlotter& operator=(const LivePointPlotter&) = delete;

    // شناسهٔ سری و متن legend را تعریف می‌کند.
    // اگر legend خالی باشد، شناسه به‌عنوان متن legend نمایش داده می‌شود.
    bool AddSeries(
        const std::string& seriesId,
        const std::string& legend = "");

    // یک نقطه به سری اضافه می‌کند و نمودار را به‌روز می‌کند.
    bool AddPoint(
        const std::string& seriesId,
        double iteration,
        double value);

    // چند نقطه را اضافه می‌کند و فقط یک‌بار نمودار را به‌روز می‌کند.
    bool AddPoints(
        double iteration,
        const std::map<std::string, double>& values);

    // رسم مجدد تمام داده‌های ثبت‌شده.
    bool UpdatePlot();

    void Close() noexcept;

private:
    struct Point {
        double x;
        double y;
    };

    struct Series {
        std::string id;
        std::string legend;
        std::vector<Point> points;
    };

    bool Initialize();

    std::string plotTitle_;
    std::string xLabel_;
    std::string yLabel_;
    bool logarithmicY_;
    std::vector<Series> series_;
    FILE* gnuplotPipe_ = nullptr;
};
