#include <bits/posix2_lim.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc == 1) {
    return 0;
  }

  for (int i = 1; i < argc; i++) {
    FILE *cur_file = NULL;
    if ((cur_file = fopen(argv[i], "r")) == NULL) {
      printf("wcat: cannot open file\n");
      exit(1);
    }

    char *cur_line = malloc(LINE_MAX + 1);
    if (cur_line == NULL) {
      perror("malloc");
      exit(1);
    }

    while (fgets(cur_line, LINE_MAX + 1, cur_file) != NULL) {
      printf("%s", cur_line);
    }

    fclose(cur_file);
  }

  return 0;
}
