#ifndef JSON_H
#define JSON_H

char *get_json_value (char *src_json, char *key);
char *get_json_array_item (char *array_json, char *key, char *needle);

#endif /* JSON_H */
