#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define BUF_SIZE 2048

struct encoding_unit {
  char ch;
  unsigned int amount;
};

int flush_buffer(struct encoding_unit encoding_buf[], unsigned int buf_size) {
  for (unsigned int flush_i = 0; flush_i < buf_size; flush_i++) {
    fwrite(&encoding_buf[flush_i].amount, sizeof(encoding_buf[flush_i].amount),
           1, stdout);
    fwrite(&encoding_buf[flush_i].ch, sizeof(encoding_buf[flush_i].ch), 1,
           stdout);
  }
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("wzip: file1 [file2 ...]\n");
    exit(1);
  }

  struct encoding_unit encoding_buf[BUF_SIZE];
  memset(&encoding_buf, 0, sizeof(encoding_buf));
  unsigned int buf_i = 0;

  for (int arg_i = 1; arg_i < argc; arg_i++) {
    FILE *input_file = fopen(argv[arg_i], "r");

    if (input_file == NULL) {
      printf("wzip: cannot open file\n");
      exit(1);
    }

    char *line = NULL;
    size_t line_size = 0;
    int chars_read;

    // getting one line at a time, each line contains \n at the end
    while ((chars_read = getline(&line, &line_size, input_file)) != -1) {
      for (int cur_char_i = 0; cur_char_i < chars_read; cur_char_i++) {

        int char_count = 1;
        int char_itr = cur_char_i + 1;
        while (char_itr < chars_read && line[cur_char_i] == line[char_itr]) {
          char_itr++;
          char_count++;
        }

        if (cur_char_i == 0 && buf_i > 0 &&
            encoding_buf[buf_i - 1].ch == line[cur_char_i]) {
          buf_i -= 1;
          encoding_buf[buf_i].amount += char_count;
        } else {
          encoding_buf[buf_i].amount = char_count;
          encoding_buf[buf_i].ch = line[cur_char_i];
        }
        buf_i++;

        if (buf_i == BUF_SIZE) {
          flush_buffer(encoding_buf, buf_i);
          buf_i = 0;
        }
        cur_char_i =
            char_itr - 1; // skipping the same characters and decrementing by
                          // one since the for loop will increment it again
      }
    }

    fclose(input_file);
  }

  if (buf_i != 0) {
    flush_buffer(encoding_buf, buf_i);
  }

  return 0;
}
