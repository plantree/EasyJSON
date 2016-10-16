#include <string>
#include <cassert>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <utility>
#include <vector>
#include <cstddef>
#include "jsonnumber.h"
namespace JSON{




    struct JSON_context{
        char *json;
    };







    void parse_whitespace(JSON_context &c);
    void _except_obj_or_arr(JSON_context &,int &);




    bool ISHEX(char c){
        if((c >= '0' && c<= '9')||(c == 'a'||c == 'A')||(c == 'b'||c == 'B')||(c == 'c'||c == 'C')||(c == 'd'||c == 'D')
                ||(c == 'e'||c == 'E') || (c == 'f'||c == 'F')){
            return true;
        }

        return false;
    }


    void parse_string(JSON_context &c){
        //不需要消去空格
        char *p = c.json;
        while(*p){

            if(*p == '\"'){
                ++p;
                break;
            }
            if(*p == '\\'){
                if(*(p+1) == 'n'||*(p+1) == 't' ||*(p+1) == 'r'
                 ||*(p+1) == 'b'||*(p+1) == 'f'||*(p+1) == '\\'
                 ||*(p+1) == '/'||*(p+1) == '"'){
                    ++p;
                }else if(*(p+1) == 'u'&&ISHEX(*(p+2))&&ISHEX(*(p+3))&&ISHEX(*(p+4))&&ISHEX(*(p+5))){
                    p+=5;
                }else{
                    throw;
                }
            }
            ++p;
        }
        c.json = p;
    }
    bool ISDIGIT(char c){
        if(c>= '0'&& c<='9'){
            return true;
        }
        return false;
    }


    bool ISSPACE_OR_ZERO_OR_COMMA(char &c){
        char *p = &c;
        while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p=='\0'||*p==',' ||*p=='}'||*p==']')
            return true;
        return false;

    }
     void parse_number(JSON_context &c){
        std::size_t i;
        for(i = 0;1;++i){
            if(ISSPACE_OR_ZERO_OR_COMMA(c.json[i])){
                break;
            }
        }
        if(is_JSONNumber(std::string(c.json,i))){
            c.json +=i;
        }else{
            throw;
        }
     }

    void parse_bool(JSON_context &c){
        if(c.json[0] == 't'||c.json[1] == 'r'||c.json[2] == 'u'||c.json[3] == 'e'){
            c.json += 4;
        }else if(c.json[0] == 'f'||c.json[1] == 'a'||c.json[2] == 'l'||c.json[3] == 's'||c.json[4] == 'e'){
            c.json += 5;
        }else {
            throw;
        }
    }
    void parse_null(JSON_context &c){
        if(c.json[0] == 'n'||c.json[1] == 'u'||c.json[2] == 'l'||c.json[3] == 'l'){
            c.json += 4;
        }else {
            throw;
        }
    }

    void parse_keystring(JSON_context &c,int&depth){
        //不需要消去空格
        parse_string(c);


        parse_whitespace(c);
        if(c.json[0] == ':'){
            ++(c.json);
        }else{
            throw;
        }

        parse_whitespace(c);

        if(c.json[0] == '{'||c.json[0] == '['){
            //不需要++depth
            _except_obj_or_arr(c,depth);
        }else if(c.json[0] == '-' ||ISDIGIT(c.json[0])){
            parse_number(c);
        }else if(c.json[0] == '"'){
            ++(c.json);
            parse_string(c);
        }else if(c.json[0] == 'f'||c.json[0] == 't'){
            parse_bool(c);
        }else if(c.json[0] == 'n'){
            parse_null(c);
        }else{
            throw;
        }

        parse_whitespace(c);

        if(c.json[0] == ','){
            ++(c.json);
            parse_whitespace(c);
            if(c.json[0] == '"'){
                ++(c.json);
                parse_keystring(c,depth);
            }else{
                throw;
            }
        }else if(c.json[0] == '}'){
            --depth;
            ++c.json;
        }else{
            throw;
        }


    }
    void parse_object(JSON_context &c,int &depth){
        parse_whitespace(c);
        if(c.json[0] == '"'){
            ++c.json;
            parse_keystring(c,depth);
        }else if(c.json[0] == '}'){
            ++c.json;
            --depth;
        }else{
            throw;
        }
    }


    std::string ITOA(int i){
        char string[16] = {0};
        sprintf(string, "%d", i);
        return std::string(string);
    }

    void parse_value(JSON_context &c,int pos,int&depth){
        parse_whitespace(c);

        if(c.json[0] == '{'||c.json[0] == '['){
            //不需要++depth
            _except_obj_or_arr(c,depth);
        }else if(c.json[0] == '-' ||ISDIGIT(c.json[0])){
            parse_number(c);
        }else if(c.json[0] == '"'){
            ++(c.json);
            parse_string(c);
        }else if(c.json[0] == 'f'||c.json[0] == 't'){
            parse_bool(c);
        }else if(c.json[0] == 'n'){
            parse_null(c);
        }else{
            throw;
        }

        parse_whitespace(c);
        if(c.json[0] == ','){
            ++(c.json);
            ++pos;
            parse_value(c,pos,depth);
        }else if(c.json[0] == ']'){
            ++(c.json);


            --depth;
        }else{
            throw;
        }
    }

    void parse_array(JSON_context &c,int &depth){
        parse_whitespace(c);
        if(c.json[0] == ']'){
            ++c.json;




            --depth;
            //直接return
        }else{
            parse_value(c,0,depth);
        }



    }


    void _except_obj_or_arr(JSON_context &c,int &depth){
        if(c.json[0] == '{'){
            ++depth;
            ++(c.json);
            parse_object(c,depth);
        }else if(c.json[0] == '['){
            ++depth;
            ++(c.json);
            parse_array(c,depth);
        }else{
            throw;
        }
    }


    void parse_whitespace(JSON_context &c) {
        char *p = c.json;
        while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
            p++;
        c.json = p;
    }



    void init_context(JSON_context &c,const  char *json){
        c.json = const_cast<char*>  (json);
    }
    void _parse_valid(const char *json){
        JSON_context c;
        int depth = 0;
        init_context(c,json);
        parse_whitespace(c);

        _except_obj_or_arr(c ,depth);


        parse_whitespace(c);
        if(c.json[0] != '\0'){
            throw;
        }

    }
}



