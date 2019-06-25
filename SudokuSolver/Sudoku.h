#ifndef SUDOKU_H
#define SUDOKU_H

#include "Searchable.h"

#include <math.h>
#include <iostream>
using std::cout;
using std::endl;

#include <set>
using std::set;


class Sudoku : public Searchable {

	private:
		vector<vector<set<int > > > board;
		int boardSize;
		int boxSize;
		
		//delete the value from all squares in the same row
		void updateRowValues(int row, int col,  int value) {
			
			for(int r = 0; r < boardSize; r++) {
				if(r != row) {
						board[r][col].erase(value);
				}
				
				if(board[r][col].size() > 1) {
					hiddenSingle(r,col);
				}
			}
		}
		
		//delete the value from all squares in the same column
		void updateColumnValues(int row, int col, int value) {
			
			for(int c = 0; c < boardSize; c++) {
				if(c != col) {
						board[row][c].erase(value);
				}
				
				if(board[row][c].size() > 1) {
					hiddenSingle(row,c);
				}
			}
		}
		
		//check the grid for hiddenSingles and if any, assign them to (x,y) square.
		//hiddenSingle = (4,8), (7,8,9), (5,6,7) = 4 as it can be assigned to only 1 square in a given row/column/box.
		void hiddenSingle(int row, int col) {
			
			//for each valid number
			for(int num = 1; num <= boardSize; num++) {
				
				int numRow;
				int numCol;
				int rowOccurence = 0;
				int colOccurence = 0;
				
				int xBox;
				int yBox;
				int boxOccurence = 0;
				
				for(int i = 0; i < boardSize; i++) {
					
					//each time a number is spotted in a row, increment the rowCounter.
					if(board[row][i].find(num) != board[row][i].end()) {
						rowOccurence++;
						if(rowOccurence == 1) { // if only 1 value found => hidden single, save column coordinate
							numCol = i;
						}
					}
					
					//each time a number is spotted in a column, increment the colCounter.
					if(board[i][col].find(num) != board[i][col].end()) {
						colOccurence++;
						if(colOccurence == 1) { // if only 1 value found => hidden single, save row coordinate
							numRow = i;
						}
					}
					
				}
				
				//if any hidden singles found in any row/column, assign them
				if(rowOccurence == 1 &&  board[row][numCol].size() > 1) {
						setSquare(row, numCol, num);
				}
				
				if(colOccurence == 1 && board[numRow][col].size() > 1) {
					if(board[numRow][col].size() > 1) {
						setSquare(numRow, col, num);
					}
				}
				
				//check for hidden singles in the boxes.
				int rowStart = row - (row % boxSize);
				int colStart = col - (col % boxSize);
				
				for(int x = rowStart; x < rowStart + 3; x++) {
					for(int y = colStart; y < colStart + 3; y++) {
						if (board[x][y].find(num) != board[x][y].end()) {
							boxOccurence++;
							if (boxOccurence == 1) {
								xBox = x;
								yBox = y;
							}
						}
					}
				}
				
				if (boxOccurence == 1) {
					if (board[xBox][yBox].size() != 1) {
						setSquare(xBox, yBox, num);
					}
				}
			}
			
		}
		
		//remove duplicate sets from rows and columns
		//if there are (2,3), (2,3,4), (2,3) sets, we can remove 2 and 3 from set:2 as numbers 2 and 3 are guaranteed to be in either set 1 or 3.
		void removeDuplicateSetsRowCol(int row, int col) {
			
			set<set<int > >   duplicateRowSets;
			set<int> duplicateRowNumbers;
			
			set<set<int > >   duplicateColSets;
			set<int> duplicatesColNumbers;
			
			for(int x = 0; x< boardSize-1; x++) {
				for(int y = x+1; y < boardSize; y++) {
					
					
					//check rows for dupilcate sets.
					if(board[row][x] == board[row][y] && board[row][x].size() == 2) {
						duplicateRowSets.insert(board[row][x]);
						duplicateRowNumbers.insert(board[row][x].begin(), board[row][x].end());
						
					}
					
					//check columns for duplicate sets.
					if(board[x][col] == board[y][col] && board[x][col].size() == 2) {
						duplicateColSets.insert(board[x][col]);
						duplicatesColNumbers.insert(board[x][col].begin(), board[x][col].end());
					}
				}
			}
			
			
			//if any duplicate sets, remove the numbers from the rest of the sets in the corresponding row/column.
			for(int index = 0; index < boardSize; index++) {
				
				if(board[row][index].size() > 1 && duplicateRowSets.find(board[row][index]) == duplicateRowSets.end()) {
						for( int num : duplicateRowNumbers) {
							board[row][index].erase(num);
						} 
				}
				
				if(board[index][col].size() > 1 && duplicateColSets.find(board[index][col]) == duplicateColSets.end()) {
						for( int num : duplicatesColNumbers) {
							board[index][col].erase(num);
						}
						
				}
			}
			
		}
		
		//remove duplicate sets from boxes.
		//if any number assignned, remove it from the rest of the squares in the box.
		void updateBox(int row, int col, int value) {
			
			set<set<int > >   duplicateSets;
			set<int> duplicateNumbers;
			int rowStart = row - (row % boxSize); //starting X index of box.
			int colStart = col - (col % boxSize); //starting Y index of box.
			
			for(int r = rowStart; r < rowStart + 3; r++) {
				for(int c = colStart; c < colStart + 3; c++) {
					if(row != r && c != col) { // delete assigned number from the rest squares in the box.
						board[r][c].erase(value);
					}
					
					//check for duplicate sets in the box.
					if(row != col && board[r][c] == board[c][r] && board[r][c].size() == 2) {
						
						duplicateSets.insert(board[r][c]);
						duplicateNumbers.insert(board[r][c].begin(), board[r][c].end());
						
					}
				}
			}
			
			//if any duplicate sets, delete the numbers from the rest of the squares in the box.
			for(int r = rowStart; r < rowStart + 3; r++) {
				for(int c = colStart; c < colStart + 3; c++) {
					if(board[r][c].size() > 1 && duplicateSets.find(board[r][c]) == duplicateSets.end()) {
						for( int num : duplicateNumbers) {
							board[r][c].erase(num);
						} 
					}
					
					//check for hidden singles.
					if(board[r][c].size() > 1) {
						hiddenSingle(r,c);
					}
					
				}
			}
			
		}
		
	public:
		Sudoku(int size) : boardSize(size) {
			board = vector<vector<set<int > > >(boardSize);
			boxSize = sqrt(boardSize);
			set<int> temp;
			for(int i = 1; i <= boardSize; i++) {
				temp.emplace(i);
			}
			
			for(int a = 0; a < boardSize; a++) {
				for(int b = 0; b < boardSize; b++) {
					board[a].push_back(temp);
				}
			}
		}
		
		int getSquare(int row, int col) const {
			
			if(board[row][col].size() == 1) {
				return *(board[row][col].begin());
			}
			
			return -1;
			
		}
		
		 bool setSquare(int row, int col, int value) {
			
			board[row][col].clear();
			board[row][col].emplace(value);
			
			hiddenSingle(row,col);
			updateColumnValues(row, col, value);
			updateRowValues(row, col, value);
			removeDuplicateSetsRowCol(row,col);
			updateBox(row,col, value);
			
			
			//if at any time a square is out of options, return false.
			for(int r = 0; r < boardSize; r++) {
				for(int c = 0; c < boardSize; c++) {
					if(board[r][c].size() == 0) {
						return false;
					}
				}
			}
			
			return true;
		}
		
		//check if the current successor filled board is a solution.
		virtual bool isSolution() const override {
			for(int r = 0; r < boardSize; r++) {
				for(int c = 0; c < boardSize; c++) {
					if(board[r][c].size() != 1) {
						return false;
					}
				}
			}
			return true;
		}
		
		
		//fancy board print. Mainly useful for 9x9 grids.
		virtual void  write(ostream & o) const override {
			cout << "--------- ----------- ---------" << endl;
			
			for (int I = 0; I < boardSize; ++I) {
				
				for (int J = 0; J < boardSize; ++J) {
					if (J == 3 || J == 6) {
						cout << "|  ";
					} 

					if (board[J][I].size() == 0)
						cout << " " << "  ";
					else if(board[J][I].size() == 1)
						cout << *board[J][I].begin() << "  ";
					else
						cout << "#" << "  ";
				}

				cout << "\n";
				
				if (I != 8) {
					cout << "\t |\t     |\n";
				}

				if (I == 2 || I == 5) {
					cout << "--------- ----------- ---------";
					cout << "\n";
					cout << "\t |\t     |\n";
				}
			}
			cout << "--------- ----------- ---------" << endl;
			cout << "--------- ----------- ---------" << endl;
			cout << endl;
			cout << endl;
			
			
			
		}
		
		//for every square with multiple options, try filling the board with the posibilities of numbers and check if any of them leads to a solution.
		virtual vector<unique_ptr<Searchable>> successors() const override {
        
			vector<unique_ptr<Searchable>> successors;
			int updatedRow = -1;
			int updatedCol = -1;
			
			//find square with multiple possibilities.
			for(int row = boardSize-1; row >= 0; row--) {
				for(int col = boardSize-1; col >= 0; col--) {
					if(board[row][col].size() > 1) {
						updatedRow = row;
						updatedCol = col;
						break;
					} 
				}
			}
			
			//try to solve the board with the numbers from the board.
			if(updatedRow >= 0 && updatedCol >= 0) {
				for(auto num : board[updatedRow][updatedCol]) {
					Sudoku* copy = new Sudoku(*this);
					if(copy->setSquare(updatedRow, updatedCol, num)) {
						successors.emplace_back(copy);
					} else {
						delete copy;
					}
				}
			}
			
			if (successors.size() == 1 && !(successors[0]->isSolution())) {
				return successors[0]->successors();
			}
			
			return successors;
		}
		
		
		virtual int heuristicValue() const override {
        int num = 0;
        for (int i = 0; i < boardSize; i++) {
            for (int j = 0; j < boardSize; j++) {
                if (board[i][j].size() > 1) {
                    num++;
                }
            }
        }
        return num;
    }
 
};


#endif
