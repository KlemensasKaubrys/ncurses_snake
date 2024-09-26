#include <cstdlib>
#include <ncurses.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int direction = 0;
int prev_direction = 0;
int snake_length = 6;
bool paused = false;

struct point {
int x;
int y;
};



void* inputHandler(void* arg) {
    int ch;
    while (1) {
                ch = getch();
                switch (ch) {
                    case KEY_UP:
                        prev_direction = direction;
                        if (prev_direction != 2) {
                        direction = 1;
                        } 
                        break;
                    case KEY_DOWN:
                        prev_direction = direction;
                        if (prev_direction != 1) {
                        direction = 2;
                        }
                        break;
                    case KEY_LEFT:
                        prev_direction = direction;
                        if (prev_direction != 4) {
                        direction = 3;
                        } 
                        break;
                    case KEY_RIGHT:
                        prev_direction = direction;                
                        if (prev_direction != 3) {
                        direction = 4;
                        }
                        break;
                    case 'q':
                        endwin();
                        _exit(0);
                    case 'p':
                        paused = true;
                }
        }
    return NULL;
}

void pause_screen() {
    int row, col;

    getmaxyx(stdscr, row, col);

    int start_row = (row - 5) / 2;
    int start_col = (col - 38) / 2; 

    scr_dump("screen_dump");
    mvprintw(start_row, start_col,     "+------------------------------------+");
    mvprintw(start_row + 1, start_col, "|  Paused. Press any key to continue |");
    mvprintw(start_row + 2, start_col, "+------------------------------------+");
    refresh();
    
    getch();
        
    paused = false;
    scr_restore("screen_dump");
    refresh();
}

void death_screen() {
        
    int row, col;

    getmaxyx(stdscr, row, col);

    int start_row = (row - 5) / 2;
    int start_col = (col - 17) / 2; 

    mvprintw(start_row, start_col, "+-----------------+");
    mvprintw(start_row + 1, start_col, "|    You died!    |");
    mvprintw(start_row + 2, start_col, "+-----------------+");
    mvprintw(start_row + 3, start_col, "| Score: %8d |", snake_length);
    mvprintw(start_row + 4, start_col, "+-----------------+");

    refresh();
    getch();
    endwin();
    _exit(0);

}

int main() {

    int max_food = 20;
    // Allocate memory for coordinates of snake body
    struct point *coordinates = (struct point *)malloc(snake_length*sizeof(struct point));
    if (coordinates == NULL) {
        printf("Memory allocation failed\n");
    }

    // Initialize NCURSES
    initscr();            // Initialize ncurses
    cbreak();             // Disable line buffering
    noecho();             // Don't echo input characters
    keypad(stdscr, TRUE); // Enable special keys like arrow keys
    curs_set(FALSE);      // Hide the cursor
    
    // Put snake head in the middle of the screen
    int y_max, x_max;
    getmaxyx(stdscr, y_max, x_max);
    mvaddch(y_max/2, x_max/2, '#');
    refresh();

    // Make food struct and generate random coordinates
    struct point food[max_food];
    srand(time(NULL));

    for (int i = 0; i < max_food; ++i) {
        food[i].x = rand() % (x_max + 1);
        food[i].y = rand() % (y_max + 1);
        mvaddch(food[i].y, food[i].x, 'O');

    }
    
    // Initialize the start position
    coordinates[snake_length - 1].y = y_max/2;
    coordinates[snake_length - 1].x = x_max/2;
    

    pthread_t input_thread;
    pthread_create(&input_thread, NULL, inputHandler, NULL);

    // Main game loop
    while (1) {
        // Check if game is paused
        if (paused) {
        pause_screen();
        }
        refresh();
        // Movement
        switch (direction) {
            case 0:
                break; 
            
            case 1:
                    if ((mvinch(coordinates[snake_length - 1].y - 1, coordinates[snake_length - 1].x) & A_CHARTEXT) == '#') {
                        death_screen();
                    }
                    mvaddch(coordinates[snake_length - 1].y - 1, coordinates[snake_length - 1].x, '#');
                    memmove(&coordinates[0], &coordinates[1], (snake_length - 1) * sizeof(struct point));
                    coordinates[snake_length - 1].y = coordinates[snake_length - 2].y - 1;
                break;

            case 2:
                    if ((mvinch(coordinates[snake_length - 1].y + 1, coordinates[snake_length - 1].x) & A_CHARTEXT) == '#') {
                        death_screen();
                    }
                    mvaddch(coordinates[snake_length - 1].y + 1, coordinates[snake_length - 1].x, '#');
                    memmove(&coordinates[0], &coordinates[1], (snake_length - 1) * sizeof(struct point));
                    coordinates[snake_length - 1].y = coordinates[snake_length - 2].y + 1;
                break;
            case 3:
                    if ((mvinch(coordinates[snake_length - 1].y, coordinates[snake_length - 1].x - 1) & A_CHARTEXT) == '#') {
                        death_screen();
                    }
                    mvaddch(coordinates[snake_length - 1].y, coordinates[snake_length - 1].x - 1, '#');  
                    memmove(&coordinates[0], &coordinates[1], (snake_length - 1) * sizeof(struct point));
                    coordinates[snake_length - 1].x = coordinates[snake_length - 2].x - 1;
                break; 
            case 4:
                    if ((mvinch(coordinates[snake_length - 1].y, coordinates[snake_length - 1].x + 1) & A_CHARTEXT) == '#') {
                        death_screen();
                    }
                    mvaddch(coordinates[snake_length - 1].y, coordinates[snake_length - 1].x+1, '#');  
                    memmove(&coordinates[0], &coordinates[1], (snake_length - 1) * sizeof(struct point));
                    coordinates[snake_length - 1].x = coordinates[snake_length - 2].x + 1;
                break; 
        }
        // Delete 1st entry of the struct
        mvaddch(coordinates[0].y, coordinates[0].x, ' ');
        
        // Check if the snake is about to eat a unit of food, if it is destroy old fruit, generate new, add length reallocate memory
        for (int p = 0; p < max_food; ++p) {
            if (food[p].x == coordinates[snake_length - 1].x && food[p].y == coordinates[snake_length - 1].y) {
                food[p].x = rand() % (x_max + 1);
                food[p].y = rand() % (y_max + 1);
                mvaddch(food[p].y, food[p].x, 'O');
                snake_length += 1;
                
                struct point *temp = (struct point *)realloc(coordinates, snake_length * sizeof(struct point));
                // Check if reallocation was successful
                if (temp == NULL) {
                    printf("Memory reallocation failed\n");
                    free(coordinates); // Free the original memory if reallocation fails
                    return 1;
                }
                    temp[snake_length - 1] = temp[snake_length - 2];
                    coordinates = temp;
            }

        }
        // Out of bounds
        if (coordinates[snake_length - 1].y == -1) {
            coordinates[snake_length - 1].y = y_max;
        } else if (coordinates[snake_length - 1].y == y_max) {
            coordinates[snake_length - 1].y = 0;
        } else if (coordinates[snake_length - 1].x == x_max) {
            coordinates[snake_length - 1].x = 0;
        } else if (coordinates[snake_length - 1 ].x == -1) {
            coordinates[snake_length - 1].x = x_max;            
        }
        refresh();
        usleep(150000);
    }
    
    free(coordinates);
    endwin();
    return 0;
}


