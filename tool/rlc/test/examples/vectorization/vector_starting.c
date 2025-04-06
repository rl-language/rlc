int something_32(int a[4], int b){
    b = 0;
    for(int i = 0; i != 4; i++){
        b += a[i];
    }
    
    return b;
}

long something_64(long a[4], long b){
    b = 0;
    for(int i = 0; i != 4; i++){
        b += a[i];
    }
    
    return b;
}

void something_32_void(int a[4], int *b){
    *b = 0;
    for(int i = 0; i != 4; i++){
        *b += a[i];
    }
}

void something_64_void(long a[4], long *b){
    *b = 0;
    for(int i = 0; i != 4; i++){
        b += a[i];
    }   
}


void vector_sum(int a[10], int b[10], int c[10]){
    
    for(int i = 0; i < 10; i++){
        c[i] = a[i] + b[i];
    }
}
// int main(){
//     int a[4] = {1, 2, 4, 6};
//     int sum = 0;

//     sum = something(a, sum);

//     // printf("sum = %d\n", sum);

//     return sum;
// }

void vector_sum_restrict(int* restrict a, int* restrict b, int* restrict c){
    for(int i = 0; i < 10; i++){
        c[i] = a[i] + b[i];
    }
}