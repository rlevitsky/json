# Simple JSON library #

## About ##

This library consists of the only function `get_json_value` which scans the JSON for the key you provided and returns a pointer to the dynamically-allocated result string.

Please note you cannot search or index inside JSON like with jq, i.e. it is impossible to properly distinct between weather samples in the list from provided `demo.json` example.

## How to use ##

Place the `json` directory into your source tree.
Refer the function definition at your program like
```
#include "json/json.h"
```

Compile your program whith json.c (gcc example)
```
gcc -o demo demo.c json/json.c
```

Please note it is your responsibility to free the memory after result string usage.

See examples for more details.

## Example programs ##

Simplest demo (simple_demo.c), both JSON string and key are defined as string arrays at the program text.

Advanced demo (demo.c), JSON read from stdin and placed to dynamically-allocated string array, key read as the only command-line argument.

## How to build demo programs ##

```
make
```

## How to test ##

### Simplest demo, both JSON string and key are defined as string arrays ###
```
./simple_demo
```

### Advanced demo, JSON read from stdin and placed to dynamically-allocated string array, key read as the only command-line argument ###
```
cat demo.json | ./demo cod
cat demo.json | ./demo city
cat demo.json | ./demo city.country
cat demo.json | ./demo city.coord.lat
cat demo.json | ./demo list
cat demo.json | ./demo list.weather
```
