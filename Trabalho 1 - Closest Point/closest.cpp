#include <iostream>
#include <fstream>
#include<stdlib.h>
#include <sstream>
#include <string>
#include <cmath>
#include <time.h>
using namespace std;

/*
Nome do autor: Tiago Gonçalves da Silva
RA do autor: 2023644
*/

typedef struct ponto
{
    double x,y;
} Pont;

typedef struct distancia
{
    Pont p1;
    Pont p2;
    double d;
} Dist;

Dist calcDistancia(Pont a, Pont b)
{
    Dist d;

    d.d = sqrt(pow(a.x - b.x,2)+pow(a.y - b.y,2));
    d.p1 = a;
    d.p2 = b;
    return d;
}

Dist distancia(Pont* p, int ini, int fim)
{
    Dist dist, aux;

    dist = calcDistancia(p[ini], p[ini+1]);
    for(int i = ini; i < fim+1; i++)
    {
        for(int j = i+1; j < fim+1; j++)
        {
            aux = calcDistancia(p[i], p[j]);
            if(aux.d < dist.d)
                dist = aux;
        }
    }
    return dist;
}

void combinaY(Pont* p, int ini, int mid, int fim)
{
    int x = 0;
    int y = 0;
    int t1 = mid-ini+1;
    Pont aux[t1];
    int t2 = fim-mid;
    Pont aux2[t2];

    int a = ini;
    while(x < t1)
        aux[x++] = p[a++];

    x = 0;

    while(x < t2)
        aux2[x++] = p[a++];

    x = 0;
    a = ini;
    while(x < t1 && y < t2)
    {
        if(aux[x].y < aux2[y].y || (aux[x].y == aux2[y].y && aux[x].x < aux2[y].x))
            p[a++] = aux[x++];
        else
            p[a++] = aux2[y++];
    }

    while(x < t1)
        p[a++] = aux[x++];
    
    while(y < t2)
        p[a++] = aux2[y++];

    return;
}

void mergeY(Pont* p, int ini, int fim)
{
    if(ini >= fim)
        return;
    
    int mid = (ini+fim)/2;

    mergeY(p, ini, mid);
    mergeY(p, mid+1, fim);
    combinaY(p, ini, mid, fim);
}

Dist faixa(Pont* faixa, int tam, double d)
{
    Dist menor, aux;

    if(tam)
        menor = calcDistancia(faixa[0], faixa[1]);
    for(int i = 0; i < tam; i++)
    {
        for(int j = i+1; j < tam && (faixa[j].y - faixa[i].y) < d; j++)
        {
            aux = calcDistancia(faixa[i], faixa[j]);
            menor = aux.d < menor.d ? aux : menor;
        }
    }

    return menor;
}

Dist closest(Pont* p, Pont* p2, int tam)
{
    if(tam == 3)
        return distancia(p, 0, tam-1);
    else if(tam == 2)
        return calcDistancia(p[0],p[1]);

    int mid = tam/2;

    Pont *pex, *pey, *pdx, *pdy;
    pex = new Pont[mid];
    pey = new Pont[mid];
    pdx = new Pont[tam-mid];
    pdy = new Pont[tam-mid];

    int aux = 0;
    for(int i = 0; i < tam ; i++)
    {
        if(i < mid)
            pex[i] = p[i];
        else
            pdx[aux++] = p[i];
    }

    int e, d;
    e = d = 0;
    for(int i = 0; i < tam; i++)
    {
        if((p2[i].x < p[mid].x || (p2[i].x == p[mid].x && p2[i].y < p[mid].y)) && e < mid)
            pey[e++] = p2[i];
        else
            pdy[d++] = p2[i];
    }

    Dist d1, d2, menor;

    d1 = closest(pex, pey, mid);
    delete[] pex;
    delete[] pey;
    d2 = closest(pdx, pdy, tam-mid);
    delete[] pdx;
    delete[] pdy;

    menor = d1.d < d2.d ? d1 : d2;

    Pont f[tam];
    int x = 0;
    for(int i = 0; i < tam; i++)
    {
        if(abs(p2[i].x - p[mid].x) < menor.d)
            f[x++] = p2[i];
    }

    Dist aux2 = faixa(f, x, menor.d);

    return (aux2.d < menor.d) ? aux2 : menor;
}

void combinaX(Pont* p, int ini, int mid, int fim)
{
    int x = 0;
    int y = 0;
    int t1 = mid-ini+1;
    Pont aux[t1];
    int t2 = fim-mid;
    Pont aux2[t2];

    int a = ini;
    while(x < t1)
        aux[x++] = p[a++];

    x = 0;

    while(x < t2)
        aux2[x++] = p[a++];

    x = 0;
    a = ini;
    while(x < t1 && y < t2)
    {
        if(aux[x].x < aux2[y].x || (aux[x].x == aux2[y].x && aux[x].y < aux2[y].y))
            p[a++] = aux[x++];
        else
            p[a++] = aux2[y++];
    }

    while(x < t1)
        p[a++] = aux[x++];
    
    while(y < t2)
        p[a++] = aux2[y++];

    return;
}

void mergeX(Pont* p, int ini, int fim)
{
    if(ini >= fim)
        return;
    
    int mid = (ini+fim)/2;

    mergeX(p, ini, mid);
    mergeX(p, mid+1, fim);
    combinaX(p, ini, mid, fim);
}

int main(int argc, char* argv[])
{
    
    Pont *v,*v2;
    Dist d;

    clock_t tempoIni = clock();

    ifstream arq;
    arq.open(argv[1], ios::in);
    if(!arq)
    {
        cout << "Falha ao ler o arquivo\n";
        return 0;
    }

    int tam;
    string linha;
    getline(arq,linha);
    istringstream in(linha);
    in >> tam;

    v = new Pont[tam];
    v2 = new Pont[tam];
    for(int i = 0; getline(arq,linha); i++)
    {
        istringstream in(linha);
        in >> v[i].x >> v[i].y;
        v2[i].x = v[i].x;
        v2[i].y = v[i].y;
    }

    mergeX(v, 0, tam-1);
    mergeY(v2,0, tam-1);

    d = closest(v, v2, tam);
    double tempo = double(clock() - tempoIni)/(double)CLOCKS_PER_SEC;

    cout << endl;
    printf("%.6lf %.6lf %.6lf %.6lf %.6lf %.6lf\n", tempo, d.d, d.p1.x, d.p1.y, d.p2.x, d.p2.y);

    delete[] v;
    delete[] v2;

    return 0;
}