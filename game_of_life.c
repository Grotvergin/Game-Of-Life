#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define N 25
#define M 80
#define DELAY 100000
#define SENSITIVITY 20000

short dynamic_input(short ***a, short n, short m);
void dynamic_malloc(short ***a, short n, short m);
void reset_field(short **data);
short menu(short **data);
void output_dynamic(short **data, int delay, short alive_counter);
short input_num(short min, short max, short *value);
short is_empty(short **data);
void how_many_alive(short **data, short *alive_counter);
short to_live_or_die(short **data, short x, short y);

int main() {
    short **data, **temp;
    short alive_counter = 0;
    dynamic_malloc(&data, N, M);
    dynamic_malloc(&temp, N, M);
    reset_field(data);
    printf("0: SWITCH TO MATRIX MODE\n1 < INT <= 2000: COORDINATE MODE\n");
    if (!menu(data)) {
        initscr();
        noecho();
        nodelay(stdscr, true);
        curs_set(0);
        if (freopen("/dev/tty", "r", stdin) != NULL) {
            keypad(stdscr, true);
            int button = 0;
            int delay = DELAY;
            output_dynamic(data, delay, alive_counter);
            while (is_empty(data) && button != 'q' && button != 'Q') {
                alive_counter = 0;
                how_many_alive(data, &alive_counter);
                output_dynamic(data, delay, alive_counter);
                for (int i = 0; i < N; ++i) {
                    for (int j = 0; j < M; ++j) {
                        temp[i][j] = to_live_or_die(data, i, j);
                    }
                }
                for (int i = 0; i < N; ++i) {
                    for (int j = 0; j < M; ++j) {
                        data[i][j] = temp[i][j];
                    }
                }
                button = getch();
                if ((button == 0403) && delay > SENSITIVITY) {
                    delay -= SENSITIVITY;
                } else if (button == 0402) {
                    delay += SENSITIVITY;
                } else if (((button == 'P') || (button == 'p')) && delay < DELAY * 100 &&
                           delay > SENSITIVITY) {
                    delay = DELAY * 100;
                } else if (((button == 'P') || (button == 'p')) && (delay >= DELAY * 100)) {
                    delay = DELAY;
                }

                usleep(delay);
            }
            endwin();
        }
    }
    free(data);
    free(temp);
    return 0;
}

short input_num(short min, short max, short *value) {
    float temp;
    short flag = 0;
    if (scanf("%f", &temp) && temp == (short)temp && temp <= max && temp >= min) {
        *value = (short)temp;
        flag = 1;
    }
    return flag;
}

short dynamic_input(short ***a, short n, short m) {
    short flag = 1;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (!(input_num(0, 1, &(*a)[i][j]))) {
                flag = 0;
            }
        }
    }
    return flag;
}

void dynamic_malloc(short ***a, short n, short m) {
    (*a) = malloc(n * m * sizeof(short) + n * sizeof(short *));
    short *p = (short *)((*a) + n);
    for (int i = 0; i < n; ++i) {
        (*a)[i] = p + m * i;
    }
}

void reset_field(short **data) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            data[i][j] = 0;
        }
    }
}

short menu(short **data) {
    short n, m, hm, flag = 0;
    if (!input_num(0, N * M, &hm)) {
        flag = 1;
    }
    if (hm == 0 && !flag) {
        printf("INSERT MATRIX SIZE NxM, THEN NxM (0/1)\n");
        if (!input_num(1, N, &n) || !input_num(1, M, &m)) {
            flag = 1;
        } else {
            short **personal_matrix;
            dynamic_malloc(&personal_matrix, n, m);
            if (dynamic_input(&personal_matrix, n, m)) {
                for (int i = 0; i < n; ++i) {
                    for (int j = 0; j < m; ++j) {
                        data[i][j] = personal_matrix[i][j];
                    }
                }
            }
            free(personal_matrix);
        }
    }
    if (!flag) {
        for (int i = 0; i < hm && !flag; i++) {
            if (!input_num(0, N - 1, &n) || !input_num(0, M - 1, &m)) {
                flag = 1;
            } else {
                data[n][m] = 1;
            }
        }
    }
    return flag;
}

void output_dynamic(short **data, int delay, short alive_counter) {
    printw("\n      Q: QUIT | P: PAUSE | KEY_UP/KEY_DOWN: DELAY %d | ALIVE: %d/%d ", delay, alive_counter,
           N * M);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            if (data[i][j] == 1) {
                mvaddch(i, j, ' ' | A_STANDOUT);
            } else {
                mvaddch(i, j, '.' | A_DIM);
            }
        }
        mvaddch(i, M, '\n');
    }
}

short is_empty(short **data) {
    short is = 0;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            if (data[i][j] == 1) {
                is = 1;
            }
        }
    }
    return is;
}

void how_many_alive(short **data, short *alive_counter) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            if (data[i][j] == 1) {
                (*alive_counter)++;
            }
        }
    }
}

short to_live_or_die(short **data, short x, short y) {
    short is_live = 0, count_live = 0;
    for (int i = x - 1; i <= x + 1; ++i) {
        for (int j = y - 1; j <= y + 1; ++j) {
            short help_i = i, help_j = j;
            if (i < 0) {
                help_i += N;
            } else if (i > N - 1) {
                help_i -= N;
            }
            if (j < 0) {
                help_j += M;
            } else if (j > M - 1) {
                help_j -= M;
            }
            if (data[help_i][help_j] == 1) {
                count_live++;
            }
        }
    }
    if (data[x][y] == 1) {
        count_live -= 1;
        if (count_live == 2 || count_live == 3) {
            is_live = 1;
        } else {
            is_live = 0;
        }
    } else {
        if (count_live == 3) {
            is_live = 1;
        } else {
            is_live = 0;
        }
    }
    return is_live;
}