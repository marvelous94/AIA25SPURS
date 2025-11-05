// Visual Studio의 보안 경고(C4996)를 비활성화합니다.
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <conio.h>  

// 학생 정보를 저장할 구조체 정의
typedef struct {
    char name[100];
    int atk;
    int def;
    int hp;
} Student;

int main() {

    // --- 1. 학번 및 이름 입력 (공백 포함 가능) ---
    char student_id[20];
    char student_name[100];

    printf("학번을 입력하세요: ");
    fgets(student_id, sizeof(student_id), stdin);
    student_id[strcspn(student_id, "\n")] = 0;

    printf("이름을 입력하세요: ");
    fgets(student_name, sizeof(student_name), stdin);
    student_name[strcspn(student_name, "\n")] = 0;


    // --- 2. students.csv 파일 읽기 ---
    FILE* fp_in = fopen("students.csv", "r");

    if (fp_in == NULL) {
        perror("오류: students.csv 파일을 열 수 없습니다.");
        printf("===================================================\n");
        printf("해결: 'students.csv' 파일을 'x64/Debug' 폴더 안에\n");
        printf("      복사했는지 반드시 확인하세요!\n");
        printf("===================================================\n");
        printf("\nPress any key to exit...");
        _getch();
        return 1;
    }

    // ==========================================================
    // [!!] 새로 추가된 부분 [!!]
    // 파일의 첫 번째 줄(헤더/제목)을 읽어서 버립니다.
    char temp_buffer[256]; // 첫 줄을 읽어 저장할 임시 공간
    fgets(temp_buffer, sizeof(temp_buffer), fp_in);
    // ==========================================================


    Student students[10];
    int student_count = 0;

    // (이제 두 번째 줄부터 읽기 시작함)
    while (student_count < 10 && fscanf(fp_in, "%[^,],%d,%d,%d\n",
        students[student_count].name,
        &students[student_count].atk,
        &students[student_count].def,
        &students[student_count].hp) == 4)
    {
        student_count++;
    }
    fclose(fp_in);

    // [오류 검사 2] 10명의 데이터를 다 못 읽었을 때
    if (student_count < 10) {
        printf("오류: 'students.csv'에서 10명의 데이터를 모두 읽지 못했습니다.\n");
        printf("      (현재 %d명만 읽음) \n", student_count);
        printf("      파일 형식이 '이름,공격력,방어력,HP' (줄바꿈) 인지 확인하세요.\n");
        printf("      (참고: 헤더 줄은 자동으로 건너뛰었습니다.)\n");
        printf("\nPress any key to exit...");
        _getch();
        return 1;
    }

    // --- 3. Test.txt 파일 쓰기 ---
    FILE* fp_out = fopen("Test.txt", "w");

    if (fp_out == NULL) {
        perror("오류: Test.txt 파일을 생성(쓰기)할 수 없습니다.");
        printf("\nPress any key to exit...");
        _getch();
        return 1;
    }

    // (1) 첫 번째 줄: 학번과 이름 출력
    fprintf(fp_out, "%s : %s\n", student_id, student_name);

    // (2) 두 번째 줄: 가장 공격력이 높은 사람 찾기
    int max_atk = -1;
    int max_atk_index = 0;
    for (int i = 0; i < student_count; i++) {
        if (students[i].atk > max_atk) {
            max_atk = students[i].atk;
            max_atk_index = i;
        }
    }
    fprintf(fp_out, "가장 공격력이 높은 사람: %s (ATK=%d)\n",
        students[max_atk_index].name, students[max_atk_index].atk);

    // (3) 세 번째 줄: 3번째 사람 (배열 인덱스 2)
    fprintf(fp_out, "3번째: %s HP=%d\n", students[2].name, students[2].hp);

    // (4) 네 번째 줄: 8번째 사람 (배열 인덱스 7)
    fprintf(fp_out, "8번째: %s HP=%d\n", students[7].name, students[7].hp);

    // (5) 다섯 번째 줄: 3번째 vs 8번째 전투
    Student p3 = students[2];
    Student p8 = students[7];

    int p3_damage_taken = (p8.atk - p3.def > 0) ? (p8.atk - p3.def) : 0;
    int p8_damage_taken = (p3.atk - p8.def > 0) ? (p3.atk - p8.def) : 0;

    if (p3_damage_taken == 0 && p8_damage_taken == 0) {
        fprintf(fp_out, "3번째 vs 8번째: 무승부\n");
    }
    else {
        int hp3 = p3.hp;
        int hp8 = p8.hp;

        int is_draw = 0;
        char* winner_name = NULL;

        while (1) {
            hp3 -= p3_damage_taken;
            hp8 -= p8_damage_taken;

            int p3_dead = (hp3 <= 0);
            int p8_dead = (hp8 <= 0);

            if (p3_dead && p8_dead) {
                is_draw = 1;
                break;
            }
            else if (p3_dead) {
                winner_name = p8.name;
                break;
            }
            else if (p8_dead) {
                winner_name = p3.name;
                break;
            }
        }

        if (is_draw) {
            fprintf(fp_out, "3번째 vs 8번째: 무승부\n");
        }
        else {
            fprintf(fp_out, "3번째 vs 8번째 승자: %s\n", winner_name);
        }
    }

    // (6) 여섯 번째 줄: 고정 문장 출력
    fprintf(fp_out, "교수님 시험문제 너무 쉽습니다. 담주에 더 어렵게 내주세요\n");

    // --- 4. 마무리 ---
    fclose(fp_out);

    printf("Test.txt 파일 생성이 완료되었습니다.\n");

    printf("\nPress any key to exit...");
    _getch();

    return 0; // 프로그램 정상 종료
}