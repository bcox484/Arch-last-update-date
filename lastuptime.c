#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 1024
#define END_CHAR 'T'
#define FILENAME "/var/log/pacman.log"

void lastUpgradeString(char *result, FILE *log, const char *update_command);

int main() {
  FILE *log = NULL;
  char result[BUF_SIZE] = {'\0'};
  log = fopen(FILENAME, "r");
  if (log == NULL) {
    printf("Log file could not be opened\n");
    return -1;
  }

  lastUpgradeString(result, log, "full system upgrade");

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
  long size = 0;

  /* Find total size of file */
  fseek(log, 0, SEEK_END);
  size = ftell(log);

  /* Increment through first 20% of log file */
  float i = 0.8;
  fseek(log, (long)floor(size * i), SEEK_SET);
  while (fgets(line, BUF_SIZE, log))
    if (strstr(line, update_command)) {
      strncpy(result, line, BUF_SIZE);
    }

  /* Increment through file in 20% chunks, stop if 'update_command' is found */
  for (i = 0.6; result[0] == '\0'; i -= 0.2) {
    if (i == 0.0)
      break;
    fseek(log, (long)floor(size * i), SEEK_SET);
    long pos = (long)ceil(size * (i + 0.2));

    /* Stop search if file position reaches beginning of last file position */
    while (ftell(log) < pos) {
      fgets(line, BUF_SIZE, log);
      if (strstr(line, update_command)) {
        strncpy(result, line, BUF_SIZE);
        printf("%ld\n", ftell(log));
      }
    }
  }
}
