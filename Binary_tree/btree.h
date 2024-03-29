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
	                                         ///1 - ������ �������������
	                                         ///2 - ��� �����
	                                         ///3 - �������� � ������
	                                         ///4 - ���������� ����� �� ������������� �������
#endif
};


int TreeOk(tree_t* tree);


int NodeChildsCount(node_t* node);

int CopyValue_t(value_t* dest, const value_t source);

char* Value_tToStr(const value_t value);

int StrToValue_t(const char* valueS, value_t* value);

int ValueCmp(const value_t value1, const value_t value2);


tree_t TreeConstructor(const char* name = "tree");

int ChangeNodeValue(node_t* node, value_t value);

int AddChild(tree_t* tree, node_t* node, value_t elem, const int side, node_t** createdNode = NULL);

int DeleteChild(tree_t* tree, node_t* node, const int side);

int AddSubtree(tree_t* tree, tree_t* subtree, node_t* node, const int side);

int DeleteSubtree(tree_t* tree, node_t* node, const int side);

tree_t SplitTree(tree_t* tree, node_t* node, const int side, const char* name = "subtree", int* err = NULL);

int DeleteTree(tree_t* tree);

int TreeDestructor(tree_t* tree);


int CreateTreeImage(tree_t* tree, const char foutName[] = "tree.png", const char gvFileName[] = "tree.gv");

int ShowTree(tree_t* tree);


char* TreeToCode(tree_t* tree, int* size);

tree_t CodeToTree(char* code, const char* treeName="tree_from_code", int* err = NULL);

char* FindNodeByValue(tree_t* tree, const value_t value, node_t*& foundNode, int* err = NULL);