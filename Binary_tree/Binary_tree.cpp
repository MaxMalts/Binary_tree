#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "btree.h"


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
	assert(tree != NULL);

	if (tree->size < 0) {
		tree->err = 1;
		return 0;
	}
	int actSize = 0;
	if (NodesOk(tree->root, &actSize) != 0) {
		tree->err = 2;
	}
	if (actSize != tree->size) {
		tree->err = 3;
		return 0;
	}

	return 1;
}
#endif


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
	fprintf(gvFile, "\t%d [label=\"%s\"]", (int)node, sValue);
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


/**
*	������� ����������� ������ � ��������� ���
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

	if (1 == CreateTreeImage(tree)) {
		return 1;
	}
	if (1 != system("tree.png")) {
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
*	@param node ����, � ������� ����������� ������� ����
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
*	@param subtree ���������, ������� ���������. ����� ������������� ����������������!
*	@param node ����, � �������� ���������
*	@param[in] side ������� ����, � ������� ���������
*
*	@return 1 - � ���� ��� ��� �������� ���� � ��������������� �������;\
 2 - �������� side ���� ������������ �������; 3, 4 - �� ���� ��������\
 ������ ��� �������������� ��������� � ������� (������ � ������ �������);\
 0 - ��� ������ ���������
*/	

int AddSubtree(tree_t* tree, tree_t* subtree, node_t* node, const int side) {
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
*	@param node ����, ��������� ��������� ������� � ��������� ����
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


/*   �� ��� ������������
*	������ � ��������
*/

struct buf_t {
	char* str = {};
	int cursor = 0;
};


/*  �� ��� ������������
*	����� ������ � ����� ������
*
*	@param buf 
*	@param[in] str 
*
*	@return 1 - �������� ��� �����������; 0 - ��� ������ ���������
*/

int Bufcat(buf_t* buf, const char* str) {
	assert(buf != NULL);
	assert(str != NULL);

	int ret = sprintf(&buf->str[buf->cursor], "%s", str);
	buf->cursor += strlen(str);
	buf->str[buf->cursor] = '\0';

	if (ret == 0) {
		return 1;
	}
	return 0;
}


/*  �� ��� ������������
*	������� ��� �� ����
*
*	@param[in] node ����
*	@param[out] buf �����
*
*	@return 0 - ��� ������ ���������
*/

int CodeFromNode(node_t* node, buf_t* buf) {
	assert(node != NULL);
	assert(buf != NULL);

	char* valueS = Value_tToStr(node->value);
	
	if (NodeChildsCount(node) > 0) {
		Bufcat(buf, valueS);
		Bufcat(buf, "{");
		if (node->left != NULL) {
			CodeFromNode(node->left, buf);
		}
		else {
			Bufcat(buf, "@");
		}
		Bufcat(buf, ",");
		if (node->right != NULL) {
			CodeFromNode(node->right, buf);
		}
		else {
			Bufcat(buf, "@");
		}
		Bufcat(buf, "}");
	}
	else {
		Bufcat(buf, "{");
		Bufcat(buf, valueS);
		Bufcat(buf, "}");
	}

	return 0;
}


/**
*	������� ��� �� ������
*
*	@param[in] tree ������
*	@param[out] str �����
*
*	@return 1 - �� ���� �������� ������ � ������� (������ � ������ �������);\
 0 - ��� ������ ���������
*/

int CodeFromTree(tree_t* tree, char* str) {
	assert(tree != NULL);
	assert(str != NULL);

#ifdef _DEBUG
	if (!TreeOk(tree)) {
		PrintTree_NOK(*tree);
		return 1;
	}
#endif

	buf_t buf = {};
	buf.str = str;
	
	Bufcat(&buf, "{");
	CodeFromNode(tree->root, &buf);
	Bufcat(&buf, "}");

	return 0;
}