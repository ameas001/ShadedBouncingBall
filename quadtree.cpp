// Name: Aranya Meas
// Quarter, Year: Spring, 2014
// Lab: 2
//
// This file is to be modified by the student.
// quadtree.cpp
////////////////////////////////////////////////////////////
#include "quadtree.h"
#include "math.h"

QuadTreeNode::QuadTreeNode(const Rect2D & space)
	: nodeType(QUAD_TREE_LEAF), space(space), objects(), topLeft(NULL), topRight(NULL), bottomLeft(NULL), bottomRight(NULL)
{}

QuadTreeNode * QuadTreeNode::findQuadrant(const Point2D & p) const
{
	if(p.x <= space.center().x && p.y >= space.center().y)
		return bottomLeft;
	else if(p.x > space.center().x && p.y >= space.center().y)
		return bottomRight;
	else if(p.x <= space.center().x && p.y < space.center().y)
		return topLeft;
	else if(p.x > space.center().x && p.y < space.center().y)
		return topRight;
	else return NULL;
}

std::vector <Line2D> QuadTreeNode::splitLine(const Line2D & val) const
{
	double gap = 0.01;
	int quad1 = findQuadrantNumber(val.p1);
	int quad2 = findQuadrantNumber(val.p2);
	std::vector <Line2D> line;
	
	if(((quad1 == 1 || quad2 == 1) || (quad1 == 3 || quad2 == 3)) && ((quad1 == 2 || quad2 == 2) || (quad1 == 4 || quad2 == 4)))
	{
		double left, right;
		if(quad1 == 1 || quad1 == 3)
		{
			left = val.p1.x;
			right = val.p2.x;
		}
		else
		{
			left = val.p2.x;
			right = val.p1.x;
		}
		line.push_back(Line2D(Point2D(left, val.p1.y), Point2D(space.center().x - gap, val.p1.y)));
		line.push_back(Line2D(Point2D(space.center().x + gap, val.p1.y), Point2D(right, val.p1.y)));
	}
	else
	{
		double top, bottom;
		if(quad1 == 1 || quad1 == 2)
		{
			top = val.p1.y;
			bottom = val.p2.y;
		}
		else
		{
			top = val.p2.y;
			bottom = val.p1.y;
		}
		line.push_back(Line2D(Point2D(val.p1.x, top), Point2D(val.p1.x, space.center().y + gap)));
		line.push_back(Line2D(Point2D(val.p1.x, bottom), Point2D(val.p1.x, space.center().y - gap)));
	}
	return line;
}

int QuadTreeNode::findQuadrantNumber(const Point2D & p) const
{
	if(p.x > space.center().x && p.y >= space.center().y)
		return 1;
	else if(p.x <= space.center().x && p.y >= space.center().y)
		return 2;
	else if(p.x <= space.center().x && p.y < space.center().y)
		return 3;
	else if(p.x > space.center().x && p.y < space.center().y)
		return 4;
	else return 0;
}

void QuadTreeNode::insert(const Line2D & value, int currentDepth, int depthLimit, int listLimit)
{
	if (nodeType == QUAD_TREE_PARENT)
	{
		QuadTreeNode * quad1 = findQuadrant(value.p1);
		QuadTreeNode * quad2 = findQuadrant(value.p2);
		
		if(quad1 == quad2)
			quad1->insert(value, currentDepth, depthLimit, listLimit);
		else
		{
			std::vector <Line2D> line = splitLine(value);
			for(int i = 0; i < line.size(); ++i)
				insert(line[i], currentDepth, depthLimit, listLimit);
		}
	}
	else if(nodeType == QUAD_TREE_LEAF)
	{
		if(objects.size() != listLimit)
			objects.push_back(value);
		else
		{
			nodeType = QUAD_TREE_PARENT;
			double tempx = space.p.x;
			double tempy = space.p.y;
			topLeft = new QuadTreeNode(Rect2D(Point2D(tempx, tempy), space.width / 2, space.height / 2));
			topRight = new QuadTreeNode(Rect2D(Point2D(space.center().x, tempy), space.width / 2, space.height / 2));
			bottomLeft = new QuadTreeNode(Rect2D(Point2D(tempx, space.center().y), space.width / 2, space.height / 2));
			bottomRight = new QuadTreeNode(Rect2D(Point2D(space.center().x, space.center().y), space.width / 2, space.height / 2));
			insert(objects[0], currentDepth, depthLimit, listLimit);
			insert(value, currentDepth, depthLimit, listLimit);
		}
	}
}

void QuadTreeNode::query(const Point2D & p, std::vector<Line2D> & ret) const
{
	Point2D temp = p;
	if (nodeType == QUAD_TREE_PARENT)
	{
		QuadTreeNode * temp2 = findQuadrant(temp);
		temp2->query(p, ret);
	}
	else if (nodeType == QUAD_TREE_LEAF)
	{
		space.render(GRAY, true);
		ret = objects;
	}
}

//Recursively deletes the QuadTree
void QuadTreeNode::dealloc()
{
	if (topLeft)
	{
		topLeft->dealloc();
		delete topLeft;
	}
	if (topRight)
	{
		topRight->dealloc();
		delete topRight;
	}
	if (bottomLeft)
	{
		bottomLeft->dealloc();
		delete bottomLeft;
	}
	if (bottomRight)
	{
		bottomRight->dealloc();
		delete bottomRight;
	}
}

void QuadTreeNode::render() const
{
	if (nodeType == QUAD_TREE_PARENT)
	{
		topRight->space.render(RED);
		topLeft->space.render(RED);
		bottomLeft->space.render(RED);
		bottomRight->space.render(RED);

		topRight->render();
		topLeft->render();
		bottomLeft->render();
		bottomRight->render();
	}
	else if(nodeType == QUAD_TREE_LEAF)
	{
		for(int i = 0; i < objects.size(); ++i)
			objects[i].render(BLACK);
	}
}

QuadTree::QuadTree(const Rect2D & space, int dlim, int llim)
	: root(new QuadTreeNode(space)), depthLimit(dlim), listLimit(llim)
{}

QuadTree::~QuadTree()
{
	root->dealloc();
}

void QuadTree::insert(const Line2D & value)
{
	root->insert(value, 1, depthLimit, listLimit);
}

std::vector<Line2D> QuadTree::query(const Point2D & p) const
{
	std::vector<Line2D> ret;
	root->query(p, ret);
	return ret;
}

std::vector <Line2D> QuadTree::query(const Point2D & p, const Circle2D c)
{
	std::vector <Point2D> points;
	std::vector <Line2D> ret;
	std::vector <Line2D> ret_temp;
	
	points.push_back(Point2D(c.p.x + c.radius,c.p.y + c.radius));
	points.push_back(Point2D(c.p.x - c.radius,c.p.y + c.radius));
	points.push_back(Point2D(c.p.x + c.radius,c.p.y - c.radius));
	points.push_back(Point2D(c.p.x - c.radius,c.p.y - c.radius));
	
	for(int i = 0; i < points.size(); ++i)
	{
		ret_temp = query(points[i]);
		for(int j = 0; j < ret_temp.size(); ++j)
			ret.push_back(ret_temp[j]);
	}
	return ret;
}

void QuadTree::render() const
{
	root->render();
}
