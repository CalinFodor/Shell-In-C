#include "../includes/glob.h"


bool is_glob(char* token){
    return token[0] == '*' || token[0] == '?' || token[0] == '[';
}

bool set_match(char letter,char** pattern_ptr){
    char* pattern = *pattern_ptr;
    if(pattern[0] != '[')
        return false;
    
    bool exclude = false;

    //pattern is [<chars>]

    if(pattern[1] == '!'){
       //exclude the following char
       exclude = true;
    }

    int brac_poz = strchr(pattern,']') - pattern;
    *pattern_ptr = substring(pattern,brac_poz);

    for(int i=0;i<brac_poz;i++){
        if(letter == pattern[i]){
            return !exclude;
        }
    }

    return exclude;
    
}

bool dot_check(char* pattern,char* name){
    if(is_empty(name) || is_empty(pattern))
        return true;
    
    if(pattern[0] == '*' && name[0] == '.')
        return false;
    return true;
}

bool match(char* pattern,char* name){

    if(is_empty(pattern))
        return is_empty(name);
    
    if(pattern[0] == '*'){
        return match(substring(pattern,1),name) 
            || (!is_empty(name) && match(pattern,substring(name,1)));
    }
    
    if(is_empty(name))
        return false;
    
    if(pattern[0] == '?' || pattern[0] == name[0] || set_match(name[0],&pattern)){
        return match(substring(pattern,1),substring(name,1));
    }
    return false;
}

StringList match_files(StringList FILE_SYSTEM,char* pattern){
    StringList results = init_strings();

    for(int i=0;i<FILE_SYSTEM.idx;i++){
        char* name = FILE_SYSTEM.elements[i];
        if(dot_check(pattern,name) && match(pattern,name)){
            add_string_to_list(&results,name);
        }
    }

    if(results.idx == 0){
        add_string_to_list(&results,pattern);
    }

    sort(results.elements,results.idx);
    return results;        
}


StringList expand_globs(StringList tokens){

    char cwd[1024];
    getcwd(cwd,sizeof cwd);

    StringList FILE_SYSTEM = get_files_in_dir(cwd);

    StringList expanded_tokens; //= init_strings();
    expanded_tokens.idx = 0;
    int n = tokens.idx;

   for(int i=0;i<n;i++){
        char* token = tokens.elements[i];
        if(is_glob(token)){
            StringList matched_files = match_files(FILE_SYSTEM,token);

            for(int j=0;j<matched_files.idx;j++){
                char* file = matched_files.elements[j];
                add_string_to_list(&expanded_tokens,file);   
            }
        }else{
            add_string_to_list(&expanded_tokens,token);
        }
    }

    empty_strings(&FILE_SYSTEM);

    return expanded_tokens;

}
