#ifndef HISTORY_H
#define HISTORY_H

char *read_file(const char *filename);
char *trim_history_to_last_three(const char *full_history);
char *trim_history_to_last_n(const char *full_history, int n);
int append_to_history(const char *formatted_game, const char *response);
void append_and_trim_history(const char *filename, const char *formatted_game, const char *response);

#endif // HISTORY_H
