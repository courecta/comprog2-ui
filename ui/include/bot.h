#ifndef GEMINI_BOT_H
#define GEMINI_BOT_H

#include "tf.h"  // 使用 game_t 結構所需

// ai_bot 主介面：傳入遊戲狀態，回傳 Gemini 給的行動（例如 "0", "b2d", "q"）
char* ai_bot(game_t *g);

// 建立目前回合的遊戲 prompt 描述
char* generate_prompt(game_t *g);

// 呼叫 Gemini API 並傳入 prompt，回傳 Gemini 的回答
char* gemini_decide(const char *prompt);

#endif  // GEMINI_BOT_H
