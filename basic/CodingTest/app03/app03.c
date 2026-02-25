// 재귀함수로 1+2+....+N 값을 출력

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

int add(int x) {
	if (x <= 1) {
		return 1;
	}
	else {
		return x + add(x - 1);
	}
}

int main(void) {
	int x, res;

	printf("숫자를 입력해주세요 : "); scanf("%d", &x);

	res = add(x);

	printf("총합 : %d\n", res);

	return 0;
}