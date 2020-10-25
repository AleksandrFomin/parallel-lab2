#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <sys/time.h>
#include <math.h>
#include "../FW_1.3.1_Lin64/fwSignal.h"

void fill_array(double *arr, int size, double left, double right, unsigned int *seedp)
{
	int i;
	for (i = 0; i < size; i++) {
		arr[i] = rand_r(seedp) / (double)RAND_MAX * (right - left) + left;
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

void map_m1(double *arr, int size)
{
	int i;
	Fw64f *pSrc = arr;
	Fw64f *pDst = malloc(sizeof(Fw64f) * size);
	Fw64f *constArr = malloc(sizeof(Fw64f) * size);
	for (i = 0; i < size; i++) {
		constArr[i] = 1;
	}

	fwsTanh_64f_A53(pSrc, pDst, size);
	fwsSub_64f(constArr, pDst, pSrc, size);
	free(pDst);
	free(constArr);
}

void map_m2(double *arr, int size, double *arr_copy)
{
	Fw64f *pSrc = arr;
	Fw64f *pSrcCopy = arr_copy;
	fwsAdd_64f_I(pSrcCopy, pSrc + 1, size - 1);
	fwsMulC_64f_I(exp(1.0), pSrc, size);
	fwsSqrt_64f_I(pSrc, size);
}

void copy_arr(double *src, int len, double *dst)
{
	int i;
	for (i = 0; i < len; i++)
		dst[i] = src[i];
}

void apply_merge_func(double *m1, double *m2, int m2_len)
{
	Fw64f *pSrc1 = m1;
	Fw64f *pSrc2 = m2;
	Fw64f *pDst = malloc(sizeof(Fw64f) * m2_len);

	fwsSub_64f(pSrc2, pSrc1, pDst, m2_len);
	fwsAbs_64f(pDst, pSrc1, m2_len);
	free(pDst);
}

// Функция "просеивания" через кучу - формирование кучи
void siftDown(double *numbers, int root, int bottom)
{
	int maxChild; // индекс максимального потомка
	int done = 0; // флаг того, что куча сформирована

	// Пока не дошли до последнего ряда
	while ((root * 2 <= bottom) && (!done)) {
		if (root * 2 == bottom)    // если мы в последнем ряду,
			maxChild = root * 2;    // запоминаем левый потомок
		// иначе запоминаем больший потомок из двух
		else if (numbers[root * 2] > numbers[root * 2 + 1])
			maxChild = root * 2;
		else
			maxChild = root * 2 + 1;
		// если элемент вершины меньше максимального потомка
		if (numbers[root] < numbers[maxChild]) {
			double temp = numbers[root]; // меняем их местами
			numbers[root] = numbers[maxChild];
			numbers[maxChild] = temp;
			root = maxChild;
		} else 
			done = 1; // пирамида сформирована
	}
}

// Функция сортировки на куче
void heapSort(double *numbers, int array_size)
{
	// Формируем нижний ряд пирамиды
	for (int i = (array_size / 2) - 1; i >= 0; i--)
		siftDown(numbers, i, array_size - 1);
	// Просеиваем через пирамиду остальные элементы
	for (int i = array_size - 1; i >= 1; i--) {
		double temp = numbers[0];
		numbers[0] = numbers[i];
		numbers[i] = temp;
		siftDown(numbers, 0, i - 1);
	}
}

double min_not_null(double *arr, int len)
{
	int i;
	double min_val = DBL_MAX;
	for (i = 0; i < len; i++) {
		if (arr[i] < min_val && arr[i] > 0)
			min_val = arr[i];
	}
	return min_val;
}

double reduce(double *arr, int len)
{
	int i;
	double min_val = min_not_null(arr, len);
	double x = 0;
	for (i = 0; i < len; i++) {
		if ((int)(arr[i] / min_val) % 2 == 0)
			x += sin(arr[i]);
	}
	return x;
}

int main(int argc, char* argv[])
{
	int i, N;
	struct timeval T1, T2;
	long delta_ms;
	double *M1, *M2, *M2_copy;
	int A = 540;
	unsigned int seed1, seed2;
	// double X;

	fwSetNumThreads(atoi(argv[1]));

	N = atoi(argv[2]); /* N равен второму параметру командной строки */
	gettimeofday(&T1, NULL); /* запомнить текущее время T1 */

	M1 = malloc(sizeof(double) * N);
	M2 = malloc(sizeof(double) * N / 2);
	M2_copy = malloc(sizeof(double) * N / 2);

	for (i = 0; i < 50; i++) /* 50 экспериментов */
	{
		seed1 = i;
		seed2 = i;
		fill_array(M1, N, 1, A, &seed1);
		fill_array(M2, N / 2, A, 10 * A, &seed2);

		// printf("Fill arrays\n");
		// print_array(M1, N);
		// print_array(M2, N / 2);
		
		map_m1(M1, N);
		copy_arr(M2, N / 2, M2_copy);
		map_m2(M2, N / 2, M2_copy);

		// printf("Map\n");
		// print_array(M1, N);
		// print_array(M2, N / 2);

		apply_merge_func(M1, M2, N / 2);
		// printf("Merge\n");
		// print_array(M2, N / 2);

		// heapSort(M2, N / 2);
		// printf("Sort\n");
		// print_array(M2, N / 2);

		reduce(M2, N / 2);
		// printf("X = %f\n", X);
	}
	gettimeofday(&T2, NULL); /* запомнить текущее время T2 */

	delta_ms = 1000 * (T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec) / 1000;
	printf("%d %ld\n", N, delta_ms); /* T2 - T1 */

	free(M1);
	free(M2);
	free(M2_copy);

	return 0;
}