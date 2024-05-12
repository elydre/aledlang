#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "aledlang.h"

char **append_to_tokens(char **tokens, int *len, char *tok, int size) {
    if (*len % 1024 == 0) {
        tokens = realloc(tokens, (*len + 1024) * sizeof(char *));
        memset(tokens + *len, 0, 1024 * sizeof(char *));
    }
    char *tmp = malloc(size + 1);
    memcpy(tmp, tok, size);
    tmp[size] = '\0';
    tokens[(*len)++] = tmp;
    return tokens;
}

void free_token(char **tokens) {
    if (!tokens)
        return;
    for (int i = 0; tokens[i]; i++)
        free(tokens[i]);
    free(tokens);
}

void close_next(char *src, int i) {
    for (; src[i]; i++) {
        if (isspace(src[i]))
            src[i] = '\0';
    }
}

char **aled_lexe(char *src, int *len) {
    char **tokens = NULL;
    int size = 0;
    char tmp[4];

    int token_start = 0;
    for (int i = 0; src[i]; i++) {
        if (isspace(src[i])) {
            if (i > token_start) {
                tokens = append_to_tokens(tokens, &size, src + token_start, i - token_start);
            }
            while (isspace(src[i]))
                i++;
            i--;
            token_start = i + 1;
        } else if (src[i] == '/' && src[i + 1] == '/') { // comment
            if (i > token_start)
                tokens = append_to_tokens(tokens, &size, src + token_start, i - token_start);
            while (src[i] && src[i] != '\n')
                i++;
            if (!src[i])
                break;
            token_start = i + 1;
        } else if (src[i] == '[') { // close comment
            if (i > token_start)
                tokens = append_to_tokens(tokens, &size, src + token_start, i - token_start);
            while (src[i] && src[i] != ']')
                i++;
            if (!src[i]) {
                free_token(tokens);
                raise_andexit("Unterminated comment");
            }
            token_start = i + 1;
        } else if (src[i] == '\'') { // char
            if (i > token_start)
                tokens = append_to_tokens(tokens, &size, src + token_start, i - token_start);
            token_start = i;
            if (src[i + 1] == '\\' && src[i + 2] && src[i + 3] == '\'') {
                if (src[i + 2] == 'n')
                    src[i + 1] = '\n';
                else if (src[i + 2] == 't')
                    src[i + 1] = '\t';
                else if (src[i + 2] == '0')
                    src[i + 1] = '\0';
                else if (src[i + 2] == '\\')
                    src[i + 1] = '\\';
                else if (src[i + 2] == '\'')
                    src[i + 1] = '\'';
                else {
                    free_token(tokens);
                    raise_andexit("Invalid escape sequence: %c", src[i + 2]);
                }
                src[i + 2] = '\'';
                tokens = append_to_tokens(tokens, &size, src + token_start, 3);
                i += 2;
                token_start = i + 1;
            } else if (src[i + 1] && src[i + 2] == '\'') {
                tokens = append_to_tokens(tokens, &size, src + token_start, 3);
                i += 2;
                token_start = i + 1;
            } else {
                close_next(src, i);
                free_token(tokens);
                raise_andexit("Invalid char: %s", src + token_start);
            }
        } else if (src[i] == '"') { // string
            if (i > token_start)
                tokens = append_to_tokens(tokens, &size, src + token_start, i - token_start);
            token_start = i;
            i++;
            while (src[i] && src[i] != '"') {
                if (src[i] == '\\') {
                    if (!src[i + 1]) {
                        free_token(tokens);
                        raise_andexit("Unterminated string");
                    }
                    if (src[i + 1] == 'n')
                        src[i] = '\n';
                    else if (src[i + 1] == 't')
                        src[i] = '\t';
                    else if (src[i + 1] == '0')
                        src[i] = '\0';
                    else if (src[i + 1] == '\\')
                        src[i] = '\\';
                    else if (src[i + 1] == '"')
                        src[i] = '"';
                    else {
                        free_token(tokens);
                        raise_andexit("Invalid escape sequence: %c", src[i + 1]);
                    }
                    for (int j = i + 1; src[j]; j++)
                        src[j] = src[j + 1];
                }
                tmp[0] = '\'';
                tmp[1] = src[i];
                tmp[2] = '\'';
                tmp[3] = '\0';
                tokens = append_to_tokens(tokens, &size, tmp, 3);
                i++;
            }
            if (!src[i]) {
                free_token(tokens);
                raise_andexit("Unterminated string");
            }
            tokens = append_to_tokens(tokens, &size, "0", 1);
            token_start = i + 1;
        } else if (!src[i + 1]) {
            if (i > token_start) {
                tokens = append_to_tokens(tokens, &size, src + token_start, i - token_start + 1);
            }
        }
    }
    *len = size;

    return tokens;
}
