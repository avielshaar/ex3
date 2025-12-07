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

#define ODD_DIVIDER 2
#define MID_DIVIDER 2

#define ERROR -1

/* Return 1 if selected column is full, 0 otherwise */
int isColumnFull(char[][COLS], int, int);

/* Return 1 if board is full, 0 otherwise */
int isBoardFull(char[][COLS], int, int);

/* Return 1 if selected square (row and column) is in bounds of board, 0 otherwise */
int isInBounds(int, int, int, int);

/* Return index of row where token will land, or -1 if column full */
int getFreeRow(char[][COLS], int, int);

/* Place token in column (0-based). Return row index or -1 if illegal */
int makeMove(char[][COLS], int, int, char);

/* Return 1 if a player has won (has a sequance of 4), 0 otherwise */
int checkVictory(char[][COLS], int, int, int, int, char);

/* Return 1 if a player has almost won (has a sequance of 3), 0 otherwise */
int checkAlmostVictory(char[][COLS], int, int, int, int, char);

/* Return 1 if a player has a vertical sequance of selected length, 0 otherwise */
int checkColSequence(char[][COLS], int, int, int, int, char, int);

/* Return 1 if a player has a horizontal sequance of selected length, 0 otherwise */
int checkRowSequence(char[][COLS], int, int, int, int, char, int);

/* Return 1 if a player has a diagonal sequance of selected length, 0 otherwise */
int checkSlantSequence(char[][COLS], int rws, int, int, int ol, char, int);

/* Human player: asks repeatedly until a valid non-full column is chosen (0-based). Return the choise */
int humanChoose(char[][COLS], int, int, char);

/* Computer player: Return the column the computer chose based on certaion parameters */
int computerChoose(char[][COLS], int, int, char);

/* Start and run the game */
void runConnectFour(char[][COLS], int, int, int, int);

/* Run a round. Return 1 if the game ended, 0 otherwise */
int runRound(char board[][COLS], int, int, int, char);

/* Initialize the board and fills is with the EMPTY token */
void initBoard(char[][COLS], int, int);

/* Print the board */
void printBoard(char[][COLS], int, int);

/* asks repeatedly until a valid player type is chosen (human or computer). Return the choise */
int getPlayerType(int);

/* Return player number based on a token */
int getPlayerNumber(int);

/* Clear the buffer */
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
    return board[rows - 1][col] != EMPTY; // Check if the top square is occupied
}

int isBoardFull(char board[][COLS], int rows, int cols)
{
    for (int j = 0; j < cols; j++)
    {
        if (!isColumnFull(board, rows, j)) // If column not full
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
    for (int i = rows - 1; i >= 0; i--) // Start the search for a free row from the top
    {
        if (board[i][col] != EMPTY)
        {
            return i + 1; // Return the row above the occupied row
        }
    }
    return 0; // If none of the rows are occupied, return the bottom row
}

int makeMove(char board[][COLS], int rows, int col, char token)
{
    int row = getFreeRow(board, rows, col);

    if (row != COL_FULL)
    {
        board[row][col] = token; // Make the move on the board
    }

    return row;
}

int checkVictory(char board[][COLS], int rows, int cols, int row, int col, char token)
{
    // Check if there is a sequance of 4 in any direction
    return (checkColSequence(board, rows, cols, row, col, token, CONNECT_N) ||
            checkRowSequence(board, rows, cols, row, col, token, CONNECT_N) ||
            checkSlantSequence(board, rows, cols, row, col, token, CONNECT_N));
}

int checkAlmostVictory(char board[][COLS], int rows, int cols, int row, int col, char token)
{
    // Check if there is a sequance of 3 in any direction
    return (checkColSequence(board, rows, cols, row, col, token, CONNECT_ALMOST_N) ||
            checkRowSequence(board, rows, cols, row, col, token, CONNECT_ALMOST_N) ||
            checkSlantSequence(board, rows, cols, row, col, token, CONNECT_ALMOST_N));
}

int checkColSequence(char board[][COLS], int rows, int cols, int row, int col, char token, int sequence)
{
    for (int i = row - 1; i > row - sequence; i--) // Start from the sqaure below the chosen square
    {
        if (!(isInBounds(rows, cols, i, col) && board[i][col] == token))
        {
            return 0; // If the sqaure does not contain a token identical to the chosen sqaure
        }
    }
    return 1; // If the (sequance) sqaures below the chosen square contained identical token
}

int checkRowSequence(char board[][COLS], int rows, int cols, int row, int col, char token, int sequence)
{
    int counter = 0;
    for (int j = col - sequence + 1; j < cols; j++) // Start from (sequance - 1) squares left of the chosen square
    {
        if (j == col)
        {
            counter++; // If j == chosen column
        }
        else if (counter == 0 && j == col + NO_SEQUENCE_POINT)
        {
            return 0; // If theres no more chance for a sequance
        }
        else if (isInBounds(rows, cols, row, j) && board[row][j] == token)
        {
            counter++; // If the sqaure does contain a token identical to the chosen sqaure
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

    // Start from (sequance - 1) squares left of the chosen square and (sequance - 1) squares under the chosen square
    for (int i = row - sequence + 1, j = col - sequence + 1; i < rows && j < cols; i++, j++)
    {
        if (i == row && j == col)
        {
            counter++; // If i == chosen row and j == chosen column
        }
        else if (counter == 0 && i == row + NO_SEQUENCE_POINT) // If theres no more chance for a sequance
        {
            break;
        }
        else if (isInBounds(rows, cols, i, j) && board[i][j] == token)
        {
            counter++; // If the sqaure does contain a token identical to the chosen sqaure
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

    // Start from (sequance - 1) squares left of the chosen square and (sequance - 1) squares above the chosen square
    for (int i = row + sequence - 1, j = col - sequence + 1; i >= 0 && j < cols; i--, j++)
    {
        if (i == row && j == col)
        {
            counter++; // If i == chosen row and j == chosen column
        }
        else if (counter == 0 && i == row - NO_SEQUENCE_POINT)
        {
            return 0; // If theres no more chance for a sequance
        }
        else if (isInBounds(rows, cols, i, j) && board[i][j] == token)
        {
            counter++; // If the sqaure does contain a token identical to the chosen sqaure
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

        if (!valid) // If the user didn't entered a number
        {
            printf("Invalid input. Enter a number.\n");
            clearBuffer();
            continue;
        }
        if (selectedCol <= 0 || selectedCol > cols) // If the user chose a column beyond the board limits
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
    // Define the direction for the column priority ordering algorithm, depends on if the cols number is odd or even
    int direction = cols % ODD_DIVIDER ? BACKWARDS : FORWARD, distance = 0, highestPriority = NO_PRIORITY;
    int priorities[COLS] = {NO_PRIORITY};
    char enemyToken = token == TOKEN_P1 ? TOKEN_P2 : TOKEN_P1;

    // Define j as the middle column of odd number of columns, or the middle-left column of even number of columns
    for (int j = cols % ODD_DIVIDER ? (int)(cols / MID_DIVIDER) : (int)(cols / MID_DIVIDER) - 1;
         // As long as j in bounds: increase the distance, and increase/decrease j based on the direction
         j >= 0; distance++, direction == BACKWARDS ? (j -= distance) : (j += distance),
             // Turns the direction to the other direction
                 direction = direction == BACKWARDS ? FORWARD : BACKWARDS)
    {
        int row = getFreeRow(board, rows, j);
        if (row == COL_FULL)
        {
            priorities[j] = COL_FULL;
        }
        // If choosing this column will cause a victory (sequance of 4)
        else if (checkVictory(board, rows, cols, row, j, token))
        {
            printf("Player %d (%c) turn.\n", getPlayerNumber(token), token);
            printf("Computer chose column %d", j + 1);
            return j;
        }
        // If choosing this column will block the enemy from winning
        else if (checkVictory(board, rows, cols, row, j, enemyToken))
        {
            highestPriority = BLOCK_LOSING;
            priorities[j] = BLOCK_LOSING;
        }
        // If choosing this column will cause an almost victory (sequance of 3)
        else if (highestPriority < WINNING_CHANCE && checkAlmostVictory(board, rows, cols, row, j, token))
        {
            highestPriority = WINNING_CHANCE;
            priorities[j] = WINNING_CHANCE;
        }
        // If choosing this column will block the enemy from almost winning
        else if (highestPriority < BLOCK_LOSING_CHANCE && checkAlmostVictory(board, rows, cols, row, j, enemyToken))
        {
            highestPriority = BLOCK_LOSING_CHANCE;
            priorities[j] = BLOCK_LOSING_CHANCE;
        }
    }

    direction = cols % ODD_DIVIDER ? BACKWARDS : FORWARD, distance = 0; // Reset the direction and the distance

    // Define j as the middle column of odd number of columns, or the middle-left column of even number of columns
    for (int j = cols % ODD_DIVIDER ? (int)(cols / MID_DIVIDER) : (int)(cols / MID_DIVIDER) - 1;
         // As long as j in bounds: increase the distance, and increase/decrease j based on the direction
         j >= 0; distance++, direction == BACKWARDS ? (j -= distance) : (j += distance),
             // Turns the direction to the other direction
                 direction = direction == BACKWARDS ? FORWARD : BACKWARDS)
    {
        if (priorities[j] == highestPriority) // If this column has the highest priority from all the columns
        {
            printf("Player %d (%c) turn.\n", getPlayerNumber(token), token);
            printf("Computer chose column %d", j + 1);
            return j;
        }
    }

    return ERROR;
}

void runConnectFour(char board[][COLS], int rows, int cols, int p1Type, int p2Type)
{
    // Run one round for player 1, and then run round for player 2, until game is over
    while (!(runRound(board, rows, cols, p1Type, TOKEN_P1) || runRound(board, rows, cols, p2Type, TOKEN_P2)))
    {
    }
}

int runRound(char board[][COLS], int rows, int cols, int pType, char token)
{
    int selectedCol = pType == HUMAN                                  // Checks if player is human
                          ? humanChoose(board, rows, cols, token)     // If so
                          : computerChoose(board, rows, cols, token); // If not

    int selectedRow = makeMove(board, rows, selectedCol, token); // Make a move
    printBoard(board, rows, cols);

    if (selectedRow != COL_FULL) // If the column the player chose isn't full
    {
        if (checkVictory(board, rows, cols, selectedRow, selectedCol, token)) // Win
        {
            printf("Player %d (%c) wins!\n", getPlayerNumber(token), token);
            return GAME_ENDED;
        }
        if (isBoardFull(board, rows, cols)) // Tie
        {
            printf("Board full and no winner. It's a tie!\n");
            return GAME_ENDED;
        }
    }

    return GAME_CONTINUED; // If the game hasn't ended yet
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