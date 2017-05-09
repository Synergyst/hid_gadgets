#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

char *c;

void sendSingleChar(char *mechar) {
  char mahchar[48];
  char strbuf[48];

  for (int i = 0; i < strlen(mechar); i++) {
    strcpy(strbuf, "echo ");
    sprintf(mahchar, "%d", mechar[i]);
    strcat(strbuf, mahchar);
    strcat(strbuf, " | hid_gadget_test /dev/hidg0 keyboard");
    printf("%d: %c(char), %d(ASCII dec), %d(exit code)\n", i, mechar[i], mechar[i], system(strbuf));
  }
}

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("Usage: %s <string/char to send> [modifier(s)]\n", argv[0]); // TODO: accept modifiers
    exit(EXIT_FAILURE);
  }

  c = argv[1];

  sendSingleChar(c);
  exit(EXIT_SUCCESS);
}
