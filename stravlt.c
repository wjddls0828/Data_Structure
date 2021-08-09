#define SHOW_STEP 0  // 데이터가 삽입되고 트리가 변경되는 과정을 보려면 1로, 0은 삽입 과정을 확인하지 않을때
#define BALANCING 1 //1이면 균형을 잡는것 0이면 균형을 안잡는것. 균형을 안잡으면 BST와 같은 것. _inset에서 사용됨
//-->이걸 우리가 함수에서도 써야 함. 
//이거 제출할 때는 0 1 로 두기

#include <stdlib.h> // malloc, rand
#include <stdio.h>
#include <time.h> // time
#include <string.h> //strcmp, strdup

#define max(x, y)	(((x) > (y)) ? (x) : (y)) //내 왼쪽 subtree의 height와 오른쪽과 뭐가더 큰지
//내 노드의 hegith 구하려면 왼쪽 서브트리의 hegiht와 오른쪽을 비교해서 더 큰 값에 1을 더해주면 내 노드의 height가 됨 

////////////////////////////////////////////////////////////////////////////////
// AVL_TREE type definition
typedef struct node
{
	char		*data; //문자열 -->크기 비교할때 strcmp 사용
	struct node	*left;
	struct node	*right;
	int			height; //균형 잡혀있는지 확인
} NODE;

typedef struct
{
	NODE	*root;
	int		count;  // number of nodes, 트리 안에 몇개 노드 있는지(노드 삽일할때, 이거 증가시키기)
} AVL_TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

AVL_TREE *AVL_Create( void);

/* Deletes all data in tree and recycles memory
*/
void AVL_Destroy( AVL_TREE *pTree);
static void _destroy( NODE *root);

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int AVL_Insert( AVL_TREE *pTree, char *data);

/* internal function
	This function uses recursion to insert the new data into a leaf node
	return	pointer to new root
*/
static NODE *_insert( NODE *root, NODE *newPtr);

static NODE *_makeNode( char *data);

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
char *AVL_Retrieve( AVL_TREE *pTree, char *key);

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE *_retrieve( NODE *root, char *key);

/* Prints tree using inorder traversal
*/
void AVL_Traverse( AVL_TREE *pTree);
static void _traverse( NODE *root);

/* Prints tree using inorder right-to-left traversal
*/
void printTree( AVL_TREE *pTree);
/* internal traversal function
*/
static void _infix_print( NODE *root, int level);

/* internal function
	return	height of the (sub)tree from the node (root)
*/
static int getHeight( NODE *root);

/* internal function
	Exchanges pointers to rotate the tree to the right
	updates heights of the nodes
	return	new root
*/
static NODE *rotateRight( NODE *root);

/* internal function
	Exchanges pointers to rotate the tree to the left
	updates heights of the nodes
	return	new root
*/
static NODE *rotateLeft( NODE *root);


/* Allocates dynamic memory for a AVL_TREE head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
AVL_TREE *AVL_Create( void) //빈 트리(트리의 헤드노드 만들어서 노드 포인터 리턴), root는 null, count=0
{
	AVL_TREE * pavl_tree=(AVL_TREE*)malloc(sizeof(AVL_TREE));
	
	if(pavl_tree)
	{
		pavl_tree->root=NULL;
		pavl_tree->count=0;
		return pavl_tree;
	}
	
	else
		return NULL;
	
	
}
/* Deletes all data in tree and recycles memory
*/
void AVL_Destroy( AVL_TREE *pTree) //트리의 모든 노드 지우고 노드가 가르키는 문자열도 삭제하고, 헤드구조체도 삭제.
{
	if (pTree)
		_destroy(pTree->root);

	free(pTree);
}
static void _destroy( NODE *root) //내부함수, AVL_Destroy
{
	if(root!=NULL)
	{
		_destroy(root->left);
		_destroy(root->right);
		free(root->data);
		free(root);
	}

	else
		return;
}
/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int AVL_Insert( AVL_TREE *pTree, char *data) //데이타 주면서, 안에서 _makeNode( char *data);사용 --> 그리고 실제 삽입은 _insert( NODE *root, NODE *newPtr);
{
	NODE* NODEPTR;
	
	NODEPTR=_makeNode(data);
	
	if(NODEPTR==NULL)
		return 0;
	
	if(pTree->root==NULL)
	{
		pTree->root=NODEPTR;
		pTree->count+=1;
		return 1;	
	}
	else
	{	
		pTree->root=_insert(pTree->root,NODEPTR);
		pTree->count+=1;
		return 1;
	}
}
/* internal function
	This function uses recursion to insert the new data into a leaf node
	return	pointer to new root
*/
static NODE *_insert( NODE *root, NODE *newPtr) //recursion으로 구현
{
	//뉴데이터가 작아서 왼쪽으로 이동할때
	if(strcmp(newPtr->data,root->data)<0)
	{
		if(root->left==NULL)
			root->left=newPtr;
		else
			root->left=_insert(root->left, newPtr);
	}	
	
	//뉴데이터와 같거나 커서 오른쪽으로 이동할때
	else
	{
		if(root->right==NULL)
			root->right=newPtr;
		else
			root->right=_insert(root->right,newPtr);
	}
	
	root->height=max(getHeight(root->left), getHeight(root->right))+1;
	
#if BALANCING==1
//삽입후 왼쪽이 더 크냐?
	if((getHeight(root->left)-getHeight(root->right))>1)//root가 LH
	{
	//subtree가 lh
		if(getHeight(root->left->left)-getHeight(root->left->right)>0)
			return rotateRight(root);
	//subtree가 rh
		else if(getHeight(root->left->left)-getHeight(root->left->right)<0)
		{
			root->left=rotateLeft(root->left);
			return rotateRight(root);
		}	
	}
	
	//삽입 후 오른쪾이 더 크냐?
	if((getHeight(root->left)-getHeight(root->right))<-1)
	{
	//right of right
		if(getHeight(root->right->left)-getHeight(root->right->right)<0)	
			return rotateLeft(root);
	//left of right
		else if(getHeight(root->right->left)-getHeight(root->right->right)>0)
		{
			root->right=rotateRight(root->right);
			return rotateLeft(root);
		}
	
	}
#endif
	return root;
}

static NODE *_makeNode( char *data) //노드 만들기, 왼쪽 오른쪽 포인터는 null, 데이터는 *data를 그대로 복사(strdup), 노드의 height도 1로 초기화(리프 노드에 새로운 노드가 삽입되니까)
{
	NODE* pnode=(NODE*)malloc(sizeof(NODE));
	
	if(pnode)
	{
		pnode->data=strdup(data);
		pnode->left=NULL;
		pnode->right=NULL;
		pnode->height=1;
		return pnode;
	}
	
	else
		return NULL;
}
/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
char *AVL_Retrieve( AVL_TREE *pTree, char *key) //문자열을 key로 받아서 해당 트리의 key가 있는지, 있으면 해당 문자열의 주소 리턴 없으면 null
{
	NODE* tempnode;
	if(!(pTree->root))
		return NULL;
	else
	{
		tempnode=_retrieve(pTree->root,key);
		if(tempnode==NULL)
			return NULL;
		return tempnode->data;
	}
}
/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE *_retrieve( NODE *root, char *key) //내부함수
{
	if(!root)
		return NULL;
	else
	{
		if(strcmp(key,root->data)>0)
			return _retrieve(root->right,key);
		
		else if(strcmp(key,root->data)<0)
			return _retrieve(root->left,key);
		
		else
			return root;
	}
}

/* Prints tree using inorder traversal
*/
void AVL_Traverse( AVL_TREE *pTree)//저번이랑 같음
{
	_traverse(pTree->root);

	return;
}
static void _traverse( NODE *root)
{
	if (root != NULL)
	{
		_traverse(root->left);
		printf("%s ", root->data);
		_traverse(root->right);
	}
}

/* Prints tree using inorder right-to-left traversal
*/
void printTree( AVL_TREE *pTree)
{
	_infix_print(pTree->root, 0);

	return;
}

/* internal traversal function
*/
static void _infix_print( NODE *root, int level)
{
	if (root == NULL)
		return;

	level++;

	_infix_print(root->right, level); //2,level=1

	printf("\n");

	for (int i = 1; i < level; i++)
		printf("\t");

	printf("%s\n", root->data);

	_infix_print(root->left, level);
}

/* internal function
	return	height of the (sub)tree from the node (root)
*/
static int getHeight( NODE *root) //노드의 높이를 알아내는 함수, null이면 0리턴 아니면 노드 안의 height라는 변수의 값 그냥 리턴
{
	if(root==NULL)
		return 0;
	else 
		return root->height;
}
/* internal function
	Exchanges pointers to rotate the tree to the right
	updates heights of the nodes
	return	new root
*/
static NODE *rotateRight( NODE *root)
{
	NODE* tempnodeptr;
	
	//포인터 연결
	tempnodeptr=root->left;
	root->left=tempnodeptr->right;
	tempnodeptr->right=root;
	
	
	//height 업데이트
	(root->height)=max(getHeight(root->left), getHeight(root->right))+1;//이것도 아래처럼 수정하기!!
	(tempnodeptr->height)=max(getHeight(tempnodeptr->left), getHeight(tempnodeptr->right))+1;
	
	//root 바꾸기
	root=tempnodeptr;
	
	return root;
	
}

/* internal function
	Exchanges pointers to rotate the tree to the left
	updates heights of the nodes
	return	new root
*/
static NODE *rotateLeft( NODE *root) //포인팅 연산이랑 height 계산을 다시해야 함(부모로 있던 노드가 옮겨가고 내 왼쪽 오른쪽 자신이 부모 자리로 올라가서 높이 변경됨)
{
	NODE* tempnodeptr2;
	
	//포인터 연결
	tempnodeptr2=root->right;
	root->right=tempnodeptr2->left;
	tempnodeptr2->left=root;
	
	//height 업데이트
	(root->height)=max(getHeight(root->left), getHeight(root->right))+1;
	(tempnodeptr2->height)=max(getHeight(tempnodeptr2->left), getHeight(tempnodeptr2->right))+1;
	
	//root 바꾸기
	root=tempnodeptr2;
	
	return root;
	
}
//왼쪽이나 오른쪽 중 더 높이가 높은 height 에 1을 더하면 새로운 height가 계산됨.
////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	AVL_TREE *tree;
	char str[1024];
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]); //트리에 삽입하고자하는 문자열들을 저장해 둔 파일을 받음
		return 0;
	}
	
	// creates a null tree
	tree = AVL_Create(); //빈트리 만들기
	
	if (!tree) //빈트리 못만들면 에러 리턴
	{
		fprintf( stderr, "Cannot create tree!\n");
		return 100;
	} 

	FILE *fp = fopen( argv[1], "rt"); //입력 파일 읽기
	if (fp == NULL) //파일 못열면 에러 리턴
	{
		fprintf( stderr, "Cannot open file! [%s]\n", argv[1]);
		return 200;
	}

	while(fscanf( fp, "%s", str) != EOF) //연 파일을 이용해서 문자열 하나씩 읽기
	{

#if SHOW_STEP
		fprintf( stdout, "Insert %s>\n", str); 
#endif		
		// insert function call
		AVL_Insert( tree, str); //str을 tree에 삽입

#if SHOW_STEP
		fprintf( stdout, "Tree representation:\n");
		printTree( tree);
#endif
	}
	
	fclose( fp);
	
#if SHOW_STEP
	fprintf( stdout, "\n");

	// inorder traversal
	fprintf( stdout, "Inorder traversal: "); //트리 안의 모든 데이터를 순회하면서 출력(알파벳순으로)
	AVL_Traverse( tree);
	fprintf( stdout, "\n");

	// print tree with right-to-left infix traversal
	fprintf( stdout, "Tree representation:\n"); //트리표현 , 해당 노드의 레벨에 따라서 탭 입력해서 트리의 구조 보는 것
	printTree(tree); //문자열 출력해야 하나ㅣ까 저번에 썻던 것에서 %d -->%s로 바꾸기
#endif
	fprintf( stdout, "Height of tree: %d\n", tree->root->height); //트리의 전체적인 높이
	fprintf( stdout, "# of nodes: %d\n", tree->count); //트리 안에 몇개의 노드가 있는지
	
	// retrieval
	char *key;
	fprintf( stdout, "Query: ");
	while( fscanf( stdin, "%s", str) != EOF) //ctrl+d로 끝내기.
	{
		key = AVL_Retrieve( tree, str); //트리 안의 것을 검색 str이 있는지 없는지
		
		if (key) fprintf( stdout, "%s found!\n", key); //key가 null이 아니면
		else fprintf( stdout, "%s NOT found!\n", str); //key값 못찾았으면, 못찾았으면 key가 null이니까 str로 출력
		
		fprintf( stdout, "Query: ");
	}
	
	AVL_Destroy( tree);

	return 0;
}

