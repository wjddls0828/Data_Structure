#include <stdlib.h> // malloc
#include <stdio.h>
#include <string.h> // strdup, strcmp
#include <ctype.h> // toupper

#define QUIT			1
#define FORWARD_PRINT	2
#define BACKWARD_PRINT	3
#define SEARCH			4
#define DELETE			5
#define COUNT			6

// User structure type definition
typedef struct 
{
	char	*token; //사람 이름, 배열로 선언한 것이 아니라 캐릭터 포인터, heap에 저장되는 문자열 가르키는 것, ordered list 이름순으로, 이미 있는 이름이면 빈도 정보만 수정,
	int		freq; //빈도 정보
} tTOKEN;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tTOKEN		*dataPtr; //데이터 가르키는 포인터
	struct node	*llink; //backword
	struct node	*rlink;//foreward
} NODE;

typedef struct
{
	int		count; //노드 개수
	NODE	*pos; // unused, 여기서는 사용 안함
	NODE	*head; //리스트의 제일 앞 노드 가르키는 포인터
	NODE	*rear;//리스트의 제일 뒤 노드 가르키는 포인터
} LIST; //리스트의 헤드 구조

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations
LIST *createList( void);
void destroyList( LIST *pList);
int addNode( LIST *pList, tTOKEN *dataInPtr);
int removeNode( LIST *pList, char *keyPtr, tTOKEN **dataOut);
int searchList( LIST *pList, char *pArgu, tTOKEN **pDataOut);
int countList( LIST *pList);
int emptyList( LIST *pList);
void printList( LIST *pList);
void printListR( LIST *pList);
static int _insert( LIST *pList, NODE *pPre, tTOKEN *dataInPtr);
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr);
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, char *pArgu);
tTOKEN *createToken( char *str);
void destroyToken( tTOKEN *pToken);
int get_action();


/* Allocates dynamic memory for a list head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
LIST *createList( void) //빈 리스트 만들기
{
	LIST* plist=(LIST*)malloc(sizeof(LIST));
	
	if(plist==NULL)
		return NULL;
	
	plist->count=0;
	plist->head=NULL;
	plist->rear=NULL;
	plist->pos=NULL;
	
	return plist;
} 

/* Deletes all data in list and recycles memory
*/
void destroyList( LIST *pList) //리스트 전체 앞의 노드부터 토큰 자료구조, 연결된 스트링, 해당 노드 다 지우기, 노드 다 사라질떄까지, 리스트 헤드구조까지 지우기
{
	if(pList==NULL)
		return;
	
	NODE* pPre;
	NODE* pLoc;
	
	pPre=NULL;
	pLoc=pList->head;
		
	while(pLoc!=NULL)
	{
		pPre=pLoc;
		pLoc=pLoc->rlink;
		
		destroyToken(pPre->dataPtr);
		free(pPre);
		
		//ptoken=pLoc->dataPtr;
	}
	free(pList);
	
	
} //이미 있는 리스트 완전히 제거

/* Inserts data into list
	return	0 if overflow 메모리 할당 안되서 추가 불가
			1 if successful 삽입 성공
			2 if duplicated key 이미 있는 이름을 다시 추가하라고 한 경우, 빈도값만 갱신
*/
int addNode( LIST *pList, tTOKEN *dataInPtr)
{
	NODE* pPre = NULL;
	NODE* pLoc= pList->head;
	
	int searchfound;
	
	searchfound=_search(pList, &pPre, &pLoc, dataInPtr->token);
	if(searchfound==0)	
		return _insert(pList, pPre, dataInPtr);
	else
	{	
		pLoc->dataPtr->freq++;
		return 2;
	}	
} //리스트에 새로운 토큰 자료구조 추가, 노드 만들고 노드의 데이터 포인터가 dataInPtr에 해당하는 토큰 가르키도록
//static int _insert( LIST *pList, NODE *pPre, tTOKEN *dataInPtr); //addNode가 사용

/* Removes data from list
	return	0 not found
			1 deleted
*/
int removeNode( LIST *pList, char *keyPtr, tTOKEN **dataOut) //keyPtr값을 가진 노드를 찾아서 노드 제거, 제거된 내용은 dataOut이 해당 토큰을 가르키도록
{
	NODE *pPre,*pLoc;
	
	pPre = NULL;
	pLoc = pList->head;
	
	if(_search(pList,&pPre,&pLoc,keyPtr))
	{
		_delete(pList, pPre, pLoc, dataOut);
		return 1;
	}
	else
		return 0;
		
}

/* interface to search function
	Argu	key being sought
	dataOut	contains found data
	return	1 successful
			0 not found
*/
int searchList( LIST *pList, char *pArgu, tTOKEN **pDataOut)
{

	NODE* pLoc;
	NODE* pPre;

	pPre = NULL;
	pLoc = pList->head;

	if (_search(pList, &pPre, &pLoc, pArgu))
	{
		*pDataOut = pLoc->dataPtr;
		return 1;
	}
	else
		return 0;
}
//static int _search( LIST *pList, NODE **pPre, NODE **pLoc, char *pArgu)

/* returns number of nodes in list
*/
int countList( LIST *pList)
{
	return pList->count;
}

/* returns	1 empty
			0 list has data
*/
int emptyList( LIST *pList) //리스트가 비었는지 안비었는지, 헤드구조의 count값이 0이냐 아니냐
{
	if(pList->count==0||pList==NULL)
		return 1;
	else
		return 0;
}

//int fullList( LIST *pList)


/* prints data from list (forward)
*/
void printList( LIST *pList) //순방향, 헤드가 가르키는 첫번째 노드부터 r 링크 따라가면서 출력
{
	if(pList==NULL)
		return;
	
	NODE* pLoc;
	pLoc=pList->head;
	
	while(pLoc!=NULL)
	{
		printf("%-7s %3d\n",pLoc->dataPtr->token,pLoc->dataPtr->freq);
		pLoc=pLoc->rlink;
	}
}

/* prints data from l ist (backward)
*/
void printListR( LIST *pList) //rear 가르키는 마지막 노드부터 시작해서 l 링크 따라가기
{
	if(pList==NULL)
		return;
	
	NODE* pLoc;
	pLoc=pList->rear;
	
	while(pLoc!=NULL)
	{
		printf("%-7s %3d\n",pLoc->dataPtr->token,pLoc->dataPtr->freq);
		pLoc=pLoc->llink;
	}


} 

/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
static int _insert( LIST *pList, NODE *pPre, tTOKEN *dataInPtr) //addNode가 사용
{
	NODE* pNEW = (NODE*)malloc(sizeof(NODE)); //새 노드 만들기
	pNEW->dataPtr=dataInPtr;
//	strcpy(pNEW->dataPtr->token, dataInPtr->token); //노드에 정보 넣기
	
	if (pNEW == NULL)
		return 0;
	
	if (pPre == NULL) //처음에 삽입
	{
		pNEW->llink = NULL;
		pNEW->rlink = pList->head;
		if(pList->head!=NULL)
			pList->head->llink=pNEW;
		pList->head = pNEW;
		pList->count++;
		pNEW->dataPtr->freq=1;
		if(pList->rear==NULL)
			pList->rear=pNEW;
	}
	else //중간이나 끝에 삽입
	{
		pNEW->rlink=pPre->rlink;
		if(pPre->rlink!=NULL)
			pPre->rlink->llink=pNEW;
		else
			pList->rear=pNEW;
		pNEW->llink = pPre;
		pPre->rlink=pNEW;
		pList->count++;
		pNEW->dataPtr->freq=1;
	}
				
	return 1;
	
}

/* internal delete function
	deletes data from a list and saves the (deleted) data to dataOut
*/
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr) //removeNode가 사용
{
	*dataOutPtr=pLoc->dataPtr; //dataoutptr에 현재 위치의 이름 저장
	
	if (pPre == NULL)
	{
		pList->head = pLoc->rlink;
		pLoc->rlink->llink = NULL;
		pList->count--;
	}
	else if(pLoc->rlink==NULL)
	{
		pList->rear=pPre;
		pPre->rlink=NULL;
		pList->count--;
	}
	else
	{
		pPre->rlink = pLoc->rlink;
		pLoc->rlink->llink = pPre;
		pList->count--;
	}
	
	free(pLoc);
	
}

/* internal search function
	searches list and passes back address of node
	containing target and its logical predecessor
	return	1 found
			0 not found
*/
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, char *pArgu)
{
	int comp=1;
	
	while(*pLoc!=NULL) //끝까지 가지 않으면서 pArgu가 더 클 동안
	{
		comp=strcmp(pArgu,(*pLoc)->dataPtr->token);
		if(comp<=0)
			break;
		*pPre=*pLoc;
		*pLoc=(*pLoc)->rlink;
	}
	
	if (*pLoc == NULL)
		return 0;
	
	else if (comp ==0)
		return 1;
	return 0;

}

/* Allocates dynamic memory for a token structure, initialize fields(token, freq) and returns its address to caller
	return	token structure pointer
			NULL if overflow
*/
tTOKEN *createToken( char *str)  //새로운 이름 들어왔을 때 토큰 만들기, 그리고 걔를 addNode로 시도//strdup
{

	tTOKEN* newTOKEN=(tTOKEN*)malloc(sizeof(tTOKEN));
	newTOKEN->token = (char*)malloc(sizeof(char)*(strlen(str)+1));
	if (newTOKEN->token == NULL)
	{
		free(newTOKEN);
		return NULL;
	}
	strcpy(newTOKEN->token, str);
	newTOKEN->freq= 1;
	
	return newTOKEN;
}

/* Deletes all data in token structure and recycles memory
	return	NULL head pointer
*/
void destroyToken( tTOKEN *pToken)
{
	free(pToken->token);
	free(pToken);
}

////////////////////////////////////////////////////////////////////////////////
/* gets user's input
*/
int get_action() //사용자 입력 받는 것
{
	char ch;
	scanf( "%c", &ch);
	ch = toupper( ch);
	switch( ch)
	{ 
		case 'Q':
			return QUIT;
		case 'F':
			return FORWARD_PRINT;
		case 'B':
			return BACKWARD_PRINT;
		case 'S':
			return SEARCH;
		case 'D':
			return DELETE;
		case 'C':
			return COUNT;
	}
	return 0; // undefined action
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	char str[1024];
	tTOKEN *pToken;
	int ret;
	FILE *fp;
	
	if (argc != 2){// 자기 프로그램 명과 입력 파일명으로 아규먼트가 두개여야
		fprintf( stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}

	//위에를 건너왔다는 것은 아규먼트 두개라는 뜻
	
	fp = fopen( argv[1], "rt"); //입력 파일 열기
	if (!fp)//입력 파일 못열었을 때 에러
	{
		fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}
	//입력 파일 잘 열었으면 여기로 오기
	
	// creates an empty list
	list = createList(); //비어있는 리스트 만들기
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	while(fscanf( fp, "%s", str) == 1) //입력 파일의 문자열을 하나씩 받음
	{
		pToken = createToken( str); //이름에 대한 token을 만듦
		
		// insert function call
		ret = addNode(list, pToken); //strdup(str): str의 내용을 메모리 할당해서 내용을 넣고 할당된 메모리 주소를 반환
		//새로운 이름이면 리스트 안에 삽입, 이미 있는 이름이면 return 2
		
		if (ret == 2) // duplicated 
			destroyToken( pToken); //방금 만든 토큰 메모리 해제
	}
	
	fclose( fp); //파일 닫기
	
	fprintf( stderr, "Select Q)uit, F)orward print, B)ackward print, S)earch, D)elete, C)ount: ");
	
	while (1)
	{
		int action = get_action();
		
		switch( action)
		{
			case QUIT: //q 입력할 때 까지 프로그램 반복
				destroyList( list); //리스트 전체 앞의 노드부터 토큰 자료구조, 연결된 스트링, 해당 노드 다 지우기, 노드 다 사라질떄까지, 리스트 헤드구조까지 지우기
				return 0;
			
			case FORWARD_PRINT:
				printList( list);
				break;
			
			case BACKWARD_PRINT:
				printListR( list);
				break;
			
			case SEARCH: //찾아서 조회
				fprintf( stderr, "Input a string to find: ");
				fscanf( stdin, "%s", str);
				
				if (searchList( list, str, &pToken)) //찾으면 1 리턴, 못찾으면 0 리턴
				{
					fprintf( stdout, "(%s, %d)\n", pToken->token, pToken->freq); //찾았으면 이름 출력
				}
				else fprintf( stdout, "%s not found\n", str); //못찾은 경우
				break;
				
			case DELETE: //이름 찾아서 노드 제거, 노드 연결된 토큰 자료구조도 제거, 
				fprintf( stderr, "Input a string to delete: ");
				fscanf( stdin, "%s", str);
				
				if (removeNode( list, str, &pToken)) //잘 지웠으면 1
				{
					fprintf( stdout, "%s deleted\n", pToken->token);
					destroyToken( pToken); //토큰 없애기
				}
				else fprintf( stdout, "%s not found\n", str); //지울 이름이 없다
				break;
			
			case COUNT:
				fprintf( stdout, "%d\n", countList( list)); //헤드 자료구조의 count 값을 리턴
				break;
			}
		
		if (action) fprintf( stderr, "Select Q)uit, F)orward print, B)ackward print, S)earch, D)elete, C)ount: ");
	}
	return 0;
}
