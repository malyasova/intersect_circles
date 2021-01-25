#define BOOST_TEST_MODULE MyTest
#include <boost/test/included/unit_test.hpp>
#include "intersect_circles.hpp"

std::ostream& operator<< (std::ostream& os, std::vector<Point> v) {
	os << "{";
	for (auto point : v)
		os << point << ", ";
	os << "}";
	return os; 
}
BOOST_AUTO_TEST_CASE( Arc_contains_test )
{
	Circle c = Circle(Point(0,0), 2);
	Point p1 = Point(0,2);
	Point p2 = Point(2,0);
	Arc arcu = Arc(c, true);
	Arc arcl = Arc(c, false);

    BOOST_CHECK(arcu.contains(p1));      
    BOOST_CHECK(arcu.contains(p2));
    BOOST_CHECK(arcl.contains(p2));
    BOOST_CHECK(!arcl.contains(p1));
}

BOOST_AUTO_TEST_CASE ( intersect_test )
{
    Circle c1 = Circle(Point(0,0), 1);
    Circle c2 = Circle(Point(2,0), 1);
    Circle c3 = Circle(Point(3,1), 1);
    Circle c4 = Circle(Point(0,0), 100);
    //tangent circles
    std::vector<Point> answer = {Point(1,0)};
    BOOST_CHECK_EQUAL(c1.intersect(c2), answer);
    //intersecting circles
    answer = {Point(2,1), Point(3,0)};
    BOOST_CHECK_EQUAL(c2.intersect(c3), answer);
    //nonintersecting circles
    answer = {};
    BOOST_CHECK_EQUAL(c1.intersect(c3), answer);
    //one circle inside another
    BOOST_CHECK_EQUAL(c1.intersect(c4), answer);
    //arc intersection: 
    //upper arc does not intersect lower arc from the same circle
    Arc arc1u = Arc(c1, true);
    Arc arc1l = Arc(c1, false);
    BOOST_CHECK_EQUAL(arc1u.intersect(arc1l), answer);
    //tangent arcs
    Circle c5 = Circle(Point(0,2), 1);
    Arc arc5l = Arc(c5, false);
    answer = {Point(0,1)};
    BOOST_CHECK_EQUAL(arc1u.intersect(arc5l), answer);
}

BOOST_AUTO_TEST_CASE ( above_test )
{
	Circle c1 = Circle(Point(1,-1), 1);
    Circle c2 = Circle(Point(1,1), 1);
    Arc arc1u = Arc(c1, true);
    Arc arc1l = Arc(c1, false);
    Arc arc2u = Arc(c2, true);
    Arc arc2l = Arc(c2, false);
    //upper arc above lower arc of same circle
    BOOST_CHECK(arc1u.above(c1.begin(), std::make_shared<Arc>(arc1l)));
    BOOST_CHECK(arc1u.above(c1.end(), std::make_shared<Arc>(arc1l)));

    BOOST_CHECK(arc2l.above(Point(1,0), std::make_shared<Arc>(arc1u)));
    BOOST_CHECK(!arc1u.above(Point(1,0), std::make_shared<Arc>(arc2l)));
}

