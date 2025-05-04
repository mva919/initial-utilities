#include <stdio.h>
#include <stdlib.h>

struct encoding_unit {
  char ch;
  unsigned int amount;
};

#define UNIT_SIZE 5

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("wunzip: file1 [file2 ...]\n");
    exit(1);
  }

  for (int arg_i = 1; arg_i < argc; arg_i++) {
    FILE *compressed_file = fopen(argv[arg_i], "rb");
    if (compressed_file == NULL) {
      printf("wunzip: cannot open file\n");
      exit(1);
    }

    void *buf = malloc(UNIT_SIZE);
    if (buf == NULL) {
      perror("malloc");
      exit(1);
    }

    while (fread(buf, UNIT_SIZE, 1, compressed_file) != 0) {
      int amount = *(int *)(buf);
      char ch = *(char *)(buf + 4);

      for (int i = 0; i < amount; i++) {
        printf("%c", ch);
      }
    }
  }

  return 0;
}
