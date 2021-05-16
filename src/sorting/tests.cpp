#include <assert.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <utility>
#include "utils.h"
#include "sorting.h"
#include <fstream>

// quick sequential, quick par tasks, quick par sections
#define NUM_RUNS 3

template <class T>
int validate_sort(int n, T* data) {
    int i;
    for (i = 0; i < n - 1; i++) {
        if (data[i] > data[i + 1]) {
            return 0;
        }
    }
    return 1;
}

double* get_data(int n) {
    double* data = (double*)malloc(sizeof(double) * n);
    for (int i = 0; i < n; i++) {
        data[i] = (double)rand() * (1 << 20) / RAND_MAX;
    }
    return data;
}

double time_func(int n, void func_to_time(int, int, double*)) {
    double* data = get_data(n);
    double st = omp_get_wtime();
    func_to_time(0, n - 1, &data[0]);
#pragma omp barrier
    double et = omp_get_wtime();
    assert(validate_sort(n, data) == 1);
    free(data);
    return et - st;
}

std::string get_date() {
    // returns a pretty formatted date.
    std::time_t rawtime;
    std::tm *timeinfo;
    char buffer[80];

    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);
    std::strftime(buffer, 80, "%Y-%m-%d-%H-%M-%S", timeinfo);
    return std::string(buffer);
}

void saveStringToFile(std::string s, std::string file_suffix) {
    auto name = get_date()+"__"+file_suffix;
    std::ofstream file;
    file.open("data/" + name + ".csv");
    file << s;
    file.close();
}

int main(int argc, char* argv[]) {
    int n = 1000;
    double* data;
    omp_set_nested(1);
    int num_threads = omp_get_num_procs();
    omp_set_num_threads(num_threads);
    double st, et;
    double timeSeq = 0, timePar = 0;
    srand(123456);
    char buffer[1000];

    void (*functions[NUM_RUNS])(int, int, double*) = {
        seq_qsort, par_q_sort_tasks, par_q_sort_sections,
        };
    char names[NUM_RUNS][50] = {
        "Sequential Quick sort",
        "Parallel Quick sort (Tasks)",
        "Parallel Quick sort (Sections)",
        };

    int num_iters = 10;
    std::string alls = "";
    sprintf(buffer, "%-50s,%-50s,%-50s,%-50s\n",
        "N","Name", "Time (s)", "Speedup over sequential");
    alls += buffer;
    for (int n = 1e2; n <= 1e5; n *= 10){
        double times[NUM_RUNS] = {0};
        for (int iters = 0; iters < num_iters; ++iters) {
            printf("\rN = %d, iter = %d/%d", n, iters+1, num_iters); fflush(stdout);
            for (int i = 0; i < NUM_RUNS; ++i) {
                times[i] += time_func(n, functions[i]) / ((double)num_iters);
            }
        }

        for (int i = 0; i < NUM_RUNS; ++i) {
            sprintf(buffer, "%-50d,%-50s,%-50f,%-50f\n",
                n, names[i], times[i], times[(i / 3) * 3] / times[i]);
            alls += buffer;
        }
    }
    printf("\n");
    saveStringToFile(alls, "");
    return 0;
}
