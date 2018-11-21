#include <string.h>
int strdrop(char *str, const char *set) {
    int writtr = 0;
    for (unsigned int seekr = 0; seekr <= strlen(str); str[writtr++] = str[seekr++])
        while (seekr < strlen(str) && strchr(set, str[seekr]) != NULL) seekr++;
    return writtr - 1;
}
