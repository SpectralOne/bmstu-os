/*https://medium.com/@SravanthiSinha/hacking-the-virtual-memory-416edf62a496*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
     size_t i = 0;
     char *s = strdup("Mem is incredible?");
     if (s == NULL) {
          fprintf(stderr, "Can't allocate mem with malloc\n");
          return EXIT_FAILURE;
     }
     while (1) {
          printf("[%lu] %s\n", i, s);
          sleep(1);
          i++;
     }
     return EXIT_SUCCESS;
}
