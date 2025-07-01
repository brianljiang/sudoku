#include "Solver.hpp"

bool Solver::solve(Grid& grid, Strategy strategy) {
    const std::vector<std::function<bool(Grid&)>> techniques = {
        nakedSingles,   
        hiddenSingles,
        nakedPairs,
        // solvePointingPairs,
        // solveBoxLineReduction,
        // solveXWing,
        // solveSwordfish
    };

    if (strategy != Strategy::BRUTE_FORCE) {
        bool changed;
        do {
            changed = false;
            for (const auto& technique : techniques) {
                changed = changed || technique(grid);
                if (grid.isComplete()) {
                    return true;
                }
            }
        } 
        while (changed);
    }

    if (strategy == Strategy::HYBRID || strategy == Strategy::BRUTE_FORCE) {
        return bruteForce(grid, 0 ,0);
    }

    return grid.isComplete();
}

bool Solver::hasUniqueSolution(const Grid& grid) {
    Grid temp = grid;
    int count = countSolutions(temp, 0, 0);
    return count == 1;
}

int Solver::countSolutions(Grid& grid, int row, int col) {
    constexpr int MAX_SOLUTIONS_NEEDED = 2; 
    
    if (row == Grid::GRID_SIZE) {
        return 1; 
    }
    
    if (col == Grid::GRID_SIZE) {
        return countSolutions(grid, row + 1, 0); 
    }
    
    if (grid.get(row, col) != Grid::EMPTY) {
        return countSolutions(grid, row, col + 1); 
    }
    
    int total = 0;
    for (int num = 1; num <= Grid::GRID_SIZE && total < MAX_SOLUTIONS_NEEDED; ++num) {
        if (canPlace(grid, row, col, num)) {
            grid.set(row, col, num);
            total += countSolutions(grid, row, col + 1);
            grid.set(row, col, Grid::EMPTY);
        }
    }
    return total;
}

bool Solver::bruteForce(Grid& grid, int row = 0, int col = 0) {
    if (row == Grid::GRID_SIZE) {
        return true;
    }

    if (col == Grid::GRID_SIZE) {
        return bruteForce(grid, row + 1, 0);
    }
    
    if (grid.get(row, col) != Grid::EMPTY) {
        return bruteForce(grid, row, col + 1);
    }

    for (int num = 1; num <= Grid::GRID_SIZE; ++num) {
        if (canPlace(grid, row, col, num)) {
            grid.set(row, col, num);
            if (bruteForce(grid, row, col + 1)) {
                return true;
            }
            grid.set(row, col, Grid::EMPTY);
        }
    }
    return false;
}

bool Solver::nakedSingles(Grid& grid) {
    bool changed = false;
    for (int row = 0; row < Grid::GRID_SIZE; ++row) {
        for (int col = 0; col < Grid::GRID_SIZE; ++col) {
            const std::set<int> &cand = grid.getCandidates(row, col);
            if (cand.size() == 1) {
                grid.set(row, col, *cand.begin());
                changed = true;
            }
        }
    }
    return changed;
}

bool Solver::hiddenSingles(Grid& grid) {
    bool changed = false;
    
    for (int row = 0; row < Grid::GRID_SIZE; ++row) {
        std::unordered_map<int, std::vector<int>> numPositions; 
        
        for (int col = 0; col < Grid::GRID_SIZE; ++col) {
            if (grid.get(row, col) == Grid::EMPTY) {
                for (int num : grid.getCandidates(row, col)) {
                    numPositions[num].push_back(col);
                }
            }
        }
        
        for (const auto& [num, cols] : numPositions) {
            if (cols.size() == 1) {
                int col = cols[0];
                if (grid.get(row, col) == Grid::EMPTY) {
                    grid.set(row, col, num);
                    changed = true;
                }
            }
        }
    }

    for (int col = 0; col < Grid::GRID_SIZE; ++col) {
        std::unordered_map<int, std::vector<int>> numPositions; 
        
        for (int row = 0; row < Grid::GRID_SIZE; ++row) {
            if (grid.get(row, col) == Grid::EMPTY) {
                for (int num : grid.getCandidates(row, col)) {
                    numPositions[num].push_back(row);
                }
            }
        }
        
        for (const auto& [num, rows] : numPositions) {
            if (rows.size() == 1) {
                int row = rows[0];
                if (grid.get(row, col) == Grid::EMPTY) {
                    grid.set(row, col, num);
                    changed = true;
                }
            }
        }
    }

    for (int boxRow = 0; boxRow < Grid::GRID_SIZE; boxRow += Grid::SUBGRID_SIZE) {
        for (int boxCol = 0; boxCol < Grid::GRID_SIZE; boxCol += Grid::SUBGRID_SIZE) {
            std::unordered_map<int, std::pair<int, int>> numPositions; 
            
            for (int r = 0; r < Grid::SUBGRID_SIZE; ++r) {
                for (int c = 0; c < Grid::SUBGRID_SIZE; ++c) {
                    int row = boxRow + r;
                    int col = boxCol + c;
                    
                    if (grid.get(row, col) == Grid::EMPTY) {
                        for (int num : grid.getCandidates(row, col)) {
                            numPositions[num] = {row, col};
                        }
                    }
                }
            }
            
            std::unordered_map<int, int> numCounts;
            for (int r = 0; r < Grid::SUBGRID_SIZE; ++r) {
                for (int c = 0; c < Grid::SUBGRID_SIZE; ++c) {
                    int row = boxRow + r;
                    int col = boxCol + c;
                    
                    if (grid.get(row, col) == Grid::EMPTY) {
                        for (int num : grid.getCandidates(row, col)) {
                            numCounts[num]++;
                        }
                    }
                }
            }
            
            for (const auto& [num, count] : numCounts) {
                if (count == 1) {
                    auto [row, col] = numPositions[num];
                    if (grid.get(row, col) == Grid::EMPTY) {
                        grid.set(row, col, num);
                        changed = true;
                    }
                }
            }
        }
    }

    return changed;
}

bool Solver::nakedPairs(Grid& grid) {
    bool changed = false;
    for (int unit = 0; unit < Grid::GRID_SIZE; ++unit) {
        changed |= findNakedPairsInRow(grid, unit);
        changed |= findNakedPairsInColumn(grid, unit);
        
        int boxRow = (unit / Grid::SUBGRID_SIZE) * Grid::SUBGRID_SIZE;
        int boxCol = (unit % Grid::SUBGRID_SIZE) * Grid::SUBGRID_SIZE;
        changed |= findNakedPairsInBox(grid, boxRow, boxCol);
    }
    return changed;
}

bool Solver::findNakedPairsInRow(Grid& grid, int row) {
    bool changed = false;
    std::vector<std::pair<int, std::set<int>>> candidatePairs; // <col, candidates>

    for (int col = 0; col < Grid::GRID_SIZE; ++col) {
        const auto& candidates = grid.getCandidates(row, col);
        if (candidates.size() == 2) {
            candidatePairs.emplace_back(col, candidates);
        }
    }

    for (size_t i = 0; i < candidatePairs.size(); ++i) {
        for (size_t j = i + 1; j < candidatePairs.size(); ++j) {
            if (candidatePairs[i].second == candidatePairs[j].second) {
                const auto& pairValues = candidatePairs[i].second;
                int col1 = candidatePairs[i].first;
                int col2 = candidatePairs[j].first;

                for (int col = 0; col < Grid::GRID_SIZE; ++col) {
                    if (col != col1 && col != col2) {
                        for (int val : pairValues) {
                            if (grid.getCandidates(row, col).count(val)) {
                                grid.toggleCandidate(row, col, val);
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
    return changed;
}

bool Solver::findNakedPairsInColumn(Grid& grid, int col) {
    bool changed = false;
    std::vector<std::pair<int, std::set<int>>> candidatePairs; // <row, candidates>

    for (int row = 0; row < Grid::GRID_SIZE; ++row) {
        const auto& candidates = grid.getCandidates(row, col);
        if (candidates.size() == 2) {
            candidatePairs.emplace_back(row, candidates);
        }
    }

    for (size_t i = 0; i < candidatePairs.size(); ++i) {
        for (size_t j = i + 1; j < candidatePairs.size(); ++j) {
            if (candidatePairs[i].second == candidatePairs[j].second) {
                const auto& pairValues = candidatePairs[i].second;
                int row1 = candidatePairs[i].first;
                int row2 = candidatePairs[j].first;

                for (int row = 0; row < Grid::GRID_SIZE; ++row) {
                    if (row != row1 && row != row2) {
                        for (int val : pairValues) {
                            if (grid.getCandidates(row, col).count(val)) {
                                grid.toggleCandidate(row, col, val);
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
    return changed;
}

bool Solver::findNakedPairsInBox(Grid& grid, int startRow, int startCol) {
    bool changed = false;
    std::vector<std::pair<std::pair<int, int>, std::set<int>>> candidatePairs; // <(row,col), candidates>

    for (int r = 0; r < Grid::SUBGRID_SIZE; ++r) {
        for (int c = 0; c < Grid::SUBGRID_SIZE; ++c) {
            int row = startRow + r;
            int col = startCol + c;
            const auto& candidates = grid.getCandidates(row, col);
            
            if (candidates.size() == 2) {
                candidatePairs.emplace_back(std::make_pair(row, col), candidates);
            }
        }
    }

    for (size_t i = 0; i < candidatePairs.size(); ++i) {
        for (size_t j = i + 1; j < candidatePairs.size(); ++j) {
            if (candidatePairs[i].second == candidatePairs[j].second) {
                const auto& pairValues = candidatePairs[i].second;
                auto [row1, col1] = candidatePairs[i].first;
                auto [row2, col2] = candidatePairs[j].first;

                for (int r = 0; r < Grid::SUBGRID_SIZE; ++r) {
                    for (int c = 0; c < Grid::SUBGRID_SIZE; ++c) {
                        int row = startRow + r;
                        int col = startCol + c;
                        
                        if ((row == row1 && col == col1) || (row == row2 && col == col2)) {
                            continue;
                        }

                        for (int val : pairValues) {
                            if (grid.getCandidates(row, col).count(val)) {
                                grid.toggleCandidate(row, col, val);
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
    return changed;
}

bool Solver::canPlace(const Grid& grid, int row, int col, int num) {
    return grid.getCandidates(row, col).count(num) > 0;
}