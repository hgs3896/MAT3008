#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "nrutil.h"

#define NRANSI
#define PRINT_EXTRA 0
#define NP 20
#define MP 20
#define MAXSTR 80

typedef float const*const*const NonModifiableMatrix;
typedef float const*const NonModifiableVector;

void mprove(float **a, float **alud, int n, int indx[], float b[], float x[]);
void lubksb(float **a, int n, int *indx, float b[]);
void gaussj(float **a, int n, float **b, int m);
void ludcmp(float **a, int n, int *indx, float *d);
void svdcmp(float **a, int m, int n, float w[], float **v);

int read_matrix(FILE* fp, int* n, int* m, float** a, float* b);
void ex1_1(int m, int n, NonModifiableMatrix a, NonModifiableVector b);
void ex1_2(int m, int n, NonModifiableMatrix a, NonModifiableVector b);
void ex1_3(int m, int n, NonModifiableMatrix a, NonModifiableVector b);
void ex2_and_3(int m, int n, NonModifiableMatrix a, NonModifiableVector b);

int main(int argc, char const *argv[])
{
    int n,m,err;
	float **a,*b;
    const char *filename;
	
	FILE *fp;

    if(argc != 2){
        printf("Usage: %s <input file>\n", argv[0]);
        return -1;
    }
    filename = argv[1];

    a=matrix(1,MP,1,NP);
    b=vector(1,NP);
	
    if ((fp = fopen(filename, "r")) == NULL){
        char buf[MAXSTR];
        sprintf(buf, "Data file %s not found\n", filename);
		nrerror(buf);
        exit(-1);
    }

    if((err = read_matrix(fp, &m, &n, a, b))){
        char buf[MAXSTR];
        sprintf(buf, "Error reading matrix a: %d\n", err);
        nrerror(buf);
        exit(-1);
    }

    ex1_1(m, n, a, b);
    ex1_2(m, n, a, b);
    ex1_3(m, n, a, b);
    ex2_and_3(m, n, a, b);

    fclose(fp);
    
	free_matrix(a,1,MP,1,NP);
    free_vector(b,1,NP);

    return 0;
}

int read_matrix(FILE* fp, int* m, int* n, float** a, float* b){
    int k,l;

    if(feof(fp))
        return -1;
	
    if(fscanf(fp,"%d %d ", m, n) != 2)
        return -2;

    for (k=1;k<=*m;k++){
        for (l=1;l<=*n;l++) {
            if(fscanf(fp,"%f ", &a[k][l]) != 1)
                return -3;
        }
    }

    for (k=1;k<=*n;k++){
        if(fscanf(fp,"%f ", &b[k]) != 1)
            return -3;
    }

    return 0;
}

void ex1_1(int m, int n, NonModifiableMatrix a, NonModifiableVector b){
    int l,k,i,j;
    float **a_inv,**u,**x;

    assert(m == n);

    a_inv=matrix(1,NP,1,NP);
	u=matrix(1,NP,1,NP);
    x=matrix(1,NP,1,NP);

    printf("Using Gauss-Jordan Elimination\n");

    /* save matrices for later testing of results */
    for (l=1;l<=n;l++) {
        for (k=1;k<=n;k++){
            a_inv[k][l]=a[k][l];
        }
    }

    /* invert matrix */
    gaussj(a_inv,n,x,n);

    #if PRINT_EXTRA

    printf("\nInverse of matrix a : \n");
    for (k=1;k<=n;k++) {
        for (l=1;l<=m;l++)
            printf("%12.6f",a_inv[k][l]);
        printf("\n");
    }

    /* check inverse */
    printf("\na times a-inverse:\n");
    for (k=1;k<=n;k++) {
        for (l=1;l<=n;l++) {
            u[k][l]=0.0;
            for (j=1;j<=m;j++)
                u[k][l] += (a[k][j]*a_inv[j][l]);
        }

        for (l=1;l<=n;l++) printf("%12.6f",u[k][l]);
        printf("\n");
    }

    #endif

    printf("Solution vector for the equations:\n");
    for (k=1;k<=n;k++) {
        float v = 0.0;
        for(l=1;l<=n;l++){
            v += a_inv[k][l]*b[l];
        }
        printf("%12.6f", v);
    }
    printf("\n");
    printf("**********************************************************************\n");

    free_matrix(x,1,NP,1,NP);
	free_matrix(u,1,NP,1,NP);
	free_matrix(a_inv,1,NP,1,NP);
}

void ex1_2(int m, int n, NonModifiableMatrix a, NonModifiableVector b){
    int l,k,i,j,dum;
    float d;
    int *indx, *jndx;
    float **aa, **xl,**xu,**x,*ans;

    indx=ivector(1,NP);
	jndx=ivector(1,NP);

    aa=matrix(1,MP,1,NP);
    xl=matrix(1,NP,1,NP);
	xu=matrix(1,NP,1,NP);
	x=matrix(1,NP,1,NP);
    ans=vector(1,MP);

    printf("Using LU Decomposition\n");

    for (k=1;k<=m;k++) {
        for(l=1;l<=n;l++){
            aa[k][l]=a[k][l];
        }
        ans[k]=b[k];
    }

    #if PRINT_EXTRA

    /* Print out a-matrix for comparison with product of
		   lower and upper decomposition matrices */
    printf("original matrix:\n");
    for (k=1;k<=n;k++) {
        for (l=1;l<=n;l++) printf("%12.6f",a[k][l]);
        printf("\n");
    }

    #endif

    /* Perform the decomposition */
    ludcmp(aa,n,indx,&d);
    
    /* Back Substitution */
    lubksb(aa,n,indx,ans);

    #if PRINT_EXTRA

    /* Compose separately the lower and upper matrices */
    for (k=1;k<=n;k++) {
        for (l=1;l<=m;l++) {
            if (l > k) {
                xu[k][l]=aa[k][l];
                xl[k][l]=0.0;
            } else if (l < k) {
                xu[k][l]=0.0;
                xl[k][l]=aa[k][l];
            } else {
                xu[k][l]=aa[k][l];
                xl[k][l]=1.0;
            }
        }
    }
    /* Compute product of lower and upper matrices for
        comparison with original matrix */
    for (k=1;k<=n;k++) {
        jndx[k]=k;
        for (l=1;l<=n;l++) {
            x[k][l]=0.0;
            for (j=1;j<=m;j++)
                x[k][l] += (xl[k][j]*xu[j][l]);
        }
    }
    printf("\n%s%s\n","product of lower and upper ",
            "matrices (rows unscrambled):");
    for (k=1;k<=n;k++) {
        dum=jndx[indx[k]];
        jndx[indx[k]]=jndx[k];
        jndx[k]=dum;
    }
    for (k=1;k<=n;k++)
        for (j=1;j<=n;j++)
            if (jndx[j] == k) {
                for (l=1;l<=n;l++)
                    printf("%12.6f",x[j][l]);
                printf("\n");
            }
    
    printf("\nlower matrix of the decomposition:\n");
    for (k=1;k<=n;k++) {
        for (l=1;l<=n;l++) printf("%12.6f",xl[k][l]);
        printf("\n");
    }
    
    printf("\nupper matrix of the decomposition:\n");
    for (k=1;k<=n;k++) {
        for (l=1;l<=n;l++) printf("%12.6f",xu[k][l]);
        printf("\n");
    }

    #endif

    printf("Solution vector for the equations:\n");
    for (k=1;k<=m;k++) printf("%12.6f",ans[k]);
    printf("\n");

    printf("**********************************************************************\n");

    free_vector(ans,1,MP);

    free_ivector(indx, 1, NP);
	free_ivector(jndx, 1, NP);

    free_matrix(aa, 1,MP,1,NP);
    free_matrix(xl, 1, NP, 1, NP);
	free_matrix(xu, 1, NP, 1, NP);
	free_matrix(x, 1, NP, 1, NP);
}

void ex1_3(int m, int n, NonModifiableMatrix a, NonModifiableVector b){
    int j,k,l;
	float **aa,*w,**u,**v;

    aa=matrix(1,NP,1,MP);
    w=vector(1,NP);
	u=matrix(1,NP,1,MP);
	v=matrix(1,NP,1,NP);

    printf("Using Singular Value Decomposition\n");

    /* copy original matrix into u */
    for (k=1;k<=m;k++)
        for (l=1;l<=n;l++) {
            aa[k][l]=a[k][l];
            u[k][l]=a[k][l];
        }

    /* perform decomposition */
    svdcmp(u,m,m,w,v);

    #if PRINT_EXTRA

    /* write results */
    printf("Decomposition matrices:\n");

    printf("Matrix u\n");
    for (k=1;k<=m;k++) {
        for (l=1;l<=n;l++)
            printf("%12.6f",u[k][l]);
        printf("\n");
    }

    printf("Diagonal of matrix w\n");
    for (k=1;k<=n;k++)
        printf("%12.6f",w[k]);
    
    printf("\nMatrix v-transpose\n");
    for (k=1;k<=n;k++) {
        for (l=1;l<=n;l++)
            printf("%12.6f",v[l][k]);
        printf("\n");
    }

    printf("\nCheck product against original matrix:\n");
    printf("Original matrix:\n");
    for (k=1;k<=m;k++) {
        for (l=1;l<=n;l++)
            printf("%12.6f",a[k][l]);
        printf("\n");
    }

    printf("Product u*w*(v-transpose):\n");
    for (k=1;k<=m;k++) {
        for (l=1;l<=n;l++) {
            aa[k][l]=0.0;
            for (j=1;j<=n;j++)
                aa[k][l] += u[k][j]*w[j]*v[l][j];
        }

        for (l=1;l<=n;l++) printf("%12.6f",aa[k][l]);
        printf("\n");
    }

    #endif

    printf("Solution vector for the equations:\n");
    for (k=1;k<=n;k++) {
        float ans = 0.0;
        for (j=1;j<=n;j++){
            float local_ans = 0.0;
            const float w_inv = (fabs(w[j]) > 1e-9 ? 1.0 / w[j] : 0.0);
            for (l=1;l<=m;l++)
                local_ans += v[k][j] * u[l][j] * b[l];
            ans += local_ans * w_inv;
        }
        printf("%12.6f", ans);
    }
    printf("\n");

    printf("**********************************************************************\n");

    free_matrix(aa, 1, NP, 1, MP);
    free_vector(w, 1, NP);
    free_matrix(u, 1, NP, 1, MP);
    free_matrix(v, 1, NP, 1, NP);
}

void ex2_and_3(int m, int n, NonModifiableMatrix a, NonModifiableVector b){
    int i,j,*indx;
	float d,*x,**aa,*col,**a_inv;

    printf("Using mprove\n");

	indx=ivector(1,n);
	x=vector(1,n);
	aa=matrix(1,n,1,n);
    a_inv=matrix(1,n,1,n);
    col=vector(1,n);
	for (i=1;i<=n;i++) {
		x[i]=b[i];
		for (j=1;j<=n;j++)
			aa[i][j]=a[i][j];
	}
	ludcmp(aa,n,indx,&d);
	lubksb(aa,n,indx,x);

	printf("Solution vector for the equations:\n");
	for (i=1;i<=n;i++) printf("%12.6f",x[i]);
	printf("\n");

    for (i=1;i<=n;i++) {
        for (j=1;j<=n;j++) col[j] = 0.0;
        col[i] = 1.0;
        lubksb(aa,n,indx,col);
        for (j=1;j<=n;j++) a_inv[j][i] = col[j];
    }
    printf("Inverse of the A matrix:\n");
    for (i=1;i<=n;i++) {
        for (j=1;j<=n;j++) printf("%12.6f",a_inv[i][j]);
        printf("\n");
    }

    for (i=1;i<=m;i++)
        d *= aa[i][i];
    printf("Determinant of the A matrix: %12.6f\n", d);

	free_matrix(aa,1,n,1,n);
	free_vector(x,1,n);
	free_ivector(indx,1,n);
    free_matrix(a_inv,1,n,1,n);
    free_vector(col,1,n);
}

#undef NRANSI