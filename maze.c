#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    int rows;
    int cols;
    unsigned char *cells;
} Map;

void initializeMap(Map *map, int rows, int cols) {
    map->rows = rows;
    map->cols = cols;
    map->cells = (unsigned char *) malloc(rows * cols * sizeof(unsigned char));
}

void freeMap(Map *map) {
    free(map->cells);
}

void loadMap(Map *map, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "[Error] Chyba pri otvarani suboru %s\n", filename);
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d %d", &map->rows, &map->cols);
    initializeMap(map, map->rows, map->cols);

    for (int i = 1; i <= map->rows; ++i) {
        for (int j = 1; j <= map->cols; ++j) {
            fscanf(file, "%hhu", &map->cells[i * map->cols + j]);
        }
    }

    fclose(file);
}

int top_bot(int *col, int *row) {
    if ((*row % 2 == 0 && *col % 2 == 0) || (*row % 2 != 0 && *col % 2 != 0))
        return 1;
    else if ((*row % 2 == 0 && *col % 2 != 0) || (*row % 2 != 0 && *col % 2 == 0))
        return 0;
    else return -1;
}

bool is_border(Map *map, int r, int c, int border) {
    unsigned char cellType = map->cells[r * map->cols + c];
    switch (border) {
        case 0:
            return (cellType & 1) != 0;
        case 1:
            return (cellType & 2) != 0;
        case 2:
            return (cellType & 4) != 0;
        default:
            return false;
    }
}

int going_left_l(Map *map, int r, int c, int current_top) {
    bool has_top_border = is_border(map, r, c, 2);
    bool has_left_border = is_border(map, r, c, 0);

    if (current_top) {
        if (!has_left_border)
            return 0;
        else if (!has_top_border)
            return 1;
        else
            return 2;
    } else {
        if (!has_top_border)
            return 3;
        else if (!has_left_border)
            return 0;
        else
            return 2;
    }
}

int going_right_l(Map *map, int r, int c, int current_top) {
    bool has_top_border = is_border(map, r, c, 2);
    bool has_right_border = is_border(map, r, c, 1);

    if (current_top) {
        if (!has_top_border)
            return 1;
        else if (!has_right_border)
            return 2;
        else
            return 0;
    } else {
        if (!has_right_border)
            return 2;
        else if (!has_top_border)
            return 3;
        else
            return 0;
    }
}

int going_up_l(Map *map, int r, int c) {
    bool has_right_border = is_border(map, r, c, 1);
    bool has_left_border = is_border(map, r, c, 0);

    if (!has_left_border) {
        return 0;
    } else if (!has_right_border) {
        return 2;
    } else {
        return 3;
    }
}

int going_down_l(Map *map, int r, int c) {
    bool has_right_border = is_border(map, r, c, 1);
    bool has_left_border = is_border(map, r, c, 0);

    if (!has_right_border)
        return 2;
    else if (!has_left_border)
        return 0;
    else
        return 1;
}

int going_left_r(Map *map, int r, int c, int current_top) {
    bool has_top_border = is_border(map, r, c, 2);
    bool has_left_border = is_border(map, r, c, 0);

    if (current_top) {
        if (!has_top_border)
            return 1;
        else if (!has_left_border)
            return 0;
        else
            return 2;
    } else {
        if (!has_left_border)
            return 0;
        else if (!has_top_border)
            return 3;
        else
            return 2;
    }
}

int going_right_r(Map *map, int r, int c, int current_top) {
    bool has_top_border = is_border(map, r, c, 2);
    bool has_right_border = is_border(map, r, c, 1);

    if (current_top) {
        if (!has_right_border)
            return 2;
        else if (!has_top_border)
            return 1;
        else
            return 0;
    } else {
        if (!has_top_border)
            return 3;
        else if (!has_right_border)
            return 2;
        else
            return 0;
    }
}

int going_up_r(Map *map, int r, int c) {
    bool has_right_border = is_border(map, r, c, 1);
    bool has_left_border = is_border(map, r, c, 0);

    if (!has_right_border) {
        return 2;
    } else if (!has_left_border) {
        return 0;
    } else {
        return 3;
    }
}

int going_down_r(Map *map, int r, int c) {
    bool has_right_border = is_border(map, r, c, 1);
    bool has_left_border = is_border(map, r, c, 0);

    if (!has_left_border)
        return 0;
    else if (!has_right_border)
        return 2;
    else
        return 1;
}

int start_border(Map *map, int r, int c, int leftright, int current_top, int current_col,
                 int current_row) {

    bool has_top_border = is_border(map, r, c, 2);
    bool has_right_border = is_border(map, r, c, 1);
    bool has_left_border = is_border(map, r, c, 0);

    if (leftright == 1) {
        if (c == 1 && !has_top_border) {
            return going_down_r(map, current_row, current_col);
        } else if (c == 1 && !has_left_border) {
            return going_right_r(map, current_row, current_col, current_top);
        } else if (r == 1 && !has_top_border) {
            return going_down_r(map, current_row, current_col);
        } else if (map->rows == current_row && !current_top && !has_top_border) {
            return going_up_r(map, current_row, current_col);
        } else if (map->cols == current_col && !has_right_border) {
            return going_left_r(map, current_row, current_col, current_top);
        }
    } else if (leftright == 0) {
        if (c == 1 && !has_top_border) {
            return going_down_r(map, current_row, current_col);
        } else if (c == 1 && !has_left_border) {
            return going_right_r(map, current_row, current_col, current_top);
        } else if (r == 1 && !has_top_border) {
            return going_down_r(map, current_row, current_col);
        } else if (map->rows == current_row && !current_top && !has_top_border) {
            return going_up_r(map, current_row, current_col);
        } else if (map->cols == current_col && !has_right_border) {
            return going_left_r(map, current_row, current_col, current_top);
        }
    }

    return -1;
}

void r_path(Map *map, int start_row, int start_col) {
    bool end = false;
    int current_row = start_row, current_col = start_col, current_top;

    current_top = top_bot(&current_col, &current_row);
    int next_border = start_border(map, current_row, current_col,
                                   1, current_top, current_col, current_row);

    while (!end) {
        current_top = top_bot(&current_col, &current_row);

        printf("%d,%d\n", current_row, current_col);

        switch (next_border) {
            case 0:
                if (current_col - 1 < 1)
                    end = true;
                current_col--;
                current_top = top_bot(&current_col, &current_row);
                next_border = going_left_r(map, current_row, current_col, current_top);
                break;
            case 1:
                if (current_row - 1 < 1)
                    end = true;
                current_row--;
                next_border = going_up_r(map, current_row, current_col);
                break;
            case 2:
                if (current_col + 1 > map->cols) {
                    end = true;
                }
                current_col++;
                current_top = top_bot(&current_col, &current_row);
                next_border = going_right_r(map, current_row, current_col, current_top);
                break;
            case 3:
                if (current_row + 1 > map->rows)
                    end = true;
                current_row++;
                next_border = going_down_r(map, current_row, current_col);
                break;
            default:
                return;
        }
    }
}

void l_path(Map *map, int start_row, int start_col) {
    bool end = false;
    int current_row = start_row;
    int current_col = start_col;
    int current_top;

    current_top = top_bot(&current_col, &current_row);

    int next_border = start_border(map, current_row, current_col,
                                   0, current_top, current_col, current_row);

    while (!end) {
        current_top = top_bot(&current_col, &current_row);
        printf("%d,%d\n", current_row, current_col);

        switch (next_border) {
            case 0:
                if (current_col - 1 < 1)
                    end = true;
                current_col--;
                current_top = top_bot(&current_col, &current_row);
                next_border = going_left_l(map, current_row, current_col, current_top);
                break;
            case 1:
                if (current_row - 1 < 1)
                    end = true;
                current_row--;
                next_border = going_up_l(map, current_row, current_col);
                break;
            case 2:
                if (current_col + 1 > map->cols) {
                    end = true;
                }
                current_col++;
                current_top = top_bot(&current_col, &current_row);
                next_border = going_right_l(map, current_row, current_col, current_top);
                break;
            case 3:
                if (current_row + 1 > map->rows)
                    end = true;
                current_row++;
                next_border = going_down_l(map, current_row, current_col);
                break;
            default:
                return;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc == 5) {
        Map maze;
        loadMap(&maze, argv[4]);

        int start_row = atoi(argv[2]);
        int start_col = atoi(argv[3]);

        if (strcmp(argv[1], "--rpath") == 0) {

            r_path(&maze, start_row, start_col);
        } else if (strcmp(argv[1], "--lpath") == 0) {

            printf("[Kontrola]Pravidlo lavej ruky\n");
            l_path(&maze, start_row, start_col);
        } else {
            fprintf(stderr, "[Error] Nespravny format vstupu\n");
            return 1;
        }

        freeMap(&maze);
    }

    if (argc < 3) {
        fprintf(stderr, "[Error] Nespravny format vstupu\n");
        return 1;
    }

    return 0;
}