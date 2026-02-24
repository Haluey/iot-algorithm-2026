// p164~165. 큐 구현

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "IntQueue.h"

int main(void) {
	IntQueue que;

	if (Initialize(&que, 8) == -1) {
		puts("큐 생성 실패!");
		return 1;	// 또는 exit(1)
	}

	int menu, x, ch;

	while (1) {
		menu = -1;

		printf("현재 데이터 수 : %d / %d\n", Size(&que), Capacity(&que));
		printf("(1)인큐, (2)디큐, (3)피크, (4)출력, (5)검색, (6)클리어, (0)종료 > "); scanf("%d", &menu);

		// 잘못된 키보드 입력 버퍼를 제거
		// 기초 프로그래밍 주소록 토이프로젝트 소스 참조
		while ((ch = getchar()) != '\n' && ch != EOF) {}

		if (menu == 0) break;

		switch (menu) {
		case 1:	// 인큐
			if (!IsFull(&que)) {
				printf("데이터 입력 > "); scanf("%d", &x);
				if (Enque(&que, x) == -1) {
					puts("\a오류 : 인큐 실패!");
				}				
			}
			else {
				puts("\a큐가 꽉 찼습니다.");
			}
			break;

		case 2:	// 디큐
			if (!IsEmpty(&que)) {
				if (Deque(&que, &x) == -1) {
					puts("\a오류 : 디큐 실패!");
				}
				else {
					printf("디큐 데이터 : %d\n", x);
				}
			}
			else {
				puts("\a큐가 비어있습니다.");
			}
			break;

		case 3:	// 피크
			if (Peek(&que, &x) == -1) {
				puts("\a오류 : 피크 실패!");
			}
			else {
				printf("피크 데이터 : %d\n", x);
			}
			break;

		case 4:	// 출력
			Print(&que);
			break;

		case 5:	// 검색
			printf("검색할 수 입력 > "); scanf("%d", &x);
			int res = Search(&que, x);
			if (res == -1) {
				puts("검색 실패");
			}
			else {
				printf("검색 결과 : stack[%d] => %d\n", res, que.que[res]);
			}
			break;

		case 6:	// 큐 클리어
			Clear(&que);
			puts("큐 클리어!");
			break;

		default:	// 오류잡기
			puts("입력오류!");
			break;
		}
	}

	Terminate(&que);	// 종료

	return 0;
}