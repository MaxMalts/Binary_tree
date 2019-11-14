#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "btree.h"


#ifdef _DEBUG
#define PrintTree_OK(stk) TreeDump(&tree, __FILE__, __LINE__, __FUNCTION__, "just looking");
#define PrintTree_NOK(stk) TreeDump(&tree, __FILE__, __LINE__, __FUNCTION__, "tree has an error");
#else
#define PrintTree_OK(tree) ;
#define PrintTree_NOK(tree) ;
#endif

/**
*	���������, ������������� �� �������� ����� �� ������ ����
*
*	@param[in] side �������
*
*	@return 1 (true) - �������������; 0 (false) - �� �������������
*/

int SideIsCorrect(const int side) {
	if (side == LEFT_CHILD || side == RIGHT_CHILD) {
		return 1;
	}
	return 0;
}


/**
*	������� ���������� �������
*
*	@param[in] node ����
*
*	@return ���������� ������� (0, 1, 2)
*/

int NodeChildsCount(node_t* node) {
	assert(node != NULL);

	if (node->left != NULL && node->right != NULL) {
		return 2;
	}
	else if (node->left != NULL || node->right != NULL) {
		return 1;
	}
	return 0;
}


/**
*	�������� ������� ���� value_t
*
*	@param[out] dest ���� ��������
*	@param[in] source ������ ��������
*
*	@return 0 - ��� ������ ���������
*/

int CopyValue_t(value_t* dest, value_t* source) {
	assert(dest != NULL);
	assert(source != NULL);

	*dest = *source;

	return 0;
}


/**
*	����������� value_t � ������
*
*	@param[in] value ������� ���� value_t
*
*	@return ��������� �� ������. �� �������� ���������� ������ �� ����� ���������!
*/

char* Value_tToStr(const value_t value) {
	const int value_tMaxStrSize = 20;

	char* str = (char*)calloc(value_tMaxStrSize + 1, sizeof(char));
	itoa(value, str, 10);
	return str;
}


/*  �� ��� ������������
*	���������� ���������� ���������� ��� ���� ����� ��� ��������� Graphviz.\
 ����������, ���������� ���� �������� �� �������� ����������� ����������� ������.
*
*	@param[in] gvFile ���� ��� ������
*	@param[in] node �������� ����
*
*	@return 0 - ��� ������ ���������
*/

int NodesOutput(FILE* gvFile, node_t* node) {
	assert(gvFile != NULL);
	assert(node != NULL);

	char* sValue = Value_tToStr(node->value);

#ifdef _DEBUG
	fprintf(gvFile, "\t%d [label=\"{%p|%p|%s|{%p|%p}}\"]\n", (int)node, node, node->parent, sValue, node->left, node->right);
#else
	fprintf(gvFile, "\t%d [label=\"%s\"]", (int)node, sValue);
#endif

	if (node->left != NULL) {
		NodesOutput(gvFile, node->left);
		fprintf(gvFile, "\t%d -> %d\n", (int)node, (int)node->left);
	}
	if (node->right != NULL) {
		NodesOutput(gvFile, node->right);
		fprintf(gvFile, "\t%d -> %d\n", (int)node, (int)node->right);
	}

	return 0;
}


/**
*	������ ������ � ������� ��������� Graphviz.
*
*	@param[in] tree ������
*	@param[in] foutName ��� ����� � �������� (�� ��������� - "tree.png")
*	@param[in] gvFileName ��� ����� � ��������� ����� ��� Graphviz (�� ��������� - "tree.gv")
*
*	@return 0 - ��� ������ ���������
*/

int CreateTreeImage(tree_t* tree, const char foutName[], const char gvFileName[]) {
	assert(tree != NULL);
	assert(gvFileName != NULL);

	FILE* gvFile = fopen(gvFileName, "w");
	if (NULL == gvFile) {
		return 1;
	}
	
#ifdef _DEBUG
	fprintf(gvFile, "digraph %s {\n", tree->name);
	fprintf(gvFile, "\tnode [shape=record];\n\n");
	fprintf(gvFile, "\tformat_node [label=\"{adress|parent|value|{left|right}}\"]\n\n");
#else
	fprintf(ftemp, "digraph {\n", tree->name);
#endif

	NodesOutput(gvFile, tree->root);

	fprintf(gvFile, "}");
	fclose(gvFile);

	char sysCommand[1000] = "";
	sprintf(sysCommand, "dot -Tpng %s -o %s", gvFileName, foutName);
	system(sysCommand);

	return 0;
}

/*  �� ��� ������������
*	������� ���������� � ������. ��� ������������ ������, ����������� ShowTree().
*
*	@param[in] tree ������
*	@param[in] file �������� �����, ����� ������� �������
*	@param[in] line ����� ������, �� ������� ������� �������
*	@param[in] function ��� �������, �� ������� ������� �������
*	@param[in] reason �������, �� ������� ������� �������
*/

void TreeDump(tree_t* tree, const char* file, const int line, const char* function, const char* reason) {
	assert(tree != NULL);
	assert(file != NULL);
	assert(line > 0);
	assert(function != NULL);
	assert(reason != NULL);

	char status[10] = "ok";
	if (tree->err != 0) {
		strcpy(status, "ERR");
	}

	printf("\nInfo about a tree from file: %s, function: %s, line: %d, reason: %s:\n", file, function, line, reason);
	printf("tree_t \"%s\" (%p):    (%s)\n", tree->name, tree, status);

	printf("\tsize = %d\n", tree->size);

	char* sValue = Value_tToStr(tree->root->value);
	printf("\troot (%p):\n", tree->root);
	printf("\t\tvalue: %s;\n", sValue);
	printf("\t\tleft: %p;\n", tree->root->left);
	printf("\t\tright: %p;\n", tree->root->right);
	printf("\t\tparent: %p;\n", tree->root->parent);
	free(sValue);

	printf("err = %d\n\n\n", tree->err);
}

/**
*	������� ����� ������. ����� ������, �������� �������� �����\
 �� ������ � ������� ������� ChangeNodeValue()!
 *
 *	@param[in] name ��� ������
 *
 *	@return ��������� �� ��������� ������
*/

tree_t TreeConstructor(const char* name) {
	assert(name != NULL);

	tree_t tree = {};
	
#ifdef _DEBUG
	strcpy(tree.name, name);
	tree.err = 0;
#endif
	
	tree.root = (node_t*)calloc(1, sizeof(node_t));
	tree.size = 1;

	return tree;
}


/**
*	�������������� �������� ����
*
*	@param node ����
*	@param[in] value ��������
*
*	@return 0 - ��� ������ ���������
*/

int ChangeNodeValue(node_t* node, value_t value) {
	assert(node != NULL);

	CopyValue_t(&node->value, &value);

	return 0;
}

/**
*	��������� ���� � ��������� ������� �� ��������
*
*	@param tree ������, � ������� ��������� ����
*	@param node ����, � �������� ���������
*	@param[in] value �������� ������ ����
*	@param[in] side �������, � ������� ��������� (LEFT_CHILD, RIGHT_CHILD)
*	@param[out] newNode ����� ������ ����
*
*	@return 1 - � ���� ��� ��� �������� ���� � ��������������� �������;\
 2 - �������� side ���� ������������ �������; 0 - ��� ������ ���������
*/

int AddChild(tree_t* tree, node_t* node, value_t value, const int side, node_t** createdNode) {
	assert(tree != NULL);
	assert(node != NULL);

	if (!SideIsCorrect(side)) {
		return 2;
	}

	node_t* newNode = (node_t*)calloc(1, sizeof(node_t));
	newNode->value = value;
	newNode->left = NULL;
	newNode->right = NULL;
	newNode->parent = node;

	switch (side) {
	case LEFT_CHILD:
		if (node->left != NULL) {
			return 1;
		}
		node->left = newNode;
		break;
	case RIGHT_CHILD:
		if (node->right != NULL) {
			return 1;
		}
		node->right = newNode;
		break;
	}

	tree->size++;

	*createdNode = newNode;

	return 0;
}


///**
//*	��������� �������� ���� � ��������� ������� �� ������
//*
//*	@param tree ������, � ������� ��������� ����
//*	@param parentNode ����, � �������� ���������
//*	@param[in] childNode �������� ����
//*	@param[in] side �������, � ������� ��������� (LEFT_CHILD, RIGHT_CHILD)
//*
//*	@return 1 - � ���� ��� ��� �������� ���� � ��������������� �������;\
// 2 - �������� side ���� ������������ �������; 0 - ��� ������ ���������
//*/
//
//int AddNode(tree_t* tree, node_t* parentNode, node_t* childNode, const int side) {
//	assert(tree != NULL);
//	assert(parentNode != NULL);
//	assert(childNode != NULL);
//
//	if (!SideIsCorrect(side)) {
//		return 2;
//	}
//
//	switch (side) {
//	case LEFT_CHILD:
//		if (parentNode->left != NULL) {
//			return 1;
//		}
//		parentNode->left = childNode;
//		break;
//	case RIGHT_CHILD:
//		if (parentNode->right != NULL) {
//			return 1;
//		}
//		parentNode->right = childNode;
//		break;
//	}
//
//	return 0;
//}

/**
*	������� �������� ����
*
*	@param tree ������, � ������� ��������� ����
*	@param node ����, � ������� ����������� ������� ����
*	@param[in] side �������, � ������� ������� (LEFT_CHILD, RIGHT_CHILD)
*
*	@return 1 - �� ���� ��������� ����; 2 - �������� ���� �������� ����������;\
 3 - �������� side ���� ������������ �������; 0 - ��� ������ ���������
*/

int DeleteChild(tree_t* tree, node_t* node, const int side) {
	assert(tree != NULL);
	assert(node != NULL);

	if (!SideIsCorrect(side)) {
		return 3;
	}

	switch (side) {
	case LEFT_CHILD:
		if (node->left == NULL) {
			return 1;
		}
		else if (NodeChildsCount(node->left) > 0) {
			return 2;
		}

		free(node->left);
		node->left = NULL;
		break;

	case RIGHT_CHILD:
		if (node->right == NULL) {
			return 1;
		}
		else if (NodeChildsCount(node->right) > 0) {
			return 2;
		}

		free(node->right);
		node->right = NULL;
		break;
	}

	tree->size--;

	return 0;
}


/**
*	��������� ��������� � ������ �� ���������� ����.
*
*	@param tree ������, � ������� ���������
*	@param subtree ���������, ������� ���������. ����� ������������� ����������������!
*	@param node ����, � �������� ���������
*	@param[in] side ������� ����, � ������� ���������
*
*	@return 1 - � ���� ��� ��� �������� ���� � ��������������� �������;\
 2 - �������� side ���� ������������ �������; 0 - ��� ������ ���������
*/	

int AddSubtree(tree_t* tree, tree_t* subtree, node_t* node, const int side) {
	assert(tree != NULL);
	assert(subtree != NULL);
	assert(node != NULL);

	if (!SideIsCorrect(side)) {
		return 2;
	}

	switch (side) {
	case LEFT_CHILD:
		if (node->left != NULL) {
			return 1;
		}
		node->left = subtree->root;
		break;

	case RIGHT_CHILD:
		if (node->right != NULL) {
			return 1;
		}
		node->right = subtree->root;
		break;
	}
	subtree->root->parent = node;
	tree->size += subtree->size;

	TreeDestructor(subtree);

	return 0;
}


/*  �� ��� ������������
*	������� ��� �������� ���� � ��� ����
*
*	@param ����
*	@param[in] count ���������� ��� ��������� �����\
 (��� ��������� ������ ��������, ���������� ������ ��� ������������ ������)
*
*	@return ���������� ��������� �����
*/

int DeleteNodes(node_t* node, int count = 0) {
	assert(node != NULL);
	assert(count >= 0);

	if (node->left != NULL) {
		count = DeleteNodes(node->left, count);
	}
	if (node->right != NULL) {
		count = DeleteNodes(node->right, count);
	}
	free(node);

	return count + 1;
}


/**
*	������� ���������
*
*	@param tree ������
*	@param node ����, ��������� ��������� ������� � ��������� ����
*	@param side ������� ��������� ����
*
*	@return 1 - �� ���� ��������� ����; 2 - �������� side ���� ������������ �������;\
 0 - ��� ������ ���������
*/

int DeleteSubtree(tree_t* tree, node_t* node, const int side) {
	assert(tree != NULL);
	assert(node != NULL);

	if (!SideIsCorrect(side)) {
		return 2;
	}

	int deletedCount = 0;
	switch (side) {
	case LEFT_CHILD:
		if (node->left == NULL) {
			return 1;
		}

		deletedCount = DeleteNodes(node->left);
		node->left = NULL;
		break;

	case RIGHT_CHILD:
		if (node->right == NULL) {
			return 1;
		}

		deletedCount = DeleteNodes(node->right);
		node->right = NULL;
		break;
	}

	tree->size -= deletedCount;
	assert(tree->size >= 0);

	return 0;
}


/**
*	���������� ������� ������, ������� ��� ��� ����.\
 ��������, ���� ������ ������ �������� ���������� �\
 ������ ������, �� ����������� ���� ��������.
*
*	@param[in] tree ������
*
*	@return 0 - ��� ������ ���������
*/

int DeleteTree(tree_t* tree) {
	assert(tree != NULL);

	DeleteNodes(tree->root);
	TreeDestructor(tree);

	return 0;
}


/**
*	������������� ������. ��������, ������ ������� �� ������� ���� ������!\
 ����� ������� ����, �������������� �������� DeleteTree()!
*
*	@param tree ������
*
*	@return 0 - ��� ������ ���������
*/

int TreeDestructor(tree_t* tree) {
	assert(tree != NULL);

	tree->root = NULL;
	tree->size = 0;

	return 0;
}