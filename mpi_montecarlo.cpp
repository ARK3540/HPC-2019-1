using namespace std;
#include <iostream>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <random>
#include <stdio.h>
#include <mpi.h>

using std::mt19937 ;
using std::random_device;
using std::uniform_real_distribution;



int MontecarloS(int const &n)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0, 1);

    int PuntosCirculo = 0;
    double x, y;
    for(int i = 0; i <= n; i++)
    {
        x = dist(mt);
        y = dist(mt);

        if (sqrt((x*x)+(y*y)) <= 1)
          PuntosCirculo++;
    }
    return PuntosCirculo;
}

void mpi_Montecarlo(int tries, double &result){
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    // Find out rank, size
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int operaciones, number, sumador, totalShoots;
    // We are assuming at least 2 processes for this task
    if (world_size < 2) {
    MPI_Abort(MPI_COMM_WORLD, 1);
    }
    //printf("Hello world from processor, rank %d out of %d processors\n",world_rank, world_size);
    if (world_rank == 0) {
        // Proceso 0 asigna la cantidad de operaciones
        operaciones= tries/(world_size-1);

         clock_t StartingPoint = clock();
 
        for (int i=1; i<world_size;i++){ // i corresponde al proceso destino
            //en caso de que la division no sea exacta, el proceso 1 realiza las operaciones extra
            //cout<<'world size'<<world_size<<'world rank'<<world_rank<<endl;
            if (tries%(world_size-1)!= 0 && i == 1){ 
                int otro =operaciones+tries%(world_size-1); //operaciones + residuo entre intentos y cantidad de procesos trabajadores;
                
                MPI_Send(
                    /* data         = */ &otro, 
                    /* count        = */ 1, 
                    /* datatype     = */ MPI_INT, 
                    /* destination  = */ i, 
                    /* tag          = */ 0, 
                    /* communicator = */ MPI_COMM_WORLD);
            }
            else{
                MPI_Send(&operaciones, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
        }
        //termina asignacion de operaciones
        //recibe el numero de disparos, colapsado de todos los procesos trabajadores
        MPI_Recv(
            /* data         = */ &totalShoots, 
            /* count        = */ 1, 
            /* datatype     = */ MPI_INT, 
            /* source       = */ 1, 
            /* tag          = */ 0, 
            /* communicator = */ MPI_COMM_WORLD, 
            /* status       = */ MPI_STATUS_IGNORE);

            
        StartingPoint = clock() - StartingPoint;
        result= 4.0*((double)totalShoots/(double)(tries));
        printf("pi: %lf \n",result);
        printf("%f.\n",((float)StartingPoint)/CLOCKS_PER_SEC);
        
    } else if (world_rank != 0) {
        // procesos trabajadores reciben la cantidad de operaciones a realizar
        MPI_Recv(
            /* data         = */ &number, 
            /* count        = */ 1, 
            /* datatype     = */ MPI_INT, 
            /* source       = */ 0, 
            /* tag          = */ 0, 
            /* communicator = */ MPI_COMM_WORLD, 
            /* status       = */ MPI_STATUS_IGNORE);
        printf("Process %d received %d operations from process 0\n", world_rank, number);
        int shoots =MontecarloS(number);
        //printf("shoots: %d\n", shoots);
        if(world_rank == world_size-1){
            //el ultimo no recibe al colapsar y debe ser el primero en enviar
            MPI_Send(
            /* data         = */ &shoots, 
            /* count        = */ 1, 
            /* datatype     = */ MPI_INT, 
            /* destination  = */ world_rank-1, 
            /* tag          = */ 0, 
            /* communicator = */ MPI_COMM_WORLD);
        }else
        {
            //recibe el numero de cortes del proceso superior
            MPI_Recv(
            /* data         = */ &sumador, 
            /* count        = */ 1, 
            /* datatype     = */ MPI_INT, 
            /* source       = */ world_rank+1, 
            /* tag          = */ 0, 
            /* communicator = */ MPI_COMM_WORLD, 
            /* status       = */ MPI_STATUS_IGNORE);
            printf("process %d recieved %d + %d from %d\n",world_rank,sumador,shoots,world_rank+1);
            //adiciona cortes de este proceso al recibido del anterior superior
            sumador= sumador + shoots;
            //envia resultado al siguiente proceso inferior
            MPI_Send(
            /* data         = */ &sumador, 
            /* count        = */ 1, 
            /* datatype     = */ MPI_INT, 
            /* destination  = */ world_rank-1, 
            /* tag          = */ 0, 
            /* communicator = */ MPI_COMM_WORLD);

        }
    }
    MPI_Finalize();
}



int main(int argc, char* argv[])
{
    double l;
    mpi_Montecarlo(stoi(argv[1]),l);
    return 0;
}