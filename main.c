#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "mines.h"

struct int2 {
    int x, y;
};

struct minesweeper_info {
    int current_bomb_count;
    int current_unknown_count;
};

// get normal stdin input in raw mode
void get_uinput(char* buffer, int length){
    char c; int index=0;
    while((c=getchar())!='\n'&&index<length){
        if(c==127){
            if(index>0){
                index--;
            }
        }
        else
        {
            buffer[index]=c;
            index++;
        }
    }
    buffer[index]='\0';
}

void m_gen_field_u_info(struct minesweeper* mines, struct minesweeper_info* info, int bomb_count){
    m_gen_field(mines, bomb_count);
    info->current_bomb_count=bomb_count;
    info->current_unknown_count=mines->width*mines->height;
}

int m_reveal_tool_u_info(struct minesweeper* mines, struct minesweeper_info* info, int x, int y){
    int count = m_reveal_tool(mines, x, y);

    info->current_unknown_count-=count;

    return count;
}

int main(){
    // init minesweeper
    struct minesweeper_info mines_def;
    struct minesweeper mines;
    m_init(&mines, 10, 10);
    m_gen_field_u_info(&mines, &mines_def, 10);

    // set stdin to raw mode
    struct termios first, second;
    tcgetattr(0, &first);
    second=first;
    second.c_lflag &= ~ECHO;
    second.c_lflag &= ~ICANON;
    tcsetattr(0, TCSANOW, &second);

    // makes cursor invisible
    printf("\x1B[?25l");

    struct int2 cursor; cursor.x=1; cursor.y=1;

    bool quit = false;
    while(!quit){
        // clear screen
        printf("\x1B[H\x1B[J");

        // draw minefield
        for(int i = 0; i < mines.height; i++){
            for(int o = 0; o < mines.width; o++){
                if(mines.grid[i*mines.width+o].flag==true){
                    printf("/ ");
                }
                else if(mines.grid[i*mines.width+o].visible==true){
                    printf("%s%i%c%c%s", "\x1B[1;3", mines.grid[i*mines.width+o].number, 'm', '0'+mines.grid[i*mines.width+o].number, "\x1B[0;0;0m ");
                }
                else
                {
                    printf("# ");
                }
            }
            printf("\n");
        }

        // draw cursor
        printf("\x1B[%i;%iH\x1B[1;36mX\x1B[0;0;0m", cursor.y, 2*cursor.x-1);

        char c = getchar();
        if(c==68 && cursor.x>1){                // left arrow
            cursor.x--;
        }
        else if(c==67 && cursor.x<mines.width){ // right arrow
            cursor.x++;
        }
        else if(c==65 && cursor.y>1){           // up arrow
            cursor.y--;
        }
        else if(c==66 && cursor.y<mines.height){// down arrow
            cursor.y++;
        }
        else if(c=='q'){        // quit
            printf("\x1B[H\x1B[J");
            quit=true;
        }
        else if(c=='w'){        // gen field
            char bomb_count_str[32];
            struct winsize w; ioctl(0, TIOCGWINSZ, &w);

            printf("\x1B[%i;%iHinput=bombs", w.ws_col, 1);
            get_uinput(bomb_count_str, sizeof(bomb_count_str));

            m_gen_field_u_info(&mines, &mines_def, atoi(bomb_count_str));
        }
        else if(c=='e'){        // change field size
            m_fina(&mines);

            struct winsize w;
            ioctl(0, TIOCGWINSZ, &w);

            char field_width_str[32]; char field_height_str[32];
            printf("\x1B[%i;%iHinput=width", w.ws_col, 1);
            get_uinput(field_width_str, sizeof(field_width_str));
            printf("\x1B[%i;%iHinput=height", w.ws_col, 1);
            get_uinput(field_height_str, sizeof(field_height_str));

            m_init(&mines, atoi(field_width_str), atoi(field_height_str));
        }
        else if(c=='a'){        // reaveal tool
            int reveal_count = m_reveal_tool_u_info(&mines, &mines_def, cursor.x-1, cursor.y-1);
            if(reveal_count<0){
                printf("\x1B[%i;%iH\x1B[1;31mYOU LOST...\x1B[0;0;0m", mines.height/2, mines.width-5);
                while((c=getchar())!='\n') {}
                m_gen_field_u_info(&mines, &mines_def, mines_def.current_bomb_count);
            }
            else if(mines_def.current_bomb_count==mines_def.current_unknown_count){
                printf("\x1B[%i;%iH\x1B[1;32mYOU WON...\x1B[0;0;0m", mines.height/2, mines.width-4);
                while((c=getchar())!='\n') {}
                m_gen_field_u_info(&mines, &mines_def, mines_def.current_bomb_count);
            }
        }
        else if(c=='s'){        // flag tool
            m_flag_tool(&mines, cursor.x-1, cursor.y-1);
        }
        else if(c=='d'){        // find tile with value 0 and reveal
            for(int i = 0; i < mines.height; i++){
                for(int o = 0; o < mines.width; o++){
                    if(m_get_tile(&mines, i, o)->number==0){
                        m_reveal_tool_u_info(&mines, &mines_def, i, o);
                        goto exit;
                    }
                }
            }
            exit:
        }
        else if(c=='f'){
            m_gen_field_u_info(&mines, &mines_def, mines_def.current_bomb_count);
        }
    }

    m_fina(&mines); // deletes field
    printf("\x1B[?25h"); // makes cursor visible
    tcsetattr(0, TCSANOW, &first); // restores fd 0 settings
}
