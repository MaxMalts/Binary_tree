#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

	node_t* root = {};                       ///<Корень
	int size = 0;                            ///<Настоящий размер дерева

#ifdef _DEBUG
	char name[30] = "";                      ///<Имя дерева
	int err = 0;                             ///<Код ошибки, содержащейся в дереве:\n
											 ///0 - нет ошибок\n
#endif
};




int TreeDestructor(tree_t* tree);


/**
*	Проверяет, соответствует ли аргумент одной из сторон узла
*
*	@param[in] side Сторона
*
*	@return 1 (true) - соответствует; 0 (false) - не соответствует
*/

int SideIsCorrect(const int side) {
	if (side == LEFT_CHILD || side == RIGHT_CHILD) {
		return 1;
	}
	return 0;
}


/**
*	Считает количество дочерних узлов уровня на 1 ниже у узла
*
*	@param[in] node Узел
*
*	@return Количество дочерних узлов (0, 1, 2)
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
*	Копирует элемент типа value_t
*
*	@param[out] dest Куда копируем
*	@param[in] source Откуда копируем
*
*	@return 0 - все прошло нормально
*/

int CopyValue_t(value_t* dest, value_t* source) {
	assert(dest != NULL);
	assert(source != NULL);

	*dest = *source;

	return 0;
}

/**
*	Создает новое дерево. После вызова, измените значение корня\
 на нужное с помощью функции ChangeNodeValue()!
 *
 *	@param[in] name Имя дерева
 *
 *	@return Указатель на созданное дерево
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
*	Перезаписывает значение узла
*
*	@param node Узел
*	@param[in] value Значение
*
*	@return 0 - все прошло нормально
*/

int ChangeNodeValue(node_t* node, value_t value) {
	assert(node != NULL);

	CopyValue_t(&node->value, &value);

	return 0;
}

/**
*	Добавляет узел с указанной стороны по значению
*
*	@param tree Дерево, в котором находится узел
*	@param node Узел, к которому добавляем
*	@param[in] elem Элемент нового узла
*	@param[in] side Сторона, с которой добавляем (LEFT_CHILD, RIGHT_CHILD)
*
*	@return 1 - у узла уже был дочерний узел с соответствующей стороны;\
 2 - параметр side имел некорректное значние; 0 - все прошло нормально
*/

int AddChild(tree_t* tree, node_t* node, value_t elem, const int side) {
	assert(tree != NULL);
	assert(node != NULL);

	if (!SideIsCorrect) {
		return 2;
	}

	node_t* newNode = (node_t*)calloc(1, sizeof(node_t));
	newNode->value = elem;
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

	return 0;
}


///**
//*	Добавляет дочерний узел с указанной стороны по адресу
//*
//*	@param tree Дерево, в котором находится узел
//*	@param parentNode Узел, к которому добавляем
//*	@param[in] childNode Дочерний узел
//*	@param[in] side Сторона, с которой добавляем (LEFT_CHILD, RIGHT_CHILD)
//*
//*	@return 1 - у узла уже был дочерний узел с соответствующей стороны;\
// 2 - параметр side имел некорректное значние; 0 - все прошло нормально
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
*	Удаляет дочерний узел
*
*	@param tree Дерево, в котором находится узел
*	@param node Узел, в котором освобождаем удаляем узел
*	@param[in] side Сторона, с которой удаляем (LEFT_CHILD, RIGHT_CHILD)
*
*	@return 1 - не было дочернего узла; 2 - дочерний узел оказался поддеревом;\
 3 - параметр side имел некорректное значние; 0 - все прошло нормально
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
*	Добавляет поддерево в дерево по указанному узлу
*
*	@param tree Дерево, в которое добавляем
*	@param subtree Поддерево, которое добавляем
*	@param node Узел, к которому добавляем
*	@param[in] side Сторона узла, с которой добавляем
*
*	@return 1 - у узла уже был дочерний узел с соответствующей стороны;\
 2 - параметр side имел некорректное значние; 0 - все прошло нормально
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

	return 0;
}


/**
*	Удаляет все дочерние узлы и сам узел
*
*	@param Узел
*	@param[in] count Количество уже удаленных узлов\
 (при первичном вызове опустить, переменная только для рекурсивного вызова)
*
*	@return Количество удаленных узлов
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
*	Удаляет поддерево
*
*	@param tree Дерево
*	@param node Узел, поддерево удаляется начиная с дочернего узла
*	@param side Сторона дочернего узла
*
*	@return 1 - не было дочернего узла; 2 - параметр side имел некорректное значние;\
 0 - все прошло нормально
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
*	Пролностью удаляет дерево, включая все его узлы.\
 Внимение, если данное дерево является поддеревом в\
 другом дереве, не пользуйтесь этой функцией.
*
*	@param[in] tree Дерево
*
*	@return 0 - Все прошло нормально
*/

int DeleteTree(tree_t* tree) {
	assert(tree != NULL);

	DeleteNodes(tree->root);
	TreeDestructor(tree);

	return 0;
}


/**
*	Деконструктор дерева. Внимание, данная функция не удаляет узлы дерева!\
 Чтобы удалить узлы, воспользуйтесь функцией DeleteTree()!
*
*	@param tree Дерево
*
*	@return 0 - Все прошло нормально
*/

int TreeDestructor(tree_t* tree) {
	assert(tree != NULL);

	tree->root = NULL;
	tree->size = 0;

	return 0;
}