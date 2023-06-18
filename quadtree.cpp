
#include "quadtree.h"


void QuadTree::print1() {
	putchar('\n');
	printf("Num positions: %i\n", entityPos.size());
	printf("Num velocities: %i\n", entityVel.size());
	for (int i = 0; i < entityPos.size() / 2; i++) {
		printf("Entity %i: p(%.3lf, %.3lf)   v(%.3lf, %.3lf)\n", i + 1, 
			entityPos[i * 2], entityPos[i * 2 + 1], entityVel[i * 2], entityVel[i * 2 + 1]);
	}
	putchar('\n');
	printf("Num entities: %i\n", entities.size());
	for (int i = 0; i < entities.size(); i++) {
		printf("Entity %i %p: p(%.3lf, %.3lf)   v(%.3lf, %.3lf)\n", i + 1, (void const*)entities[i],
			entityPos[entities[i]->dataIndex], entityPos[entities[i]->dataIndex + 1], 
			entityVel[entities[i]->dataIndex], entityVel[entities[i]->dataIndex + 1]);
	}
	putchar('\n');
	printf("Num nodes: %i\n", nodes.size());
	print2(nodes[0], "ROOT");
	putchar('\n');
}

int QuadTree::print2(TreeNode* node, std::string path) {
	int totalEntities = 0;
	std::string contents = "";
	if (!node->isLeaf) {
		totalEntities += print2(node->children[0], path + "->BL");
		totalEntities += print2(node->children[1], path + "->BR");
		totalEntities += print2(node->children[2], path + "->TL");
		totalEntities += print2(node->children[3], path + "->TR");
	}
	else {
		EntityData* curEntity = node->head;
		totalEntities += 0;
		while (curEntity != nullptr) {
			std::ostringstream address;
			address << (void const*)curEntity;
			std::string add = address.str();
			contents += "\t" + add + "   " + "p(" + std::to_string(entityPos[curEntity->dataIndex]) + ", " +
				std::to_string(entityPos[curEntity->dataIndex + 1]) + ")   v(" +
				std::to_string(entityVel[curEntity->dataIndex]) + ", " +
				std::to_string(entityVel[curEntity->dataIndex + 1]) + ")\n";
			curEntity = curEntity->next;
			totalEntities++;
		}
	}
	std::cout << path << " " << node << "\n\t" << ((node->isLeaf) ? "LEAF" : "INTERNAL") << " " << node->numEntities << " " << totalEntities << " " <<
		((node->head == nullptr) ? "NULL" : "NOT NULL") << std::endl;
	std::cout << contents;

	return totalEntities;
}





QuadTree::QuadTree() {
	entityRO = new RenderObject(false);
	quadTreeRO = new RenderObject(true);

	nodes.push_back(new TreeNode());
}
QuadTree::~QuadTree() {
	delete entityRO;
	delete quadTreeRO;

	for (EntityData* ed : entities) if (ed) delete ed;
	for (TreeNode* tn : nodes) if (tn) delete tn;
}

void QuadTree::setup(unsigned int numToAdd) {
	const GLfloat dvel = 0.001;
	for (int i = 0; i < numToAdd; i++) {
		addPoint((rand() / (float)RAND_MAX) * 2.0 - 1.0, (rand() / (float)RAND_MAX) * 2.0 - 1.0,
			((rand() / (float)RAND_MAX) * 2.0 - 1.0) * dvel, ((rand() / (float)RAND_MAX) * 2.0 - 1.0) * dvel);
	}
	//for (int i = 0; i < 9; i++) {
	//	addPoint(-0.99, -0.99, 0.00001, 0.00001);
	//}
}
void QuadTree::addPoint(float x_, float y_, float vx_, float vy_) {
	entityPos.push_back(x_);
	entityPos.push_back(y_);
	entityVel.push_back(vx_);
	entityVel.push_back(vy_);

	// THE INDEX OF THE ENTITY IS THE RELATIVE INDEX IN THE POSITIONS VECTOR SO
	// # OF ENTITIES * 2 to point at the x coordinate
	EntityData* newEntity = new EntityData(entityPos.size() - 2);
	entities.push_back(newEntity);

	insert(newEntity);
}


void QuadTree::insert(EntityData* newEntity) {
	TreeNode* curNode = nodes[0];

	// Bounding box center and half width/height
	float bx = 0, by = 0, bw = 1, bh = 1;

	// First find the leaf
	for (int depth = 1; depth <= maxDepth; depth++) {
		// Node is not a leaf
		if (!curNode->isLeaf) {
			float x = entityPos[newEntity->dataIndex];
			float y = entityPos[newEntity->dataIndex + 1];

			// Figure out which child to add the entity to
			bw /= 2.0;
			bh /= 2.0;
			if (y < by) {
				if (x < bx) {	// BL
					curNode = curNode->children[0];
					bx -= bw;
					by -= bh;
				}
				else {			// BR
					curNode = curNode->children[1];
					bx += bw;
					by -= bh;
				}
			}
			else {
				if (x < bx) {	// TL
					curNode = curNode->children[2];
					bx -= bw;
					by += bh;
				}
				else {			// TR
					curNode = curNode->children[3];
					bx += bw;
					by += bh;
				}
			}
		}
		// Node is a leaf
		else {
			if (curNode->numEntities == 0) { // If node is empty then set as head
				curNode->head = newEntity;
				curNode->numEntities++;
			}
			else if (curNode->numEntities < maxCapacity || depth == maxDepth) { // If its below capacity or depth = maxDepth then append it
				EntityData* tempEntity = curNode->head;
				while (tempEntity->next != nullptr) {
					if (tempEntity == newEntity) {
						std::cout << "POINTERS EQUAL: " << ((void const*)tempEntity) << " == " << ((void const*)newEntity) << std::endl;
						return;
					}
					tempEntity = tempEntity->next;
				}
				tempEntity->next = newEntity;
				curNode->numEntities++;
			}
			else if (curNode->numEntities == maxCapacity) { // If node is at capacity then split
				// Set this node to not a leaf
				curNode->isLeaf = false;
				// Create children
				curNode->children[0] = new TreeNode();
				curNode->children[1] = new TreeNode();
				curNode->children[2] = new TreeNode();
				curNode->children[3] = new TreeNode();
				nodes.push_back(curNode->children[0]);
				nodes.push_back(curNode->children[1]);
				nodes.push_back(curNode->children[2]);
				nodes.push_back(curNode->children[3]);
				// Remove each node, set next to nullptr, insert
				EntityData* temp1 = curNode->head;
				EntityData* temp2 = curNode->head->next;
				while (temp1 != nullptr) {
					temp1->next = nullptr;
					insert(temp1);
					temp1 = temp2;
					if (temp2 != nullptr) temp2 = temp2->next;
				}
				// Set curNode head to null and numEntities to 0
				curNode->head = nullptr;
				curNode->numEntities = 0;
				insert(newEntity);
			}
			else { // ERROR
				std::cout << "ERROR: Node has more than maxCapacity entities" << std::endl;
				exit(1);
			}
			return;
		}
	}
}

// Much faster ~ 229 ms at 1mil entities
void QuadTree::updatePoints() {
	updatePointsHelper(nodes[0], 0, 0, 1, 1);

	for (EntityData* nd : toReinsert) {
		insert(nd);
	}
	toReinsert.clear();
}
void QuadTree::updatePointsHelper(TreeNode* curNode, float bx, float by, float bw, float bh) {
	// Recursively descend down the tree to a leaf
	// Move the point
	// Check if the point leaves the current box
	//	- If it does, remove it and add to insert vector
	//		- Use a 'toInsert' vector so we don't add in points to nodes we havent traversed yet
	//	- Otherwise do nothing
	if (!curNode->isLeaf) {
		bw /= 2.0;
		bh /= 2.0;
		updatePointsHelper(curNode->children[0], bx - bw, by - bh, bw, bh);	// BL
		updatePointsHelper(curNode->children[1], bx + bw, by - bh, bw, bh);	// BR
		updatePointsHelper(curNode->children[2], bx - bw, by + bh, bw, bh);	// TL
		updatePointsHelper(curNode->children[3], bx + bw, by + bh, bw, bh);	// TR
	}
	if (curNode->numEntities > 0) {
		GLfloat* x, * y, * vx, * vy;
		float dx, dy;

		bool firstIsSet = false;	// Keep track if this nodes head is set
		EntityData* temp1 = curNode->head;	// Keep track of the last kept entity to point to the next kept
		EntityData* temp2 = curNode->head;
		EntityData* temp3 = curNode->head->next;
		while (temp2 != nullptr) {
			// Do position update
			x = &entityPos[temp2->dataIndex];
			y = &entityPos[temp2->dataIndex + 1];
			vx = &entityVel[temp2->dataIndex];
			vy = &entityVel[temp2->dataIndex + 1];

			*x += *vx;
			*y += *vy;
			if (*x > 1) {
				*x = -(*x - 1) + 1;
				*vx *= -1.0;
			}
			if (*x < -1) {
				*x = -(*x + 1) - 1;
				*vx *= -1.0;
			}
			if (*y > 1) {
				*y = -(*y - 1) + 1;
				*vy *= -1.0;
			}
			if (*y < -1) {
				*y = -(*y + 1) - 1;
				*vy *= -1.0;
			}

			// Check boundary conditions
			dx = *x - bx;
			dy = *y - by;
			if (dx < -bw || dx >= bw || dy < -bh || dy >= bh) {
				// Remove from the tree and add to insert list
				temp2->next = nullptr;
				toReinsert.push_back(temp2);
				temp2 = temp3;
				if (!firstIsSet) temp1 = temp2;
				if (temp3 != nullptr) temp3 = temp3->next;
				curNode->numEntities--;
			}
			else {
				if (temp1 != temp2) {
					temp1->next = temp2;
				}
				temp1 = temp2;
				if (!firstIsSet) {
					curNode->head = temp1;
					firstIsSet = true;
				}
				temp2 = temp3;
				if (temp3 != nullptr) temp3 = temp3->next;
			}
		}
		if (!firstIsSet) curNode->head = nullptr;
		if (temp1 != nullptr) temp1->next = nullptr;
	}
}

void QuadTree::cleanup() {
	// For every node check if all the children are leafs
	// If their total occupancies sum to <= maxCapacity then remove the contents
	//	add the children to be removed
	std::vector<TreeNode*> removedNodes;
	std::vector<TreeNode*> newNodeVec;
	for (TreeNode* tn : nodes) {
		if (!tn->isLeaf && tn->children[0]->isLeaf && tn->children[1]->isLeaf &&
			tn->children[2]->isLeaf && tn->children[3]->isLeaf) {

			unsigned int numEntities = tn->children[0]->numEntities + tn->children[1]->numEntities + tn->children[2]->numEntities + tn->children[3]->numEntities;

			if (numEntities == 0) {
				removedNodes.push_back(tn->children[0]);
				removedNodes.push_back(tn->children[1]);
				removedNodes.push_back(tn->children[2]);
				removedNodes.push_back(tn->children[3]);
				tn->children[0] = nullptr;
				tn->children[1] = nullptr;
				tn->children[2] = nullptr;
				tn->children[3] = nullptr;
				tn->isLeaf = true;
			}
			// TODO
			// THIS LEAKS MEMORY
			else if (numEntities <= maxCapacity) {
				// Iterate over the children and add all the entities to the movedEntities vector
				// Add all the children to be removed
				std::vector<EntityData*> movedEntities(numEntities);
				unsigned int curEntInd = 0;
				EntityData* temp;
				for (int i = 0; i < 4; i++) {
					temp = tn->children[i]->head;
					while (temp != nullptr) {
						movedEntities[curEntInd++] = temp;
						temp = temp->next;
					}
					removedNodes.push_back(tn->children[i]);
					tn->children[i] = nullptr;
				}

				// Add the children to the parent
				tn->head = movedEntities[0];
				temp = tn->head;
				for (int i = 1; i < numEntities; i++) {
					temp->next = movedEntities[i];
					temp = temp->next;
				}
				temp->next = nullptr;

				tn->isLeaf = true;
				tn->numEntities = numEntities;
			}
		}
	}

	if (removedNodes.size() > 0) {
		nodes.erase(std::remove_if(nodes.begin(), nodes.end(),
			[&removedNodes](const TreeNode* tn) -> bool {
				return std::find(removedNodes.begin(), removedNodes.end(), tn) != removedNodes.end();
			}), nodes.end());
		for (TreeNode* tn : removedNodes) delete tn;
	}
}





// Packing and Drawing logic
void QuadTree::drawEntities() {
	entityRO->drawArrays(GL_POINTS, entityPos.size() / 2);
}
void QuadTree::packEntities(bool repack) {
	entityRO->pack(&entityPos[0], entityPos.size() * sizeof(GLfloat), repack);
}

void QuadTree::drawQuadTree() {
	quadTreeRO->drawElements(GL_LINE_LOOP, treeInds.size(), true);
}
void QuadTree::packQuadTree(bool repack) {
	treeVerts.clear();
	treeInds.clear();

	float bx = 0, by = 0, bw = 1, bh = 1;

	treeVerts.push_back(bx - bw);
	treeVerts.push_back(by - bh);
	treeVerts.push_back(bx + bw);
	treeVerts.push_back(by - bh);
	treeVerts.push_back(bx + bw);
	treeVerts.push_back(by + bh);
	treeVerts.push_back(bx - bw + 0.0001);
	treeVerts.push_back(by + bh - 0.0001);

	treeInds.push_back(0);
	treeInds.push_back(1);
	treeInds.push_back(2);
	treeInds.push_back(3);
	treeInds.push_back(globals::PRIM_RESTART_INDEX);

	if (!nodes[0]->isLeaf) {
		bw /= 2.0;
		bh /= 2.0;
		packQuadTreeHelper(nodes[0]->children[0], bx - bw, by - bh, bw, bh, 0, 0);
		packQuadTreeHelper(nodes[0]->children[1], bx + bw, by - bh, bw, bh, 1, 1);
		packQuadTreeHelper(nodes[0]->children[3], bx + bw, by + bh, bw, bh, 2, 2);
		packQuadTreeHelper(nodes[0]->children[2], bx - bw, by + bh, bw, bh, 3, 3);
	}

	quadTreeRO->pack(&treeVerts[0], treeVerts.size() * sizeof(GLfloat),
		&treeInds[0], treeInds.size() * sizeof(GLuint), repack);
}
void QuadTree::packQuadTreeHelper(TreeNode* curNode, float bx, float by, float bw, float bh, unsigned char quadrant, GLuint preservedIndex) {
	int vertsSize = treeVerts.size() / 2;
	switch (quadrant) {
	case 0:
		treeVerts.push_back(bx + bw);
		treeVerts.push_back(by - bh);
		treeVerts.push_back(bx + bw);
		treeVerts.push_back(by + bh);
		treeVerts.push_back(bx - bw);
		treeVerts.push_back(by + bh);

		treeInds.push_back(preservedIndex);
		treeInds.push_back(vertsSize);
		treeInds.push_back(vertsSize + 1);
		treeInds.push_back(vertsSize + 2);
		treeInds.push_back(globals::PRIM_RESTART_INDEX);

		if (!curNode->isLeaf) {
			bw /= 2.0;
			bh /= 2.0;
			packQuadTreeHelper(curNode->children[0], bx - bw, by - bh, bw, bh, 0, preservedIndex);
			packQuadTreeHelper(curNode->children[1], bx + bw, by - bh, bw, bh, 1, vertsSize);
			packQuadTreeHelper(curNode->children[3], bx + bw, by + bh, bw, bh, 2, vertsSize + 1);
			packQuadTreeHelper(curNode->children[2], bx - bw, by + bh, bw, bh, 3, vertsSize + 2);
		}
		break;
	case 1:
		treeVerts.push_back(bx - bw);
		treeVerts.push_back(by - bh);
		treeVerts.push_back(bx + bw);
		treeVerts.push_back(by + bh);
		treeVerts.push_back(bx - bw);
		treeVerts.push_back(by + bh);

		treeInds.push_back(vertsSize);
		treeInds.push_back(preservedIndex);
		treeInds.push_back(vertsSize + 1);
		treeInds.push_back(vertsSize + 2);
		treeInds.push_back(globals::PRIM_RESTART_INDEX);

		if (!curNode->isLeaf) {
			bw /= 2.0;
			bh /= 2.0;
			packQuadTreeHelper(curNode->children[0], bx - bw, by - bh, bw, bh, 0, vertsSize);
			packQuadTreeHelper(curNode->children[1], bx + bw, by - bh, bw, bh, 1, preservedIndex);
			packQuadTreeHelper(curNode->children[3], bx + bw, by + bh, bw, bh, 2, vertsSize + 1);
			packQuadTreeHelper(curNode->children[2], bx - bw, by + bh, bw, bh, 3, vertsSize + 2);
		}
		break;
	case 2:
		treeVerts.push_back(bx - bw);
		treeVerts.push_back(by - bh);
		treeVerts.push_back(bx + bw);
		treeVerts.push_back(by - bh);
		treeVerts.push_back(bx - bw);
		treeVerts.push_back(by + bh);

		treeInds.push_back(vertsSize);
		treeInds.push_back(vertsSize + 1);
		treeInds.push_back(preservedIndex);
		treeInds.push_back(vertsSize + 2);
		treeInds.push_back(globals::PRIM_RESTART_INDEX);

		if (!curNode->isLeaf) {
			bw /= 2.0;
			bh /= 2.0;
			packQuadTreeHelper(curNode->children[0], bx - bw, by - bh, bw, bh, 0, vertsSize);
			packQuadTreeHelper(curNode->children[1], bx + bw, by - bh, bw, bh, 1, vertsSize + 1);
			packQuadTreeHelper(curNode->children[3], bx + bw, by + bh, bw, bh, 2, preservedIndex);
			packQuadTreeHelper(curNode->children[2], bx - bw, by + bh, bw, bh, 3, vertsSize + 2);
		}
		break;
	case 3:
		treeVerts.push_back(bx - bw);
		treeVerts.push_back(by - bh);
		treeVerts.push_back(bx + bw);
		treeVerts.push_back(by - bh);
		treeVerts.push_back(bx + bw);
		treeVerts.push_back(by + bh);

		treeInds.push_back(vertsSize);
		treeInds.push_back(vertsSize + 1);
		treeInds.push_back(vertsSize + 2);
		treeInds.push_back(preservedIndex);
		treeInds.push_back(globals::PRIM_RESTART_INDEX);

		if (!curNode->isLeaf) {
			bw /= 2.0;
			bh /= 2.0;
			packQuadTreeHelper(curNode->children[0], bx - bw, by - bh, bw, bh, 0, vertsSize);
			packQuadTreeHelper(curNode->children[1], bx + bw, by - bh, bw, bh, 1, vertsSize + 1);
			packQuadTreeHelper(curNode->children[3], bx + bw, by + bh, bw, bh, 2, vertsSize + 2);
			packQuadTreeHelper(curNode->children[2], bx - bw, by + bh, bw, bh, 3, preservedIndex);
		}
		break;
	default:
		break;
	}
}