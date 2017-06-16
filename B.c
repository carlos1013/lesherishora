#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_NOME_ARQUIVO 15
#define MAX_CHAVES(t) ((2 * t) -1)
#define MAX_FILHOS(t) (2 * t)


typedef struct no{
    int folha,nChaves;
    int *chaves;
    char **filhos;
}TNO;

void criar(char *nome,int num);
void criar_nome(char* nome, int i);
void inicializa(char *nome);
TNO *mapear(char* nome);
void desmapear(TNO *no);
void salvar(char* nome,TNO *no);
void liberar(char *nome);
char *buscar (char *nome, int num);
void ins_aux (char* nome,int num,int t);
void inserir (char* nome, int num, int t);
void imprimir(char *nome, int n);

int t;

void criar(char *nome,int num){
    FILE *fp = fopen(nome,"wb");
    if (!fp) exit(1);
    int x = 1;
    fwrite(&x,sizeof(int),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);
}

void criar_nome(char* nome, int i)  {
    itoa(i, nome, 10);
    strcat(nome, ".dat");
}

void inicializa(char *nome){
    FILE *fp = fopen(nome,"wb");
    if (!fp) exit(1);
    fclose(fp);
}

TNO* mapear (char* nome) {
    FILE *arq = fopen(nome, "rb+");
    if(!arq){
        printf("erro ao tentar mapear %s\n", nome);
        exit(1);
    }
    int r;

    TNO *novo = (TNO*)malloc(sizeof(TNO));
    r = fread(&novo->nChaves, sizeof(int), 1, arq);
    if (!r){
        free(novo);
        return NULL;
    }

    novo->chaves = (int*)malloc(sizeof(int)*MAX_CHAVES(t));
    r = fread(novo->chaves, sizeof(int), novo->nChaves, arq);

    if (!r){
        free(novo->chaves);
        free(novo);
        return NULL;
    }

    int iNomeFilhos = sizeof(int)*(1 + novo->nChaves);
    fseek(arq, 0, SEEK_END);
    if (iNomeFilhos == ftell(arq)) {
        novo->folha = 1;
        return novo;
    }
    novo->folha = 0;
    fseek(arq, iNomeFilhos, SEEK_SET);
    novo->filhos = (char**)malloc(MAX_FILHOS(t)*sizeof(char*));
    int i;
    for(i=0; i< MAX_FILHOS(t); i++) {
        novo->filhos[i] = (char*)malloc(sizeof(char)*TAM_NOME_ARQUIVO);
        fread(novo->filhos[i], sizeof(char)*TAM_NOME_ARQUIVO, 1, arq);
    }

    fclose(arq);
    return novo;
}

void desmapear(TNO *no){
    if (!no->folha){
        int i;
        for(i=0; i < MAX_FILHOS(t); i++)
            free(no->filhos[i]);
        free(no->filhos);
    }
    free(no->chaves);
    free(no);
}

void salvar(char* nome,TNO *no){
    FILE* arq = fopen(nome, "wb");
    if (!arq) exit(1);
    fwrite(&no->nChaves, sizeof(int), 1, arq);
    fwrite(no->chaves, sizeof(int), no->nChaves, arq);
    int i;
    if(!no->folha) {
        for(i=0; i< no->nChaves + 1; i++)
            fwrite(no->filhos[i], sizeof(char)*TAM_NOME_ARQUIVO, 1, arq);
    }
    desmapear(no);
    fclose(arq);
}

void liberar(char *nome){
    printf("liberando arquivo: %s\n",nome);
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

/*void remocao(char *nome,int num,int t){
    TNO *atual = mapear(nome);

}


void remover(char *nome,int num,int t){
    char *b = buscar(nome,num);
    if (b) {
        remocao(nome,num,t);
        free(b);
    }
}*/

void dividir(char *rz,char *f_esq){
    TNO *raiz = mapear(rz); TNO *filho_esq = mapear(f_esq);
    char f_dir[TAM_NOME_ARQUIVO];
    criar_nome(f_dir,filho_esq->chaves[t]);
    criar(f_dir,filho_esq->chaves[t]);
    TNO *filho_dir = mapear(f_dir);

    int x,y;
    for (x=0;x<raiz->nChaves;x++){
        if (num<raiz->chaves[x]) break;
    }
    for (y=raiz->nChaves-1;y>x;y--){
        raiz->chaves[y] = raiz->chaves[y-1];

    }
    if (x==0) {
        rename()
    }
    raiz->chaves[x] = filho_esq->chaves[t-1];

}

void ins_aux (char* nome,int num,int t){
    int x,y;
    TNO* atual = mapear(nome);
    if (!atual) exit(1);
    if (atual->folha){
        for (x=0;x<atual->nChaves;x++){
            if (num<atual->chaves[x]) break;
        }
        atual->nChaves++;
        for (y=atual->nChaves;y>x;y--){
            atual->chaves[y] = atual->chaves[y-1];
        }
        atual->chaves[x] = num;
        salvar(nome,atual);
        return;
    }
    for (x=0;x<atual->nChaves;x++){
        if (num<atual->chaves[x]) break;
    }
    if (atual->chaves[x] == MAX_CHAVES(t)){
        char direcao[TAM_NOME_ARQUIVO];
        strcpy(direcao,atual->filhos[x]);
        desmapear(atual);
        dividir(nome,direcao);
    }
}

void inserir (char* nome, int num, int t) {
    TNO* raiz = mapear(nome);
    if (!raiz){
        criar(nome,num);
    }

    char *p = buscar(nome,num);
    if (p){
        desmapear(raiz);
        free(p);
        return;
    }

    if (raiz->nChaves == MAX_CHAVES(t)){
        int x;
        char n_raiz[TAM_NOME_ARQUIVO],f_dir[TAM_NOME_ARQUIVO];
        criar_nome(n_raiz,raiz->chaves[t-1]); criar_nome(f_dir,raiz->chaves[t]);
        criar(n_raiz,raiz->chaves[t-1]); criar(f_dir,raiz->chaves[t]);

        TNO *nova_raiz = mapear(n_raiz); TNO *filho_dir = mapear(f_dir);
        nova_raiz->filhos[0]=nome;
        nova_raiz->filhos[1]=f_dir;
        nova_raiz->folha=0;
        filho_dir->folha = raiz->folha;
        filho_dir->nChaves = t-1;
        if(!raiz->folha){
            for (x=1;x<t-1;x++){
                filho_dir->chaves[x] = raiz->chaves[x+t];
                filho_dir->filhos[x] = raiz->filhos[x+t];
            }
            filho_dir->filhos[x] = raiz->filhos[x+t];
        }
        else{
            for (x=1;x<t-1;x++){
                filho_dir->chaves[x] = raiz->chaves[x+t];
            }
        }
        raiz->nChaves = t-1;
        salvar(n_raiz,nova_raiz); salvar(nome,raiz); salvar(f_dir,filho_dir);
        strcpy(nome,n_raiz);
        ins_aux(nome,num,t);
        return;
    }
}


void imprimir(char *nome, int n){
    TNO* no = mapear(nome);
    int i, j, nChaves;
    nChaves = no->nChaves;
    for (j = n; j > 0; j--) printf("  |");
    printf("(*");
    for (i = 0; i < nChaves; i++) printf("%d*", no->chaves[i]);
    printf(")\n");
    if (no->folha) {
        desmapear(no);
        return;
    }
    char filho[TAM_NOME_ARQUIVO];
    for (i = 0; i <= nChaves; i++) {
        strcpy(filho, no->filhos[i]);
        desmapear(no);
        imprimir(filho, n + 1);
        if (i < nChaves) no = mapear(nome);
    }
}

int main(){
    char nome[TAM_NOME_ARQUIVO];
    int op,num;
    printf("Insira o t da arvore:\n");
    scanf("%d",&t);
    printf("Insira o nome do arquivo:\n");
    scanf("%s",nome);
    setbuf(stdin,NULL);
    printf("Esta arvore ja foi criada?\n0:Sim\n1:Nao\n");
    scanf("%d",&op);
    if (op){
        inicializa(nome);
    }
    while(1) {
        printf("Operacoes: \n1. Inserir \n2. Remover \n3. Buscar \n4. Imprimir \n0. Sair \n-1. Sair sem liberar\n");
        scanf("%d", &op);
        if(op == 1) {
            printf("Digite o numero a ser inserido\n");
            scanf("%d", &num);
            if(num > 0){
                inserir(nome,num,t);
            }
        }
        else if (op == 2) {
            printf("Digite o numero a ser removido\n");
            scanf("%d", &num);
            if(num > 0) {int beta = 0;}
                //remover(nome,num,t);
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
        else if (op == 4) imprimir(nome,0);

        else if (op == 0){
            liberar(nome);
            return 0;
        }

        else if (op == -1) return 0;

        else printf("Operacao invalida\n");

    }
}
