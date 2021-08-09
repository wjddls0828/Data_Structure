#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h> // malloc, atoi
#include <stdio.h>
#include <ctype.h> // isdigit
#include <assert.h> // assert
#include <string.h>

#define MAX_STACK_SIZE	50

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	char		data;
	struct node* left;
	struct node* right;
} NODE;//실제 데이터 들어가는 부분

typedef struct
{
	NODE* root;
} TREE; // 헤드구조체

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE* createTree(void)//트리 구조체, 비어있는 TREE 만들기
{
	TREE* pTree = (TREE*)malloc(sizeof(TREE));

	if (pTree == NULL)
		return NULL;

	pTree->root = NULL;

	return pTree;
}

/* Deletes all data in tree and recycles memory
*/
void destroyTree(TREE* pTree);//트리제거, TREE가 가르키는 것 다 순회하면서 노드 삭제하고 나중에 TREE도 제거

static void _destroy(NODE* root)//destroyTree가 내부적으로 사용, 루트부터 시작해서 노드를 따라다니며 제거
{
	if(root!=NULL)
	{
		_destroy(root->left);
		_destroy(root->right);
		free(root);
	}
	else
		return;
	/*if(root->left!=NULL)
	{
		_destroy(root->left);
	}
	
	if(root->right!=NULL)
	{
		_destroy(root->right);
	}
		
	free(root);*/
}

/*  Allocates dynamic memory for a node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
static NODE* _makeNode(char ch)//노드 만들기, 메모리 할당하고 그 주소를 리턴
{
	NODE* pNODE = (NODE*)malloc(sizeof(NODE));

	if (pNODE == NULL)
		return NULL;

	pNODE->data = ch;
	pNODE->left = NULL;
	pNODE->right = NULL;

	return pNODE;

}

typedef struct arraystack
{
	NODE* stackarr[MAX_STACK_SIZE];
	int topIndex;
}ARRAYSTACK;


static ARRAYSTACK* _makearraystack(void)
{
	ARRAYSTACK* pSTACK = (ARRAYSTACK*)malloc(sizeof(ARRAYSTACK));

	if (pSTACK == NULL)
		return NULL;

	pSTACK->topIndex = -1;
	
	for(int i=0;i<MAX_STACK_SIZE;i++)
	{
		pSTACK->stackarr[i]=NULL;
	}

	return pSTACK;
}

void destroySTACK(ARRAYSTACK *pstack)
{
	for(int i=0;i<=pstack->topIndex;i++)
	{
		_destroy(pstack->stackarr[i]);
	}
	
	free(pstack);
	
}


/* converts postfix expression to binary tree
	return	1 success
			0 invalid postfix expression
*/
int postfix2tree(char* expr, TREE* pTree)//후외표기식으로 작성된 expression을 expression tree로 변환, exptr=입력, pTree=그 결과를 여기에 저장
{
	int i;
	ARRAYSTACK* pSTACK;
	NODE* pnode;
	int k=0;

	pSTACK = _makearraystack();

	for (i = 0; i < (strlen(expr)); i++)
	{

		if (isdigit(expr[i]))//피연산자라면
		{
			pnode = _makeNode(expr[i]);
			pSTACK->topIndex += 1;
			pSTACK->stackarr[pSTACK->topIndex] = pnode;//스택에 넣기
			pTree->root = pnode;
		}
	
		else if(expr[i]=='/'||expr[i]=='*'||expr[i]=='+'||expr[i]=='-')//연산자라면
		{
			pnode = _makeNode(expr[i]);
			
			if (pSTACK->topIndex == -1)
			{
				_destroy(pnode);
				free(pSTACK);
				pTree->root=NULL;
				return 0;
			}

			pnode->right = pSTACK->stackarr[pSTACK->topIndex]; //오른쪽에 나중에 들어간 피연산자 연결
			pSTACK->stackarr[pSTACK->topIndex]=	NULL;
			pSTACK->topIndex -= 1; //topindex 감소시키기
			
			

			if (pSTACK->topIndex == -1)
			{
				_destroy(pnode);
				free(pSTACK);
				pTree->root=NULL;
				return 0;
			}

			pnode->left = pSTACK->stackarr[pSTACK->topIndex]; //왼쪽에 처음에 들어간 피연산자 연결
			pSTACK->stackarr[pSTACK->topIndex]=	NULL;
			pSTACK->topIndex -= 1; //topindex 1 감소시키기

			pSTACK->topIndex += 1;//데이터 추가 위한 인덱스 증가
			pTree->root = pnode;
			pSTACK->stackarr[pSTACK->topIndex] = pTree->root;//루트의 주소(연산자)의 주소값을 push


		}
		
		else
		{
			free(pSTACK);
			return 0;
		}
	}

	if (pSTACK->topIndex == 0)//수식이 제대로 되면
	{
		free(pSTACK);
		return 1;
	}

	else//끝났는데 top 위치가 0이 아님
	{
		pTree->root=NULL;
		destroySTACK(pSTACK);
		return 0;
	}

}

/* Print node in tree using inorder traversal
*/
void traverseTree(TREE* pTree);//화면에 트리 안의 내용을 순회하면서 출력, inorder 중위 표기식의 수식을 중위식으로, 괄호를 중간중간 넣으면서 수식 표현

/* internal traversal function
	an implementation of ALGORITHM 6-6
*/
static void _traverse(NODE* root)//강의자료 알고리즘 6-6 구현
{
	if (root != NULL)
	{
		if (isdigit(root->data))//피연산자라면
			printf("%c", root->data);
		else
		{
			printf("(");

			_traverse(root->left);
			printf("%c", root->data);
			_traverse(root->right);

			printf(")");
		}
	}
}

/* Print tree using inorder right-to-left traversal
*/
void printTree(TREE* pTree);//트리의 모양, inorder traversal인데 오른쪽에서 왼쪽으로 순회

/* internal traversal function
*/
static void _infix_print(NODE* root, int level)//printTree의 내부에서 사용, level=내 깊이에 따라 탭 입력해서 노드의 데이터 값 출력, 처음 level은 0
{
	if (root == NULL)
		return;

	level++;

	_infix_print(root->right, level); //2,level=1

	printf("\n");

	for (int i = 1; i < level; i++)
		printf("\t");

	printf("%c\n", root->data);

	_infix_print(root->left, level);
}

typedef struct arraystackforeval
{
	float stackarrforeval[MAX_STACK_SIZE];
	int topIndex;

}ARRAYSTACKEVAL;


static ARRAYSTACKEVAL* _makearraystackforeval(void)
{
	ARRAYSTACKEVAL* pstack = (ARRAYSTACKEVAL*)malloc(sizeof(ARRAYSTACKEVAL));

	if (pstack == NULL)
		return NULL;

	pstack->topIndex = -1;
	//pSTACK->(&stackarr)=NULL;

	return pstack;
}

/* evaluate postfix expression
	return	value of expression
*/
float evalPostfix(char* expr)//후외 표기식의 실제 값을 계산해서 float 값으로 내주는 것
{
	ARRAYSTACKEVAL* pSTACK;
	NODE* pnode;

	
	int i;
	float RESULT;
	float n1,n2;
	char tn1, tn2;

	pSTACK = _makearraystackforeval();

	for (i = 0; i < strlen(expr); i++)
	{
		if (isdigit(expr[i])) //피연산자라면
		{
			pSTACK->topIndex += 1;
			pSTACK->stackarrforeval[pSTACK->topIndex] = expr[i]-'0';
		}
		else//피연산자가 아니라면
		{
			n2 = pSTACK->stackarrforeval[pSTACK->topIndex];
		
			pSTACK->topIndex -= 1;

			n1 = pSTACK->stackarrforeval[pSTACK->topIndex];
			
			pSTACK->topIndex -= 1;

			switch (expr[i])
			{
			case '+':
				pSTACK->topIndex += 1;
				pSTACK->stackarrforeval[pSTACK->topIndex]= (n1 + n2);
				break;

			case '-':
				pSTACK->topIndex += 1;
				pSTACK->stackarrforeval[pSTACK->topIndex] = (n1 - n2);
				break;

			case '/':
				pSTACK->topIndex += 1;
				pSTACK->stackarrforeval[pSTACK->topIndex]=(n1 / n2);
				break;

			case '*':
				pSTACK->topIndex += 1;
				pSTACK->stackarrforeval[pSTACK->topIndex] = (n1 * n2);
				break;
			}
		}
	}
	
	RESULT=pSTACK->stackarrforeval[0];
	
	free(pSTACK);
	
	return RESULT;
}

////////////////////////////////////////////////////////////////////////////////
void destroyTree(TREE* pTree)
{
	if (pTree)//pTree가 null이 아니면
	{
		_destroy(pTree->root);
	}

	free(pTree);
}

////////////////////////////////////////////////////////////////////////////////
void printTree(TREE* pTree)
{
	_infix_print(pTree->root, 0);

	return;
}

////////////////////////////////////////////////////////////////////////////////
void traverseTree(TREE* pTree)
{
	_traverse(pTree->root);

	return;
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	TREE* tree;
	char expr[1024];

	fprintf(stdout, "\nInput an expression (postfix): ");//후위표기식으로 수식을 입력하라

	while (fscanf(stdin, "%s", expr) == 1)//문자열을 입력하면 expr에 저장됨, 컨트롤 d 키 누를 때 까지 반복
	{
		// creates a null tree
		tree = createTree(); //빈트리를 만든다

		if (!tree) //빈트리 못만들면
		{
			printf("Cannot create tree\n");
			return 100;
		}

		// postfix expression -> expression tree
		int ret = postfix2tree(expr, tree); //사용자가 입력한 수식을 트리에 변환해서 저장, 이 함수의 리턴값이 0이면 실패, 1이면 성공

		if (!ret) //수식이 후위표기식으로 제대로 작성되지 않는 경우
		{
			fprintf(stdout, "invalid expression!\n");
			destroyTree(tree); //헤드 구조만 삭제, 잘못된 수식이면 중간에 노드를 만들다가 제대로된 트리를 못만들기 때문에 중간만들다 만 노드들을 메모리 해제해야 함.
			continue;
		}

		// expression tree -> infix expression 
		fprintf(stdout, "\nInfix expression : ");
		traverseTree(tree); //표현을 중위표기식으로 찍어봐라

		// print tree with right-to-left infix traversal
		fprintf(stdout, "\n\nTree representation:\n");
		printTree(tree); //트리 자체를 트리 형태를 찍어봐라, inorder가 맞기는 한데 오른쪽에서 왼쪽으로 infix traversal, 좌측으로 90도 고개를 돌려서 보면 트리의 형태가 이해하기 쉬운 형태가 됨

		// evaluate postfix expression
		float val = evalPostfix(expr); //수식자체를 받아서 그 결과를 계산해서 리턴해주는 것
		fprintf(stdout, "\nValue = %f\n", val); //수식의 값이 얼마라고 출력해주는 것

		// destroy tree
		destroyTree(tree);

		fprintf(stdout, "\nInput an expression (postfix): ");
	}
	return 0;
}
