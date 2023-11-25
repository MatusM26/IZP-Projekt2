#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Definice struktury pro mapu
typedef struct {
    int rows;
    int cols;
    unsigned char *cells;
} Map;

// Podpůrná funkce pro inicializaci mapy
void initializeMap(Map *map, int rows, int cols) {
    map->rows = rows;
    map->cols = cols;
    map->cells = (unsigned char *) malloc(rows * cols * sizeof(unsigned char));
}

// Podpůrná funkce pro uvolnění paměti používané mapou
void freeMap(Map *map) {
    free(map->cells);
}

// Podpůrná funkce pro načítání mapy ze souboru
void loadMap(Map *map, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "[Error] Chyba pri otvarani suboru %s\n", filename);
        exit(EXIT_FAILURE);
    }

    // Načítání velikosti mapy
    fscanf(file, "%d %d", &map->rows, &map->cols);
    initializeMap(map, map->rows, map->cols);

    // Načítání hodnot jednotlivých políček
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
        case 0:  // Levá šikmá hranice
//            printf("Celltype 1: %i\n", (cellType & 1) != 0);
            return (cellType & 1) != 0;
        case 1:  // Pravá šikmá hranice
//            printf("Celltype 2: %i\n", (cellType & 2) != 0);
            return (cellType & 2) != 0;
        case 2:  // Horní nebo spodní hranice
//            printf("Celltype 4: %i\n", (cellType & 4) != 0);
            return (cellType & 4) != 0;
        default:
            // Neznámá hranice, mělo by být ošetřeno vhodným chováním
            return false;
    }
}

int going_left(Map *map, int r, int c, int current_top) {
    bool has_top_border = is_border(map, r, c, 2);
    bool has_left_border = is_border(map, r, c, 0);

    if (current_top) {
        if (!has_top_border)
            return 1; //row--
        else if (has_left_border)
            return 2; //col++
        else
            return 0; //col--
    } else {
        if (!has_left_border)
            return 0;
        else if (!has_top_border)
            return 3;
        else
            return 2;
    }
}

int going_right(Map *map, int r, int c, int current_top) {
    bool has_top_border = is_border(map, r, c, 2);
    bool has_right_border = is_border(map, r, c, 1);
//    printf("%i\n", current_top);
    if (current_top) {

        if (!has_right_border)
            return 2; //col++
        else if (!has_top_border)
            return 1; //row--
        else
            return 0; //col--
    } else {
        if (!has_top_border)
            return 3; //row++
        else if (!has_right_border)
            return 2; //col++
        else
            return 0; //col--
    }
}

int going_up(Map *map, int r, int c) {
    bool has_right_border = is_border(map, r, c, 1);
    bool has_left_border = is_border(map, r, c, 0);

    if (!has_right_border) {
//        printf("col++\n");
        return 2; //col++
    } else if (!has_left_border) {
        return 0; //col--
//        printf("col--\n");
    } else {
//        printf("row++\n");
        return 3;
    }
} //row++

int going_down(Map *map, int r, int c) {
    bool has_right_border = is_border(map, r, c, 1);
    bool has_left_border = is_border(map, r, c, 0);

    if (!has_left_border)
        return 0; //col--
    else if (!has_right_border)
        return 2; //col++
    else
        return 1; //row--
}

int start_border(Map *map, int r, int c, int leftright, int current_top, int current_col,
                 int current_row) {
    // Zjistíme, zda se nacházíme na sudém nebo lichém řádku
    int even_row = r % 2 == 0;

    // Zjistíme, zda má políčko horní a/nebo dolní hranici
    bool has_top_border = is_border(map, r, c, 2);

    // Zjistíme, zda máme pravou hranici
//    bool has_right_border = is_border(map, r, c, 1);

    // Zjistíme, zda máme levou hranici
    bool has_left_border = is_border(map, r, c, 0);

    // Nastavíme pravidla pro pravou a levou ruku
    if (leftright == 1) { // Pravá ruka
        if ((c == 1 && !even_row && !has_left_border) || (c == 1 && even_row && !has_left_border)) {
//            printf("right %i\n", going_right(map, current_row, current_col, current_top));
            return going_right(map, current_row, current_col, current_top);
        } else if (r == 1 && !even_row && !has_top_border) {
//            printf("down %i\n", going_down(map, current_row, current_col));
            return going_down(map, current_row, current_col);
        } else if (map->rows == current_row && !current_top && !has_top_border) {
//            printf("up %i\n", going_up(map, current_row, current_col));
            return going_up(map, current_row, current_col);
        }
    }

        // Vracíme defaultní hodnotu
    return -1;
}

void r_path(Map *map, int start_row, int start_col) {
    bool end = false;
    int current_row = start_row;
    int current_col = start_col;
    int current_top;

    current_top = top_bot(&current_col, &current_row);
    // Zjistíme, kterou hranici následovat podle pravidla levé ruky
    int next_border = start_border(map, current_row, current_col,
                                   1, current_top, current_col, current_row);

    // Hlavní smyčka pro hledání cesty
    while (!end) {
//        printf("%i\n", next_border);
        current_top = top_bot(&current_col, &current_row);


        // Vypis aktuální pozice (můžeš odebrat, pokud nechceš vypsávat každý krok)
        printf("%d,%d\n", current_row, current_col);

        // Pokud jsme našli cíl (v prvním sloupci), ukončíme hledání

        // Přesuneme se na další pozici v bludišti podle zvolené hranice
        switch (next_border) {
            case 0:
                if (current_col - 1 < 1)
                    end = true;
                current_col--;
//                printf("curtop0: %i\n", current_top);
                current_top = top_bot(&current_col, &current_row);
//                printf("curtop1: %i\n", current_top);
                next_border = going_left(map, current_row, current_col, current_top);
                break;
            case 1:
                if (current_row - 1 < 1)
                    end = true;
                current_row--;
                next_border = going_up(map, current_row, current_col);
                break;
            case 2:
                if (current_col + 1 > map->cols) {
                    end = true;
                }
                current_col++;
//                printf("curtop0: %i\n", current_top);
                current_top = top_bot(&current_col, &current_row);
//                printf("curtop1: %i\n", current_top);
                next_border = going_right(map, current_row, current_col, current_top);
                break;
            case 3:  // Nový case pro pohyb dopředu v řádku
                if (current_row + 1 > map->rows)
                    end = true;
                current_row++;
                next_border = going_up(map, current_row, current_col);
                break;
            default:
                return;  // Pokud není definována žádná hranice, ukončíme hledání
        }
    }
}

/*void l_path(Map *map, int start_row, int start_col, int *current_top) {
    int current_row = start_row;
    int current_col = start_col;

    // Hlavní smyčka pro hledání cesty
    while (1) {
        // Vypis aktuální pozice (můžeš odebrat, pokud nechceš vypsávat každý krok)
        printf("%d,%d\n", current_row, current_col);

        // Zjistíme, kterou hranici následovat podle pravidla levé ruky
        int next_border = start_border(map, current_row, current_col, 1, current_top);

        // Pokud jsme našli cíl (v prvním sloupci), ukončíme hledání
        if ((next_border == 0 && current_col == 0) ||
            (next_border == 2 && (current_row == map->rows)) ||
            (next_border == 3 && current_col == 0)) {
            break;
        }

        // Přesuneme se na další pozici v bludišti podle zvolené hranice
        switch (next_border) {
            case 0:
                current_col--;
                break;
            case 1:
                current_row--;
                break;
            case 2:
                current_col++;
                break;
            case 3:  // Nový case pro pohyb dopředu v řádku
                current_row++;
                break;
            default:
                return;  // Pokud není definována žádná hranice, ukončíme hledání
        }
    }
}*/

// Testovací funkce pro výpis načteného bludiště
/*void printMaze(Map *map) {
    for (int i = 0; i < map->rows; ++i) {
        for (int j = 0; j < map->cols; ++j) {
            printf("%hhu ", map->cells[i * map->cols + j]);
        }
        printf("\n");
    }
}*/

int main(int argc, char *argv[]) {
    if (argc == 5) {
        Map maze;
        loadMap(&maze, argv[4]);

//        printMaze(&maze);

        int start_row = atoi(argv[2]);
        int start_col = atoi(argv[3]);

        if (strcmp(argv[1], "--rpath") == 0) {
            // Spustení pravidla pravé ruky
            r_path(&maze, start_row, start_col);
        } else if (strcmp(argv[1], "--lpath") == 0) {
            // Spustení pravidla levé ruky
            printf("[Kontrola]Pravidlo lavej ruky\n");
//            l_path(&maze, start_row, start_col, &current_top);
        } else {
            fprintf(stderr, "[Error] Nespravny format vstupu\n");
            return 1;
        }

        // Uvolnění paměti používané mapou
        freeMap(&maze);
    }

    if (argc < 3) {
        fprintf(stderr, "[Error] Nespravny format vstupu\n");
        return 1;
    }

    return 0;
}

