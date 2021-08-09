#include <stdlib.h> // malloc, atoi, rand
#include <stdio.h>
#include <assert.h> // assert
#include <time.h> // time

////////////////////////////////////////////////////////////////////////////////
// TREE type definition
typedef struct node
{
	int			data;
	struct node	*left;
	struct node	*right;
} NODE;

typedef struct
{
	NODE	*root;
} TREE; //헤드 구조, 헤드 노드, 트리의 루트 노드를 가르키는 포인터를 가지는 헤드 구조

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations
TREE *BST_Create( void);

/* Deletes all data in tree and recycles memory
*/
void BST_Destroy( TREE *pTree);

/* internal function (not mandatory)
*/
static void _destroy( NODE *root);

/* Inserts new data into the tree
   return	1 success
   0 overflow
   */
int BST_Insert( TREE *pTree, int data);

/* internal function (not mandatory)
*/
static void _insert( NODE *root, NODE *newPtr);

NODE *_makeNode( int data);

/* Deletes a node with dltKey from the tree
   return	1 success
   0 not found
   */
int BST_Delete( TREE *pTree, int dltKey);

/* internal function
   success is 1 if deleted; 0 if not
   return	pointer to root
   */
static NODE *_delete( NODE *root, int dltKey, int *success);

/* Retrieve tree for the node containing the requested key
   return	address of data of the node containing the key
   NULL not found
   */
int *BST_Retrieve( TREE *pTree, int key);

/* internal function
   Retrieve node containing the requested key
   return	address of the node containing the key
   NULL not found
   */
static NODE *_retrieve( NODE *root, int key);

/* prints tree using inorder traversal
*/
void BST_Traverse( TREE *pTree);
static void _traverse( NODE *root);

/* Print tree using inorder right-to-left traversal
*/
void printTree( TREE *pTree);
/* internal traversal function
*/
static void _infix_print( NODE *root, int level);

/* 
   return 1 if the tree is empty; 0 if not
   */
int BST_Empty( TREE *pTree);

/* Allocates dynamic memory for a tree head node and returns its address to caller
   return	head node pointer
   NULL if overflow
   */

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	TREE *tree;
	int data;

	// creates a null tree
	tree = BST_Create(); //빈트리를 만들어서 tree에 넘겨줌

	if (!tree)
	{
		printf( "Cannot create a tree!\n");
		return 100;
	}

	fprintf( stdout, "How many numbers will you insert into a BST: ");

	int numbers;
	scanf( "%d", &numbers);

	fprintf( stdout, "Inserting: ");

	srand( time(NULL));
	for (int i = 0; i < numbers; i++)
	{
		data = rand() % (numbers*3) + 1; // random number (1 ~ numbers * 3)

		fprintf( stdout, "%d ", data);

		// insert funtion call
		int ret = BST_Insert( tree, data);// 위에서 랜덤으로 선택된 난수를 트리에 삽입
		if (!ret) break;
	}
	fprintf( stdout, "\n");//for문이 끝나면 트리 완성

	// inorder traversal
	fprintf( stdout, "Inorder traversal: ");
	BST_Traverse( tree); //오름차순으로 트리 내부 순회
	fprintf( stdout, "\n");

	// print tree with right-to-left infix traversal
	fprintf( stdout, "Tree representation:\n");
	printTree(tree);

	while (1)
	{
		fprintf( stdout, "Input a number to delete: "); 
		int num;
		int ret = scanf( "%d", &num);
		if (ret != 1) break;//읽은 숫자가 숫자가 아닌 경우 무한루프 while문 종료

		ret = BST_Delete( tree, num);//읽은 숫자를 삭제 시도
		if (!ret)//삭제가 되지 않으면
		{
			fprintf( stdout, "%d not found\n", num);
			continue;
		}

		// print tree with right-to-left infix traversal
		fprintf( stdout, "Tree representation:\n");
		printTree(tree);//삭제가 잘 되면 변화된 트리 출력

		if (BST_Empty( tree)) //트리가 empty인 경우에 while문 끝남.
		{
			fprintf( stdout, "Empty tree!\n");
			break;
		}
	}

	BST_Destroy( tree); //메모리 해제

	return 0;
}

TREE *BST_Create( void) //빈 헤드노드를 가진 트리 만드는 것
{

	TREE* pTree = (TREE*)malloc(sizeof(TREE));

	if (pTree == NULL)
		return NULL;

	pTree->root = NULL;

	return pTree;

}

/* Deletes all data in tree and recycles memory
*/
void BST_Destroy( TREE *pTree)//트리를 완전히 메모리 해제, 노드들 따라다니면서 제거하고, 헤드구조도 제거
{
	if (pTree)//pTree가 null이 아니면
		_destroy(pTree->root);

	free(pTree);
}

/* internal function (not mandatory)
*/
static void _destroy( NODE *root) //BST_Destroy에서 호출하는 내부함수, 반드시 만들어야하는 것은 아님
{
	if(root!=NULL)
	{
		_destroy(root->left);
		_destroy(root->right);
		free(root);
	}

	else
		return;
}

/* Inserts new data into the tree
   return	1 success
   0 overflow
   */
int BST_Insert( TREE *pTree, int data)//삽입, 트리를 주고, data는 삽입시킬 데이터, 삽입 성공하면1 실패하면 0
{
	NODE* pNODE;
	pNODE=_makeNode(data);

	if(pNODE==NULL)
		return 0;

	else
	{
		if(pTree->root==NULL)
			pTree->root=pNODE;
		else
			_insert(pTree->root,pNODE);
		return 1;
	}
}

/* internal function (not mandatory)
*/
static void _insert( NODE *root, NODE *newPtr) //BST_Insert내부 함수, 노드포인터를 다룸. 반드시 만들어야 하는 것은 아님.
{
	if(newPtr->data < root->data)
	{
		if(root->left!=NULL)
			_insert(root->left,newPtr);
		else
			root->left=newPtr;
	}
	else
	{
		if(root->right!=NULL)
			_insert(root->right,newPtr);
		else
			root->right=newPtr;
	}

}

NODE *_makeNode( int data)//노드 하나 만드는 것.
{
	NODE* pNODE = (NODE*)malloc(sizeof(NODE));

	if (pNODE == NULL)
		return NULL;

	pNODE->data = data;
	pNODE->left = NULL;
	pNODE->right = NULL;

	return pNODE;
}

/* Deletes a node with dltKey from the tree
   return	1 success
   0 not found
   */
int BST_Delete( TREE *pTree, int dltKey)//트리를 주고 삭제할 키(dltKey)를 넘겨주고 삭제하는것, 성공하면1 실패하면 0
{
	int success;

	if(pTree->root==NULL)//빈트리이면
		return 0;
	pTree->root = _delete(pTree->root,dltKey,&success);
	return success;
}

/* internal function
   success is 1 if deleted; 0 if not
   return	pointer to root
   */
//BST_Delete 의 내부함수, root 노드 받아서, dltKey(삭제할 키), 삭제 성공하면 success에 1 넣어주고 실패하면 0 넣어주기
//트리의 루트가 바뀌는 경우 트리가 받아서 트리의 루트가 여기서 내주는 트리의 루트를 받아서 연결해줘야하기 때문에 노드 포인터 그렇지않으면 노드 구조의 이중 포인터를 잡아야
static int smallest(NODE*root){
	if(root->left==NULL)
		return root->data;
	return smallest(root->left);
}
static NODE *_delete( NODE *root, int dltKey, int *success)
{
	NODE *toReturn;
	if(!root){
		*success=0;
		return NULL;
	}
	if(root->data>dltKey){
		root->left = _delete(root->left,dltKey,success);
		return root;
	}
	if(root->data<dltKey){
		root->right = _delete(root->right,dltKey,success);
		return root;
	}
	*success=1;
	if(root->left==NULL){
		toReturn=root->right;
		free(root);
		return toReturn;
	}
	if(root->right==NULL){
		toReturn=root->left;
		free(root);
		return toReturn;
	}
	root->data=smallest(root->right);
	root->right=_delete(root->right,root->data,success);
	return root;
}
/*
   NODE* parentnode=NULL;
   NODE* pnode;

   pnode=root;

   while(pnode!=NULL&&(pnode->data)!=dltKey)//삭제할 노드 찾기
   {
   parentnode=pnode;

   if(dltKey<(pnode->data))
   {
   pnode=pnode->left;

   }
   else if(dltKey>(pnode->data))
   {
   pnode=pnode->right;
   }
   }

   if(parentnode==NULL){
 *success=0;
 return pnode;
 }
//pnode의 값이 삭제할 값과 같은 경우
if(pnode->left==NULL)//무자식, 왼쪽자식 없는 경우
{
if(parentnode->right==pnode)
{
parentnode->right=pnode->right;
 *success=1;
 return pnode;
 }
 else
 {
 parentnode->left=pnode->right;
 *success=1;
 return pnode;
 }

 }
 else if(pnode->right==NULL)//오른쪽 자식이 없는 경우
 {
 if(parentnode->left==pnode)
 {
 parentnode->left=pnode->left;
 *success=1;
 return pnode;
 }
 else
 {
 parent->right=pnode->left;
 success=1;
 return pnode;
 }
 }
 else//자식이 양쪽으로 있는 경우
 {
 NODE* smallest;
 smallest=findsmallest(pnode->right);
 pnode->data=smallest->data;
 return _delete( pnode->right, (smallest->data), &success);
 }
 }*/

/* Retrieve tree for the node containing the requested key
   return	address of data of the node containing the key
   NULL not found
   */
//int *BST_Retrieve( TREE *pTree, int key);//키 값 검색하는 함수, 메임함수에서 사용 안해서 구현 안해도됨!!!

/* internal function
   Retrieve node containing the requested key
   return	address of the node containing the key
   NULL not found
   */
//static NODE *_retrieve( NODE *root, int key); //*BST_Retrieve 내부함수

/* prints tree using inorder traversal
*/
void BST_Traverse( TREE *pTree) //inorder traversal, 숫자의 오름차순으로 내부 노드를 순회하는 함수
{
	_traverse(pTree->root);

	return;
}
static void _traverse( NODE *root)//BST_Traverse의 내부함수
{
	if (root != NULL)
	{
		_traverse(root->left);
		printf("%d ", root->data);
		_traverse(root->right);
	}
}
/* Print tree using inorder right-to-left traversal
*/
void printTree( TREE *pTree)//트리의 구조 화면에 찍어보는 것
{
	_infix_print(pTree->root, 0);

	return;
}
/* internal traversal function
*/
static void _infix_print( NODE *root, int level) // printTree 내부함수
{
	if (root == NULL)
		return;

	level++;

	_infix_print(root->right, level); //2,level=1

	printf("\n");

	for (int i = 1; i < level; i++)
		printf("\t");

	printf("%d\n", root->data);

	_infix_print(root->left, level);
}

/* 
   return 1 if the tree is empty; 0 if not
   */
int BST_Empty( TREE *pTree) //트리가 비어있는지, 비어있으면 1 리턴, 비어있지 않으면 0 리턴
{
	if(pTree->root==NULL)
		return 1;
	else
		return 0;
}



