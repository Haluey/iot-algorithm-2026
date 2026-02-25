// 최고 점수 학생의 이름과 점수를 출력

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	char name[20];
	int score;
} Student;

int main(void) {
	Student* st;
	int n, i, m_idx;
	int max = 0;

	printf("학생 수를 입력하세요 : "); scanf("%d", &n);
	st = calloc(n, sizeof(Student));

	for (i = 0; i < n; i++) {
		scanf("%s%d", &st[i].name, &st[i].score);
	}

	for (i = 0; i < n; i++) {
		if (st[i].score > max) {
			max = st[i].score;
			m_idx = i;
		}
	}

	printf("최고 점수 학생 : %s %d", st[m_idx].name, st[m_idx].score);

	return 0;
}