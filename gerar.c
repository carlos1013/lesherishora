#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct no{
    int folha,nChaves;
    int *chaves;
    char **filhos;
}TNO;

int main () {
    int r=0;
    int nChaves = 3;
    int chave[3] = {10, 20, 30};
    char **nomes = (char**) malloc(sizeof(char*) * 4);
    int i;
    for (i = 0; i < 4; i++)
        nomes[i] = (char*) malloc(sizeof(char) * 30);
    strcpy(nomes[0], "arq01.dat");
    strcpy(nomes[1], "arq02.dat");
    strcpy(nomes[2], "arq03.dat");
    strcpy(nomes[3], "arq04.dat");
    for (i= 0; i < 4; i++)
        printf("nome = %s\n", nomes[i]);


    //escrita
    FILE* f = fopen("arq.dat", "wb");
    if (!f) exit(1);
    fwrite(&nChaves, sizeof(int), 1, f);
    fwrite(chave, sizeof(int), 3, f);
    for (i=0; i<4; i++)
        r = fwrite(nomes[i], sizeof(char)*30, 1, f);
    fclose(f);



    //leitura
    FILE* d = fopen("arq.dat", "rb");
    if (!d) exit(1);

    int nc = 0; //numero de chaves
    int c[3]; //chaves
    char **n = (char**) malloc(sizeof(char*) * 4); //nomes dos arquivos
    for (i = 0; i < 4; i++)
        n[i] = (char*) malloc(sizeof(char) * 30);
    fread(&nc, sizeof(int), 1, d);
    fread(c, sizeof(int), 3, d);
    fread(n, sizeof(char) * 30, 4, d);
    for (i = 0; i < 3; i++) {
        printf("chave %d= %d\n", i, c[i]);
        printf("nomes %d= %s\n", i, n[i]);
    }
    printf("nomes %d= %s\n", i, n[i]);
	fclose(d);
    return 0;
}
