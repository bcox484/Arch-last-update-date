#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 1024
#define END_CHAR 'T'
#define PARTIAL_GUARD 64
#define FILENAME "/var/log/pacman.log"
#define SEARCH_STRING "full system upgrade"

void lastUpgradeString(char *result, FILE *log, const char *update_command);

int main() {
  FILE *log = NULL;
  char result[BUF_SIZE] = {'\0'};
  log = fopen(FILENAME, "r");
  if (log == NULL) {
    printf("Log file could not be opened\n");
    return -1;
  }

  lastUpgradeString(result, log, SEARCH_STRING);

  fclose(log);
  if (result[0] == '\0') {
    printf("Update not found\n");
    return -1;
  }

  /* Copy memory address of result to date, increment through date */
  char *date = result;
  for (date++; *date != END_CHAR; date++)
    printf("%c", *date);

  printf("\n");
  return 0;
}

/* Search file bottom up in 1/5 chunks for last update in log */
void lastUpgradeString(char *result, FILE *log, const char *update_command) {
  char line[BUF_SIZE] = "";
  unsigned int size = 0;

  /* Find total size of file */
  fseek(log, 0, SEEK_END);
  size = ftell(log);

  float i = 0.8;
  /* Search through the last 20% of log file for the `update_command' */
  fseek(log, floor(size * i), SEEK_SET);
  while (fgets(line, BUF_SIZE, log))
    if (strstr(line, update_command))
      strncpy(result, line, BUF_SIZE);

  /* Increment through file in 20% chunks, stop if 'update_command' is found */
  for (i = 0.6; result[0] == '\0' || i > 0.0; i -= 0.2) {
    fseek(log, floor(size * i), SEEK_SET);
    unsigned int pos = ftell(log);

    /* Stop search if file position reaches beginning of last file position */
    while ((ftell(log) < pos + PARTIAL_GUARD)) {
      fgets(line, BUF_SIZE, log);
      if (strstr(line, update_command)) {
        strncpy(result, line, BUF_SIZE);
      }
    }
  }
}
