#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void add_char(char *s, char c)
{
    int l = strlen(s);
    s[l] = c;
    s[l + 1] = '\0';
}

void add_string(char *strings[], char *string, int poz)
{
    int s_len = strlen(string) + 1;
    strings[poz] = (char *)malloc(s_len);
    strncpy(strings[poz], string, s_len - 1);
    strings[poz][s_len - 1] = '\0';
}

void copy_string(char *dest, char *src)
{
    int s_len = strlen(src) + 1;
    dest = (char *)malloc(s_len);
    strncpy(dest, src, s_len - 1);
    dest[s_len - 1] = '\0';
}

int tokenize_redirects(char line[], char *tokens[])
{
    char token[128] = {'\0'};
    int token_index = 0;

    int line_length = strlen(line);
    int in_token = 0;

    for (int i = 0; i < line_length; i++)
    {
        char character = line[i];

        if (isdigit(character) && !in_token && i != line_length - 1)
        {
            char next_character = line[i + 1];
            if (next_character == '>' || next_character == '<')
            {
                add_char(token,character);
                add_char(token,next_character);
                int i_moves = 1;

                if (i <= line_length - 3)
                {
                    char next_next_character = line[i + 2];
                    if (next_character == next_next_character)
                    {
                       add_char(token,next_character);
                       i_moves++;
                    }
                }

                add_string(tokens, token, token_index++);
                i += i_moves;
                token[0] = '\0';
                continue;
            }
        }

        if (character == '>' || character == '<')
        {
            if (in_token)
            {
                add_string(tokens, token, token_index);
                token_index++;
                token[0] = '\0';
                in_token = 0;
            }

            add_char(token,character);

            if (i != line_length - 1)
            {
                char next_character = line[i + 1];
                if (character == next_character)
                {
                    add_char(token,character);
                    i++;
                }
            }

            add_string(tokens, token, token_index++);
            token[0] = '\0';
        }
        else if (character != ' ')
        {
            add_char(token, character);
            in_token = 1;
        }
        else if (in_token)
        {
            add_string(tokens, token, token_index);
            token_index++;
            token[0] = '\0';
            in_token = 0;
        }

        if (i == line_length - 1 && in_token)
        {
            add_string(tokens, token, token_index++);
        }
    }
    return token_index;
}


typedef struct _Redirect
{
    char *stream;
    char *operation;
    char *target;
} Redirect;

Redirect new_redirect(char *stream, char *operation, char *target)
{
    Redirect redir;

    copy_string(redir.stream, stream);
    copy_string(redir.operation, operation);
    copy_string(redir.target, target);

    return redir;
}

int is_redirect_operator(char* token){
    
    

}

void parse_redirects(char *tokens[], int n)
{
    char *words[128];
    int words_index;

    Redirect redirects[128];
    int redirects_index;

    for (int i = 0; i < n; i++)
    {

    }
}


void print_tokens(char *tokens[], int n)
{
    for (int i = 0; i < n; i++)
        printf("[%s] ",tokens[i]);
    printf("\n");
}

void free_tokens(char *tokens[], int n)
{
    for (int i = 0; i < n; i++)
        free(tokens[i]);
}

void process_redirects(char line[])
{
    char *tokens[100];
    int n = tokenize_redirects(line, tokens);
    print_tokens(tokens, n);
    free_tokens(tokens, n);
}

int main(void)
{
    char line[1024];
    while (fgets(line, sizeof line, stdin))
    {
        if (line[0] == '\n' || line[0] == 0)
            continue;
        line[strlen(line) - 1] = '\0';
        process_redirects(line);
    }
    return 0;
}