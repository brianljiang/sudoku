#include "Grid.hpp"

// TODO: add error handling

Grid::Grid() {
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            cells[row][col] = EMPTY;
            cellStates[row][col] = CellState::Editable;
        }
    }
};

int Grid::get(int row, int col) const {
    if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) {
        throw std::out_of_range("Grid::get - Row or column index out of bounds");
    }
    return cells[row][col];
}

void Grid::set(int row, int col, int val) {
    if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) {
        throw std::out_of_range("Grid::set - Row or column index out of bounds");
    }
    if (val < 0 || val > GRID_SIZE) {
        throw std::invalid_argument("Grid::set - Value must be between 0 and 9");
    }
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
    if (input.size() != GRID_SIZE) {
        throw std::invalid_argument("Grid::loadFromStrings - Input must have exactly 9 lines");
    }

    for (int row = 0; row < GRID_SIZE; ++row) {
        const std::string& line = input[row];

        if (line.length() != GRID_SIZE) {
            throw std::invalid_argument(
                "Grid::loadFromStrings - Each line must have exactly 9 characters. "
                "Line " + std::to_string(row + 1) + " has " + 
                std::to_string(line.length()) + " characters"
            );
        }

        for (int col = 0; col < GRID_SIZE; ++col) {
            char c = line[col];
            if (c < '0' || c > '9') {
                throw std::invalid_argument(
                    "Grid::loadFromStrings - All characters must be digits 0-9. "
                    "Found '" + std::string(1, c) + "' at row " + 
                    std::to_string(row + 1) + ", column " + std::to_string(col + 1)
                );
            }
            int val = c - '0';
            cells[row][col] = val;
            cellStates[row][col] = static_cast<CellState>(val != 0);
        }
    }
}

std::vector<std::string> Grid::readPuzzleFromConsole() {
    std::vector<std::string> puzzle;
    std::cout << "Enter the Sudoku puzzle (9 lines of 9 digits each, use 0 for empty cells):\n";
    
    for (int i = 0; i < GRID_SIZE; ++i) {
        std::string line;
        while (true) {
            std::cout << "Row " << (i + 1) << ": ";
            std::getline(std::cin, line);
            
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
            
            if (line.length() != GRID_SIZE) {
                std::cout << "Error: Each line must have exactly 9 digits. Try again.\n";
                continue;
            }
            
            bool valid = true;
            for (char c : line) {
                if (c < '0' || c > '9') {
                    valid = false;
                    break;
                }
            }
            
            if (!valid) {
                std::cout << "Error: Only digits 0-9 are allowed. Try again.\n";
                continue;
            }
            
            break;
        }
        puzzle.push_back(line);
    }
    return puzzle;
}

CellState Grid::getCellState(int row, int col) const {
    return cellStates[row][col];
}

void Grid::setCellState(int row, int col, CellState state) {
    cellStates[row][col] = state;
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
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            if (cells[row][col] == EMPTY) {
                return false;
            }
        }
    }
    return isValid();
}

int Grid::getBoxIndex(int row, int col) {
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