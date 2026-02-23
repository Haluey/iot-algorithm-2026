#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TITLE_MAX   100
#define GENRE_MAX    40
#define FILE_MAGIC   "MOVDB1"   // 파일 포맷 식별자(간단한 헤더)

// -----------------------------
// 데이터 구조
// -----------------------------
typedef struct {
    int  id;                          // 고유 ID
    char title[TITLE_MAX];            // 제목
    char genre[GENRE_MAX];            // 장르
    int  year;                        // 개봉년도
    int  runtime;                     // 러닝타임(분)
    float rating;                     // 평점(0.0 ~ 10.0)
} Movie;

typedef struct {
    Movie* data;      // 동적 배열 시작 주소
    size_t size;      // 실제 원소 개수
    size_t capacity;  // 할당된 용량
    int next_id;      // 다음에 부여할 ID
} MovieDB;

// -----------------------------
// 유틸: 입력 처리
// -----------------------------
static void clear_stdin_line(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
}

static void read_line(const char* prompt, char* out, size_t out_size) {
    if (prompt) printf("%s", prompt);
    if (!fgets(out, (int)out_size, stdin)) {
        // EOF 등
        out[0] = '\0';
        return;
    }
    size_t n = strlen(out);
    if (n > 0 && out[n - 1] == '\n') out[n - 1] = '\0';
    else clear_stdin_line(); // 버퍼에 남은 것 정리
}

static int read_int(const char* prompt, int min, int max) {
    char buf[64];
    while (1) {
        read_line(prompt, buf, sizeof(buf));
        char* end = NULL;
        long v = strtol(buf, &end, 10);
        if (end == buf || *end != '\0') {
            puts("  - 숫자를 다시 입력해줘.");
            continue;
        }
        if (v < min || v > max) {
            printf("  - 범위(%d ~ %d)로 입력해줘.\n", min, max);
            continue;
        }
        return (int)v;
    }
}

static float read_float(const char* prompt, float min, float max) {
    char buf[64];
    while (1) {
        read_line(prompt, buf, sizeof(buf));
        char* end = NULL;
        double v = strtod(buf, &end);
        if (end == buf || *end != '\0') {
            puts("  - 실수값을 다시 입력해줘.");
            continue;
        }
        if (v < min || v > max) {
            printf("  - 범위(%.1f ~ %.1f)로 입력해줘.\n", min, max);
            continue;
        }
        return (float)v;
    }
}

// 대소문자 무시 부분 문자열 검색(간단 구현)
static int contains_case_insensitive(const char* haystack, const char* needle) {
    if (!needle || needle[0] == '\0') return 1;
    if (!haystack) return 0;

    size_t hlen = strlen(haystack);
    size_t nlen = strlen(needle);
    if (nlen > hlen) return 0;

    for (size_t i = 0; i + nlen <= hlen; ++i) {
        size_t j = 0;
        for (; j < nlen; ++j) {
            char a = (char)tolower((unsigned char)haystack[i + j]);
            char b = (char)tolower((unsigned char)needle[j]);
            if (a != b) break;
        }
        if (j == nlen) return 1;
    }
    return 0;
}

// -----------------------------
// DB(동적배열) 관리
// -----------------------------
static void db_init(MovieDB* db) {
    db->data = NULL;
    db->size = 0;
    db->capacity = 0;
    db->next_id = 1;
}

static void db_free(MovieDB* db) {
    free(db->data);
    db->data = NULL;
    db->size = 0;
    db->capacity = 0;
    db->next_id = 1;
}

static int db_reserve(MovieDB* db, size_t new_cap) {
    if (new_cap <= db->capacity) return 1;
    Movie* p = (Movie*)realloc(db->data, new_cap * sizeof(Movie));
    if (!p) return 0;
    db->data = p;
    db->capacity = new_cap;
    return 1;
}

static int db_push(MovieDB* db, const Movie* m) {
    if (db->size == db->capacity) {
        size_t new_cap = (db->capacity == 0) ? 4 : db->capacity * 2;
        if (!db_reserve(db, new_cap)) return 0;
    }
    db->data[db->size++] = *m;
    return 1;
}

static int db_find_index_by_id(const MovieDB* db, int id) {
    for (size_t i = 0; i < db->size; ++i) {
        if (db->data[i].id == id) return (int)i;
    }
    return -1;
}

static int db_delete_by_index(MovieDB* db, size_t idx) {
    if (idx >= db->size) return 0;
    // 뒤쪽을 앞으로 당김
    if (idx + 1 < db->size) {
        memmove(&db->data[idx], &db->data[idx + 1],
            (db->size - (idx + 1)) * sizeof(Movie));
    }
    db->size--;

    // 너무 넉넉하면 줄이기(선택사항)
    if (db->capacity >= 8 && db->size <= db->capacity / 4) {
        size_t new_cap = db->capacity / 2;
        Movie* p = (Movie*)realloc(db->data, new_cap * sizeof(Movie));
        if (p) {
            db->data = p;
            db->capacity = new_cap;
        }
    }
    return 1;
}

// -----------------------------
// 출력
// -----------------------------
static void print_movie_header(void) {
    puts("--------------------------------------------------------------------------------");
    printf("%-5s | %-30s | %-10s | %-4s | %-7s | %-6s\n",
        "ID", "TITLE", "GENRE", "YEAR", "RUNTIME", "RATE");
    puts("--------------------------------------------------------------------------------");
}

static void print_movie_row(const Movie* m) {
    printf("%-5d | %-30.30s | %-10.10s | %-4d | %-7d | %-6.1f\n",
        m->id, m->title, m->genre, m->year, m->runtime, m->rating);
}

// -----------------------------
// 기능: 추가/목록/검색/수정/삭제
// -----------------------------
static void ui_add_movie(MovieDB* db) {
    Movie m;
    m.id = db->next_id++;

    read_line("제목: ", m.title, sizeof(m.title));
    read_line("장르: ", m.genre, sizeof(m.genre));
    m.year = read_int("개봉년도(1900~2100): ", 1900, 2100);
    m.runtime = read_int("러닝타임(1~500): ", 1, 500);
    m.rating = read_float("평점(0.0~10.0): ", 0.0f, 10.0f);

    if (!db_push(db, &m)) {
        puts("메모리 부족으로 추가 실패!");
        db->next_id--; // 롤백(간단 처리)
        return;
    }
    puts("추가 완료!");
}

static void ui_list_movies(const MovieDB* db) {
    if (db->size == 0) {
        puts("등록된 영화가 없어.");
        return;
    }
    print_movie_header();
    for (size_t i = 0; i < db->size; ++i) {
        print_movie_row(&db->data[i]);
    }
    puts("--------------------------------------------------------------------------------");
    printf("총 %zu개\n", db->size);
}

static void ui_search_movies(const MovieDB* db) {
    char key[128];
    read_line("검색어(제목/장르 부분일치): ", key, sizeof(key));

    int found = 0;
    print_movie_header();
    for (size_t i = 0; i < db->size; ++i) {
        const Movie* m = &db->data[i];
        if (contains_case_insensitive(m->title, key) ||
            contains_case_insensitive(m->genre, key)) {
            print_movie_row(m);
            found = 1;
        }
    }
    if (!found) puts("검색 결과 없음.");
    puts("--------------------------------------------------------------------------------");
}

static void ui_edit_movie(MovieDB* db) {
    if (db->size == 0) {
        puts("수정할 영화가 없어.");
        return;
    }
    int id = read_int("수정할 영화 ID: ", 1, 1000000000);
    int idx = db_find_index_by_id(db, id);
    if (idx < 0) {
        puts("해당 ID를 찾을 수 없어.");
        return;
    }

    Movie* m = &db->data[idx];
    puts("현재 정보:");
    print_movie_header();
    print_movie_row(m);
    puts("--------------------------------------------------------------------------------");

    char buf[128];

    read_line("새 제목(그대로=엔터): ", buf, sizeof(buf));
    if (buf[0] != '\0') strncpy(m->title, buf, sizeof(m->title));

    read_line("새 장르(그대로=엔터): ", buf, sizeof(buf));
    if (buf[0] != '\0') strncpy(m->genre, buf, sizeof(m->genre));

    read_line("새 개봉년도(그대로=엔터): ", buf, sizeof(buf));
    if (buf[0] != '\0') {
        char* end = NULL;
        long v = strtol(buf, &end, 10);
        if (end != buf && *end == '\0' && v >= 1900 && v <= 2100) m->year = (int)v;
        else puts("  - 개봉년도 입력 무시(형식/범위 오류)");
    }

    read_line("새 러닝타임(그대로=엔터): ", buf, sizeof(buf));
    if (buf[0] != '\0') {
        char* end = NULL;
        long v = strtol(buf, &end, 10);
        if (end != buf && *end == '\0' && v >= 1 && v <= 500) m->runtime = (int)v;
        else puts("  - 러닝타임 입력 무시(형식/범위 오류)");
    }

    read_line("새 평점(그대로=엔터): ", buf, sizeof(buf));
    if (buf[0] != '\0') {
        char* end = NULL;
        double v = strtod(buf, &end);
        if (end != buf && *end == '\0' && v >= 0.0 && v <= 10.0) m->rating = (float)v;
        else puts("  - 평점 입력 무시(형식/범위 오류)");
    }

    puts("수정 완료!");
}

static void ui_delete_movie(MovieDB* db) {
    if (db->size == 0) {
        puts("삭제할 영화가 없어.");
        return;
    }
    int id = read_int("삭제할 영화 ID: ", 1, 1000000000);
    int idx = db_find_index_by_id(db, id);
    if (idx < 0) {
        puts("해당 ID를 찾을 수 없어.");
        return;
    }

    puts("삭제 대상:");
    print_movie_header();
    print_movie_row(&db->data[idx]);
    puts("--------------------------------------------------------------------------------");

    char yn[8];
    read_line("정말 삭제? (y/n): ", yn, sizeof(yn));
    if (tolower((unsigned char)yn[0]) == 'y') {
        db_delete_by_index(db, (size_t)idx);
        puts("삭제 완료!");
    }
    else {
        puts("삭제 취소.");
    }
}

// -----------------------------
// 파일 저장/불러오기(바이너리)
// -----------------------------
static int db_save(const MovieDB* db, const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) return 0;

    // 헤더
    fwrite(FILE_MAGIC, 1, strlen(FILE_MAGIC), fp);

    // next_id, size
    fwrite(&db->next_id, sizeof(db->next_id), 1, fp);
    fwrite(&db->size, sizeof(db->size), 1, fp);

    // 데이터
    if (db->size > 0) {
        fwrite(db->data, sizeof(Movie), db->size, fp);
    }

    fclose(fp);
    return 1;
}

static int db_load(MovieDB* db, const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;

    char magic[16] = { 0 };
    fread(magic, 1, strlen(FILE_MAGIC), fp);
    if (strcmp(magic, FILE_MAGIC) != 0) {
        fclose(fp);
        return 0;
    }

    int next_id = 1;
    size_t size = 0;

    if (fread(&next_id, sizeof(next_id), 1, fp) != 1) { fclose(fp); return 0; }
    if (fread(&size, sizeof(size), 1, fp) != 1) { fclose(fp); return 0; }

    // 기존 메모리 정리 후 재할당
    db_free(db);
    db_init(db);

    if (size > 0) {
        if (!db_reserve(db, size)) { fclose(fp); return 0; }
        if (fread(db->data, sizeof(Movie), size, fp) != size) { fclose(fp); return 0; }
        db->size = size;
        db->next_id = next_id;
        // capacity는 reserve에서 설정됨
    }
    else {
        db->next_id = next_id;
    }

    fclose(fp);
    return 1;
}

static void ui_save_db(const MovieDB* db) {
    char fn[256];
    read_line("저장 파일명(예: movies.dat): ", fn, sizeof(fn));
    if (fn[0] == '\0') {
        puts("파일명이 비어있어 취소.");
        return;
    }
    if (db_save(db, fn)) puts("저장 완료!");
    else puts("저장 실패!");
}

static void ui_load_db(MovieDB* db) {
    char fn[256];
    read_line("불러올 파일명(예: movies.dat): ", fn, sizeof(fn));
    if (fn[0] == '\0') {
        puts("파일명이 비어있어 취소.");
        return;
    }
    if (db_load(db, fn)) puts("불러오기 완료!");
    else puts("불러오기 실패! (파일 없음/포맷 불일치)");
}

// -----------------------------
// 메뉴
// -----------------------------
static void print_menu(void) {
    puts("\n========== 영화 관리 프로그램 (동적 배열) ==========");
    puts("1. 영화 추가");
    puts("2. 전체 목록");
    puts("3. 검색(제목/장르)");
    puts("4. 수정(ID)");
    puts("5. 삭제(ID)");
    puts("6. 저장(파일)");
    puts("7. 불러오기(파일)");
    puts("0. 종료");
    puts("===============================================");
}

int main(void) {
    MovieDB db;
    db_init(&db);

    while (1) {
        print_menu();
        int choice = read_int("메뉴 선택: ", 0, 7);

        switch (choice) {
        case 1: ui_add_movie(&db); break;
        case 2: ui_list_movies(&db); break;
        case 3: ui_search_movies(&db); break;
        case 4: ui_edit_movie(&db); break;
        case 5: ui_delete_movie(&db); break;
        case 6: ui_save_db(&db); break;
        case 7: ui_load_db(&db); break;
        case 0:
            db_free(&db);
            puts("종료!");
            return 0;
        default:
            puts("잘못된 선택!");
            break;
        }
    }
}