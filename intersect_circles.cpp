#include <queue>
#include <cmath>
#include <vector>
#include <iostream>
#include <map>
#include <cassert>
#include <algorithm>
#include <memory>
#include "intersect_circles.hpp"

/**
Return all points of intersection with another circle
If circles coincide, returns an empty vector
*/
std::vector<Point> Circle::intersect(const Circle& circle) {
    float d = center.distance(circle.center);
    if (d == 0)
    // we assume all circles are distinct
        return {};
    if ((radius + circle.radius < d) or (d < std::abs(radius - circle.radius)))
        return {};
    //distance from center to radical axis
    float a = (radius*radius + d*d - circle.radius*circle.radius)/(2*d);
    Point diff = circle.center.sub(center);
    //point on radical axis
    Point p = center.add(diff.scale(a/d));
    float h = sqrt(radius*radius - a*a);
    if (h == 0)
        return {p};
    Point n = diff.turn().scale(h/d);
    return {p.add(n), p.sub(n)};
};

std::ostream& operator<< (std::ostream& os, const Point& point) {
    os << "[" << point.x << "," << point.y << "]";
    return os;
}

bool operator< (const Point& a, const Point& b) {
    return (a.x < b.x) or ((a.x == b.x) and (a.y < b.y));
}

bool operator== (const Point& a, const Point& b) {
    return (a.x == b.x) and (a.y == b.y);
}

bool operator!= (const Point& a, const Point& b) {
    return !(a == b);
}


bool operator!= (const Circle& a, const Circle& b) {
    return (a.center != b.center) or (a.radius != b.radius);
}

std::ostream& operator<< (std::ostream& os, const Circle& circle) {
    os << circle.center << " " << circle.radius;
    return os;
}

struct Node;
/**
*Return the y-coordiante of the arc at given x-coordinate.
*If x is outside the domain of the arc, return the y-coordinate of the center.
*/
float Arc::at(float x) const {
    //y-coordinate of arc at x
    if (std::abs(x - circle.center.x) > circle.radius)
        return circle.center.y;
    float dx = x - circle.center.x;
    float dy = std::sqrt(circle.radius * circle.radius - dx * dx);
    if (upper)
        return circle.center.y + dy;
    else
        return circle.center.y - dy;
}
/**
 * Return true if the arc is above the other arc right after point p.
 * If other arc is nullptr, return true if the arc is above point p or contains it.
*/
bool Arc::above(const Point& p, const std::shared_ptr<Arc> other) {
    //
    if (other==nullptr)
        return (at(p.x) >= p.y);
    else
        return (at(p.x + 1e-3) >= other->at(p.x + 1e-3));
}
/**
Return all intersection points of two arcs
*/
std::vector<Point> Arc::intersect(const Arc& arc) {
    std::vector<Point> points = circle.intersect(arc.circle);
    points.erase(std::remove_if(points.begin(), 
                                points.end(), 
                                [&](Point p){return std::abs(at(p.x) - arc.at(p.x)) > 1e-5;}),
                 points.end());
    return points;
}

std::ostream& operator<< (std::ostream& os, const Arc& arc) {
    os << arc.circle;
    if (arc.upper) os << " upper"; else os << " lower";
    return os;
}

/* Red-Black tree*/

std::ostream& operator<< (std::ostream& os, const Node& node) {
    os << "(";
    if (node.key != nullptr)
        os << *(node.key);
    if (node.color == BLACK)
        os << " B)";
    else
        os << " R)";
    return os;
};

void swap(Node* node1, Node* node2) {
    node1->key->node = node2;
    node2->key->node = node1;
    std::swap(node1->key, node2->key);
};

RBTree::RBTree() {
    nil = new Node(nullptr, BLACK);
    root = nil;
}

Node* RBTree::lower_bound(Node* node) {
    if (node == nil)
        return nullptr;
    if (node->left != nil) {
        node = node->left;
        while (node->right != nil) node = node->right;
        return node;
    } else {
        while (node != root) {
            if (node == node->parent->left)
                node = node->parent;
            else
                return node->parent;
        }
        return nullptr;
    }
}
Node* RBTree::upper_bound(Node* node) {
    if (node == nil)
        return nullptr;
    if (node->right != nil) {
        node = node->right;
        while (node->left != nil) node = node->left;
        return node;
    } else {
        while (node != root) {
            if (node == node->parent->right)
                node = node->parent;
            else
                return node->parent;
        }
        return nullptr;
    }
}
Node* RBTree::upper_bound(Point& p, std::shared_ptr<Arc> arc) {
    return upper_bound(insertPos(p, arc));
}
Node* RBTree::lower_bound(Point& p, std::shared_ptr<Arc> arc){
    return lower_bound(insertPos(p, arc));
}
/**
Return all arcs in the tree that contain p
*/
std::vector<std::shared_ptr<Arc>> RBTree::contains(const Point& p) {
    std::vector<std::shared_ptr<Arc>> answer = {};
    Node* y = insertPos(p);
    while ((y != nil) and (y != nullptr)) {
        if (y->key->contains(p))
            answer.push_back(y->key);
        else
            if (answer.size() > 0)
                break;
        y = upper_bound(y);
    }
    return answer;
}
void RBTree::insert(std::shared_ptr<Arc> arc, Point& p) {
    Node* y = insertPos(p, arc);
    Node* node = new Node(arc, RED, nil, nil, y);
    arc->node = node;
    if (y == nil)
        root = node;
    else if (y->key->above(p, arc))
        y->left = node;
    else
        y->right = node;
    insertFixup(node);
}
void RBTree::remove(Node* node) {
    Node* y = node;
    Node* x;
    Color color = y->color;
    if (node->left == nil) {
        x = node->right;
        replaceNode(node, node->right);
    } else if (node->right == nil) {
        x = node->left;
        replaceNode(node, node->left);
    } else {
        y = upper_bound(node);
        color = y->color;
        x = y->right;
        if (y->parent == node)
            //this might is necessary when x == nil
            //because we'll call DeleteFixup(x)
            x->parent = y;
        else {
            replaceNode(y, y->right);
            y->right = node->right;
            y->right->parent = y;
        }
        replaceNode(node, y);
        y->left = node->left;
        y->left->parent = y;
        y->color = node->color;
    }
    delete node;
    if (color == BLACK)
        removeFixup(x);
}    
/**
Print subtree starting at node inorder
*/
void RBTree::print (Node* node) {
    if (node != nil) {
        print(node->left);
        std::cout << *node << "\n";
        print(node->right);
    }
} 

void RBTree::destroyRecursive(Node* node) {
    if (node) {
        destroyRecursive(node->left);
        destroyRecursive(node->right);
        delete node;
    }
}
void RBTree::leftRotate(Node* node) {
    Node* y = node->right;
    assert (y!= nil);
    node->right = y->left;
    y->left = node;
    if (node->right != nil)
        node->right->parent = node;
    y->parent = node->parent;
    if (node->parent == nil)
        root = y;
    else if (node == node->parent->left)
        node->parent->left = y;
    else
        node->parent->right = y;
    node->parent = y;
}
void RBTree::rightRotate(Node* node) {
    Node* y = node->left;
    assert (y != nil);
    node->left = y->right;
    y->right = node;
    if (node->left != nil)
        node->left->parent = node;
    y->parent = node->parent;
    if (node->parent == nil)
        root = y;
    else if (node == node->parent->right)
        node->parent->right = y;
    else
        node->parent->left = y;
    node->parent = y;
}
/**
Return the node where the arc would be inserted in the tree.
*/
Node* RBTree::insertPos(const Point& p, std::shared_ptr<Arc> arc) {
    Node* y = nil;
    Node* x = root;
    while (x != nil) {
        y = x;
        if (x->key->above(p, arc))
            x = x->left;
        else
            x = x->right;
    }
    return y;
}
void RBTree::insertFixup(Node* z) {
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            Node* uncle = z->parent->parent->right;
            if (uncle->color == RED) {
                //recolor
                uncle->color = BLACK;
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                //if z is a right child turn it into a left child
                if (z == z->parent->right) {
                    z = z->parent;
                    leftRotate(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rightRotate(z->parent->parent);
            }
        } else {
            Node* uncle = z->parent->parent->left;
            if (uncle->color == RED) {
                //recolor
                uncle->color = BLACK;
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rightRotate(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                leftRotate(z->parent->parent);
            }
        }
    }
    root->color = BLACK;
}
void RBTree::replaceNode(Node* n, Node* child) {
  child->parent = n->parent;
  if (n->parent != nil) {
      if (n == n->parent->left)
        n->parent->left = child;
      else
        n->parent->right = child;
  } else
      root = child;
}
    
void RBTree::removeFixup(Node* x) {
    while ((x != root) and (x->color == BLACK)) {
        if (x == x->parent->left) {
            Node* w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                leftRotate(x->parent);
                w = x->parent->right;
            }
            if ((w->left->color == BLACK) and (w->right->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rightRotate(w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                leftRotate(x->parent);
                x = root;
            }
        } else {
            Node* w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rightRotate(x->parent);
                w = x->parent->left;
            }
            if ((w->right->color == BLACK) and (w->left->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    leftRotate(w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rightRotate(x->parent);
                x = root;
            }
        }
    }
    x->color = BLACK;
};
/**
Print the tree level-by-level.
For inorder traversal, see RBTree::printInorder method.
*/
std::ostream& operator<< (std::ostream& os, const RBTree& tree) {
    std::queue<Node*> nodes;
    nodes.push(tree.root);
    while (!nodes.empty()) {
        Node* node = nodes.front();
        os << *node << ", \n";
        if (node != tree.nil) {
            nodes.push(node->left);
            nodes.push(node->right);
        }
        nodes.pop();
    }
    return os;
}
/**
Use sweep-line algorithm to find all intersections of circles
*/
std::vector<Point> circleIntersect(std::vector<Circle> circles) {
    //returns all intersection of circles
    std::vector<Point> intersections = {};
    std::map<Point, std::vector<std::shared_ptr<Arc>>> events;
    RBTree status = RBTree();
    for (auto circle: circles) {
        //create events
        Point begin = circle.begin();
        std::shared_ptr<Arc> arcu = std::make_shared<Arc>(circle, true);
        std::shared_ptr<Arc> arcl = std::make_shared<Arc>(circle, false);
        auto ptr = events.find(begin);
        if (ptr != events.end()) {
            ptr->second.push_back(arcu);
            ptr->second.push_back(arcl);
        } else {
            events[begin] = {arcu, arcl};
        };

        Point end = circle.end();
        if (events.find(end) == events.end())
            events[end] = {};
    }
    while (!events.empty()) {
        auto event = events.begin();
        Point p = event->first;
        //find all arcs in status that contain p
        std::vector<std::shared_ptr<Arc>> contains = status.contains(p);
        //if there are two different circles passing through p, mark as an intersection
        if (event->second.size() > 2) {
            intersections.push_back(p);
        } else if (contains.size() > 2) {
            intersections.push_back(p);
        } else if (event->second.size() > 0) {
            Circle circle = event->second[0]->circle;
            for (auto arc : contains) 
                if (arc->circle != circle) {
                    intersections.push_back(p);
                }
        }
        //delete arcs from status
        for (auto arc : contains)
            status.remove(arc->node);
        //reinsert nonend arcs segments into status
        std::vector<std::shared_ptr<Arc>> uandc = {};
        //vector of upper end arcs and passing arcs
        for (auto arc : contains)
            if (p != arc->circle.end()) {
                status.insert(arc, p);
                uandc.push_back(arc);
            }
        for (auto arc : event->second) {
            status.insert(arc, p);
            uandc.push_back(arc);
        }
        if (uandc.size() == 0) {
            //endpoint
            Node* lb = status.lower_bound(p, nullptr);
            Node* ub = status.upper_bound(p, nullptr);
            if ((lb != nullptr) and (ub != nullptr)) {
                for (auto point : lb->key->intersect(*(ub->key)))
                    if ((events.find(point) == events.end()) and (p < point)) {
                        intersections.push_back(point);
                        events[point] = {};
                    }
            }
        } else {
            //find the extreme arcs of u and c
            std::shared_ptr<Arc> min_arc = *min_element(uandc.begin(), uandc.end(), [&](std::shared_ptr<Arc> a, 
                std::shared_ptr<Arc> b){return b->above(p, a);});
            std::shared_ptr<Arc> max_arc = *max_element(uandc.begin(), uandc.end(), [&](std::shared_ptr<Arc> a,
                std::shared_ptr<Arc> b){return b->above(p, a);});
            Node* lb = status.lower_bound(min_arc->node);
            Node* ub = status.upper_bound(max_arc->node);
            if (lb != nullptr)
                for (auto point : lb->key->intersect(*min_arc))
                    if ((events.find(point) == events.end()) and (p < point)) {
                        intersections.push_back(point);
                        events[point] = {};
                    }
            if (ub != nullptr)
                for (auto point : ub->key->intersect(*max_arc))
                    if ((events.find(point) == events.end()) and (p < point)) {
                        events[point] = {};
                        intersections.push_back(point);
                    }
        }
    events.erase(event);
    }
    return intersections;
};