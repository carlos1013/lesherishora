#include <stdio.h>
#include <stdlib.h>

typedef struct no{
    int folha,nChaves;
    int *chaves;
    char **filhos;
}TNO;

TNO *mapear(char* nome){
    FILE *fp = fopen(nome,"rb");
    if (!fp) exit(1);
}

void desmapear(TNO *no){
}

void salvar(char* nome,TNO *no){
}

char *buscar (char *nome,int num){
    TNO *atual = mapear(nome);
    int x;
    for (x=0;x<atual->nChaves;x++){
        if (atual->chaves[x]>num) break;
        if (atual->chaves[x]==num){
            char n[] = malloc(sizeof(char[30]));

        }
    }
}


int main(){
    char *v = buscar();
    if (v){
        printf("%s\n",v);
    }
    return 0;
}
