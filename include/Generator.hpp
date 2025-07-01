#pragma once
#include "Grid.hpp"
#include "Solver.hpp"
#include <random>

class Generator {
public:
    // value = starting square amount
    enum class Difficulty {
        EASY = 35,    
        MEDIUM = 30,   
        HARD = 25,     
        EXPERT = 20    
    };

    Generator();

    Grid generate(Difficulty diff = Difficulty::EASY);

private:
    std::mt19937 rng; 

    // helpers
    void fillDiagonal(Grid& grid);
    bool fillRemaining(Grid& grid, int row, int col);
    void removeNumbers(Grid& grid, Difficulty difficulty);
    int countFilledCells(const Grid& grid) const;
};