#include "Solver.hpp"

bool Solver::solve(Grid& grid) {
    return solveHelper(grid, 0, 0);
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

bool Solver::solveHelper(Grid& grid, int row = 0, int col = 0) {
    if (row == Grid::GRID_SIZE) {
        return true;
    }

    if (col == Grid::GRID_SIZE) {
        return solveHelper(grid, row + 1, 0);
    }
    
    if (grid.get(row, col) != 0) {
        return solveHelper(grid, row, col + 1);
    }

    for (int num = 1; num <= Grid::GRID_SIZE; ++num) {
        if (canPlace(grid, row, col, num)) {
            grid.set(row, col, num);
            if (solveHelper(grid, row, col + 1)) {
                return true;
            }
            grid.set(row, col, 0);
        }
    }
    return false;
}

bool Solver::canPlace(const Grid& grid, int row, int col, int num) {
    const int boxRow = (row / 3) * 3;
    const int boxCol = (col / 3) * 3;
    
    for (int i = 0; i < Grid::GRID_SIZE; ++i) {
        if (grid.get(row, i) == num || grid.get(i, col) == num || grid.get(boxRow + i / 3, boxCol + i % 3) == num ) {
            return false;
        }
    }
    return true;
}