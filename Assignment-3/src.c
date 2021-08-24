#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "mpi.h"

int numtasks, myrank;
// THIS FUNCTION TAKES IN AN ARRAY OF SIZE ROW_COUNT*COLUMN_COUNT  AND THEN EXTRACTS THE 
// MIN FROM EACH COLUMN AND RETURNS A ARRAY CONSISTING OF MIN VALUE WITH RESPECT TO EACH COLUMN.
void min_of_each_col(float *arr,int row_count,int column_count,float *result)
{
    // printf("myrank=%d\n",myrank);
    for(int st=0;st<column_count;st++)
    {
      float minval = FLT_MAX;
      for(int r=0;r<row_count*column_count;r+=column_count)
      {
        if(arr[st+r]<minval)
          minval = arr[st+r];
      }

      result[st]=minval;
    }
    

    // return result;
}



int main(int argc, char *argv[]) 
{
  // initialize MPI

  char *fileName=argv[1];

  // printf("%s\n",fileName);

  double stime,dtime,time,maxtime;
  MPI_Init (&argc, &argv);

  //  get number of tasks
  MPI_Comm_size (MPI_COMM_WORLD, &numtasks);

  // get my rank
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  int no_of_rows=0;
  int no_of_columns =  0;
  FILE *fp;
  int m=0;
  ssize_t read;
  size_t len = 0;
  char * line = NULL;

  // GET NO OF rows and columns from THE CSV FILE   

  if(myrank==0)
  {
    char ch;


    fp=fopen(fileName,"r");

    while((ch=fgetc(fp))!=EOF) {
      if(ch=='\n')
      no_of_rows++;
    }
    fclose(fp);



    no_of_rows = no_of_rows-1;  
    fp=fopen(fileName,"r");

    if ((read = getline(&line, &len, fp)) != -1) 
    {

      char * token1 = strtok(line, ",");

      while( token1 != NULL ) 
      {
        no_of_columns=no_of_columns+1;

        token1 = strtok(NULL, ",");
      }
    }


    fclose(fp);
    
    no_of_columns= no_of_columns-2;


  }

  // 2-D ARRAY TO STORE THE CSV FILE VALUES
  float *arr =(float *)malloc(no_of_rows  * no_of_columns * sizeof(float ));
  if(myrank==0)
  {

    fp=fopen(fileName,"r");



    while ((read = getline(&line, &len, fp)) != -1) 
    {
      m=m+1;

      // SKIP THE HEADER OF THE TABLES 
      if(m==1)
      {
        continue;
      }
      //  printf("%s   %d\n",line,m);
      char * token = strtok(line, ",");
      int indx=0;
      int length=0;

      while( token != NULL ) 
      {
        length=length+1;
        if(length>2)
        {
          arr[(m-2)*no_of_columns + indx]=atof(token);
          indx++;
        } 
        token = strtok(NULL, ",");

      }

    }

    fclose(fp);
  }
  // if(myrank==0)
  // {
  //   printf("no_of_columns=%d\n",no_of_columns);
  //   printf("no_of_rows=%d\n",no_of_rows);
  //   printf("numtasks=%d\n",numtasks);
  // }

  float minival = FLT_MAX;
  float *result;
  stime=MPI_Wtime();

  // Checking total processes is more than one or not.
  if(numtasks>1)
  {
    // broadcasting the number of columns and rows to other processes
    MPI_Bcast(&no_of_columns,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&no_of_rows,1,MPI_INT,0,MPI_COMM_WORLD);
    result =(float *)malloc(no_of_columns  * sizeof(float ));


    // checking if no_of_rows are completely divisible by total process. If true all processes will get the equal data and can be done by mpi scatter()
    if(no_of_rows%numtasks==0)
    {
      int recvcount = (no_of_rows*no_of_columns)/numtasks;
      float *recvbuff =(float *)malloc(recvcount  * sizeof(float ));
      MPI_Scatter(arr,recvcount,MPI_FLOAT,recvbuff,recvcount,MPI_FLOAT,0,MPI_COMM_WORLD);
      // Finding the min in each column 
      min_of_each_col(recvbuff, recvcount/no_of_columns ,no_of_columns,result);

    }
    else
    {
      // data is not distributed to each process. Need to use scatterv()

      int extra_rows=no_of_rows%numtasks;
      int i=0;

      int *sendcounts=(int *)malloc(numtasks*sizeof(int));
      int *displs=(int *)malloc(numtasks*sizeof(int));

      int count=0;
      int temp=no_of_rows/numtasks;
      for(i=0;i< extra_rows;i++)
      {
        displs[i]=count;
        count+=(temp+1)*no_of_columns;
        sendcounts[i]=(temp+1)*no_of_columns;
      }

      for (;i<numtasks;i++)
      {
        displs[i]=count;
        count+=(temp)*no_of_columns;
        sendcounts[i]=(temp)*no_of_columns;

      }




      // need to scatter sendcounts array so every process will know how much data they will recieve from root process.

      int recvcount=0;

      MPI_Scatter(sendcounts,1,MPI_INT,&recvcount,1,MPI_INT,0,MPI_COMM_WORLD);

      // printf("recvcount=%d myrank=%d\n",recvcount,myrank);

      float *recvbuff =(float *)malloc(recvcount  * sizeof(float ));
      // // double recvbuff[recvcount];
      MPI_Scatterv(arr,sendcounts,displs,MPI_FLOAT,recvbuff,recvcount,MPI_FLOAT,0,MPI_COMM_WORLD);
      min_of_each_col(recvbuff, recvcount/no_of_columns ,no_of_columns,result);



    }

    // each process has their computed result in result array and we are doing MPI reduce to bring the overall column wise min to root 0.
    float *reduce_result=(float *)malloc(no_of_columns*sizeof(float ));
    MPI_Reduce(result, reduce_result,no_of_columns,MPI_FLOAT,MPI_MIN,0,MPI_COMM_WORLD);

    // need to write the result in output.txt and also needs to find the global minima of reduce_result array.
    if(myrank==0)
    {

      for (int i = 0; i < no_of_columns-1; ++i)
      {
        /* code */
   //     printf("%.2f,",reduce_result[i]);
      }
   //   printf("%.2f\n",reduce_result[no_of_columns-1]);




      // find the global minima of the data 

      for(int r=0;r<no_of_columns;r++)
      {
        if(reduce_result[r]<minival)
          minival = reduce_result[r];
      }

//       printf("%.2f\n",minival);
    }

  }
  else
  {
    result =(float *)malloc(no_of_columns  * sizeof(float ));

    min_of_each_col(arr, no_of_rows,no_of_columns,result);


    for (int i = 0; i < no_of_columns-1; ++i)
    {
  //    printf("%.2f,",result[i] );
    }
  //  printf("%.2f\n",result[no_of_columns-1] );


    // find global minima of data

    
      for(int r=0;r<no_of_columns;r++)
      {
        if(result[r]<minival)
          minival = result[r];
      }

   // printf("%.2f\n",minival);

  }
  dtime=MPI_Wtime();
  time=dtime-stime;
  MPI_Reduce(&time,&maxtime,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);

  if(!myrank)
  {
    // writing all the result in output.txt file
    fp=fopen("output.txt", "w");
    for(int i=0;i<no_of_columns-1;i++)
      fprintf(fp, "%.2f,", result[i]);
    fprintf(fp, "%.2f\n", result[no_of_columns-1]);
    fprintf(fp, "%.2f\n", minival);
    fprintf(fp, "%lf\n", maxtime);
    
    fclose(fp);
  }





  MPI_Finalize();
  return 0;

}
