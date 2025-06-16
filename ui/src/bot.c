#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tf.h"
// 外部函數，由 main.c 提供 prompt
extern char *gemini_decide(const char *prompt);
extern char *generate_prompt(game_t *g);

// ai_bot()：接收 main.c 傳入的 prompt，回傳 Gemini 的決策（字串形式）
char* ai_bot(game_t *g) {
    char *prompt = generate_prompt(g);
    if (!prompt) {
        return strdup("q");
    }
    
    char *res = gemini_decide(prompt);  // 呼叫 Gemini 決策
    free(prompt);  // 释放prompt内存
    
    // 處理結果（去除首尾空白）
    if (!res || strlen(res) == 0) {
        return strdup("q");
    }
    
    // 清理并检查是否仅包含"q"
    char *cleaned = strdup(res);
    free(res);  // 释放原始结果
    
    // 去掉前后空白
    char *start = cleaned;
    while (*start && (*start == ' ' || *start == '\n' || *start == '\r' || *start == '\t')) 
        start++;
    
    // 如果只剩下"q"字符(可能后面还有空白)，则返回纯净的"q"
    if (start[0] == 'q' && (start[1] == '\0' || start[1] == ' ' || start[1] == '\n' || start[1] == '\r' || start[1] == '\t')) {
        free(cleaned);
        return strdup("q");
    }
    
    return cleaned;
}
