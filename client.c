#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "objstore.h"

//#define ERRSTR objstore_errstr
#define test_str "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Praesent iaculis arcu eu tempor cras amet.\n"
#define test_strlen 100

static int failed_tests = 0;

//Create 20 strings, with sizes ranging from 55 * 100 to
char *repeat_str(int n) {
    char *repeat = (char*)calloc(test_strlen * n, sizeof(char));
    int seek = 0;
    while (seek < test_strlen * n - 100) {
        memcpy(repeat + seek, test_str, 100);
        seek = seek + test_strlen;
    }
	memcpy(repeat + seek, test_str, 99);
    return repeat;
}

int test1() {
    if (!os_store("Object1", test_str, test_strlen)) {
        failed_tests++;
        return 0;
    }
    for (int i = 1; i <= 18; i++) {
        char name[128];
        memset(name, 0, 128);
        sprintf(name, "Object%d", i + 1);
        char *str = repeat_str(i * 55);
        if (!os_store(name, str, i * test_strlen * 55)) {
            free(str);
            failed_tests++;
            return 0;
        }
        free(str);
    }
    char *str = repeat_str(1000);
    if (!os_store("Object20", str, 1000 * test_strlen)) {
        free(str);
        failed_tests++;
        return 0;
    }
    free(str);
    os_disconnect();
    return 1;
}

int test2() {
    char *str_first = os_retrieve("Object1");
    if (!(str_first && strlen(str_first) == 100)) {
        failed_tests++;
        free(str_first);
        return 0;
    }
    free(str_first);
    for (int i = 2; i <= 19; i++) {
        char name[128];
        memset(name, 0, 128);
        sprintf(name, "Object%d", i);
        char *str = os_retrieve(name);
        if (!(str && strlen(str) + 1 == (i - 1) * test_strlen * 55)) {
            failed_tests++;
            free(str);
            return 0;
        }
        free(str);
    }
    char *str_last = os_retrieve("Object20");
    if (!(str_last && strlen(str_last) + 1 == 100000)) {
        failed_tests++;
        free(str_last);
        return 0;
    }
    free(str_last);
    os_disconnect();
    return 1;
}

int test3() {
    for (int i = 1; i <= 20; i++) {
        char name[128];
        memset(name, 0, 128);
        sprintf(name, "Object%d", i);
        if (!os_delete(name)) {
            failed_tests++;
            os_disconnect();
            return 0;
        };
    }
    os_disconnect();
    return 1;
}

int main(int argc, char *argv[]) {
    if (os_connect(argv[1]) && argv[2]) {
        switch (atoi(argv[2])) {
            case 1:
                printf("Test passati: %d\n", 20 - failed_tests);
                printf("Test fallito: %d\n", failed_tests);
                return !test1();
                break;

            case 2:
                printf("Test passati: %d\n", 20 - failed_tests);
                printf("Test fallito: %d\n", failed_tests);
                return !test2();
                break;

            case 3:
                printf("Test passati: %d\n", 20 - failed_tests);
                printf("Test fallito: %d\n", failed_tests);
                return !test3();
                break;

            default:
                break;
        }
    } else return 1;
}