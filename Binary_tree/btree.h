#pragma once

#define LEFT_CHILD 1
#define RIGHT_CHILD 2

typedef int value_t;

struct node_t {
	value_t value = {};
	node_t* left = NULL;
	node_t* right = NULL;
	node_t* parent = NULL;
};

struct tree_t {

	node_t* root = {};                       ///<������
	int size = 0;                            ///<��������� ������ ������

#ifdef _DEBUG
	char name[30] = "";                      ///<��� ������
	int err = 0;                             ///<��� ������, ������������ � ������:\n
											 ///0 - ��� ������\n
#endif
};


int NodeChildsCount(node_t* node);

int CopyValue_t(value_t* dest, value_t* source);


tree_t TreeConstructor(const char* name);

int ChangeNodeValue(node_t* node, value_t value);

int AddChild(tree_t* tree, node_t* node, value_t elem, const int side, node_t** createdNode = NULL);

int DeleteChild(tree_t* tree, node_t* node, const int side);

int AddSubtree(tree_t* tree, tree_t* subtree, node_t* node, const int side);

int DeleteSubtree(tree_t* tree, node_t* node, const int side);

int DeleteTree(tree_t* tree);

int TreeDestructor(tree_t* tree);


int CreateTreeImage(tree_t* tree, const char foutName[] = "tree.png", const char gvFileName[] = "tree.gv");