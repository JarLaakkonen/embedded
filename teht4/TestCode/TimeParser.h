#ifndef TIMEPARSER_H
#define TIMEPARSER_H

// Error codes for time
#define TIME_LEN_ERROR      -1
#define TIME_VALUE_OVERFLOW_ERROR    -2
#define TIME_VALUE_NEGATIVE_ERROR    -3
#define TIME_VALUE_SIZE_ERROR       -4
#define TIME_VALUE_CHAR_ERROR       -5
#define TIME_VALUE_NULL_ERROR      -6
#define TIME_VALUE_SECOND_ZERO_ERROR      -7

// Error codes for light
#define LIGHT_LEN_ERROR -1
#define LIGHT_NULL_ERROR -2
#define LIGHT_NUMERIC_ERROR -3
#define LIGHT_LOWER_CASE_ERROR -4
#define LIGHT_NOT_CHAR_ERROR -5
#define LIGHT_INVALID_VALUE_ERROR -6
#define LIGHT_VALUES_OK 0

using namespace std;

int time_parse(char *time);
int light_parse(const char *light);

#endif
