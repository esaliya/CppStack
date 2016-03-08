// VSTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

using namespace std;

volatile DWORD dwStart;
volatile int global = 0;

double test2(int num_steps) {
	int i;
	global++;
	double x, pi, sum = 0.0, step;

	step = 1.0 / (double)num_steps;

#pragma omp parallel for reduction(+:sum) private(x)
	for (i = 1; i <= num_steps; i++) {
		x = (i - 0.5) * step;
		sum = sum + 4.0 / (1.0 + x*x);
	}

	pi = step * sum;
	return pi;
}

void calcPi(int argc, char* argv[]) {
	double   d;
	int n = 1000000;

	if (argc > 1)
		n = atoi(argv[1]);

	dwStart = GetTickCount();
	d = test2(n);
	printf_s("For %d steps, pi = %.15f, %d milliseconds\n", n, d, GetTickCount() - dwStart);

	dwStart = GetTickCount();
	d = test2(n);
	printf_s("For %d steps, pi = %.15f, %d milliseconds\n", n, d, GetTickCount() - dwStart);
}


int main(int argc, char* argv[])
{
	cout << "hello";
	cout << "Size of int : " << sizeof(int) << endl;

	enum color {red, blue=5, green} c;
	c = red;
	cout << c << blue << green << endl;

	calcPi(argc, argv);

	

	system("pause");
	return 0;
}

