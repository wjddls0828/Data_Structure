#include <stdio.h>
#include <stdlib.h> // malloc, rand
#include <time.h> // time
#include <math.h>

#define MAX_ELEM	20 //20개의 난수 발생

typedef struct
{
	int *heapArr;
	int	last;
	int	capacity;
} HEAP;

HEAP *heapCreate( int capacity);
void heapDestroy( HEAP *heap);
int heapInsert( HEAP *heap, int data);
static void _reheapUp( HEAP *heap, int index);
int heapDelete( HEAP *heap, int* data);
static void _reheapDown( HEAP *heap, int index);
void heapPrint( HEAP *heap);

/* Allocates memory for heap and returns address of heap head structure
if memory overflow, NULL returned
*/
HEAP *heapCreate( int capacity)// heap 구조체 메모리 할당, 배열 메모리 할당
{
	HEAP* pheap=(HEAP*)malloc(sizeof(HEAP));
	if(!pheap)
		return NULL;
	
	pheap->heapArr=(int*)malloc(sizeof(int)*MAX_ELEM);
	pheap->last=-1;
	pheap->capacity=MAX_ELEM;
	
	return pheap;	
}

/* Free memory for heap
*/
void heapDestroy( HEAP *heap) //heap 메모리 해제 heaparr가 가르키는 메모리 영역 해제, HEAP 구조체 메모리 해제
{
	free(heap->heapArr);
	free(heap);
	
}
/* Inserts data into heap
return 1 if successful; 0 if heap full
*/
int heapInsert( HEAP *heap, int data)//데이터를 HEAP 자료구조에 삽입, 삽입 성공하면 1 리턴 실패하면 0리턴(힙이 꽉찼을때)
{
	if(heap->last==19)
		return 0;
	++(heap->last);
	heap->heapArr[heap->last]=data;
	_reheapUp(heap,heap->last);
	return 1;
}
/* Reestablishes heap by moving data in child up to correct location heap array
*/
static void _reheapUp( HEAP *heap, int index) //HEAPINSERT가 사용, LAST값을 하나 증가시키면서 거기에  데이터를 넣어 두고 거기서부터 REHAPUP해서 자기 위치를 찾아서 올라가기
{
	int parent;
	int temp;
	
	if(index)
	{
		parent=floor((index-1)/2);
		if(heap->heapArr[index]>heap->heapArr[parent])
		{
			temp=heap->heapArr[parent];
			heap->heapArr[parent]=heap->heapArr[index];
			heap->heapArr[index]=temp;
			_reheapUp(heap,parent);	
		}
	}
	
	return;
}
/* Deletes root of heap and passes data back to caller
return 1 if successful; 0 if heap empty
*/
int heapDelete( HEAP *heap, int* data) //루트에서 하나의 원소를 꺼내서 data에 저장
{
	if(heap->last==-1)
	{
		return 0;
	}
	*data=heap->heapArr[0];
	heap->heapArr[0]=heap->heapArr[heap->last];
	(heap->last)--;
	_reheapDown(heap,0);
	return 1;
}
/* Reestablishes heap by moving data in root down to its correct location in the heap
*/
static void _reheapDown( HEAP *heap, int index) //내부함수, heapdelete가 호출, index는 항상 0(루트 삭제니까)
{
	int* leftKey;
	int* rightKey;
	int large;
	int temp2;
	
	if((index*2+1)<=(heap->last)) //left subtree exist
	{
		leftKey=&(heap->heapArr[index*2+1]);
		if((index*2+2)<=(heap->last))//오른쪽 서브트리 존재
		{
			rightKey=&(heap->heapArr[index*2+2]);
		}
		else//오른쪾 서브트리 없으면
		{
			rightKey=NULL;
		}
		
		if((!rightKey)||((*leftKey)>(*rightKey))) //오른쪽 자식이 없거나 왼쪽 값이 더 크면
			large=index*2+1;
		else //오른쪽 자식이 더 크면
			large=index*2+2;
		
		if(heap->heapArr[index]<heap->heapArr[large])
		{
			temp2=heap->heapArr[index];
			heap->heapArr[index]=heap->heapArr[large];
			heap->heapArr[large]=temp2;
			_reheapDown(heap,large);
		}	
	}
	return;
}
/* Print heap array */
void heapPrint( HEAP *heap) //힙의 내용 출력
{
	for(int i=0;i<=(heap->last);i++)
	{
		printf("%6d",heap->heapArr[i]);
	}
	printf("\n");
}
int main(void)
{
	HEAP *heap; //헤드구조 가르키는 포인터
	int data;
	int i;
	
	heap = heapCreate(MAX_ELEM);
	
	srand( time(NULL)); //난수발생기 초기화
	
	for (i = 0; i < MAX_ELEM; i++) 
	{
		data = rand() % MAX_ELEM * 3 + 1; // 1 ~ MAX_ELEM*3 random number
				
		fprintf( stdout, "Inserting %d: ", data);
		
		// insert function call
		heapInsert( heap, data);
		
		heapPrint( heap);
 	}

	while (heap->last >= 0) //heap의 last가 -1이면 끝남(heap이 빌때까지)
	{
		// delete function call
		heapDelete( heap, &data);

		printf( "Deleting %d: ", data);

		heapPrint( heap);
 	}
	
	heapDestroy( heap);
	
	return 0;
}

