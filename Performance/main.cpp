//
// Author: Yannick Huggler
//

#include <random>
#include <algorithm>
#include <iostream>
#include "RangeQuery.h"

using namespace std;

int main() {
    Stopwatch stopwatch;
    default_random_engine engine;

    uniform_real_distribution<double> coordsRange(-1000, 1000);
    uniform_real_distribution<double> deltaRange(100, 200);

    constexpr size_t numOfPoints = 50000;

    vector<Point3> points(numOfPoints);

    for (size_t i = 0; i < numOfPoints; i++) {
        points[i] = Point3({coordsRange(engine), coordsRange(engine), coordsRange(engine)});
    }

    cout << "Starting the performance test for a trivial and efficient implementation for a range query." << endl << endl;

    stopwatch.start();
    RangeQuery<Point3> rangeQuery(points);
    stopwatch.stop();

    cout << "The instantiation of the RangeQuery class took " << stopwatch.getElapsedTimeSeconds() << " seconds."
         << endl << endl;

    double elapsedTimeTrivial = 0;
    double elapsedTimeEfficient = 0;

    for (int i = 0; i < 25000; i++) {
        auto p1x = coordsRange(engine);
        auto p1y = coordsRange(engine);
        auto p1z = coordsRange(engine);

        auto p2x = p1x + deltaRange(engine);
        auto p2y = p1y + deltaRange(engine);
        auto p2z = p1z + deltaRange(engine);

        auto[trivial, efficient] = rangeQuery.performance(Point3({p1x, p1y, p1z}), Point3({p2x, p2y, p2z}));

        elapsedTimeTrivial += trivial;
        elapsedTimeEfficient += efficient;
    }

    cout << "The trivial implementation of the rangeQuery took " << elapsedTimeTrivial << " seconds." << endl;
    cout << "The efficient implementation of the rangeQuery took " << elapsedTimeEfficient << " seconds." << endl
         << endl;

    cout << "The efficient implementation was roughly " << floor(elapsedTimeTrivial / elapsedTimeEfficient)
         << " times faster than the trivial implementation." << endl << endl;

    cout << "Performance test is finished" << endl;
}

