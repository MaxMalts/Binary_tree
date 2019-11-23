#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "btree.h"

int main() {
	tree_t tree1 = TreeConstructor("tree1");

	node_t* nodes[10] = {};
	AddChild(&tree1, tree1.root, 15, LEFT_CHILD, &nodes[0]);
	//ShowTree(&tree1);

	AddChild(&tree1, nodes[0], 6, RIGHT_CHILD, &nodes[1]);
	//ShowTree(&tree1);

	AddChild(&tree1, nodes[0], 7, LEFT_CHILD, &nodes[2]);
	ShowTree(&tree1);

	int a = 6;
	char* way = FindNodeByValue(&tree1, &a);

	tree_t tree2 = TreeConstructor("tree2");
	AddChild(&tree2, tree2.root, 2, LEFT_CHILD, &nodes[3]);
	//ShowTree(&tree2);

	AddChild(&tree2, tree2.root, 3, RIGHT_CHILD, &nodes[4]);
	//ShowTree(&tree2);


	AddSubtree(&tree1, &tree2, tree1.root, RIGHT_CHILD);
	ShowTree(&tree1);


	char code[] = "{2{3{{4},@},5{@,6{{7},{8}}}}}";

	int err = 0;
	tree_t tree3 = CodeToTree(code, "tree3", &err);
	//ShowTree(&tree3);
	

	tree_t tree4 = SplitTree(&tree1, tree1.root, LEFT_CHILD);
	ShowTree(&tree1);
	ShowTree(&tree4);

	DeleteChild(&tree4, nodes[0], RIGHT_CHILD);
	ShowTree(&tree4);


	DeleteSubtree(&tree1, tree1.root, LEFT_CHILD);
	ShowTree(&tree1);

	DeleteTree(&tree1);

	getchar();
	
	return 0;
}