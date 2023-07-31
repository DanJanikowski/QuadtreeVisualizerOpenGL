#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <glad/glad.h>

#include "renderObject.h"


struct EntityData {
	int dataIndex;
	EntityData* next;

	EntityData() : dataIndex(-1), next(nullptr) {};
	EntityData(int dataIndex_) : dataIndex(dataIndex_), next(nullptr) {};
	~EntityData() {
		next = nullptr;
	};
};

struct TreeNode {
	bool isLeaf;
	unsigned int numEntities;

	EntityData* head;
	TreeNode* children[4] = { nullptr, nullptr, nullptr, nullptr };

	TreeNode() : isLeaf(true), numEntities(0), head(nullptr) {};
	~TreeNode() {
		head = nullptr;
		children[0] = nullptr;
		children[1] = nullptr;
		children[2] = nullptr;
		children[3] = nullptr;
	};
};

struct QuadTree {
	std::vector<EntityData*> entities;
	std::vector<TreeNode*> nodes;

	QuadTree();
	~QuadTree();

	void setup(unsigned int numToAdd);
	void addPoint(float x_, float y_, float vx_, float vy_);

	void insert(EntityData* ed);
	void updatePoints();
	void updatePointsHelper(TreeNode* curNode, float bx, float by, float bw, float bh);
	void cleanup();

	void drawEntities();
	void packEntities(bool repack);

	void drawQuadTree();
	void packQuadTree(bool repack);
	void packQuadTreeHelper(TreeNode* curNode, float bx, float by, float bw, float bh, unsigned char quadrant, GLuint preservedIndex);

	void print1();
	int print2(TreeNode* node, std::string path);

private:
	const int maxDepth = 8;
	const int maxCapacity = 128;

	std::vector<EntityData*> toReinsert;

	RenderObject *entityRO, *quadTreeRO;

	// For drawing:
	// Points
	std::vector<GLfloat> entityPos;
	std::vector<GLfloat> entityVel;

	// QuadTree grid
	std::vector<GLfloat> treeVerts;
	std::vector<GLuint> treeInds;

};