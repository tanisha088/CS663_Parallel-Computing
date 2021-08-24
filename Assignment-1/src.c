#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


int main( int argc, char *argv[])
{
    // char message[20],recvmessage[20];
    int myrank;
    int size;
    double stime,dtime,time,maxtime;
    MPI_Status status;
    int num_time_steps = 50;
    int N=sqrt(atoi(argv[1]));

    MPI_Init( &argc, &argv );
    // int count=atoi(argv[1]);
    // int arr[16][16];


    // double arr[N][N]; 
    // double arr2[N][N]; 
    // double arr3[N][N]; 
    // double result[N][N];
    double left_arr[N],right_arr[N],top_arr[N],bottom_arr[N];
    double buff1[N],buff2[N],buff3[N],buff4[N];
    double recv_buff1[N],recv_buff2[N],recv_buff3[N],recv_buff4[N];


    double** arr;

    arr=(double **)malloc(N * sizeof(double *));
    arr[0] = (double *)malloc(N*N * sizeof(double)); 

    for(int i = 1; i < N; i++) {
      arr[i] = arr[i-1] + N;
    }

    double** arr2;

    arr2=(double **)malloc(N * sizeof(double *));
    arr2[0] = (double *)malloc(N*N * sizeof(double)); 

    for(int i = 1; i < N; i++) {
      arr2[i] = arr2[i-1] + N;
    }

    double** arr3;

    arr3=(double **)malloc(N * sizeof(double *));
    arr3[0] = (double *)malloc(N*N * sizeof(double)); 

    for(int i = 1; i < N; i++) {
      arr3[i] = arr3[i-1] + N;
    }

    double** result;

    result=(double **)malloc(N * sizeof(double *));
    result[0] = (double *)malloc(N*N * sizeof(double)); 

    for(int i = 1; i < N; i++) {
      result[i] = result[i-1] + N;
    }

    double** vec1;

    vec1=(double **)malloc(N * sizeof(double *));
    vec1[0] = (double *)malloc(N*N * sizeof(double)); 

    for(int i = 1; i < N; i++) {
      vec1[i] = vec1[i-1] + N;
    }



    MPI_Comm_rank( MPI_COMM_WORLD, &myrank );
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int P=sqrt(size);

    for(int i=0;i<N;i++)
        for (int j=0;j<N;j++)
            arr[i][j]=myrank+i+j;


    for (int i=0;i<N;i++)
    {
        left_arr[i]=0;
        right_arr[i]=0;
        top_arr[i]=0;
        bottom_arr[i]=0;

    }

    

    int comp=myrank/P;
    stime=MPI_Wtime();
    for(int i=0;i<num_time_steps;i++)
    {
        if((myrank%P)-1>=0)
        {
            //left neighbor exist
            for(int j=0;j<N;j++)
            {
                buff1[0]=arr[j][0];
                MPI_Send(buff1,1,MPI_DOUBLE,myrank-1,myrank,MPI_COMM_WORLD);
                MPI_Recv(recv_buff1,1, MPI_DOUBLE, myrank-1, myrank-1,MPI_COMM_WORLD,&status);
                left_arr[j]=recv_buff1[0];
            }
        }
        if((myrank%P)+1<P)
        {
            //right neighbor exist
            for(int j=0;j<N;j++)
            {
                MPI_Recv(recv_buff2,1, MPI_DOUBLE, myrank+1, myrank+1,MPI_COMM_WORLD,&status);
                right_arr[j]=recv_buff2[0];
                buff2[0]=arr[j][N-1];
                MPI_Send(buff2,1,MPI_DOUBLE,myrank+1,myrank,MPI_COMM_WORLD);

            }
        }
        if(comp>0)
        {
            // top neighbor exist
            for(int j=0;j<N;j++)
            {
                buff3[0]=arr[0][j];
                MPI_Send(buff3,1,MPI_DOUBLE,myrank-P,myrank,MPI_COMM_WORLD);
                MPI_Recv(recv_buff3,1, MPI_DOUBLE, myrank-P, myrank-P,MPI_COMM_WORLD,&status);
                top_arr[j]=recv_buff3[0];

            }
        }
        if(comp<P-1)
        {
            //bottom neighbor exist
            for(int j=0;j<N;j++)
            {
                MPI_Recv(recv_buff4,1, MPI_DOUBLE, myrank+P, myrank+P,MPI_COMM_WORLD,&status);
                bottom_arr[j]=recv_buff4[0];
                buff4[0]=arr[N-1][j];
                MPI_Send(buff4,1,MPI_DOUBLE,myrank+P,myrank,MPI_COMM_WORLD);

            }
        }

        //stencil computation


        if(myrank==0)
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(arr[i+1][j]+arr[i][j+1])/2;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr[i][j-1]+arr[i+1][j]+arr[i][j+1])/3;
                    else if(i==N-1 && j==0)
                        average=(arr[i-1][j]+bottom_arr[j]+arr[i][j+1])/3;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(arr[i-1][j]+arr[i+1][j]+arr[i][j+1])/3;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+bottom_arr[j]+arr[i][j+1])/4;
                    else if(i==N-1 && j==N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+bottom_arr[j]+right_arr[j])/4;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i+1][j]+right_arr[j])/4;
                    else if(i==0 && j==N-1)
                        average=(right_arr[j]+arr[i+1][j]+arr[i][j-1])/3;

                    else
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i+1][j]+arr[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank==P-1)
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(left_arr[j]+arr[i+1][j]+arr[i][j+1])/3;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr[i][j-1]+arr[i+1][j]+arr[i][j+1])/3;
                    else if(i==N-1 && j==0)
                        average=(left_arr[j]+arr[i-1][j]+bottom_arr[j]+arr[i][j+1])/4;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(left_arr[j]+arr[i-1][j]+arr[i+1][j]+arr[i][j+1])/4;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+bottom_arr[j]+arr[i][j+1])/4;
                    else if(i==N-1 && j==N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+bottom_arr[j])/3;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i+1][j])/3;
                    else if(i==0 && j==N-1)
                        average=(arr[i+1][j]+arr[i][j-1])/2;

                    else
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i+1][j]+arr[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank==P*(P-1))
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(top_arr[j]+arr[i+1][j]+arr[i][j+1])/3;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr[i][j-1]+top_arr[j]+arr[i+1][j]+arr[i][j+1])/4;
                    else if(i==N-1 && j==0)
                        average=(arr[i-1][j]+arr[i][j+1])/2;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(arr[i-1][j]+arr[i+1][j]+arr[i][j+1])/3;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i][j+1])/3;
                    else if(i==N-1 && j==N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+right_arr[j])/3;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i+1][j]+right_arr[j])/4;
                    else if(i==0 && j==N-1)
                        average=(right_arr[j]+top_arr[j]+arr[i+1][j]+arr[i][j-1])/4;

                    else
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i+1][j]+arr[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank==P*P-1)
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(left_arr[j]+top_arr[j]+arr[i+1][j]+arr[i][j+1])/4;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr[i][j-1]+top_arr[j]+arr[i+1][j]+arr[i][j+1])/4;
                    else if(i==N-1 && j==0)
                        average=(left_arr[j]+arr[i-1][j]+arr[i][j+1])/3;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(left_arr[j]+arr[i-1][j]+arr[i+1][j]+arr[i][j+1])/4;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i][j+1])/3;
                    else if(i==N-1 && j==N-1)
                        average=(arr[i][j-1]+arr[i-1][j])/2;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i+1][j])/3;
                    else if(i==0 && j==N-1)
                        average=(top_arr[j]+arr[i+1][j]+arr[i][j-1])/3;

                    else
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i+1][j]+arr[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank>0 && myrank<P-1)
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {   
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(left_arr[j]+arr[i+1][j]+arr[i][j+1])/3;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr[i][j-1]+arr[i+1][j]+arr[i][j+1])/3;
                    else if(i==N-1 && j==0)
                        average=(left_arr[j]+arr[i-1][j]+bottom_arr[j]+arr[i][j+1])/4;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(left_arr[j]+arr[i-1][j]+arr[i+1][j]+arr[i][j+1])/4;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+bottom_arr[j]+arr[i][j+1])/4;
                    else if(i==N-1 && j==N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+bottom_arr[j]+right_arr[j])/4;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i+1][j]+right_arr[j])/4;
                    else if(i==0 && j==N-1)
                        average=(right_arr[j]+arr[i+1][j]+arr[i][j-1])/3;

                    else
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i+1][j]+arr[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank>P*(P-1) && myrank<P*P-1)
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(left_arr[j]+top_arr[j]+arr[i+1][j]+arr[i][j+1])/4;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr[i][j-1]+top_arr[j]+arr[i+1][j]+arr[i][j+1])/4;
                    else if(i==N-1 && j==0)
                        average=(left_arr[j]+arr[i-1][j]+arr[i][j+1])/3;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(left_arr[j]+arr[i-1][j]+arr[i+1][j]+arr[i][j+1])/4;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i][j+1])/3;
                    else if(i==N-1 && j==N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+right_arr[j])/3;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i+1][j]+right_arr[j])/4;
                    else if(i==0 && j==N-1)
                        average=(right_arr[j]+top_arr[j]+arr[i+1][j]+arr[i][j-1])/4;

                    else
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i+1][j]+arr[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank%P==0 && myrank>0 && myrank<P*(P-1))
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(top_arr[j]+arr[i+1][j]+arr[i][j+1])/3;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr[i][j-1]+top_arr[j]+arr[i+1][j]+arr[i][j+1])/4;
                    else if(i==N-1 && j==0)
                        average=(arr[i-1][j]+bottom_arr[j]+arr[i][j+1])/3;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(arr[i-1][j]+arr[i+1][j]+arr[i][j+1])/3;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+bottom_arr[j]+arr[i][j+1])/4;
                    else if(i==N-1 && j==N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+bottom_arr[j]+right_arr[j])/4;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i+1][j]+right_arr[j])/4;
                    else if(i==0 && j==N-1)
                        average=(right_arr[j]+top_arr[j]+arr[i+1][j]+arr[i][j-1])/4;

                    else
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i+1][j]+arr[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank%P==P-1 && myrank>P-1 && myrank<P*P-1)
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(left_arr[j]+top_arr[j]+arr[i+1][j]+arr[i][j+1])/4;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr[i][j-1]+top_arr[j]+arr[i+1][j]+arr[i][j+1])/4;
                    else if(i==N-1 && j==0)
                        average=(left_arr[j]+arr[i-1][j]+bottom_arr[j]+arr[i][j+1])/4;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(left_arr[j]+arr[i-1][j]+arr[i+1][j]+arr[i][j+1])/4;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+bottom_arr[j]+arr[i][j+1])/4;
                    else if(i==N-1 && j==N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+bottom_arr[j])/3;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i+1][j])/3;
                    else if(i==0 && j==N-1)
                        average=(top_arr[j]+arr[i+1][j]+arr[i][j-1])/3;

                    else
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i+1][j]+arr[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(left_arr[j]+top_arr[j]+arr[i+1][j]+arr[i][j+1])/4;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr[i][j-1]+top_arr[j]+arr[i+1][j]+arr[i][j+1])/4;
                    else if(i==N-1 && j==0)
                        average=(left_arr[j]+arr[i-1][j]+bottom_arr[j]+arr[i][j+1])/4;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(left_arr[j]+arr[i-1][j]+arr[i+1][j]+arr[i][j+1])/4;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+bottom_arr[j]+arr[i][j+1])/4;
                    else if(i==N-1 && j==N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+bottom_arr[j]+right_arr[j])/4;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i+1][j]+right_arr[j])/4;
                    else if(i==0 && j==N-1)
                        average=(right_arr[j]+top_arr[j]+arr[i+1][j]+arr[i][j-1])/4;

                    else
                        average=(arr[i][j-1]+arr[i-1][j]+arr[i+1][j]+arr[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        for(int i=0;i<N;i++)
            for(int j=0;j<N;j++)
                arr[i][j]=result[i][j];


        
    }

    dtime=MPI_Wtime();
    time=dtime-stime;
    MPI_Reduce(&time,&maxtime,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    if (!myrank)
        printf("%lf\n",maxtime);

    // printf("%lf \n",time);

    // if(myrank==0)
    // {

    //     printf("N=%d\n",N);
    //     printf("Process=%d\n",P);

    //     for(int i=0;i<N;i++)
    //         printf("%lf ",left_arr[i]); 
    //     printf("\n");
    //     for(int i=0;i<N;i++)
    //         printf("%lf ",right_arr[i]); 
    //     printf("\n");  
    //     for(int i=0;i<N;i++)
    //         printf("%lf ",top_arr[i]); 
    //     printf("\n"); 
    //     for(int i=0;i<N;i++)
    //         printf("%lf ",bottom_arr[i]); 
    //     printf("\n"); 
    //     printf("\n"); 

    // }

    // Part-2 of Assignment



    // double arr2[N][N];

    for(int i=0;i<N;i++)
        for (int j=0;j<N;j++)
            arr2[i][j]=myrank+i+j; 

    for (int i=0;i<N;i++)
    {
        left_arr[i]=0;
        right_arr[i]=0;
        top_arr[i]=0;
        bottom_arr[i]=0;

    }

    stime=MPI_Wtime();
    for(int i=0;i<num_time_steps;i++)
    {
        if((myrank%P)-1>=0)
        {
            //left neighbor exist
            int s_position=0,r_position=0;
            for(int j=0;j<N;j++)
            {
                MPI_Pack(&arr2[j][0], 1, MPI_DOUBLE, buff1, 8*N, &s_position, MPI_COMM_WORLD);
            }
                MPI_Send(buff1,s_position,MPI_PACKED,myrank-1,myrank,MPI_COMM_WORLD);
                MPI_Recv(recv_buff1,N*8, MPI_PACKED, myrank-1, myrank-1,MPI_COMM_WORLD,&status);

            for(int j=0;j<N;j++)
            {
                MPI_Unpack(recv_buff1, N*8, &r_position, &left_arr[j], 1, MPI_DOUBLE, MPI_COMM_WORLD);
            }
        }
        if((myrank%P)+1<P)
        {
            //right neighbor exist
            int s_position=0,r_position=0;
            MPI_Recv(recv_buff2,N*8, MPI_PACKED, myrank+1, myrank+1,MPI_COMM_WORLD,&status);
            for(int j=0;j<N;j++)
            {
                MPI_Unpack(recv_buff2, N*8, &r_position, &right_arr[j], 1, MPI_DOUBLE, MPI_COMM_WORLD);
            }
            for(int j=0;j<N;j++)
            {
                MPI_Pack(&arr2[j][N-1], 1, MPI_DOUBLE, buff2, 8*N, &s_position, MPI_COMM_WORLD);
            }
                MPI_Send(buff2,s_position,MPI_PACKED,myrank+1,myrank,MPI_COMM_WORLD);
        }
        if(comp>0)
        {
            // top neighbor exist
            int s_position=0,r_position=0;
            for(int j=0;j<N;j++)
            {
                MPI_Pack(&arr2[0][j], 1, MPI_DOUBLE, buff3, 8*N, &s_position, MPI_COMM_WORLD);

            }
                MPI_Send(buff3,s_position,MPI_PACKED,myrank-P,myrank,MPI_COMM_WORLD);
                MPI_Recv(recv_buff3,N*8, MPI_PACKED, myrank-P, myrank-P,MPI_COMM_WORLD,&status);
            for(int j=0;j<N;j++)
            {
                MPI_Unpack(recv_buff3, N*8, &r_position, &top_arr[j], 1, MPI_DOUBLE, MPI_COMM_WORLD);
            }
        }
        if(comp<P-1)
        {
            //bottom neighbor exist
            int s_position=0,r_position=0;
            MPI_Recv(recv_buff4,N*8, MPI_PACKED, myrank+P, myrank+P,MPI_COMM_WORLD,&status);
            for(int j=0;j<N;j++)
            {
                MPI_Unpack(recv_buff4, N*8, &r_position, &bottom_arr[j], 1, MPI_DOUBLE, MPI_COMM_WORLD);
            }
            for(int j=0;j<N;j++)
            {
                MPI_Pack(&arr2[N-1][j], 1, MPI_DOUBLE, buff4, 8*N, &s_position, MPI_COMM_WORLD);

            }
            MPI_Send(buff4,s_position,MPI_PACKED,myrank+P,myrank,MPI_COMM_WORLD);
        }

        //stencil computation
        
        if(myrank==0)
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(arr2[i+1][j]+arr2[i][j+1])/2;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr2[i][j-1]+arr2[i+1][j]+arr2[i][j+1])/3;
                    else if(i==N-1 && j==0)
                        average=(arr2[i-1][j]+bottom_arr[j]+arr2[i][j+1])/3;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(arr2[i-1][j]+arr2[i+1][j]+arr2[i][j+1])/3;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+bottom_arr[j]+arr2[i][j+1])/4;
                    else if(i==N-1 && j==N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+bottom_arr[j]+right_arr[j])/4;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i+1][j]+right_arr[j])/4;
                    else if(i==0 && j==N-1)
                        average=(right_arr[j]+arr2[i+1][j]+arr2[i][j-1])/3;

                    else
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i+1][j]+arr2[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank==P-1)
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(left_arr[j]+arr2[i+1][j]+arr2[i][j+1])/3;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr2[i][j-1]+arr2[i+1][j]+arr2[i][j+1])/3;
                    else if(i==N-1 && j==0)
                        average=(left_arr[j]+arr2[i-1][j]+bottom_arr[j]+arr2[i][j+1])/4;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(left_arr[j]+arr2[i-1][j]+arr2[i+1][j]+arr2[i][j+1])/4;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+bottom_arr[j]+arr2[i][j+1])/4;
                    else if(i==N-1 && j==N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+bottom_arr[j])/3;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i+1][j])/3;
                    else if(i==0 && j==N-1)
                        average=(arr2[i+1][j]+arr2[i][j-1])/2;

                    else
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i+1][j]+arr2[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank==P*(P-1))
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(top_arr[j]+arr2[i+1][j]+arr2[i][j+1])/3;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr2[i][j-1]+top_arr[j]+arr2[i+1][j]+arr2[i][j+1])/4;
                    else if(i==N-1 && j==0)
                        average=(arr2[i-1][j]+arr2[i][j+1])/2;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(arr2[i-1][j]+arr2[i+1][j]+arr2[i][j+1])/3;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i][j+1])/3;
                    else if(i==N-1 && j==N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+right_arr[j])/3;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i+1][j]+right_arr[j])/4;
                    else if(i==0 && j==N-1)
                        average=(right_arr[j]+top_arr[j]+arr2[i+1][j]+arr2[i][j-1])/4;

                    else
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i+1][j]+arr2[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank==P*P-1)
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(left_arr[j]+top_arr[j]+arr2[i+1][j]+arr2[i][j+1])/4;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr2[i][j-1]+top_arr[j]+arr2[i+1][j]+arr2[i][j+1])/4;
                    else if(i==N-1 && j==0)
                        average=(left_arr[j]+arr2[i-1][j]+arr2[i][j+1])/3;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(left_arr[j]+arr2[i-1][j]+arr2[i+1][j]+arr2[i][j+1])/4;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i][j+1])/3;
                    else if(i==N-1 && j==N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j])/2;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i+1][j])/3;
                    else if(i==0 && j==N-1)
                        average=(top_arr[j]+arr2[i+1][j]+arr2[i][j-1])/3;

                    else
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i+1][j]+arr2[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank>0 && myrank<P-1)
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {   
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(left_arr[j]+arr2[i+1][j]+arr2[i][j+1])/3;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr2[i][j-1]+arr2[i+1][j]+arr2[i][j+1])/3;
                    else if(i==N-1 && j==0)
                        average=(left_arr[j]+arr2[i-1][j]+bottom_arr[j]+arr2[i][j+1])/4;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(left_arr[j]+arr2[i-1][j]+arr2[i+1][j]+arr2[i][j+1])/4;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+bottom_arr[j]+arr2[i][j+1])/4;
                    else if(i==N-1 && j==N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+bottom_arr[j]+right_arr[j])/4;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i+1][j]+right_arr[j])/4;
                    else if(i==0 && j==N-1)
                        average=(right_arr[j]+arr2[i+1][j]+arr2[i][j-1])/3;

                    else
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i+1][j]+arr2[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank>P*(P-1) && myrank<P*P-1)
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(left_arr[j]+top_arr[j]+arr2[i+1][j]+arr2[i][j+1])/4;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr2[i][j-1]+top_arr[j]+arr2[i+1][j]+arr2[i][j+1])/4;
                    else if(i==N-1 && j==0)
                        average=(left_arr[j]+arr2[i-1][j]+arr2[i][j+1])/3;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(left_arr[j]+arr2[i-1][j]+arr2[i+1][j]+arr2[i][j+1])/4;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i][j+1])/3;
                    else if(i==N-1 && j==N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+right_arr[j])/3;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i+1][j]+right_arr[j])/4;
                    else if(i==0 && j==N-1)
                        average=(right_arr[j]+top_arr[j]+arr2[i+1][j]+arr2[i][j-1])/4;

                    else
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i+1][j]+arr2[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank%P==0 && myrank>0 && myrank<P*(P-1))
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(top_arr[j]+arr2[i+1][j]+arr2[i][j+1])/3;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr2[i][j-1]+top_arr[j]+arr2[i+1][j]+arr2[i][j+1])/4;
                    else if(i==N-1 && j==0)
                        average=(arr2[i-1][j]+bottom_arr[j]+arr2[i][j+1])/3;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(arr2[i-1][j]+arr2[i+1][j]+arr2[i][j+1])/3;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+bottom_arr[j]+arr2[i][j+1])/4;
                    else if(i==N-1 && j==N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+bottom_arr[j]+right_arr[j])/4;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i+1][j]+right_arr[j])/4;
                    else if(i==0 && j==N-1)
                        average=(right_arr[j]+top_arr[j]+arr2[i+1][j]+arr2[i][j-1])/4;

                    else
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i+1][j]+arr2[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank%P==P-1 && myrank>P-1 && myrank<P*P-1)
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(left_arr[j]+top_arr[j]+arr2[i+1][j]+arr2[i][j+1])/4;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr2[i][j-1]+top_arr[j]+arr2[i+1][j]+arr2[i][j+1])/4;
                    else if(i==N-1 && j==0)
                        average=(left_arr[j]+arr2[i-1][j]+bottom_arr[j]+arr2[i][j+1])/4;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(left_arr[j]+arr2[i-1][j]+arr2[i+1][j]+arr2[i][j+1])/4;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+bottom_arr[j]+arr2[i][j+1])/4;
                    else if(i==N-1 && j==N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+bottom_arr[j])/3;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i+1][j])/3;
                    else if(i==0 && j==N-1)
                        average=(top_arr[j]+arr2[i+1][j]+arr2[i][j-1])/3;

                    else
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i+1][j]+arr2[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(left_arr[j]+top_arr[j]+arr2[i+1][j]+arr2[i][j+1])/4;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr2[i][j-1]+top_arr[j]+arr2[i+1][j]+arr2[i][j+1])/4;
                    else if(i==N-1 && j==0)
                        average=(left_arr[j]+arr2[i-1][j]+bottom_arr[j]+arr2[i][j+1])/4;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(left_arr[j]+arr2[i-1][j]+arr2[i+1][j]+arr2[i][j+1])/4;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+bottom_arr[j]+arr2[i][j+1])/4;
                    else if(i==N-1 && j==N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+bottom_arr[j]+right_arr[j])/4;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i+1][j]+right_arr[j])/4;
                    else if(i==0 && j==N-1)
                        average=(right_arr[j]+top_arr[j]+arr2[i+1][j]+arr2[i][j-1])/4;

                    else
                        average=(arr2[i][j-1]+arr2[i-1][j]+arr2[i+1][j]+arr2[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        for(int i=0;i<N;i++)
            for(int j=0;j<N;j++)
                arr2[i][j]=result[i][j];
    }

    dtime=MPI_Wtime();
    time=dtime-stime;
    MPI_Reduce(&time,&maxtime,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    if (!myrank)
        printf("%lf\n",maxtime);


    // if(myrank==0)
    // {

    //     printf("N=%d\n",N);
    //     printf("Process=%d\n",P);

    //     for(int i=0;i<N;i++)
    //         printf("%lf ",left_arr[i]); 
    //     printf("\n");
    //     for(int i=0;i<N;i++)
    //         printf("%lf ",right_arr[i]); 
    //     printf("\n");  
    //     for(int i=0;i<N;i++)
    //         printf("%lf ",top_arr[i]); 
    //     printf("\n"); 
    //     for(int i=0;i<N;i++)
    //         printf("%lf ",bottom_arr[i]); 
    //     printf("\n"); 
    //     printf("\n"); 

    // }


    //part-3 of Assignment


    // double arr3[N][N];
    for(int i=0;i<N;i++)
        for (int j=0;j<N;j++)
            arr3[i][j]=myrank+i+j; 

    for (int i=0;i<N;i++)
    {
        left_arr[i]=0;
        right_arr[i]=0;
        top_arr[i]=0;
        bottom_arr[i]=0;

    }

    MPI_Datatype rowtype,columntype;

    MPI_Type_vector(1,N,N,MPI_DOUBLE,&rowtype);
    MPI_Type_commit(&rowtype);

    MPI_Type_vector(N,1,N,MPI_DOUBLE,&columntype);
    MPI_Type_commit(&columntype);

    stime=MPI_Wtime();
    for(int i=0;i<num_time_steps;i++)
    {
        if((myrank%P)-1>=0)
        {
            //left neighbor exist
            MPI_Send(&arr3[0][0],1,columntype,myrank-1,myrank,MPI_COMM_WORLD);
            MPI_Recv(&vec1[0][0],1, columntype, myrank-1, myrank-1,MPI_COMM_WORLD,&status);
            for(int i=0;i<N;i++)
                left_arr[i]=vec1[i][0];
        }
        if((myrank%P)+1<P)
        {
            //right neighbor exist
            MPI_Recv(&vec1[0][N-1],1, columntype, myrank+1, myrank+1,MPI_COMM_WORLD,&status);
            for(int i=0;i<N;i++)
                right_arr[i]=vec1[i][N-1];
            MPI_Send(&arr3[0][N-1],1,columntype,myrank+1,myrank,MPI_COMM_WORLD);
        }
        if(comp>0)
        {
            // top neighbor exist
            MPI_Send(&arr3[0][0],1,rowtype,myrank-P,myrank,MPI_COMM_WORLD);
            MPI_Recv(top_arr,N, rowtype, myrank-P, myrank-P,MPI_COMM_WORLD,&status);
        }
        if(comp<P-1)
        {
            //bottom neighbor exist
            MPI_Recv(bottom_arr,N, rowtype, myrank+P, myrank+P,MPI_COMM_WORLD,&status);
            MPI_Send(&arr3[N-1][0],1,rowtype,myrank+P,myrank,MPI_COMM_WORLD);
        }


        //stencil computation
        
        if(myrank==0)
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(arr3[i+1][j]+arr3[i][j+1])/2;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr3[i][j-1]+arr3[i+1][j]+arr3[i][j+1])/3;
                    else if(i==N-1 && j==0)
                        average=(arr3[i-1][j]+bottom_arr[j]+arr3[i][j+1])/3;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(arr3[i-1][j]+arr3[i+1][j]+arr3[i][j+1])/3;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+bottom_arr[j]+arr3[i][j+1])/4;
                    else if(i==N-1 && j==N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+bottom_arr[j]+right_arr[j])/4;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i+1][j]+right_arr[j])/4;
                    else if(i==0 && j==N-1)
                        average=(right_arr[j]+arr3[i+1][j]+arr3[i][j-1])/3;

                    else
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i+1][j]+arr3[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank==P-1)
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(left_arr[j]+arr3[i+1][j]+arr3[i][j+1])/3;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr3[i][j-1]+arr3[i+1][j]+arr3[i][j+1])/3;
                    else if(i==N-1 && j==0)
                        average=(left_arr[j]+arr3[i-1][j]+bottom_arr[j]+arr3[i][j+1])/4;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(left_arr[j]+arr3[i-1][j]+arr3[i+1][j]+arr3[i][j+1])/4;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+bottom_arr[j]+arr3[i][j+1])/4;
                    else if(i==N-1 && j==N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+bottom_arr[j])/3;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i+1][j])/3;
                    else if(i==0 && j==N-1)
                        average=(arr3[i+1][j]+arr3[i][j-1])/2;

                    else
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i+1][j]+arr3[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank==P*(P-1))
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(top_arr[j]+arr3[i+1][j]+arr3[i][j+1])/3;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr3[i][j-1]+top_arr[j]+arr3[i+1][j]+arr3[i][j+1])/4;
                    else if(i==N-1 && j==0)
                        average=(arr3[i-1][j]+arr3[i][j+1])/2;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(arr3[i-1][j]+arr3[i+1][j]+arr3[i][j+1])/3;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i][j+1])/3;
                    else if(i==N-1 && j==N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+right_arr[j])/3;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i+1][j]+right_arr[j])/4;
                    else if(i==0 && j==N-1)
                        average=(right_arr[j]+top_arr[j]+arr3[i+1][j]+arr3[i][j-1])/4;

                    else
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i+1][j]+arr3[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank==P*P-1)
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(left_arr[j]+top_arr[j]+arr3[i+1][j]+arr3[i][j+1])/4;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr3[i][j-1]+top_arr[j]+arr3[i+1][j]+arr3[i][j+1])/4;
                    else if(i==N-1 && j==0)
                        average=(left_arr[j]+arr3[i-1][j]+arr3[i][j+1])/3;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(left_arr[j]+arr3[i-1][j]+arr3[i+1][j]+arr3[i][j+1])/4;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i][j+1])/3;
                    else if(i==N-1 && j==N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j])/2;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i+1][j])/3;
                    else if(i==0 && j==N-1)
                        average=(top_arr[j]+arr3[i+1][j]+arr3[i][j-1])/3;

                    else
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i+1][j]+arr3[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank>0 && myrank<P-1)
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {   
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(left_arr[j]+arr3[i+1][j]+arr3[i][j+1])/3;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr3[i][j-1]+arr3[i+1][j]+arr3[i][j+1])/3;
                    else if(i==N-1 && j==0)
                        average=(left_arr[j]+arr3[i-1][j]+bottom_arr[j]+arr3[i][j+1])/4;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(left_arr[j]+arr3[i-1][j]+arr3[i+1][j]+arr3[i][j+1])/4;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+bottom_arr[j]+arr3[i][j+1])/4;
                    else if(i==N-1 && j==N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+bottom_arr[j]+right_arr[j])/4;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i+1][j]+right_arr[j])/4;
                    else if(i==0 && j==N-1)
                        average=(right_arr[j]+arr3[i+1][j]+arr3[i][j-1])/3;

                    else
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i+1][j]+arr3[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank>P*(P-1) && myrank<P*P-1)
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(left_arr[j]+top_arr[j]+arr3[i+1][j]+arr3[i][j+1])/4;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr3[i][j-1]+top_arr[j]+arr3[i+1][j]+arr3[i][j+1])/4;
                    else if(i==N-1 && j==0)
                        average=(left_arr[j]+arr3[i-1][j]+arr3[i][j+1])/3;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(left_arr[j]+arr3[i-1][j]+arr3[i+1][j]+arr3[i][j+1])/4;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i][j+1])/3;
                    else if(i==N-1 && j==N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+right_arr[j])/3;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i+1][j]+right_arr[j])/4;
                    else if(i==0 && j==N-1)
                        average=(right_arr[j]+top_arr[j]+arr3[i+1][j]+arr3[i][j-1])/4;

                    else
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i+1][j]+arr3[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank%P==0 && myrank>0 && myrank<P*(P-1))
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(top_arr[j]+arr3[i+1][j]+arr3[i][j+1])/3;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr3[i][j-1]+top_arr[j]+arr3[i+1][j]+arr3[i][j+1])/4;
                    else if(i==N-1 && j==0)
                        average=(arr3[i-1][j]+bottom_arr[j]+arr3[i][j+1])/3;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(arr3[i-1][j]+arr3[i+1][j]+arr3[i][j+1])/3;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+bottom_arr[j]+arr3[i][j+1])/4;
                    else if(i==N-1 && j==N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+bottom_arr[j]+right_arr[j])/4;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i+1][j]+right_arr[j])/4;
                    else if(i==0 && j==N-1)
                        average=(right_arr[j]+top_arr[j]+arr3[i+1][j]+arr3[i][j-1])/4;

                    else
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i+1][j]+arr3[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else if(myrank%P==P-1 && myrank>P-1 && myrank<P*P-1)
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(left_arr[j]+top_arr[j]+arr3[i+1][j]+arr3[i][j+1])/4;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr3[i][j-1]+top_arr[j]+arr3[i+1][j]+arr3[i][j+1])/4;
                    else if(i==N-1 && j==0)
                        average=(left_arr[j]+arr3[i-1][j]+bottom_arr[j]+arr3[i][j+1])/4;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(left_arr[j]+arr3[i-1][j]+arr3[i+1][j]+arr3[i][j+1])/4;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+bottom_arr[j]+arr3[i][j+1])/4;
                    else if(i==N-1 && j==N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+bottom_arr[j])/3;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i+1][j])/3;
                    else if(i==0 && j==N-1)
                        average=(top_arr[j]+arr3[i+1][j]+arr3[i][j-1])/3;

                    else
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i+1][j]+arr3[i][j+1])/4;

                    result[i][j]=average;




                }
            }

        }

        else
        {
            // printf("myrank=%d\n",myrank );
            for(int i=0;i<N;i++)
            {
                for(int j=0;j<N;j++)
                {
                    double average=0;
                    if(i==0 && j==0)
                        average=(left_arr[j]+top_arr[j]+arr3[i+1][j]+arr3[i][j+1])/4;
                    else if(i==0 && j>0 && j<N-1)
                        average=(arr3[i][j-1]+top_arr[j]+arr3[i+1][j]+arr3[i][j+1])/4;
                    else if(i==N-1 && j==0)
                        average=(left_arr[j]+arr3[i-1][j]+bottom_arr[j]+arr3[i][j+1])/4;
                    else if(j==0 && i>0 && i!=N-1)
                        average=(left_arr[j]+arr3[i-1][j]+arr3[i+1][j]+arr3[i][j+1])/4;
                    else if(i==N-1 && j>0 && j<N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+bottom_arr[j]+arr3[i][j+1])/4;
                    else if(i==N-1 && j==N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+bottom_arr[j]+right_arr[j])/4;
                    else if(i>0 && j==N-1 && i<N-1)
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i+1][j]+right_arr[j])/4;
                    else if(i==0 && j==N-1)
                        average=(right_arr[j]+top_arr[j]+arr3[i+1][j]+arr3[i][j-1])/4;

                    else
                        average=(arr3[i][j-1]+arr3[i-1][j]+arr3[i+1][j]+arr3[i][j+1])/4;

                    result[i][j]=average;




                }
            }
        }


        for(int i=0;i<N;i++)
            for(int j=0;j<N;j++)
                arr3[i][j]=result[i][j];
    }

    dtime=MPI_Wtime();
    time=dtime-stime;
    MPI_Reduce(&time,&maxtime,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    if (!myrank)
        printf("%lf\n",maxtime);

    // if(myrank==0)
    // {

    //     printf("N=%d\n",N);
    //     printf("Process=%d\n",P);

    //     for(int i=0;i<N;i++)
    //         printf("%lf ",left_arr[i]); 
    //     printf("\n");
    //     for(int i=0;i<N;i++)
    //         printf("%lf ",right_arr[i]); 
    //     printf("\n");  
    //     for(int i=0;i<N;i++)
    //         printf("%lf ",top_arr[i]); 
    //     printf("\n"); 
    //     for(int i=0;i<N;i++)
    //         printf("%lf ",bottom_arr[i]); 
    //     printf("\n"); 
    //     printf("\n"); 

    // }

    // if(myrank==0)
    // {

    //     // printf("N=%d\n",N);
    //     // printf("Process=%d\n",P);

    //     // for(int i=0;i<N;i++)
    //     //     printf("%lf ",left_arr[i]); 
    //     // printf("\n");
    //     // for(int i=0;i<N;i++)
    //     //     printf("%lf ",right_arr[i]); 
    //     // printf("\n");  
    //     // for(int i=0;i<N;i++)
    //     //     printf("%lf ",top_arr[i]); 
    //     // printf("\n"); 
    //     // for(int i=0;i<N;i++)
    //     //     printf("%lf ",bottom_arr[i]); 
    //     // printf("\n"); 
    //     // printf("\n"); 
    //     // for(int i=0;i<N;i++)
    //     // {
    //     //     for (int j=0;j<N;j++)
    //     //         printf("%lf ",arr[i][j]);
    //     //     printf("\n");
    //     // }
    //     // printf("\n");

    //     // for(int i=0;i<N;i++)
    //     // {
    //     //     for (int j=0;j<N;j++)
    //     //         printf("%lf ",arr2[i][j]);
    //     //     printf("\n");
    //     // }
    //     // printf("\n");
    //     // for(int i=0;i<N;i++)
    //     // {
    //     //     for (int j=0;j<N;j++)
    //     //         printf("%lf ",arr3[i][j]);
    //     //     printf("\n");
    //     // }
    //     printf("\n");
    //     int flag=0;
    //     for(int i=0;i<N;i++)
    //     {
    //         for(int j=0;j<N;j++)
    //         {
    //             if(abs(arr[i][j]-arr2[i][j])>1e-9)
    //             {
    //                 flag=1;
    //                 break;
    //             }
    //         }
    //     }

    //     if(flag==0){
    //         printf("Part-1 and Part-2 output Matched\n");
    //     }
    //     else
    //         printf("Part-1 and Part-2 output Unmatched\n");

    //     flag=0;

    //     for(int i=0;i<N;i++)
    //     {
    //         for(int j=0;j<N;j++)
    //         {
    //             if(abs(arr[i][j]-arr3[i][j])>1e-9)
    //             {
    //                 flag=1;
    //                 break;
    //             }
    //         }
    //     }

    //     if(flag==0){
    //         printf("Part-1 and Part-3 output Matched\n");
    //     }
    //     else
    //         printf("Part-1 and Part-3 output Unmatched\n");


    // }

    MPI_Type_free(&rowtype);
    MPI_Type_free(&columntype);

    free(arr[0]);
    free(arr2[0]);
    free(arr3[0]);
    free(result[0]);
    free(arr);
    free(arr2);
    free(arr3);
    free(result);

    MPI_Finalize();
    return 0;
}
