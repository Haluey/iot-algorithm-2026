// 스택을 사용하여 문자열을 뒤집어 출력

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int max;
	int ptr;
	char* stk;
} Stack;

int main(void) {
	char str[10] = "hello";

	Stack st = { .ptr = 0, .max = strlen(str)};

	st.stk = calloc(st.max, sizeof(char));

	for (int i = 0; i < st.max; i++) {
		st.stk[st.ptr++] = str[i];
	}

	for (int i = 0; i < st.max; i++) {
		printf("%c ", st.stk[--st.ptr]);
	}

	return 0;
}