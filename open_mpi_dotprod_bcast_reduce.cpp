#include "mpi.h"
#include <iostream>
#include <iomanip>

using namespace std;
int main(int argc, char *argv[]) {

//---------- initializing the necessary variables -----------------------------------------
int n, rank, num_procs, *array = NULL, start, end, p_sum, sum_tot, arr_size;
double tic, toc, diff;

//--------- setting up the MPI environment ---------------------------------------
MPI_Init(&argc, &argv);
MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Status sts;
tic =  MPI_Wtime();

//--------- initializing the partial sum to be zero -------------------------------
p_sum = 0;

//--------- master process takes in data values form the user and broadcasts it to all othere processes---------------------
if(rank == 0) 
{
	cout<<"Enter the size of an array\n";
	cin>>arr_size;
}

MPI_Bcast(&arr_size, 1, MPI_INT, 0, MPI_COMM_WORLD);   //---------1
array = new int[arr_size];

if(rank == 0)
{
	cout<<"Enter the elements of an array\n";
	for(int i=0;i<arr_size;i++)
		cin>>array[i];
}

MPI_Bcast(&array[0], arr_size, MPI_INT, 0, MPI_COMM_WORLD);     //-------2

//---------- setting the start and end indices of the array for each process ------------------------- 
start = rank*arr_size/num_procs;
end = (rank+1)*arr_size/num_procs;

for(int i=start;i<end;i++)
	p_sum += array[i]*array[i];
cout<<"I am node 0 and my partial sum is : "<<p_sum<<endl;

//----------- calculating the sum total by master process by obtaining partial sums from each process ------------
MPI_Reduce(&p_sum, &sum_tot, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);     //------3

if(rank==0)
{
	cout<<"I am the master and the total sum is : "<<sum_tot<<endl;
}

//-------- printing the time taken for each process and deleting the arrays to free up memory ----------------------------
toc = MPI_Wtime();
diff = toc - tic; 
cout<<"I am node "<<rank<<", and my time of completion is : "<<diff<<"s"<<endl;
delete [] array;

//-------- close the MPI environment -----------------------
MPI_Finalize();
return 0;
}
