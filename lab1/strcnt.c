// assume string and set null terminated
#include <stdlib.h>
#include <string.h>
int strcnt(char* const str, const char* const set) {
    int writtr = 0;
    for (int seekr = 0; seekr <= strlen(str); str[writtr++] = str[seekr++])
        while (seekr < strlen(str) && strchr(set, str[seekr]) != NULL) seekr++;
    return writtr - 1;
}
