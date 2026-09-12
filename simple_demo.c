#include <stdio.h>
#include <stdlib.h>
#include "json.h"

int main (void) {
  char line[]="{\"higher\":{\"lower\":654321}}";
  char key[]="higher.lower";
  char *out;

  out = get_json_value(line, key);
  if (out) printf("%s\n", out);
  free(out);
  return 0;
}
