#include <iostream>
#include <string>
#include <vector>

using namespace std;

/*

Design a Tic Tac Toe Game


Requirements

The Tic-Tac-Toe game should be played on a 3x3 grid.
Two players take turns marking their symbols (X or O) on the grid.
The first player to get three of their symbols in a row (horizontally,
vertically, or diagonally) wins the game. If all the cells on the grid are
filled and no player has won, the game ends in a draw. The game should have a
user interface to display the grid and allow players to make their moves. The
game should handle player turns and validate moves to ensure they are legal. The
game should detect and announce the winner or a draw at the end of the game.

*/

class Player {
private:
  string name;
  char symbol;

public:
  Player(string n, char s) : name(n), symbol(s) {}
  string getName() const { return name; }
  char getSymbol() const { return symbol; }
};

class Board {
private:
  vector<vector<char>> grid;

public:
  Board() : grid(3, vector<char>(3, '-')) {}

  void display() const {
    cout << endl;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        cout << " " << grid[i][j];
        if (j < 2)
          cout << " |";
      }
      cout << endl;
      if (i < 2)
        cout << "---|---|---" << endl;
    }
    cout << endl;
  }

  bool makeMove(int row, int col, char symbol) {
    if (row < 0 || row > 2 || col < 0 || col > 2 || grid[row][col] != '-') {
      return false;
    }
    grid[row][col] = symbol;
    return true;
  }

  bool checkWin(char symbol) const {
    for (int i = 0; i < 3; i++) {
      if (grid[i][0] == symbol && grid[i][1] == symbol && grid[i][2] == symbol)
        return true;
      if (grid[0][i] == symbol && grid[1][i] == symbol && grid[2][i] == symbol)
        return true;
    }
    if (grid[0][0] == symbol && grid[1][1] == symbol && grid[2][2] == symbol)
      return true;
    if (grid[0][2] == symbol && grid[1][1] == symbol && grid[2][0] == symbol)
      return true;
    return false;
  }

  bool isFull() const {
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
        if (grid[i][j] == '-')
          return false;
    return true;
  }
};

class Game {
private:
  Board board;
  Player *players[2];
  int currentPlayerIndex;

public:
  Game(Player *p1, Player *p2) : currentPlayerIndex(0) {
    players[0] = p1;
    players[1] = p2;
  }

  void play() {
    cout << "Game Start!" << endl;
    board.display();

    while (true) {
      Player *current = players[currentPlayerIndex];
      cout << current->getName() << "'s turn (" << current->getSymbol()
           << "). Enter row and col (0-2): ";

      int row, col;
      cin >> row >> col;

      if (!board.makeMove(row, col, current->getSymbol())) {
        cout << "Invalid move. Try again." << endl;
        continue;
      }

      board.display();

      if (board.checkWin(current->getSymbol())) {
        cout << current->getName() << " wins!" << endl;
        return;
      }

      if (board.isFull()) {
        cout << "It's a draw!" << endl;
        return;
      }

      currentPlayerIndex = 1 - currentPlayerIndex;
    }
  }
};

int main() {
  Player p1("Player1", 'X');
  Player p2("Player2", 'O');

  Game game(&p1, &p2);
  game.play();

  return 0;
}