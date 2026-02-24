// p172. 팩토리얼 재귀방식
// result를 int로 하면 12!까지만 계산 가능
// unsigned long long이면 20!까지 계산 가능

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

unsigned long long factorial(int n);	// int factorial(int n) 와 비교

int main(void) {
	int x;
	unsigned long long result = 0;
	puts("팩토리얼 재귀호출");

	while (1) {
		printf("정수 입력 > "); scanf("%d", &x);
		if (x <= 0) break;

		result = factorial(x);

		printf("%d! = %llu \n", x, result);
	}
	
	return 0;
}

// factorial
// 0! == 1
// n! == n x (n - 1)!
unsigned long long factorial(int n) {
	if (n > 0) {	// n x (n - 1)!
		// return n * factorial(n - 1);	// int일 경우
		return (unsigned long long)n * factorial(n - 1);	// 5 * factorial(4) -> 5 * 4 * factorial(3) -> ...
	}
	else {	// 0!
		return 1;	// 반드시 종료조건이 있어야 함!
	}
}