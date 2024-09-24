#include <cstdlib>
#include <ncurses.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int direction = 0;

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
                direction = 1;
                break;
            case KEY_DOWN:
                direction = 2;
                break;
            case KEY_LEFT:
                direction = 3;
                break;
            case KEY_RIGHT:
                direction = 4;
                break;
            case 'q':
                endwin();
                _exit(0);
        }
    }
    return NULL;
}

int main() {
    // Allocate memory for coordinates of snake body
    int snake_length = 6;
    struct point *coordinates = (struct point *)malloc(snake_length*sizeof(struct point));
    if (coordinates == NULL) {
        printf("Memory allocation failed\n");
        return 1;
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

    // Initialize the start position
    coordinates[snake_length - 1].y = y_max/2;
    coordinates[snake_length - 1].x = x_max/2;
    

    pthread_t input_thread;
    pthread_create(&input_thread, NULL, inputHandler, NULL);

    // Main game loop
    while (1) {
        // Movement
        switch (direction) {
            case 0:
                break; 
            
            case 1:
            mvaddch(coordinates[snake_length - 1].y - 1, coordinates[snake_length - 1].x, '#');
            memmove(&coordinates[0], &coordinates[1], (snake_length - 1) * sizeof(struct point));
            coordinates[snake_length - 1].y = coordinates[snake_length - 2].y - 1;
                break;

            case 2:
            mvaddch(coordinates[snake_length - 1].y + 1, coordinates[snake_length - 1].x, '#');
            memmove(&coordinates[0], &coordinates[1], (snake_length - 1) * sizeof(struct point));
            coordinates[snake_length - 1].y = coordinates[snake_length - 2].y + 1;


                break;
            case 3:

            mvaddch(coordinates[snake_length - 1].y, coordinates[snake_length - 1].x - 1, '#');  
            memmove(&coordinates[0], &coordinates[1], (snake_length - 1) * sizeof(struct point));
            coordinates[snake_length - 1].x = coordinates[snake_length - 2].x - 1;

                break; 

            case 4:
            mvaddch(coordinates[snake_length - 1].y, coordinates[snake_length - 1].x+1, '#');  
            memmove(&coordinates[0], &coordinates[1], (snake_length - 1) * sizeof(struct point));
            coordinates[snake_length - 1].x = coordinates[snake_length - 2].x + 1;

                break; 
        }
        mvaddch(coordinates[0].y, coordinates[0].x, ' ');  
        refresh();
        usleep(100000); // Sleep for 100 milliseconds
    }
    
    free(coordinates);
    endwin(); // End ncurses mode
    return 0;
}

