 /**
  * File:    pro.cpp
  * Date:    April 2018
  * Author:  Klara Necasova
  * Email:   xnecas24@stud.fit.vutbr.cz
  * Project: Implementation of preorder order assignment to vertexes - project 3 for PRL 
  */

#include <cmath>
#include <cstring>
#include <iostream> 
#include <mpi.h>
#include <stdlib.h>
  
#define TAG 0

using namespace std;

/**
 * Computes next edge in Euler path.
 * @param myId Process ID.
 * @param numOfNodes Number of nodes (length of input).
 * @param numOfProcs Number of processes.
 * @return Next edge in Euler path.
 */ 
int computeNextEdge(int myId, int numOfNodes, int numOfProcs) 
{
	int next;

	// next edge setting in case of two nodes
	if (numOfNodes == 2 && myId == 0) {
		return 1;
	}
	else if (numOfNodes == 2 && myId == 1) {
		return -1;
	}
	// the last edge has no next edge
	else if (myId == numOfNodes) {
		next = -1;
	}
	// goes to right child of root
	else if (myId == numOfNodes - 1) {
		next = 1;
	}
	// goes to parent from right child
	else if (myId >= numOfNodes - 1 && (myId == numOfProcs - 1 || (myId - (numOfNodes - 1)) % 2 == 1)) { 
		if ((myId - (numOfNodes - 1)) % 2 == 0)
			next = (myId - (numOfNodes - 1) - 2)/2 + numOfNodes - 1;
		else
			next = (myId - (numOfNodes - 1) - 3)/2 + numOfNodes - 1;
	}
	// goes to right child from left child
	else if (myId >= numOfNodes - 1) {
		next = myId - (numOfNodes - 2);
	}
	// goes to itself using reverse edge
	else if (myId + (numOfNodes - 1) >= numOfProcs - ceil((float)numOfNodes/2)) {
		next = myId + numOfNodes - 1;
	}
	// goes to left child
	else if (myId < numOfNodes - 1) {
		next = 2 * myId + 2;
	}
	
	return next;
}

/**
 * Gets edge weight.
 * @param myId Process ID.
 * @param numOfNodes Number of nodes (length of input).
 * @return Weight of edge.
 */ 
int getWeight(int myId, int numOfNodes)
{
	// 0 - reversed edge, 1 - forward edge
	if (myId >= 0 && myId <= numOfNodes - 2)
		return 1;
	return 0;
}

/**
 * Computes suffix sum.
 * @param myId Process ID.
 * @param numOfNodes Number of nodes (length of input).
 * @param numOfProcs Number of processes.
 * @param next Next edge in Euler path.
 * @return Suffix sum.
 */
int computeSuffixSum(int myId, int numOfNodes, int numOfProcs, int next)
{
	int *sendData = new int[2];
	int *receiveData = new int[2];
	int stopReceiving = 0;
	int stopReceivingTmp = 0;
	int weight;
	MPI_Status status;
	
	// gets weight of edge
	weight = getWeight(myId, numOfNodes);
	
	// in case of two or less nodes, returns weight immediately
	if (numOfNodes < 3)
		return weight;

	if (myId == 0) {
		stopReceiving = 1;
		stopReceivingTmp = 1;
	}
	sendData[0] = next;
	sendData[1] = weight;
	receiveData[0] = 0;
	receiveData[1] = 0;
	
	// computes suffix sum	
	for (int i = 0; i < (int)ceil(log2(numOfProcs)); i++) {
		// sends/receives stopReceiving flag, if stopReceiving == true, edge will not receive any data
		if (stopReceiving == 1 && next != numOfNodes && next != -1)
			MPI_Send(&stopReceiving, 1, MPI_INT, next, TAG, MPI_COMM_WORLD);
		else if (next == numOfNodes && stopReceiving != 1)
			MPI_Recv(&stopReceivingTmp, 1, MPI_INT, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);
		else if (next != numOfNodes && next != -1 && stopReceiving != 1)
			MPI_Sendrecv(&stopReceiving, 1, MPI_INT, next, TAG, &stopReceivingTmp, 1, MPI_INT, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);
		
		MPI_Barrier(MPI_COMM_WORLD);

		// sends/receives next edge and weight
		if (stopReceiving == 1 && next != numOfNodes && next != -1)
			MPI_Recv(receiveData, 2, MPI_INT, next, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		else if (next == numOfNodes && stopReceiving != 1)
			MPI_Send(sendData, 2, MPI_INT, status.MPI_SOURCE, TAG, MPI_COMM_WORLD);
		else if (next != numOfNodes && next != -1 && stopReceiving != 1)
			MPI_Sendrecv(sendData, 2, MPI_INT, status.MPI_SOURCE, TAG, receiveData, 2, MPI_INT, next, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		// updates stopReceiving flag, weight and next edge
		stopReceiving = stopReceivingTmp;
		if (next != -1 && next != numOfNodes)
			next = receiveData[0];
		weight += receiveData[1];
		
		// stores updated next edge and weight that will send in next iteration
		sendData[0] = next;
		sendData[1] = weight;
		// cleans buffer for data receiving
		receiveData[0] = 0;
		receiveData[1] = 0;
	
		MPI_Barrier(MPI_COMM_WORLD);
	}
	
	delete[] sendData;
	delete[] receiveData;
	
	return weight;
}

/**
 * Computes and prints preorder order of nodes.
 * @param myId Process ID.
 * @param numOfNodes Number of nodes (length of input).
 * @param weight Weight of edge.
 * @param argv Arguments.
 */
void computeAndPrintPreorderPosition(int myId, int numOfNodes, int weight, char *argv[])
{
	// master is the first reversed edge
	int master = numOfNodes - 1;
	// computes position in preorder
	if (myId < numOfNodes - 1) {
		int preorderPosition = numOfNodes - weight + 1;
		MPI_Send(&preorderPosition, 1, MPI_INT, master, TAG, MPI_COMM_WORLD);
	}

        // master receives and prints node order
	if (myId == master) {
		char *output = new char[numOfNodes];
		// the first char in input (root) is always at first position
		output[0] = argv[1][0];
		for (int i = 0; i < numOfNodes - 1; i++) {
			int position;
			MPI_Recv(&position, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			output[position - 1] = argv[1][i + 1];
		}
		int i;
		for (i = 0; i < numOfNodes - 1; i++)
			cout << output[i];
		cout << output[i] << endl;
		
		delete[] output;
	}
}
  
 /**
  * Main program.
  */
int main(int argc, char *argv[])
{
	int myId;
	int numOfProcs;	
	int next;
	int suffixSum;
	
	int numOfNodes = strlen(argv[1]);

	// MPI initialization     
	MPI_Init(&argc, &argv);
	// number of running processes
	MPI_Comm_size(MPI_COMM_WORLD, &numOfProcs);
	// id of my process
	MPI_Comm_rank(MPI_COMM_WORLD, &myId);
	
	// gets next edge in Euler path
	next = computeNextEdge(myId, numOfNodes, numOfProcs);
	// computes suffix sum
	suffixSum = computeSuffixSum(myId, numOfNodes, numOfProcs, next);
	// computes preorder position and prints preorder order of nodes
	computeAndPrintPreorderPosition(myId, numOfNodes, suffixSum, argv);

	MPI_Finalize();
	
	return EXIT_SUCCESS;
}
