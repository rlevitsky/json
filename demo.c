#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"

typedef struct {
  void* bp;
  unsigned int  size;
} mybuff;

void mybuffappend(mybuff* buf, void* vp, unsigned int size)
{
  void *new_ptr;

  if (!buf->size) buf->size = 0;
  new_ptr = realloc(buf->bp, buf->size + size);
  if (!new_ptr) {
    fprintf(stderr, "realloc failed\n");
    return;
  }
  buf->bp = new_ptr;
  memcpy( buf->bp + buf->size, vp, size );
  buf->size += size;
}

int main (int argc, char **argv) {
  char *line = NULL;
  char *result = NULL;
  mybuff buffer = {0,0};
  size_t len = 0;
  int c;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <json_key>\n", argv[0]);
    return 1;
  }

  while ((c = getline(&line, &len, stdin)) >= 0)
  {
    mybuffappend(&buffer, line, c);
  }

  result = get_json_value((char *)buffer.bp, argv[1]);
  if (result) printf("%s\n", result);

  free(buffer.bp);
  free(result);
  free(line);
  return 0;
}
