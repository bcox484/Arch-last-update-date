#include <math.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 1024
#define END_CHAR 'T'
#define FILENAME "/var/log/pacman.log"
#define SEARCH_ITEM "full system upgrade"

void lastUpgradeString(char *result, FILE *log, const char *upgrade_string);
void findStr(char *line, char *result, const char *upgrade_string);

int main() {
  FILE *log = NULL;
  char result[BUF_SIZE] = {'\0'};
  log = fopen(FILENAME, "r");
  if (log == NULL) {
    printf("Log file could not be opened\n");
    return -1;
  }

  lastUpgradeString(result, log, SEARCH_ITEM);

  fclose(log);
  if (result[0] == '\0') {
    printf("Update not found\n");
    return -1;
  }

  /* Copy memory address of result to date, increment through date */
  char *date = result;
  for (date++; *date != END_CHAR && *date != '\0'; date++)
    printf("%c", *date);

  printf("\n");
  return 0;
}

void findStr(char *line, char *result, const char *upgrade_string) {
  if (strstr(line, upgrade_string)) {
    strncpy(result, line, BUF_SIZE);
    result[BUF_SIZE - 1] = '\0';
  }
}

/* Search file bottom up in 1/5 chunks for last update in log */
void lastUpgradeString(char *result, FILE *log, const char *upgrade_string) {
  char line[BUF_SIZE] = "";
  long size = 0;

  /* Find total size of file */
  fseek(log, 0, SEEK_END);
  size = ftell(log);

  /* Increment through first 20% of log file */
  float i = 0.8;
  fseek(log, (long)floor(size * i), SEEK_SET);
  while (fgets(line, BUF_SIZE, log) != NULL)
    findStr(line, result, upgrade_string);

  /* Go through file in 20% chunks, stop if 'update_command' is found */
  for (i = 0.6; result[0] == '\0' && i >= 0.0; i -= 0.2) {
    fseek(log, (long)floor(size * i), SEEK_SET);
    long previous_pos = (long)ceil(size * (i + 0.2));

    /* Stop search if file position reaches beginning of last file position */
    while (ftell(log) <= (previous_pos + strlen(upgrade_string)) &&
           fgets(line, BUF_SIZE, log) != NULL) {
      findStr(line, result, upgrade_string);
    }
  }
}
