/* 

RULES FOR CONWAYS GAME OF LIFE: 

Any live cell with fewer than two live neighbours dies, as if caused by under-population.
Any live cell with two or three live neighbours lives on to the next generation.
Any live cell with more than three live neighbours dies, as if by overcrowding.
Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>



#define DIMENSION 38

void draw(void);
void seedDraw(void);
void initialize(void);
void populate(void);
void cycle(void);
int neighbors(int row, int col); //takes in location and searches all around for neighbors, returns number of neighbors
void markRules(int row, int col, int numberOfNeighbors); //follows rules for marking death or life
void seedMode(void);
void help(void); // get that "help" garbage outta main
void make(void);
void makeBlinker(void);
void makeGlider(void);
void makeRpentimino(void);
void makeBeacon(void);
void makeTenInfinite(void);
void makeFiveByFive(void);
void gospersGun(void);
void load(void);
void save(void);


char grid[DIMENSION][DIMENSION];
char mark[DIMENSION][DIMENSION];
char initial[DIMENSION][DIMENSION];
char saved[DIMENSION][DIMENSION];
char DEAD = '`';
char ALIVE = '#';


/*
 * Note: The following two functions were taken from the CS50 Library
 *
 * Copyright (c) 2013, Glenn Holloway, David J. Malan
 *
 * You can learn more here: https://manual.cs50.net/library/
 *
 */
typedef char* string;


string GetString(void)
{
    // growable buffer for chars
    string buffer = NULL;
    
    // capacity of buffer
    unsigned int capacity = 0;
    
    // number of chars actually in buffer
    unsigned int n = 0;
    
    // character read or EOF
    int c;
    
    // iteratively get chars from standard input
    while ((c = fgetc(stdin)) != '\n' && c != EOF)
    {
        // grow buffer if necessary
        if (n + 1 > capacity)
        {
            // determine new capacity: start at 32 then double
            if (capacity == 0)
            {
                capacity = 32;
            }
            else if (capacity <= (UINT_MAX / 2))
            {
                capacity *= 2;
            }
            else
            {
                free(buffer);
                return NULL;
            }
            
            // extend buffer's capacity
            string temp = realloc(buffer, capacity * sizeof(char));
            if (temp == NULL)
            {
                free(buffer);
                return NULL;
            }
            buffer = temp;
        }
        
        // append current character to buffer
        buffer[n++] = c;
    }
    
    // return NULL if user provided no input
    if (n == 0 && c == EOF)
    {
        return NULL;
    }
    
    // minimize buffer
    string minimal = malloc((n + 1) * sizeof(char));
    strncpy(minimal, buffer, n);
    free(buffer);
    
    // terminate string
    minimal[n] = '\0';
    
    // return string
    return minimal;
}

int GetInt(void)
{
    // try to get an int from user
    while (1)
    {
        // get line of text, returning INT_MAX on failure
        string line = GetString();
        if (line == NULL)
        {
            return INT_MAX;
        }
        
        // return an int if only an int (possibly with
        // leading and/or trailing whitespace) was provided
        int n; char c;
        if (sscanf(line, " %d %c", &n, &c) == 1)
        {
            free(line);
            return n;
        }
        else
        {
            free(line);
            printf("Retry: ");
        }
    }
}

// --- --- --- -- - END CS50 LIBRARY - -- --- --- --- //


/*
 * display help
 *
 */
void help() {
    printf("Press enter to iterate to the next step.\n\n");

    printf("Type any of the following commands for useful actions: \n");
    printf("make / edit / clear / initial / save / load / exit / help \n\n");

    printf("Try \"make\" to auto-generate interesting creatures!\n");
    printf("Try \"edit\" to return to seed mode.\n");
    printf("Try \"clear\" to clear the grid.\n");
    printf("Try \"initial\" to return to the last state after edit mode.\n");
    printf("Try \"save\" to create a save state.\n");
    printf("Try \"load\" to load the save state.\n");
    printf("Try \"exit\" to quit the program.\n");
    printf("Try \"help\" to view these tips anytime!\n\n");

    return;
}

/*
 *   draws the GRID and coordinates during seeding
 *
 */
void seedDraw(void) {

    int colCount = 1;
    int rowCount = 0;
    int row;
    int col;
    for (row = 0; row < DIMENSION; row++) {

        //newline and row Coordinates
        if (rowCount >= 10) {
            rowCount = 0;
        }
        if (row < DIMENSION - 1) { //prints proper
            printf("\n %d ", rowCount);
            rowCount++;
        }

        for (col = 0; col < DIMENSION; col++) {
            if (row == 0) { //PRINTS COORDINATES ON FIRST LINE
                if (col < DIMENSION - 2) {
                    if (colCount >= 10) {
                        colCount = 0;
                    }
                    printf(" %d", colCount);
                    colCount++;
                }
            } else { //PRINT WHOLE GRID
                printf("%c ", grid[row][col]); //PRINTS GRID OTHERWISE
            }

        }
    }
    //and after the grid is completely drawn
    printf("\n\n");
}

/*
 *   draws the GRID and coordinates DURRING LIFE PHASE
 *
 */
void draw(void) {
    int row;
    int col;

    for (row = 0; row < DIMENSION; row++) {
        //newline
        printf("\n  ");
        for (col = 0; col < DIMENSION; col++) {
            printf("%c ", grid[row][col]); //PRINTS GRID
        }

    }
    //and after the grid is completely drawn
    printf("\n\n");
}

/*
 *  sets all the GRID spaces to '.' and also sets initial SEEDS 
 *
 */
void initialize(void) {
    int row;
    int col; 

    for (row = 0; row < DIMENSION; row++) {
        for (col = 0; col < DIMENSION; col++)
            if (row != 0 && col != 0 && row != DIMENSION - 1 && col != DIMENSION - 1) { //so long as we arent on edges
                mark[row][col] = 'x';
            }
    }
    populate();
}

/*
 * Allows the user to enter seeds
 *
 */
void seedMode(void) {
    seedDraw(); //initial seedDraw
    //user seeds
    int getRow;
    int getCol;
    int row;
    int col; 

    do {
        //explain program and get seeds!
        printf("Provide seeds. Input row and column; Keep values within 1 and %d!\n", DIMENSION - 2);
        printf("(Type \"-1\" to end seeding and start the program.)\n");


        //GET ROW 
        do {
            printf("Row: ");
            getRow = GetInt();
            if (getRow == -1)
                break;
        } while (getRow > DIMENSION - 2 || getRow == 0 || getRow<-1);
        //GET COL
        if (getRow != -1) {
            do {
                printf("Column: ");
                getCol = GetInt();
                if (getCol == -1)
                    break;
            } while (getCol > DIMENSION - 2 || getCol == 0 || getCol<-1);

            if (getCol != -1) {
                //determine state and flip it
                if (grid[getRow][getCol] == DEAD) {
                    mark[getRow][getCol] = 'o';
                } else if (grid[getRow][getCol] == ALIVE) {
                    mark[getRow][getCol] = 'x';
                }

                //draw
                populate();
                seedDraw();
            }
        }

    } while (getRow != -1 && getCol != -1);

    draw(); //final draw

    //CREATE INITIAL STATE
    for (row = 0; row < DIMENSION; row++) {
        for (col = 0; col < DIMENSION; col++) {
            initial[row][col] = mark[row][col];
        }
    }
}

/*
 *   turns all marked locations into "graphics"
 *
 */
void populate(void) {
    int row;
    int col;

    for (row = 0; row < DIMENSION; row++) {
        for (col = 0; col < DIMENSION; col++) {
            if (mark[row][col] == 'o') { //if mark found, populate life on GRID
                grid[row][col] = ALIVE;
            }
            if (mark[row][col] == 'x') { //if marked for death
                grid[row][col] = DEAD;
            }
        }
    }
}

/*
 *   cycles through the grid once and marks locations for new life
 *
 */

void cycle(void) {
    int row; 
    int col;

    for (row = 0; row < DIMENSION; row++) {
        for (col = 0; col < DIMENSION; col++) {
            if (row != 0 && col != 0) //so long as we arent on the UP/LEFT edge
                if (row != DIMENSION - 1 && col != DIMENSION - 1) //or the DOWN/RIGHT edge
                    markRules(row, col, neighbors(row, col));
        }
    }
}

/*
 *  returns number of neighbors
 *
 */
int neighbors(int row, int col) {
    int count = 0;
    // 1 2 3
    // 4 x 6
    // 7 8 9 
    //1: up-left
    if (grid[row - 1][col - 1] == ALIVE) {
        count++;
    }
    //2: up
    if (grid[row - 1][col] == ALIVE) {
        count++;
    }
    //3: up-right
    if (grid[row - 1][col + 1] == ALIVE) {
        count++;
    }
    //4: left
    if (grid[row][col - 1] == ALIVE) {
        count++;
    }
    //6: right
    if (grid[row][col + 1] == ALIVE) {
        count++;
    }
    //7: down-left
    if (grid[row + 1][col - 1] == ALIVE) {
        count++;
    }
    //8: down
    if (grid[row + 1][col] == ALIVE) {
        count++;
    }
    //9: down-right
    if (grid[row + 1][col + 1] == ALIVE) {
        count++;
    }

    return count;
}

/*
 * follows rules and kills/creates life by marking locations with 'o' '.' or 'x'
 *
 */
void markRules(int row, int col, int numberOfNeighbors) {

    //live and less than 2, mark for death
    if (grid[row][col] == ALIVE && numberOfNeighbors < 2) {
        mark[row][col] = 'x'; //death!
        return;
    }
    //live and two or three, no change
    if (grid[row][col] == ALIVE && (numberOfNeighbors == 2 || numberOfNeighbors == 3)) {
        return;
    }
    //live and more than three, mark for death
    if (grid[row][col] == ALIVE && numberOfNeighbors > 3) {
        mark[row][col] = 'x';
        return;
    }
    //if dead, and 3 neighbors: mark for life
    if (grid[row][col] == DEAD && numberOfNeighbors == 3) {
        mark[row][col] = 'o';
        return;
    }
}

/*
 * saves a state!
 *
 */
void save() {
    int row;
    int col; 

    for (row = 0; row < DIMENSION; row++) {
        for (col = 0; col < DIMENSION; col++) {
            saved[row][col] = mark[row][col];
        }
    }
    printf("\nState Saved! Type \"load\" to return to this state!\n");
    return;
}

/*
 * loads saved state!
 *
 */
void load() {
    int row;
    int col; 

    initialize();
    cycle();
    for (row = 0; row < DIMENSION; row++) {
        for (col = 0; col < DIMENSION; col++) {
            mark[row][col] = saved[row][col];
        }
    }
    printf("\nState loaded!\n");
    cycle();
    populate();
    draw();
    return;

}

///////////////////////MAKE/////////////////////////////////////////

/*
 * What would you like to make?
 *
 */
void make() {

    printf("\n\nWhat would you like to make?\n");
    printf("Try: ");
    printf("blinker / glider / rpentimino / beacon / teninfinite / fivebyfive / gospersgun \n\n");
    string input = GetString();

    if (strcmp(input, "blinker") == 0) { //blinker
        makeBlinker();
    } else if (strcmp(input, "glider") == 0) { //glider
        makeGlider();
    } else if (strcmp(input, "rpentimino") == 0 || strcmp(input, "rpent") == 0) { //rpent
        makeRpentimino();
    } else if (strcmp(input, "beacon") == 0) { //beacon
        makeBeacon();
    } else if (strcmp(input, "teninfinite") == 0) {
        makeTenInfinite();
    } else if (strcmp(input, "fivebyfive") == 0) {
        makeFiveByFive();
    } else if (strcmp(input, "gospersgun") == 0) {
        if (DIMENSION >= 38) {
            gospersGun();
        } else
            printf("Sorry, the dimensions need to be at least 41. Currently they are: %d", DIMENSION);
    } else {
        printf("Sorry, I don't know how to make that creature!");
    }

    populate();
    draw();

    return;
}

/*
 * makes stuff!   
 *
 */
void makeBlinker(void) {
    mark[DIMENSION / 2][DIMENSION / 2] = 'o';
    mark[DIMENSION / 2 + 1][DIMENSION / 2] = 'o';
    mark[DIMENSION / 2 - 1][DIMENSION / 2] = 'o';
    return;
}

void makeGlider(void) {
    mark[DIMENSION / 2][DIMENSION / 2] = 'o';
    mark[DIMENSION / 2][DIMENSION / 2 - 1] = 'o';
    mark[DIMENSION / 2][DIMENSION / 2 - 2] = 'o';
    mark[DIMENSION / 2 - 1][DIMENSION / 2] = 'o';
    mark[DIMENSION / 2 - 2][DIMENSION / 2 - 1] = 'o';
    return;
}

void makeRpentimino(void) {
    mark[DIMENSION / 2 - 1][DIMENSION / 2] = 'o';
    mark[DIMENSION / 2][DIMENSION / 2 - 1] = 'o';
    mark[DIMENSION / 2][DIMENSION / 2] = 'o';
    mark[DIMENSION / 2 + 1][DIMENSION / 2] = 'o';
    mark[DIMENSION / 2 - 1][DIMENSION / 2 + 1] = 'o';
    return;
}

void makeBeacon(void) {
    mark[DIMENSION / 2 - 1][DIMENSION / 2 - 1] = 'o';
    mark[DIMENSION / 2 - 1][DIMENSION / 2] = 'o';
    mark[DIMENSION / 2][DIMENSION / 2 - 1] = 'o';
    mark[DIMENSION / 2][DIMENSION / 2] = 'o';

    mark[DIMENSION / 2 + 1][DIMENSION / 2 + 1] = 'o';
    mark[DIMENSION / 2 + 1][DIMENSION / 2 + 2] = 'o';
    mark[DIMENSION / 2 + 2][DIMENSION / 2 + 1] = 'o';
    mark[DIMENSION / 2 + 2][DIMENSION / 2 + 2] = 'o';
    return;
}

void makeTenInfinite(void) {
    mark[DIMENSION / 2][DIMENSION / 2] = 'o';
    mark[DIMENSION / 2 - 1][DIMENSION / 2 - 1] = 'o';
    mark[DIMENSION / 2][DIMENSION / 2 - 1] = 'o';
    mark[DIMENSION / 2 + 1][DIMENSION / 2 - 1] = 'o';

    mark[DIMENSION / 2][DIMENSION / 2 - 3] = 'o';
    mark[DIMENSION / 2 + 1][DIMENSION / 2 - 3] = 'o';
    mark[DIMENSION / 2 + 2][DIMENSION / 2 - 3] = 'o';

    mark[DIMENSION / 2 + 3][DIMENSION / 2 - 5] = 'o';
    mark[DIMENSION / 2 + 4][DIMENSION / 2 - 5] = 'o';

    mark[DIMENSION / 2 + 4][DIMENSION / 2 - 7] = 'o';

    return;
}

void makeFiveByFive(void) {
    mark[DIMENSION / 2][DIMENSION / 2] = 'o';

    mark[DIMENSION / 2][DIMENSION / 2 - 2] = 'o';
    mark[DIMENSION / 2][DIMENSION / 2 - 3] = 'o';
    mark[DIMENSION / 2][DIMENSION / 2 - 4] = 'o';
    mark[DIMENSION / 2 + 1][DIMENSION / 2 - 4] = 'o';

    mark[DIMENSION / 2 + 2][DIMENSION / 2 - 1] = 'o';
    mark[DIMENSION / 2 + 2][DIMENSION / 2] = 'o';
    mark[DIMENSION / 2 + 3][DIMENSION / 2] = 'o';
    mark[DIMENSION / 2 + 4][DIMENSION / 2] = 'o';

    mark[DIMENSION / 2 + 4][DIMENSION / 2 - 2] = 'o';
    mark[DIMENSION / 2 + 3][DIMENSION / 2 - 2] = 'o';
    mark[DIMENSION / 2 + 3][DIMENSION / 2 - 3] = 'o';

    mark[DIMENSION / 2 + 4][DIMENSION / 2 - 4] = 'o';

    return;
}

void gospersGun(void) {

    if (DIMENSION>=36){
    //left square
    mark[5][1] = 'o';
    mark[6][1] = 'o';
    mark[5][2] = 'o';
    mark[6][2] = 'o';
    //left ship
    mark[5][11] = 'o';
    mark[6][11] = 'o';
    mark[7][11] = 'o';
    
    mark[4][12] = 'o';
    
    mark[8][12] = 'o';
    
    mark[3][13] = 'o';
    mark[3][14] = 'o';
    
    mark[9][13] = 'o';
    mark[9][14] = 'o';
    
    mark[6][15] = 'o';
    
    mark[4][16] = 'o';
    
    mark[8][16] = 'o';
    
    mark[6][17] = 'o';
    mark[6][18] = 'o';
    mark[7][17] = 'o';
    mark[5][17] = 'o';
    //right ship
    mark[5][21] = 'o';
    mark[4][21] = 'o';
    mark[3][21] = 'o';
    mark[3][22] = 'o';
    mark[4][22] = 'o';
    mark[5][22] = 'o';
    
    mark[6][23] = 'o';
   
    mark[2][23] = 'o';

    mark[2][25] = 'o';
    mark[1][25] = 'o';
    
    mark[6][25] = 'o';
    mark[7][25] = 'o';
    //right square
    mark[3][35] = 'o';
    mark[3][36] = 'o';
    mark[4][35] = 'o';
    mark[4][36] = 'o';
    }
    return;
 
    
}




/*
 * ======================  M  A  I  N
 */
int main(void) {
    int row;
    int col;

    initialize(); //set all to "."
    seedMode();
    help();

    string input;

    while (1) {
        input = GetString();

        if (strcmp(input, "edit") == 0) { //EDIT
            seedMode();
        } else if (strcmp(input, "help") == 0) { //HELP
            help();
        } else if (strcmp(input, "exit") == 0) { //EXIT
            break;
        } else if (strcmp(input, "initial") == 0) { //INITIAL
            initialize();
            cycle();
            for (row = 0; row < DIMENSION; row++) {
                for (col = 0; col < DIMENSION; col++) {
                    mark[row][col] = initial[row][col];
                }
            }
            cycle();
            populate();
            draw();
        } else if (strcmp(input, "clear") == 0) { //CLEAR
            initialize();
            cycle();
            populate();
            draw();
        } else if (strcmp(input, "make") == 0) { //make!
            make();
        } else if (strcmp(input, "save") == 0) { //save state
            save();
        } else if (strcmp(input, "load") == 0) { //load save
            load();
        } else { //ENTER (no input)
            cycle();
            populate();
            draw();
        }


    }
    return 0;
}