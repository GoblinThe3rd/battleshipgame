#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <queue>
#include <set>
#include <iomanip>

using namespace std;

class BattleshipGame {
private:
    int boardSize;
    vector<vector<char>> playerBoard;
    vector<vector<char>> computerBoard;
    vector<vector<bool>> playerShots; 
    vector<vector<bool>> computerShots;

   
    int playerShips2, playerShips3, playerShips4;
    int computerShips2, computerShips3, computerShips4;


    queue<pair<int, int>> computerTargets;
    set<pair<int, int>> computerHits;
    vector<pair<int, int>> directions = { {-1,0}, {1,0}, {0,-1}, {0,1} };

    mt19937 rng;

public:
    BattleshipGame() : rng(random_device{}()) {
        playerShips2 = playerShips3 = playerShips4 = 0;
        computerShips2 = computerShips3 = computerShips4 = 0;
        cout << "Ласкаво просимо до гри 'Морський бій'!\n";
        initializeGame();
    }

    void initializeGame() {
        cout << "Введіть розмір поля : ";
        cin >> boardSize;

        if (boardSize < 5) {
            cout << "Мінімальний розмір поля - 5x5!\n";
            boardSize = 5;
        }

        
        playerBoard.assign(boardSize, vector<char>(boardSize, '.'));
        computerBoard.assign(boardSize, vector<char>(boardSize, '.'));
        playerShots.assign(boardSize, vector<bool>(boardSize, false));
        computerShots.assign(boardSize, vector<bool>(boardSize, false));

        cout << "\nПочинаємо розстановку кораблів!\n";
        cout << "Ви можете розставляти кораблі різних розмірів.\n";
        cout << "Доступні розміри: 1x2, 1x3, 1x4\n";
        cout << "Розставляйте кораблі доти, доки не вирішите зупинитися.\n";
        cout << "Комп'ютер автоматично отримає таку ж кількість кораблів кожного розміру.\n\n";

        setupPlayerShips();
        setupComputerShips();

        cout << "\nВсі кораблі розставлені! Починаємо гру!\n";
        displayShipCounts();
        gameLoop();
    }

  
    int getDigitCount(int number) {
        if (number == 0) return 1;
        int count = 0;
        while (number > 0) {
            count++;
            number /= 10;
        }
        return count;
    }

    void printBoard(const vector<vector<char>>& board, bool hideShips = false) {
      
        int width = getDigitCount(boardSize - 1) + 1;

      
        cout << setw(width) << "";

        for (int i = 0; i < boardSize; i++) {
            cout << setw(width) << i;
        }
        cout << "\n";

    
        for (int i = 0; i < boardSize; i++) {
            cout << setw(width - 1) << i << " ";
            for (int j = 0; j < boardSize; j++) {
                char cell = board[i][j];
                if (hideShips && cell == 'S') {
                    cout << setw(width) << '.';
                }
                else {
                    cout << setw(width) << cell;
                }
            }
            cout << "\n";
        }
        cout << "\n";
    }

    bool isValidPosition(int row, int col, int length, bool horizontal, const vector<vector<char>>& board) {
     
        if (horizontal) {
            if (col + length > boardSize) {
                return false;
            }
        } else {
            if (row + length > boardSize) {
                return false;
            }
        }

      
        for (int i = 0; i < length; i++) {
            int r = horizontal ? row : row + i;
            int c = horizontal ? col + i : col;

          
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    int nr = r + dr, nc = c + dc;
                    if (nr >= 0 && nr < boardSize && nc >= 0 && nc < boardSize) {
                        if (board[nr][nc] == 'S') return false;
                    }
                }
            }
        }
        return true;
    }

    void placeShip(int row, int col, int length, bool horizontal, vector<vector<char>>& board) {
        for (int i = 0; i < length; i++) {
            int r = horizontal ? row : row + i;
            int c = horizontal ? col + i : col;
            board[r][c] = 'S';
        }
    }

    void displayCurrentShips() {
        cout << "\nВаші кораблі:\n";
        int totalShips = playerShips2 + playerShips3 + playerShips4;
        if (totalShips == 0) {
            cout << "Поки що немає кораблів.\n";
        }
        else {
            if (playerShips2 > 0) cout << "Розмір 1x2: " << playerShips2 << " кораблів\n";
            if (playerShips3 > 0) cout << "Розмір 1x3: " << playerShips3 << " кораблів\n";
            if (playerShips4 > 0) cout << "Розмір 1x4: " << playerShips4 << " кораблів\n";
        }
        cout << "\n";
    }

    void setupPlayerShips() {
        cout << "Ваше поле:\n";
        printBoard(playerBoard);

        while (true) {
            displayCurrentShips();

            cout << "1 - Розставити новий корабель\n";
            cout << "0 - Завершити розстановку";

            int totalShips = playerShips2 + playerShips3 + playerShips4;

            if (totalShips == 0) {
                cout << " (потрібно розставити хоча б один корабель!)";
            }
            cout << "\nВаш вибір: ";

            int choice;
            cin >> choice;

            if (choice == 0) {
                if (totalShips == 0) {
                    cout << "Потрібно розставити хоча б один корабель!\n";
                    continue;
                }
                break;
            }
            else if (choice == 1) {
               
                int shipSize;
                cout << "\nВиберіть розмір корабля:\n";
                cout << "2 - Корабель 1x2\n";
                cout << "3 - Корабель 1x3\n";
                cout << "4 - Корабель 1x4\n";
                cout << "Ваш вибір: ";
                cin >> shipSize;

                if (shipSize < 2 || shipSize > 4) {
                    cout << "Неправильний розмір! Виберіть 2, 3 або 4.\n";
                    continue;
                }

                bool placed = false;
                int attempts = 0;
                const int MAX_PLAYER_ATTEMPTS = 3;

                while (!placed && attempts < MAX_PLAYER_ATTEMPTS) {
                    int row, col, orientation;
                    cout << "\nРозставляння корабля розміром " << "1x"  << shipSize<< "\n";
                    cout << "Введіть координати (рядок стовпець): ";
                    cin >> row >> col;

                    if (row < 0 || row >= boardSize || col < 0 || col >= boardSize) {
                        cout << "Неправильні координати!\n";
                        attempts++;
                        continue;
                    }

                    cout << "Орієнтація (0 - горизонтально, 1 - вертикально): ";
                    cin >> orientation;

                    bool horizontal = (orientation == 0);

                    if (isValidPosition(row, col, shipSize, horizontal, playerBoard)) {
                        placeShip(row, col, shipSize, horizontal, playerBoard);
                        placed = true;

                        if (shipSize == 2) playerShips2++;
                        else if (shipSize == 3) playerShips3++;
                        else if (shipSize == 4) playerShips4++;

                        cout << "Корабель розставлено!\n";
                        printBoard(playerBoard);
                    }
                    else {
                        cout << "Неможливо розмістити корабель тут! Спробуйте ще раз.\n";
                        attempts++;
                    }
                }

                if (!placed) {
                    cout << "Неможливо розмістити корабель після " << MAX_PLAYER_ATTEMPTS
                        << " спроб! Спробуйте інші координати.\n";
                }
            }
            else {
                cout << "Неправильний вибір! Введіть 0 або 1.\n";
            }
        }

     
        computerShips2 = playerShips2;
        computerShips3 = playerShips3;
        computerShips4 = playerShips4;
    }

    void setupComputerShips() {
        cout << "Комп'ютер розставляє кораблі...\n";

        uniform_int_distribution<int> posDist(0, boardSize - 1);
        uniform_int_distribution<int> orientDist(0, 1);


        if (computerShips2 > 0) {
            cout << "Розставляння кораблів розміром 1x2 (" << computerShips2 << " штук)...\n";
            int successfulShips = 0;

            for (int ship = 0; ship < computerShips2; ship++) {
                bool placed = false;
                int attempts = 0;
                const int MAX_ATTEMPTS = 2000;

                while (!placed && attempts < MAX_ATTEMPTS) {
                    int row = posDist(rng);
                    int col = posDist(rng);
                    bool horizontal = orientDist(rng);

                    if (isValidPosition(row, col, 2, horizontal, computerBoard)) {
                        placeShip(row, col, 2, horizontal, computerBoard);
                        placed = true;
                        successfulShips++;
                    }
                    attempts++;
                }

                if (!placed) {
                    cout << "Комп'ютер не зміг розмістити корабель " << (ship + 1)
                        << " розміром 1x2!\n";
                    break;
                }
            }

            computerShips2 = successfulShips;
            cout << "Комп'ютер розставив " << successfulShips << " кораблів розміром 1x2.\n";
        }

      
        if (computerShips3 > 0) {
            cout << "Розставляння кораблів розміром 1x3 (" << computerShips3 << " штук)...\n";
            int successfulShips = 0;

            for (int ship = 0; ship < computerShips3; ship++) {
                bool placed = false;
                int attempts = 0;
                const int MAX_ATTEMPTS = 2000;

                while (!placed && attempts < MAX_ATTEMPTS) {
                    int row = posDist(rng);
                    int col = posDist(rng);
                    bool horizontal = orientDist(rng);

                    if (isValidPosition(row, col, 3, horizontal, computerBoard)) {
                        placeShip(row, col, 3, horizontal, computerBoard);
                        placed = true;
                        successfulShips++;
                    }
                    attempts++;
                }

                if (!placed) {
                    cout << "Комп'ютер не зміг розмістити корабель " << (ship + 1)
                        << " розміром 1x3!\n";
                    break;
                }
            }

            computerShips3 = successfulShips;
            cout << "Комп'ютер розставив " << successfulShips << " кораблів розміром 1x3.\n";
        }

      
        if (computerShips4 > 0) {
            cout << "Розставляння кораблів розміром 1x4 (" << computerShips4 << " штук)...\n";
            int successfulShips = 0;

            for (int ship = 0; ship < computerShips4; ship++) {
                bool placed = false;
                int attempts = 0;
                const int MAX_ATTEMPTS = 2000;

                while (!placed && attempts < MAX_ATTEMPTS) {
                    int row = posDist(rng);
                    int col = posDist(rng);
                    bool horizontal = orientDist(rng);

                    if (isValidPosition(row, col, 4, horizontal, computerBoard)) {
                        placeShip(row, col, 4, horizontal, computerBoard);
                        placed = true;
                        successfulShips++;
                    }
                    attempts++;
                }

                if (!placed) {
                    cout << "Комп'ютер не зміг розмістити корабель " << (ship + 1)
                        << " розміром 1x4!\n";
                    break;
                }
            }

            computerShips4 = successfulShips;
            cout << "Комп'ютер розставив " << successfulShips << " кораблів розміром 1x4.\n";
        }
    }

    void displayShipCounts() {
        cout << "\n=== КОНФІГУРАЦІЯ КОРАБЛІВ ===\n";
        cout << "Ваші кораблі:\n";
        if (playerShips2 > 0) cout << "  Розмір 1x2: " << playerShips2 << " кораблів\n";
        if (playerShips3 > 0) cout << "  Розмір 1x3: " << playerShips3 << " кораблів\n";
        if (playerShips4 > 0) cout << "  Розмір 1x4: " << playerShips4 << " кораблів\n";

        cout << "Кораблі комп'ютера:\n";
        if (computerShips2 > 0) cout << "  Розмір 1x2: " << computerShips2 << " кораблів\n";
        if (computerShips3 > 0) cout << "  Розмір 1x3: " << computerShips3 << " кораблів\n";
        if (computerShips4 > 0) cout << "  Розмір 1x4: " << computerShips4 << " кораблів\n";
        cout << "===============================\n\n";
    }

    bool isShipDestroyed(int row, int col, const vector<vector<char>>& board) {
     
        vector<pair<int, int>> shipCells;
        vector<vector<bool>> visited(boardSize, vector<bool>(boardSize, false));

      
        findShipCells(row, col, board, visited, shipCells);

       
        for (auto& cell : shipCells) {
            if (board[cell.first][cell.second] != 'X') {
                return false;
            }
        }

        return !shipCells.empty();
    }

    void findShipCells(int r, int c, const vector<vector<char>>& board, vector<vector<bool>>& visited, vector<pair<int, int>>& shipCells) {
        if (r < 0 || r >= boardSize || c < 0 || c >= boardSize ||
            visited[r][c] || (board[r][c] != 'X' && board[r][c] != 'S')) {
            return;
        }
        visited[r][c] = true;
        shipCells.push_back({ r, c });

      
        for (auto& dir : directions) {
            findShipCells(r + dir.first, c + dir.second, board, visited, shipCells);
        }
    }

    void markAroundDestroyedShip(int row, int col, vector<vector<char>>& board, vector<vector<bool>>& shotBoard) {
     
        vector<pair<int, int>> shipCells;
        vector<vector<bool>> visited(boardSize, vector<bool>(boardSize, false));

        findDestroyedShipCells(row, col, board, visited, shipCells);

       
        set<pair<int, int>> aroundCells;
        for (auto& cell : shipCells) {
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    int nr = cell.first + dr;
                    int nc = cell.second + dc;
                    if (nr >= 0 && nr < boardSize && nc >= 0 && nc < boardSize &&
                        board[nr][nc] != 'X') {
                        aroundCells.insert({ nr, nc });
                    }
                }
            }
        }

       
        for (auto& cell : aroundCells) {
            if (board[cell.first][cell.second] == '.') {
                board[cell.first][cell.second] = 'O';
                shotBoard[cell.first][cell.second] = true;
            }
        }
    }

    void findDestroyedShipCells(int r, int c, const vector<vector<char>>& board, vector<vector<bool>>& visited, vector<pair<int, int>>& shipCells) {
        if (r < 0 || r >= boardSize || c < 0 || c >= boardSize ||
            visited[r][c] || board[r][c] != 'X') {
            return;
        }
        visited[r][c] = true;
        shipCells.push_back({ r, c });

        for (auto& dir : directions) {
            findDestroyedShipCells(r + dir.first, c + dir.second, board, visited, shipCells);
        }
    }

    bool makeShot(int row, int col, vector<vector<char>>& targetBoard, vector<vector<bool>>& shotBoard, bool isPlayer) {
        if (shotBoard[row][col]) {
            if (isPlayer) cout << "Ви вже стріляли сюди!\n";
            return false;
        }

        shotBoard[row][col] = true;

        if (targetBoard[row][col] == 'S') {
            targetBoard[row][col] = 'X';

       
            bool shipDestroyed = isShipDestroyed(row, col, targetBoard);

            if (isPlayer) {
                if (shipDestroyed) {
                    cout << "Попадання! Корабель знищено!\n";
                    markAroundDestroyedShip(row, col, targetBoard, shotBoard);
                }
                else {
                    cout << "Попадання!\n";
                }
            }
            else {
                if (shipDestroyed) {
                    cout << "Комп'ютер знищив ваш корабель!\n";
                    markAroundDestroyedShip(row, col, targetBoard, shotBoard);
               
                    while (!computerTargets.empty()) computerTargets.pop();
                }
                else {
                    cout << "Комп'ютер влучив у ваш корабель!\n";
                
                    computerHits.insert({ row, col });
               
                    for (auto& dir : directions) {
                        int nr = row + dir.first;
                        int nc = col + dir.second;
                        if (nr >= 0 && nr < boardSize && nc >= 0 && nc < boardSize &&
                            !computerShots[nr][nc]) {
                            computerTargets.push({ nr, nc });
                        }
                    }
                }
            }
            return true;
        }
        else {
            targetBoard[row][col] = 'O'; 
            if (isPlayer) {
                cout << "Промах!\n";
            }
            else {
                cout << "Комп'ютер промахнувся.\n";
            }
            return false;
        }
    }

    pair<int, int> getComputerMove() {
     
        if (!computerTargets.empty()) {
            auto target = computerTargets.front();
            computerTargets.pop();

         
            if (!computerShots[target.first][target.second]) {
                return target;
            }
       
            return getComputerMove();
        }

       
        uniform_int_distribution<int> dist(0, boardSize - 1);
        int row, col;
        do {
            row = dist(rng);
            col = dist(rng);
        } while (computerShots[row][col]);

        return { row, col };
    }

    bool isGameOver() {
      
        bool playerHasShips = false;
        bool computerHasShips = false;

        for (int i = 0; i < boardSize; i++) {
            for (int j = 0; j < boardSize; j++) {
                if (playerBoard[i][j] == 'S') playerHasShips = true;
                if (computerBoard[i][j] == 'S') computerHasShips = true;
            }
        }

        if (!playerHasShips) {
            cout << "Комп'ютер переміг! Всі ваші кораблі знищено.\n";
            return true;
        }

        if (!computerHasShips) {
            cout << "Вітаємо! Ви перемогли! Всі кораблі комп'ютера знищено.\n";
            return true;
        }

        return false;
    }

    void gameLoop() {
        bool playerTurn = true;

        while (!isGameOver()) {
            cout << "\n" << string(50, '=') << "\n";

            if (playerTurn) {
                cout << "Ваш хід!\n";
                cout << "Поле комп'ютера:\n";
                printBoard(computerBoard, true);
                cout << "Ваше поле:\n";
                printBoard(playerBoard);

                int row, col;
                cout << "Введіть координати для пострілу (рядок стовпець): ";
                cin >> row >> col;

                if (row < 0 || row >= boardSize || col < 0 || col >= boardSize) {
                    cout << "Неправильні координати!\n";
                    continue;
                }

                bool hit = makeShot(row, col, computerBoard, playerShots, true);
                if (!hit) playerTurn = false; 

            }
            else {
                cout << "Хід комп'ютера...\n";
                auto move = getComputerMove();
                cout << "Комп'ютер стріляє по клітинці (" << move.first
                    << ", " << move.second << ")\n";

                bool hit = makeShot(move.first, move.second, playerBoard,
                    computerShots, false);
                if (!hit) playerTurn = true;
            }
        }

        cout << "\nФінальний стан гри:\n";
        cout << "Поле комп'ютера:\n";
        printBoard(computerBoard);
        cout << "Ваше поле:\n";
        printBoard(playerBoard);
    }
};


int main() {
    setlocale(LC_ALL, "Ukr");
    BattleshipGame game;
}
