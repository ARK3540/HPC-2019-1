using namespace std;
#include <iostream>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <random>
#include <omp.h>

int InLine(const int &n, const double &MePi)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    int j, cont = 0;

    double LongAguja, DistLinea, AgujaBajo, AgujaAlto, Angulo;

    LongAguja=2.0;
    DistLinea=2.0;

    for (j = 0; j < n; j++)
    {
        AgujaBajo = dist(mt)*DistLinea;
        Angulo = dist(mt)*180;
        AgujaAlto = AgujaBajo + sin(MePi*Angulo/180)*LongAguja;
        if (AgujaAlto >= DistLinea)
        {
            cont++;
        }
    }
    return cont;

}

void BuffonP(const int &stop, const double &MePi, double &Npi)
{

    int NumLanzados, NumCortados;
    NumCortados = 0;

    int nChunk = stop/1000;
    //cout << "nChunk: " << nChunk << endl;
    int chunk = stop/nChunk;
    //cout << "Chunk: " << chunk << endl;

    #pragma omp parallel for shared(nChunk, chunk) reduction(+:NumCortados)
    for (NumLanzados = 0; NumLanzados < stop ; NumLanzados++)
    {
        NumCortados += InLine(chunk, MePi);
    }
    //cout<<"tries:"<<NumLanzados<<" cuts:"<<NumCortados;
    Npi= (double)NumLanzados*2/(double)NumCortados;
}


void BuffonS(const int &stop, const double &MePi, double &Npi)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    int NumLanzados, NumCortados, i;
    double LongAguja, DistLinea, AgujaBajo, AgujaAlto, Angulo;
    NumLanzados=0;
    NumCortados=0;
    LongAguja=2.0;
    DistLinea=2.0;

    for (NumLanzados = 0; NumLanzados < stop ; NumLanzados++)
    {
        AgujaBajo=dist(mt)*DistLinea;
        Angulo=dist(mt)*180;
        AgujaAlto=AgujaBajo + sin(MePi*Angulo/180)*LongAguja;//M_PI de math.h
        //cout<<Angulo<<endl;
        if (AgujaAlto >= DistLinea)
        {
            NumCortados++;
        }

    }
    //cout<<"tries:"<<NumLanzados<<" cuts:"<<NumCortados;
    Npi= (double)NumLanzados*2/(double)NumCortados;
}


int main(int argc, char* argv[])
{
    int stop;
    // cin >> stop;
    stop = stoi(argv[1]);

    double  Npi, MePi = 3.14159265359;

    //Calculo de pi = 2N/N'
    clock_t StartingPoint = clock();
    BuffonS(stop, MePi, Npi);
    StartingPoint = clock() - StartingPoint;

    printf("\n%f \n", Npi);
    printf("%f \n",((float)StartingPoint)/CLOCKS_PER_SEC);

    return 0;
}
