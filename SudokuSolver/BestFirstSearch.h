#ifndef BEST_FIRST_SEARCH_H
#define BEST_FIRST_SEARCH_H

#include "Searchable.h"

#include <memory>
using std::unique_ptr;
#include <queue>
using std::priority_queue;
#include <utility>
using std::move;

//Sort in ascending order, depending on heuristicValue.
class HeurusticComparison {
	public:    
		bool operator()(unique_ptr<Searchable> & a, unique_ptr<Searchable> & b) const {
			return a->heuristicValue() > b->heuristicValue(); 
		}
};

class BestFirstSearch {

protected:

    /// Make sure you increment this every time you 'expand' a node, by getting it successors and putting them on the queue        
    int nodes = 0;
	priority_queue<unique_ptr<Searchable>, vector<unique_ptr<Searchable>>, HeurusticComparison> sortedQueue;
    
    
public:

    BestFirstSearch(std::unique_ptr<Searchable> && startFrom) {
        sortedQueue.push(move(startFrom));
    }
    
    int getNodesExpanded() const {
        return nodes;
    }
    
    Searchable * solve() {
            
		//if next item is solution return.
        while(!sortedQueue.empty()) {
			if(sortedQueue.top() -> isSolution()) {
				return sortedQueue.top().get();
			}
			
			
			//Move the front queue's pointer into an unique_ptr.
			unique_ptr<Searchable> curr(move(const_cast<unique_ptr<Searchable> &>(sortedQueue.top())));
			sortedQueue.pop();
			
			vector<unique_ptr<Searchable>> successors = curr->successors();
            
            for (auto & successor : successors) {
                // push each successor onto the back of queue.
                sortedQueue.push(move(successor));
            }
			
			nodes++;
			
		}
    
        return nullptr;
    }
};


// Do not edit below this line

#endif
