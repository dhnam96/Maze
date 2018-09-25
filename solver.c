#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "solver.h"

/*
 * Initializes the board with beginning values by scanning the output of the
 * maze file.
 *
 * Parameters:
 *  - num_rows: number of rows
 *  - num_cols: number of columns
 *  - maze: a 2D array of maze_room structs representing your maze
 *  - result: a 2D array of storage that represents the decimal value of the
 * maze
 *  - mf: a file that contains the hexadecimal representation of the maze
 * created in the generator.
 *
 * Returns:
 *  - nothing - the 2D array of maze_room is initialized and room connections
 * are recreated by scanning from the maze file.
 */
void maze_creater(int num_rows, int num_cols,
                  struct maze_room maze[num_rows][num_cols],
                  unsigned int result[num_rows][num_cols], FILE *mf) {
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            struct maze_room *room = &maze[i][j];
            room->row = i;
            room->col = j;
            room->visited = 0;
            room->next = NULL;
            fscanf(mf, "%1x", &result[i][j]);
            create_room_connections(room, result[i][j]);
        }
    }
}

/*
 * Given a pointer to a maze_room, set its connections in all four directions
 *based on the hex value passed in.
 *
 * For example:
 *
 * 		create_room_connections(&maze[3][4], 0xb)
 *
 * 0xb is 1011 in binary, which means we have walls on all sides except the
 *WEST. This means that every connection in maze[3][4] should be set to 1,
 *except the WEST connection, which should be set to 0.
 *
 * See the handout for more details about our hexadecimal representation, as
 *well as examples on how to use bit masks to extract information from this
 *hexadecimal representation.
 *
 * Parameters:
 *	- room: pointer to the current room
 *	- hex: hexadecimal integer (between 0-15 inclusive) that represents the
 *connections in all four directions from the given room.
 *
 * Returns:
 *	- nothing. The connections should be saved in the maze_room struct
 *pointed to by the parameter (make sure to use pointers correctly!).
 */
void create_room_connections(struct maze_room *room, unsigned int hex) {
    int binary[4] = {0, 0, 0, 0};
    if (hex >= 8) {
        binary[3] = 1;
        hex = hex - 8;
    }
    if (hex >= 4) {
        binary[2] = 1;
        hex = hex - 4;
    }
    if (hex >= 2) {
        binary[1] = 1;
        hex = hex - 2;
    }
    if (hex >= 1) {
        binary[0] = 1;
    }

    room->connections[3] = binary[3];
    room->connections[2] = binary[2];
    room->connections[1] = binary[1];
    room->connections[0] = binary[0];
}

/*
 * Recursive depth-first search algorithm for solving your maze.
 * This function should also print out either every visited room as it goes, or
 * the final pruned solution, depending on whether the FULL macro is set.
 *
 * Note that you may need to add extra parameters to this function's signature,
 * depending on how you choose to store the pruned solution.
 *
 * See handout for more details, as well as a pseudocode implementation.
 *
 * Parameters:
 *	- row: row of the current room
 *	- col: column of the current room
 *	- goal_row: row of the goal room
 *	- goal_col: col of the goal room
 *	- num_rows: number of rows in the maze
 *	- num_cols: number of columns in the maze
 *	- maze: a 2d array of maze_room structs representing your maze
 *	- file: the file to write the solution to
 *
 * Returns:
 *	- 1 if the current branch finds a valid solution, 0 if no branches are
 *valid.
 */
int dfs(int row, int col, int goal_row, int goal_col, int num_rows,
        int num_cols, struct maze_room maze[num_rows][num_cols], FILE *pf) {
    struct maze_room *room = &maze[row][col];
    struct maze_room *goal = &maze[goal_row][goal_col];
    if (room == goal) {
#ifdef FULL
        fprintf(pf, "%d, %d\n", goal->row, goal->col);
#endif
        return -1;
    }

#ifdef FULL
    fprintf(pf, "%d, %d\n", room->row, room->col);
#endif

    room->visited = 1;
    for (int i = 0; i < 4; i++) {
        struct maze_room *neighbor =
            get_neighbor(num_rows, num_cols, maze, room, i);
        if (neighbor != NULL) {
            if ((room->connections[i] == 0) && (neighbor->visited == 0)) {
                room->next = neighbor;
                int a = dfs(neighbor->row, neighbor->col, goal_row, goal_col,
                            num_rows, num_cols, maze, pf);
                if (a == 0) {
#ifdef FULL
                    fprintf(pf, "%d, %d\n", room->row, room->col);
#endif
                } else {
                    return -1;
                }
            }
        }
    }
    return 0;
}

/*
 * Main function
 *
 * Parameters:
 *	- argc: the number of command line arguments - for this function 9
 *	- **argv: a pointer to the first element in the command line
 *            arguments array - for this function:
 *		      ["solver", <input maze file>, <number of rows>, <number of
 *columns> <output path file>, <starting row>, <starting column>, <ending row>,
 *<ending column>]
 *
 * Returns:
 *	- 0 if program exits correctly, 1 if there is an error
 */

int main(int argc, char **argv) {
    int num_rows, num_cols, start_row, start_col, goal_row, goal_col;
    char *maze_file;
    char *path_file;
    if (argc != 9) {
        printf("Incorrect number of arguments.\n");
        printf(
            "./solver <input maze file> <number of rows> <number of columns>");
        printf(" <output path file> <starting row> <starting column>");
        printf(" <ending row> <ending column>\n");
        return 1;
    } else {
        maze_file = argv[1];
        num_rows = atoi(argv[2]);
        num_cols = atoi(argv[3]);
        path_file = argv[4];
        start_row = atoi(argv[5]);
        start_col = atoi(argv[6]);
        goal_row = atoi(argv[7]);
        goal_col = atoi(argv[8]);
    }

    // ERROR HANDLING
    if (start_row >= num_rows) {
        printf("Starting row must be within range \n");
        return 1;
    }
    if (start_col >= num_cols) {
        printf("Starting column must be within range \n");
        return 1;
    }
    if (goal_row >= num_rows) {
        printf("Goal row must be within range \n");
        return 1;
    }
    if (goal_col >= num_cols) {
        printf("Goal column must be within range \n");
        return 1;
    }
    if (start_row < 0 || start_col < 0 || goal_row < 0 || goal_col < 0) {
        printf("Index on the maze cannot be negative \n");
        return 1;
    }
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            if (i > 15 || i < 0) {
                printf("The input maze file encoding is incorrect");
                return 1;
            }
        }
    }

    // Body of main
    struct maze_room maze[num_rows][num_cols];
    unsigned int result[num_rows][num_cols];

    FILE *mf = fopen(maze_file, "r");
    FILE *pf = fopen(path_file, "w");

#ifdef FULL
    fprintf(pf, "%s\n", "FULL");
#endif

    maze_creater(num_rows, num_cols, maze, result, mf);
    int a = dfs(start_row, start_col, goal_row, goal_col, num_rows, num_cols,
                maze, pf);

    if (a == 0) {
        printf("There are no valid solution branches");
    }
// if macro is not specified, then the solution pruned is printed to the file
#ifndef FULL
    if (a == -1) {
        fprintf(pf, "%s\n", "PRUNED");
        struct maze_room *current = &maze[start_row][start_col];
        while (current->next != NULL) {
            fprintf(pf, "%d, %d\n", current->row, current->col);
            current = current->next;
        }
        fprintf(pf, "%d, %d\n", goal_row, goal_col);
    }
#endif

    fclose(mf);
    fclose(pf);

    return 0;
}
