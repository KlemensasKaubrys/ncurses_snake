#include <cstdlib>
#include <ncurses.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

    int snake_length = 6;
    int max_food = 6;

    // Allocate memory for coordinates of snake body
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


        // Refresh the screen
        refresh();
        usleep(200000); // Sleep for 100 milliseconds
    }
    
    free(coordinates);
    endwin(); // End ncurses mode
    return 0;
}

