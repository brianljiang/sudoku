#pragma once
#include "Grid.hpp"
#include <stack>

struct Move {
    int row;
    int col;
    int oldValue;
    int newValue;
    std::set<int> oldCandidates; 
    std::set<int> newCandidates;
};

class Game {
public:
    explicit Game(const Grid& grid);

    // player actions
    bool makeMove(int row, int col, int value);
    void undo();
    void redo();
    void reset();

    // state checks
    bool isSolved() const;
    const Grid& getGrid() const;

private:
    // data
    Grid grid;                     
    std::stack<Move> undoStack;
    std::stack<Move> redoStack; 

    // helpers
    void clearHistory();
};