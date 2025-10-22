#define _CRT_SECURE_NO_WARNINGS // scanf 보안 경고를 비활성화합니다.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>
#include <windows.h>

#define RESET_COLOR 7
#define GREEN 10

typedef struct {
    char name[50];
    char talk[200];
    char answer[50];
    char talkO[100];
    char talkX[100];
} Fish;

void set_color(int color_code) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color_code);
}

void print_game_stats() {
    FILE* file = fopen("log.txt", "r");
    if (file == NULL) {
        printf("1번째 게임입니다!!\n");
        printf("정답률 0/0\n\n");
        return;
    }

    int total_games = 0;
    int correct_count = 0;
    char line1[100];
    char line2[10];

    while (fgets(line1, sizeof(line1), file) != NULL && fgets(line2, sizeof(line2), file) != NULL) {
        total_games++;
        if (strcmp(line2, "O\n") == 0) {
            correct_count++;
        }
    }
    fclose(file);

    printf("%d번째 게임입니다!!\n", total_games + 1);
    printf("정답률 %d/%d\n\n", correct_count, total_games);
}

void toLowerStr(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = (char)tolower(str[i]);
    }
}

int main() {
    setlocale(LC_ALL, "Korean");
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    print_game_stats();

    char input;
    while (1) {
        printf("찌를 던진다 (Y/N) > ");
        scanf(" %c", &input);
        while (getchar() != '\n');

        if (input == 'Y' || input == 'y') {
            break;
        }
        else if (input == 'N' || input == 'n') {
            printf("게임을 종료합니다.\n");
            return 0;
        }
        else {
            printf("잘못된 입력입니다. Y 또는 N을 입력해주세요.\n");
        }
    }

    printf("낚시 중");
    fflush(stdout);
    for (int i = 0; i < 5; i++) {
        Sleep(500);
        printf(".");
        fflush(stdout);
    }
    printf("\n");

    set_color(GREEN);
    printf("입질이 왔다!!!!\n");
    set_color(RESET_COLOR);

    Fish fishes[5] = {
        {"나루토", "나루토의 주인공 이름은?", "나루토", "맞아! 나루토다!", "아니야, 다시 생각해봐!"},
        {"토토로", "애니 '이웃집 토토로'에서 토토로는 어떤 동물일까?", "토토로", "정답! 토토로는 신비한 생물이야!", "틀렸어, 토토로를 다시 생각해봐!"},
        {"해리포터", "해리포터 시리즈에서 주인공 해리의 부모님은 어떻게 되셨나요?", "죽음", "맞아, 부모님은 돌아가셨어!", "아니야, 다시 맞춰봐!"},
        {"겨울왕국", "겨울왕국에서 엘사의 마법은 무엇인가요?", "얼음", "정답! 얼음 마법이야!", "틀렸어, 다시 생각해봐!"},
        {"왕좌의게임", "왕좌의 게임에서 '왕좌'를 차지하기 위한 주요 가문 중 하나는?", "스타크", "맞아, 스타크 가문이 중요하지!", "아니야, 다른 가문을 생각해봐!"}
    };

    srand((unsigned int)time(NULL));
    int fish_index = rand() % 5;
    Fish caught_fish = fishes[fish_index];

    char user_answer[100];
    printf("\n[%s] \"%s\"\n> ", caught_fish.name, caught_fish.talk);
    fgets(user_answer, sizeof(user_answer), stdin);
    user_answer[strcspn(user_answer, "\n")] = 0;

    char user_answer_lower[100];
    char answer_lower[100];
    strcpy(user_answer_lower, user_answer);
    strcpy(answer_lower, caught_fish.answer);
    toLowerStr(user_answer_lower);
    toLowerStr(answer_lower);

    int is_correct = (strcmp(user_answer_lower, answer_lower) == 0);

    if (is_correct) {
        printf("\n[%s] \"%s\"\n", caught_fish.name, caught_fish.talkO);
    }
    else {
        printf("\n[%s] \"%s\"\n", caught_fish.name, caught_fish.talkX);
    }

    FILE* log_file = fopen("log.txt", "a");
    if (log_file == NULL) {
        printf("오류: 로그 파일을 열 수 없습니다.\n");
        return 1;
    }

    fprintf(log_file, "%s\n", caught_fish.name);
    fprintf(log_file, "%s\n", is_correct ? "O" : "X");
    fclose(log_file);

    printf("\n로그가 기록되었습니다. 게임을 종료합니다.\n");

    return 0;
}
