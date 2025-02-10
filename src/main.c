#define _DEFAULT_SOURCE

#include "alloc.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define HEADER_SIZE (sizeof(struct header))

static int SUCCESS_CASES = 0;
static int TOTAL_CASES = 246;
static int TOTAL_SCORE = 0;

void print_test_result() {
  char buf[48];
  snprintf(buf, 48, "Score: %d, Success cases: %d/%d\n", TOTAL_SCORE,
           SUCCESS_CASES, TOTAL_CASES);
  write(STDOUT_FILENO, buf, strlen(buf));
}

void TEST(int line, bool f, int pts) {
  if (f) {
    SUCCESS_CASES += 1;
    TOTAL_SCORE += pts;
  } else {
    char buf[32];
    snprintf(buf, 32, "Test failed at line %d\n", line);
    write(STDOUT_FILENO, buf, strlen(buf));
  }
  print_test_result();
}

void test_correct_space_allocation() {
  struct allocinfo info;
  void *p[11] = {NULL};
  for (int i = 0; i < 11; i++) {
    int size = sizeof(int32_t) * 2;
    int32_t *ptr = (int32_t *)alloc(size);

    if (ptr != NULL) {
      ptr[0] = 0;
      ptr[1] = 1;
      if (i < 4) {
        TEST(__LINE__,
             *(uint64_t *)((char *)ptr - HEADER_SIZE) == size + HEADER_SIZE, 1);
        info = allocinfo();
        TEST(__LINE__,
             info.free_size ==
                 INCREMENT - HEADER_SIZE - ((size + HEADER_SIZE) * (i + 1)),
             1);
      } else if (i == 9) {
        TEST(__LINE__, *(uint64_t *)((char *)ptr - HEADER_SIZE) == 40, 1);
        info = allocinfo();
        TEST(__LINE__,
             info.free_size ==
                 INCREMENT - HEADER_SIZE - ((size + HEADER_SIZE) * (i + 1)),
             1);
      }
    }
    p[i] = ptr;
  }
  for (int i = 5; i < 11; i++) {
    if (i < 10) {
      TEST(__LINE__, p[i] != NULL, 1);
    } else {
      TEST(__LINE__, p[i] == NULL, 5);
    }
  }
  info = allocinfo();
  TEST(__LINE__, info.free_size == 0, 5);
  for (int i = 5; i < 11; i++) {
    int32_t *ptr = (int32_t *)p[i];
    if (ptr != NULL) {
      TEST(__LINE__, ptr[0] == 0, 1);
      TEST(__LINE__, ptr[1] == 1, 1);
    }
  }

  for (int i = 0; i < 11; i++) {
    if (p[i] != NULL) {
      dealloc(p[i]);
      p[i] = NULL;
    }
  }
  info = allocinfo();
  TEST(__LINE__, info.free_size == 240, 5);
}

void test_grow() {
  struct allocinfo info;
  void *p[40] = {NULL};
  for (int i = 0; i < 40; i++) {
    int size = sizeof(int32_t) * 2;
    int32_t *ptr = (int32_t *)alloc(size);

    ptr[0] = 0;
    ptr[1] = 1;
    if (i >= 30 && i < 40) {
      if (i < 39)
        TEST(__LINE__,
             *(uint64_t *)((char *)ptr - HEADER_SIZE) == size + HEADER_SIZE, 1);
      else if (i == 39)
        TEST(__LINE__, *(uint64_t *)((char *)ptr - HEADER_SIZE) == 40, 1);

      info = allocinfo();
      TEST(__LINE__,
           info.free_size ==
               INCREMENT - HEADER_SIZE - ((size + HEADER_SIZE) * (i % 10 + 1)),
           1);
    }
    p[i] = ptr;
  }
  for (int i = 35; i < 40; i++) {
    TEST(__LINE__, p[i] != NULL, 1);
    int32_t *ptr = (int32_t *)p[i];
    TEST(__LINE__, ptr[0] == 0, 1);
    TEST(__LINE__, ptr[1] == 1, 1);
  }

  for (int i = 0; i < 40; i++) {
    dealloc(p[i]);
    p[i] = NULL;
  }
  info = allocinfo();
  TEST(__LINE__, info.free_size == 1008, 5);
}

void test_sbrk() {
  char *base = sbrk(0);
  void *ptr = alloc(1);
  TEST(__LINE__, ptr != NULL && sbrk(0) == base + INCREMENT, 2);
  ptr = alloc(1);
  TEST(__LINE__, ptr != NULL && sbrk(0) == base + INCREMENT, 2);
  ptr = alloc(256);
  TEST(__LINE__, ptr != NULL && sbrk(0) == base + INCREMENT * 2, 2);
  alloc(256);
  TEST(__LINE__, ptr != NULL && sbrk(0) == base + INCREMENT * 3, 2);
  alloc(1);
  TEST(__LINE__, ptr != NULL && sbrk(0) == base + INCREMENT * 3, 2);
}

void test_first_fit_algorithm() {
  void *p[11] = {NULL};
  for (int i = 0; i < 11; i++) {
    p[i] = alloc(4);
  }
  for (int i = 0; i < 10; i++) {
    unsigned long j = (char *)p[i + 1] - (char *)p[i];
    TEST(__LINE__, j == 20, 1);
  }

  dealloc(p[1]);
  dealloc(p[3]);
  dealloc(p[5]);

  TEST(__LINE__, alloc(3) == p[5], 10);
  TEST(__LINE__, alloc(3) == p[3], 10);
  TEST(__LINE__, alloc(3) == p[1], 10);
}

void test_first_fit(int i) {
  if (i == 0) {
    allocopt(FIRST_FIT, 256);
    test_correct_space_allocation();
  } else if (i == 1) {
    allocopt(FIRST_FIT, 256);
    test_first_fit_algorithm();
  } else if (i == 2) {
    allocopt(FIRST_FIT, 1024);
    test_grow();
  } else if (i == 3) {
    allocopt(FIRST_FIT, 768);
    test_sbrk();
  }
}

void test_best_fit_algorithm() {
  void *p[11] = {NULL};
  for (int i = 0; i < 11; i++) {
    p[i] = alloc(i + 1);
  }
  for (int i = 0; i < 10; i++) {
    unsigned long j = (char *)p[i + 1] - (char *)p[i];
    TEST(__LINE__, j == (HEADER_SIZE + i + 1), 1);
  }

  dealloc(p[1]);
  dealloc(p[3]);
  dealloc(p[5]);
  dealloc(p[7]);

  TEST(__LINE__, alloc(7) == p[7], 10);
  TEST(__LINE__, alloc(5) == p[5], 10);
  TEST(__LINE__, alloc(3) == p[3], 10);
}

void test_best_fit(int i) {
  if (i == 0) {
    allocopt(BEST_FIT, 256);
    test_correct_space_allocation();
  } else if (i == 1) {
    allocopt(BEST_FIT, 256);
    test_best_fit_algorithm();
  } else if (i == 2) {
    allocopt(BEST_FIT, 1024);
    test_grow();
  } else if (i == 3) {
    allocopt(BEST_FIT, 768);
    test_sbrk();
  }
}

void test_worst_fit_algorithm() {
  void *p[11] = {NULL};
  for (int i = 0; i < 11; i++) {
    p[i] = alloc(i + 1);
  }
  for (int i = 0; i < 10; i++) {
    unsigned long j = (char *)p[i + 1] - (char *)p[i];
    TEST(__LINE__, j == (HEADER_SIZE + i + 1), 1);
  }

  dealloc(p[1]);
  dealloc(p[3]);
  dealloc(p[5]);
  dealloc(p[7]);

  TEST(__LINE__, alloc(3) == p[7], 10);
  TEST(__LINE__, alloc(3) == p[5], 10);
  TEST(__LINE__, alloc(3) == p[3], 10);
}

void test_worst_fit(int i) {
  if (i == 0) {
    allocopt(WORST_FIT, 256);
    test_correct_space_allocation();
  } else if (i == 1) {
    allocopt(WORST_FIT, 256);
    test_worst_fit_algorithm();
  } else if (i == 2) {
    allocopt(WORST_FIT, 1024);
    test_grow();
  } else if (i == 3) {
    allocopt(WORST_FIT, 768);
    test_sbrk();
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    for (int i = 0; i < 4; i++) {
      test_first_fit(i);
      test_best_fit(i);
      test_worst_fit(i);
    }
  } else {
    int caseIndex = atoi(argv[1]);
    int testIndex = atoi(argv[2]);
    if (caseIndex == 0) {
      test_first_fit(testIndex);
    } else if (caseIndex == 1) {
      test_best_fit(testIndex);
    } else if (caseIndex == 2) {
      test_worst_fit(testIndex);
    }
  }

  return 0;
}
