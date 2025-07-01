#pragma once
#include "Grid.hpp"

class Solver {
public:
    enum class Strategy {
        BRUTE_FORCE,       
        HUMAN,      
        HYBRID             
    };

    // solve function
    static bool solve(Grid& grid, Strategy strategy);

    // TODO: add solver with preprocessing to speed up brute force

    // checkers (used in Solver and Generator)
    static bool hasUniqueSolution(const Grid& grid);
    static int countSolutions(Grid& grid, int row = 0, int col = 0);

private:
    // helpers
    // types of solves
    static bool bruteForce(Grid& grid, int row, int col);
    static bool nakedSingles(Grid& grid);
    static bool hiddenSingles(Grid& grid);
    static bool nakedPairs(Grid& grid);

    // solve helpers
    static bool findNakedPairsInRow(Grid& grid, int row);
    static bool findNakedPairsInColumn(Grid& grid, int col);
    static bool findNakedPairsInBox(Grid& grid, int startRow, int startCol);

    // general helper
    static bool canPlace(const Grid& grid, int row, int col, int num);
};