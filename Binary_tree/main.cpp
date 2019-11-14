#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "btree.h"

int main() {
	tree_t tree1 = TreeConstructor("tree1");

	node_t* nodes[10] = {};
	AddChild(&tree1, tree1.root, 4, LEFT_CHILD, &nodes[0]);
	ShowTree(&tree1);

	AddChild(&tree1, nodes[0], 6, RIGHT_CHILD, &nodes[1]);
	ShowTree(&tree1);

	AddChild(&tree1, nodes[0], 7, LEFT_CHILD, &nodes[2]);
	ShowTree(&tree1);


	tree_t tree2 = TreeConstructor("tree2");
	AddChild(&tree2, tree2.root, 2, LEFT_CHILD, &nodes[3]);
	ShowTree(&tree2);

	AddChild(&tree2, tree2.root, 3, RIGHT_CHILD, &nodes[4]);
	ShowTree(&tree2);


	AddSubtree(&tree1, &tree2, tree1.root, RIGHT_CHILD);
	ShowTree(&tree1);


	DeleteChild(&tree1, nodes[0], RIGHT_CHILD);
	ShowTree(&tree1);


	DeleteSubtree(&tree1, tree1.root, LEFT_CHILD);
	ShowTree(&tree1);

	DeleteTree(&tree1);
	
	return 0;
}