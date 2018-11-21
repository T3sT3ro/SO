#include <string.h>
// assume the set has unique symbols
int strcnt(const char *str, const char *set) {
    int cnt = 0;
    for (unsigned int i = 0; i < strlen(set); i++, cnt += strchr(str, set[i]) != NULL)
        ;
    return cnt;
}
