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
  char *str, *ktoken, *value;
  char *kptr;
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
     * so we just get what's inside double quotes, or the raw scalar text
     * (number, true, false, null) if the value isn't quoted */
    else {
      begin++; /* step past ':' */
      while (*begin == ' ' || *begin == '\t' || *begin == '\n' || *begin == '\r') begin++;

      if (*begin == '"') {
        /* Quoted string: take everything up to the matching closing quote,
         * so spaces inside the value (e.g. multi-word text) are preserved */
        begin++; /* step past opening quote */
        end = strchr(begin, '"');
        if (!end) return NULL;  // Unterminated string, invalid JSON
      } else {
        /* Unquoted scalar (number, true, false, null): stops at the next
         * structural delimiter or whitespace */
        end = begin;
        while (*end && *end != ',' && *end != '}' && *end != ']' &&
               *end != ' ' && *end != '\t' && *end != '\n' && *end != '\r') end++;
      }

      token_len = (size_t)(end - begin);
      value = malloc(token_len + 1);
      memcpy(value, begin, token_len);
      value[token_len] = '\0';
      return value;
    }
  }
  /* No requested key found */
  return NULL;
}

/* Search a JSON array for the first element whose given key's value
 * exactly matches 'needle'. 'array_json' must be the text of
 * a JSON array (e.g. the result of get_json_value() for an array key).
 * 'key' may be a dotted path, same as get_json_value().
 * Returns a newly-allocated copy of the whole matching element
 * (object/array/scalar text), or NULL if nothing matched or on
 * malformed input. Caller must free() the result. */
char *get_json_array_item (char *array_json, char *key, char *needle)
{
  char *p, *elem_start, *elem_end, *element, *value;
  size_t elem_len;

  if (!array_json || !key || !needle) return NULL;

  p = strchr(array_json, '[');
  if (!p) return NULL;
  p++; /* step past '[' */

  while (*p) {
    /* skip separators between elements */
    while (*p == ',' || *p == ' ' || *p == '\n' || *p == '\t' || *p == '\r') p++;
    if (*p == ']' || *p == '\0') break; /* end of array, no match */

    elem_start = p;
    if (*p == '{') {
      elem_end = find_matching_bracket(p, '{', '}');
    } else if (*p == '[') {
      elem_end = find_matching_bracket(p, '[', ']');
    } else if (*p == '"') {
      elem_end = strchr(p + 1, '"');
    } else {
      elem_end = p;
      while (*elem_end && *elem_end != ',' && *elem_end != ']') elem_end++;
      elem_end--; /* step back onto the last real character */
    }
    if (!elem_end) break; /* malformed JSON */

    elem_len = elem_end - elem_start + 1;
    element = malloc(elem_len + 1);
    memcpy(element, elem_start, elem_len);
    element[elem_len] = '\0';

    value = get_json_value(element, key);
    if (value) {
      if (strcmp(value, needle) == 0) {
        free(value);
        return element; /* match found, caller frees */
      }
      free(value);
    }
    free(element);

    p = elem_end + 1; /* resume after this element */
  }
  return NULL;
}
