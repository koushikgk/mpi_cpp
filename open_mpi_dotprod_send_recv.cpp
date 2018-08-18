#include "mpi.h"
#include <iostream>
#include <iomanip>

using namespace std;
int main(int argc, char *argv[]) {

//---------- initializing the necessary variables -----------------------------------------
int n, rank, num_procs, *array = NULL, start, end, p_sum, sum_tot, arr_size, arr_size_recv, *array_recv = NULL, sum_recv;
double tic, toc, diff;

//--------- setting up the MPI environment ---------------------------------------
MPI_Init(&argc, &argv);
MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Status sts;
tic =  MPI_Wtime();

//--------- initializing the partial sum to be zero -------------------------------
p_sum = 0;

//--------- master process takes in data values form the user ---------------------
if(rank == 0) 
{
	cout<<"Enter the size of an array\n";
	cin>>arr_size;
	array = new int[arr_size];
	cout<<"Enter the elements of an array\n";
	for(int i=0;i<arr_size;i++)
		cin>>array[i];

//--------- sent the information to the workers --------------------------	
	for(int j=1;j<num_procs;j++) {
		MPI_Send(&arr_size, 1, MPI_INT, j, 1, MPI_COMM_WORLD);          //---------- 1
		MPI_Send(&array[0], arr_size, MPI_INT, j, 2, MPI_COMM_WORLD);         //-------- 2
	}

//--------- set the subarray indices for the master process to work with ------
	start = 0;
	end = arr_size/num_procs;

//--------- calculate the partial sum for masterr process ----------------
	for(int i=start;i<end;i++)
		p_sum += array[i]*array[i];
	cout<<"I am node 0 and my partial sum is : "<<p_sum<<endl;
	sum_tot = p_sum;

//--------- obtain the partial sums from the respective worker processes ------------
	for(int j=1;j<num_procs;j++) {
		MPI_Recv(&sum_recv, 1, MPI_INT, j, 3, MPI_COMM_WORLD, &sts);       //--------- 3
		sum_tot = sum_tot+sum_recv;
	}
	cout<<"I am the master and the total sum is : "<<sum_tot<<endl;
}

//--------- worker recieves data from the master and calculates the partial sum -----------
if(rank != 0) 
{
	MPI_Recv(&arr_size_recv, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &sts);     //----------- 1'
	array_recv = new int[arr_size_recv];
	MPI_Recv(&array_recv[0], arr_size_recv, MPI_INT, 0, 2, MPI_COMM_WORLD, &sts);    //---------- 2'
	start = rank*arr_size_recv/num_procs;
	end = (rank+1)*arr_size_recv/num_procs;
	for(int i=start;i<end;i++) 
		p_sum += array_recv[i]*array_recv[i];
	cout<<"I am node "<<rank<<", and my partial sum is : "<<p_sum<<endl;
	MPI_Send(&p_sum, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);         //----------- 3'
}

//-------- printing the time taken for each process and deleting the arrays to free up memory ----------------------------
toc = MPI_Wtime();
diff = toc - tic; 
cout<<"I am node "<<rank<<", and my time of completion is : "<<diff<<"s"<<endl;
delete [] array; delete [] array_recv;

//-------- close the MPI environment -----------------------
MPI_Finalize();
return 0;
}
