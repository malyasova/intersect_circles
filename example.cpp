
#include "intersect_circles.hpp"

int main() {
    Circle c1 = Circle(Point(0,0), 1);
    Circle c2 = Circle(Point(1,1), 1);
    Circle c3 = Circle(Point(1,-1), 1);
    Circle c4 = Circle(Point(2,0), 1);

    std::vector<Circle> circles = {c1, c2, c3, c4};
    std::vector<Point> intersections = circleIntersect(circles);
    std::cout << "intersections\n";
    for (auto p : intersections)
        std::cout << p << std::endl;
        
    return 0;
}
