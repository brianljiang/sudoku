#include "Grid.hpp"

const std::set<int> Grid::ALL_CANDIDATES = {1,2,3,4,5,6,7,8,9}; 

Grid::Grid() {
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            cells[row][col] = EMPTY;
            cellStates[row][col] = CellState::Editable;
            candidates[row][col] = ALL_CANDIDATES;
        }
    }
}

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
        if (val != EMPTY) {
            candidates[row][col].clear(); 
        } 
        else {
            candidates[row][col] = ALL_CANDIDATES;  
        }
        updateAllCandidates(); 
    }
}

void Grid::reset() {
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            if (cellStates[row][col] == CellState::Editable) {
                cells[row][col] = EMPTY;
                candidates[row][col] = ALL_CANDIDATES;
            }
        }
    }
    updateAllCandidates();
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
            cellStates[row][col] = (val != 0) ? CellState::Fixed : CellState::Editable;
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

const std::set<int>& Grid::getCandidates(int row, int col) const {
    if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) {
        throw std::out_of_range("Grid::getCandidates - Row or column index out of bounds");
    }
    return candidates[row][col];
}

void Grid::updateCandidatesForCell(int row, int col) {
    if (cells[row][col] != EMPTY) {
        candidates[row][col].clear();
        return;
    }

    std::set<int> possible = ALL_CANDIDATES;

    for (int c = 0; c < GRID_SIZE; ++c) {
        possible.erase(cells[row][c]);
    }

    for (int r = 0; r < GRID_SIZE; ++r) {
        possible.erase(cells[r][col]);
    }

    int boxStartRow = (row / SUBGRID_SIZE) * SUBGRID_SIZE;
    int boxStartCol = (col / SUBGRID_SIZE) * SUBGRID_SIZE;

    for (int r = 0; r < SUBGRID_SIZE; ++r) {
        for (int c = 0; c < SUBGRID_SIZE; ++c) {
            possible.erase(cells[boxStartRow + r][boxStartCol + c]);
        }
    }

    candidates[row][col] = std::move(possible);
}

void Grid::setCandidates(int row, int col, const std::set<int>& newCandidates) {
    if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE) {
        candidates[row][col] = newCandidates;
    }
}

void Grid::clearCellCandidates(int row, int col) {
    candidates[row][col].clear();
}

void Grid::clearCandidates() {
    for (int r = 0; r < GRID_SIZE; ++r) {
        for (int c = 0; c < GRID_SIZE; ++c) {
            candidates[r][c].clear();
        }
    }
}

void Grid::toggleCandidate(int row, int col, int digit) {
    if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) {
        throw std::out_of_range("Grid::toggleCandidate - Row or column index out of bounds");
    }

    if (digit < 0 || digit > GRID_SIZE) {
        throw std::invalid_argument("Grid::toggleCandidate - Digit must be between 0 and 9");
    }

    if (cells[row][col] != EMPTY) {
        return;
    }

    std::set<int> &cellCandidates = candidates[row][col];
    if (cellCandidates.count(digit)) {
        cellCandidates.erase(digit);
    } 
    else {
        cellCandidates.insert(digit);
    }
}

bool Grid::isValid() const {
    bool row[GRID_SIZE][GRID_SIZE] = {};
    bool col[GRID_SIZE][GRID_SIZE] = {};
    bool box[GRID_SIZE][GRID_SIZE] = {};

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (cells[i][j] == EMPTY) {
                continue;
            }

            int num = cells[i][j] - 1; 
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

int Grid::getBoxIndex(int row, int col) const {
    return (row / SUBGRID_SIZE) * SUBGRID_SIZE + (col / SUBGRID_SIZE);
}

void Grid::updateAllCandidates() {
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            if (cells[row][col] != EMPTY) {
                candidates[row][col].clear();
            }
        }
    }

    std::array<std::set<int>, GRID_SIZE> rowForbidden;
    std::array<std::set<int>, GRID_SIZE> colForbidden;
    std::array<std::set<int>, GRID_SIZE> boxForbidden;

    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            const int val = cells[row][col];
            if (val != EMPTY) {
                rowForbidden[row].insert(val);
                colForbidden[col].insert(val);
                boxForbidden[getBoxIndex(row, col)].insert(val);
            }
        }
    }

    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            if (cells[row][col] == EMPTY) {
                std::set<int> possible;
                const int boxIdx = getBoxIndex(row, col);
                
                for (int num : ALL_CANDIDATES) {
                    if (!rowForbidden[row].count(num) &&
                        !colForbidden[col].count(num) && 
                        !boxForbidden[boxIdx].count(num)) {
                        possible.insert(num);
                    }
                }
                candidates[row][col] = std::move(possible);
            }
        }
    }
}