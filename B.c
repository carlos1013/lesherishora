#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_NOME_ARQUIVO 30
#define MAX_CHAVES(t) ((2 * t) -1)
#define MAX_FILHOS(t) (2 * t)


typedef struct no{
    int folha,nChaves;
    int *chaves;
    char **filhos;
}TNO;

TNO *mapear(char* nome);
void desmapear(TNO *no);
void salvar(char* nome,TNO *no);
char *buscar (char *nome, int num);

TNO* mapear (char* nome) {
    FILE *arq = fopen(nome, "rb");
    if(!arq) exit(1);

    TNO *novo = (TNO*)malloc(sizeof(TNO));

    fread(&novo->nChaves, sizeof(int), 1, arq);
    novo->chaves = (int*)malloc(sizeof(int)*novo->nChaves);
    fread(novo->chaves, sizeof(int), novo->nChaves, arq);

    int iNomeFilhos = sizeof(int)*(1 + novo->nChaves);
    fseek(arq, 0, SEEK_END);
    if (iNomeFilhos == ftell(arq)) {
        novo->folha = 1;
        return novo;
    }
    novo->folha = 0;
    fseek(arq, iNomeFilhos, SEEK_SET);
    novo->filhos = (char**)malloc((novo->nChaves + 1)*sizeof(char*));
    int i;
    for(i=0; i< novo->nChaves + 1; i++) {
        novo->filhos[i] = (char*)malloc(sizeof(char)*TAM_NOME_ARQUIVO);
        fread(novo->filhos[i], sizeof(char)*TAM_NOME_ARQUIVO, 1, arq);
    }

    fclose(arq);
    return novo;
}

void desmapear(TNO *no){
    int i;
    for(i=0; i< no->nChaves + 1; i++)
        free(no->filhos[i]);
    free(no->filhos);
    free(no->chaves);
    free(no);
}

void salvar(char* nome,TNO *no){
    FILE *fp = fopen(nome,"wb+");
}

void liberar(char *nome){
    FILE *fp = fopen(nome,"rb");
    if (!fp) exit(1);
    int n_chaves,r,x=0;
    fread(&n_chaves,sizeof(int),1,fp);
    char filhos[n_chaves+1][TAM_NOME_ARQUIVO];
    fseek(fp,sizeof(int)+(n_chaves*sizeof(int)),SEEK_SET);
    while(1){
        r = fread(filhos[x],sizeof(char)*TAM_NOME_ARQUIVO,1,fp);
        if (r==0) break;
        x++;
    }
    fclose(fp);
    remove(nome);
    if (!x) return;
    for (x=0;x<n_chaves+1;x++){
        liberar(filhos[x]);
    }
}


char *buscar (char *nome,int num){
    FILE *arq = fopen(nome, "rb");
    if (!arq) exit(1);
    int nChaves;
    fread(&nChaves, sizeof(int), 1, arq);

    int i, chaveAtual;
    for(i=0; i<nChaves; i++) {
        fread(&chaveAtual, sizeof(int), 1, arq);
        if (chaveAtual > num) break;
        if (chaveAtual == num) {
            char *n = (char *) malloc(sizeof(char)*TAM_NOME_ARQUIVO);
            strcpy(n,nome);
            return n;
        }
    }
    int iNomeFilhos = sizeof(int)*(1 + nChaves);
    fseek(arq, 0, SEEK_END);
    if (iNomeFilhos == ftell(arq)) return NULL;

    fseek(arq, iNomeFilhos + i*sizeof(char)*TAM_NOME_ARQUIVO, SEEK_SET);

    char n[TAM_NOME_ARQUIVO];
    fread(n, sizeof(char), TAM_NOME_ARQUIVO, arq);

    fclose(arq);
    return buscar(n,num);
}

void remocao(char *nome,int num,int t){
    TNO *atual = mapear(nome);

}


void remover(char *nome,int num,int t){
    char *b = buscar(nome,num);
    if (b) {
        remocao(nome,num,t);
        free(b);
    }
}

char* inserir (char* nome, int n, int t) {
    /*if (!buscar(nome, n)) {
        TNO* atual = mapear(nome);
        if(atual->folha) {
            if( atual->nChaves < MAX_CHAVES(t) )
               inserirNaFolha(nome, n);
        }

    }*/
}

void criar(char* nome) {
    FILE *arq = fopen(nome,"wb");
    if(!arq) exit(1);
    int i = 0;
    fwrite(&i, sizeof(int), 1, arq);
    fclose(arq);
}

void imprimir(char *nome){
}

int main(){
    char nome[TAM_NOME_ARQUIVO];
    int t;
    printf("Insira o t da arvore:\n");
    scanf("%d",&t);
    printf("Insira o nome do arquivo:\n");
    scanf("%s",nome);
    setbuf(stdin,NULL);
    //criar(nome);
    int op,num;
    while(1) {
        printf("Operacoes: \n1. Inserir \n2. Remover \n3. Buscar \n4. Imprimir \n0. Sair \n");
        scanf("%d", &op);
        if(op == 1) {
            scanf("%d", &num);
            if(num > 0) {int alfa=0;}
                //nome = inserir(nome,num,t);
        }
        else if (op == 2) {
            scanf("%d", &num);
            if(num > 0)
                remover(nome,num,t);
        }
        else if (op == 3) {
            printf("Digite o valor a ser buscado:\n");
            scanf("%d", &num);
            if(num > 0){
                char *local = buscar(nome,num);
                if (local){
                    printf("O valor esta no arquivo %s\n",local);
                    free(local);
                }
                else{
                    printf("O valor nao foi encontrado\n");
                }
            }
        }
        else if (op == 4) imprimir(nome);

        else if (op == 0){
            liberar(nome);
            return 0;
        }

        else printf("Operacao invalida\n");

    }
}
