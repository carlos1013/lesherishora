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

TNO *mapear(char* nome){
    FILE *fp = fopen(nome,"rb");
    if (!fp) exit(1);
}

void desmapear(TNO *no){
}

void salvar(char* nome,TNO *no){
    FILE *fp = fopen(nome,"wb+");


}

char *buscar (char *nome,int num){
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

void remocao(char *nome,int num,int t){
    TNO *atual = mapear(nome);




}


void remover(char *nome,int num,int t){
    char *b = buscar(nome,num);
    if (b) {
        remocao(nome,num,int t);
        free(b);
    }
}

int main(){
    char *v = buscar("oi.txt",2);
    if (v){
        printf("%s\n",v);
    }
    return 0;
}
