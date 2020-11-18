#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<windows.h>

    int MAX_VERTICES;
    FILE *fv, *fi, *fa;//Arquivos de saida
    int **Matriz;//Matriz de  adjacencia
    float densidade, DENSIDADE;//Densidade total gerada
    char arquivo[100], arqv[100], arqi[100], arqa[100];

    int MaxArestas, arestas;

    float Random(){
        float i = rand()%10000;
        return (i/10000);
    }

    void MostrarMatriz(int arestas){
                int soma=0;
                /*for(int i=0;i<MAX_VERTICES;i++){
                    for(int j=i;j<MAX_VERTICES;j++){
                        printf("%d ",Matriz[i][j]);//printa cada elemento da matriz
                    }
                    printf("\n");
                }*/
                densidade = (((float)arestas)/(MAX_VERTICES*(MAX_VERTICES-1)/2))*100;//calcula a densidade final obtida
                printf("\n___________________________________\n");
                printf("\n Limite = %d",(MAX_VERTICES*(MAX_VERTICES-1)/2));//Quantidadede arestas se o grafo fosse completo
                printf("\n Conflitos = %d",arestas);
                printf("\n Densidade = %.3f %%\n",densidade);
                printf("___________________________________\n");
    }

    void GravaArquivoAdjacencia(){
              for(int i=0;i<MAX_VERTICES;i++){
                    for(int j=0;j<MAX_VERTICES;j++){
                        fprintf(fa,"%d ",Matriz[i][j]);
                    }
                    fprintf(fa,"\n");
                }
                fclose(fa);
    }

    void GravaArquivoVizualizavel(){
              fprintf(fv,"graph [ \n");
              for(int i=0;i<MAX_VERTICES;i++){
                    fprintf(fv,"\tnode\n\t[\n\t id %4d\n\t]\n",i);
                    for(int j=i;j<MAX_VERTICES;j++){
                      if(Matriz[i][j]==1)
                       fprintf(fv,"\tedge\n\t[\n\t source %4d\n\t target %4d\n\t]\n",i,j);
                    }
                }
                fprintf(fv,"]");
                fclose(fv);
    }

    void GravaArquivoInstancias(){
              float tempo=0;
              int vetor[MAX_VERTICES];

             //Conta quantos conflitos tem cada atividade e salva em "vetor"
              for(int i=0;i<MAX_VERTICES;i++){
                    vetor[i]=0;
                    for(int j=0;j<MAX_VERTICES;j++){
                        if(Matriz[i][j]==1)
                            vetor[i]++;
                    }
              }

              for(int i=0;i<MAX_VERTICES;i++){//Gera um tempo aleatório de 0 a 12
                    tempo = rand()%12+1;
                    if(tempo>12)
                        tempo=12;

                    fprintf(fi,"%d %.2f %d ",i,tempo,vetor[i]); //Printa a atividade, tempo e n de conflitos
                    for(int j=0;j<MAX_VERTICES;j++){//printa os conflitos
                          if(Matriz[i][j]==1){
                                fprintf(fi,"%d ",j);
                          }
                    }
                     fprintf(fi,"\n");
                }
                /*printf("Vetor\n");
                    for(int i=0;i<MAX_VERTICES;i++){
                        printf("%d ",vetor[i]);
                    }*/
             fclose(fi);
    }

int main(){

            srand(time(NULL));
            arestas=0;
            char aux1[10], aux2[10], pasta[100] = "Instancia_", comando[100] = "mkdir ";

            printf("Digite o numero de vertices desejados: ");
            scanf("%d",&MAX_VERTICES);
            printf("Digite a densidade desejada: ");
            scanf("%f",&DENSIDADE);

            //Transforma os numeros em strings
             sprintf(aux1, "%i", MAX_VERTICES);
             sprintf(aux2, "%.3f", DENSIDADE);

            DENSIDADE/=100;

            //Aloca a matriz de adjacencia
            Matriz = (int **) malloc(sizeof(int *) * MAX_VERTICES);
            for(int i = 0; i < MAX_VERTICES; i++)
            Matriz[i] = (int *) malloc(sizeof(int) * MAX_VERTICES);

            MaxArestas = (int)((MAX_VERTICES*(MAX_VERTICES-1)/2)*(DENSIDADE));
            printf("\n Quantidade de arestas desejada = %d\n",MaxArestas);

            //Gera os nomes da pasta e dos arquivos
            strcat(pasta,aux1);
            strcat(pasta,"_");
            strcat(pasta,aux2);
            strcat(pasta,"%");
            strcpy(arqv,pasta);
            strcpy(arqi,pasta);
            strcpy(arqa,pasta);
            strcat(arqv,"\\Grafo");
            strcat(arqi,"\\Instancia");
            strcat(arqa,"\\MatrizAdjacencia");
            strcat(arqv,"_");
            strcat(arqi,"_");
            strcat(arqa,"_");
            strcat(arqv,aux1);
            strcat(arqi,aux1);
            strcat(arqa,aux1);
            strcat(arqv,"_");
            strcat(arqi,"_");
            strcat(arqa,"_");
            strcat(arqv,aux2);
            strcat(arqi,aux2);
            strcat(arqa,aux2);
            strcat(arqv,"%.gml");
            strcat(arqi,"%.txt");
            strcat(arqa,"%.txt");

             strcat(comando,pasta);
             system (comando);//Cria a pasta

            //Cria o arquivo para gerar o grafo vizualizavel
            fv = fopen(arqv,"w");
                if(fv==NULL){
                    printf("\nErro ao criar Arquivo\n");
                return 0;
            }
            //Cria o arquivo completo com as instancias
            fi = fopen(arqi,"w");
                if(fi==NULL){
                    printf("\nErro ao criar Arquivo\n");
                return 0;
            }
            //Cria o arquivo com a matriz de adjacencias
            fa = fopen(arqa,"w");
                if(fa==NULL){
                    printf("\nErro ao criar Arquivo\n");
                return 0;
            }

             for(int i=0;i<MAX_VERTICES;i++){
                for(int j=0;j<MAX_VERTICES;j++){
                    Matriz[i][j]=0;
                }
            }

            //Aqui se preenche a matriz de adjacencia
            for(int i=0;i<MAX_VERTICES;i++)
                for(int j=i;j<MAX_VERTICES;j++)
                    if(i!=j)
                        if(Random()<DENSIDADE){
                            Matriz[i][j]=1;
                            Matriz[j][i]=1;
                            arestas++;
                        }

            GravaArquivoVizualizavel();
            GravaArquivoAdjacencia();
            GravaArquivoInstancias();
            MostrarMatriz(arestas);

            system("PAUSE");
            return 1;
}

