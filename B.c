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
        printf("%s\n", novo->filhos[i]);
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

char *buscar (char *nome,int num){
    printf("Tentando abrir %s\n", nome);
    TNO *atual = mapear(nome);
    char * resp;
    int x;
    for (x=0;x<atual->nChaves;x++){
        if (atual->chaves[x]>num) break;
        if (atual->chaves[x]==num){
            char *n = (char *) malloc(sizeof(char)*TAM_NOME_ARQUIVO);
            strcpy(n,nome);
            return n;
        }
    }
    if(atual->folha) resp = NULL;
    else{
        resp = buscar(atual->filhos[x],num);
    }
    desmapear(atual);
    return resp;
}

char *buscar2 (char *nome,int num){
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

    //verificar se é folha
    int iNomeFilhos = sizeof(int)*(1 + nChaves);
    fseek(arq, 0, SEEK_END);
    if (iNomeFilhos == ftell(arq)) return NULL;

    fseek(arq, iNomeFilhos + i*sizeof(char)*TAM_NOME_ARQUIVO, SEEK_SET);

    char *n = (char *) malloc(sizeof(char)*TAM_NOME_ARQUIVO);
    fread(n, sizeof(char)*TAM_NOME_ARQUIVO, 1, arq);

    fclose(arq);
    return buscar(n,num);
}

void remocao(char *nome,int num,int t){
    TNO *atual = mapear(nome);

}


/*void remover(char *nome,int num,int t){
    char *b = buscar(nome,num);
    if (b) {
        remocao(nome,num,t);
        free(b);
    }
}*/

int main(){
    char *n = buscar("arq.dat",13);
    if (!n)
        printf("Não encontrou");
    else
        printf("%s",n);

    return 0;
}
