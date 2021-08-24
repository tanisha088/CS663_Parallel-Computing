#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

// #include <math.h>

  int numtasks, myrank;
  int arrSize;
  int cores;
  int number_of_nodes_per_group;

  double* message;
  double* recvMessage_unoptimized;

  double* recvMessage_intra_Node;
  double* recvMessage_intra_switch;
  double* recvMessage_inter_switch;

  int *sendbytes;

  MPI_Group world_group;




void mpi_gather_default()
{
  for (int j = 0; j < arrSize; j++) {
    message[j] = myrank;
  }

  double average[5];

  for(int j=0;j<5;j++)
  {

    double stime,dtime,time,maxtime;
    stime=MPI_Wtime();
    MPI_Gather(message, arrSize, MPI_DOUBLE, recvMessage_unoptimized, arrSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    dtime=MPI_Wtime();
    time=dtime-stime;
    MPI_Reduce(&time,&maxtime,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    if (!myrank)
    {
        average[j]=maxtime;
    }
  }

  if(!myrank)
  {
    double total_average=0;
    for (int i = 0; i < 5; ++i)
    {
      total_average=+average[i];
    }
    printf("Gather_Default=%lf\n",total_average/5);
  }
}

void mpi_bcast_default()
{
  for (int j = 0; j < arrSize; j++) {
    message[j] = myrank;
  }

  double average[5];

  for(int j=0;j<5;j++)
  {

    double stime,dtime,time,maxtime;
    stime=MPI_Wtime();
    MPI_Bcast(message, arrSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    dtime=MPI_Wtime();
    time=dtime-stime;
    MPI_Reduce(&time,&maxtime,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    if (!myrank)
    {
        average[j]=maxtime;
    }
  }

  if(!myrank)
  {
    double total_average=0;
    for (int i = 0; i < 5; ++i)
    {
      total_average=+average[i];
    }
    printf("Bcast_Default=%lf\n",total_average/5);
  }
}

void mpi_reduce_default()
{

  for (int j = 0; j < arrSize; j++) 
  {
    message[j] = myrank;
  }

  double average[5];

  for(int j=0;j<5;j++)
  {

    double stime,dtime,time,maxtime;
    stime=MPI_Wtime();
    MPI_Reduce(message, recvMessage_unoptimized ,arrSize, MPI_DOUBLE , MPI_SUM, 0, MPI_COMM_WORLD); 

    dtime=MPI_Wtime();
    time=dtime-stime;
    MPI_Reduce(&time,&maxtime,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    if (!myrank)
    {
        average[j]=maxtime;
    }
  }

  if(!myrank)
  {
    double total_average=0;
    for (int i = 0; i < 5; ++i)
    {
      total_average=+average[i];
    }
    printf("Reduce_Default=%lf\n",total_average/5);
  }
}


void mpi_alltoallv_default()
{
  double *sendmessage;
  // int *sendbytes;
  int *sdispls;
  double *recievemessage;
  int *rdispls;
  int *recievebytes;
  time_t t;
  srand((unsigned) time(&t));
  double average[5];
  sendbytes=(int *)malloc(numtasks * sizeof(int ));

  for(int i=0;i<numtasks;i++)
  {
    if(i==myrank)
      sendbytes[i]=0;
    else
      sendbytes[i]=rand() % arrSize;
  }

  for (int k=0;k<5;k++)
  {
    double stime,dtime,time,maxtime;
    stime=MPI_Wtime();
    sdispls=(int *)malloc(numtasks * sizeof(int ));
    int total_send_bytes=0;
    for (int i = 0; i < numtasks; ++i)
    {
      sdispls[i]=total_send_bytes;
      total_send_bytes+=sendbytes[i];
    }


    sendmessage=(double *)malloc(total_send_bytes * sizeof(double ));

    for (int i = 0; i < total_send_bytes; ++i)
    {
      sendmessage[i]=myrank;
    }

    recievebytes=(int *)malloc(numtasks * sizeof(int ));
    rdispls=(int *)malloc(numtasks * sizeof(int ));
    MPI_Alltoall(sendbytes,1,MPI_INT,recievebytes,1,MPI_INT,MPI_COMM_WORLD);

    int total_recieve_bytes=0;
    for (int i = 0; i < numtasks; ++i)
    {
      /* code */
      rdispls[i]=total_recieve_bytes;
      total_recieve_bytes+=recievebytes[i];

    }
    recievemessage =(double *)malloc(total_recieve_bytes * sizeof(double ));

    MPI_Alltoallv(sendmessage,sendbytes,sdispls,MPI_DOUBLE,recievemessage,recievebytes,rdispls,MPI_DOUBLE,MPI_COMM_WORLD);

    dtime=MPI_Wtime();
    time=dtime-stime;
    MPI_Reduce(&time,&maxtime,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    if (!myrank)
    {
      average[k]=maxtime;
    }
  }
  if(!myrank)
  {
     // for (int i = 0; i < arrSize; ++i)
     // {
     //   printf("%lf\n", recvMessage_inter_switch[i]);
     // }

    double total_average=0;
    for (int j = 0; j < 5; ++j)
    {
      total_average=+average[j];
    }
    printf("Alltoallv_Default=%lf\n",total_average/5);
  }

}

void mpi_alltoallv_optimized()
{
  time_t t;
  srand((unsigned) time(&t));

    /* code */
    double *sendmessage;
    // int *sendbytes;
    int *sdispls;
    double average[5];
    double stime,dtime,time,maxtime;

    for (int k=0;k<5;k++)
    {
      stime=MPI_Wtime();
      if(cores==1)
      {
        if(number_of_nodes_per_group>=numtasks)
        {
          double *recievemessage;
          int *rdispls;
          int *recievebytes;
          stime=MPI_Wtime();
          sdispls=(int *)malloc(numtasks * sizeof(int ));
          int total_send_bytes=0;
          for (int i = 0; i < numtasks; ++i)
          {
            sdispls[i]=total_send_bytes;
            total_send_bytes+=sendbytes[i];
          }


          sendmessage=(double *)malloc(total_send_bytes * sizeof(double ));

          for (int i = 0; i < total_send_bytes; ++i)
          {
            sendmessage[i]=myrank;
          }

          recievebytes=(int *)malloc(numtasks * sizeof(int ));
          rdispls=(int *)malloc(numtasks * sizeof(int ));
          MPI_Alltoall(sendbytes,1,MPI_INT,recievebytes,1,MPI_INT,MPI_COMM_WORLD);

          int total_recieve_bytes=0;
          for (int i = 0; i < numtasks; ++i)
          {
            /* code */
            rdispls[i]=total_recieve_bytes;
            total_recieve_bytes+=recievebytes[i];

          }
          recievemessage =(double *)malloc(total_recieve_bytes * sizeof(double ));

          MPI_Alltoallv(sendmessage,sendbytes,sdispls,MPI_DOUBLE,recievemessage,recievebytes,rdispls,MPI_DOUBLE,MPI_COMM_WORLD);

          dtime=MPI_Wtime();
          time=dtime-stime;
          MPI_Reduce(&time,&maxtime,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
          if (!myrank)
          {
            average[k]=maxtime;
          }
        }

        else
        {
          // intra switch alltoallv

          sdispls=(int *)malloc(numtasks * sizeof(int ));
          int total_send_bytes=0;
          for (int i = 0; i < numtasks; ++i)
          {

            sdispls[i]=total_send_bytes;
            total_send_bytes+=sendbytes[i];
          }


          sendmessage=(double *)malloc(total_send_bytes * sizeof(double ));

          for (int i = 0; i < total_send_bytes; ++i)
          {
            sendmessage[i]=myrank;
          }


          int **ranks;
          int index_p=numtasks/number_of_nodes_per_group;
          ranks=(int **)malloc(index_p * sizeof(int *));
          ranks[0] = (int *)malloc(index_p*number_of_nodes_per_group * sizeof(int));
          for(int j = 1; j < index_p; j++) {
            ranks[j] = ranks[j-1] + number_of_nodes_per_group;
          }

          int *interrank=(int *)malloc(index_p * sizeof(int ));


          MPI_Group grp[index_p];
          MPI_Comm prime_comm[index_p];

          int i=myrank/number_of_nodes_per_group;
          for(int j=0;j<number_of_nodes_per_group;j++)
            ranks[i][j]=i*number_of_nodes_per_group + j;
          
          MPI_Group_incl(world_group,number_of_nodes_per_group, ranks[i], &grp[i]);
          MPI_Comm_create_group(MPI_COMM_WORLD, grp[i],i, &prime_comm[i]);
          // printf("k=%d myrank=%d\n",k,myrank);

          double *intra_switch_recv;
          int *intra_switch_recvbytes;
          int *intra_switch_rdispls;

          intra_switch_recvbytes=(int *)malloc(number_of_nodes_per_group * sizeof(int ));
          intra_switch_rdispls=(int *)malloc(number_of_nodes_per_group * sizeof(int ));


          MPI_Alltoall(sendbytes+i*number_of_nodes_per_group,1,MPI_INT,intra_switch_recvbytes,1,MPI_INT,prime_comm[i]);

          int total_intra_recieve_bytes=0;
          for (int j = 0; j < number_of_nodes_per_group; ++j)
          {
            /* code */
            intra_switch_rdispls[j]=total_intra_recieve_bytes;
            total_intra_recieve_bytes+=intra_switch_recvbytes[j];

          }

          intra_switch_recv =(double *)malloc(total_intra_recieve_bytes * sizeof(double ));

          MPI_Alltoallv(sendmessage+i*number_of_nodes_per_group,sendbytes+i*number_of_nodes_per_group,sdispls+i*number_of_nodes_per_group,MPI_DOUBLE,intra_switch_recv,intra_switch_recvbytes,intra_switch_rdispls,MPI_DOUBLE,prime_comm[i]);

          int remaining_tasks=numtasks - number_of_nodes_per_group;

          int *remaining_sendbytes;

          remaining_sendbytes=(int *)malloc(remaining_tasks * sizeof(int ));

          for (int j = 0,x=0; j < numtasks;)
          {
            if(j==i*number_of_nodes_per_group)
            {
              for (int t = 0; t < number_of_nodes_per_group; ++t)
              {
                j++;
              }
            }
            else
            {
              remaining_sendbytes[x]=sendbytes[j];
              x++;
              j++;

            }
          }

          int *remaining_sendbytes_to_leader;
          remaining_sendbytes_to_leader=(int *)malloc(remaining_tasks*number_of_nodes_per_group * sizeof(int ));

          MPI_Allgather(remaining_sendbytes,remaining_tasks,MPI_INT,remaining_sendbytes_to_leader,remaining_tasks,MPI_INT,prime_comm[i]);

          double *remaining_sendmessage;

          int remaining_sendmessage_size=0;
          for (int j = 0; j < remaining_tasks; ++j)
          {
            remaining_sendmessage_size+= remaining_sendbytes_to_leader[(myrank%number_of_nodes_per_group)*remaining_tasks+j];
          }

          remaining_sendmessage =(double *)malloc(remaining_sendmessage_size * sizeof(double ));

          for (int j = 0,x=0; j < numtasks;)
          {
            if(j==i*number_of_nodes_per_group)
            {
              for (int t = 0; t < number_of_nodes_per_group; ++t)
              {
                j++;
              }
            }
            else
            {
              for (int t = 0; t < sendbytes[j]; ++t)
              {
                /* code */
                remaining_sendmessage[x]=sendmessage[t];
                x++;
              }
              j++;
            }
          }


          int *leader_recvcount=(int *)malloc(number_of_nodes_per_group * sizeof(int ));
          int *leader_displs=(int *)malloc(number_of_nodes_per_group * sizeof(int ));

          int total_leader_recvbytes=0;
          for (int j = 0; j < number_of_nodes_per_group; ++j)
          {
            int temp_size=0;
            for (int t = 0; t < remaining_tasks ; ++t)
            {
              temp_size+=remaining_sendbytes_to_leader[j*remaining_tasks+t];
            }
            leader_recvcount[j]=temp_size;
            leader_displs[j]=total_leader_recvbytes;
            total_leader_recvbytes+=temp_size;

          }


          double *leader_recvmessage=(double *)malloc(total_leader_recvbytes * sizeof(double ));
          MPI_Gatherv(remaining_sendmessage,remaining_sendmessage_size,MPI_DOUBLE,leader_recvmessage,leader_recvcount,leader_displs,MPI_DOUBLE,0,prime_comm[i]);

          int *scatter_recvbytes=(int*)malloc(numtasks*sizeof(int));
          int *scatter_sendbytes = (int*)malloc(number_of_nodes_per_group*numtasks*sizeof(int));
          int *scatter_displs = (int*)malloc(number_of_nodes_per_group*sizeof(int));
          int *scatter_sendcount = (int*)malloc(number_of_nodes_per_group*sizeof(int));
          double *scatter_sendmessage;

          if(myrank%number_of_nodes_per_group==0)
          {

            MPI_Group intergrp;
            MPI_Comm intercomm;
            for (int j = 0; j < numtasks/number_of_nodes_per_group; ++j)
              interrank[j]=j*number_of_nodes_per_group;

             MPI_Group_incl(world_group,numtasks/number_of_nodes_per_group,interrank,&intergrp);
             MPI_Comm_create_group(MPI_COMM_WORLD,intergrp,0, &intercomm);

            double *leader_sendmessage=(double *)malloc(total_leader_recvbytes * sizeof(double ));


            int* result =  (int *)malloc(number_of_nodes_per_group*number_of_nodes_per_group*((numtasks/number_of_nodes_per_group)-1) * sizeof(int ));

                int in=0;
               for(int j=0;j<(numtasks-number_of_nodes_per_group);j+=number_of_nodes_per_group)
               {
                for(int p=0;p<(numtasks-number_of_nodes_per_group)*number_of_nodes_per_group;p=p+(((numtasks/number_of_nodes_per_group)-2)*number_of_nodes_per_group))
                {
                  for(int m=0;m<number_of_nodes_per_group;m++)
                  {
                    result[in] =  remaining_sendbytes_to_leader[j+p];
                    in++;
                    p++;
                  }
                }
               }
               int *leader_sendbytes=malloc(numtasks*number_of_nodes_per_group * sizeof(int ));
               int s=0;
               for (int j = 0; j < index_p; ++j)
               {
                int flag=0;
                for(int p=0;p<number_of_nodes_per_group*number_of_nodes_per_group;p++)
                {
                   if(j==i)
                   {
                    leader_sendbytes[j*number_of_nodes_per_group*number_of_nodes_per_group+p]=0;
                   }
                   else
                   {
                    leader_sendbytes[j*number_of_nodes_per_group*number_of_nodes_per_group+p]=result[p+s*number_of_nodes_per_group*number_of_nodes_per_group];
                    flag=1;

                   }
                }
                if(flag==1)
                {
                  flag=0;
                  s++;
                }

               }
             
               int *leader_recvbytes=malloc(numtasks*number_of_nodes_per_group * sizeof(int ));

               MPI_Alltoall(leader_sendbytes,number_of_nodes_per_group*number_of_nodes_per_group,MPI_INT,leader_recvbytes,number_of_nodes_per_group*number_of_nodes_per_group,MPI_INT,intercomm);
               

               int *inter_leader_recvcount=(int *)malloc(index_p * sizeof(int ));
               int *inter_leader_displs=(int *)malloc(index_p * sizeof(int ));
               
               int total_inter_leader_sendbytes=0;
               for (int j = 0; j < index_p; ++j)
               {
                inter_leader_displs[j]=total_inter_leader_sendbytes;
                int size=0;
                 for (int p = 0; p < number_of_nodes_per_group*number_of_nodes_per_group; ++p)
                 {
                   total_inter_leader_sendbytes+=leader_sendbytes[p+j*number_of_nodes_per_group*number_of_nodes_per_group];
                   size+=leader_sendbytes[p+j*number_of_nodes_per_group*number_of_nodes_per_group];
                 }
                 inter_leader_recvcount[j]=size;
               }


               int *inter_leader_recvcount1=(int *)malloc(index_p * sizeof(int ));
               int *inter_leader_displs1=(int *)malloc(index_p * sizeof(int ));
               
               int total_inter_leader_recvbytes=0;
               for (int j = 0; j < index_p; ++j)
               {
                inter_leader_displs1[j]=total_inter_leader_recvbytes;
                int size=0;
                 for (int p = 0; p < number_of_nodes_per_group*number_of_nodes_per_group; ++p)
                 {
                   total_inter_leader_recvbytes+=leader_recvbytes[p+j*number_of_nodes_per_group*number_of_nodes_per_group];
                   size+=leader_recvbytes[p+j*number_of_nodes_per_group*number_of_nodes_per_group];
                 }
                 inter_leader_recvcount1[j]=size;
               }


            int *count=(int *)malloc((index_p-1)*number_of_nodes_per_group * sizeof(int ));

            for (int j = 0; j < (index_p-1)*number_of_nodes_per_group; ++j)
            {
              int temp_size=0;
              for (int t = 0; t < number_of_nodes_per_group; ++t)
              {
                temp_size+=remaining_sendbytes_to_leader[t+j*number_of_nodes_per_group];
              }
              count[j]=temp_size;
            }

            int temp_count1=0;
            for (int j = 0; j < index_p-1; ++j)
            {
              int temp_size=0;
              int temp_count2=0;
              int flag=1;
              for (int t = j; t < (index_p-1)*number_of_nodes_per_group; t=t+index_p-1)
              {
                if(t>0 && flag)
                {
                  for (int p = 0; p < j; ++p)
                  {
                    temp_size+=count[p];
                  }
                  flag=0;
                }

                for (int x = 0; x < count[t]; ++x)
                {
                  leader_sendmessage[x+temp_count1]=leader_recvmessage[temp_count2+temp_size+x];
                }
                temp_count1+=count[t];
                temp_count2+=count[t];
                for (int p = 0; p < index_p-2 && p< (index_p-1)*number_of_nodes_per_group-t-1; ++p)
                {
                  temp_size+=count[t+p+1];
                  /* code */
                }
              }
            }

            double *inter_leader_recvmessage=(double *)malloc(total_inter_leader_recvbytes * sizeof(double ));






            MPI_Alltoallv(leader_sendmessage,inter_leader_recvcount,inter_leader_displs,MPI_DOUBLE,inter_leader_recvmessage,inter_leader_recvcount1,inter_leader_displs1,MPI_DOUBLE,intercomm);



            int ind1=0;
            int error_check=0;
            for(int j=0;j<(number_of_nodes_per_group);j+=1)
            {
              for(int p=0;p<(numtasks)*number_of_nodes_per_group;p+=number_of_nodes_per_group)
              {
                scatter_sendbytes[ind1] = leader_recvbytes[j+p];
                ind1++;
              }
            }
            ind1=0;
            for(int j=0;j<number_of_nodes_per_group;j++)
            {
              int temp_size=0;
              scatter_displs[j]=ind1;
              for(int p=0;p<numtasks;p++)
              {
                temp_size+=scatter_sendbytes[j*numtasks+p];
              }
              scatter_sendcount[j]=temp_size;
              ind1+=temp_size;
            }


            scatter_sendmessage =(double *)malloc(ind1 * sizeof(double ));

            int *count1=(int*)malloc(numtasks*number_of_nodes_per_group*sizeof(int));
            count1[0]=scatter_sendbytes[0];
            for(int j=1;j<numtasks*number_of_nodes_per_group;j++)
            {
              count1[j]=count1[j-1]+leader_recvbytes[j];
            }

            temp_count1=0;
            for(int j=0;j<number_of_nodes_per_group;j++)
            {
              int temp_size=0;
              int temp_count2=0;
              int flag=1;
              for (int p = j; p < numtasks*number_of_nodes_per_group; p=p+number_of_nodes_per_group)
              {
                if(j>0 && flag)
                {
                  temp_size+=count1[p-1];
                  flag=0;
                }


                for(int x=0;x<leader_recvbytes[p];x++)
                {

                  scatter_sendmessage[x+temp_count1]=inter_leader_recvmessage[x+temp_size+temp_count2];
                }
                for(int x=0;x<number_of_nodes_per_group-1 && p<(numtasks-1)*number_of_nodes_per_group;x++ )
                  temp_count2+= leader_recvbytes[x+p+1];
                temp_count1+=leader_recvbytes[p];
                temp_size+=leader_recvbytes[p];
              }
            }


          }


          MPI_Scatter(scatter_sendbytes,numtasks,MPI_INT,scatter_recvbytes,numtasks,MPI_INT,0,prime_comm[i] );


          int scatter_recvcount=0;
          for(int j=0;j<numtasks;j++)
            scatter_recvcount+=scatter_recvbytes[j];


          double *scatter_recvmessage=(double *)malloc(scatter_recvcount * sizeof(double ));

          MPI_Scatterv(scatter_sendmessage,scatter_sendcount,scatter_displs,MPI_DOUBLE,scatter_recvmessage,scatter_recvcount,MPI_DOUBLE,0,prime_comm[i]);

          int total_individual_recvbytes=total_intra_recieve_bytes +scatter_recvcount;

          double *individual_recvmessage=(double *)malloc( total_individual_recvbytes * sizeof(double ));
          int temp_count1=0;
          int temp_count2=0;
          for(int j=0;j<index_p;j++)
          {
            int temp_size=0;
            for(int p=0;p<number_of_nodes_per_group;p++)
              temp_size+=scatter_recvbytes[j*number_of_nodes_per_group+p];
            if(temp_size==0)
            {
              for (int p=0;p<total_intra_recieve_bytes;p++)
                individual_recvmessage[p+temp_count1]= intra_switch_recv[p];
              temp_count1+=total_intra_recieve_bytes;
            }
            else
            {
              for (int p=0;p<temp_size;p++)
                individual_recvmessage[p+temp_count1]= scatter_recvmessage[p+temp_count2];
              temp_count1+=temp_size;
              temp_count2+=temp_size;

            }
          }


        }



      }
      else
      {
        // intra Node alltoallv

        sdispls=(int *)malloc(numtasks * sizeof(int ));
        int total_send_bytes=0;
        for (int i = 0; i < numtasks; ++i)
        {
          sdispls[i]=total_send_bytes;
          total_send_bytes+=sendbytes[i];
        }


        sendmessage=(double *)malloc(total_send_bytes * sizeof(double ));

        for (int i = 0; i < total_send_bytes; ++i)
        {
          sendmessage[i]=myrank;
        }


        int **ranks;
        int index_p=numtasks/cores;
        ranks=(int **)malloc(index_p * sizeof(int *));
        ranks[0] = (int *)malloc(index_p*cores * sizeof(int));
        for(int j = 1; j < index_p; j++) {
          ranks[j] = ranks[j-1] + cores;
        }

        int *interrank=(int *)malloc(index_p * sizeof(int ));


        MPI_Group grp[index_p];
        MPI_Comm prime_comm[index_p];

        int i=myrank/cores;
        for(int j=0;j<cores;j++)
          ranks[i][j]=i*cores + j;
        
        MPI_Group_incl(world_group,cores, ranks[i], &grp[i]);
        MPI_Comm_create_group(MPI_COMM_WORLD, grp[i],i, &prime_comm[i]);
        // printf("k=%d myrank=%d\n",k,myrank);

        double *intra_switch_recv;
        int *intra_switch_recvbytes;
        int *intra_switch_rdispls;

        intra_switch_recvbytes=(int *)malloc(cores * sizeof(int ));
        intra_switch_rdispls=(int *)malloc(cores * sizeof(int ));


        MPI_Alltoall(sendbytes+i*cores,1,MPI_INT,intra_switch_recvbytes,1,MPI_INT,prime_comm[i]);

        int total_intra_recieve_bytes=0;
        for (int j = 0; j < cores; ++j)
        {
          /* code */
          intra_switch_rdispls[j]=total_intra_recieve_bytes;
          total_intra_recieve_bytes+=intra_switch_recvbytes[j];

        }

        intra_switch_recv =(double *)malloc(total_intra_recieve_bytes * sizeof(double ));

        MPI_Alltoallv(sendmessage+i*cores,sendbytes+i*cores,sdispls+i*cores,MPI_DOUBLE,intra_switch_recv,intra_switch_recvbytes,intra_switch_rdispls,MPI_DOUBLE,prime_comm[i]);

        int remaining_tasks=numtasks - cores;

        int *remaining_sendbytes;

        remaining_sendbytes=(int *)malloc(remaining_tasks * sizeof(int ));

        for (int j = 0,x=0; j < numtasks;)
        {
          if(j==i*cores)
          {
            for (int t = 0; t < cores; ++t)
            {
              j++;
            }
          }
          else
          {
            remaining_sendbytes[x]=sendbytes[j];
            x++;
            j++;

          }
        }

        int *remaining_sendbytes_to_leader;
        remaining_sendbytes_to_leader=(int *)malloc(remaining_tasks*cores * sizeof(int ));

        MPI_Allgather(remaining_sendbytes,remaining_tasks,MPI_INT,remaining_sendbytes_to_leader,remaining_tasks,MPI_INT,prime_comm[i]);

        double *remaining_sendmessage;

        int remaining_sendmessage_size=0;
        for (int j = 0; j < remaining_tasks; ++j)
        {
          remaining_sendmessage_size+= remaining_sendbytes_to_leader[(myrank%cores)*remaining_tasks+j];
          // if(myrank==0)
          //   printf("%d\n", remaining_sendmessage_size);
        }
        remaining_sendmessage =(double *)malloc(remaining_sendmessage_size * sizeof(double ));

        for (int j = 0,x=0; j < numtasks;)
        {
          if(j==i*cores)
          {
            for (int t = 0; t < cores; ++t)
            {
              j++;
            }
          }
          else
          {

            for (int t = 0; t < sendbytes[j]; ++t)
            {
              /* code */
              remaining_sendmessage[x]=sendmessage[t];
              x++;
            }
            j++;
          }
        }


        int *leader_recvcount=(int *)malloc(cores * sizeof(int ));
        int *leader_displs=(int *)malloc(cores * sizeof(int ));

        int total_leader_recvbytes=0;
        for (int j = 0; j < cores; ++j)
        {
          int temp_size=0;
          for (int t = 0; t < remaining_tasks ; ++t)
          {
            temp_size+=remaining_sendbytes_to_leader[j*remaining_tasks+t];
          }
          leader_recvcount[j]=temp_size;
          leader_displs[j]=total_leader_recvbytes;
          total_leader_recvbytes+=temp_size;

        }


        double *leader_recvmessage=(double *)malloc(total_leader_recvbytes * sizeof(double ));
        MPI_Gatherv(remaining_sendmessage,remaining_sendmessage_size,MPI_DOUBLE,leader_recvmessage,leader_recvcount,leader_displs,MPI_DOUBLE,0,prime_comm[i]);

        int *scatter_recvbytes=(int*)malloc(numtasks*sizeof(int));
        int *scatter_sendbytes = (int*)malloc(cores*numtasks*sizeof(int));
        int *scatter_displs = (int*)malloc(cores*sizeof(int));
        int *scatter_sendcount = (int*)malloc(cores*sizeof(int));
        double *scatter_sendmessage;

        if(myrank%cores==0)
        {

          MPI_Group intergrp;
          MPI_Comm intercomm;
          for (int j = 0; j < numtasks/cores; ++j)
            interrank[j]=j*cores;

           MPI_Group_incl(world_group,numtasks/cores,interrank,&intergrp);
           MPI_Comm_create_group(MPI_COMM_WORLD,intergrp,0, &intercomm);

          double *leader_sendmessage=(double *)malloc(total_leader_recvbytes * sizeof(double ));


          int* result =  (int *)malloc(cores*cores*((numtasks/cores)-1) * sizeof(int ));

              int in=0;
             for(int j=0;j<(numtasks-cores);j+=cores)
             {
              for(int p=0;p<(numtasks-cores)*cores;p=p+(((numtasks/cores)-2)*cores))
              {
                for(int m=0;m<cores;m++)
                {
                  result[in] =  remaining_sendbytes_to_leader[j+p];
                  in++;
                  p++;
                }
              }
             }
             int *leader_sendbytes=malloc(numtasks*cores * sizeof(int ));
             int s=0;
             for (int j = 0; j < index_p; ++j)
             {
              int flag=0;
              for(int p=0;p<cores*cores;p++)
              {
                 if(j==i)
                 {
                  leader_sendbytes[j*cores*cores+p]=0;
                 }
                 else
                 {
                  leader_sendbytes[j*cores*cores+p]=result[p+s*cores*cores];
                  flag=1;

                 }
              }
              if(flag==1)
              {
                flag=0;
                s++;
              }

             }

           
             int *leader_recvbytes=malloc(numtasks*cores * sizeof(int ));

             MPI_Alltoall(leader_sendbytes,cores*cores,MPI_INT,leader_recvbytes,cores*cores,MPI_INT,intercomm);
             

             int *inter_leader_recvcount=(int *)malloc(index_p * sizeof(int ));
             int *inter_leader_displs=(int *)malloc(index_p * sizeof(int ));
             
             int total_inter_leader_sendbytes=0;
             for (int j = 0; j < index_p; ++j)
             {
              inter_leader_displs[j]=total_inter_leader_sendbytes;
              int size=0;
               for (int p = 0; p < cores*cores; ++p)
               {
                 total_inter_leader_sendbytes+=leader_sendbytes[p+j*cores*cores];
                 size+=leader_sendbytes[p+j*cores*cores];
               }
               inter_leader_recvcount[j]=size;
             }


             int *inter_leader_recvcount1=(int *)malloc(index_p * sizeof(int ));
             int *inter_leader_displs1=(int *)malloc(index_p * sizeof(int ));
             
             int total_inter_leader_recvbytes=0;
             for (int j = 0; j < index_p; ++j)
             {
              inter_leader_displs1[j]=total_inter_leader_recvbytes;
              int size=0;
               for (int p = 0; p < cores*cores; ++p)
               {
                 total_inter_leader_recvbytes+=leader_recvbytes[p+j*cores*cores];
                 size+=leader_recvbytes[p+j*cores*cores];
               }
               inter_leader_recvcount1[j]=size;
             }


          int *count=(int *)malloc((index_p-1)*cores * sizeof(int ));

          for (int j = 0; j < (index_p-1)*cores; ++j)
          {
            int temp_size=0;
            for (int t = 0; t < cores; ++t)
            {
              temp_size+=remaining_sendbytes_to_leader[t+j*cores];
            }
            count[j]=temp_size;
          }

          int temp_count1=0;
          for (int j = 0; j < index_p-1; ++j)
          {
            int temp_size=0;
            int temp_count2=0;
            int flag=1;
            for (int t = j; t < (index_p-1)*cores; t=t+index_p-1)
            {
              if(t>0 && flag)
              {
                for (int p = 0; p < j; ++p)
                {
                  temp_size+=count[p];
                }
                flag=0;
              }

              for (int x = 0; x < count[t]; ++x)
              {
                
                leader_sendmessage[x+temp_count1]=leader_recvmessage[temp_count2+temp_size+x];
              }
              
              temp_count1+=count[t];
              temp_count2+=count[t];
              for (int p = 0; p < index_p-2 && p<(index_p-1)*cores-t-1; ++p)
              {
                temp_size+=count[t+p+1];
                /* code */
              }
            }
          }

          double *inter_leader_recvmessage=(double *)malloc(total_inter_leader_recvbytes * sizeof(double ));

          MPI_Alltoallv(leader_sendmessage,inter_leader_recvcount,inter_leader_displs,MPI_DOUBLE,inter_leader_recvmessage,inter_leader_recvcount1,inter_leader_displs1,MPI_DOUBLE,intercomm);



          int ind1=0;
          int error_check=0;
          for(int j=0;j<(cores);j+=1)
          {
            for(int p=0;p<(numtasks)*cores;p+=cores)
            {
              scatter_sendbytes[ind1] = leader_recvbytes[j+p];
              ind1++;
            }
          }
          ind1=0;
          for(int j=0;j<cores;j++)
          {
            int temp_size=0;
            scatter_displs[j]=ind1;
            for(int p=0;p<numtasks;p++)
            {
              temp_size+=scatter_sendbytes[j*numtasks+p];
            }
            scatter_sendcount[j]=temp_size;
            ind1+=temp_size;
          }

          scatter_sendmessage =(double *)malloc(ind1 * sizeof(double ));

          int *count1=(int*)malloc(numtasks*cores*sizeof(int));
          count1[0]=scatter_sendbytes[0];
          for(int j=1;j<numtasks*cores;j++)
          {
            count1[j]=count1[j-1]+leader_recvbytes[j];
          }

          temp_count1=0;
          for(int j=0;j<cores;j++)
          {
            int temp_size=0;
            int temp_count2=0;
            int flag=1;
            for (int p = j; p < numtasks*cores; p=p+cores)
            {
              if(j>0 && flag)
              {
                temp_size+=count1[p-1];
                flag=0;
              }


              for(int x=0;x<leader_recvbytes[p];x++)
              {

                scatter_sendmessage[x+temp_count1]=inter_leader_recvmessage[x+temp_size+temp_count2];
              }
              for(int x=0;x<cores-1 && p<(numtasks-1)*cores;x++ )
                temp_count2+= leader_recvbytes[x+p+1];
              temp_count1+=leader_recvbytes[p];
              temp_size+=leader_recvbytes[p];
            }
          }


        }


        MPI_Scatter(scatter_sendbytes,numtasks,MPI_INT,scatter_recvbytes,numtasks,MPI_INT,0,prime_comm[i] );


        int scatter_recvcount=0;
        for(int j=0;j<numtasks;j++)
          scatter_recvcount+=scatter_recvbytes[j];


        double *scatter_recvmessage=(double *)malloc(scatter_recvcount * sizeof(double ));

        MPI_Scatterv(scatter_sendmessage,scatter_sendcount,scatter_displs,MPI_DOUBLE,scatter_recvmessage,scatter_recvcount,MPI_DOUBLE,0,prime_comm[i]);

        int total_individual_recvbytes=total_intra_recieve_bytes +scatter_recvcount;

        double *individual_recvmessage=(double *)malloc( total_individual_recvbytes * sizeof(double ));
        int temp_count1=0;
        int temp_count2=0;
        for(int j=0;j<index_p;j++)
        {
          int temp_size=0;
          for(int p=0;p<cores;p++)
            temp_size+=scatter_recvbytes[j*cores+p];
          if(temp_size==0)
          {
            for (int p=0;p<total_intra_recieve_bytes;p++)
              individual_recvmessage[p+temp_count1]= intra_switch_recv[p];
            temp_count1+=total_intra_recieve_bytes;
          }
          else
          {
            for (int p=0;p<temp_size;p++)
              individual_recvmessage[p+temp_count1]= scatter_recvmessage[p+temp_count2];
            temp_count1+=temp_size;
            temp_count2+=temp_size;

          }
        }


      }

      dtime=MPI_Wtime();
      time=dtime-stime;
      MPI_Reduce(&time,&maxtime,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
      if (!myrank)
      {
        average[k]=maxtime;
      }
    
    }

    if(!myrank)
    {

      double total_average=0;
      for (int j = 0; j < 5; ++j)
      {
        total_average=+average[j];
      }
      printf("Alltoallv_Optimized=%lf\n",total_average/5);
    }


}


void mpi_reduce_optimized()
{
 
  for (int j = 0; j < arrSize; j++) {
    message[j] = myrank;
  }
  double average[5];
  for (int k = 0; k < 5; ++k)
  {
    /* code */
    
    double stime,dtime,time,maxtime;
    stime=MPI_Wtime();


    if(cores==1)
    {
      if (number_of_nodes_per_group>=numtasks)
      {
            MPI_Reduce(message, recvMessage_inter_switch ,arrSize, MPI_DOUBLE , MPI_SUM, 0, MPI_COMM_WORLD); 
      }
      else
      {
        // intra switch gather
        int **ranks;
        int index_p=numtasks/number_of_nodes_per_group;
        ranks=(int **)malloc(index_p * sizeof(int *));
        ranks[0] = (int *)malloc(index_p*number_of_nodes_per_group * sizeof(int));
        for(int j = 1; j < index_p; j++) {
          ranks[j] = ranks[j-1] + number_of_nodes_per_group;
        }

        int *interrank=(int *)malloc(index_p * sizeof(int ));


        MPI_Group grp[index_p];
        MPI_Comm prime_comm[index_p];

        int i=myrank/number_of_nodes_per_group;
        for(int j=0;j<number_of_nodes_per_group;j++)
          ranks[i][j]=i*number_of_nodes_per_group + j;
        
        MPI_Group_incl(world_group,number_of_nodes_per_group, ranks[i], &grp[i]);
        MPI_Comm_create_group(MPI_COMM_WORLD, grp[i],i, &prime_comm[i]);

        MPI_Reduce(message, recvMessage_intra_switch ,arrSize, MPI_DOUBLE , MPI_SUM, 0, prime_comm[i]); 


        // inter switch gather
        if(myrank%number_of_nodes_per_group==0)
        {
          MPI_Group intergrp;
          MPI_Comm intercomm;
          for (int j = 0; j < numtasks/number_of_nodes_per_group; ++j)
            interrank[j]=j*number_of_nodes_per_group;

          MPI_Group_incl(world_group,numtasks/number_of_nodes_per_group,interrank,&intergrp);
          MPI_Comm_create_group(MPI_COMM_WORLD,intergrp,0, &intercomm);
          // printf("myrank=%d\n",myrank);

          MPI_Reduce(recvMessage_intra_switch, recvMessage_inter_switch ,arrSize, MPI_DOUBLE , MPI_SUM, 0, intercomm); 
         }
        
        // if(!myrank)
        // {
        //   for (int j = 0; j < arrSize*numtasks; ++j)
        //   {
        //     printf("%lf\n", recvMessage_inter_switch[j]);
        //   }
        // }

      }      
    }

    else
    {
      // intra Node gather
      int **ranks;
      int index_p=numtasks/cores;
      ranks=(int **)malloc(index_p * sizeof(int *));
      ranks[0] = (int *)malloc(index_p*cores * sizeof(int));
      for(int j = 1; j < index_p; j++) {
        ranks[j] = ranks[j-1] + cores;
      }

      MPI_Group grp[index_p];
      MPI_Comm prime_comm[index_p];

      int i=myrank/cores;
      for(int j=0;j<cores;j++)
        ranks[i][j]=i*cores + j;

      // if(myrank%cores==0)
      // {
        // for (int j = 0; j < cores; ++j)
        // {
        //   /* code */
        //   printf("%d myrank=%d\n", ranks[i][j],myrank);
        // }
      // }
      
      MPI_Group_incl(world_group,cores, ranks[i], &grp[i]);
      MPI_Comm_create_group(MPI_COMM_WORLD, grp[i],i, &prime_comm[i]);
      MPI_Reduce(message, recvMessage_intra_Node ,arrSize, MPI_DOUBLE , MPI_SUM, 0, prime_comm[i]); 


      if(myrank%cores==0)
      {

        index_p=index_p/number_of_nodes_per_group;

        if(number_of_nodes_per_group>=index_p)
        {

          MPI_Group intragrp;
          MPI_Comm intracomm;
          int *intrarank=(int *)malloc(numtasks/cores * sizeof(int ));

          for (int j = 0; j < numtasks/cores; ++j)
            intrarank[j]=j*cores;

           MPI_Group_incl(world_group,numtasks/cores,intrarank,&intragrp);
           MPI_Comm_create_group(MPI_COMM_WORLD,intragrp,0, &intracomm);
          // printf("myrank=%d\n",myrank);
                MPI_Reduce(recvMessage_intra_Node, recvMessage_inter_switch ,arrSize, MPI_DOUBLE , MPI_SUM, 0, intracomm); 

        }
      // printf("done\n");

        else
        {
          int **intra_ranks;
          intra_ranks=(int **)malloc(index_p * sizeof(int *));
          intra_ranks[0] = (int *)malloc(index_p*number_of_nodes_per_group * sizeof(int));
          for(int j = 1; j < index_p; j++) {
            intra_ranks[j] = intra_ranks[j-1] + number_of_nodes_per_group;
          }

          MPI_Group intra_grp[index_p];
          MPI_Comm intra_comm[index_p];

          i=myrank/(cores*number_of_nodes_per_group);
          for(int j=0;j<number_of_nodes_per_group;j++)
            intra_ranks[i][j]=i*number_of_nodes_per_group*cores + j*cores;

          MPI_Group_incl(world_group,number_of_nodes_per_group, intra_ranks[i], &intra_grp[i]);
          MPI_Comm_create_group(MPI_COMM_WORLD, intra_grp[i],i, &intra_comm[i]);

                          MPI_Reduce(recvMessage_intra_Node, recvMessage_intra_switch ,arrSize, MPI_DOUBLE , MPI_SUM, 0, intra_comm[i]); 


          if(myrank%(cores*number_of_nodes_per_group)==0)
          {
            MPI_Group intergrp;
            MPI_Comm intercomm;
            int *interrank=(int *)malloc(index_p * sizeof(int ));

            for (int j = 0; j < index_p; ++j)
              interrank[j]=j*number_of_nodes_per_group*cores;

            MPI_Group_incl(world_group,index_p,interrank,&intergrp);
            MPI_Comm_create_group(MPI_COMM_WORLD,intergrp,0, &intercomm);
            // printf("myrank=%d\n",myrank);
            MPI_Reduce(recvMessage_intra_switch, recvMessage_inter_switch ,arrSize, MPI_DOUBLE , MPI_SUM, 0, intercomm); 
          }

        }

      }

    }

    dtime=MPI_Wtime();
    time=dtime-stime;
    MPI_Reduce(&time,&maxtime,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    if (!myrank)
    {
      average[k]=maxtime;
    }
  }

  if(!myrank)
  {
     // for (int i = 0; i < arrSize; ++i)
     // {
     //   printf("%lf\n", recvMessage_inter_switch[i]);
     // }

    double total_average=0;
    for (int j = 0; j < 5; ++j)
    {
      total_average=+average[j];
    }
    printf("Reduce_Optimized=%lf\n",total_average/5);
  }
}

void mpi_bcast_optimized()
{
 
 if(myrank==0)
  for (int j = 0; j < arrSize; j++) {
    message[j] = 10;
  }
  double average[5];
  for (int k = 0; k < 5; ++k)
  {
    /* code */
    
    double stime,dtime,time,maxtime;
    stime=MPI_Wtime();


    if(cores==1)
    {
      if (number_of_nodes_per_group>=numtasks)
      {
        MPI_Bcast(message, arrSize, MPI_DOUBLE,0, MPI_COMM_WORLD);
      }
      else
      {
        // intra switch gather
        int **ranks;
        int index_p=numtasks/number_of_nodes_per_group;
        ranks=(int **)malloc(index_p * sizeof(int *));
        ranks[0] = (int *)malloc(index_p*number_of_nodes_per_group * sizeof(int));
        for(int j = 1; j < index_p; j++) {
          ranks[j] = ranks[j-1] + number_of_nodes_per_group;
        }


        int *interrank=(int *)malloc(index_p * sizeof(int ));


        // inter switch gather
        if(myrank%number_of_nodes_per_group==0)
        {
          MPI_Group intergrp;
          MPI_Comm intercomm;
          for (int j = 0; j < numtasks/number_of_nodes_per_group; ++j)
            interrank[j]=j*number_of_nodes_per_group;

           MPI_Group_incl(world_group,numtasks/number_of_nodes_per_group,interrank,&intergrp);
           MPI_Comm_create_group(MPI_COMM_WORLD,intergrp,0, &intercomm);
          // printf("myrank=%d\n",myrank);
           MPI_Bcast(message,arrSize, MPI_DOUBLE, 0, intercomm);
         }



        MPI_Group grp[index_p];
        MPI_Comm prime_comm[index_p];

        int i=myrank/number_of_nodes_per_group;
        for(int j=0;j<number_of_nodes_per_group;j++)
          ranks[i][j]=i*number_of_nodes_per_group + j;
        
        MPI_Group_incl(world_group,number_of_nodes_per_group, ranks[i], &grp[i]);
        MPI_Comm_create_group(MPI_COMM_WORLD, grp[i],i, &prime_comm[i]);

        MPI_Bcast(message, arrSize, MPI_DOUBLE,0, prime_comm[i]);

       /* 
         if(myrank==numtasks-1)
         {
           for (int j = 0; j < arrSize; ++j)
           {
            printf("%lf\n", message[j]);
           }
         }
        */
      }      
    }

    else
    {
      // intra Node gather
      int **ranks;
      int index_p=numtasks/cores;
      ranks=(int **)malloc(index_p * sizeof(int *));
      ranks[0] = (int *)malloc(index_p*cores * sizeof(int));
      for(int j = 1; j < index_p; j++) {
        ranks[j] = ranks[j-1] + cores;
      }

      MPI_Group grp[index_p];
      MPI_Comm prime_comm[index_p];

      int i=myrank/cores;
      for(int j=0;j<cores;j++)
        ranks[i][j]=i*cores + j;

      // if(myrank%cores==0)
      // {
        // for (int j = 0; j < cores; ++j)
        // {
        //   /* code */
        //   printf("%d myrank=%d\n", ranks[i][j],myrank);
        // }
      // }

      if(myrank%cores==0)
      {

        index_p=index_p/number_of_nodes_per_group;

        if(number_of_nodes_per_group>=index_p)
        {

          MPI_Group intragrp;
          MPI_Comm intracomm;
          int *intrarank=(int *)malloc(numtasks/cores * sizeof(int ));

          for (int j = 0; j < numtasks/cores; ++j)
            intrarank[j]=j*cores;

           MPI_Group_incl(world_group,numtasks/cores,intrarank,&intragrp);
           MPI_Comm_create_group(MPI_COMM_WORLD,intragrp,0, &intracomm);
          // printf("myrank=%d\n",myrank);
           MPI_Bcast(message,arrSize, MPI_DOUBLE,0, intracomm);
        }
      // printf("done\n");

        else
        {
          int **intra_ranks;
          intra_ranks=(int **)malloc(index_p * sizeof(int *));
          intra_ranks[0] = (int *)malloc(index_p*number_of_nodes_per_group * sizeof(int));
          for(int j = 1; j < index_p; j++) {
            intra_ranks[j] = intra_ranks[j-1] + number_of_nodes_per_group;
          }
        
          if(myrank%(cores*number_of_nodes_per_group)==0)
          {
            MPI_Group intergrp;
            MPI_Comm intercomm;
            int *interrank=(int *)malloc(index_p * sizeof(int ));

            for (int j = 0; j < index_p; ++j)
              interrank[j]=j*number_of_nodes_per_group*cores;

             MPI_Group_incl(world_group,index_p,interrank,&intergrp);
             MPI_Comm_create_group(MPI_COMM_WORLD,intergrp,0, &intercomm);
            // printf("myrank=%d\n",myrank);
             MPI_Bcast(message,arrSize, MPI_DOUBLE,0, intercomm);
          }

          MPI_Group intra_grp[index_p];
          MPI_Comm intra_comm[index_p];

          i=myrank/(cores*number_of_nodes_per_group);
          for(int j=0;j<number_of_nodes_per_group;j++)
            intra_ranks[i][j]=i*number_of_nodes_per_group*cores + j*cores;

          MPI_Group_incl(world_group,number_of_nodes_per_group, intra_ranks[i], &intra_grp[i]);
          MPI_Comm_create_group(MPI_COMM_WORLD, intra_grp[i],i, &intra_comm[i]);
          MPI_Bcast(message,arrSize, MPI_DOUBLE,0, intra_comm[i]);

         
        }

      }

      MPI_Group_incl(world_group,cores, ranks[i], &grp[i]);
      MPI_Comm_create_group(MPI_COMM_WORLD, grp[i],i, &prime_comm[i]);
      MPI_Bcast(message, arrSize, MPI_DOUBLE,0, prime_comm[i]);

      // if(myrank==numtasks-1)
      // {
      //   for (int j = 0; j < arrSize; ++j)
      //   {
      //     printf("%lf\n", message[j]);
      //   }
      // }


    }

    dtime=MPI_Wtime();
    time=dtime-stime;
    MPI_Reduce(&time,&maxtime,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    if (!myrank)
    {
      average[k]=maxtime;
    }
  }

  if(!myrank)
  {
    // for (int i = 0; i < arrSize*numtasks; ++i)
    // {
    //   printf("%lf\n", recvMessage_inter_switch[i]);
    // }

    double total_average=0;
    for (int j = 0; j < 5; ++j)
    {
      total_average=+average[j];
    }
    printf("Bcast_Optimized=%lf\n",total_average/5);
  }
}



void mpi_gather_optimized()
{
 
  for (int j = 0; j < arrSize; j++) {
    message[j] = myrank;
  }
  double average[5];
  for (int k = 0; k < 5; ++k)
  {
    /* code */
    
    double stime,dtime,time,maxtime;
    stime=MPI_Wtime();


    if(cores==1)
    {
      if (number_of_nodes_per_group>=numtasks)
      {
        MPI_Gather(message, arrSize, MPI_DOUBLE, recvMessage_inter_switch, arrSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
      }
      else
      {
        // intra switch gather
        int **ranks;
        int index_p=numtasks/number_of_nodes_per_group;
        ranks=(int **)malloc(index_p * sizeof(int *));
        ranks[0] = (int *)malloc(index_p*number_of_nodes_per_group * sizeof(int));
        for(int j = 1; j < index_p; j++) {
          ranks[j] = ranks[j-1] + number_of_nodes_per_group;
        }

        int *interrank=(int *)malloc(index_p * sizeof(int ));


        MPI_Group grp[index_p];
        MPI_Comm prime_comm[index_p];

        int i=myrank/number_of_nodes_per_group;
        for(int j=0;j<number_of_nodes_per_group;j++)
          ranks[i][j]=i*number_of_nodes_per_group + j;
        
        MPI_Group_incl(world_group,number_of_nodes_per_group, ranks[i], &grp[i]);
        MPI_Comm_create_group(MPI_COMM_WORLD, grp[i],i, &prime_comm[i]);

        MPI_Gather(message, arrSize, MPI_DOUBLE, recvMessage_intra_switch, arrSize, MPI_DOUBLE,0, prime_comm[i]);

        // inter switch gather
        if(myrank%number_of_nodes_per_group==0)
        {
          MPI_Group intergrp;
          MPI_Comm intercomm;
          for (int j = 0; j < numtasks/number_of_nodes_per_group; ++j)
            interrank[j]=j*number_of_nodes_per_group;

           MPI_Group_incl(world_group,numtasks/number_of_nodes_per_group,interrank,&intergrp);
           MPI_Comm_create_group(MPI_COMM_WORLD,intergrp,0, &intercomm);
          // printf("myrank=%d\n",myrank);
           MPI_Gather(recvMessage_intra_switch,number_of_nodes_per_group*arrSize*cores, MPI_DOUBLE, recvMessage_inter_switch, number_of_nodes_per_group*arrSize*cores, MPI_DOUBLE,0, intercomm);
         }
        
        // if(!myrank)
        // {
        //   for (int j = 0; j < arrSize*numtasks; ++j)
        //   {
        //     printf("%lf\n", recvMessage_inter_switch[j]);
        //   }
        // }

      }      
    }

    else
    {
      // intra Node gather
      int **ranks;
      int index_p=numtasks/cores;
      ranks=(int **)malloc(index_p * sizeof(int *));
      ranks[0] = (int *)malloc(index_p*cores * sizeof(int));
      for(int j = 1; j < index_p; j++) {
        ranks[j] = ranks[j-1] + cores;
      }

      MPI_Group grp[index_p];
      MPI_Comm prime_comm[index_p];

      int i=myrank/cores;
      for(int j=0;j<cores;j++)
        ranks[i][j]=i*cores + j;

      // if(myrank%cores==0)
      // {
        // for (int j = 0; j < cores; ++j)
        // {
        //   /* code */
        //   printf("%d myrank=%d\n", ranks[i][j],myrank);
        // }
      // }
      
      MPI_Group_incl(world_group,cores, ranks[i], &grp[i]);
      MPI_Comm_create_group(MPI_COMM_WORLD, grp[i],i, &prime_comm[i]);
      MPI_Gather(message, arrSize, MPI_DOUBLE, recvMessage_intra_Node, arrSize, MPI_DOUBLE,0, prime_comm[i]);

      if(myrank%cores==0)
      {

        index_p=index_p/number_of_nodes_per_group;

        if(number_of_nodes_per_group>=index_p)
        {

          MPI_Group intragrp;
          MPI_Comm intracomm;
          int *intrarank=(int *)malloc(numtasks/cores * sizeof(int ));

          for (int j = 0; j < numtasks/cores; ++j)
            intrarank[j]=j*cores;

           MPI_Group_incl(world_group,numtasks/cores,intrarank,&intragrp);
           MPI_Comm_create_group(MPI_COMM_WORLD,intragrp,0, &intracomm);
          // printf("myrank=%d\n",myrank);
           MPI_Gather(recvMessage_intra_Node,arrSize*cores, MPI_DOUBLE, recvMessage_inter_switch, arrSize*cores, MPI_DOUBLE,0, intracomm);
        }
      // printf("done\n");

        else
        {
          int **intra_ranks;
          intra_ranks=(int **)malloc(index_p * sizeof(int *));
          intra_ranks[0] = (int *)malloc(index_p*number_of_nodes_per_group * sizeof(int));
          for(int j = 1; j < index_p; j++) {
            intra_ranks[j] = intra_ranks[j-1] + number_of_nodes_per_group;
          }

          MPI_Group intra_grp[index_p];
          MPI_Comm intra_comm[index_p];

          i=myrank/(cores*number_of_nodes_per_group);
          for(int j=0;j<number_of_nodes_per_group;j++)
            intra_ranks[i][j]=i*number_of_nodes_per_group*cores + j*cores;

          MPI_Group_incl(world_group,number_of_nodes_per_group, intra_ranks[i], &intra_grp[i]);
          MPI_Comm_create_group(MPI_COMM_WORLD, intra_grp[i],i, &intra_comm[i]);
          MPI_Gather(recvMessage_intra_Node, cores*arrSize, MPI_DOUBLE, recvMessage_intra_switch, arrSize*cores, MPI_DOUBLE,0, intra_comm[i]);

          if(myrank%(cores*number_of_nodes_per_group)==0)
          {
            MPI_Group intergrp;
            MPI_Comm intercomm;
            int *interrank=(int *)malloc(index_p * sizeof(int ));

            for (int j = 0; j < index_p; ++j)
              interrank[j]=j*number_of_nodes_per_group*cores;

             MPI_Group_incl(world_group,index_p,interrank,&intergrp);
             MPI_Comm_create_group(MPI_COMM_WORLD,intergrp,0, &intercomm);
            // printf("myrank=%d\n",myrank);
             MPI_Gather(recvMessage_intra_switch,number_of_nodes_per_group*arrSize*cores, MPI_DOUBLE, recvMessage_inter_switch, number_of_nodes_per_group*arrSize*cores, MPI_DOUBLE,0, intercomm);
          }

        }

      }

    }

    dtime=MPI_Wtime();
    time=dtime-stime;
    MPI_Reduce(&time,&maxtime,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    if (!myrank)
    {
      average[k]=maxtime;
    }
  }

  if(!myrank)
  {
    // for (int i = 0; i < arrSize*numtasks; ++i)
    // {
    //   printf("%lf\n", recvMessage_inter_switch[i]);
    // }

    double total_average=0;
    for (int j = 0; j < 5; ++j)
    {
      total_average=+average[j];
    }
    printf("Gather_Optimized=%lf\n",total_average/5);
  }

}

int highestPowerof2(int n)
{
    int res = 0;
    for (int i=n; i>=1; i--)
    {
        // If i is a power of 2
        if ((i & (i-1)) == 0)
        {
            res = i;
            break;
        }
    }
    return res;
}


int main(int argc, char *argv[]) 
{
  

  // initialize MPI
  char hostname[MPI_MAX_PROCESSOR_NAME];
  int len;
  
  MPI_Init (&argc, &argv);
  arrSize = atoi(argv[1]);
  // cores = atoi(argv[2]);

  int count=0;
  FILE *fp;
  fp = fopen ("hostfile", "r");
  char filename[100];
    char c;
    for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n') 
            count = count + 1;

    fclose(fp);

  number_of_nodes_per_group = count/6;

  number_of_nodes_per_group=highestPowerof2(number_of_nodes_per_group);


  // get number of tasks
  MPI_Comm_size (MPI_COMM_WORLD, &numtasks);

  // get my rank
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  MPI_Comm_group(MPI_COMM_WORLD, &world_group);


  int *host_info=(int*)malloc(numtasks*sizeof(int));
  int individual_host_info;

  MPI_Get_processor_name (hostname, &len);

  individual_host_info=atoi(hostname+5);

  MPI_Allgather(&individual_host_info ,1,MPI_INT,host_info,1,MPI_INT,MPI_COMM_WORLD );

  if(host_info[0]==host_info[1])
    cores=8;
  else
    cores=1;

  // printf("%d\n",cores );

  message=(double *)malloc(arrSize * sizeof(double ));
  recvMessage_unoptimized=(double *)malloc(arrSize * numtasks * sizeof(double ));


  recvMessage_intra_Node=(double *)malloc(arrSize * cores * sizeof(double ));
  recvMessage_intra_switch=(double *)malloc(arrSize * number_of_nodes_per_group * cores * sizeof(double ));
  recvMessage_inter_switch=(double *)malloc(arrSize * numtasks * sizeof(double ));



  mpi_bcast_default();
  mpi_bcast_optimized();
  mpi_reduce_default();
  mpi_reduce_optimized();
  mpi_gather_default();
  mpi_gather_optimized();
  mpi_alltoallv_default();
  mpi_alltoallv_optimized();
    

  MPI_Group_free(&world_group);
  MPI_Finalize();

}
