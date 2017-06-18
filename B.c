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
    if (sprintf(nome, "%d.dat", nome_count++) == -1) printf("erro ao tentar criar nome\n");
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
    FILE *fp = fopen(nome,"rb");
    if (!fp) exit(1);
    int n_chaves,r,x=0;
    r = fread(&n_chaves,sizeof(int),1,fp);
    if (r==0){
        fclose(fp);
        remove(nome);
        return;
    }

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

void caso_3a_irm_dir(TNO *atual,TNO *caminho,TNO *irmao_dir,int i){
    int j;
    caminho->chaves[caminho->nChaves] = atual->chaves[i];
    strcpy(caminho->filhos[caminho->nChaves+1],irmao_dir->filhos[0]);
    caminho->nChaves++;
    atual->chaves[i] = irmao_dir->chaves[0];
    for (j=0;j<irmao_dir->nChaves-1;j++){
        irmao_dir->chaves[j] = irmao_dir->chaves[j+1];
    }
    if(!irmao_dir->folha){
        for (j=0;j<irmao_dir->nChaves;j++){
            strcpy(irmao_dir->filhos[j],irmao_dir->filhos[j+1]);
        }
    }
    irmao_dir->nChaves--;
}

void caso_3a_irm_esq(TNO *atual,TNO *caminho,TNO *irmao_esq,int i){
    int j;
    for (j=caminho->nChaves;j<0;j--){
        caminho->chaves[j] = caminho->chaves[j-1];
    }
    if (!caminho->folha){
        for (j=caminho->nChaves+1;j<0;j--){
            strcpy(caminho->filhos[j],caminho->filhos[j-1]);
        }
    }
    caminho->chaves[0] = atual->chaves[i];
    strcpy(caminho->filhos[0],irmao_esq->filhos[irmao_esq->nChaves]);
    caminho->nChaves++;
    atual->chaves[i] = irmao_esq->chaves[irmao_esq->nChaves-1];
    irmao_esq->nChaves--;
}



void remocao(char *nome,int num){
    TNO *atual = mapear(nome);
    printf("tentando remover %d no arquivo %s\n", num, nome);
    int i, j;
    char e[TAM_NOME_ARQUIVO];
    for(i = 0; i<atual->nChaves && atual->chaves[i] < num; i++);// encontrar o lugar da chave
    if(num == atual->chaves[i]){ // Se o num esta no NO atual (caso 1, 2A, 2B e 2C)
        //caso 1
        if(atual->folha) {
            atual->nChaves--;
            for(j=i; j<atual->nChaves; j++){               // reorganiza as chaves
                atual->chaves[j] = atual->chaves[j+1];
            }
            salvar(nome, atual);
            return;
        }
        //caso 2A
        TNO *filho_esq = mapear(atual->filhos[i]);
        if(filho_esq->nChaves >= t) {
            while(!filho_esq->folha) { // a partir do filho a esquerda, pega o filho mais a direita do fim da arvore
                strcpy(e,filho_esq->filhos[filho_esq->nChaves]);
                desmapear(filho_esq);
                filho_esq = mapear(e);
            }
            int temp = filho_esq->chaves[filho_esq->nChaves - 1];
            atual->chaves[i] = temp;
            salvar(nome,atual);
            desmapear(filho_esq);
            strcpy(e,atual->filhos[i]);
            remocao(e,temp);
            return;
        }
        //caso 2B
        TNO *filho_dir = mapear(atual->filhos[i+1]);
        if(filho_dir->nChaves >= t) {
            desmapear(filho_esq);
            while(!filho_dir->folha) { // a partir do filho a direita, pega o filho mais a esquerda do fim da arvore
                strcpy(e,filho_dir->filhos[0]);
                desmapear(filho_dir);
                filho_dir = mapear(e);
            }
            int temp = filho_dir->chaves[0];
            atual->chaves[i] = temp;
            salvar(nome,atual);
            desmapear(filho_dir);
            strcpy(e,atual->filhos[i+1]);
            remocao(e,temp);
            return;
        }

        //caso 2C
        if(filho_esq->nChaves == t-1 && filho_dir->nChaves == t-1) {
            filho_esq->chaves[filho_esq->nChaves] = num;
            for(j=0; j<t-1; j++)
                filho_esq->chaves[j+t] = filho_dir->chaves[j];
            if(!filho_esq->folha) {
                for(j=0; j<=t-1; j++)
                    strcpy(filho_esq->filhos[j+t], filho_dir->filhos[j]);
            }
            filho_esq->nChaves = MAX_CHAVES(t);
            remove(atual->filhos[i+1]);
            desmapear(filho_dir);
            if (atual->nChaves == 1) {
                salvar(nome,filho_esq);            // salva na raiz o filho da esquerda
                remove(atual->filhos[i]);                // remove o filho da esquerda da face da terra
                desmapear(atual);
                return;
            }
            salvar(atual->filhos[i],filho_esq);
            atual->nChaves--;
            for(j=i; j < atual->nChaves; j++)
                atual->chaves[j] = atual->chaves[j+1];
            for(j=i+1; j <= atual->nChaves; j++)
                strcpy(atual->filhos[j], atual->filhos[j+1]);
            strcpy(e,atual->filhos[i]);
            salvar(nome, atual);
            remocao(e,num);
            return;
        }
        desmapear(filho_dir);
        desmapear(filho_esq);
    }
    TNO* caminho = mapear(atual->filhos[i]); // filho q pode conter a chave...
    if (caminho->nChaves == t-1) {
        //caso 3a
        if(i>0) {
            TNO *irmao_dir = mapear(atual->filhos[i+1]);
            if (irmao_dir->nChaves>t-1){
                caso_3a_irm_dir(atual,caminho,irmao_dir,i);
                salvar(atual->filhos[i+1],irmao_dir);  salvar(e,caminho);  salvar(nome,atual);
                return remocao(e,num);
            }
            desmapear(irmao_dir);
        }
        if (i<atual->nChaves){
            TNO *irmao_esq = mapear(atual->filhos[i-1]);
            if (irmao_esq->nChaves>t-1){
                caso_3a_irm_esq(atual,caminho,irmao_esq,i);
                salvar(atual->filhos[i-1],irmao_esq);  salvar(e,caminho);  salvar(nome,atual);
                return remocao(e,num);
            }
            desmapear(irmao_esq);
        }
        TNO* irmao_dir = mapear(atual->filhos[i+1]);
        caminho->chaves[caminho->nChaves] = atual->chaves[i];
        for(j=0; j<irmao_dir->nChaves; j++) // juntar as chaves do irmao
            caminho->chaves[j+t] = irmao_dir->chaves[j];
        caminho->nChaves = MAX_CHAVES(t);
        for(j=0; j<irmao_dir->nChaves+1; j++) // receber os  filhos do irmao
            strcpy(caminho->filhos[j+t], irmao_dir->filhos[j]);
        remove(atual->filhos[i+1]);
        desmapear(irmao_dir);
        salvar(atual->filhos[i],caminho);
        // reorganizar as chaves do meu no atual
        atual->nChaves--;
        for(j=i; j<atual->nChaves; j++)
            atual->nChaves[j] = atual->nChaves[j+1];
        // a partir do irmao do caminho, reorganizar os filhos
        for(j=i+1; j =< atual->nChaves; j++)
            strcpy(atual->filhos[j], atual->filhos[j+1]);

        strcpy(e, atual->filhos[i]);
        salvar(nome,atual);
        remocao(e, num);
    }
    strcpy(e,atual->filhos[i]);
    desmapear(atual);
    remocao(e,num);
}

void remover(char *nome,int num){
    char *b = buscar(nome,num);
    if (b) {
        free(b);
        remocao(nome,num);
    }
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
    int nChaves_f = f->nChaves;
    desmapear(f);
    if (nChaves_f == MAX_CHAVES(t)){
        char direcao[TAM_NOME_ARQUIVO];
        strcpy(direcao,atual->filhos[x]);
        desmapear(atual);
        dividir(nome,direcao,x);
        atual = mapear(nome);
    }
    for (x=0;x<atual->nChaves;x++){
        if (num<atual->chaves[x]) break;
    }
    char direcao[TAM_NOME_ARQUIVO];
    strcpy(direcao,atual->filhos[x]);
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
    if (!no) return;
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
            if(num > 0) {
                remover(nome,num);
            }
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

        else if (op == -1) {
            printf("raiz atual: %s",nome);
            return 0;
        }

        else printf("Operacao invalida\n");

    }
}
