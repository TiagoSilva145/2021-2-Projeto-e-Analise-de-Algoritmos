#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <climits>
#include <time.h>
#include <vector>
using namespace std;

// Ponto no plano
typedef struct ponto
{
    int x;
    int y;
} Ponto;

// Struct para cada um dos elementos do heap
typedef struct heapNode
{
    double chave;
    int pai;
    int indice;
} HeapNode;

// Struct para cada um dos elementos da AGM
typedef struct agmNode
{
    struct agmNode* prox;
    struct agmNode* ultimo;
    int indice;
} AgmNode;

// Recebe a arvore geradora minima, preenchendo o vetor sequencia com a sequencia da busca em profundidade
void profundidade(vector<AgmNode*> agm, int i, vector<int> &sequencia, vector<bool> &visitado)
{
    AgmNode* aux = agm[i];

    visitado[i] = true;
    sequencia.push_back(i);

    while(aux)
    {
        if(aux->prox)
            if(visitado[aux->prox->indice] == false)
                profundidade(agm, aux->prox->indice, sequencia, visitado);
        aux = aux->prox;
    }
}

// Funcao auxiliar que troca duas posicoes no heap
void trocar (vector<HeapNode> &v, int a, int b)
{
    HeapNode aux = v[a];
    v[a] = v[b];
    v[b] = aux;
}

// Funcao auxiliar para atualizar o hep
void min_heapify(vector<HeapNode> &v, int tam, int i)
{
    int e = (i * 2) + 1;
    int d = (i * 2) + 2;
    int menor = i;

    if(e < tam && v[e].chave < v[menor].chave)
        menor = e;
    if(d < tam && v[d].chave < v[menor].chave)
        menor = d;

    if(menor != i)
    {
        trocar(v, menor, i);
        min_heapify(v, tam, menor);
    }
}

// Funcao para remover o valor minimo do heap
HeapNode remove(vector<HeapNode> &v, int *tam)
{
    if(*tam < 1)
        printf("Erro, um unico elemento no heap\n");

    HeapNode min = v[0];

    trocar(v, 0, *tam - 1);
    (*tam)--;

    min_heapify(v, *tam, 0);

    return min;
}

// Eh executada quando um valor do heap eh atualizado no algoritmo prim, atualiza o heap para que continue sendo um heap minimo
void atualiza(vector<HeapNode> &v, int i, int tam)
{
    int p = (i - 1)/2;

    while(i > 0 && v[p].chave > v[i].chave)
    {
        trocar(v, p, i);
        min_heapify(v, tam, i);
        i = p;
        p = (i - 1)/2;
    }
}

// Adiciona um node na agm
void adicionaNode(vector<AgmNode*> agm, HeapNode u)
{
    AgmNode* aux = NULL;

    aux = new agmNode;
    aux->indice = u.indice;
    aux->prox = NULL;

    if(agm[u.pai]->ultimo)
        agm[u.pai]->ultimo->prox = aux;
    else
        agm[u.pai]->prox = aux;

    agm[u.pai]->ultimo = aux;
}

//Calcula a distancia euclidiana entre dois pontos
double calcDistancia(Ponto a, Ponto b)
{
    return sqrt(pow(a.x - b.x,2)+pow(a.y - b.y,2));
}

// Funcao principal da segunda parte do problema
void prim(vector<Ponto> pontos, vector<AgmNode*> agm)
{
    //Variaveis para o heap
    int tam = pontos.size();
    vector<HeapNode> heap;

    //Variaveis para gravar o arquivo de saida
    ofstream grav;
    grav.open("tree.txt", ios::out);

    //Preenche o heap
    HeapNode u;
    u.chave = INT_MAX;
    u.pai = -1;
    for(int i = 0; i < tam; i++)
    {
        u.indice = i;
        heap.push_back(u);
    }
    heap[0].chave = 0;
    heap[0].pai = -1;

    double dist = 0;
    //Enquanto o heap estiver com elementos
    while(tam)
    {
        //Remove um vertice do heap
        u = remove(heap, &tam);

        //Analisa as arestas que ainda estao no heap e que saem deste vertice, atualizando caso seja menor
        for(int i = 0; i < tam; i++)
        {
            dist = calcDistancia(pontos[u.indice], pontos[heap[i].indice]);

            //Se distancia nova menor que a anterior
            if(dist < heap[i].chave)
            {
                heap[i].chave = dist;
                heap[i].pai = u.indice;
                atualiza(heap, i, tam);
            }
        }

        //Grava o arquivo tree.txt da agm
        if(u.pai != -1)
        {
            grav << pontos[u.pai].x << " " << pontos[u.pai].y << endl;
            grav << pontos[u.indice].x << " " << pontos[u.indice].y << endl;

            adicionaNode(agm, u);
        }
    }

    heap.clear();
    grav.close();
}

int main(int argc, char* argv[])
{
    clock_t tempoIni = clock();

    // Faz a leitura do arquivo input
    ifstream arq;
    arq.open("input.txt", ios::in);
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

    //Coloca os pontos do arquivo num vetor
    vector<Ponto> pontos;
    Ponto auxP;
    for(int i = 0; getline(arq,linha); i++)
    {
        istringstream in(linha);
        in >> auxP.x >> auxP.y;
        pontos.push_back(auxP);
    }
    arq.close();

    //Inicializa a agm
    vector<AgmNode*> agm;
    AgmNode* aux = NULL;
    for(int i = 0; i < tam; i++)
    {
        aux = new AgmNode;
        aux->indice = i;
        aux->prox = NULL;
        aux->ultimo = NULL;
        agm.push_back(aux);
    }

    //Faz o prim, gravando a agm
    prim(pontos, agm);

    //Faz a busca em profundidade e grava a sequencia em s
    vector<bool> visitado;
    for(int i = 0; i < pontos.size(); i++)
        visitado.push_back(false);

    vector<int> s;
    profundidade(agm, 0, s, visitado);

    //Grava o caminho no arquivo cycle.txt
    ofstream grav;
    grav.open("cycle.txt", ios::out);

    double distancia = 0;
    for(int i = 0; i < s.size(); i++)
    {
        grav << pontos[s[i]].x << " " << pontos[s[i]].y << endl;
        if(i+1 != s.size())
            distancia += calcDistancia(pontos[s[i]], pontos[s[i+1]]);
    }
    grav << pontos[s[0]].x << " " << pontos[s[0]].y << endl;
    
    distancia += calcDistancia(pontos[s[0]], pontos[s[s.size()-1]]);

    double tempo = double(clock() - tempoIni)/(double)CLOCKS_PER_SEC;

    //Imprime a saida final
    printf("%.6f %.6f\n", tempo, distancia);

    return 0;
}
