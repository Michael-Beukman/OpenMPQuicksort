#include <omp.h>
#include "defines.h"
template <class T>
int partition(int p, int r, T* data){
    T pivot = data[p];
    int left = p;
    int right = r;
    while (left < right){
        // increase until you get to a point where the element is greater that the pivot
        while (left < r && data[left] <= pivot) ++left;
        // increase until you get to a point where the element is less or equal to the pivot
        while (right >= 0 && data[right] > pivot) --right;
        // swap if within bounds
        if (left < right && left <r && right >= 0){
            std::swap(data[left], data[right]);
        }
    }

    // swap at the end
    if (left < right && left <r && right >= 0){
        std::swap(data[left], data[right]);
    }
    data[p] = data[right];
    data[right] = pivot;
    return right;
}

template <class T>
void seq_qsort(int p, int r, T* data) {
    if (p < r) {
        int q = partition<T>(p, r, data);
        seq_qsort(p, q - 1, data);
        seq_qsort(q + 1, r, data);
    }
}
template <class T>
void q_sort_tasks(int p, int r, T* data, int low_limit) {
    if (p < r) {
        if (r - p < low_limit) 
        {
            // small list => sequential.
            return seq_qsort<T>(p, r, data);
        }else{

            int q = partition<T>(p, r, data);
            // create two tasks
            #pragma omp task shared(data)
                q_sort_tasks<T>(p, q - 1, data, low_limit);
            #pragma omp task shared(data)
                q_sort_tasks<T>(q + 1, r, data, low_limit);
        }
    } 
}

template <class T>
void par_q_sort_tasks(int p, int r, T* data){
    #pragma omp parallel
        {
            #pragma omp single
            q_sort_tasks<T>(p, r, data, TASK_LIMIT - 1);
            #pragma omp taskwait
        }
}

template <class T>
void q_sort_sections(int p, int r, T* data, int low_limit) {
    if (p < r) {
        if (r - p < low_limit) 
        {
            return seq_qsort<T>(p, r, data);
        }else{
            int q = partition<T>(p, r, data);
            // only 2 threads because we only have 2 things to do in this recursive call.
            #pragma omp parallel sections shared(data) num_threads(2)
            {
                #pragma omp section
                q_sort_sections<T>(p, q - 1, data, low_limit);
                #pragma omp section
                q_sort_sections<T>(q + 1, r, data, low_limit);
            }
        }
    } 
}


template <class T>
void par_q_sort_sections(int p, int r, T* data){
    q_sort_sections<T>(p, r, data, SECTION_LIMIT - 1); // we have to use this, because our quicksort uses an exclusive upper bound
}