class Board:
    __slots__ = ('_board', '_empty', '_score')

    def __init__(self, board=None):
        if board:
            self._board = [[*row] for row in board]
            self._empty = 0
            for row in self._board:
                self._empty += row.count(0)
        else:
            self._board = [
                [0, 0, 0], 
                [0, 0, 0], 
                [0, 0, 0]]
            self._empty = 9
        self._score = 0.0

    @property
    def board(self):
        return self._board

    @property
    def empty(self):
        return self._empty

    @property
    def score(self):
        return self._score

    @score.setter
    def score(self, value):
        self._score = value

    def place(self, r, c, player):
        self._board[r][c] = player
        if self.empty > 0:
            self._empty -= 1

    def show(self):
        for row in self.board:
            print(row)

    def copy(self):
        new = []
        for row in self.board:
            new.append([*row])
        return Board(new)

    def __call__(self):
        return self._board


def expand(board: Board, player: int):
    children = []
    for r in range(3):
        for c in range(3):
            if board.board[r][c] == 0:
                copy = Board(board.board)
                copy.place(r, c, player)
                children.append(copy)
    return children
    

def check_winner(board: Board):
    # Horizontal check
    for row in board.board:
        if all([row[0] == c and c for c in row]):
            return row[0]

    # Vertical check
    for c in range(3):
        if board.board[0][c] == board.board[1][c] == board.board[2][c]:
            return board.board[0][c]

    # Diagnol checl
    if board.board[0][0] == board.board[1][1] == board.board[2][2]:
        if board.board[1][1]:
            return board.board[0][0]
    if board.board[0][2] == board.board[1][1] == board.board[2][0]:
        if board.board[1][1]:
            return board.board[1][1]

    # Tie
    if board.empty == 0:
        return -1
    return 0


def static_evaluation(board: Board, turn: int, winner: int, starter: int):
    opponent = 1 if starter == 2 else 1
    if winner == starter:
        board.score *= 42
    if winner == opponent:
        board.score /= 4 


def heuristic_evaluation(board: Board, turn: int):
    opponent = 1 if turn == 2 else 1
    score = 0

    # Count scores row-major
    for row in board.board:
        alone = all([col == turn or col == 0 for col in row])
        if alone:
            empty = row.count(0)
            score = 3-empty if score==0 else score * (3-empty)

    # Count scores column-major
    for i in range(3):
        col = [
            board.board[0][i], 
            board.board[1][i],
            board.board[2][i]]
        alone = all([c == turn or c == 0 for c in col])
        if alone:
            empty = col.count(0)
            score = 3-empty if score==0 else score * (3-empty)

    # Count scores on first diagnol
    diagnol = [
            board.board[0][0],
            board.board[1][1],
            board.board[2][2]]
    alone = all([d == turn or d == 0 for d in diagnol])
    if alone:
        empty = diagnol.count(0)
        score = 3-empty if score==0 else score * (3-empty)

    # Count scores on second diagnol
    diagnol = [
            board.board[0][2],
            board.board[1][1],
            board.board[2][0]]
    alone = all([d == turn or d == 0 for d in diagnol])
    if alone:
        empty = diagnol.count(0)
        score = 3-empty if score==0 else score * (3-empty)

    return score


def minimax(board: Board, is_max: bool, turn: int, starter: int):
#    print(is_max, turn, starter)
    winner = check_winner(board)
    if winner in (-1, 1, 2):
        static_evaluation(board, turn, winner, starter)
        #board.show()
        #print()
       # exit()
        return board

    board.score += heuristic_evaluation(board, turn)
    children = expand(board, turn)
#    print('Children of:')
#    board.show()
#    print()
#    for child in children:
#        child.show()
#        print()
#    print('---------------')
    best = None
    if is_max:
        for child in children:
            _min = minimax(child, False, 1 if turn == 2 else 2, starter)
            print(_min.score)
            if best == None:
                best = child
            elif _min.score > best.score:
                best = child
    else:
        for child in children:
            _max = minimax(child, True, 1 if turn == 2 else 2, starter)
            if best == None:
                best = child
            elif _max.score < best.score:
                best = child
    return best

if __name__ == "__main__":
    board = Board()
    turn = 1
    while True:
        if turn == 1:
            best = minimax(board, True, turn, turn)
            print("The best:")
            best.show()
            print()
            board = best
        else:
            print('my turn')
            exit()
            r, c = [int(i) for i in input().split()]
            board.place(r, c, turn)
        print('Final result:')
        board.show()
        print()
        turn = 1 if turn == 2 else 2


