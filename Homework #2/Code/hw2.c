#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

float myfunc(float);
void myfunc_(float, float *, float *);
float bessj0(float x);
void bessj0_(float, float *, float *);
float rtbis(float (*func)(float), float x1, float x2, float xacc);
float rtflsp(float (*func)(float), float x1, float x2, float xacc);
float rtsec(float (*func)(float), float x1, float x2, float xacc);
float rtnewt(void (*funcd)(float, float *, float *), float x1, float x2, float xacc);
float rtsafe(void (*funcd)(float, float *, float *), float x1, float x2, float xacc);
float rtmuller(float (*func)(float), float x1, float x2, float xacc);
void zbrak(float (*fx)(float), float x1, float x2, int n, float xb1[], float xb2[], int *nb);

static int num_steps;
#define PRINT_AND_MEASURE(name, expr) \
	num_steps = 0; \
	printf(name ": %f (Required Steps = %d)\n", (expr), num_steps);

int main(int argc, char const *argv[])
{
    static const float XACC = 1.0e-6;
    float xb1[101], xb2[101];
    int i,nb=20;

    float x1 = 1.0, x2 = 10.0;
    printf("Using bracketing method, find the possible roots of bessj0.\n");
    zbrak(bessj0, x1, x2, 100, xb1, xb2, &nb);
    for (i=1;i<=nb;i++){
        x1 = xb1[i];
        x2 = xb2[i];
        printf("Root in [%f, %f]\n", x1, x2);
		PRINT_AND_MEASURE("Bisection method", rtbis(bessj0, x1, x2, XACC));
		PRINT_AND_MEASURE("Linear Interpolation method", rtflsp(bessj0, x1, x2, XACC));
		PRINT_AND_MEASURE("Secant method", rtsec(bessj0, x1, x2, XACC));
		PRINT_AND_MEASURE("Newton-Raphson method", rtnewt(bessj0_, x1, x2, XACC));
		PRINT_AND_MEASURE("Newton with bracketing", rtsafe(bessj0_, x1, x2, XACC));
		PRINT_AND_MEASURE("Muller Method", rtmuller(bessj0, x1, x2, XACC));
    }

	printf("Using bracketing method, find the possible roots of my nonlinear function (e^(-x)*cos(x)).\n");
	x1 = -5.0; x2 = 5.0;
    zbrak(myfunc, x1, x2, 100, xb1, xb2, &nb);
    for (i=1;i<=nb;i++){
        x1 = xb1[i];
        x2 = xb2[i];
        printf("Root in [%f, %f]\n", x1, x2);
		PRINT_AND_MEASURE("Bisection method", rtbis(myfunc, x1, x2, XACC));
		PRINT_AND_MEASURE("Linear Interpolation method", rtflsp(myfunc, x1, x2, XACC));
		PRINT_AND_MEASURE("Secant method", rtsec(myfunc, x1, x2, XACC));
		PRINT_AND_MEASURE("Newton-Raphson method", rtnewt(myfunc_, x1, x2, XACC));
		PRINT_AND_MEASURE("Newton with bracketing", rtsafe(myfunc_, x1, x2, XACC));
		PRINT_AND_MEASURE("Muller Method", rtmuller(myfunc, x1, x2, XACC));
    }

    return 0;
}

float myfunc(float x){
	// f(x) = e^(-x)*cos(x)
	return expf(-x) * cosf(x);
}

void myfunc_(float rtn, float *f, float *df){
	float x = rtn;
	*f = myfunc(x);
	// f'(x) = -e^(-x)*cos(x) - e^(-x)*sin(x)
	*df = -expf(-x) * cosf(x) + -expf(-x) * sinf(x);
}

float rtmuller(float (*func)(float), float x1, float x2, float xacc){
    float x3 = 0.5 * (x1 + x2);
    float x4;
    float a, b, c;
	const double eps = 1.084202e-19;
    do{
        ++num_steps;
        c = func(x3);
		b = (x1 - x3) / (x2 - x3 + eps) / (x1 - x2 + eps) * (func(x2) - func(x3)) - (x2 - x3) / (x1 - x3 + eps) / (x1 - x2 + eps) * (func(x1) - func(x3));
        a = (func(x1) - func(x3)) / (x1 - x3 + eps) / (x1 - x2 + eps) - (func(x2) - func(x3)) / (x2 - x3 + eps) / (x1 - x2 + eps);
        // b = ((x1 - x3) * (x1 - x3) * (func(x2) - func(x3)) - (x2 - x3) * (x2 - x3) * (func(x1) - func(x3))) / ((x1 - x3) * (x2 - x3) * (x1 - x2));
        // a = ((x2 - x3) * (func(x1) - func(x3)) - (x1 - x3) * (func(x2) - func(x3))) / ((x1 - x3) * (x2 - x3) * (x1 - x2));
		if(b * b - 4 * a * c < 0){
			break;
		}
        x4 = x3 - 2 * c / (b + (b > 0 ? 1 : -1) * sqrt(b * b - 4 * a * c) + eps);
        x1 = x2; x2 = x3; x3 = x4;
    }while(fabs(func(x4)) > xacc);
    return x4;
}

void nrerror(char error_text[])
/* Numerical Recipes standard error handler */
{
	fprintf(stderr,"Numerical Recipes run-time error...\n");
	fprintf(stderr,"%s\n",error_text);
	fprintf(stderr,"...now exiting to system...\n");
	exit(1);
}

float bessj0(float x)
{
	float ax,z;
	double xx,y,ans,ans1,ans2;

	if ((ax=fabs(x)) < 8.0) {
		y=x*x;
		ans1=57568490574.0+y*(-13362590354.0+y*(651619640.7
			+y*(-11214424.18+y*(77392.33017+y*(-184.9052456)))));
		ans2=57568490411.0+y*(1029532985.0+y*(9494680.718
			+y*(59272.64853+y*(267.8532712+y*1.0))));
		ans=ans1/ans2;
	} else {
		z=8.0/ax;
		y=z*z;
		xx=ax-0.785398164;
		ans1=1.0+y*(-0.1098628627e-2+y*(0.2734510407e-4
			+y*(-0.2073370639e-5+y*0.2093887211e-6)));
		ans2 = -0.1562499995e-1+y*(0.1430488765e-3
			+y*(-0.6911147651e-5+y*(0.7621095161e-6
			-y*0.934945152e-7)));
		ans=sqrt(0.636619772/ax)*(cos(xx)*ans1-z*sin(xx)*ans2);
	}
	return ans;
}

float bessj1(float x)
{
	float ax,z;
	double xx,y,ans,ans1,ans2;

	if ((ax=fabs(x)) < 8.0) {
		y=x*x;
		ans1=x*(72362614232.0+y*(-7895059235.0+y*(242396853.1
			+y*(-2972611.439+y*(15704.48260+y*(-30.16036606))))));
		ans2=144725228442.0+y*(2300535178.0+y*(18583304.74
			+y*(99447.43394+y*(376.9991397+y*1.0))));
		ans=ans1/ans2;
	} else {
		z=8.0/ax;
		y=z*z;
		xx=ax-2.356194491;
		ans1=1.0+y*(0.183105e-2+y*(-0.3516396496e-4
			+y*(0.2457520174e-5+y*(-0.240337019e-6))));
		ans2=0.04687499995+y*(-0.2002690873e-3
			+y*(0.8449199096e-5+y*(-0.88228987e-6
			+y*0.105787412e-6)));
		ans=sqrt(0.636619772/ax)*(cos(xx)*ans1-z*sin(xx)*ans2);
		if (x < 0.0) ans = -ans;
	}
	return ans;
}

void bessj0_(float rtn, float *f, float *df){
    *f = bessj0(rtn);
    *df = -bessj1(rtn);
}

#define JMAX 40

float rtbis(float (*func)(float), float x1, float x2, float xacc)
{
	void nrerror(char error_text[]);
	int j;
	float dx,f,fmid,xmid,rtb;

	f=(*func)(x1);
	fmid=(*func)(x2);
	if (f*fmid >= 0.0) nrerror("Root must be bracketed for bisection in rtbis");
	rtb = f < 0.0 ? (dx=x2-x1,x1) : (dx=x1-x2,x2);
	for (j=1;j<=JMAX;j++) {
		fmid=(*func)(xmid=rtb+(dx *= 0.5));
		++num_steps;
		if (fmid <= 0.0) rtb=xmid;
		if (fabs(dx) < xacc || fmid == 0.0) return rtb;
	}
	nrerror("Too many bisections in rtbis");
	return 0.0;
}

#undef JMAX

#define MAXIT 30

float rtflsp(float (*func)(float), float x1, float x2, float xacc)
{
	void nrerror(char error_text[]);
	int j;
	float fl,fh,xl,xh,swap,dx,del,f,rtf;

	fl=(*func)(x1);
	fh=(*func)(x2);
	if (fl*fh > 0.0) nrerror("Root must be bracketed in rtflsp");
	if (fl < 0.0) {
		xl=x1;
		xh=x2;
	} else {
		xl=x2;
		xh=x1;
		swap=fl;
		fl=fh;
		fh=swap;
	}
	dx=xh-xl;
	for (j=1;j<=MAXIT;j++) {
		rtf=xl+dx*fl/(fl-fh);
		f=(*func)(rtf);
		++num_steps;
		if (f < 0.0) {
			del=xl-rtf;
			xl=rtf;
			fl=f;
		} else {
			del=xh-rtf;
			xh=rtf;
			fh=f;
		}
		dx=xh-xl;
		if (fabs(del) < xacc || f == 0.0) return rtf;
	}
	nrerror("Maximum number of iterations exceeded in rtflsp");
	return 0.0;
}
#undef MAXIT

#define MAXIT 30

float rtsec(float (*func)(float), float x1, float x2, float xacc)
{
	void nrerror(char error_text[]);
	int j;
	float fl,f,dx,swap,xl,rts;

	fl=(*func)(x1);
	f=(*func)(x2);
	if (fabs(fl) < fabs(f)) {
		rts=x1;
		xl=x2;
		swap=fl;
		fl=f;
		f=swap;
	} else {
		xl=x1;
		rts=x2;
	}
	for (j=1;j<=MAXIT;j++) {
		dx=(xl-rts)*f/(f-fl);
		xl=rts;
		fl=f;
		rts += dx;
		f=(*func)(rts);
		++num_steps;
		if (fabs(dx) < xacc || f == 0.0) return rts;
	}
	nrerror("Maximum number of iterations exceeded in rtsec");
	return 0.0;
}
#undef MAXIT

#define JMAX 20

float rtnewt(void (*funcd)(float, float *, float *), float x1, float x2,
	float xacc)
{
	void nrerror(char error_text[]);
	int j;
	float df,dx,f,rtn;

	rtn=0.5*(x1+x2);
	for (j=1;j<=JMAX;j++) {
		(*funcd)(rtn,&f,&df);
		dx=f/df;
		rtn -= dx;
		++num_steps;
		if ((x1-rtn)*(rtn-x2) < 0.0)
			nrerror("Jumped out of brackets in rtnewt");
		if (fabs(dx) < xacc) return rtn;
	}
	nrerror("Maximum number of iterations exceeded in rtnewt");
	return 0.0;
}
#undef JMAX

#define MAXIT 100

float rtsafe(void (*funcd)(float, float *, float *), float x1, float x2,
	float xacc)
{
	void nrerror(char error_text[]);
	int j;
	float df,dx,dxold,f,fh,fl;
	float temp,xh,xl,rts;

	(*funcd)(x1,&fl,&df);
	(*funcd)(x2,&fh,&df);
	if ((fl > 0.0 && fh > 0.0) || (fl < 0.0 && fh < 0.0))
		nrerror("Root must be bracketed in rtsafe");
	if (fl == 0.0) return x1;
	if (fh == 0.0) return x2;
	if (fl < 0.0) {
		xl=x1;
		xh=x2;
	} else {
		xh=x1;
		xl=x2;
	}
	rts=0.5*(x1+x2);
	dxold=fabs(x2-x1);
	dx=dxold;
	(*funcd)(rts,&f,&df);
	for (j=1;j<=MAXIT;j++) {
		++num_steps;
		if ((((rts-xh)*df-f)*((rts-xl)*df-f) > 0.0)
			|| (fabs(2.0*f) > fabs(dxold*df))) {
			dxold=dx;
			dx=0.5*(xh-xl);
			rts=xl+dx;
			if (xl == rts) return rts;
		} else {
			dxold=dx;
			dx=f/df;
			temp=rts;
			rts -= dx;
			if (temp == rts) return rts;
		}
		if (fabs(dx) < xacc) return rts;
		(*funcd)(rts,&f,&df);
		if (f < 0.0)
			xl=rts;
		else
			xh=rts;
	}
	nrerror("Maximum number of iterations exceeded in rtsafe");
	return 0.0;
}
#undef MAXIT

void zbrak(float (*fx)(float), float x1, float x2, int n, float xb1[], float xb2[], int *nb)
{
	int nbb,i;
	float x,fp,fc,dx;

	nbb=0;
	dx=(x2-x1)/n;
	fp=(*fx)(x=x1);
	for (i=1;i<=n;i++) {
		fc=(*fx)(x += dx);
		if (fc*fp <= 0.0) {
			xb1[++nbb]=x-dx;
			xb2[nbb]=x;
			if(*nb == nbb) return;

		}
		fp=fc;
	}
	*nb = nbb;
}