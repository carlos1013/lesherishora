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
void criar_nome(char* nome);
void inicializa(char *nome);
TNO *mapear(char* nome);
void desmapear(TNO *no);
void salvar(char* nome,TNO *no);
void liberar(char *nome);
char *buscar (char *nome, int num);
void ins_aux (char* nome,int num);
void inserir (char* nome, int num);
void imprimir(char *nome, int n);

int t;
int nome_count;

void criar(char *nome,int num){
    FILE *fp = fopen(nome,"wb");
    if (!fp) exit(1);
    int x = 1;
    fwrite(&x,sizeof(int),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);
}

void criar_nome(char* nome)  {
    itoa(nome_count++,nome,10);
    strcat(nome,".dat");
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
        fclose(arq);
        return NULL;
    }

    novo->chaves = (int*)malloc(sizeof(int)*MAX_CHAVES(t));
    r = fread(novo->chaves, sizeof(int), novo->nChaves, arq);

    if (!r){
        free(novo->chaves);
        free(novo);
        fclose(arq);
        return NULL;
    }

    int iNomeFilhos = sizeof(int)*(1 + novo->nChaves);
    fseek(arq, 0, SEEK_END);
    novo->folha = iNomeFilhos == ftell(arq);

    fseek(arq, iNomeFilhos, SEEK_SET);
    novo->filhos = (char**)malloc(MAX_FILHOS(t)*sizeof(char*));
    int i;
    for(i=0; i< MAX_FILHOS(t); i++) {
        novo->filhos[i] = (char*)malloc(sizeof(char)*TAM_NOME_ARQUIVO);
        if (!novo->folha && i<= novo->nChaves){
            fread(novo->filhos[i], sizeof(char)*TAM_NOME_ARQUIVO, 1, arq);
        }
    }
    fclose(arq);
    return novo;
}

void desmapear(TNO *no){
    int i;
    for(i=0; i < MAX_FILHOS(t); i++) free(no->filhos[i]);
    free(no->filhos);
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
            fclose(arq);
            return n;
        }
    }
    int iNomeFilhos = sizeof(int)*(1 + nChaves);
    fseek(arq, 0, SEEK_END);
    if (iNomeFilhos == ftell(arq)) {
        fclose(arq);
        return NULL;
    }

    fseek(arq, iNomeFilhos + i*sizeof(char)*TAM_NOME_ARQUIVO, SEEK_SET);

    char n[TAM_NOME_ARQUIVO];
    fread(n, sizeof(char), TAM_NOME_ARQUIVO, arq);
    fclose(arq);
    return buscar(n,num);
}


char* remocao(char *nome,int num){
    TNO *atual = mapear(nome);
    printf("tentando remover %d no arquivo %s\n", num, nome);
    int i, j;
    for(i = 0; i<atual->nChaves && atual->chaves[i] < num; i++);// encontrar o lugar da chave
    if(i < atual->nChaves && num == atual->chaves[i]){ // Se o num esta no NO atual (caso 1, 2A, 2B e 2C)
        if(atual->folha) { // caso 1
            atual->nChaves--;
            for(j=i; j<atual->nChaves; j++) // reorganiza as chaves
                atual->chaves[j] = atual->chaves[j+1];
            salvar(nome, atual);
            return nome;
        }
        else {
            char e[TAM_NOME_ARQUIVO];
            strcpy(e, atual->filhos[i]);
            TNO *filho_esq = mapear(e);
            if(filho_esq->nChaves >= t) { //caso 2A
                while(!filho_esq->folha) { // a partir do filho a esquerda, pega o filho mais a direita do fim da arvore
                    strcpy(e, filho_esq->filhos[filho_esq->nChaves]);
                    desmapear(filho_esq);
                    filho_esq = mapear(e);
                }
                int temp = filho_esq->chaves[filho_esq->nChaves - 1];
                atual->chaves[i] = temp;
                salvar(nome, atual);

                desmapear(filho_esq);
                remocao(e, temp);

                /* esta caralha fazia uma merda colossal pq substituia o filho pelo seu filho UAU!!!

                strcpy(e, remocao(e, temp));
                atual = mapear(nome);
                strcpy(atual->filhos[i], e);
                strcpy(filho_esq->filhos[i], e);
                salvar(nome, atual);
                */
                return nome;
            }
            else {
                desmapear(filho_esq);
                char d[TAM_NOME_ARQUIVO];
                strcpy(d, atual->filhos[i+1]);
                TNO* filho_dir = mapear(d);

                if(filho_dir->nChaves >= t) {//caso 2b
                    while(!filho_dir->folha) { // a partir do filho a direita, pega o filho mais a esquerda do fim da arvore
                        strcpy(d, filho_dir->filhos[0]);
                        desmapear(filho_dir);
                        filho_dir = mapear(d);
                    }

                    int temp = filho_dir->chaves[0];
                    atual->chaves[i] = temp;
                    salvar(nome, atual);

                    desmapear(filho_dir);
                    remocao(d, temp);

                    /*strcpy(d, remocao(d, temp));

                    atual = mapear(nome);
                    strcpy(atual->filhos[i+1], d);
                    salvar(nome, atual);*/
                    return nome;
                }
                else {
                    desmapear(atual);// tentando trabalhar apenas com 2  nós...
                    filho_esq = mapear(e);
                    if(filho_esq->nChaves == t-1 && filho_dir->nChaves == t-1) {//caso 2c
                        filho_esq->chaves[filho_esq->nChaves] = num;
                        for(j=0; j<t-1; j++)
                            filho_esq->chaves[j+t] = filho_dir->chaves[j];
                        if(!filho_esq->folha) {
                            for(j=0; j<=t; j++)
                                strcpy(filho_esq->filhos[j+t], filho_dir->filhos[j]);
                        }
                        filho_esq->nChaves = MAX_CHAVES(t);

                        salvar(e, filho_esq);
                        if(filho_dir->folha)
                            remove(d);
                        desmapear(filho_dir);

                        strcpy(e, remocao(e, num));
                        atual = mapear(nome);
                        // da erro se a arvore tiver apenas dois niveis e a raiz tiver apenas uma chave. segue a gambiarra:
                        if (atual->nChaves == 1) { // acho que so acontece na raiz...
                            salvar(nome, mapear(e));// salva na raiz o filho da esquerda
                            remove(e); // remove o filho da esquerda da face da terra
                            desmapear(atual);
                            return nome;
                        }
                        atual->nChaves--;
                        for(j=i; j < atual->nChaves; j++)
                            atual->chaves[j] = atual->chaves[j+1];
                        for(j=i+1; j <= atual->nChaves; j++)
                            strcpy(atual->filhos[j], atual->filhos[j+1]);
                        strcpy(atual->filhos[i], e);
                        salvar(nome, atual);

                        return nome;
                    }
                }
            }
        }
    }

    char e[TAM_NOME_ARQUIVO];
    strcpy(e, atual->filhos[i]);
    TNO* fi = mapear(e); // filho q pode conter a chave...
    if (fi->nChaves == t-1) { printf("caso 3x num = %d", num);// caso 3a e 3b
        if(i < atual->nChaves) {
            TNO* fib = mapear(atual->filhos[i+1]); // irmao imediato a direita do filho i
            if(fib->nChaves >= t) { // se irmao imediato do filho i tem pelo menos t chaves... 3A :)
                fi->chaves[t - 1] = atual->chaves[i]; // a chave t-1 do filho i sera a cave i do no atual
                fi->nChaves++;

                atual->chaves[i] = fib->chaves[0]; // substituir a chave i do no atual pela primeira chave do irmao
                for(j=0; j<fib->nChaves; j++)
                    fib->chaves[j] = fib->chaves[j+1]; // reorganizar as chaves do irmao

                strcpy(fi->filhos[fi->nChaves], fib->filhos[0]); // o ultimo filho do no i sera o primeiro filho do seu irmao (risos)
                for(j=0; j<fib->nChaves; j++)
                    strcpy(fib->filhos[j], fib->filhos[j+1]);
                fib->nChaves--;

                salvar(e, fi);
                salvar(atual->filhos[i+1], fib);
                salvar(nome, atual);

                strcpy(e, remocao(e, num));
                atual = mapear(nome);
                strcpy(atual->filhos[i], e);
                salvar(nome, atual);
                return nome;
            }
            desmapear(fib);
        }
    }


    desmapear(atual);
    desmapear(fi);
    strcpy(e, remocao(e, num));
    atual = mapear(nome);
    strcpy(atual->filhos[i], e);
    salvar(nome,atual);
    return nome;
}

void remover(char *nome,int num){
    char *b = buscar(nome,num);
    if (b)
        remocao(nome,num);
    free(b);
}


void imprimir_no(TNO *no){
    int x;
    printf("folha? %d\n",no->folha);
    printf("chaves:\n");
    for (x=0;x<no->nChaves;x++){
        printf("%d\n",no->chaves[x]);
    }
    printf("filhos:\n");
    for (x=0;x<=no->nChaves;x++){
        printf("%s\n",no->filhos[x]);
    }
    printf("\n");
}

void dividir(char *rz,char *f_esq,int x){
    TNO *raiz = mapear(rz); TNO *filho_esq = mapear(f_esq);
    char f_dir[TAM_NOME_ARQUIVO];
    criar_nome(f_dir);
    int y;

    for (y=raiz->nChaves;y>x;y--){  //inserindo o novo numero na raiz
        raiz->chaves[y] = raiz->chaves[y-1];
        strcpy(raiz->filhos[y+1],raiz->filhos[y]);
    }
    strcpy(raiz->filhos[y+1],raiz->filhos[y]);
    raiz->nChaves++;
    raiz->chaves[x] = filho_esq->chaves[t-1];
    //strcpy(raiz->filhos[x],f_esq);
    strcpy(raiz->filhos[x+1],f_dir);
    salvar(rz,raiz);

    criar(f_dir,filho_esq->chaves[t]);
    TNO *filho_dir = mapear(f_dir);
    filho_dir->nChaves = t-1;     //dividindo os nos
    filho_dir->folha = filho_esq->folha;
    if (filho_dir->folha){
        for (x=1;x<filho_dir->nChaves;x++){
            filho_dir->chaves[x] = filho_esq->chaves[x+t];
        }
    }
    else{
        for (x=1;x<filho_dir->nChaves;x++){
            filho_dir->chaves[x] = filho_esq->chaves[x+t];
        }
        for (x=0;x<=filho_dir->nChaves;x++){
            strcpy(filho_dir->filhos[x],filho_esq->filhos[x+t]);
        }
    }
    filho_esq->nChaves = t-1;
    salvar(f_esq,filho_esq); salvar(f_dir,filho_dir);
}

void ins_aux (char* nome,int num){
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
    TNO* f = mapear(atual->filhos[x]);
    if (f->nChaves == MAX_CHAVES(t)){
        char direcao[TAM_NOME_ARQUIVO];
        strcpy(direcao,atual->filhos[x]);
        desmapear(f);
        desmapear(atual);
        dividir(nome,direcao,x);
        atual = mapear(nome);
    }
    for (x=0;x<atual->nChaves;x++){
        if (num<atual->chaves[x]) break;
    }
    char direcao[TAM_NOME_ARQUIVO];
    strcpy(direcao,atual->filhos[x]);
    desmapear(f);
    desmapear(atual);
    inserir(direcao,num);
}

void inserir (char* nome, int num) {
    TNO* raiz = mapear(nome);
    if (!raiz){
        criar(nome,num);
        return;
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
        criar_nome(n_raiz); criar_nome(f_dir);
        criar(n_raiz,raiz->chaves[t-1]); criar(f_dir,raiz->chaves[t]);

        TNO *nova_raiz = mapear(n_raiz); TNO *filho_dir = mapear(f_dir);
        strcpy(nova_raiz->filhos[0],nome);
        strcpy(nova_raiz->filhos[1],f_dir);
        nova_raiz->folha=0;
        filho_dir->folha = raiz->folha;
        filho_dir->nChaves = t-1;
        if(!raiz->folha){
            for (x=1;x<t-1;x++){
                filho_dir->chaves[x] = raiz->chaves[x+t];
            }
            for (x=0;x<=t-1;x++){
                strcpy(filho_dir->filhos[x],raiz->filhos[x+t]);
            }
        }
        else{
            for (x=1;x<t-1;x++){
                filho_dir->chaves[x] = raiz->chaves[x+t];
            }
        }
        raiz->nChaves = t-1;
        salvar(n_raiz,nova_raiz); salvar(nome,raiz); salvar(f_dir,filho_dir);
        strcpy(nome,n_raiz);
        ins_aux(nome,num);
        return;
    }
    ins_aux(nome,num);
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
    nome_count = 2;
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
                inserir(nome,num);
            }
        }
        else if (op == 2) {
            printf("Digite o numero a ser removido\n");
            scanf("%d", &num);
            if(num > 0) {int beta = 0;}
                remover(nome,num);
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
