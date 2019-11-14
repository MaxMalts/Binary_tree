#include <stdio.h>
#include "btree.h"

int main() {
	tree_t myTree = TreeConstructor("myTree");

	AddChild(&myTree, myTree.root, 4, LEFT_CHILD);

	DeleteTree(&myTree);
	
	return 0;
}