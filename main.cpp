#include <iostream>
#include <stdexcept>
#include <vector>
#include <limits>
#include <string>
#include <sstream>


typedef std::vector<std::vector<int> > t_board;
typedef std::vector<t_board> t_children;
typedef std::pair<t_board, int> t_pair;


// Print the board.
void print_board(t_board &board)
{
    for (int i = 0; i < board.size(); i++)
    {
        for (int j = 0; j < board.size(); j++)
        {
            if (board[i][j] == 1)
                std::cout << "X";
            else if (board[i][j] == 2)
                std::cout << "O";
            else
                std::cout << "-";
            if (j != board.size()-1)
                std::cout << " | ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


int check_rows(t_board &board)
{
    for (int i = 0; i < board.size(); i++)
    {
        if (board[i][0] == board[i][1] && board[i][0] == board[i][2])
            return board[i][0];
    }
    return 0;
}


int check_columns(t_board &board)
{
    for (int i = 0; i < board.size(); i++)
    {
        if (board[0][i] == board[1][i] && board[0][i] == board[2][i])
            return board[0][i];
    }
    return 0;
}


int check_diagnols(t_board &board)
{
    if ((board[0][0] == board[1][1] && board[0][0] == board[2][2]) || 
        (board[0][2] == board[1][1] && board[0][2] == board[2][0]))
        return board[1][1];
    return 0;
}


bool check_filled(t_board &board)
{
    for (int i = 0; i < board.size(); i++)
    {
        for (int j = 0; j < board.size(); j++)
        {
            if (board[i][j] == 0)
                return false;
        }
    }
    return true;
}


// Check if there is a winner on the board, or it is a tie.
int check_winner(t_board &board)
{
    int winner = 0;

    winner = check_rows(board);
    if (winner) return winner;
    winner = check_columns(board);
    if (winner) return winner;
    winner = check_diagnols(board);
    if (winner) return winner;

    if (check_filled(board))
        return -1;
    return 0;
}


// Generate children of board.
t_children create_children(t_board &board, int turn)
{
    t_children children;

    std::cout << children.size() << std::endl;
    for (int i = 0; i < board.size(); i++)
    {
        for (int j = 0; j < board.size(); j++)
        {
            if (board[i][j] == 0)
            {
                t_board child = board;
                child[i][j] = turn;
                children.push_back(child);
            }
        }
    }
    return children;
}


// Run a static evaluation of the board.
int static_evaluation(t_board &board, int turn, int starting_turn)
{
    t_board simulation_board;
    int winner;
    int score;
    int opponent;
    bool alone;
    bool found;
    int  empty_spots;

    std::cout << "Static evaluation:\n";
    print_board(board);
    score = 0;
    opponent = turn == 1 ? 2 : 1;

    //  If the current board is a winning board, then return the highest possible score.
    winner = check_winner(board);
    std::cout << "*** " << winner <<std::endl; 
    if (winner == turn && starting_turn == turn)
        return 42;
    else if (winner == opponent)    return -42;
    else if (winner == -1)          return 0;


    // Count scores row-major
    for (int r = 0; r < board.size(); r++)
    {
        alone = true;
        found = false;
        empty_spots = 0;
        for (int c = 0; c < board.size() && alone; c++)
        {
            if (board[r][c] == opponent)
                alone = false;
            else if (board[r][c] == turn)
                found = true;
            else
                empty_spots += 1;
        }
        if (found && alone)
            score += 3 - empty_spots; // score += 3 - #empty_spots  <==> high empty_spots, low score
    }

    std::cout << "Score: " << score << std::endl;

    // Count scores column-major
    for (int c = 0; c < board.size(); c++)
    {
        alone = true;
        found = false;
        empty_spots = 0;
        for (int r = 0; r < board.size() && alone; r++)
        {
            if (board[r][c] == opponent)
                alone = false;
            else if (board[r][c] == turn)
                found = true;
            else
                empty_spots += 1;
        }
        if (found && alone) 
            score += 3 - empty_spots; // score += 3 - #empty_spots  <==> high empty_spots, low score
    }
    std::cout << "Score: " << score << std::endl;


    // Count scores on first diagnol (top -> bottom)
    std::vector<int> diagnol_1{board[0][0], board[1][1], board[2][2]};
    alone = true;
    found = false;
    empty_spots = 0;
    for (int i = 0; i < diagnol_1.size() && alone; i++)
    {
        if (diagnol_1[i] == opponent)
            alone = false;
        else if (diagnol_1[i] == turn)
            found = true;
        else
            empty_spots += 1;
    }
    if (found && alone)
        score += 3 - empty_spots; // score += 3 - #empty_spots  <==> high empty_spots, low score
    std::cout << "Score: " << score << std::endl;


    // Count scores on second diagnol (bottom -> top)
    std::vector<int> diagnol_2{board[2][0], board[1][1], board[0][2]};
    alone = true;
    found = false;
    empty_spots = 0;
    for (int i = 0; i < diagnol_2.size() && alone; i++)
    {
        if (diagnol_2[i] == opponent)
            alone = false;
        else if (diagnol_2[i] == turn)
            found = true;
        else
            empty_spots += 1;
    }
    if (found && alone)
        score += 3 - empty_spots; // score += 3 - #empty_spots  <==> high empty_spots, low score
    std::cout << "Score: " << score << std::endl;

    return score;
}


// Minimax algorithm without alpha-beta prouning.
t_pair minimax(t_board &board, int is_maximizer, int turn, int starting_turn)
{
    t_children children;
    t_pair best;
    t_pair current;
    int winner;

    // Return evaluation score of the leaf board.
    winner = check_winner(board);
    if (winner == 1 || winner == 2 || winner == -1)
    {
        best.first = board;
        best.second = static_evaluation(board, turn, starting_turn);
        return best;
    }

    children = create_children(board, turn);
    
    if (is_maximizer)
    {
        best.first = children[0];
        best.second = INT32_MIN;
        for (int i = 0; i < children.size(); i++)
        {
            // std::cout << "Parent:\n";
            // print_board(board);
            // std::cout << "Maximizing\n";
            // print_board(children[i]);
            current = minimax(children[i], false, turn == 1 ? 2 : 1, starting_turn);
            if (current.second > best.second)
            {
                best.first = current.first;
                best.second += current.second;
            }
        }
    }
    else
    {
        best.first = children[0];
        best.second = INT32_MAX;
        for (int i = 0; i < children.size(); i++)
        {
            // std::cout << "Parent:\n";
            // print_board(board);
            // std::cout << "Minimizing\n";
            // print_board(children[i]);
            current = minimax(children[i], true, turn == 1 ? 2 : 1, starting_turn);
            if (current.second < best.second)
            {
                best.first = children[i];
                best.second -= current.second;
            }
        }
    }
    std::cout << "best:\n";
    print_board(board);
    return best;
}


int main()
{
    int winner;
    int turn = 1;
    t_board board;
    t_pair best_move;

    // Initialize the board with zeros.
    for (int i = 0; i < 3; i++)
        board.push_back(std::vector<int>(3, 0));


    // Game loop.
    while (true)
    {

        // Break the infinit loop if there is a winner or if it's a tie.
        winner = check_winner(board);
        if (winner)
            break;

        // Make the bot play first.
        if (turn == 1)
        {
            std::cout << "bot turn" << std::endl;
            best_move = minimax(board, true, turn, turn);
            board = best_move.first;
            std::cout << best_move.second << std::endl;
        }
        else
        {
            std::cout << "player turn" << std::endl;
            std::string line;
            std::stringstream ss;
            std::vector<int> coords;
            int value;

            std::getline(std::cin, line);
            ss << line;
            while (!ss.eof())
            {
                ss >> value;
                coords.push_back(value);
            }
            board[coords[0]][coords[1]] = turn;
        }

        // Print the current state of the board.
        print_board(board);

        // Flip the turn so the other player can play.
        turn = turn == 1 ? 2 : 1;
    }

    // Print proper message at the end of the game.
    if (winner == 1)
        std::cout << "X won" << std::endl;
    else if (winner == 2)
        std::cout << "O won" << std::endl;
    else
        std::cout << "It's a tie" << std::endl;
    return (0);
}