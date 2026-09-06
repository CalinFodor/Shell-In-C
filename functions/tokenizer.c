#include "../includes/tokenizer.h"

bool is_operator(char c){
    return (c=='>') || (c=='<') || (c=='='); 
}

TokenList tokenize(char line[]){

    TokenList token_list = init_strings();

    TokenizeState current_state = DEFAULT;

    char token[128] = {'\0'};

    int line_length = strlen(line);

    int space_encountered = 0;
    int escaped = 0;
    bool in_token = false;

    for(int i=0;i<line_length;i++)
    {
        char character = line[i];
        
        if (character == '\'' && current_state != SINGLE_QUOTE){
            current_state = SINGLE_QUOTE;
            continue;
        }

        if (character == '"' && current_state == DEFAULT){
            current_state = DOUBLE_QUOTE;
            continue;
        }

        switch (current_state){
            case DEFAULT:
                if(character == '\\'){
                   char next_char = line[i+1];
                   add_char(token,next_char);
                   i++;
                   continue; 
                }

                if(is_operator(character)){
                    if (in_token){
                        add_string_to_list(&token_list,token);
                        in_token=false;
                    }

                    token[0] = character;
                    int offset = 1;
                    if(i != line_length - 1){
                        char next_character = line[i+1];
                        if(next_character == character){
                            token[1] = next_character;
                            offset++;
                            i++;
                        }
                    }

                    token[offset] = '\0';
                    add_string_to_list(&token_list,token);

                    token[0] = '\0';
                    continue;
                }

                if(character != ' '){
                    add_char(token,character);
                    in_token = true;
                }    

                if((character == ' ' || i == line_length - 1) && in_token){
                    add_string_to_list(&token_list,token);
                    in_token = false;
                    //reset token
                    token[0] = '\0';
                }

                break;

            case SINGLE_QUOTE:
                if(character == '\''){
                    add_string_to_list(&token_list,token);
                    //reset token
                    token[0] = '\0';
                    current_state = DEFAULT;
                }else if(i == line_length - 1){
                    printf("ERROR: Not found matching '\n");
                    return token_list;
                }
                else {
                    add_char(token,character);
                }    

                break;
            case DOUBLE_QUOTE:
                if(character != ' ' && space_encountered){
                    space_encountered = 0;
                }

                if(character == '"'){
                    add_string_to_list(&token_list,token);

                    //reset token
                    token[0] = '\0';
                    current_state = DEFAULT;
                }else if(i == line_length - 1){
                    printf("ERROR: Not found matching \"\n");
                    return token_list;
                }else if(character == '\\'){
                    char next_char = line[i+1];
                    add_char(token,next_char);
                    i++;
                }
                else if(character != ' '){
                    add_char(token,character);
                }
                else if(character == ' ' && !space_encountered){
                    add_char(token,character);
                    space_encountered = 1;
                }
                break;
        }
    }
    return token_list;
}
