#include <stdio.h>
#include <stdlib.h>	// malloc
#include <string.h>	// strdup
#include <ctype.h>	// isupper, tolower

#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW(end of word)
#define EOW			'$'	// end of word

// used in the following functions: trieInsert, trieSearch, triePrefixList(여기서 getIndex사용)
#define getIndex(x)		(((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a')) //x 문자가 들어갔을 때, 해당되는 포인터 배열의 인덱스 찾아주는 함수

// TRIE type definition
typedef struct trieNode {
	int 			index; // -1 (non-word 해당 노드에  문자열이 연결되지 않으면), 0, 1, 2, ... 그 문자열의 배열의 인덱스(해당 문자열을 가르키는 인덱스)
	struct trieNode	*subtrees[MAX_DEGREE]; // 노드를 가르키는 포인터 타입. 이 자체가 배열로 구현됨. 배열의 크기는 알파벳의 크기 만큼
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations
TRIE *trieCreateNode(void);
void trieDestroy(TRIE *root); 
int trieInsert(TRIE *root, char *str, int dic_index);
int trieSearch(TRIE *root, char *str);
void trieList(TRIE *root, char *dic[]);
void triePrefixList(TRIE *root, char *str, char *dic[]);



/* Allocates dynamic memory for a trie node and returns its add ress to caller
return	node pointer
NULL if overflow
*/
TRIE *trieCreateNode(void) //노드 하나 만들기(루트 포함해서), 문자 따라갈때 해당 노드 자리가 null이면 노드 하나 만들어주기
{
	TRIE* pnode=(TRIE*)malloc(sizeof(TRIE));
	
	if(!pnode)
		return NULL;
	else
	{
		pnode->index=-1;
		for(int j=0;j<MAX_DEGREE;j++)
		{
			pnode->subtrees[j]=NULL;
		}
		return pnode;
	}
	
	
}
//노드 만들 때, 처음 만들면 인덱스는 -1로 세팅, 나머지 포인터는 null로 세팅, 메모리 잡고
/* Deletes all data in trie and recycles memory
*/
void trieDestroy(TRIE *root) //데이터 삭제, 메모리 해제
{
	if(root==NULL)
		return;
	
	for(int i=0;i<MAX_DEGREE;i++)
		trieDestroy(root->subtrees[i]);
	
	free(root);
		
}

/* Inserts new entry into the trie
return	1 success
0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함(해당 위치 찾았는데 걔의 index가 -1이 아니면 이미 있는 것)
// 대소문자를 소문자로 통일하여 삽입(str 변환시키기) tolower로 소문자로 바꾸기, isupper로 대문자인지 확인
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음(삽입 실패)
int trieInsert(TRIE *root, char *str, int dic_index) //str문자열을 받아서 dic_index이 문자열의 사전 인덱스 받아서 root 가 가르키는 트라이에 삽입
{
	
	
	
		
	if(*str=='\0') //끝까지 읽음
	{
		if(root->index!=-1) //이미 있는 것
			return 0;
		root->index=dic_index;
		return 1;
	}
	
	if((!isalpha(*str)) && ((*str)!='$'))
		return 0;
	
		
	if(root->subtrees[getIndex(tolower(*str))]==NULL) //아직 아무것도 연결 X
	{
		root->subtrees[getIndex(tolower(*str))]=trieCreateNode(); //노드 만들기
		if(!(root->subtrees[getIndex(tolower(*str))])) //노드가 제대로 안만들어지면
			return 0;
	}
	
	return trieInsert(root->subtrees[getIndex(tolower(*str))],str+1,dic_index);
}

/* Retrieve trie for the requested key
return	index in dictionary (trie) if key found
-1 key not found
*/
int trieSearch(TRIE *root, char *str) //트리 안에 str이 있는지 없는지 있으면 해당 노드의 인덱스값 리턴 없으면 -1 리턴
{
	if(*str=='\0')
	{
		if(root->index!=-1)
			return root->index;
		else
			return -1;
	}
	
	if((!isalpha(*str)) && ((*str)!='$'))
		return -1;
	
	if(root->subtrees[getIndex(*str)]!=NULL)// 같으면
		return trieSearch(root->subtrees[getIndex(*str)],str+1);
		
	return -1;
	
}

/* prints all entries in trie using preorder traversal
*/
void trieList(TRIE *root, char *dic[])//주어진 노드로부터 트리를 다 순회, 저장된 문자열을 다 출력, 검색이 되면 인덱스가 나오니까 그 인덱스로 문자열 확인
{
	if(!root)
		return;
	
	if(root->index!=-1)
	{
		printf("%s\n",dic[root->index]);
	}
	
	int subtreeindex=0;
	
	while(subtreeindex<27)
	{
		if(root->subtrees[subtreeindex])
			trieList(root->subtrees[subtreeindex],dic);
		subtreeindex++;
	}
	
	return;
	

}	

//배열에는 지금 숫자밖에 없기 떄문에 *dic[]
//preorder로 순회, 전위순회, 나부터 출력하고 자식들 나중에 출력

/* prints all entries starting with str (as prefix) in trie
ex) "abb" -> "abbess", "abbesses", "abbey", ...(abb로 시작하는 모든 단어를 출력)
using trieList function
*/
//abb까지 트라이에서 따라 내려가고, 거기서부터 모두다 순회하면 된다
//이 함수는 내부적으로 trieList함수 사용.
void triePrefixList(TRIE *root, char *str, char *dic[])
{
	TRIE* temptrieptr=root;
	
	while(*str)
	{
			
		if(temptrieptr==NULL)
			return;
		temptrieptr=temptrieptr->subtrees[getIndex(*str)];
		(str)++;
	}
	
	trieList(temptrieptr,dic);
}


////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	TRIE *trie; //루트를 가르키는 포인터
	int ret; 
	char str[100];
	FILE *fp;
	char *dic[100000]; 
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]); //단어리스트를 입력파일로 받는다 .얘를 트라이 구조로 저장
		return 1;
	}
	
	fp = fopen( argv[1], "rt"); //입력파일 열기
	if (fp == NULL) //못열었으면 에러 리턴
	{
		fprintf( stderr, "File open error: %s\n", argv[1]);
		return 1;
	}
	
	trie = trieCreateNode(); //비어있는 루트 노드 만들기
	
	int index = 0;
	while (fscanf( fp, "%s", str) != EOF) //입력받은 파일로부터 문자열 하나씩 읽어들이기
	{
		ret = trieInsert( trie, str, index); //읽어들인 문자열 삽입, 삽입 성공시 ret이 1

		if (ret) dic[index++] = strdup( str); //인덱스값이 0부터 하나씩 증가됨. 처음 문자열이 0번 인덱스, 삽입이 성공했을 때! 사전 갱신
	}
	//트라이와 사전 완성
	
	fclose( fp);
	
	fprintf( stdout, "\nQuery: ");
	while (fscanf( stdin, "%s", str) != EOF) //문자열 하나 받기
	{
		// wildcard search
		if (str[strlen(str)-1] == '*') //문자열 끝에 *가 들어있으면 와일드카드 서치, ab*
		{
			str[strlen(str)-1] = 0; //해당 부분은 null로 *를 없애주는 것 ab
			triePrefixList( trie, str, dic); //ab로 시작되는 모든 문자열을 찾아봐라 순회해서 화면에 출력
		}
		// keyword search
		else
		{
			ret = trieSearch( trie, str);
			if (ret == -1) printf( "[%s] not found!\n", str); //리턴값이 -1이면 없다
			else printf( "[%s] found!\n", dic[ret]); //찾았다! 그 찾은 문자열은 사전에서 인덱스로 나온 리턴값으로 사전 문자열 출력
		}
		
		fprintf( stdout, "\nQuery: ");
	}
	
	
	
	for (int i = 0; i < index; i++)
		free( dic[i]); 
	
	trieDestroy( trie); //포인터 많이 해제! (l,r말고도 많음)
	
	return 0;
}
