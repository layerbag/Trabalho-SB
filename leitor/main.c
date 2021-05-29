#include <stdio.h>

int f1(int x, int*v);
int f2(int x){
    return x * 2;
}

int main(){
    int x = 3;
    int v[6] = {1,2,3,4,5,6};
    printf("%d\n", f1(x,v));

    return 0;
}