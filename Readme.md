## Detecting circle intersections in C++

An implementation of the Bentley-Ottmann sweep line algorithm 
for finding all intersections of a set of circles. For an input consisting of
n circles with k crossings, the algorithm takes time O((n+k) log n). 
In cases where k = o(n^2 / log n), this is an improvement on a naive algorithm that tests every pair of circles, and takes O(n^2).
The algorithm works like the [Bentley-Ottmann algorithm for finding segment intersections](https://en.wikipedia.org/wiki/Bentley%E2%80%93Ottmann_algorithm), 
but uses circle arcs instead of segments.

### Example usage

```
#include "intersect_circles.hpp"

Circle c1 = Circle(Point(0,0), 1); // Circle(center, radius)
Circle c2 = Circle(Point(1,1), 1);
Circle c3 = Circle(Point(1,-1), 1);
Circle c4 = Circle(Point(2,0), 1);

std::vector<Circle> circles = {c1, c2, c3, c4};
std::vector<Point> intersections = circleIntersect(circles);
```cpp
