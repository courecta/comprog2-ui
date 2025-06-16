#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>
#include "history.h"

#define API_KEY "AIzaSyAhE3PvoREBt6Z4iX9h0TVEeIoihDl-mU4"

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) return 0;

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

void extract_answer(const char *json, char *output, size_t maxlen) {
    const char *parts = strstr(json, "\"parts\"");
    if (!parts) {
        strncpy(output, "[No parts found]", maxlen);
        return;
    }

    const char *text_key = "\"text\": \"";
    const char *start = strstr(parts, text_key);
    if (!start) {
        strncpy(output, "[No answer found]", maxlen);
        return;
    }

    start += strlen(text_key);
    const char *end = start;
    while (*end && !(*end == '"' && *(end - 1) != '\\')) end++;

    if (*end != '"') {
        strncpy(output, "[Malformed response]", maxlen);
        return;
    }

    size_t i = 0;
    for (const char *p = start; p < end && i < maxlen - 1; p++) {
        if (*p == '\\') {
            p++;
            if (*p == 'n') output[i++] = '\n';
            else if (*p == 't') output[i++] = '\t';
            else if (*p == '"') output[i++] = '"';
            else output[i++] = *p;
        } else {
            output[i++] = *p;
        }
    }
    output[i] = '\0';
}

char *json_escape(const char *input) {
    size_t len = strlen(input);
    char *escaped = malloc(len * 2 + 1);
    if (!escaped) return NULL;

    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        switch (input[i]) {
            case '"':  escaped[j++] = '\\'; escaped[j++] = '"'; break;
            case '\\': escaped[j++] = '\\'; escaped[j++] = '\\'; break;
            case '\n': escaped[j++] = '\\'; escaped[j++] = 'n'; break;
            case '\r': escaped[j++] = '\\'; escaped[j++] = 'r'; break;
            case '\t': escaped[j++] = '\\'; escaped[j++] = 't'; break;
            default:   escaped[j++] = input[i];
        }
    }
    escaped[j] = '\0';
    return escaped;
}

char* gemini_decide(const char *formatted_game) {
    char *rules = read_file("rules.txt");
    char *requirement = read_file("requirement.txt");
    char *full_history = read_file("history.txt");

    if (!rules || !requirement || !full_history) {
        fprintf(stderr, "[Gemini] Failed to read rules.txt / requirement.txt / history.txt\n");
        free(rules); free(requirement); free(full_history);
        return strdup("0");
    }

    char *history = trim_history_to_last_n(full_history, 10);
    free(full_history);

    size_t total_len = strlen(rules) + strlen(requirement) + strlen(history) + strlen(formatted_game) + 512;
    char *prompt = malloc(total_len);
    snprintf(prompt, total_len,
             "[規則]\n%s\n\n[要求]\n%s\n\n[對話紀錄]\n%s\n\n[目前狀態]\n%s\n",
             rules, requirement, history, formatted_game);
    free(rules); free(requirement); free(history);

    char *escaped = json_escape(prompt);
    free(prompt);

    size_t json_len = strlen(escaped) + 256;
    char *json_data = malloc(json_len);
    snprintf(json_data, json_len,
             "{\"contents\":[{\"parts\":[{\"text\":\"%s\"}]}]}", escaped);
    free(escaped);

    struct MemoryStruct chunk = { .memory = malloc(1), .size = 0 };
    CURL *curl = curl_easy_init();
    if (!curl) {
        free(json_data); free(chunk.memory);
        return strdup("0");
    }

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    char url[512];
    snprintf(url, sizeof(url),
             "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash-lite:generateContent?key=%s",
             API_KEY);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "Request failed: %s\n", curl_easy_strerror(res));
        free(json_data); free(chunk.memory);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return strdup("0");
    }

    char *response = calloc(1, 4096);
    extract_answer(chunk.memory, response, 4096);
    append_and_trim_history("history.txt", formatted_game, response);

    free(json_data); free(chunk.memory);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return response;
}
