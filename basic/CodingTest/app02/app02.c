// 1부터 N까지의 카드
// 맨 위 카드 1장을 버림
// 그 다음 맨 위 카드 1장을 맨 아래로 옮김
// 다음 과정을 카드가 1장 남을 때까지 반복
// 마지막 카드번호를 출력

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int max;
	int num;
	int front;
	int rear;
	int* que;
} Queue;

void restart(Queue* qu) {
	qu->rear %= qu->max;
	qu->front %= qu->max;
}

void throw(Queue* qu){
	restart(qu);
	qu->front++;
	qu->num--;
}

void change(Queue* qu) {
	restart(qu);
	int temp;

	temp = qu->que[qu->front++];
	qu->que[qu->rear++] = temp;
}

int main(void) {
	int n, i;
	Queue qu;
	qu.num = qu.front = qu.rear = 0;

	printf("숫자를 입력하세요 : "); scanf("%d", &n);
	qu.max = n;
	qu.que = calloc(n, sizeof(int));
	for (i = 1; i <= n; i++) {
		qu.que[qu.rear++] = i;
		qu.num++;
	}

	while (qu.num > 1) {
		throw(&qu);
		change(&qu);
	}

	printf("마지막 카드 번호 : %d\n", qu.que[qu.rear - 1]);

	return 0;
}