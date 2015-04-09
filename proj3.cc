/***********************************************************************
 *
 * CISC 3595, Fall 2013
 * Project 3: Evaluating the LRU Algorithm
 *
 * Author: Ken Yoneda
 * Date: 9 December 2013
 *
 * This program evaluates the LRU algorithm. It takes as input (from
 * file or cin) the number of physical page frames, virtual page
 * frames and a reference string of page requests. The program then
 * calculates and prints the following: (1) "snapshot" of the stack,
 * (2) # of page faults.
 **********************************************************************/

/*
 * includes
 */
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

/*
 * datatype declarations
 */
struct Memory
{
    unsigned int physPgs;         // number of physical page frames
    unsigned int virPgs;          // number of virtual pages
    unsigned int pgsInMemory;     // how many pages are currently in memory

    // page frame vector, one slot per virtual page frame
    // Here, pgFrame[i] is the virtual page that currently occupies the
    // i-th page frame.
    // If there are  n  pages of physical memory, then the top  n
    // entries of pgFrame represent physical memory pages, and the
    // remaining entries represent virtual memory pages 
    vector<int> pgFrame;
};

struct Request
{
    vector<int> page;            // the list of page requests
    unsigned int pgFaults;       // number of page faults
};

/*
 * function prototypes
 */
void initialize(char* progname, Memory &mem, Request &req);
void doLRU(Memory &mem, Request &req);
void processRequest(int i, Memory &mem, Request &req);
int search(const Memory& mem, int value);

/*
 * the usual main() function
 */
int main(int argc, char *argv[])
{
    Memory mem;                  // memory
    Request req;                 // requests
    
    // command-line argument processing
    
    if (argc == 2) {              // redirect stdin
	if (!(freopen(argv[1], "r", stdin))) {
	    cerr << argv[0] << ": Could not open " << argv[1] << endl;
	    return 1;
	}
    }
    else if (argc != 1) {
	cerr << "Usage: " << argv[0] << " [dataFile]" << endl;
	return 2;
    }
    
    initialize(argv[0], mem, req);
    doLRU(mem, req);
}

/*
 * reads number of physical pages, number of virtual pages, and
 * request string from cin
 * initializes memory and sets sizes of C and F vectors
 */
void initialize(char* progname, Memory &mem, Request &req)
{
    // get # of physical/virtual pages
    int phys_pgs, vir_pgs;
    cin >> phys_pgs >> vir_pgs;
    mem.physPgs = phys_pgs;
    mem.virPgs = vir_pgs;  
    
    // get reference string
    vector<int> request;
    int the_request;
    while (cin >> the_request)
	request.push_back(the_request);
    req.page = request;
    
    // initialize # of pgsInMemory, # of pageFaults, pgFrame vector
    mem.pgsInMemory = 0;
    req.pgFaults = 0;
    mem.pgFrame.assign(mem.virPgs, -1);
}

/*
 * process the page requests using the LRU algorithm
 */
void doLRU(Memory &mem, Request &req)
{
    cout << "Stack:\n";
    
    for (unsigned int i = 0; i < req.page.size(); i++) {
	processRequest(req.page[i], mem, req);
    }

    cout << "Page faults: " << req.pgFaults << endl;
}

/*
 * process the i-th request
 */
void processRequest(int i, Memory &mem, Request &req)
{
    // search
    bool pf = false;
    int position = search(mem, i);
    
    // if i is already in memory
    if (position != -1)
	if (position == 0) {}
	else {
	    mem.pgFrame.erase(mem.pgFrame.begin() + position);
	    mem.pgFrame.insert(mem.pgFrame.begin(), i);
	    if (static_cast<unsigned>(position) >= mem.physPgs){
		req.pgFaults++;
		pf = true;
	    }
	}
    // if i is not in memory
    else if (mem.pgsInMemory < mem.virPgs) {
	mem.pgFrame.pop_back();
	mem.pgFrame.insert(mem.pgFrame.begin(), i);
	mem.pgsInMemory++;
	req.pgFaults++;
	pf = true;
    }
    else {
	mem.pgFrame.pop_back();
	mem.pgFrame.insert(mem.pgFrame.begin(), i);
	req.pgFaults++;
	pf = true;
    }
    
    // print snapshot of stack
    for (unsigned int i = 0; i < mem.pgFrame.size(); i++) {
	if (mem.pgFrame[i] == -1)
	    cout << "_  ";
	else
	    cout << mem.pgFrame[i] << "  ";
	if (i == mem.physPgs - 1)   
	    cout << "|  ";
    }
    
    if (pf)
	cout << "P";
    cout << endl;
}

/*
 * search for value in mem
 * returns location in which value is found, -1 if not found
 */
int search(const Memory& mem, int value)
{
    for (unsigned int i = 0; i <  mem.pgFrame.size(); i++) {
	if (mem.pgFrame[i] == value)
	    return i;
    }
    return -1;
}
