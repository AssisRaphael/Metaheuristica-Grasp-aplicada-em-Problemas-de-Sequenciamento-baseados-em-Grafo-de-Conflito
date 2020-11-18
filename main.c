 #include <time.h>
#include <sys/time.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//#include "Grasp.h"

#define MAX_ITERATION 10
#define FATOR_DE_BUSCA 2
#define FATOR_GLOBAL 2

///*Habilita ver os makespans em tempo real*/
//#define Ver_Makespans
///*Seleciona o tipo de construção inicial*/
//#define Habilita_Construcao_Gulosa
#define Habilita_Construcao_SemiGulosa
///*Seleciona o tipo de construção avançada*/
//#define _Construcao_Conflitos
#define _Construcao_Tempo
//#define _Construcao_Tempo_Conflito
///*Seleciona o tipo de busca local*/
int Busca = 1; //1, 2 ou 3

typedef struct operacao{
        float Tinicio, Tfinal, Tempo;
        int Conflitos, isEscolhido, index;
}Operacao;//struct da operação do porto

int VerificaConflito(int a, int b);
void ResetaOperacoes();
void AtualizaAgendamento();
void QuickSort_Conflito(Operacao *a, int left, int right);
void QuickSort_Tempo(Operacao *a, int left, int right);
void QuickSort_Tempo_Conflito(Operacao *a, int left, int right);
float ConstrucaoSimples();
void ConstrucaoGulosa();
void ConstrucaoSemiGulosa();
void Construcao();
void Grasp();
void Construcao_Conflito();
void Construcao_Tempo();
void Construcao_Tempo_Conflito();
void Construcao_Aleatoria();
void GerarArquivo();
void LerArquivo();
void BuscaGlobal();
void BuscaLocal1();
void BuscaLocal2();
void BuscaLocal3();
void Permutacao1(int n);
void Permutacao2(int n);
void Permutacao3(int n);
void Permutacao4(int n);

//Variáveis globais
struct timeval start, end;
int MAX_VERTICES; //numero de vertices
float DENSIDADE;
char nomeInstancia[100], PASTA[100];
long int cont;//variável para limitar as buscas
FILE *fs, *fa, *fg;//arquivos de saida e leitura
char **Matriz_ADJ;//matriz de adjacencia
float MakeSpan;//tempo total de processamento
Operacao *operacao;//vetor com os elementos de operações, "CANDIDATOS"
Operacao *operacao2;//vetor com os elementos de operações atualizadas, "CANDIDATOS"

int VerificaConflito(int a, int b){//se haver conflito retorna 1, senão 0.
        return Matriz_ADJ[a][b];
}

void ResetaOperacoes(){//O vetor operacao2 tem seu campo isEscolhido resetado
    for(int i=0;i<MAX_VERTICES;i++)
            operacao2[i].isEscolhido=0;
}

void AtualizaAgendamento(){//o vetor operacao recebe o vetor operacao2 com o melhor agendamento
    for(int i=0;i<MAX_VERTICES;i++)
        operacao[i]=operacao2[i];
}

void QuickSort_Conflito(Operacao *a, int left, int right){// QuickSort, ordena com base no número de conflitos
    int i, j;
    Operacao x, y;

    i = left;
    j = right;
    x = a[(left + right) / 2];

    while(i <= j) {
        while(a[i].Conflitos < x.Conflitos && i < right) {
            i++;
        }
        while(a[j].Conflitos > x.Conflitos && j > left) {
            j--;
        }
        if(i <= j) {
            y = a[i];
            a[i] = a[j];
            a[j] = y;
            i++;
            j--;
        }
    }

    if(j > left) {
        QuickSort_Conflito(a, left, j);
    }
    if(i < right) {
        QuickSort_Conflito(a, i, right);
    }
}

void QuickSort_Tempo(Operacao *a, int left, int right){// QuickSort, ordena com base no tempo de operação
    int i, j;
    Operacao x, y;

    i = left;
    j = right;
    x = a[(left + right) / 2];

    while(i <= j) {
        while(a[i].Tempo < x.Tempo && i < right) {
            i++;
        }
        while(a[j].Tempo > x.Tempo && j > left) {
            j--;
        }
        if(i <= j) {
            y = a[i];
            a[i] = a[j];
            a[j] = y;
            i++;
            j--;
        }
    }

    if(j > left) {
        QuickSort_Tempo(a, left, j);
    }
    if(i < right) {
        QuickSort_Tempo(a, i, right);
    }
}

void QuickSort_Tempo_Conflito(Operacao *a, int left, int right){// QuickSort, ordena com base no tempo de operação e numero de conflitos
    int i, j;
    Operacao x, y;

    i = left;
    j = right;
    x = a[(left + right) / 2];

    while(i <= j) {
        while((a[i].Tempo * a[i].Conflitos) < (x.Tempo * x.Conflitos) && i < right) {
            i++;
        }
        while((a[j].Tempo * a[i].Conflitos) > (x.Tempo * x.Conflitos) && j > left) {
            j--;
        }
        if(i <= j) {
            y = a[i];
            a[i] = a[j];
            a[j] = y;
            i++;
            j--;
        }
    }

    if(j > left) {
        QuickSort_Tempo_Conflito(a, left, j);
    }
    if(i < right) {
        QuickSort_Tempo_Conflito(a, i, right);
    }
}

float ConstrucaoSimples(){//constrói um solução válida
    //Realiza uma construção na ordem do vetor operacao2
    int maiorTempoConflito;//variável que armazena o maior tempo de termino de uma atividade em conflito com outra
    float makespan=0;
    //Função gulosa que seleciona quais operações devem ser incluidas em cada passo
     for(int i=0;i<MAX_VERTICES;i++){//percorre todas as operações
            maiorTempoConflito=0;//reseta o maior tempo de termino de uma atividade em conflito com outra
            for(int j=0;j<MAX_VERTICES;j++)
                        if(operacao2[j].isEscolhido)//Encontra uma operação já agendada
                                if(VerificaConflito(operacao2[i].index,operacao2[j].index))//a operação tem conflito com uma atividade selecionada
                                    if(operacao2[j].Tfinal>maiorTempoConflito)//a operação tem conflito com outra atividade que dura mais tempo
                                        maiorTempoConflito=operacao2[j].Tfinal;//maior tempo de termino de uma atividade em conflito com outra
            //maiorTempoConflito é o tempo de inicio da atividade, pois antes desse tempo há atividades com conflito em operação
            operacao2[i].isEscolhido=1;
            operacao2[i].Tinicio=maiorTempoConflito;
            operacao2[i].Tfinal=maiorTempoConflito+operacao2[i].Tempo;
            if(operacao2[i].Tfinal>makespan)
                makespan=operacao2[i].Tfinal;
     }//fim for geral
     #ifdef Ver_Makespans
        printf("Makespan = %f\n",makespan);
     #endif
     return makespan;
}//fim construção simples

void ConstrucaoGulosa(){//Realiza um construção ótima agendando primeiramente as operações com menos conflitos
    int maiorTempoConflito;//variável que armazena o maior tempo de termino de uma atividade em conflito com outra
    float makespan=0;

    QuickSort_Tempo_Conflito(operacao, 0, MAX_VERTICES - 1) ; //Ordena o vetor de acordo com o número de conflitos

     for(int i=0;i<MAX_VERTICES;i++){//Função gulosa que seleciona quais operações devem ser incluidas em cada passo
            maiorTempoConflito=0;//reseta o maior tempo de termino de uma atividade em conflito com outra
            for(int j=0;j<MAX_VERTICES;j++)
                        if(operacao[j].isEscolhido)//Encontra uma operação já agendada
                                if(VerificaConflito(operacao[i].index,operacao[j].index))//a operação tem conflito com uma atividade selecionada
                                    if(operacao[j].Tfinal>maiorTempoConflito)//a operação tem conflito com outra atividade que dura mais tempo
                                        maiorTempoConflito=operacao[j].Tfinal;//maior tempo de termino de uma atividade em conflito com outra
            //maiorTempoConflito é o tempo de inicio da atividade, pois antes desse tempo há atividades com conflito em operação
            operacao[i].isEscolhido=1;
            operacao[i].Tinicio=maiorTempoConflito;
            operacao[i].Tfinal=maiorTempoConflito+operacao[i].Tempo;
            if(operacao[i].Tfinal>makespan)
                makespan=operacao[i].Tfinal;
     }//fim for geral

     MakeSpan=makespan;
     printf("Makespan construcao gulosa = %f\n",makespan);

}//fim construção

void ConstrucaoSemiGulosa(){//Realiza um construção ótima agendando primeiramente as operações com menos conflitos
    int maiorTempoConflito;//variável que armazena o maior tempo de termino de uma atividade em conflito com outra
    float makespan=0;
    int op, aux;

    QuickSort_Tempo(operacao, 0, MAX_VERTICES - 1) ; //Ordena o vetor de acordo com o número de conflitos

    //Seleciona uma operação aleatória para iniciar no tempo zero
    //Há mais chance de operações com menos conflitos serem selecionadas aleatoriamente (escolha viciada)
    aux = rand()%100;
    if(aux<60)
        op = rand()%(MAX_VERTICES/3);
    else
        op = rand()%MAX_VERTICES;

    //printf("inicial = %d \n",op);
    operacao[op].isEscolhido = 1;
    operacao[op].Tinicio = 0;
    operacao[op].Tfinal = operacao[op].Tempo;

     for(int i=0;i<MAX_VERTICES;i++){//Função gulosa que seleciona quais operações devem ser incluidas em cada passo
         //printf("i = %d\tmakespan = %f\n",i,makespan);
            if(operacao[i].isEscolhido)
                continue;
            maiorTempoConflito=0;//reseta o maior tempo de termino de uma atividade em conflito com outra
            for(int j=0;j<MAX_VERTICES;j++)
                if(operacao[j].isEscolhido)//Encontra uma operação já agendada
                        if(VerificaConflito(operacao[i].index,operacao[j].index))//a operação tem conflito com uma atividade selecionada
                            if(operacao[j].Tfinal>maiorTempoConflito)//a operação tem conflito com outra atividade que dura mais tempo
                                maiorTempoConflito=operacao[j].Tfinal;//maior tempo de termino de uma atividade em conflito com outra
            //maiorTempoConflito é o tempo de inicio da atividade, pois antes desse tempo há atividades com conflito em operação
            operacao[i].isEscolhido=1;
            operacao[i].Tinicio=maiorTempoConflito;
            operacao[i].Tfinal=maiorTempoConflito+operacao[i].Tempo;
            //printf("op: %d  Ti = %.2f  Tf = %.2f\n",operacao[i].index,operacao[i].Tinicio,operacao[i].Tfinal);
            if(operacao[i].Tfinal>makespan)
                makespan=operacao[i].Tfinal;
     }//fim for geral

     MakeSpan=makespan;
     printf("Makespan construcao semi-gulosa = %f\n",makespan);

}//fim construção

void Construcao(){//O objetivo da função é gerar perturbações na solução de construção a fim de encontrar um makespan melhor na vizinhança
    #ifdef Habilita_Construcao_Gulosa
         for(int i=0;i<MAX_VERTICES;i++)
                operacao[i].isEscolhido=0;
        #ifdef Ver_Makespans
            printf("Iniciando construcao gulosa...\n");
        #endif
        ConstrucaoGulosa();
    #endif

    #ifdef Habilita_Construcao_SemiGulosa
        for(int i=0;i<MAX_VERTICES;i++)
                operacao[i].isEscolhido=0;
        #ifdef Ver_Makespans
                printf("Iniciando construcao semi gulosa ...\n");
        #endif
        ConstrucaoSemiGulosa();
    #endif

   //O vetor de operacao possui os valores do agendamento final que será escrito no arquivo. O vetor operacao2
   //possui uma copia do vetor operacao e é ele que será alterado nas buscas locais. Assim, o vetor operacao possui
   //sempre o agendamento final enquanto o vetor operacao2 é bagunçado nas buscas locais

    for(int i=0;i<MAX_VERTICES;i++)//operacao2 recebe uma copia do vetor operacao
        operacao2[i]=operacao[i];

   #ifdef _Construcao_Conflitos
       #ifdef Ver_Makespans
            printf("\nIniciando Construcao_Conflitos\n"); //Busca por conflitos
        #endif
        Construcao_Conflito();
    #endif

    #ifdef _Construcao_Tempo
       #ifdef Ver_Makespans
            printf("\nIniciando Construcao_Tempo\n");
        #endif
        Construcao_Tempo(); //Busca por tempo
    #endif

    #ifdef _Construcao_Tempo_Conflito
       #ifdef Ver_Makespans
            printf("\nIniciando Construcao_Tempo_Conflito\n");
        #endif
        Construcao_Tempo_Conflito(); //Busca por tempo X conflitos
    #endif

    #ifdef _Construcao_Aleatoria
       #ifdef Ver_Makespans
            printf("\nIniciando  Construcao Aleatoria\n");
        #endif
        Construcao_Aleatoria(); //Construção randomizada
    #endif
    printf("Makespan construcao final = %f\n",MakeSpan);
}

void Construcao_Conflito(){//Pertuba as operações selecionando primeiramente as operações com menos conflitos

    int i;
    int maiorTempoConflito;//variável que armazena o maior tempo de termino de uma atividade em conflito com outra
    float makespan, delta;
    int Troca1 = 0; //Variável auxiliar para trocar posições da solução nas buscas locais

    QuickSort_Conflito(operacao2, 0, MAX_VERTICES - 1) ; //Ordena o vetor de acordo com o número de conflitos

    while(Troca1<MAX_VERTICES/FATOR_DE_BUSCA && cont<MAX_ITERATION){
                makespan=0;//Reseta o makespan
                ResetaOperacoes();//As operações não foram escolhidas ainda
                operacao2[Troca1].Tinicio = 0;//Altera a operação que deverá iniciar no tempo zero
                operacao2[Troca1].Tfinal = operacao2[Troca1].Tempo;//agenda o tempo de fim
                operacao2[Troca1].isEscolhido = 1;
                for(i=0;i<MAX_VERTICES;i++){//reagenda todas as operações apartir da alteração acima
                        if(operacao2[i].isEscolhido)
                            continue;
                        maiorTempoConflito=0;//reseta o maior tempo de termino de uma atividade em conflito com outra
                        for(int j=0;j<MAX_VERTICES;j++)
                            if(operacao2[j].isEscolhido)//checa as atividades já agendadas
                                if(VerificaConflito(operacao[i].index,operacao[j].index))//a operação tem conflito com uma atividade selecionada
                                    if(operacao2[j].Tfinal>maiorTempoConflito)//a operação tem conflito com outra atividade que dura mais tempo
                                        maiorTempoConflito=operacao2[j].Tfinal;//maior tempo de termino de uma atividade em conflito com outra
                        operacao2[i].isEscolhido=1;
                        operacao2[i].Tinicio=maiorTempoConflito;
                        operacao2[i].Tfinal=maiorTempoConflito+operacao2[i].Tempo;
                        if(operacao2[i].Tfinal>makespan)
                            makespan=operacao2[i].Tfinal;
                 }//fim for geral

                 #ifdef Ver_Makespans
                    printf("Makespan Conflito encontrado: %f\n",makespan);
                 #endif

                 if(makespan<MakeSpan){
                            AtualizaAgendamento();
                            delta = MakeSpan - makespan;
                            MakeSpan = makespan;
                            #ifdef Ver_Makespans
                                printf("***** A solucao melhorou em %.2f unidades de tempo!*****\n",delta);
                            #endif
                  }
            Troca1++;
            cont++;
        }
}

void Construcao_Tempo(){//Pertuba as operações selecionando primeiramente as operações com menor tempo de duração

    int i;
    int maiorTempoConflito;//variável que armazena o maior tempo de termino de uma atividade em conflito com outra
    float makespan, delta;
    int Troca1 = 0; //Variável auxiliar para trocar posições da solução nas buscas locais

    //QuickSort_Tempo(operacao2, 0, MAX_VERTICES - 1) ; //Ordena o vetor de acordo com o tempo de operação

    while(Troca1<MAX_VERTICES/FATOR_DE_BUSCA && cont<MAX_ITERATION){
                makespan=0;//Reseta o makespan
                ResetaOperacoes();//As operações não foram escolhidas ainda
                operacao2[Troca1].Tinicio = 0;//Altera a operação que deverá iniciar no tempo zero
                operacao2[Troca1].Tfinal = operacao2[Troca1].Tempo;//agenda o tempo de fim
                operacao2[Troca1].isEscolhido = 1;
                for(i=0;i<MAX_VERTICES;i++){//reagenda todas as operações apartir da alteração acima
                        if(operacao2[i].isEscolhido)
                            continue;
                        maiorTempoConflito=0;//reseta o maior tempo de termino de uma atividade em conflito com outra
                        for(int j=0;j<MAX_VERTICES;j++)
                            if(operacao2[j].isEscolhido)//checa as atividades já agendadas
                                if(VerificaConflito(operacao[i].index,operacao[j].index))//a operação tem conflito com uma atividade selecionada
                                    if(operacao2[j].Tfinal>maiorTempoConflito)//a operação tem conflito com outra atividade que dura mais tempo
                                        maiorTempoConflito=operacao2[j].Tfinal;//maior tempo de termino de uma atividade em conflito com outra
                        operacao2[i].isEscolhido=1;
                        operacao2[i].Tinicio=maiorTempoConflito;
                        operacao2[i].Tfinal=maiorTempoConflito+operacao2[i].Tempo;
                        if(operacao2[i].Tfinal>makespan)
                            makespan=operacao2[i].Tfinal;
                 }//fim for geral

                 #ifdef Ver_Makespans
                    printf("Makespan Tempo encontrado: %f\n",makespan);
                 #endif

                 if(makespan<MakeSpan){
                            AtualizaAgendamento();
                            delta = MakeSpan - makespan;
                            MakeSpan = makespan;
                            #ifdef Ver_Makespans
                                printf("***** A solucao melhorou em %.2f unidades de tempo!*****\n",delta);
                            #endif
                  }
            Troca1++;
            cont++;
        }
}

void Construcao_Tempo_Conflito(){//Pertuba as operações selecionando primeiramente as operações com menor tempo de duração e menor número de conflitos

    int i;
    int maiorTempoConflito;//variável que armazena o maior tempo de termino de uma atividade em conflito com outra
    float makespan, delta;
    int Troca1 = 0; //Variável auxiliar para trocar posições da solução nas buscas locais

    QuickSort_Tempo_Conflito(operacao2, 0, MAX_VERTICES - 1) ; //Ordena o vetor de acordo com o tempo de operação

    while(Troca1<MAX_VERTICES/FATOR_DE_BUSCA && cont<MAX_ITERATION){
                makespan=0;//Reseta o makespan
                ResetaOperacoes();//As operações não foram escolhidas ainda
                operacao2[Troca1].Tinicio = 0;//Altera a operação que deverá iniciar no tempo zero
                operacao2[Troca1].Tfinal = operacao2[Troca1].Tempo;//agenda o tempo de fim
                operacao2[Troca1].isEscolhido = 1;
                for(i=0;i<MAX_VERTICES;i++){//reagenda todas as operações apartir da alteração acima
                        if(operacao2[i].isEscolhido)
                            continue;
                        maiorTempoConflito=0;//reseta o maior tempo de termino de uma atividade em conflito com outra
                        for(int j=0;j<MAX_VERTICES;j++)
                            if(operacao2[j].isEscolhido)//checa as atividades já agendadas
                                if(VerificaConflito(operacao[i].index,operacao[j].index))//a operação tem conflito com uma atividade selecionada
                                    if(operacao2[j].Tfinal>maiorTempoConflito)//a operação tem conflito com outra atividade que dura mais tempo
                                        maiorTempoConflito=operacao2[j].Tfinal;//maior tempo de termino de uma atividade em conflito com outra
                        operacao2[i].isEscolhido=1;
                        operacao2[i].Tinicio=maiorTempoConflito;
                        operacao2[i].Tfinal=maiorTempoConflito+operacao2[i].Tempo;
                        if(operacao2[i].Tfinal>makespan)
                            makespan=operacao2[i].Tfinal;
                 }//fim for geral

                 #ifdef Ver_Makespans
                    printf("Makespan TempoConflito encontrado: %f\n",makespan);
                 #endif

                 if(makespan<MakeSpan){
                            AtualizaAgendamento();
                            delta = MakeSpan - makespan;
                            MakeSpan = makespan;
                            #ifdef Ver_Makespans
                                printf("***** A solucao melhorou em %.2f unidades de tempo!*****\n",delta);
                            #endif
                  }
            Troca1++;
            cont++;
        }
}

void Grasp(){

    gettimeofday(&start, NULL);//Inicia contagem de tempo
    cont = 0;// reseta o contador de iterações

    Construcao();//Gera uma solução inicial aleatória, dispersa e com boa localidade

    switch(Busca){//Realiza as buscas locais
        case 1 :
            BuscaLocal1();//Embaralha as rotas e reconstrói
            break;
        case 2 :
            BuscaLocal2();//Embaralha as rotas e reconstrói
            break;
         case 3 :
            BuscaLocal3();//Embaralha as rotas e reconstrói
            break;
        default :
            break;
    }
    gettimeofday(&end, NULL);//Encerra contagem de tempo

    ///* Print dos resultados*/
    printf("\n*********************************************\n\n");
    printf("Agendamento concluido com sucesso!\n");
    printf("Makespan final = %.2f\n",MakeSpan);
    printf("Tempo de execucao = ");
    printf("%ld segundos\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec))/1000000);
    printf("\n*********************************************\n");

    //Libera a memória utilizada
    free(Matriz_ADJ);
    free(operacao2);
}

void Permutacao1(int n){//Permuta o vetor gerando uma permutação com probabilidade de 1/n
    int elementos_restantes = n;
    Operacao aux ;
    int valorAleatorio;
    while (elementos_restantes > 0){
        valorAleatorio = rand() % (elementos_restantes);
        //Troca as duas posições aleatorias valorAleatorio1 com valorAleatorio2
        aux  = operacao2[ valorAleatorio];
        operacao2[ valorAleatorio] = operacao2[elementos_restantes-1];
        operacao2[elementos_restantes-1] = aux ;
        elementos_restantes--;
    }
}

void Permutacao2(int n){//Permuta o vetor trocando duas posições aleatórias
    Operacao aux ;
    int valorAleatorio1, valorAleatorio2;
    valorAleatorio1 = rand() % n;
    valorAleatorio2 = rand() % n;
    aux  = operacao2[valorAleatorio1];
    operacao2[valorAleatorio1] = operacao2[valorAleatorio2];
    operacao2[valorAleatorio2] = aux ;
}

void Permutacao3(int n){//Permuta o vetor trocando duas posições aleatórias e a primeira posição com uma aleatória
   Operacao aux ;
    int valorAleatorio1, valorAleatorio2, valorAleatorio3;
    valorAleatorio1 = rand() % n;
    valorAleatorio2 = rand() % n;
    valorAleatorio3 = rand() % n;
    //Troca as duas posições aleatorias valorAleatorio1 com valorAleatorio2
    aux  = operacao2[valorAleatorio1];
    operacao2[valorAleatorio1] = operacao2[valorAleatorio2];
    operacao2[valorAleatorio2] = aux ;
    //Troca as duas posições aleatorias 0 com 3
    aux  = operacao2[valorAleatorio3];
    operacao2[valorAleatorio3] = operacao2[0];
    operacao2[0] = aux ;
}

void Permutacao4(int n){//Permuta o vetor trocando as posições pares e ímpares entre si
   Operacao aux ;
    //Troca as duas posições
    if(n<MAX_VERTICES - 3){
        aux  = operacao2[n];
        operacao2[n] = operacao2[n+2];
        operacao2[n+2] = aux ;
    }
}

void BuscaGlobal(){//Perturba completamente a solução gerando uma nova permutação de rotas
    float makespan, delta;
    #ifdef Ver_Makespans
        printf("\nbusca Global...\n\n");
    #endif
     Permutacao1(MAX_VERTICES);
     ResetaOperacoes();
     makespan = ConstrucaoSimples();
     if(makespan<MakeSpan){
        AtualizaAgendamento();
        delta = MakeSpan - makespan;
        MakeSpan = makespan;
        #ifdef Ver_Makespans
            printf("***** A solucao melhorou em %.2f unidades de tempo!*****\n",delta);
        #endif
      }
     cont++;
}//fim Busca local

void BuscaLocal1(){//Permuta o vetor trocando duas posições aleatórias
   float makespan, delta;
    printf("\nIniciando busca local 1...\n\n");
    while(cont<MAX_ITERATION){
         if(cont%(MAX_ITERATION/FATOR_GLOBAL)==0)
             BuscaGlobal();
         else
             Permutacao2(MAX_VERTICES);
         ResetaOperacoes();
         makespan = ConstrucaoSimples();
         if(makespan<MakeSpan){
            AtualizaAgendamento();
            delta = MakeSpan - makespan;
            MakeSpan = makespan;
            #ifdef Ver_Makespans
                printf("***** A solucao melhorou em %.2f unidades de tempo!*****\n",delta);
            #endif
          }
         cont++;
    }
}//fim Busca local

void BuscaLocal2(){//Permuta o vetor trocando duas posições aleatórias e a primeira posição com uma aleatória
  float makespan, delta;
    printf("\nIniciando busca local 2...\n\n");
    while(cont<MAX_ITERATION){
        if(cont%(MAX_ITERATION/FATOR_GLOBAL)==0)
             BuscaGlobal();
         else
             Permutacao3(MAX_VERTICES);
         ResetaOperacoes();
         makespan = ConstrucaoSimples();
         if(makespan<MakeSpan){
            AtualizaAgendamento();
            delta = MakeSpan - makespan;
            MakeSpan = makespan;
            #ifdef Ver_Makespans
                printf("***** A solucao melhorou em %.2f unidades de tempo!*****\n",delta);
            #endif
          }
         cont++;
    }
}//fim Busca local

void BuscaLocal3(){//Permuta o vetor trocando duas posições aleatórias e a primeira posição com uma aleatória
  float makespan, delta;
  int n = 0;
    printf("\nIniciando busca local 3...\n\n");
    while(cont<MAX_ITERATION){
        if(n >= MAX_VERTICES - 3 || cont%(MAX_ITERATION/FATOR_GLOBAL)==0){
             BuscaGlobal();
             n = 0;
        }
         else
             Permutacao4(n);
         ResetaOperacoes();
         makespan = ConstrucaoSimples();
         if(makespan<MakeSpan){
            AtualizaAgendamento();
            delta = MakeSpan - makespan;
            MakeSpan = makespan;
            #ifdef Ver_Makespans
                printf("***** A solucao melhorou em %.2f unidades de tempo!*****\n",delta);
            #endif
          }
         cont++;
         n++;
    }
}//fim Busca local

void GerarArquivo(){//Gera o arquivo de solução com o nome "Solução_Vertices_Densidade.txt"
    //Gera um arquivo com o tempo de inicio e termino de todas as atividades

    printf("\nSalvando arquivo...\n");

     char aux1[10], aux2[10];
     char solucao[100], grafico[100] ;

     Operacao * aux;
     aux = (Operacao *) malloc(sizeof(Operacao) * MAX_VERTICES);

      for(int i=0;i<MAX_VERTICES;i++)//Reorganiza as operações na ordem dada
            aux[operacao[i].index] = operacao[i];

      sprintf(aux1, "%i", MAX_VERTICES);
      sprintf(aux2, "%.3f", DENSIDADE);

      //Gera o nome dos arquivos
      strcpy(solucao,PASTA);
      strcpy(grafico,PASTA);
      strcat(solucao,"Solucao_");
      strcat(grafico,"ZGrafico_");
      strcat(solucao,aux1);
      strcat(grafico,aux1);
      strcat(solucao,"_");
      strcat(grafico,"_");
      strcat(solucao,aux2);
      strcat(grafico,aux2);
      strcat(solucao,"%.txt");
      strcat(grafico,"%.txt");

     fs = fopen(solucao,"w");
                if(fs==NULL){
                    printf("\nErro ao criar Arquivo Solucao\n");
                exit(500);
     }

     fg = fopen(grafico,"w");
                if(fg==NULL){
                    printf("\nErro ao criar Arquivo Grafico\n");
                exit(501);
     }

     for(int i=0;i<MAX_VERTICES;i++)
            fprintf(fs,"rota: %02d Tinicio = %.1f Tfim = %.1f\n",i,aux[i].Tinicio,aux[i].Tfinal);
    fprintf(fs,"\n\nMakespan = %f\n",MakeSpan);

    for(int i=0;i<MAX_VERTICES;i++)
            fprintf(fg,"%d %.2f %.2f\n",i,aux[i].Tinicio,aux[i].Tfinal);

    free(operacao);
    fclose(fs);
    fclose(fg);

    printf("Arquivos salvos com sucesso!\n");
    printf("\n*********************************************\n");
}

void LerArquivo(){//Abre o arquivo com a matriz de adjacencias e preenche os campos globais

    int linha, aux;
    char aux1[10], aux2[10], arq[100];

    //printf("Digite o nome do arquivo de instancia: ");
    //scanf("%s",arquivo);
    printf("Digite o numero de vertices: ");
    scanf("%d",&MAX_VERTICES);
    printf("Digite a porcentagem de conflitos: ");
    scanf("%f",&DENSIDADE);

     //Transforma os numeros em strings
    sprintf(aux1, "%i", MAX_VERTICES);
    sprintf(aux2, "%.3f", DENSIDADE);

    //Gera o nome da pasta
    strcpy(PASTA,"Instancia_");
    strcat(PASTA,aux1);
    strcat(PASTA,"_");
    strcat(PASTA,aux2);
    strcpy(arq,PASTA); //arq = "Instancia_V_D"
    strcat(PASTA,"%\\"); //PASTA = "Instancia_V_D%\"

    //Gera o nome da Instancia
    strcpy(nomeInstancia,PASTA);
    strcat(nomeInstancia,arq);
    strcat(nomeInstancia,"%");
    strcat(nomeInstancia,".txt"); //PASTA = "Instancia_V_D%\Instancia_V_D%.txt"

    //Instancia as matrizes e vetores
    Matriz_ADJ = (char **) malloc(sizeof(char *) * MAX_VERTICES);
    for(int i = 0; i < MAX_VERTICES; i++)
        Matriz_ADJ[i] = (char *) malloc(sizeof(char) * MAX_VERTICES);
    operacao = (Operacao *) malloc(sizeof(Operacao) * MAX_VERTICES);
    operacao2 = (Operacao *) malloc(sizeof(Operacao) * MAX_VERTICES);

    printf("nomeInstancia: %s\n",nomeInstancia);

    //Abre o arquivo
    fa = fopen(nomeInstancia,"r");
        if(fa==NULL){
            printf("\nErro ao abrir Arquivo\n");
            exit(404);
        }

        //Zera a matriz de adjacencia
        for(int i=0;i<MAX_VERTICES;i++)
            for(int j=0;j<MAX_VERTICES;j++)
                Matriz_ADJ[i][j]=0;  //zera a matriz de adjacencia

        while((fscanf(fa,"%d",&linha))!=EOF){//lê o numero da linha da matriz
                fscanf(fa,"%f",&operacao[linha].Tempo);//lê o tempo daquela atividade
                operacao[linha].isEscolhido=0;//registra que a operação ainda não foi analisada
                operacao[linha].index=linha;//registra o identificador da operação
                fscanf(fa,"%d",&operacao[linha].Conflitos);//lê o numero de conflitos da operação
                //printf("%d %.2f",linha,operacao[linha].Tempo);
                for(int i=0;i<operacao[linha].Conflitos;i++){
                    fscanf(fa,"%d",&aux);//lê todos os conflitos e os salva no vetor
                    Matriz_ADJ[linha][aux]=1;
                    //printf(" %d",aux);
                }
                //printf("\n");
        }

            /*//_________________________________________________________________________
            printf("\n Matriz\n");//começa a imprimir a matriz de adjacencia
            for(int i=0;i<MAX_VERTICES;i++){
                    for(int j=0;j<MAX_VERTICES;j++){
                        printf("%d ",Matriz_ADJ[i][j]);//printa cada elemento da matriz
                    }
                    printf("\n");
                }
            //_________________________________________________________________________*/
            fclose(fa);
}

int main(){ //main Principal
            srand(time(NULL));
            LerArquivo();
            //Construcao();
            Grasp();
            GerarArquivo();
            system("PAUSE");
            return 0;
}
