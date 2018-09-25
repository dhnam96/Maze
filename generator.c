#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "generator.h"

/*
 * Given a direction, get its opposite
 *
 * Parameters:
 *	- dir: direction
 *
 * Returns:
 *	- the opposite direction to dir
 */
Direction get_opposite_dir(Direction dir) {
    switch (dir) {
        case EAST:
            return WEST;
        case WEST:
            return EAST;
        case NORTH:
            return SOUTH;
        case SOUTH:
            return NORTH;
    }
    return EAST;
}

/*
 * Given two directions, swaps the elements
 *
 * Parameters:
 *  - Direction: two directions
 *
 * Returns:
 *  - nothing - the directions are swapped
 */
void swap(Direction *a, Direction *b) {
    Direction temp = *a;
    *a = *b;
    *b = temp;
}

/*
 * Given an array of the four directions, randomizes the order of the directions
 *
 * Parameters:
 *	- directions: an array of Direction enums of size 4
 *
 * Returns:
 *	- nothing - the array should be shuffled in place
 */
void shuffle_array(Direction directions[]) {
    for (int i = 0; i < 4; i++) {
        int j = (rand() % (4 - i)) + i;
        swap(&directions[i], &directions[j]);
    }
}

/*
 * Initializes the board with beginning values
 *
 * Parameters:
 *  - num_rows: number of rows
 *  - num_cols: number of columns
 *  - maze: a 2D array of maze_room structs representing your maze
 *
 * Returns:
 *  - nothing - the 2D array of maze_room is initialized
 */
void initial_board(int num_rows, int num_cols,
                   struct maze_room maze[num_rows][num_cols]) {
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            struct maze_room *room = &maze[i][j];
            room->row = i;
            room->col = j;
            room->visited = 0;
            room->connections[0] = -1;
            room->connections[1] = -1;
            room->connections[2] = -1;
            room->connections[3] = -1;
        }
    }
}

/*
 * Recursive function for stepping through a maze and setting its
 *connections/walls using the drunken_walk algorithm
 *
 * Parameters:
 * 	- row: row of the current room
 *	- col: column of the current room
 *	- num_rows: number of rows in the maze
 *	- num_cols: number of columns in the maze
 *	- maze: a 2D array of maze_room structs representing your maze
 * Returns:
 *	- nothing - the state of your maze should be saved in the maze array
 *being passed in (make sure to use pointers correctly!).
 */

void drunken_walk(int row, int col, int num_rows, int num_cols,
                  struct maze_room maze[num_rows][num_cols]) {
    int row_offset;
    int col_offset;
    struct maze_room *room = &maze[row][col];
    room->visited = 1;

    Direction directions_array[4] = {NORTH, SOUTH, WEST, EAST};
    shuffle_array(directions_array);
    for (int i = 0; i < 4; i++) {
        Direction dir = directions_array[i];

        if (dir == 0) {
            row_offset = -1;
            col_offset = 0;
        }
        if (dir == 1) {
            row_offset = 1;
            col_offset = 0;
        }
        if (dir == 2) {
            row_offset = 0;
            col_offset = -1;
        }
        if (dir == 3) {
            row_offset = 0;
            col_offset = 1;
        }

        if (is_in_range(room->row + row_offset, room->col + col_offset,
                        num_rows, num_cols) == 0) {
            room->connections[dir] = 1;
        } else {
            struct maze_room *neighbor =
                get_neighbor(num_rows, num_cols, maze, room, dir);
            if (neighbor != NULL) {
                if (neighbor->visited == 0) {
                    room->connections[dir] = 0;
                    drunken_walk(neighbor->row, neighbor->col, num_rows,
                                 num_cols, maze);
                } else {
                    Direction opp_dir = get_opposite_dir(dir);
                    int opp_connection = neighbor->connections[opp_dir];
                    if (opp_connection != -1) {
                        room->connections[dir] = opp_connection;
                    } else {
                        room->connections[dir] = 1;
                    }
                }
            }
        }
    }
}

/*
 * Represents a maze_room struct as a hexadecimal number based on its
 *connections
 *
 * Parameters:
 *	- room: a struct maze_room to be converted to a hexadecimal number
 *
 * Returns:
 *	- the hexadecimal representation of room
 */
int encode_room(struct maze_room room) {
    int value = (room.connections[3] * 8) + (room.connections[2] * 4) +
                (room.connections[1] * 2) + (room.connections[0] * 1);
    return value;
}

/*
 * Represents a maze_room array as a hexadecimal array based on its connections
 *
 * Parameters:
 *	- num_rows: number of rows in the maze
 *	- num_cols: number of columns in the maze
 *	- maze: a 2D array of maze_room structs representing the maze
 *	- result: a 2D array of hexadecimal numbers representing the maze
 *
 * Returns:
 *	- nothing - each maze_room in the maze should be represented
 *	  as a hexadecimal number  and put in nums at the corresponding index
 */
void encode_maze(int num_rows, int num_cols,
                 struct maze_room maze[num_rows][num_cols],
                 int result[num_rows][num_cols]) {
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            struct maze_room room = maze[i][j];
            int hexadecimal = encode_room(room);
            result[i][j] = hexadecimal;
        }
    }
}

/*
 * Helper function that prints out the hexadecimal encoding of the maze into a
 * file
 *
 * Parameters:
 *  - num_rows: number of rows in the maze
 *  - num_cols: number of columns in the maze
 *  - maze: a 2D array of hexadecimal numbers representing the maze
 *  - file: prints the hexadecimal representation to a file
 *
 * Returns:
 *  - nothing: prints the hexadecimal representation of the maze into an output
 * file
 */
void print_result(int num_rows, int num_cols, int result[num_rows][num_cols],
                  FILE *file) {
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            fprintf(file, "%x", result[i][j]);
        }
        fprintf(file, "\n");
    }
}
/*
 * Main function
 *
 * Parameters:
 *	- argc: the number of command line arguments - for this function 4
 *	- **argv: a pointer to the first element in the command line
 *            arguments array - for this function:
 *		      ["generator", <output file>, <number of rows>, <number of
 *columns>]
 *
 * Returns:
 *	- 0 if program exits correctly, 1 if there is an error
 */
int main(int argc, char **argv) {
    char *file_name;
    int num_rows;
    int num_cols;

    if (argc != 4) {
        printf("Incorrect number of arguments.\n");
        printf(
            "./generator <output file> <number of rows> <number of columns>\n");
        return 1;
    } else {
        file_name = argv[1];
        num_rows = atoi(argv[2]);
        num_cols = atoi(argv[3]);
    }

    if (num_rows == 0 || num_cols == 0) {
        printf("num_rows and num_cols must be greater than 0! \n");
        return 1;
    }

    struct maze_room maze[num_rows][num_cols];
    int result[num_rows][num_cols];

    srand(time(NULL));
    initial_board(num_rows, num_cols, maze);
    drunken_walk(0, 0, num_rows, num_cols, maze);

    FILE *fp = fopen(file_name, "w");
    encode_maze(num_rows, num_cols, maze, result);
    print_result(num_rows, num_cols, result, fp);
    fclose(fp);

    // ERROR HANDLING
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            struct maze_room *room = &maze[i][j];
            int q = result[i][j];
            if (room->visited != 1) {
                printf("The drunken_walk algorithm does not visit all rooms");
                return 1;
            }
            if (q > 15 || q < 0) {
                printf(
                    "The maze encoding from decimal to hexadecimal is "
                    "incorrect");
                return 1;
            }
        }
    }

    return 0;
}
