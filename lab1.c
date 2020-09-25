#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

void fill_array(double *arr, int size, double from, double to, unsigned int *seedp)
{
	int i;
	for (i = 0; i < size; i++) {
		arr[i] = rand_r(seedp) * (to - from) / RAND_MAX + from;
	}
}

void print_array(double *arr, int size)
{
	int i;
	printf("arr=[");
	for (i = 0; i < size; i++) {
		printf(" %f", arr[i]);
	}
	printf("]\n");
}

int main(int argc, char* argv[])
{
	int i, N;
	struct timeval T1, T2;
	long delta_ms;
	double *M1;
	int A = 540;
	unsigned int seed;

	N = atoi(argv[1]); /* N равен первому параметру командной строки */
	gettimeofday(&T1, NULL); /* запомнить текущее время T1 */

	M1 = malloc(sizeof(double) * N);
	M2 = malloc(sizeof(double) * N / 2);

	for (i = 0; i < 50; i++) /* 50 экспериментов */
	{
		seed = i;
		fill_array(M1, N, 1, A, &seed);
		fill_array(M2, N, A, 10 * A, &seed);

		// srand(i); /* инициализировать начальное значение ГСЧ */
		/* Решить поставленную задачу, заполнить массив с результатами
		*/
		/* Отсортировать массив с результатами указанным методом */
	}
	gettimeofday(&T2, NULL); /* запомнить текущее время T2 */

	delta_ms = 1000 * (T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec) / 1000;
	printf("\nN=%d. Milliseconds passed: %ld\n", N, delta_ms); /* T2 - T1 */

	return 0;
}