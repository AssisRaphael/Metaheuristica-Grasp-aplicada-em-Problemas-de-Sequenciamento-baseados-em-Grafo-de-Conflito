#include <time.h>
#include <sys/time.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<windows.h>

typedef struct operacao{
        float Tinicio, Tfinal, Tempo;
        int Conflitos, isEscolhido, index;
        int * op;
}Operacao;//struct da operação do porto

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

int VerificaOperacoes(Operacao* op,int n){
    for(int i=0;i<MAX_VERTICES;i++)
        if(!op[i].isEscolhido)
            return 0;
    return 1;
}

float ConstrucaoSimples(){//constrói um solução válida
    //Realiza um construção extremamente simples, sem procurar melhores opções
    int maiorTempoConflito;//variável que armazena o maior tempo de termino de uma atividade em conflito com outra
    float makespan=0;
    //Função gulosa que seleciona quais operações devem ser incluidas em cada passo
     for(int i=0;i<MAX_VERTICES;i++){//percorre todas as operações
           if(operacao2[i].isEscolhido)
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
     printf("Makespan construcao simples = %f\n",makespan);
     return makespan;
}//fim construção simples

void Otima(){//Encontra a solução ótima do problema    ****Errada
    int i, j;
    float makespan;
    gettimeofday(&start, NULL);//Inicia contagem de tempo
    for(i=0;i<MAX_VERTICES;i++)//operacao2 recebe uma copia do vetor operacao
        operacao2[i]=operacao[i];
    MakeSpan = INFINITE;
    for(i=0;i<MAX_VERTICES;i++)//Percorre todos os vértices
        for(j=0;j<operacao[i].Conflitos;j++){//Percorre todos os conflitos desse vértice
                ResetaOperacoes();
                //Coloca a operacao[i] como inicial
                operacao[i].Tinicio = 0;
                operacao[i].Tfinal = operacao[i].Tempo;
                operacao[i].isEscolhido = 1;
                //Coloca todas as operações em conflito como próximas
                operacao[j].Tinicio = operacao[i].Tfinal;
                operacao[j].Tfinal = operacao[i].Tfinal + operacao[j].Tempo;
                operacao[j].isEscolhido = 1;
                makespan = ConstrucaoSimples();
                 if(makespan<MakeSpan)
                    AtualizaAgendamento();
        }
    gettimeofday(&end, NULL);//Encerra contagem de tempo
    printf("\n*********************************************\n\n");
    printf("Agendamento concluido com sucesso!\n");
    printf("Tempo de execucao = ");
    printf("%ld mksegundos\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
    printf("Makespan final = %f\n",MakeSpan);
    printf("Iteracoes = %d\n",cont);
    printf("\n*********************************************\n");
    //Libera a memória utilizada
    free(Matriz_ADJ);
    free(operacao2);
}

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
      strcat(solucao,"SolucaoOtima_");
      strcat(grafico,"ZGraficoOtimo_");
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
    free(aux);
    fclose(fs);
    fclose(fg);

    printf("Arquivos salvos com sucesso!\n");
    printf("\n*********************************************\n");
}

void LerArquivo(){//Abre o arquivo com a matriz de adjacencias e preenche os campos globais

    int linha, aux;
    char aux1[10], aux2[10], arq[100];

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
                operacao[linha].op = NULL;//reseta o vetor de conflitos
                fscanf(fa,"%f",&operacao[linha].Tempo);//lê o tempo daquela atividade
                operacao[linha].isEscolhido=0;//registra que a operação ainda não foi analisada
                operacao[linha].index=linha;//registra o identificador da operação
                fscanf(fa,"%d",&operacao[linha].Conflitos);//lê o numero de conflitos da operação
                if(operacao[linha].Conflitos)
                    operacao[linha].op = malloc(sizeof(int)*operacao[linha].Conflitos);//Aloca o vetor de conflitos
                //printf("%d %.2f",linha,operacao[linha].Tempo);
                for(int i=0;i<operacao[linha].Conflitos;i++){
                    fscanf(fa,"%d",&aux);//lê todos os conflitos e os salva no vetor
                    operacao[linha].op[i] = aux;
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
    Otima();
    GerarArquivo();
    system("PAUSE");
    return 0;
}
