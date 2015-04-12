//
//  main.c
//  hw3AI
//
//  Created by Jianshen He on 10/29/14.
//  Copyright (c) 2014 Jianshen He. All rights reserved.
//
/*
#include <stdio.h>

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    return 0;
}
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BOARD_SIZE  6
#define MAX_CHILD_NODES	MAX_BOARD_SIZE*MAX_BOARD_SIZE

#define CUTOFF          7

#define EMPTY		0
#define X_PLAYER	1
#define O_PLAYER	2

#define MAX_INFINITY	1000000
#define DRAW		0
#define MIN_INFINITY	-1000000

int last_move_x;
int last_move_y;
int computer_move_x;
int computer_move_y;
int boards_checked;
int moves = 0;
int flag = 0;

int bug() {  return 1; } // for break in debugging.

/*
 The board is a one-dimensional array of unsigned integers such that
 each row of the board is represented by a single unsigned integer.
 Each position of the board is represented by 2 bits in this unsigned integer:
 0 for blank; 1 for player 1, and 2 for player 2.
 */

int getCell( int x, int y, unsigned int *board )
{ // cell(x, y) is represented by 2 bits in board[x].
    return ((board[x] >> (y*2)) & 0x3);
}


void putCell( int player, int x, int y, unsigned int *board )
{  // cell(x,y) is set to player.
    board[x] |= (player << (y*2));
    last_move_x = x; last_move_y = y;
    return;
}


void resetCell( int x, int y, unsigned int *board )
{ // cell(x,y) is set to 0.
    board[x] &= (0x3 << (y*2))^0x7FFFFFFF;
    return;
}


char convert( int contents )
{
    if      (contents == X_PLAYER) return 'X';
    else if (contents == O_PLAYER) return 'O';
    return ' ';
}


void emitBoard( unsigned int *board )
{
    /* Emit the current board */
    int x, y;
    
    printf("\n     ");
    for (y = 0; y < MAX_BOARD_SIZE; y++) printf("%d   ", y);
    for (x = 0; x < MAX_BOARD_SIZE; x++) {
        printf("\n   -");
        for (y = 0; y < MAX_BOARD_SIZE; y++) printf("----");
        printf("\n %d | ", x);
        for (y = 0; y < MAX_BOARD_SIZE; y++)
            printf("%c | ", convert(getCell(x, y, board)));
    }
    printf("\n   -");
    for (y = 0; y < MAX_BOARD_SIZE; y++) printf("----");
    printf("\n");
    return;
}


int hasNeighbor(int x, int y, unsigned int *board )
{ // return 1 if cell(x, y) has an non-empty neighbor
    if (x > 0 && y > 0 && getCell(x-1, y-1, board)) return 1;
    if (x > 0 && getCell(x-1, y, board)) return 1;
    if (y > 0 && getCell(x, y-1, board)) return 1;
    if (x < MAX_BOARD_SIZE-1 && y < MAX_BOARD_SIZE-1 && getCell(x+1, y+1, board)) return 1;
    if (x < MAX_BOARD_SIZE-1 && getCell(x+1, y, board)) return 1;
    if (y < MAX_BOARD_SIZE-1 && getCell(x, y+1, board)) return 1;
    if (x < MAX_BOARD_SIZE-1 && y > 0 && getCell(x+1, y-1, board)) return 1;
    if (x > 0 && y < MAX_BOARD_SIZE-1 && getCell(x-1, y+1, board)) return 1;
    return 0;
}

int countScore( int player, int x, int y, unsigned int *board, int expect )
{
    // return MAX_INFINITY if the number of pieces of "player" >= "expect" at position (x y).
    // Otherise, if we place "player" at position (x,y), and let the max length of the same pieces
    // (containing position (x,y)) at all four directions be a1, a2, a3, a4, respectively, and let
    // e be the number of empty positions at both ends of these pieces, then return the value
    // a1*a1 + a2*a2 + a3*a3 + a4*a4 + e as the score of the position (x, y).
    
    int i, j;
    int n, score = 0;
    
    n = 1; i = x-1; j = y;
    while (i >= 0) {
        if (getCell(i, j, board) == player) { i--; n++; }
        else { if (getCell(i, j, board) == EMPTY) score++; break; }
    }
    i = x+1;
    while (i < MAX_BOARD_SIZE) {
        if (getCell(i, j, board) == player) { i++; n++; }
        else { if (getCell(i, j, board) == EMPTY) score++; break; }
    }
    if (n >= expect) return MAX_INFINITY;
    score += n*n;
    
    n = 1; i = x; j = y-1;
    while (j >= 0) {
        if (getCell(i, j, board) == player) { j--; n++; }
        else { if (getCell(i, j, board) == EMPTY) score++; break; }
    }
    j = y+1;
    while (j < MAX_BOARD_SIZE) {
        if (getCell(i, j, board) == player) { j++; n++; }
        else { if (getCell(i, j, board) == EMPTY) score++; break; }
    }
    if (n >= expect) return MAX_INFINITY;
    score += n*n;
    
    n = 1; i = x-1; j = y-1;
    while (j >= 0 && i >= 0) {
        if (getCell(i, j, board) == player) { i--; j--; n++; }
        else { if (getCell(i, j, board) == EMPTY) score++; break; }
    }
    i = x+1; j = y+1;
    while (i < MAX_BOARD_SIZE && j < MAX_BOARD_SIZE) {
        if (getCell(i, j, board) == player) { i++; j++; n++; }
        else { if (getCell(i, j, board) == EMPTY) score++; break; }
    }
    if (n >= expect) return MAX_INFINITY;
    score += n*n;
    
    n = 1; i = x+1; j = y-1;
    while (j >= 0 && i < MAX_BOARD_SIZE) {
        if (getCell(i, j, board) == player) {i++; j--; n++; }
        else { if (getCell(i, j, board) == EMPTY) score++; break; }
    }
    i = x-1; j = y+1;
    while (i >= 0 && j < MAX_BOARD_SIZE) {
        if (getCell(i, j, board) == player) { i--; j++; n++; }
        else { if (getCell(i, j, board) == EMPTY) score++; break; }
    }
    if (n >= expect) return MAX_INFINITY;
    score += n*n;
    
    return score;
}


int checkPlayerWin( int player, unsigned int *board )
{
    // Check if there are four same pieces in a row at position (last_move_x, last_move_y)
    return (countScore(player, last_move_x, last_move_y, board, 4) == MAX_INFINITY);
}


void getHumanMove( int player, unsigned int *board )
{
    int selectionx, selectiony;
    
    /* Get human move */
    while (1) {
        selectionx = selectiony = -1;
        printf("Select a move for Player %d, type row column: ", player);
        scanf("%d", &selectionx);
        scanf("%d", &selectiony);
        printf("\n");
        if ((selectionx >= 0) && (selectionx < MAX_BOARD_SIZE) &&
            (selectiony >= 0) && (selectiony < MAX_BOARD_SIZE)) {
            if (moves && hasNeighbor(selectionx, selectiony, board) == 0)
                printf("cell has no neighbors -- choose again.\n");
            else if (getCell(selectionx, selectiony, board))
                printf("cell taken -- choose again.\n");
            else break;
        } else {
            printf("bad input -- choose again.\n");
            getchar();
        }
    }
    
    putCell( player, selectionx, selectiony, board );
    return;
}

int evaluateMaxMove( unsigned int *board, int depth );

int evaluateMinMove( unsigned int *board, int depth )
{
    int value, x, y;
    int min = MAX_INFINITY+1;
    int scores1[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    int scores2[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    
    // printf("DEPTH=%d\n", depth);
    boards_checked++;
    /*
     for (i = 0; i < depth; i++) printf(" ");
     printf("%d max %d %d\n", depth, last_move_x, last_move_y);
     */
    
    /* Player 2 (max) just made a move, so we evaluate that move here */
    if (checkPlayerWin(O_PLAYER, board)) return MAX_INFINITY;
    
    /* If Player 1 has a winning position, take it */
    for (x = 0; x < MAX_BOARD_SIZE; x++)
        for (y = 0; y < MAX_BOARD_SIZE; y++)
            if (getCell(x, y, board) == EMPTY && hasNeighbor(x, y, board)) {
                scores1[x][y] = countScore( X_PLAYER, x, y, board, 4);
                if (scores1[x][y] == MAX_INFINITY) return MIN_INFINITY;
            }
    
    /* If Player 2 has a winning position, block it */
    for (x = 0; x < MAX_BOARD_SIZE; x++)
        for (y = 0; y < MAX_BOARD_SIZE; y++)
            if (getCell(x, y, board) == EMPTY && hasNeighbor(x, y, board)) {
                scores2[x][y] = countScore( O_PLAYER, x, y, board, 4);
                if (scores2[x][y] == MAX_INFINITY) {
                    putCell( X_PLAYER, x, y, board );
                    min = evaluateMaxMove( board, depth+1 );
                    resetCell( x, y, board);
                    return min;
                }
            }
    
    /* Cutoff checking */
    if (depth >= CUTOFF) {
        // printf("depth %d scores\n", depth);
        for (x = 0; x < MAX_BOARD_SIZE; x++)
            for (y = 0; y < MAX_BOARD_SIZE; y++)
                if (getCell(x, y, board) == EMPTY && hasNeighbor(x, y, board)) {
                    // the value of position (x, y) is the sum of its score when the opponent is placed
                    // at that position plus the twice of the score when itself is placed at that position.
                    value = (scores2[x][y])+(scores1[x][y]<<1);
                    
                    // printf("min (%d %d) %d %d %d\n", x, y, value, scores1[x][y], scores2[x][y]);
                    
                    // the value is negated for min-move
                    value = -value;
                    if (value < min) {
                        min = value;
                    }
                }
        return min;
    }
    
    for (x = 0; x < MAX_BOARD_SIZE; x++)
        for (y = 0; y < MAX_BOARD_SIZE; y++)
            if (getCell(x, y, board) == EMPTY && hasNeighbor(x, y, board)) {
                
                putCell( X_PLAYER, x, y, board );
                value = evaluateMaxMove( board, depth+1 );
                resetCell(x, y, board);
                
                // printf("depth=%d value = %d for %d %d\n", depth, value, x, y);
                if (value < min) {
                    min = value;
                    if (min == MIN_INFINITY) return min;
                }
            }
    
    /* No move is possible -- draw */
    if (min == MAX_INFINITY+1) {
        return DRAW;
    }
    
    return min;
}


int evaluateMaxMove( unsigned int *board, int depth )
{
    int value, x, y;
    int max = MIN_INFINITY-1;
    int scores1[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    int scores2[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    
    // printf("DEPTH=%d\n", depth);
    boards_checked++;
    /*
     for (i = 0; i < depth; i++) printf(" ");
     printf("%d min %d %d\n", depth, last_move_x, last_move_y);
     */
    
    /* Player 1 (min) just made a move, so we evaluate that move here */
    if (checkPlayerWin(X_PLAYER, board)) return MIN_INFINITY;
    
    /* If Player 2 has a winning position, take it */
    for (x = 0; x < MAX_BOARD_SIZE; x++)
        for (y = 0; y < MAX_BOARD_SIZE; y++) {
            
            if (getCell(x, y, board) == EMPTY && hasNeighbor(x, y, board)) {
                scores1[x][y] = countScore( O_PLAYER, x, y, board, 4);
                if (scores1[x][y] == MAX_INFINITY) {
                    if (depth == 0) { computer_move_x = x; computer_move_y = y; }
                    return MAX_INFINITY;
                }
            }
        }
    
    /* If Player 1 has a winning position, block it */
    for (x = 0; x < MAX_BOARD_SIZE; x++)
        for (y = 0; y < MAX_BOARD_SIZE; y++)
            if (getCell(x, y, board) == EMPTY && hasNeighbor(x, y, board)) {
                
                scores2[x][y] = countScore( X_PLAYER, x, y, board, 4);
                if (scores2[x][y] == MAX_INFINITY) {
                    putCell( O_PLAYER, x, y, board );
                    if (depth == 0) { computer_move_x = x; computer_move_y = y; return 0; }
                    max = evaluateMinMove( board, depth+1 );
                    resetCell( x, y, board);
                    return max;
                }
            }
    
    /* Cutoff checking */
    if (depth >= CUTOFF) {
        // printf("depth %d scores\n", depth);
        for (x = 0; x < MAX_BOARD_SIZE; x++)
            for (y = 0; y < MAX_BOARD_SIZE; y++)
                if (getCell(x, y, board) == EMPTY && hasNeighbor(x, y, board)) {
                    value = (scores2[x][y])+(scores1[x][y]<<1);
                    // printf("max (%d %d) %d %d %d\n", x, y, value, scores1[x][y], scores2[x][y]);
                    if (value > max) {
                        max = value;
                    }
                }
        return max;
    }
    
    for (x = 0; x < MAX_BOARD_SIZE; x++)
        for (y = 0; y < MAX_BOARD_SIZE; y++)
            if (getCell(x, y, board) == EMPTY && hasNeighbor(x, y, board)) {
                
                putCell( O_PLAYER, x, y, board );
                value = evaluateMinMove( board, depth+1 );
                resetCell( x, y, board);
                
                // printf("depth=%d value = %d for %d %d\n", depth, value, x, y);
                if (value > max) {
                    max = value;
                    if (depth == 0) { 
                        computer_move_x = x; computer_move_y = y; 
                    }
                    if (max == MAX_INFINITY) return max;
                }
                
            }
    
    /* No move is possible -- draw */
    if (max == MIN_INFINITY-1) {
        return DRAW;
    }
    
    return max;
}


void getComputerMove( unsigned int *board )
{
    int value = 0;
    boards_checked = 0;
    
    if (moves == 1) {
        if (last_move_x < MAX_BOARD_SIZE/2) 
            computer_move_x = last_move_x+1; else computer_move_x = last_move_x-1;
        if (last_move_y < MAX_BOARD_SIZE/2) 
            computer_move_y = last_move_y+1; else computer_move_y = last_move_y-1;
    } else
        value = evaluateMaxMove( board, 0 );
    
    printf("Player 2's move is %d %d (%d boards checked; score is %d)\n", 
           computer_move_x, computer_move_y, boards_checked, value);
    
    putCell( O_PLAYER, computer_move_x, computer_move_y, board );
    
    return;
}


int main()
{
    unsigned int cur_board[MAX_BOARD_SIZE];
    int won = 0;
    int i;
    
    for (i = 0; i < MAX_BOARD_SIZE; i++) cur_board[i] = 0;
    
    while (!won) {
        
        if (moves == MAX_CHILD_NODES) { printf("draw\n"); break; }
        
        emitBoard( cur_board );
        
        getHumanMove( X_PLAYER, cur_board );
        
        won = checkPlayerWin( X_PLAYER, cur_board );
        moves++;
        
        if (!won) {
            
            if (moves == MAX_CHILD_NODES) { printf("draw\n"); break; }
            
            emitBoard( cur_board );
            
            getComputerMove( cur_board );
            // getHumanMove( O_PLAYER, cur_board );
            moves++;
            
            won = checkPlayerWin( O_PLAYER, cur_board );
            
            if (won) { printf("\nPlayer 2 wins!\n"); }
            
        } else {
            printf("\nPlayer 1 wins!\n");
        }
        
    }
    
    emitBoard( cur_board );
    return 0;
}
