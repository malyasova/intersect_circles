#pragma once
#include <memory>

class Point {
public:
    Point (float x, float y): x(x), y(y) {};
    float distance (const Point& p) const {
        return sqrt((x-p.x)*(x-p.x) + (y-p.y)*(y-p.y));
    }
    Point add (const Point& p) const {
        return Point(x+p.x, y+p.y);
    }
    Point sub (const Point& p) const {
        return Point(x-p.x, y-p.y);
    }
    Point scale (const float s) const {
        return Point(s*x, s*y);
    }
    Point turn() const {
        return Point(-y, x);
    }
    float x,y;
};

std::ostream& operator<< (std::ostream& os, const Point& point);

bool operator< (const Point& a, const Point& b);

bool operator!= (const Point& a, const Point& b);
bool operator== (const Point& a, const Point& b);

class Circle {
public:
    Circle (const Point& center, float r): center(center), radius(r) {};
    Point begin() const {
    	return center.sub(Point(radius, 0));
    }
    Point end() const {
    	return center.add(Point(radius, 0));
    }
    std::vector<Point> intersect(const Circle& circle);
    Point center;
    float radius;
};


bool operator!= (const Circle& a, const Circle& b);

std::ostream& operator<< (std::ostream& os, const Circle& circle);

struct Node;
class Arc {
public:
    Arc(Circle circle, bool upper): circle(circle), upper(upper) {};
    float at(float x) const;
    bool contains(const Point& p) {
    	return std::abs(at(p.x) - p.y) < 1e-5;
    }
    bool above(const Point& p, const std::shared_ptr<Arc> other=nullptr);
    std::vector<Point> intersect(const Arc& arc) ;
    Circle circle;
    bool upper;
    Node* node=nullptr;
};

std::ostream& operator<< (std::ostream& os, const Arc& arc) ;

enum Color {RED, BLACK};

struct Node {
  Node(std::shared_ptr<Arc> key, const Color& color):key(key),color(color){};
  Node(std::shared_ptr<Arc> key, const Color& color, Node* left, Node* right, Node* parent):
  key(key),color(color),left(left),right(right),parent(parent) {};
  std::shared_ptr<Arc> key;
  Color color;
  Node* left=nullptr;
  Node* right=nullptr;
  Node* parent=nullptr;
};

std::ostream& operator<< (std::ostream& os, const Node& node) ;
void swap(Node* node1, Node* node2) ;


class RBTree {
public:
    RBTree() ;
    ~RBTree(){destroyRecursive(root);}
    Node* lower_bound(Node* node);
    Node* upper_bound(Node* node);
    Node* upper_bound(Point& p, std::shared_ptr<Arc> arc=nullptr);
    Node* lower_bound(Point& p, std::shared_ptr<Arc> arc=nullptr);
    std::vector<std::shared_ptr<Arc>> contains(const Point& p);
    void insert(std::shared_ptr<Arc> arc, Point& p);
    void remove(Node* node);
    /**
    Inorder traversal of the tree.
    */
    void printInorder(){print(root);}
    void print (Node* node);
    /**
    Print the tree level-by-level
    */
    friend std::ostream& operator<< (std::ostream& os, const RBTree& tree);
private:
    void destroyRecursive(Node* node);
	void leftRotate(Node* node);
    void rightRotate(Node* node);
    Node* insertPos(const Point& p, std::shared_ptr<Arc> arc=nullptr);
    void insertFixup(Node* z);
    void replaceNode(Node* n, Node* child);
    void removeFixup(Node* x);
    Node* nil;
    Node* root;
};

std::vector<Point> circleIntersect(std::vector<Circle> circles) ;