#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define THERM 100000
#define TAKEMEASEVERY 5
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

    i = rand()%N;

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
    int **configs;
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

    sprintf(fname,"dataIsing2D_L%d/config_L%d_T%.3f.bin",L,L,temp);

    pacc = 1.-exp(-2./temp);

    s = (int*) malloc(sizeof(int)*N);

    for(int i = 0;i<N;i++){
        if(drand()>0.5) s[i] = 1;
        else s[i] = -1;
    }

    configs = (int**)malloc(sizeof(int*)*outSize);
    for(int i = 0;i<outSize;i++) configs[i] = (int*)malloc(sizeof(int)*N);

    for(int step = 0;step < MCS+THERM;step++){
        wolffStep(N);

        if((step%TAKEMEASEVERY==0) && (step>=THERM)){
            int idx = (int)(step-THERM)/TAKEMEASEVERY;
            for(int k = 0;k<N;k++) configs[idx][k]=s[k];
        }
    }
    
    exportArrayToBinary2(fname,configs,outSize,N);

    free(s);
    for(int i = 0;i<outSize;i++) free(configs[i]);
    free(configs);

    return 0;
}