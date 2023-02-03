#include "../include/typefunctions.h"

formattedTime_t formatTime(double seconds) {
    formattedTime_t time;

    time.hours = seconds / 3600;
    time.minutes = (seconds - (time.hours * 3600)) / 60;
    time.seconds = (double)(seconds - (time.hours * 3600) - (time.minutes * 60));

    return time;
}

void removeTrailingNewLine(char16_t *string) {
    string[wcscspn(string, u"\r\n")] = u'\0';
}