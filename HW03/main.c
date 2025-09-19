#include <stdio.h>
#include <windows.h>

void gotoxy(int x, int y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// 폭발 전/후 폭탄 출력
// isExploded: 0=폭발 전, 1=폭발
void printBomb(int isExploded) {
    if (isExploded) {
        printf("\x1b[A^^^^^^^");
        printf("\x1b[B\x1b[7D!!BAM!!");
        printf("\x1b[B\x1b[7D^^^^^^^");
    }
    else {
        printf("(  b  )");
    }
}

int main(void) {
    int sx = 14, sy = 9; // 시작 좌표 저장
    int x = sx, y = sy;

    gotoxy(x, y);
    printf("*"); // 시작점: * 로 표시

    // 나선 이동 길이 (각 방향별)
    int steps[] = {
        1, 1,  // ↓ → 
        2, 2,  // ↑ ←
        4, 4,  // ↓ → 
        6, 6,  // ↑ ←
        8, 8,  // ↓ → 
        10,10, // ↑ ←
        12,12, // ↓ → 
        14,14  // ↑ ← (마지막 폭탄 위치)
    };

    // 방향: 0=아래,1=오른쪽,2=위,3=왼쪽
    int dir = 0;
    int nsteps = sizeof(steps) / sizeof(steps[0]);

    /* --------------- 1) 전체 모양 먼저 그리기 --------------- */
    {
        int tx = sx, ty = sy;   // 임시 펜 위치
        int tdir = 0;           // 임시 방향
        for (int i = 0; i < nsteps; i++) {
            for (int j = 0; j < steps[i]; j++) {
                if (tdir == 0) ty++;      // ↓
                else if (tdir == 1) tx++; // →
                else if (tdir == 2) ty--; // ↑
                else if (tdir == 3) tx--; // ←

                gotoxy(tx, ty);
                if (i == nsteps - 1 && j == steps[i] - 1) {
                    // 마지막 칸: 폭탄 설치
                    printBomb(0);
                }
                else {
                    // 나머지: #
                    putchar('#');
                }
            }
            tdir = (tdir + 1) % 4; // 회전
        }
    }

    /* --------------- 2) 지우면서 이동하는 애니메이션 --------------- */
    // 펜을 시작점으로 되돌리고, 동일 경로를 따라가며 "직전 위치"를 지움
    gotoxy(sx, sy);
    x = sx; y = sy; dir = 0;

    int px = x, py = y; // 직전에 있던 위치(지울 대상) — 처음엔 시작점('*')
    for (int i = 0; i < nsteps; i++) {
        for (int j = 0; j < steps[i]; j++) {
            // (a) 직전 위치 지우기
            gotoxy(px, py);
            putchar(' ');

            // (b) 한 칸 이동
            if (dir == 0) y++;      // ↓
            else if (dir == 1) x++; // →
            else if (dir == 2) y--; // ↑
            else if (dir == 3) x--; // ←

            // (c) 현재 위치로 커서만 이동(그리지는 않음: 이미 전체 모양이 그려져 있음)
            gotoxy(x, y);

            // 다음 루프에서 지울 위치 업데이트
            px = x; py = y;

            Sleep(200); // 200ms로 속도 조절
        }
        dir = (dir + 1) % 4; // 시계방향 회전
    }

    // 3) 폭발 애니메이션 (마지막 칸에는 이미 폭탄 설치됨)
    Sleep(1000);
    gotoxy(x, y);
    printBomb(1);

    gotoxy(0, 30); // 커서 이동
    return 0;
}