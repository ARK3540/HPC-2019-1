using namespace std;
#include <iostream>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <stdlib.h>
#include <string>
#include <random>
#include <stdio.h>

using std::mt19937 ;
using std::random_device;
using std::uniform_real_distribution;


int InCircle(const int &n, const auto &x, const auto &y)
{
    int j, cont = 0;
    for(j = 0; j < n; j++)
    {
        if (x*x + y*y < 1)
            cont++;
    }
    return cont;
}

void MontecarloP(int const &n, double &pi)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0, 1);

    int PuntosCirculo = 0;
    int nChunk = n/100;
    int chunk = n/nChunk;
    int i;

    #pragma omp parallel for shared(nChunk, chunk) reduction(+:PuntosCirculo)
    for(i = 0; i <= nChunk; i++)
    {
        auto x = dist(mt);
        auto y = dist(mt);
        PuntosCirculo += InCircle(chunk, x, y);
    }
    pi = 4.0*(double(PuntosCirculo)/n);
}

void MontecarloS(int const &n, double &pi)
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
    pi = 4.0*((double)PuntosCirculo/(double)(n));
}

// int argc, char* argv[]
main(int argc, char* argv[])
{
    int s;
    // cin >> s;
    s = stoi(argv[1]); //s = atoi(argv[1]);
    double pi;
    // Monte Carlo Paralelo
    clock_t StartingPoint = clock();
    MontecarloP(s, pi);
    StartingPoint = clock() - StartingPoint;


    printf("%f \n",pi);
    printf("%f \n",((float)StartingPoint)/CLOCKS_PER_SEC);

    return 0;
}
