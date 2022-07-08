#include "utility.h"

int8_t util_SpaceSearch(char *str) {
    for (int8_t k = 23; k >= 0; k--) {
        if (str[k] == ' ') {
            return k + 1;
        }
    }
    return 24;
}
