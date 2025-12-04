#include <stdio.h>

#ifndef ROWS
#define ROWS 6
#endif

#ifndef COLS
#define COLS 7
#endif

#define CONNECT_N 4
#define CONNECT_ALMOST_N CONNECT_N - 1

/* Tokens */
#define EMPTY '.'
#define TOKEN_P1 'X'
#define TOKEN_P2 'O'

/* Player numbers */
#define PLAYER_1 1
#define PLAYER_2 2

/* Player types */
#define HUMAN 1
#define COMPUTER 2

/* Priorities */
#define WINNING 4
#define BLOCK_LOSING 3
#define WINNING_CHANCE 2
#define BLOCK_LOSING_CHANCE 1
#define NO_PRIORITY 0

#define BACKWARDS -1
#define FORWARD 1

#define GAME_ENDED 1
#define GAME_CONTINUED 0

#define COL_FULL -1

#define NO_SEQUENCE_POINT 2

#define MID_DIVIDER 2

int isColumnFull(char[][COLS], int, int);

int isBoardFull(char[][COLS], int, int);

int isInBounds(int, int, int, int);

/* Return index of row where token will land, or -1 if column full */
int getFreeRow(char[][COLS], int, int);

/* Place token in column (0-based). Return row index or -1 if illegal */
int makeMove(char[][COLS], int, int, char);

int checkVictory(char[][COLS], int, int, int, int, char);

int checkAlmostVictory(char[][COLS], int, int, int, int, char);

int checkColSequence(char[][COLS], int, int, int, int, char, int);

int checkRowSequence(char[][COLS], int, int, int, int, char, int);

int checkSlantSequence(char[][COLS], int rws, int, int, int ol, char, int);

/* Human player: asks repeatedly until a valid non-full column is chosen (0-based) */
int humanChoose(char[][COLS], int, int, char);

/* Computer */
int computerChoose(char[][COLS], int, int, char);

void runConnectFour(char[][COLS], int, int, int, int);

int runRound(char board[][COLS], int, int, int, char);

void initBoard(char[][COLS], int, int);

void printBoard(char[][COLS], int, int);

int getPlayerType(int);

int getPlayerNumber(int);

void clearBuffer();

int main()
{
    char board[ROWS][COLS];
    printf("Connect Four (%d rows x %d cols)\n\n", ROWS, COLS);
    int p1Type = getPlayerType(1);
    int p2Type = getPlayerType(2);
    initBoard(board, ROWS, COLS);
    printBoard(board, ROWS, COLS);
    runConnectFour(board, ROWS, COLS, p1Type, p2Type);
    return 0;
}

int isColumnFull(char board[][COLS], int rows, int col)
{
    return board[rows - 1][col] != EMPTY;
}

int isBoardFull(char board[][COLS], int rows, int cols)
{
    for (int j = 0; j < cols; j++)
    {
        if (!isColumnFull(board, rows, j))
        {
            return 0;
        }
    }
    return 1;
}

int isInBounds(int rows, int cols, int row, int col)
{
    return row >= 0 && row < rows && col >= 0 && col < cols;
}

int getFreeRow(char board[][COLS], int rows, int col)
{
    if (isColumnFull(board, rows, col))
    {
        return COL_FULL;
    }
    for (int i = rows - 1; i >= 0; i--)
    {
        if (board[i][col] != EMPTY)
        {
            return i + 1;
        }
    }
    return 0;
}

int makeMove(char board[][COLS], int rows, int col, char token)
{
    int row = getFreeRow(board, rows, col);

    if (row != COL_FULL)
    {
        board[row][col] = token;
    }

    return row;
}

int checkVictory(char board[][COLS], int rows, int cols, int row, int col, char token)
{
    return checkColSequence(board, rows, cols, row, col, token, CONNECT_N) || checkRowSequence(board, rows, cols, row, col, token, CONNECT_N) || checkSlantSequence(board, rows, cols, row, col, token, CONNECT_N);
}

int checkAlmostVictory(char board[][COLS], int rows, int cols, int row, int col, char token)
{
    return checkColSequence(board, rows, cols, row, col, token, CONNECT_ALMOST_N) || checkRowSequence(board, rows, cols, row, col, token, CONNECT_ALMOST_N) || checkSlantSequence(board, rows, cols, row, col, token, CONNECT_ALMOST_N);
}

int checkColSequence(char board[][COLS], int rows, int cols, int row, int col, char token, int sequence)
{
    for (int i = row - 1; i > row - sequence; i--)
    {
        if (!(isInBounds(rows, cols, i, col) && board[i][col] == token))
        {
            return 0;
        }
    }
    return 1;
}

int checkRowSequence(char board[][COLS], int rows, int cols, int row, int col, char token, int sequence)
{
    int counter = 0;
    for (int j = col - sequence + 1; j < cols; j++)
    {

        if (j == col)
        {
            counter++;
        }
        else if (counter == 0 && j == col + NO_SEQUENCE_POINT)
        {
            return 0;
        }
        else if (isInBounds(rows, cols, row, j) && board[row][j] == token)
        {
            counter++;
        }
        else
        {
            counter = 0; // Reset the counter
        }
        if (counter == sequence)
        {
            return 1;
        }
    }
    return 0;
}

int checkSlantSequence(char board[][COLS], int rows, int cols, int row, int col, char token, int sequence)
{
    int counter = 0;
    for (int i = row - sequence + 1, j = col - sequence + 1; i < rows && j < cols; i++, j++)
    {
        if (i == row && j == col)
        {
            counter++;
        }
        else if (counter == 0 && i == row + NO_SEQUENCE_POINT)
        {
            break;
        }
        else if (isInBounds(rows, cols, i, j) && board[i][j] == token)
        {
            counter++;
        }
        else
        {
            counter = 0; // Reset the counter
        }
        if (counter == sequence)
        {
            return 1;
        }
    }

    counter = 0;
    for (int i = row + sequence - 1, j = col - sequence + 1; i >= 0 && j < cols; i--, j++)
    {
        if (i == row && j == col)
        {
            counter++;
        }
        else if (counter == 0 && i == row - NO_SEQUENCE_POINT)
        {
            return 0;
        }
        else if (isInBounds(rows, cols, i, j) && board[i][j] == token)
        {
            counter++;
        }
        else
        {
            counter = 0; // Reset the counter
        }
        if (counter == sequence)
        {
            return 1;
        }
    }

    return 0;
}

int humanChoose(char board[][COLS], int rows, int cols, char token)
{
    int player = getPlayerNumber(token), selectedCol = 0;
    printf("Player %d (%c) turn.\n", player, token);
    while (1)
    {
        printf("Enter column (1-%d): ", cols);
        int valid = scanf(" %d", &selectedCol);

        if (valid != 1) // If the user didn't entered a number
        {
            printf("Invalid input. Enter a number.\n");
            clearBuffer();
            continue;
        }
        if (selectedCol <= 0 || selectedCol > cols)
        {
            printf("Invalid column. Choose between 1 and %d.\n", cols);
            continue;
        }
        if (isColumnFull(board, rows, selectedCol - 1))
        {
            printf("Column %d is full. Choose another column.\n", selectedCol);
            continue;
        }

        return selectedCol - 1; // If passed all tests
    }
}

int computerChoose(char board[][COLS], int rows, int cols, char token)
{
    int priorities[COLS] = {NO_PRIORITY};
    int direction = cols % 2 ? BACKWARDS : FORWARD, distance = 0, highestPriority = NO_PRIORITY;
    char enemyToken = token == TOKEN_P1 ? TOKEN_P2 : TOKEN_P1;

    for (int j = cols % 2 ? (int)(cols / MID_DIVIDER) : (int)(cols / MID_DIVIDER) - 1; j >= 0; distance++, direction == BACKWARDS ? (j -= distance) : (j += distance), direction = direction == BACKWARDS ? FORWARD : BACKWARDS)
    {
        int row = getFreeRow(board, rows, j);
        if (row == COL_FULL)
        {
            priorities[j] = COL_FULL;
        }
        else if (checkVictory(board, rows, cols, row, j, token))
        {
            printf("Player %d (%c) turn.\n", getPlayerNumber(token), token);
            printf("Computer chose column %d", j + 1);
            return j;
        }
        else if (checkVictory(board, rows, cols, row, j, enemyToken))
        {
            highestPriority = BLOCK_LOSING;
            priorities[j] = BLOCK_LOSING;
        }
        else if (highestPriority < WINNING_CHANCE && checkAlmostVictory(board, rows, cols, row, j, token))
        {
            highestPriority = WINNING_CHANCE;
            priorities[j] = WINNING_CHANCE;
        }
        else if (highestPriority < BLOCK_LOSING_CHANCE && checkAlmostVictory(board, rows, cols, row, j, enemyToken))
        {
            highestPriority = BLOCK_LOSING_CHANCE;
            priorities[j] = BLOCK_LOSING_CHANCE;
        }
    }

    direction = cols % 2 ? BACKWARDS : FORWARD, distance = 0;
    for (int j = cols % 2 ? (int)(cols / MID_DIVIDER) : (int)(cols / MID_DIVIDER) - 1; j >= 0; distance++, direction == BACKWARDS ? (j -= distance) : (j += distance), direction = direction == BACKWARDS ? FORWARD : BACKWARDS)
    {
        if (priorities[j] == highestPriority)
        {
            printf("Player %d (%c) turn.\n", getPlayerNumber(token), token);
            printf("Computer chose column %d", j + 1);
            return j;
        }
    }
    return -1;
}

void runConnectFour(char board[][COLS], int rows, int cols, int p1Type, int p2Type)
{
    while (!(runRound(board, rows, cols, p1Type, TOKEN_P1) || runRound(board, rows, cols, p2Type, TOKEN_P2)))
    {
    }
}

int runRound(char board[][COLS], int rows, int cols, int pType, char token)
{
    int selectedCol = pType == HUMAN ? humanChoose(board, rows, cols, token) : computerChoose(board, rows, cols, token);
    int selectedRow = makeMove(board, rows, selectedCol, token);
    printBoard(board, rows, cols);
    if (selectedRow != COL_FULL)
    {
        if (checkVictory(board, rows, cols, selectedRow, selectedCol, token))
        {
            printf("Player %d (%c) wins!\n", getPlayerNumber(token), token);
            return GAME_ENDED;
        }
        if (isBoardFull(board, rows, cols))
        {
            printf("Board full and no winner. It's a tie!\n");
            return GAME_ENDED;
        }
    }
    return GAME_CONTINUED;
}

void initBoard(char board[][COLS], int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            board[i][j] = EMPTY;
        }
    }
}

void printBoard(char board[][COLS], int rows, int cols)
{
    printf("\n");
    for (int r = rows - 1; r >= 0; r--)
    {
        printf("|");
        for (int c = 0; c < cols; c++)
        {
            putchar(board[r][c]);
            printf("|");
        }
        printf("\n");
    }
    for (int c = 1; c <= cols; c++)
    {
        printf(" %d", c % 10);
    }
    printf("\n\n");
}

int getPlayerType(int playerNumber)
{
    char ch;
    while (1)
    {
        printf("Choose type for player %d: h - human, c - computer: ", playerNumber);
        int n = scanf(" %c", &ch);
        if (n != 1)
        {
            printf("Input error. Try again.\n");
            clearBuffer();
            continue;
        }
        if (ch == 'h' || ch == 'H')
            return HUMAN;
        if (ch == 'c' || ch == 'C')
            return COMPUTER;

        printf("Invalid selection. Enter h or c.\n");
        clearBuffer();
    }
}

int getPlayerNumber(int token)
{
    return token == TOKEN_P1 ? PLAYER_1 : PLAYER_2;
}

void clearBuffer()
{
    while (getchar() != '\n')
        ;
}