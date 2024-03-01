#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define THERM 100000
#define TAKEMEASEVERY 5
//#define EXPORTCONFIGS 
#define EXPORTCORR
int L,N;
int *s;
double temp;
double pacc; 
void exit_failure(char *s){
    fprintf(stderr,"%s",s);
    exit(EXIT_FAILURE);
}
void exportArrayToBinary2(const char *filename, int **array, int dim1, int dim2) {
    FILE *file = fopen(filename, "wb");

    if (file == NULL) {
        fprintf(stderr, "Error opening file for writing.\n");
    }

    for (int i = 0; i < dim1; i++) {
            fwrite(array[i], sizeof(int), dim2, file);
    }

    fclose(file);
}
void exportArrayToBinary(const char *filename, int *array, int dim1) {
    FILE *file = fopen(filename, "wb");

    if (file == NULL) {
        fprintf(stderr, "Error opening file for writing.\n");
    }

    fwrite(array, sizeof(int), dim1, file);

    fclose(file);
}
void init_rand(){
     unsigned int myrand;
    FILE *devran = fopen("/dev/random","r");
    size_t k=fread(&myrand,4,1,devran);
    fclose(devran);
    srand(myrand);
}
double drand(){
    return (double) rand()/RAND_MAX;
}

void wolffStep(const int N){
    int i; 
    int sp;
    int os,ns;
    int current,nn;
    int stack[N];

    i = (int)drand()*N;

    stack[0] = i;
    sp = 1;
    os = s[i];
    ns = -s[i];
    s[i] = ns;

    while(sp){
        current = stack[--sp];
        if((nn=current+1)>=N) nn-=N;
        if(s[nn] == os)
            if(drand()<pacc){
                stack[sp++] = nn;
                s[nn] = ns;
            }
        if((nn=current-1)<0) nn+=N;
        if(s[nn] == os)
            if(drand()<pacc){
                stack[sp++] = nn;
                s[nn] = ns;
            }
        if((nn=current+L)>=N) nn-=N;
        if(s[nn] == os)
            if(drand()<pacc){
                stack[sp++] = nn;
                s[nn] = ns;
            }
        if((nn=current-L)<0) nn+=N;
        if(s[nn] == os)
            if(drand()<pacc){
                stack[sp++] = nn;
                s[nn] = ns;
            }
    }
}
int main(int argc, char **argv){
    int MCS,outSize;
    #ifdef EXPORTCONFIGS
    int **configs;
    #endif
    #ifdef EXPORTCORR
    int **corrmat,*magns;
    #endif
    char fname[100],dirname[50];

    if(argc!=4)
        exit_failure("Need <L> <Temp> <MCS> \n");
    L = atoi(argv[1]);
    N = L*L;
    temp = atof(argv[2]);
    MCS = atof(argv[3]);
    outSize = (int)(MCS/TAKEMEASEVERY);
    
    //sprintf(dirname,"rm -rv dataIsing2D_L%d",L);
    //system(dirname);
    sprintf(dirname,"mkdir dataIsing2D_L%d",L);
    system(dirname);

    pacc = 1.-exp(-2./temp);

    s = (int*) malloc(sizeof(int)*N);

    for(int i = 0;i<N;i++){
        s[i] = 1;
    }
    #ifdef EXPORTCONFIGS
    configs = (int**)malloc(sizeof(int*)*outSize);
    for(int i = 0;i<outSize;i++) configs[i] = (int*)malloc(sizeof(int)*N);
    #endif
    #ifdef EXPORTCORR
    corrmat = (int**)malloc(sizeof(int*)*N);
    for(int i = 0;i<N;i++){
        corrmat[i] = (int*)malloc(sizeof(int)*N);
        for(int j = 0;j<N;j++){
            corrmat[i][j] = 0;
        }
    }
    magns = (int*)malloc(sizeof(int)*outSize);

    #endif
    for(int step = 0;step < MCS+THERM;step++){
        wolffStep(N);
        if((step%TAKEMEASEVERY==0) && (step>=THERM)){
            int idx = (int)(step-THERM)/TAKEMEASEVERY;
            #ifdef EXPORCONFIGS
            for(int k = 0;k<N;k++) configs[idx][k]=s[k];
            #endif
            #ifdef EXPORTCORR
            int sum = 0;
            for(int k = 0;k<N;k++){
                sum += s[k];
                corrmat[k][k] += s[k]*s[k];
                for(int j = k+1;j<N;j++){
                    corrmat[k][j] += s[k]*s[j];
                    corrmat[j][k] = corrmat[k][j];
                }
            }
            magns[idx] = sum;
            #endif
        }
    }
    #ifdef EXPORTCONFIGS
    sprintf(fname,"dataIsing2D_L%d/config_L%d_T%.3f.bin",L,L,temp);
    exportArrayToBinary2(fname,configs,outSize,N);
    for(int i = 0;i<outSize;i++) free(configs[i]);
    free(configs);
    #endif
    #ifdef EXPORTCORR
    sprintf(fname,"dataIsing2D_L%d/corrmat_T%.3f.bin",L,temp);
    exportArrayToBinary2(fname,corrmat,N,N);
    sprintf(fname,"dataIsing2D_L%d/magn_T%.3f.bin",L,temp);
    exportArrayToBinary(fname,magns,outSize);

    for(int i = 0;i<N;i++) free(corrmat[i]);
    free(corrmat);
    free(magns);
    #endif

    free(s);

    return 0;
}