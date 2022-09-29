#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "random.h"

struct field_tile {
    int number;
    bool flag, visible;
};

struct minesweeper {
    struct field_tile* grid; int width, height;
};

void m_init(struct minesweeper* mines, int width, int height);
void m_fina(struct minesweeper* mines);

void m_place_bomb(struct minesweeper* mines, int x, int y);
void m_place_bomb_recursive(struct minesweeper* mines, int x, int y, bool* table);

void m_gen_field(struct minesweeper* mines, int mine_count);

int m_reveal_tool(struct minesweeper* mines, int x, int y);
int m_reveal_recursive(struct minesweeper* mines, int x, int y, bool* table);

void m_flag_tool(struct minesweeper* mines, int x, int y);

struct field_tile* m_get_tile(struct minesweeper* mines, int x, int y);
