#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <windows.h>
#include "btree.h"
#include "buffer.h"


#ifdef _DEBUG
#define PrintTree_OK(tree) TreeDump(&tree, __FILE__, __LINE__, __FUNCTION__, "just looking");
#define PrintTree_NOK(tree) TreeDump(&tree, __FILE__, __LINE__, __FUNCTION__, "tree has an error");
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


/**
*	����������� ������ � value_t
*
*	@param[in] valueS ������
*	@param[out] value �������� value_t
*
*	@return 1 - � ������ �������� ������������� value_t; 0 - ��� ������ ���������
*/

int StrToValue_t(const char* valueS, value_t* value) {
	assert(valueS != NULL);
	assert(value != NULL);

	if (!isdigit(valueS[0]) && valueS[0] != '-') {
		return 1;
	}
	
	*value = atoi(valueS);

	return 0;
}


/*  �� ��� ������������
*	��������� ���� ���� ��������� � ������� �� ����������
*
*	@param[in] node ����
*	@param[out] NNodes ���������� �����. �� ����� �������� ������ ���� 0!
*
*	@return 1 - ������ ����; 0 - ������ ���
*/

#ifdef _DEBUG
int NodesOk(node_t* node, int* NNodes) {
	assert(node != NULL);
	assert(NNodes != NULL);
	
	(*NNodes)++;
	int err = 0;
	if (node->left != NULL) {
		if (node->left->parent != node) {
			return 1;
		}
		err = NodesOk(node->left, NNodes);
		if (err != 0) {
			return err;
		}
	}
	if (node->right != NULL) {
		if (node->right->parent != node) {
			return 1;
		}
		err = NodesOk(node->right, NNodes);
		if (err != 0) {
			return err;
		}
	}
	
	return 0;
}
#endif


/**
*	��������� ������ � ���������� � ���� ��� ������
*
*	@param[in] tree ������
*
*	@return 0 - ������ ������������; 1 - ������ ����������
*/

#ifdef _DEBUG
int TreeOk(tree_t* tree) {
	if (tree == NULL) {
		return 0;
	}
	if (tree->size < 0) {
		tree->err = 1;
		return 0;
	}
	if (tree->root == NULL) {
		tree->err = 2;
		return 0;
	}
	int actSize = 0;
	if (NodesOk(tree->root, &actSize) != 0) {
		tree->err = 3;
	}
	if (actSize != tree->size) {
		tree->err = 4;
		return 0;
	}

	return 1;
}
#endif


/**
*	���������� ������ � ������
*
*	@param[in] tree ������
*
*	@retrurn ��� ������. ������������ ��. � ����������� ������
*/

int TreeErr(tree_t* tree) {
	assert(tree != NULL);

	return tree->err;
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

#ifdef _DEBUG
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

	char* valueS = Value_tToStr(tree->root->value);
	printf("\troot (%p):\n", tree->root);
	printf("\t\tvalue: %s;\n", valueS);
	printf("\t\tleft: %p;\n", tree->root->left);
	printf("\t\tright: %p;\n", tree->root->right);
	printf("\t\tparent: %p;\n", tree->root->parent);
	free(valueS);

	printf("err = %d\n\n\n", tree->err);
}
#endif


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

	char* valueS = Value_tToStr(node->value);

#ifdef _DEBUG
	fprintf(gvFile, "\t%d [label=\"{%p|%p|%s|{%p|%p}}\"]\n", (int)node, node, node->parent, valueS, node->left, node->right);
#else
	fprintf(gvFile, "\t%d [label=\"%s\"]", (int)node, valueS);
#endif
	free(valueS);

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
*	@return 1 - �� ������� ������� ���� ��� ������; 2 - �� ���� �������� ������\
� ������� (������ � ������ �������); 0 - ��� ������ ���������
*/

int CreateTreeImage(tree_t* tree, const char foutName[], const char gvFileName[]) {
	assert(tree != NULL);
	assert(gvFileName != NULL);

#ifdef _DEBUG
	if (!TreeOk(tree)) {
		PrintTree_NOK(*tree);
		return 2;
	}
#endif

	FILE* gvFile = fopen(gvFileName, "w");
	if (NULL == gvFile) {
		return 1;
	}
	
#ifdef _DEBUG
	fprintf(gvFile, "digraph %s {\n", tree->name);
	fprintf(gvFile, "\tnode [shape=record]\n\n");
	fprintf(gvFile, "\tformat_node [label=\"{adress|parent|value|{left|right}}\"]\n\n");
#else
	fprintf(gvFile, "digraph {\n");
#endif

	NodesOutput(gvFile, tree->root);

	fprintf(gvFile, "}");
	fclose(gvFile);

	char sysCommand[1000] = "";
	sprintf(sysCommand, "start /wait dot -Tpng %s -o %s", gvFileName, foutName);
	system(sysCommand);

	return 0;
}


/**
*	������� ����������� ������ � ��������� ���. ������� ���������, �� ����������� �����!
*
*	@param[in] tree ������
*
*	@return 1 - �������� ��� �������� �����������; 2 - �������� ��� �������� �����������;\
 3 - �� ���� �������� ������ � ������� (������ � ������ �������); 0 - ��� ������ ���������
*/

int ShowTree(tree_t* tree) {
	assert(tree != NULL);

#ifdef _DEBUG
	if (!TreeOk(tree)) {
		PrintTree_NOK(*tree);
		return 3;
	}
#endif

	Sleep(700);   //���� ����� �� �������, �� ���������� ����������� ����������� ��� �����.

	if (CreateTreeImage(tree) == 1) {
		return 1;
	}
	if (system("tree.png") != 0) {
		return 2;
	}
	return 0;
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

#ifdef _DEBUG
	if (TreeOk(&tree)) {
		PrintTree_OK(tree);
	}
	else {
		PrintTree_NOK(tree);
	}
#endif

	return tree;
}


/*  �� ��� ������������
*	������� ���������� �����-�������� � ������ �������� ���� (����������)
*
*	@param[in] node ����
*	@param[out] size ����������
*
*	@return 0 - ��� ������ ���������
*/

int CalcNodesCount(node_t* node, int* size) {
	assert(node != NULL);
	assert(size != NULL);

	if (node->left != NULL) {
		int err = CalcNodesCount(node->left, size);
		if (err != 0) {
			return err;
		}
	}
	if (node->right != NULL) {
		int err = CalcNodesCount(node->right, size);
		if (err != 0) {
			return err;
		}
	}
	(*size)++;
	return 0;
}


/*  �� ��� ������������
*	������������� ������ ������
*
*	@param tree ������
*
*	@return ��. ������������ �������� CalcNodesCount
*/

int RecalcTreeSize(tree_t* tree) {
	assert(tree != NULL);
	assert(tree->root != NULL);

	return CalcNodesCount(tree->root, &tree->size);
}

/*  �� ��� ������������
*	������� ����� ������. ����� ������, �������� �������� �����\
 �� ������ � ������� ������� ChangeNodeValue()!
 *
 *	@param[in] name ��� ������
 *
 *	@return ��������� �� ��������� ������
*/

tree_t TreeRootConstructor(node_t* root, const char* name) {
	assert(root != NULL);
	assert(root->parent == NULL);
	assert(name != NULL);

	tree_t tree = {};

#ifdef _DEBUG
	strcpy(tree.name, name);
	tree.err = 0;
#endif

	tree.root = root;
	RecalcTreeSize(&tree);

#ifdef _DEBUG
	if (TreeOk(&tree)) {
		PrintTree_OK(tree);
	}
	else {
		PrintTree_NOK(tree);
	}
#endif

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


/*  �� ��� ������������
*	������� ����� ����
*
*	@return ��������� �� ����� ����. �� �������� ���������� ������ �� ����� ���������!
*/

node_t* CreateNode() {

	node_t* node = (node_t*)calloc(1, sizeof(node_t));
	node->value = {};
	node->left = NULL;
	node->right = NULL;
	node->parent = NULL;

	return node;
}

/**
*	��������� ���� � ��������� ������� �� ��������
*
*	@param tree ������, � ������� ��������� ����
*	@param node ����, � �������� ���������. ������ ���� ������ ������, ����� � ������ ��������� ������.
*	@param[in] value �������� ������ ����
*	@param[in] side �������, � ������� ��������� (LEFT_CHILD, RIGHT_CHILD)
*	@param[out] newNode ����� ������ ����
*
*	@return 1 - � ���� ��� ��� �������� ���� � ��������������� �������;\
 2 - �������� side ���� ������������ �������; 3 - �� ���� �������� ������ �\
  ������� (������ � ������ �������); 0 - ��� ������ ���������
*/

int AddChild(tree_t* tree, node_t* node, value_t value, const int side, node_t** createdNode) {
	assert(tree != NULL);
	assert(node != NULL);

#ifdef _DEBUG
	if (!TreeOk(tree)) {
		PrintTree_NOK(*tree);
		return 3;
	}
#endif

	if (!SideIsCorrect(side)) {
		return 2;
	}

	node_t* newNode = CreateNode();
	newNode->value = value;
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

	if (createdNode != NULL) {
		*createdNode = newNode;
	}

#ifdef _DEBUG
	if (TreeOk(tree)) {
		PrintTree_OK(*tree);
	}
	else {
		PrintTree_NOK(*tree);
	}
#endif

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
*	@param node ����, � ������� ����������� ������� ����.\
 ������ ���� ������ ������, ����� � ������ ��������� ������.
*	@param[in] side �������, � ������� ������� (LEFT_CHILD, RIGHT_CHILD)
*
*	@return 1 - �� ���� ��������� ����; 2 - �������� ���� �������� ����������;\
 3 - �������� side ���� ������������ �������; 4 - �� ���� �������� ������ �\
 ������� (������ � ������ �������); 0 - ��� ������ ���������
*/

int DeleteChild(tree_t* tree, node_t* node, const int side) {
	assert(tree != NULL);
	assert(node != NULL);

#ifdef _DEBUG
	if (!TreeOk(tree)) {
		PrintTree_NOK(*tree);
		return 4;
	}
#endif

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

#ifdef _DEBUG
	if (TreeOk(tree)) {
		PrintTree_OK(*tree);
	}
	else {
		PrintTree_NOK(*tree);
	}
#endif

	return 0;
}


/**
*	��������� ��������� � ������ �� ���������� ����.
*
*	@param tree ������, � ������� ���������
*	@param node ����, � �������� ���������. ������ ���� ������ ������, ����� � ������ ��������� ������.
*	@param subtree ���������, ������� ���������. ����� ������������� ����������������!
*	@param[in] side ������� ����, � ������� ���������
*
*	@return 1 - � ���� ��� ��� �������� ���� � ��������������� �������;\
 2 - �������� side ���� ������������ �������; 3, 4 - �� ���� ��������\
 ������ ��� �������������� ��������� � ������� (������ � ������ �������);\
 0 - ��� ������ ���������
*/	

int AddSubtree(tree_t* tree, node_t* node, tree_t* subtree, const int side) {
	assert(tree != NULL);
	assert(subtree != NULL);
	assert(node != NULL);

#ifdef _DEBUG
	if (!TreeOk(tree)) {
		PrintTree_NOK(*tree);
		return 3;
	}
#endif
#ifdef _DEBUG
	if (!TreeOk(subtree)) {
		PrintTree_NOK(*subtree);
		return 4;
	}
#endif

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

#ifdef _DEBUG
	if (TreeOk(tree)) {
		PrintTree_OK(*tree);
	}
	else {
		PrintTree_NOK(*tree);
	}
#endif

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
*	@param node ����, ��������� ��������� ������� � ��������� ����.\
 ������ ���� ������ ������, ����� � ������ ��������� ������.
*	@param side ������� ��������� ����
*
*	@return 1 - �� ���� ��������� ����; 2 - �������� side ���� ������������ �������;\
 3 - �� ���� �������� ������ � ������� (������ � ������ �������); 0 - ��� ������ ���������
*/

int DeleteSubtree(tree_t* tree, node_t* node, const int side) {
	assert(tree != NULL);
	assert(node != NULL);

#ifdef _DEBUG
	if (!TreeOk(tree)) {
		PrintTree_NOK(*tree);
		return 3;
	}
#endif

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

#ifdef _DEBUG
	if (TreeOk(tree)) {
		PrintTree_OK(*tree);
	}
	else {
		PrintTree_NOK(*tree);
	}
#endif

	return 0;
}


/**
*	��������� ������ ����� �������� ������ ������ - ��������� ���������
*
*	@param tree �������� ������
*	@param[in] node ����, �������� ���� �������� ����� ������ ������ ������.\
 ������ ���� ������ ������, ����� � ������ ��������� ������.
*	@param[in] side ������� ����
*	@param[in] name ��� ������ ������
*	@param[out] err ������: 1 - �� ���� �������� ������������ ������;\
 2 - ������������ �������; 0 - ��� ������ ���������
*
*	@return ���������� ���������
*/

tree_t SplitTree(tree_t* tree, node_t* node, const int side, const char* name, int* err) {
	assert(tree != NULL);
	assert(node != NULL);
	assert(name != NULL);

	tree_t err_tree = {};

#ifdef _DEBUG
	if (!TreeOk(tree)) {
		PrintTree_NOK(*tree);
		if (err != NULL) {
			*err = 1;
		}
		return err_tree;
	}
#endif

	if (!SideIsCorrect(side)) {
		if (err != NULL) {
			*err = 2;
		}
		return err_tree;
	}

	tree_t newTree = {}; 

	switch (side) {
	case LEFT_CHILD:
		node->left->parent = NULL;
		newTree = TreeRootConstructor(node->left, name);
		node->left = NULL;
		break;
	case RIGHT_CHILD:
		node->right->parent = NULL;
		newTree = TreeRootConstructor(node->right, name);
		node->right = NULL;
		break;
	default:
		assert(0);
	}
	tree->size -= newTree.size;

#ifdef _DEBUG
	if (TreeOk(&newTree)) {
		PrintTree_OK(newTree);
	}
	else {
		PrintTree_NOK(newTree);
	}
	if (TreeOk(tree)) {
		PrintTree_OK(*tree);
	}
	else {
		PrintTree_NOK(*tree);
	}
#endif

	if (err != NULL) {
		*err = 0;
	}
	return newTree;
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

#ifdef _DEBUG
	if (!TreeOk(tree)) {
		PrintTree_NOK(*tree);
	}
#endif

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


/*  �� ��� ������������
*	������� ��� �� ����
*
*	@param[in] node ����
*	@param[out] codeBuf ����� � �����
*
*	@return 0 - ��� ������ ���������
*/

int NodesToCode(node_t* node, buf_t* codeBuf) {
	assert(node != NULL);
	assert(codeBuf != NULL);

	char* valueS = Value_tToStr(node->value);
	
	if (NodeChildsCount(node) > 0) {
		Bufcat(codeBuf, valueS);
		Bufcat(codeBuf, "{");
		if (node->left != NULL) {
			NodesToCode(node->left, codeBuf);
		}
		else {
			Bufcat(codeBuf, "@");
		}
		Bufcat(codeBuf, ",");
		if (node->right != NULL) {
			NodesToCode(node->right, codeBuf);
		}
		else {
			Bufcat(codeBuf, "@");
		}
		Bufcat(codeBuf, "}");
	}
	else {
		Bufcat(codeBuf, "{");
		Bufcat(codeBuf, valueS);
		Bufcat(codeBuf, "}");
	}

	return 0;
}


/**
*	������� ��� �� ������
*
*	@param[in] tree ������
*	@param[out] size ����� ����������� ���� (��� ����� '\0').\
 ���� �������� �������������, �� �������� ��������� ������:\
 -1 - �� ���� �������� ������ � ������� (������ � ������ �������);\
 -2 -�������� ��� �������� ������; 0 - ��� ������ ���������
*
*	@return ��������� �� ����� � �����. � ������ ������ ����� NULL.\
 �� �������� ���������� ������ �� ����� ���������!
*/

char* TreeToCode(tree_t* tree, int* size) {
	assert(tree != NULL);

#ifdef _DEBUG
	if (!TreeOk(tree)) {
		PrintTree_NOK(*tree);
		if (size != NULL) {
			*size = -1;
		}
		return NULL;
	}
#endif

	int bufConstrErr = 0;
	buf_t codeBuf = BufConstructor('w', &bufConstrErr);
	if (bufConstrErr != 0) {
		if (size != NULL) {
			*size = -2;
		}
		return NULL;
	}

	Bufcat(&codeBuf, "{");
	NodesToCode(tree->root, &codeBuf);
	Bufcat(&codeBuf, "}");

	char* res = (char*)calloc(codeBuf.lastChar + 2, sizeof(char));
	strncpy(res, codeBuf.str, codeBuf.lastChar + 1);
	if (size != NULL) {
		*size = codeBuf.lastChar + 1;
	}

	BufDestructor(&codeBuf);

	return res;
}


/*
*	���������� ������� ���� �� ����
*
*	@param[in] buf ����� � �����
*	@param[out] node ������� ����. ��������, ��� ��������� ������\
 ������ ���� NULL, ���� ��������� �������� ����!
*	@param[out] size
*
*	@return 1 - ������ � ����; 0 - ��� ������ ���������
*/

int CodeToNodes(buf_t* buf, node_t*& node, int* size) {
	assert(buf != NULL);
	assert(size != NULL);

	char curCh = Bgetc(buf);
	if (curCh == '{') {
		char nextCh = Bgetc(buf);
		if (nextCh == EOB) {
			return 1;
		}
		if (nextCh != '{') {
			Bseek(buf, -1, BSEEK_CUR);
		}

		if (Bgetc(buf) == '@') {
			node->left = NULL;

			int err = 0;
			err = CodeToNodes(buf, node, size);
			if (err != 0) {
				return err;
			}
		}
		else {
			Bseek(buf, -1, BSEEK_CUR);

			char valueS[100] = "";
			ReadToChar(valueS, buf, "{}");

			node_t* newNode = CreateNode();

			value_t value = {};
			StrToValue_t(valueS, &value);
			ChangeNodeValue(newNode, value);
			if (node != NULL) {
				node->left = newNode;
			}
			newNode->parent = node;
			if (node == NULL) {
				node = newNode;
			}
			(*size)++;

			int err = 0;
			err = CodeToNodes(buf, newNode, size);
			if (err != 0) {
				return err;
			}
		}
	}
	else if (curCh == ',') {
		char nextCh = Bgetc(buf);
		if (nextCh == EOB) {
			return 1;
		}
		if (nextCh != '{') {
			Bseek(buf, -1, BSEEK_CUR);
		}

		if (Bgetc(buf) == '@') {
			node->right = NULL;

			int err = 0;
			err = CodeToNodes(buf, node, size);
			if (err != 0) {
				return err;
			}
		}
		else {
			Bseek(buf, -1, BSEEK_CUR);

			char valueS[100] = "";
			ReadToChar(valueS, buf, "{}");

			node_t* newNode = CreateNode();

			value_t value = {};
			StrToValue_t(valueS, &value);
			ChangeNodeValue(newNode, value);
			node->right = newNode;
			newNode->parent = node;
			(*size)++;

			int err = 0;
			err = CodeToNodes(buf, newNode, size);
			if (err != 0) {
				return err;
			}
		}
	}
	else if (curCh == '}') {

		if (node->parent != NULL) {
			int err = 0;
			err = CodeToNodes(buf, node->parent, size);
			if (err != 0) {
				return err;
			}
		}
	}
	else {
		return 1;
	}

	return 0;
}


/**
*	������� ������ �� ����
*
*	@param[in] code ���
*	@param[in] treeName ��� ������ (�� ��������� "tree_from_code")
*	@param[out] err ��� ������ (�� �������): 1 - ������������ ������;\
 2 - ������ � ����; 0 - ��� ������ ���������
*
*	@return ��������������� ������
*/

tree_t CodeToTree(char* code, const char* treeName, int* err) {
	assert(code != NULL);

	tree_t tree = TreeConstructor(treeName);

#ifdef _DEBUG
	if (TreeOk(&tree)) {
		PrintTree_OK(tree);
	}
	else {
		PrintTree_NOK(tree);
	}
#endif

	int constructErr = 0;
	buf_t codeBuf = BufConstructor('r', code, strlen(code), &constructErr);
	if (constructErr != 0) {
		*err = 1;
		return tree;
	}

	free(tree.root);
	tree.root = NULL;
	tree.size = 0;
	int retErr = CodeToNodes(&codeBuf, tree.root, &tree.size);
	
	if (retErr != 0) {
		if (err != NULL) {
			*err = 2;
		}
		return tree;
	}

#ifdef _DEBUG
	if (!TreeOk(&tree)) {
		if (err != NULL) {
			*err = 2;
		}
		return tree;
	}
#endif

	return tree;
}