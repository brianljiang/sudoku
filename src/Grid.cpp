#include "Grid.hpp"

Grid::Grid() {
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            cells[row][col] = EMPTY;
            cellStates[row][col] = CellState::Editable;
        }
    }
};

int Grid::get(int row, int col) const {
    return cells[row][col];
}

void Grid::set(int row, int col, int val) {
    if (cellStates[row][col] == CellState::Editable) {
        cells[row][col] = val;
    }
}

void Grid::reset() {
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            if (cellStates[row][col] == CellState::Editable) {
                cells[row][col] = EMPTY;
            }
        }
    }
}

void Grid::loadFromStrings(const std::vector<std::string>& input) {
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            char c = input[row][col];
            cells[row][col] = c - '0';  
            if (c != '0') {
                cellStates[row][col] = CellState::Fixed; 
            }
        }
    }
}

std::vector<std::string> Grid::readPuzzleFromConsole() {
    std::vector<std::string> puzzle;
    std::cout << "Enter the Sudoku puzzle (9 lines of 9 digits each):\n";
    
    for (int i = 0; i < 9; ++i) {
        std::string line;
        std::getline(std::cin, line);
        puzzle.push_back(line);
    }
    return puzzle;
}

CellState Grid::getCellState(int row, int col) const {
    return cellStates[row][col];
}

bool Grid::isValid() const {
    bool row[GRID_SIZE][GRID_SIZE] = {};
    bool col[GRID_SIZE][GRID_SIZE] = {};
    bool box[GRID_SIZE][GRID_SIZE] = {};

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (cells[i][j] == EMPTY) {
                continue;  // skip empty cells
            }

            int num = cells[i][j] - 1;  // 0-based
            int boxIdx = getBoxIndex(i, j);

            if (row[i][num] || col[j][num] || box[boxIdx][num]) {
                return false;
            }
            row[i][num] = col[j][num] = box[boxIdx][num] = true;
        }
    }
    return true;
}

bool Grid::isComplete() const {
    for (const auto& row : cells) {      
        for (int cell : row) {
            if (cell == EMPTY) {
                return false;
            }
        }
    }
    return isValid();
}

int Grid::getBoxIndex(int row, int col) const {
    return (row / SUBGRID_SIZE) * SUBGRID_SIZE + (col / SUBGRID_SIZE);
}

void Grid::printBoard() const {
    std::string output;
    output.reserve(GRID_SIZE * (GRID_SIZE + 1));

    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            int val = cells[row][col];
            output += (val == EMPTY ? '0' : static_cast<char>('0' + val));
        }
        output += '\n';
    }

    std::cout << output;
}

void Grid::prettyPrintBoard() const {
    std::ostringstream oss;
    for (int row = 0; row < GRID_SIZE; ++row) {
        if (row % SUBGRID_SIZE == 0) {
            oss << "-------------------------\n";
        }
        for (int col = 0; col < GRID_SIZE; ++col) {
            if (col % SUBGRID_SIZE == 0) {
                oss << "| ";
            }
            oss << (cells[row][col] == EMPTY ? '.' : static_cast<char>('0' + cells[row][col])) << ' ';
        }
        oss << "|\n";
    }
    oss << "-------------------------\n";
    std::cout << oss.str();
}