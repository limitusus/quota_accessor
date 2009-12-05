#include <stdio.h>
#include <stdlib.h>

#include "access.h"

char* progname;


int main(int argc, char **argv) {
  progname = basename(argv[0]);
  getquota();
  return 0;
}


