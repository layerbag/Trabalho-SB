#include <stdio.h>

int* f1(int x, int*v);

int main(){
    int x = 3;
    int v1[3] = {20,40,60};
    int*v;
    
    v = f1(x,v1);
    for(x = 0; x < 3; x++){
        printf("%d\n",v[x]);
    }
    return 0;
}