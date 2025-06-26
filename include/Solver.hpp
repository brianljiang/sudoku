#pragma once
#include "Grid.hpp"

class Solver {
public:
    // brute force backtracker
    static bool solve(Grid& grid);

    // TODO: add solver with preprocessing to speed up brute force

    // checkers
    static bool hasUniqueSolution(const Grid& grid);
    static int countSolutions(Grid& grid, int row = 0, int col = 0);

private:
    // helpers
    static bool solveHelper(Grid& grid, int row, int col);
    static bool canPlace(const Grid& grid, int row, int col, int num);
};