#ifndef REGRESSION_TEST_SUPPORT_H
#define REGRESSION_TEST_SUPPORT_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *sCaseFilter;
static const char *sCurrentCase;
static int sListCases, sCasesRun, sCasesFailed;

// Initialize once per fixture process; static case state starts zero-initialized.
static void test_init(int argc, char **argv) {
    setvbuf(stdout, NULL, _IONBF, 0);
    if (argc == 2 && strcmp(argv[1], "--list") == 0) {
        sListCases = 1;
    } else if (argc == 3 && strcmp(argv[1], "--case") == 0) {
        sCaseFilter = argv[2];
    } else if (argc != 1) {
        fprintf(stderr, "HARNESS ERROR: invalid test arguments\n");
        exit(2);
    }
}

static int test_selected(const char *name) {
    if (sListCases) {
        puts(name);
        return 0;
    }
    if (sCaseFilter && strcmp(name, sCaseFilter)) {
        return 0;
    }
    sCurrentCase = name;
    sCasesRun++;
    return 1;
}

static void test_report(const char *name, int ok) {
    printf("%s %s\n", ok ? "PASS" : "FAIL", name);
    sCasesFailed += !ok;
}

static int test_finish(void) {
    if (!sListCases && !sCasesRun) {
        fprintf(stderr, "HARNESS ERROR: no case matched %s; use --list\n", sCaseFilter);
        return 2;
    }
    return sCasesFailed ? 1 : 0;
}

// Integer checks evaluate each operand once and return 0 from the test helper on failure.
// Pass that helper result to test_report so the case failure is counted.
#define EXPECT_EQ(actual, expected)                                                                    \
    do {                                                                                               \
        long long got_ = (long long) (actual), want_ = (long long) (expected);                         \
        if (got_ != want_) {                                                                           \
            fprintf(stderr, "%s:%d: %s: %s: expected %lld, observed %lld\n", __FILE__, __LINE__,       \
                    sCurrentCase, #actual, want_, got_);                                               \
            return 0;                                                                                  \
        }                                                                                              \
    } while (0)
#endif
