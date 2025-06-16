#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"history.h"

char *read_file(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return NULL;
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    rewind(fp);

    char *content = malloc(len + 1);
    if (!content) {
        fclose(fp);
        return NULL;
    }
    fread(content, 1, len, fp);
    content[len] = '\0';
    fclose(fp);
    return content;
}

char *trim_history_to_last_three(const char *full_history) {
    int count = 0;
    const char *ptr = full_history;
    while ((ptr = strstr(ptr, "[Bot]")) != NULL) {
        count++;
        ptr += 5;
    }

    if (count <= 3) return strdup(full_history);

    ptr = full_history;
    int skip = count - 3;
    while (skip-- > 0) {
        ptr = strstr(ptr, "[Bot]");
        if (!ptr) break;
        ptr += 5;
    }

    return strdup(ptr ? ptr - 5 : full_history);
}

char *trim_history_to_last_n(const char *full_history, int n) {
    int count = 0;
    const char *ptr = full_history;
    while ((ptr = strstr(ptr, "[Bot]")) != NULL) {
        count++;
        ptr += 5;
    }

    if (count <= n) return strdup(full_history);

    ptr = full_history;
    int skip = count - n;
    while (skip-- > 0) {
        ptr = strstr(ptr, "[Bot]");
        if (!ptr) break;
        ptr += 5;
    }

    return strdup(ptr ? ptr - 5 : full_history);
}

int append_to_history(const char *formatted_game, const char *response) {
    FILE *hf = fopen("history.txt", "a");
    if (!hf) return 0;
    fprintf(hf, "[Bot]\n%s\n[Gemini]\n%s\n\n", formatted_game, response);
    fclose(hf);
    return 1;
}

void append_and_trim_history(const char *filename, const char *formatted_game, const char *response) {
    // 讀取原始歷史紀錄
    char *old_history = read_file(filename);
    if (!old_history) old_history = strdup("");

    // 加入新紀錄
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        free(old_history);
        return;
    }

    // 裁切歷史為最後3段
    char *trimmed = trim_history_to_last_n(old_history, 10);
    fprintf(fp, "%s[Bot]\n%s\n[Gemini]\n%s\n\n", trimmed, formatted_game, response);

    fclose(fp);
    free(old_history);
    free(trimmed);
}
