#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("wgrep: searchterm [file ...]\n");
    exit(1);
  }

  FILE *input_file = stdin;
  if (argc == 3) {
    input_file = fopen(argv[2], "r");
    if (input_file == NULL) {
      printf("wgrep: cannot open file\n");
      exit(1);
    }
  }

  char *search_str = argv[1];
  char *cur_line = NULL;
  size_t line_size = 0;
  int chars_read;
  int search_str_size = strlen(search_str);

  while ((chars_read = getline(&cur_line, &line_size, input_file)) != -1) {
    // if the current line is shorter than what we are looking for skip it
    if (search_str_size > chars_read) {
      continue;
    }

    for (int i = 0; i < chars_read - search_str_size; i++) {
      int j = 0;
      bool found = true;

      for (; j < search_str_size; j++) {
        if (search_str[j] != cur_line[i + j]) {
          found = false;
          break;
        }
      }
      if (found) {
        printf("%s", cur_line);
        break;
      }
    }
  }

  free(cur_line);
  if (input_file != stdin) {
    fclose(input_file);
  }

  return 0;
}
