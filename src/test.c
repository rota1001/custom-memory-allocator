#include "allocator.h"
#include "cpucycles.h"
#include <stdlib.h>
#include <time.h>

#define PERFTEST(func, ...)  \
    ({                       \
        int64_t start, end;  \
        start = cpucycles(); \
        func(__VA_ARGS__);   \
        end = cpucycles();   \
        end - start;         \
    })

#define TEST_NUM 1000000
uint64_t sum[2][TEST_NUM];


int main()
{
    allocator_t *allocator = alloc_create(0x100000);
    char *a[1000];
    char *b[1000];
    uint64_t sum_alloc = 0;
    uint64_t sum_malloc = 0;
    int num = 0;
    srand(time(NULL));
    for (int  i = 0; i < 1000; i++) {
        a[i] = NULL;
        b[i] = NULL;
    }

    for(int i = 0; i < TEST_NUM; i++) {
        int k = rand() % 1000;
        if (rand() & 1) {
            if (a[k]) {
                sum_alloc += PERFTEST(alloc_free, allocator, a[k]);
                sum[0][i] = sum_alloc;
                a[k] = NULL;
            } else {
                i--;
                continue;
            }
            if (b[k]) {
                sum_malloc += PERFTEST(free, a[k]);
                sum[1][i] = sum_malloc;
            }
        }else{
            size_t si = rand() % 1000;
            uint64_t st, en;
            if (a[k])
                alloc_free(allocator, a[k]);
            st = cpucycles();
            a[k] = alloc_alloc(allocator, si);
            en = cpucycles();
            sum_alloc += en - st;
            if (!a[k])
                num++;

            st = cpucycles();
            b[k] = malloc(si);
            en = cpucycles();
            sum_malloc += en - st;

            sum[0][i] = sum_alloc;
            sum[1][i] = sum_malloc;
        }
    }
    printf("malloc:\t\t%ld\n", sum_malloc);
    printf("custom-alloc:\t%ld\n", sum_alloc);
    FILE *file[2];
    file[0] = fopen("file0.txt", "w");
    file[1] = fopen("file1.txt", "w");
    for (int i = 0; i < TEST_NUM; i++) {
        fprintf(file[0], "%lu\n", sum[0][i]);
        fprintf(file[1], "%lu\n", sum[1][i]);
    }
    printf("%d\n", num);
    fclose(file[0]);
    fclose(file[1]);
}