#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "nrutil.h"

extern void ludcmp(double **a, int n, int *indx, double *d);
extern void lubksb(double **a, int n, int *indx, double b[]);

typedef double result_t;
typedef double* vec_t;
typedef const double* cvec_t;

/* Function Sets */
static inline result_t f_0(cvec_t x){ return x[1]; }
static inline result_t f_1(cvec_t x){ return x[2]; }
static inline result_t f_2(cvec_t x){ return 1; }
static result_t (*f[])(cvec_t x) = {f_0, f_1, f_2};

static result_t* least_square(const result_t** data, const result_t* y, int m, int n);

int main(int argc, const char *argv[]){
    const int n = 3;
    int m;
    result_t** data;
    result_t *x, *y;

    m = 77;
    if(argc == 2){
        m = atoi(argv[1]);
    }

    data=dmatrix(1,m,1,n-1);
    x = dvector(1, m);  // x' = a_1*x + a_2*y + a_3*1
    y = dvector(1, m);  // y' = a_4*x + a_5*y + a_6*1

    for(int i=1; i<=m; i++){
        for(int j=1; j<n; j++){
            scanf("%lf", &data[i][j]);
        }
        scanf("%lf", &x[i]);
        scanf("%lf", &y[i]);
    }

    result_t* a_123 = least_square(data, x, m, n);
    result_t* a_456 = least_square(data, y, m, n);
    printf("The result is: %lf, %lf, %lf, %lf, %lf, %lf\n", a_123[1], a_123[2], a_123[3], a_456[1], a_456[2], a_456[3]);
    free_dvector(a_123, 1, n);
    free_dvector(a_456, 1, n);

    free_dvector(x, 1, m);
    free_dvector(y, 1, m);
    free_dmatrix(data,1,m,1,n-1);
    return 0;
}

/*
 * const result_t** data : (m x n) matrix
 * const result_t* y     : (m x 1) vector
 * int m                 : number of data
 * int n                 : number of variables
 */
static result_t* least_square(const result_t** data, const result_t* y, int m, int n){
    double determinant;
    result_t* c = dvector(1, n);
    result_t** J_t = dmatrix(1, n, 1, m);
    result_t** A = dmatrix(1, n, 1, n);
    int* indx = ivector(1, n);

    // Calculating J_t
    // where J^T = -[
    //    [x_1, x_2, ..., x_m],
    //    [y_1, y_2, ..., y_m],
    //    [1  , 1  , ...,   1],
    // ]
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            J_t[i][j] = -f[i-1](data[j]);
        }
    }

    // Calculating A = J^T * J
    for(int i = 1; i <= n; i++){
        for(int j = 1; j <= n; j++){
            A[i][j] = 0;
            for(int k = 1; k <= m; ++k){
                A[i][j] += J_t[i][k] * J_t[j][k];
            }
        }
    }

    // Calculating yy = -J^T * y
    for(int i = 1; i <= n; i++){
        c[i] = 0;
        for(int j = 1; j <= m; j++){
            c[i] += -J_t[i][j] * y[j];
        }
    }

    // Solve Ac=yy (J^T * J * c = -J^T * y)
    /* 1. Perform the decomposition */
    ludcmp(A,n,indx,&determinant);
    if(fabs(determinant) < 1e-6){
        printf("Singular matrix in routine ludcmp\n");
        goto clean_up;
    }
    /* 2. Back Substitution */
    lubksb(A,n,indx,c);

clean_up:
    free_ivector(indx, 1, n);
    free_dmatrix(J_t, 1, n, 1, m);
    free_dmatrix(A, 1, n, 1, n);
    return c;
}