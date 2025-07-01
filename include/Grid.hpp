#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <array>
#include <vector>
#include <set>
#include <algorithm>

enum class CellState { Editable, Fixed };

class Grid {
    public:
        static constexpr int GRID_SIZE = 9; // size for whole sudoku board
        static constexpr int SUBGRID_SIZE = 3; // size for a subgrid
        static constexpr int EMPTY = 0; // if cell == 0 -> empty
        static const std::set<int> ALL_CANDIDATES;  // pencil marks

        Grid();

        // core functionality 
        int get(int row, int col) const;
        void set(int row, int col, int val);
        CellState getCellState(int row, int col) const;
        void setCellState(int row, int col, CellState state);
        const std::set<int>& getCandidates(int row, int col) const;
        void updateAllCandidates();
        void updateCandidatesForCell(int row, int col);
        void setCandidates(int row, int col, const std::set<int>& newCandidates);
        void clearCellCandidates(int row, int col); 
        void clearCandidates(); 
        void toggleCandidate(int row, int col, int digit);
        void reset();

        // reading input
        void loadFromStrings(const std::vector<std::string>& input);
        static std::vector<std::string> readPuzzleFromConsole();

        // checkers
        bool isValid() const;
        bool isComplete() const;

        // visualization
        void printBoard() const;
        void prettyPrintBoard() const;
    private:
        // data
        std::array<std::array<int, GRID_SIZE>, GRID_SIZE> cells;
        std::array<std::array<CellState, GRID_SIZE>, GRID_SIZE> cellStates;
        std::array<std::array<std::set<int>, GRID_SIZE>, GRID_SIZE> candidates;

        // helpers
        int getBoxIndex(int row, int col) const;
};