#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CAN(x) (1 << (x)) //候选数 mask
#define NUM CAN(10)       //已填写标识位
#define BLANK ((NUM)-2)   //未标记初始值

#define IS_NUM(i, j) (grid[i][j] & NUM) //是否已经填写

#define START(i) i / 3 * 3  //获取当前格所在块的起始行/列
#define END(i) START(i) + 3 //获取当前格所在块的末尾行/列

#define each(x, start, end) (char x = start; x < end; x++) //模拟 for in

void read_data(const char *filename);
void print_data(int status);

short get_num(short mask);

short grid[9][9];

/* 标记: 按照数独规则标记所有未填充格子的“候选数” */
void mark();
/* 清除不可能候选数 */
void clean();
/* 更新:
 * 1. 某格只剩一个候选数
 * 2. 当前行、列、块內某格候选数只在此格可填
 * 遇到上述情况，将这个格子填充为这个候选数
 */
int update();

int main(int argc, char *argv[]) {
    read_data(argv[1]);
    print_data(0);
    do {
        mark();
        clean();
    } while (update());

    print_data(1);
}

void mark() {
    for each (i, 0, 9) {
        for each (j, 0, 9) {
            if (!IS_NUM(i, j)) continue;
            for each (k, 0, 9) {
                if (!IS_NUM(i, k)) grid[i][k] &= ~grid[i][j];
                if (!IS_NUM(k, j)) grid[k][j] &= ~grid[i][j];
            }
            for each (k, START(i), END(i)) {
                for each (l, START(j), END(j)) {
                    if (!IS_NUM(k, l)) grid[k][l] &= ~grid[i][j];
                }
            }
        }
    }
}

void clean() {}

int update() {
    int cnt = 0;
    for each (i, 0, 9) {
        for each (j, 0, 9) {
            if (!IS_NUM(i, j)) {
                //唯一候选数
                if (get_num(grid[i][j])) {
                    grid[i][j] |= NUM;
                    cnt++;
                }
                short mask, tmp;
                //当前行
                tmp = mask = grid[i][j];
                grid[i][j] = 0;
                for each (k, 0, 9) mask &= ~grid[i][k];
                if (mask) goto fill;
                else grid[i][j] = tmp;
                //当前列
                tmp = mask = grid[i][j];
                grid[i][j] = 0;
                for each (k, 0, 9) mask &= ~grid[k][j];
                if (mask) goto fill;
                else grid[i][j] = tmp;
                //当前块
                tmp = mask = grid[i][j];
                grid[i][j] = 0;
                for each (k, START(i), END(i)) {
                    for each (l, START(j), END(j)) {
                        mask &= ~grid[k][l];
                    }
                }
                if (!mask) {
                    grid[i][j] = tmp;
                    continue;
                }

                goto fill;
            fill:
                grid[i][j] = mask | NUM;
                cnt++;
            }
        }
    }
    return cnt;
}

void read_data(const char *filename) {
    int file = open(filename, O_RDONLY);
    if (file < 0) {
        perror("open file");
        exit(1);
    }

    int i = 0;
    char buf;
    while (i < 81 && read(file, &buf, 1)) {
        if (isdigit(buf)) {
            short *curr = &grid[i / 9][i % 9];
            if (buf != '0') {
                *curr = CAN(buf - '0') | NUM;
            } else {
                *curr = BLANK;
            }
            i++;
        }
    }
    if (i < 81) {
        perror("not enough data");
        exit(1);
    }
}

short get_num(short mask) {
    mask &= BLANK;
    for each (i, 1, 10) {
        if (CAN(i) == mask)
            return i;
    }
    return 0;
}

void print_data(int status) {
    printf("+---------------------------------+---------------------------------+---------------------------------+\n");
    for each (i, 0, 9) {
        printf("|");
        for each (j, 0, 9) {
            if (IS_NUM(i, j)) {
                printf("[    %d    ]", get_num(grid[i][j]));
            } else {
                char mask[12] = "[         ]";
                if (status) {
                    for each (k, 1, 10) {
                        if (CAN(k) & grid[i][j])
                            mask[k] = '0' + k;
                    }
                }
                printf("%s", mask);
            }
            if (j % 3 == 2) {
                printf("|");
            }
        }
        printf("\n");
        if (i % 3 == 2) {
            printf("+---------------------------------+---------------------------------+---------------------------------+\n");
        }
    }
}