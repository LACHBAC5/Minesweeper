#include "mines.h"

// allocate heap space for field
void m_init(struct minesweeper* mines, int width, int height){
    mines->grid = calloc(width*height, sizeof(struct field_tile));
    mines->width=width; mines->height=height;
}

// destroy heap space of field
void m_fina(struct minesweeper* mines){
    if(mines->grid>0){
        free(mines->grid);
    }
}

// Place a bomb at location no checks. Increment surrounding values if not bombs.
void m_place_bomb(struct minesweeper* mines, int x, int y){
    mines->grid[y*mines->width+x].number=-1;
    if(x>0 && mines->grid[y*mines->width+x-1].number>-1){
        mines->grid[y*mines->width+x-1].number++;
    }
    if(x<mines->width-1 && mines->grid[y*mines->width+x+1].number>-1){
        mines->grid[y*mines->width+x+1].number++;
    }
    if(y>0 && mines->grid[(y-1)*mines->width+x].number>-1){
        mines->grid[(y-1)*mines->width+x].number++;
    }
    if(y<mines->height-1 && mines->grid[(y+1)*mines->width+x].number>-1){
        mines->grid[(y+1)*mines->width+x].number++;
    }

    if(x>0 && y>0 && mines->grid[(y-1)*mines->width+x-1].number>-1){
        mines->grid[(y-1)*mines->width+x-1].number++;
    }
    if(x<mines->width-1 && y<mines->height-1 && mines->grid[(y+1)*mines->width+x+1].number>-1){
        mines->grid[(y+1)*mines->width+x+1].number++;
    }
    if(x>0 && y<mines->height-1 && mines->grid[(y+1)*mines->width+x-1].number>-1){
        mines->grid[(y+1)*mines->width+x-1].number++;
    }
    if(x<mines->width-1 && y>0 && mines->grid[(y-1)*mines->width+x+1].number>-1){
        mines->grid[(y-1)*mines->width+x+1].number++;
    }
}

// Place a bomb at location if there isn't one yet, else search for a spot. (DFS)
void m_place_bomb_recursive(struct minesweeper* mines, int x, int y, bool* table){
    if(table[0]==true || table[y*mines->width+x+1]==true || x>=mines->width || x < 0 || y >= mines->height || y < 0){
        return;
    }

    table[y*mines->width+x+1]=true;

    if(mines->grid[y*mines->width+x].number>-1){
        m_place_bomb(mines, x, y);
        table[0]=true;
        return;
    }

    m_place_bomb_recursive(mines, x-1, y, table);
    m_place_bomb_recursive(mines, x+1, y, table);
    m_place_bomb_recursive(mines, x, y-1, table);
    m_place_bomb_recursive(mines, x, y+1, table);

    m_place_bomb_recursive(mines, x-1, y-1, table);
    m_place_bomb_recursive(mines, x+1, y+1, table);
    m_place_bomb_recursive(mines, x+1, y-1, table);
    m_place_bomb_recursive(mines, x-1, y+1, table);
}

// For times in mine_count place a bomb using m_place_bomb_recursive(...).
void m_gen_field(struct minesweeper* mines, int mine_count){
    memset(mines->grid, 0, mines->width*mines->height*sizeof(struct field_tile));
    for(int i = 0; i < mine_count; i++){
        int x = genInt(0, mines->width), y = genInt(0, mines->height);
        
        bool* table = malloc(mines->width*mines->height+1); memset(table, 0, mines->width*mines->height+1);
        m_place_bomb_recursive(mines, x, y, table);
        free(table);
    }
}

// Allocate a lookup array, call m_reveal_recursive(...), free array.
int m_reveal_tool(struct minesweeper* mines, int x, int y){
    bool* table = malloc(mines->width*mines->height+1); memset(table, 0, mines->width*mines->height+1);
    int out = m_reveal_recursive(mines, x, y, table);
    if(table[0]==true){
        out=-1;
    }
    free(table);

    return out;
}

// reveal each bordering square recursively if the number of flags equals the sqare number.
int m_reveal_recursive(struct minesweeper* mines, int x, int y, bool* table){
    if(table[y*mines->width+x+1]==true || table[0]==true || mines->grid[y*mines->width+x].flag==true || x>=mines->width || x < 0 || y >= mines->height || y < 0){
        return 0;
    }
    else if(mines->grid[y*mines->width+x].number==-1&&mines->grid[y*mines->width+x].flag==false){
        table[0]=true;
        return 0;
    }

    int sum_flags=0;
    if(x>0 && mines->grid[y*mines->width+x-1].flag==true){
        sum_flags++;
    }
    if(x<mines->width-1 && mines->grid[y*mines->width+x+1].flag==true){
        sum_flags++;
    }
    if(y>0 && mines->grid[(y-1)*mines->width+x].flag==true){
        sum_flags++;
    }
    if(y<mines->height-1 && mines->grid[(y+1)*mines->width+x].flag==true){
        sum_flags++;
    }
    if(x>0 && y>0 && mines->grid[(y-1)*mines->width+x-1].flag==true){
        sum_flags++;
    }
    if(x<mines->width-1 && y<mines->height-1 && mines->grid[(y+1)*mines->width+x+1].flag==true){
        sum_flags++;
    }
    if(x>0 && y<mines->height-1 && mines->grid[(y+1)*mines->width+x-1].flag==true){
        sum_flags++;
    }
    if(x<mines->width-1 && y>0 && mines->grid[(y-1)*mines->width+x+1].flag==true){
        sum_flags++;
    }

    table[y*mines->width+x+1]=true;

    int sum=0;
    if(sum_flags>=mines->grid[y*mines->width+x].number){
        sum = m_reveal_recursive(mines, x-1, y, table)+
              m_reveal_recursive(mines, x+1, y, table)+
              m_reveal_recursive(mines, x, y-1, table)+
              m_reveal_recursive(mines, x, y+1, table)+
              m_reveal_recursive(mines, x-1, y-1, table)+
              m_reveal_recursive(mines, x+1, y+1, table)+
              m_reveal_recursive(mines, x+1, y-1, table)+
              m_reveal_recursive(mines, x-1, y+1, table);
    }

    if(mines->grid[y*mines->width+x].visible==false){
        mines->grid[y*mines->width+x].visible=true;
        sum++;
    }
    return sum;
}

// if no flag place one else remove it.
void m_flag_tool(struct minesweeper* mines, int x, int y){
    if(mines->grid[y*mines->width+x].flag==true){
        mines->grid[y*mines->width+x].flag=false;
    }
    else if(mines->grid[y*mines->width+x].visible==false)
    {
        mines->grid[y*mines->width+x].flag=true;
    }
}

// get tile by coords.
struct field_tile* m_get_tile(struct minesweeper* mines, int x, int y){
    return &mines->grid[y*mines->width+x];
}
