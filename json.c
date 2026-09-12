#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Helper: Find matching closing bracket for opening bracket at position 'start'
 * bracket_open: '{' or '[',  bracket_close: '}' or ']'
 * Returns pointer to matching closing bracket, or NULL if not found */
static char *find_matching_bracket(char *start, char bracket_open, char bracket_close)
{
  int depth = 0;
  char *p = start;

  while (*p) {
    if (*p == bracket_open) depth++;
    else if (*p == bracket_close) {
      depth--;
      if (depth == 0) return p;
    }
    p++;
  }
  return NULL;
}

/* get the value of JSON key, be it single variable, or object, or array */
char *get_json_value (char *src_json, char *inkey)
{
  char key[64];
  char *begin, *end, *bracket, *sbracket, *result;
  char *str, *token, *ktoken, *value;
  char *sptr, *kptr;
  size_t len, rlen, token_len;

  if(!inkey) return NULL;

  /* Recursion requested, split the key by period */
  if (strstr(inkey, ".")) {
    rlen = strlen(inkey);
    if (rlen > 63) rlen = 63;
    memcpy(key, inkey, rlen);
    memset(&key[rlen], 0, 1);

    ktoken = strtok_r(key, ".", &kptr);
    str = src_json;
    while (ktoken != NULL) {
        str = get_json_value(str, ktoken);
        if (!str) return NULL;  // Stop if recursive call fails
        ktoken = strtok_r(NULL, ".", &kptr);
    }
    return str;
  }

  /* Wrap the key in double quote */
  len = strlen(inkey);
  if (len > 62) len = 62;
  memset(&key[0],34,1);
  memcpy(&key[1], inkey, len);
  memset(&key[len+1], 34, 1);
  memset(&key[len+2], 0, 1);

  begin = strstr(src_json, key);
  if (begin) {
    begin = strstr(begin,":");
    if (!begin) return NULL;  // Colon not found, invalid JSON

    end = strstr(begin,",");
    if(!end) end = strstr(begin,"}");
    if (!end) return NULL;  // No delimiter found

    bracket = strstr(begin,"{"); 
    sbracket = strstr(begin,"["); 
    /* Square bracket is here, get it */
    if (sbracket && (sbracket < end)) {
      begin = strstr(begin,"[");
      if (!begin) return NULL;
      end = find_matching_bracket(begin, '[', ']');
      if (!end) return NULL;
      result = malloc(end-begin+2);
      memcpy(result, begin, end-begin+1);
      memset(&result[end-begin+1], 0, 1);
      return result;
    }
    /* Curly bracket is here, get it */
    else if (bracket && (bracket < end)) {
      begin = strstr(begin,"{");
      if (!begin) return NULL;
      end = find_matching_bracket(begin, '{', '}');
      if (!end) return NULL;
      result = malloc(end-begin+2);
      memcpy(result, begin, end-begin+1);
      memset(&result[end-begin+1], 0, 1);
      return result;
    }
    /* We don't see curly bracket or square bracket, or they are after comma
     * so we just get what's inside double quotes */
    else {
      result = malloc(end-begin+1);
      memcpy(result, begin, end-begin+1);
      token = strtok_r(result, "{:,\"} \n", &sptr);
      if (token) {
        token_len = strlen(token) + 1;
        value = malloc(token_len);
        memcpy(value, token, token_len);
        free(result);
        return value;
      }
      free(result);
      return NULL;
    }
  }
  /* No requested key found */
  return NULL;
}
