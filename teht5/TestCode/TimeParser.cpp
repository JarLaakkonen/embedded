#include <stdlib.h>
#include <string.h>
#include "TimeParser.h"
#include <ctype.h>
// otettu pois turhat kommentit, jotta koodi ei vie niin paljoa tilaa näytöllä :D
// time format: HHMMSS (6 characters)
int time_parse(char *time) {
	// jos ei muuta niin antaa time len error eli sitten koodi ei toimi
	int seconds = TIME_LEN_ERROR;
	// onko arvo 0
	 if (time == NULL){
		return 	TIME_VALUE_NULL_ERROR;
	}
	// liian iso tai pieni
	if (strlen(time) != 6) {  // tarkistetaan pituus täsmälleen 6
    return TIME_VALUE_SIZE_ERROR;
	}
	// kirjaimia seassa
	for (int i = 0; i < 6;i++){
		if (time[i] == '-') {
			return TIME_VALUE_NEGATIVE_ERROR;
		}
		if (!isdigit((unsigned char)time[i])){
			return TIME_VALUE_CHAR_ERROR;
		}
	}
    int values[3];
	values[2] = atoi(time+4); // seconds
	time[4] = 0;
	values[1] = atoi(time+2); // minutes
	time[2] = 0;
	values[0] = atoi(time); // hours
	// liian isoja arvoja
	if (values[0] > 23 || values[1] > 59 || values[2] > 59) {
		seconds = TIME_VALUE_OVERFLOW_ERROR;
	}
	else {
		seconds = values[0]*3600 + values[1]*60 + values[2] ;
	}
	if (seconds == 0){
		return TIME_VALUE_SECOND_ZERO_ERROR;
	}
	return seconds;
}
int light_parse(const char *light) {
    int value = LIGHT_LEN_ERROR;

    // Tarkista että merkkijono ei ole NULL tai tyhjä
    if (light == NULL || strlen(light) == 0) {
        return LIGHT_NULL_ERROR;
    }
    int length = strlen(light); // Käydään läpi jokainen merkki
    for (int i = 0; i < length; i++) {
        char c = light[i];
        if (isdigit((unsigned char)c)) {
            return LIGHT_NUMERIC_ERROR;  // Numero ei sallittu
        }
        else if (islower((unsigned char)c)) {
            return LIGHT_LOWER_CASE_ERROR;  // Pienet kirjaimet ei sallittu
        }
        else if (!isalpha((unsigned char)c)) {
            return LIGHT_NOT_CHAR_ERROR; 
        }
		 else if (c != 'R' && c != 'Y' && c != 'G') {
            return LIGHT_INVALID_VALUE_ERROR; // Sallitaan vain arvot R Y G
        }
    }
    value = LIGHT_VALUES_OK;    // Kaikki ok
    return value;
}