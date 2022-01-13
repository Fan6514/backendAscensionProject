/* preprocessor.c */
#include <stdio.h>

int main()
{
    printf("[INFO] %d-%d %s:%d", __DATE__, __TIME__, __FILE__, __LINE__);
    return 0;
}