 /**
  * File:    mss.cpp
  * Date:    April 2018
  * Author:  Klara Necasova
  * Email:   xnecas24@stud.fit.vutbr.cz
  * Project: Implementation of "Merge-splitting sort" algorithm - project 2 for PRL 
  */

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <vector>

using namespace std;

#define TAG 0
#define FILENAME "numbers"

/**
 * Reads input binary file with numbers.
 * @param nums Vector for loaded numbers.
 */ 
void readFile(vector<int> &nums)
{
    int number;
    fstream fin;
    // opens file    
    fin.open(FILENAME, ios::in);
    // reads file
    while(fin.good()) {
        number = fin.get();
        if(!fin.good())
            break;
        nums.push_back(number);
    }
    // closes file
    fin.close();
}

/**
 * Prints out unordered sequence of numbers.
 * @param nums Vector of numbers.
 */
void printUnorderedNums(vector<int>& nums)
{
    for (int i = 0; i < nums.size()-1; i++)
        cout << nums[i] << " ";
    cout << nums[nums.size()-1] << endl;
}

/**
 * Prints out ordered sequence of numbers.
 * @param nums Vector of numbers.
 */
void printOrderedNums(vector<int>& nums)
{
    for (int i = 0; i < nums.size(); i++)
        cout << nums[i] << endl;
}

/**
 * Sends the subsequence to left neighbor 
 * and receives higher half of merged numbers. 
 * @param myNumVec Subsequence of numbers.
 * @param myNumCnt Length of the subsequence.
 * @param myId Process ID. 
 */
void transferOfNeighborNums(vector<int> &myNumVec, int myNumCnt, int myId)
{
	// sends assigned numbers to left neighbor
    MPI_Send(&myNumCnt, 1, MPI_INT, myId - 1, TAG, MPI_COMM_WORLD);
    MPI_Send(myNumVec.data(), myNumCnt, MPI_INT, myId - 1, TAG, MPI_COMM_WORLD);
	// receives higher half of merged numbers
	MPI_Recv(&myNumCnt, 1, MPI_INT, myId - 1, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(myNumVec.data(), myNumCnt, MPI_INT, myId - 1, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

/**
 * Merges own subsequence with the subsequence of right neighbor
 * and splits it.
 * @param myNumVec Subsequence of numbers.
 * @param myId Process ID. 
 */
void mergeAndSplit(vector<int> &myNumVec, int myId)
{
	int neighborNum = 0;
	// gets numbers assigned to right neighbor
	MPI_Recv(&neighborNum, 1, MPI_INT, myId + 1, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	vector<int> neighborVec (neighborNum, 0);
	MPI_Recv(neighborVec.data(), neighborNum, MPI_INT, myId + 1, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	
	// merges "my" numbers with numbers of right neighbor
	vector<int> mergedVec(myNumVec.size() + neighborVec.size());
	merge(myNumVec.begin(), myNumVec.end(), neighborVec.begin(), neighborVec.end(), mergedVec.begin());
	
	// splits merged numbers - lower half is assigned to "me", higher half is assigned to right neighbor 
	int splitIndex = ceil((float)mergedVec.size() / 2);
	myNumVec.assign(mergedVec.begin(), mergedVec.begin() + splitIndex);
	neighborVec.assign(mergedVec.begin() + splitIndex, mergedVec.end());
	MPI_Send(&neighborNum, 1, MPI_INT, myId + 1, TAG, MPI_COMM_WORLD); 
	MPI_Send(neighborVec.data(), neighborNum, MPI_INT, myId + 1, TAG, MPI_COMM_WORLD);  
}

/**
 * Main program.
 */
int main(int argc, char *argv[])
{
    int numOfProcs;
    int myId;
    vector<int> nums;
    int numsPerProc;
    int numOfProcsWithLessNums = -1;
    int actIndex;
    int myNumCnt;
    int numberOfNumbers = 0;

    // MPI initialization     
    MPI_Init(&argc, &argv);
    // number of running processes
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcs);
    // id of my process
    MPI_Comm_rank(MPI_COMM_WORLD, &myId);

    if(myId == 0) {
        // reads file containing random numbers
        readFile(nums);
        // prints unordered numbers
        printUnorderedNums(nums);
		
		// edits number of processes if number of processes is greater than number of numbers
		if (numOfProcs > nums.size())
			numOfProcs = nums.size();
		
        // assigned numbers to one process
        numsPerProc = ceil((float)nums.size() / numOfProcs);
        // number of processes containing less numbers
		if (nums.size() % numOfProcs != 0)
			numOfProcsWithLessNums = numOfProcs - (nums.size() % numOfProcs);
        
        // sends number of processes to each process
        for(int i = 0; i < numOfProcs; i++) {
            if ((numOfProcs - i) == numOfProcsWithLessNums)
                numsPerProc--;
            MPI_Send(&numsPerProc, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
        } 
    }
	
    // broadcasts number of processes to all processes
    MPI_Bcast(&numOfProcs, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
    // terminates redundant processes
    if(myId >= numOfProcs) {
        MPI_Finalize();
        return 0;
    }
	
    // receives number of processes
    MPI_Recv(&myNumCnt, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	
    vector<int> myNumVec (myNumCnt, 0);
	
    // sends part of random numbers to each process 
    if(myId == 0) {
        actIndex = 0;
        // assigned numbers to one process
        numsPerProc = ceil((float)nums.size() / numOfProcs);
        // sends assigned numbers to each process		
        for(int i = 0; i < numOfProcs; i++) {
            if ((numOfProcs - i) == numOfProcsWithLessNums)
                 numsPerProc--;
            if (i != 0)
                MPI_Send(nums.data() + actIndex, numsPerProc, MPI_INT, i, TAG, MPI_COMM_WORLD);
            else
                // process 0 also stores assigned numbers
                memcpy(static_cast<void*>(myNumVec.data()), static_cast<void*>(nums.data() + actIndex), sizeof(int) * numsPerProc);
            actIndex += numsPerProc;
        }
    }
    else {
        // processes stores assigned numbers
        MPI_Recv(myNumVec.data(), myNumCnt, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    // sorts assigned numbers
    if(myNumCnt >= 2)
        sort(myNumVec.begin(), myNumVec.end());
    
    // number of iterations    
    int numberOfIterations = (int)ceil(numOfProcs / 2.0) + 1;
    for (int i = 0; i < numberOfIterations; i++) {
		if(myId % 2 == 1) {
            // odd processes send assigned numbers to left neighbor,
			// odd processes then receive higher half of merged numbers
			transferOfNeighborNums(myNumVec, myNumCnt, myId);
        } 
		else if ((myId % 2 == 0) && (myId + 1 != numOfProcs)) {
            // even processes merge and split their assigned numbers and
			// numbers of their right neighbors
			mergeAndSplit(myNumVec, myId);          
        }
		
		if(myId % 2 == 0 && myId > 0) {
            // even processes send assigned numbers to left neighbor,
			// even processes then receive higher half of merged numbers
			transferOfNeighborNums(myNumVec, myNumCnt, myId);
        }
        else if ((myId % 2 != 0) && (myId + 1 != numOfProcs)) {
            // odd processes merge and split their assigned numbers and
			// numbers of their right neighbors
            mergeAndSplit(myNumVec, myId);         
        }
    }
    
	// each process sends its ordered part of numbers to the process 0
	MPI_Send(&myNumCnt, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
    MPI_Send(myNumVec.data(), myNumCnt, MPI_INT, 0, TAG, MPI_COMM_WORLD);
    
	vector<int> results;
    if (myId == 0) {
        int actIndex = 0;
		// receives and joins ordered parts of numbers from all processes
        for (int i = 0; i < numOfProcs; i++) {
            int myNums = 0;
            MPI_Recv(&myNums, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(nums.data() + actIndex, myNums, MPI_INT, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            actIndex += myNums;
        }
    }
    
    // prints ordered numbers
    printOrderedNums(nums);
    
    MPI_Finalize();
    return 0;
}