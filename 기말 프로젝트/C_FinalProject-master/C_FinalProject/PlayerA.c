#define _CRT_SECURE_NO_WARNINGS
#include "api.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// [1] 데이터 구조체 정의
typedef struct {
    int id;
    char name[64];
    char slot[4];
    int atk;
    int def;
    int hp;
    char curse[16];
    char key_frag[16];
} ItemData;

ItemData database[100];
int item_count = 0;

// [2] 데이터 로딩 함수
void load_data() {
    FILE* fp = fopen("AI1-2_C_Final.csv", "r");
    if (fp == NULL) {
        printf("[ERROR] CSV file not found!\n");
        return;
    }
    char line[256];
    int row = 0;
    item_count = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (row == 0) { row++; continue; } // 헤더 스킵
        ItemData* item = &database[item_count];
        // CSV 파싱
        sscanf(line, "%d,%[^,],%[^,],%d,%d,%d,%[^,],%s",
            &item->id, item->name, item->slot, &item->atk, &item->def, &item->hp, item->curse, item->key_frag);
        item_count++;
        row++;
    }
    fclose(fp);
    printf(">> Loaded %d items from DB.\n", item_count);
}

// =========================================================================
// [문제 풀이 함수들]
// =========================================================================

// [문제 1] 독(Poison): ATK>=4, DEF<=5, HP<=100 역순 연결
void solve_poison(char* result) {
    char temp[1024] = "";
    // 역순으로 탐색
    for (int i = item_count - 1; i >= 0; i--) {
        if (database[i].atk >= 4 && database[i].def <= 5 && database[i].hp <= 100) {
            if (strlen(temp) > 0) strcat(temp, "|");
            strcat(temp, database[i].name);
        }
    }
    strcpy(result, temp);
}

// [문제 2] 강타(Strike): SLOT='W'인 무기들의 'T' 인덱스 합산 + "key"
void solve_strike(char* result) {
    int sum_index = 0;
    for (int i = 0; i < item_count; i++) {
        if (strcmp(database[i].slot, "W") == 0) {
            char* ptr = strchr(database[i].key_frag, 'T'); // '*' 포함된 문자열에서 T 찾기
            if (ptr != NULL) {
                sum_index += (int)(ptr - database[i].key_frag);
            }
            // 없으면 0 더함 (변화 없음)
        }
    }
    sprintf(result, "%dkey", sum_index);
}

// [문제 3] 점멸(Blink): 4가지 복합 조건
void solve_blink(char* result) {
    char key1[16] = "", key2[16] = "", key3[16] = "", key4[16] = "";

    // 1. ID 202 DEF + ID 208 DEF = HP 인 아이템의 Key
    int target_hp = 0;
    for (int i = 0; i < item_count; i++) {
        if (database[i].id == 202) target_hp += database[i].def;
        if (database[i].id == 208) target_hp += database[i].def;
    }
    for (int i = 0; i < item_count; i++) {
        if (database[i].hp == target_hp && strcmp(database[i].key_frag, "NIL") != 0) {
            strcpy(key1, database[i].key_frag); break;
        }
    }

    // 2. ID 205 ATK * ID 212 ATK = ATK 인 아이템의 Key
    int target_atk = 1;
    for (int i = 0; i < item_count; i++) {
        if (database[i].id == 205) target_atk *= database[i].atk;
        if (database[i].id == 212) target_atk *= database[i].atk;
    }
    for (int i = 0; i < item_count; i++) {
        if (database[i].atk == target_atk && strcmp(database[i].key_frag, "NIL") != 0) {
            strcpy(key2, database[i].key_frag); break;
        }
    }

    // 3. CURSE "C_01" 포함 마지막 아이템
    for (int i = item_count - 1; i >= 0; i--) {
        if (strstr(database[i].curse, "C_01") != NULL && strcmp(database[i].key_frag, "NIL") != 0) {
            strcpy(key3, database[i].key_frag); break;
        }
    }

    // 4. NAME 'I'로 시작하는 첫 번째 아이템
    for (int i = 0; i < item_count; i++) {
        if (database[i].name[0] == 'I' && strcmp(database[i].key_frag, "NIL") != 0) {
            strcpy(key4, database[i].key_frag); break;
        }
    }

    sprintf(result, "%s%s%s%s", key1, key2, key3, key4);
}

// [문제 4] 회복(Heal): strcmp(NAME, SLOT) >= 0 인 첫 번째
void solve_heal(char* result) {
    for (int i = 0; i < item_count; i++) {
        if (strcmp(database[i].name, database[i].slot) >= 0) {
            strcpy(result, database[i].key_frag);
            return;
        }
    }
    strcpy(result, "FAIL");
}

// [문제 5] 원거리(Range): KEY_FRAG="*K*" 아이템 HP만큼 이동 후 읽기
// 주의: KEY_FRAG 파일에는 "*K*" 처럼 저장될 수 있으므로 포함 여부로 체크
void solve_range(char* result) {
    int offset = 0;
    // 1. K가 들어있는 키 찾기
    for (int i = 0; i < item_count; i++) {
        if (strstr(database[i].key_frag, "K") != NULL) {
            offset = database[i].hp;
            break;
        }
    }

    // 2. 파일 다시 열어서 바이트 단위 이동
    FILE* fp = fopen("AI1-2_C_Final.csv", "rb"); // 바이너리 읽기
    if (fp) {
        fseek(fp, offset, SEEK_SET);
        char buf[6] = { 0 };
        fread(buf, 1, 5, fp); // 5바이트 읽기
        fclose(fp);
        sprintf(result, "\"%s\"", buf); // 앞뒤 따옴표 추가
    }
    else {
        strcpy(result, "FAIL");
    }
}

// [문제 6] 축복(Bless): "Sword" 포함 Key 연결 -> '*'로 토큰화 -> 가장 긴 것
void solve_bless(char* result) {
    char combined[256] = "";
    for (int i = 0; i < item_count; i++) {
        if (strstr(database[i].name, "Sword") != NULL) {
            strcat(combined, database[i].key_frag);
        }
    }

    // 토큰 분리 (구분자 '*')
    // KEY_FRAG 예: *T*, *E* -> 합치면 *T**E*
    // 구분자로 자르면 T, E 가 나옴
    char* token = strtok(combined, "*");
    char longest[64] = "";

    while (token != NULL) {
        if (strlen(token) > strlen(longest)) {
            strcpy(longest, token);
        }
        token = strtok(NULL, "*");
    }
    strcpy(result, longest);
}

// [문제 7] 가로/세로(H_ATTACK): 긴 이름 앞3 + 짧은 저주 뒤3
void solve_line_attack(char* result) {
    int max_len = -1;
    int min_curse_len = 999;
    int long_idx = 0;
    int short_curse_idx = 0;

    for (int i = 0; i < item_count; i++) {
        int n_len = strlen(database[i].name);
        int c_len = strlen(database[i].curse);

        if (n_len > max_len) { max_len = n_len; long_idx = i; }
        if (c_len < min_curse_len) { min_curse_len = c_len; short_curse_idx = i; }
    }

    char part1[4] = { 0 };
    strncpy(part1, database[long_idx].name, 3);

    const char* curse_str = database[short_curse_idx].curse;
    const char* part2 = curse_str + strlen(curse_str) - 3; // 뒤에서 3글자 포인터

    sprintf(result, "%s%s", part1, part2);
}

// [문제 8] 비밀(Secrete): "Stone" 포함, 모음으로 자르고 가장 긴 토큰
void solve_secrete(char* result) {
    char target_name[64] = "";
    for (int i = 0; i < item_count; i++) {
        if (strstr(database[i].name, "Stone") != NULL) {
            strcpy(target_name, database[i].name);
            break;
        }
    }

    char* token = strtok(target_name, "AEIOUaeiou"); // 모음 구분자
    char longest[64] = "";

    while (token != NULL) {
        if (strlen(token) > strlen(longest)) {
            strcpy(longest, token);
        }
        token = strtok(NULL, "AEIOUaeiou");
    }
    strcpy(result, longest);
}


// [AI 로직] (기존 유지)
static int calculate_distance(const Player* p1, const Player* p2) {
    return abs(get_player_x(p1) - get_player_x(p2)) + abs(get_player_y(p1) - get_player_y(p2));
}

int simple_killer_ai(const Player* my_info, const Player* opponent_info) {
    // 1. 공격 가능하면 공격
    if (calculate_distance(my_info, opponent_info) <= 1) return CMD_ATTACK;

    // 2. 적 방향으로 이동
    int my_x = get_player_x(my_info);
    int opp_x = get_player_x(opponent_info);
    if (my_x < opp_x) return CMD_RIGHT;
    if (my_x > opp_x) return CMD_LEFT;

    int my_y = get_player_y(my_info);
    int opp_y = get_player_y(opponent_info);
    if (my_y < opp_y) return CMD_DOWN;
    return CMD_UP;
}

// =================================================================
// [MAIN ENTRY]
// =================================================================
void student1_ai_entry() {
    int my_secret_key = register_player_ai("TEAM-ALPHA", simple_killer_ai);

    // 1. 데이터 로드 (필수)
    load_data();

    char answer[256];

    // [1] 독 해금
    solve_poison(answer);
    attempt_skill_unlock(my_secret_key, CMD_POISON, answer);
    if (is_skill_unlocked(my_secret_key, CMD_POISON)) printf("[OK] POISON: %s\n", answer);
    else printf("[FAIL] POISON: %s\n", answer);

    // [2] 강타 해금
    solve_strike(answer);
    attempt_skill_unlock(my_secret_key, CMD_STRIKE, answer);
    if (is_skill_unlocked(my_secret_key, CMD_STRIKE)) printf("[OK] STRIKE: %s\n", answer);

    // [3] 점멸 해금
    solve_blink(answer);
    attempt_skill_unlock(my_secret_key, CMD_BLINK_DOWN, answer); // 4방향 자동 해금됨
    if (is_skill_unlocked(my_secret_key, CMD_BLINK_DOWN)) printf("[OK] BLINK: %s\n", answer);

    // [4] 회복 해금
    solve_heal(answer);
    attempt_skill_unlock(my_secret_key, CMD_HEAL_ALL, answer);
    if (is_skill_unlocked(my_secret_key, CMD_HEAL_ALL)) printf("[OK] HEAL: %s\n", answer);

    // [5] 원거리 해금
    solve_range(answer);
    attempt_skill_unlock(my_secret_key, CMD_RANGE_ATTACK, answer);
    if (is_skill_unlocked(my_secret_key, CMD_RANGE_ATTACK)) printf("[OK] RANGE: %s\n", answer);

    // [6] 축복 해금 (자폭 문제 로직 사용)
    solve_bless(answer);
    attempt_skill_unlock(my_secret_key, CMD_BLESS, answer);
    if (is_skill_unlocked(my_secret_key, CMD_BLESS)) printf("[OK] BLESS: %s\n", answer);

    // [7] 가로세로 해금
    solve_line_attack(answer);
    attempt_skill_unlock(my_secret_key, CMD_H_ATTACK, answer);
    if (is_skill_unlocked(my_secret_key, CMD_H_ATTACK)) printf("[OK] H/V ATTACK: %s\n", answer);

    // [8] 비밀 메시지
    solve_secrete(answer);
    attempt_skill_unlock(my_secret_key, CMD_SECRETE, answer);
    if (is_skill_unlocked(my_secret_key, CMD_SECRETE)) {
        set_custom_secrete_message(my_secret_key, "I AM READY");
        printf("[OK] SECRET UNLOCKED\n");
    }

    printf("TEAM-ALPHA : ALL SET. PRESS KEY.\n");
    getchar();
}