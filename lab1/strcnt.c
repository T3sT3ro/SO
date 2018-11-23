#include <string.h>
// assume the set has unique symbols; strchr() expands as __builtin_strchr()
int strcnt(const char *str, const char *set) {
    int cnt = 0;
    for (unsigned int i = -1; ++i < strlen(set); cnt += (strchr(str, set[i]) != NULL))
        ;
    return cnt;
}
