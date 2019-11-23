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
*	Считает количество сыновей
*
*	@param[in] node Узел
*
*	@return Количество сыновей (0, 1, 2)
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

int CopyValue_t(value_t* dest, value_t source) {
	assert(dest != NULL);

	*dest = source;

	return 0;
}


/**
*	Преобразует value_t в строку
*
*	@param[in] value Элемент типа value_t
*
*	@return Указатель на строку. Не забудьте освободить память по этому указателю!
*/

char* Value_tToStr(const value_t value) {

	const int value_tMaxStrSize = 20;

	char* str = (char*)calloc(value_tMaxStrSize + 1, sizeof(char));
	itoa(value, str, 10);

	return str;
}


/**
*	Преобразует строку в value_t
*
*	@param[in] valueS Строка
*	@param[out] value Значение value_t
*
*	@return 1 - в строке неверное представление value_t; 0 - все прошло нормально
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


/**
*	Сравнивает значения value_t
*
*	@param[in] value1 Первое значение
*	@param[in] value2 Второе значение
*
*	@return < 0 - первое значение меньше второго; 0 - значения равны; > 0 - первое значение больше второго
*/

int ValueCmp(const value_t value1, const value_t value2) {

	int sub = value1 - value2;
	if (sub < 0) {
		return -1;
	}
	if (sub == 0) {
		return 0;
	}
	if (sub > 0) {
		return 1;
	}
}


/*  Не для пользователя
*	Проверяет узлы ниже заданного и считает их количество
*
*	@param[in] node Узел
*	@param[out] NNodes Количество узлов. На входе значение должно быть 0!
*
*	@return 1 - ошибка есть; 0 - ошибок нет
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
*	Проверяет дерево и записывает в него код ошибки
*
*	@param[in] tree Дерево
*
*	@return 0 - дерево некорректное; 1 - дерево корректное
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


/*  Не для пользователя
*	Выводит информацию о дереве. Для визуализации дерева, используйте ShowTree().
*
*	@param[in] tree Дерево
*	@param[in] file Название файла, окуда вызвали функцию
*	@param[in] line Номер строки, из которой вызвали функцию
*	@param[in] function Имя функции, из которой вызвали функцию
*	@param[in] reason Причина, по которой вызвали функцию
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


/*  Не для пользователя
*	Рекурсивно записывает информацию обо всех узлах для программы Graphviz.\
 Информация, написанная этой функцией не является полноценным оформлением дерева.
*
*	@param[in] gvFile Файл для записи
*	@param[in] node Корневой узел
*
*	@return 0 - все прошло нормально
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
*	Рисует дерево с помощью программы Graphviz.
*
*	@param[in] tree Дерево
*	@param[in] foutName Имя файла с картнкой (по умолчанию - "tree.png")
*	@param[in] gvFileName Имя файла с описанием узлов для Graphviz (по умолчанию - "tree.gv")
*
*	@return 1 - не удалось открыть файл для записи; 2 - на вход подалось дерево\
с ошибкой (только в режиме отладки); 0 - все прошло нормально
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
*	Создает изображение дерева и открывает его. Функция медленная, не используйте часто!
*
*	@param[in] tree Дерево
*
*	@return 1 - Проблема при создании изображения; 2 - проблема при открытии изображения;\
 3 - на вход подалось дерево с ошибкой (только в режиме отладки); 0 - все прошло нормально
*/

int ShowTree(tree_t* tree) {
	assert(tree != NULL);

#ifdef _DEBUG
	if (!TreeOk(tree)) {
		PrintTree_NOK(*tree);
		return 3;
	}
#endif

	Sleep(700);   //Если этого не сделать, то предыдущее изображение открывается как новое.

	if (CreateTreeImage(tree) == 1) {
		return 1;
	}
	if (system("tree.png") != 0) {
		return 2;
	}
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


/*  Не для пользователя
*	Считает количество узлов-потомков с учетом входного узла (рекурсивно)
*
*	@param[in] node Узел
*	@param[out] size Количество
*
*	@return 0 - все прошло нормально
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


/*  Не для пользователя
*	Пересчитывает размер дерева
*
*	@param tree Дерево
*
*	@return См. возвращаемое значение CalcNodesCount
*/

int RecalcTreeSize(tree_t* tree) {
	assert(tree != NULL);
	assert(tree->root != NULL);

	return CalcNodesCount(tree->root, &tree->size);
}

/*  Не для пользователя
*	Создает новое дерево. После вызова, измените значение корня\
 на нужное с помощью функции ChangeNodeValue()!
 *
 *	@param[in] name Имя дерева
 *
 *	@return Указатель на созданное дерево
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
*	Перезаписывает значение узла
*
*	@param node Узел
*	@param[in] value Значение
*
*	@return 0 - все прошло нормально
*/

int ChangeNodeValue(node_t* node, value_t value) {
	assert(node != NULL);

	CopyValue_t(&node->value, value);

	return 0;
}


/*  Не для пользователя
*	Создает новый узел
*
*	@return Указатель на новый узел. Не забудьте освободить память по этому указателю!
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
*	Добавляет узел с указанной стороны по значению
*
*	@param tree Дерево, в котором находится узел
*	@param node Узел, к которому добавляем. Должен быть частью дерева, иначе поведение не определено.
*	@param[in] value Значение нового узла
*	@param[in] side Сторона, с которой добавляем (LEFT_CHILD, RIGHT_CHILD)
*	@param[out] newNode Адрес нового узла
*
*	@return 1 - у узла уже был дочерний узел с соответствующей стороны;\
 2 - параметр side имел некорректное значние; 3 - на вход подалось дерево с\
  ошибкой (только в режиме отладки); 0 - все прошло нормально
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
*	@param node Узел, в котором освобождаем удаляем узел.\
 Должен быть частью дерева, иначе в дереве возникнет ошибка.
*	@param[in] side Сторона, с которой удаляем (LEFT_CHILD, RIGHT_CHILD)
*
*	@return 1 - не было дочернего узла; 2 - дочерний узел оказался поддеревом;\
 3 - параметр side имел некорректное значние; 4 - на вход подалось дерево с\
 ошибкой (только в режиме отладки); 0 - все прошло нормально
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
*	Добавляет поддерево в дерево по указанному узлу.
*
*	@param tree Дерево, в которое добавляем
*	@param node Узел, к которому добавляем. Должен быть частью дерева, иначе поведение не определено.
*	@param subtree Поддерево, которое добавляем. После присоединения деконструируется!
*	@param[in] side Сторона узла, с которой добавляем
*
*	@return 1 - у узла уже был дочерний узел с соответствующей стороны;\
 2 - параметр side имел некорректное значние; 3, 4 - на вход подалось\
 дерево или соответственно поддерево с ошибкой (только в режиме отладки);\
 0 - все прошло нормально
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


/*  Не для пользователя
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
*	@param node Узел, поддерево удаляется начиная с дочернего узла.\
 Должен быть частью дерева, иначе поведение не определено.
*	@param side Сторона дочернего узла
*
*	@return 1 - не было дочернего узла; 2 - параметр side имел некорректное значние;\
 3 - на вход подалось дерево с ошибкой (только в режиме отладки); 0 - все прошло нормально
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
*	Разделяет дерево путем создания нового дерева - поддерева исходного
*
*	@param tree Исходное дерево
*	@param[in] node Узел, дочерний узел которого будет корнем нового дерева.\
 Должен быть частью дерева, иначе поведение не определено.
*	@param[in] side Сторона узла
*	@param[in] name Имя нового дерева
*	@param[out] err Ошибка: 1 - на вход подалось некорректное дерево;\
 2 - некорректная сторона; 0 - все прошло нормально
*
*	@return Полученное поддерево
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


/*  Не для пользователя
*	Создает код по узлу
*
*	@param[in] node Узел
*	@param[out] codeBuf Буфер с кодом
*
*	@return 0 - все прошло нормально
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
*	Создает код по дереву
*
*	@param[in] tree Дерево
*	@param[out] size Длина полученного кода (без учета '\0').\
 Если значение отрицательное, то возникла следующая ошибка:\
 -1 - на вход подалось дерево с ошибкой (только в режиме отладки);\
 -2 -проблема при создании буфера; 0 - все прошло нормально
*
*	@return Указатель на буфер с кодом. В случае ошибки равен NULL.\
 Не забудьте освободить память по этому указателю!
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
*	Рекурсивно создает узлы по коду
*
*	@param[in] buf Буфер с кодом
*	@param[out] node Текущий узел. Внимание, при первичном вызове\
 должен быть NULL, сюда запишется корневой узел!
*	@param[out] size
*
*	@return 1 - ошибка в коде; 0 - все прошло нормально
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
*	Создает дерево по коду
*
*	@param[in] code Код
*	@param[in] treeName Имя дерева (по умолчанию "tree_from_code")
*	@param[out] err Код ошибки (по желанию): 1 - произвольная ошибка;\
 2 - ошибка в коде; 0 - все прошло нормально
*
*	@return Сгенерированное дерево
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


/*  Не для пользователя
*	Ищет узел с указанным значением (рекурсивно)
*
*	@param[in] curNode Текущий узел (при первичном вызове должен быть корнем дерева!)
*	@param[in] value Значение
*	@param[out] way Путь до найденного узла (если нашелся) в формате строки из '0' и '1',\
 где '0' означает левого сына, '1' - правого. В конце строки ставится '\0'
*
*	@return 1 (true) - узел найдет; 0 (false) - узел не найден
*/

int NodeByValue(node_t* curNode, const value_t* value, buf_t* way, node_t*& foundNode) {
	assert(curNode != NULL);
	assert(value != NULL);
	assert(way != NULL);

	if (ValueCmp(curNode->value, *value) == 0) {
		Bputc(way, '\0');
		foundNode = curNode;
		return 1;
	}
	if (NodeChildsCount(curNode) == 0) {
		Bseek(way, -1, SEEK_CUR);
		Bputc(way, '\0');
		Bseek(way, -1, SEEK_CUR);
		return 0;
	}

	if (curNode->left != NULL) {
		Bputc(way, '0');
		if (NodeByValue(curNode->left, value, way, foundNode) == 1) {
			return 1;
		}
	}
	if (curNode->right != NULL) {
		Bputc(way, '1');
		if (NodeByValue(curNode->right, value, way, foundNode) == 1) {
			return 1;
		}
	}
	return 0;
}


/**
*	Ищет узел с указанным значением
*
*	@param[in] tree Дерево
*	@param[in] value Значение
*	@param[out] foundNode Найденный узел. Должен быть NULL при вызове.
*	@param[out] err Ошибка: 1 - произвольная ошибка; 2 - узел с таким значением не найден
*
*	@return Указатель на строку с путем до найденного узла (если нашелся) в виде '0' и '1',\
 где '0' означает левого сына, '1' - правого. В конце строки ставится '\0'.\
 Не забудьте освободить память по этому указателю! Если возникла ошибка, возвращает NULL.
*/

char* FindNodeByValue(tree_t* tree, const value_t value, node_t*& foundNode, int* err) {
	assert(tree != NULL);
	assert(foundNode == NULL);

	int constrErr = 0;
	buf_t way = BufConstructor('w', &constrErr);
	if (constrErr != 0) {
		if (err != NULL) {
			*err = 1;
		}
		return NULL;
	}

	if (!NodeByValue(tree->root, &value, &way, foundNode)) {
		if (err != NULL) {
			*err = 2;
		}
		free(way.str);
		BufDestructor(&way);
		return NULL;
	}

	char* resWay = way.str;
	BufDestructor(&way);
	if (err != NULL) {
		*err = 0;
	}
	return resWay;
}