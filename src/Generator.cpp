#include "Generator.hpp"
#include "Solver.hpp"
#include <algorithm>

Generator::Generator() : rng(std::random_device{}()) {}

Grid Generator::generate(Difficulty difficulty) {
    Grid grid;
    Solver solver;
    
    fillDiagonal(grid);
    solver.solve(grid, Solver::Strategy::BRUTE_FORCE);
    removeNumbers(grid, difficulty);

    for (int i = 0; i < Grid::GRID_SIZE; ++i) {
        for (int j = 0; j < Grid::GRID_SIZE; ++j) {
            if (grid.get(i, j) != Grid::EMPTY) {
                grid.setCellState(i, j, CellState::Fixed);
                grid.clearCellCandidates(i, j);
            }
        }
    }
    
    grid.updateAllCandidates();
    return grid;
}

void Generator::fillDiagonal(Grid& grid) {
    for (int box = 0; box < Grid::GRID_SIZE; box += Grid::SUBGRID_SIZE) {
        std::array<int, Grid::SUBGRID_SIZE * Grid::SUBGRID_SIZE> nums;
        std::iota(nums.begin(), nums.end(), 1);
        std::shuffle(nums.begin(), nums.end(), rng);
        
        int index = 0;
        for (int i = 0; i < Grid::SUBGRID_SIZE; ++i) {
            for (int j = 0; j < Grid::SUBGRID_SIZE; ++j) {
                grid.set(box + i, box + j, nums[index++]);
            }
        }
    }
}

void Generator::removeNumbers(Grid& grid, Difficulty difficulty) {
    int targetClues = static_cast<int>(difficulty);
    int currentClues = countFilledCells(grid);
    
    std::vector<std::pair<int, int>> positions;
    for (int i = 0; i < Grid::GRID_SIZE; ++i) {
        for (int j = 0; j < Grid::GRID_SIZE; ++j) {
            if (grid.get(i, j) != Grid::EMPTY) {
                positions.emplace_back(i, j);
            }
        }
    }
    
    std::shuffle(positions.begin(), positions.end(), rng);
    
    for (const auto& [row, col] : positions) {
        if (currentClues <= targetClues) {
            break;
        }
        
        int savedValue = grid.get(row, col);
        grid.set(row, col, Grid::EMPTY);
        
        Grid temp = grid;
        if (Solver::hasUniqueSolution(temp)) {
            --currentClues;
        } 
        else {
            grid.set(row, col, savedValue);
            grid.updateCandidatesForCell(row, col);
        }
    }
}

int Generator::countFilledCells(const Grid& grid) const {
    int count = 0;
    for (int i = 0; i < Grid::GRID_SIZE; ++i) {
        for (int j = 0; j < Grid::GRID_SIZE; ++j) {
            if (grid.get(i, j) != Grid::EMPTY) {
                count++;
            }
        }
    }
    return count;
}