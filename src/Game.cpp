#include "Game.hpp"

Game::Game(const Grid& grid) : grid(grid) {}

// player actions
bool Game::makeMove(int row, int col, int value) {
     if (row < 0 || row >= Grid::GRID_SIZE || col < 0 || col >= Grid::GRID_SIZE) {
        return false;
    }
    
    if (value < 1 || value > Grid::GRID_SIZE) {
        return false;
    }

    if (grid.getCellState(row, col) == CellState::Editable) {
        Move move;
        move.row = row;
        move.col = col;
        move.oldValue = grid.get(row, col);
        move.oldCandidates = grid.getCandidates(row, col);
        
        grid.set(row, col, value);
        
        move.newValue = value;
        move.newCandidates = grid.getCandidates(row, col);
        
        undoStack.push(move);
        return true;
    }
    return false;
}

void Game::undo() {
    if (!undoStack.empty()) {
        const Move& lastMove = undoStack.top();
        grid.set(lastMove.row, lastMove.col, lastMove.oldValue);
        grid.setCandidates(lastMove.row, lastMove.col, lastMove.oldCandidates);
        redoStack.push(lastMove);
        undoStack.pop();
    }
}

void Game::redo() {
    if (!redoStack.empty()) {
        const Move& nextMove = redoStack.top();
        grid.set(nextMove.row, nextMove.col, nextMove.newValue);
        grid.setCandidates(nextMove.row, nextMove.col, nextMove.newCandidates);
        undoStack.push(nextMove);
        redoStack.pop();
    }
}

void Game::reset(){
    grid.reset();
    clearHistory();
}

// state checks
bool Game::isSolved() const {
    return grid.isComplete();
}

const Grid& Game::getGrid() const{
    return grid;
}

void Game::clearHistory() {
    undoStack = std::stack<Move>(); 
    redoStack = std::stack<Move>();
}
