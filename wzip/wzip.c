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

// writes all of the data in the buffer up to buf_size - 1 to stdout
// writes in batches of 5 bytes:
// - 4 bytes for the amount (int)
// - 1 byte for the character
void flush_buffer(struct encoding_unit encoding_buf[], unsigned int buf_size) {
  for (unsigned int flush_i = 0; flush_i < buf_size; flush_i++) {
    fwrite(&encoding_buf[flush_i].amount, sizeof(encoding_buf[flush_i].amount),
           1, stdout);
    fwrite(&encoding_buf[flush_i].ch, sizeof(encoding_buf[flush_i].ch), 1,
           stdout);
  }
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
    // count the number of instances each character occcurs and store it
    // in a buffer then move on to next character, if at any moment the buffer
    // fills up we flush it to stdout and reset the size of the buffer
    while ((chars_read = getline(&line, &line_size, input_file)) != -1) {
      for (int cur_char_i = 0; cur_char_i < chars_read; cur_char_i++) {

        int char_count = 1;
        int char_itr = cur_char_i + 1;
        while (char_itr < chars_read && line[cur_char_i] == line[char_itr]) {
          char_itr++;
          char_count++;
        }

        // if we are dealing with multiple files and file 1 ends with char a (no
        // LF) and file 2 also starts with char a we want to encode it together
        // (this loose track of start and end of files - which is what we want)
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
          flush_buffer(
              encoding_buf,
              buf_i); // FIXME: if we run into the situation described by the
                      // comment above and we flush the buffer before going to
                      // the new file we won't encode the chars together (edge
                      // case) - more frequent with small buffer size
          buf_i = 0;
        }
        cur_char_i =
            char_itr - 1; // skipping the same characters and decrementing by
                          // one since the for loop will increment it again
      }
    }

    fclose(input_file);
  }

  // flush out any remaining data in buffer
  if (buf_i != 0) {
    flush_buffer(encoding_buf, buf_i);
  }

  return 0;
}
