#include <string>
#include <cassert>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <utility>
#include <vector>
#include <cstddef>
#include <cstdlib>
#include "jsonnumber.h"
namespace EasyJSON{

    enum JSONTYPE{
        JSON_NULL = 0, JSON_FALSE, JSON_TRUE, JSON_NUMBER, JSON_STRING, JSON_ARRAY, JSON_OBJECT
    };

    struct JSON_value{
        std::string s;
        double n;
        JSONTYPE type;
    };

    struct JSON_context{
        char *json;
    };

    struct JSON_status{
        int depth = 0;
        bool is_object = true;
    };

    struct JSON_node{
        std::string node;
    };

    std::vector<std::pair<std::string,JSON_status>> no;
    std::vector<std::pair<std::pair<std::string,JSON_status>,JSON_value>> vec_x;
    class Value{
    public:
        Value():type(JSON_NULL),vec_x(){}


        Value(const Value& _v):type(_v.type),vec_x(_v.vec_x){}

        Value&operator =(const Value& _rh){
            type = _rh.type;
            vec_x = _rh.vec_x;
            return *this;
        }


        Value &operator[](const char *s){
            s = s;
            return *this;
        }
        Value &operator[](std::size_t pos){
            pos = pos;

            return *this;
        }
    private:

        void set_vec(const std::vector<std::pair<std::pair<std::string,JSON_status>,JSON_value>> &v){
            vec_x = v;
        }

        JSONTYPE type;
        std::vector<std::pair<std::pair<std::string,JSON_status>,JSON_value>> vec_x;

        friend Value parse(const char *json);
    };


    void parse_whitespace(JSON_context &c);
    void _except_obj_or_arr(JSON_context &c);


    int depth = 0;

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

    void parse_keystring(JSON_context &c){
        //不需要消去空格
        char *temp1 = c.json;
        parse_string(c);
        char *temp2 = c.json;
        ptrdiff_t num = (temp2 - temp1) - 1;
        JSON_status status;
        status.depth = depth;
        status.is_object = true;
        no.push_back(std::make_pair( std::string(temp1,num),status));
        parse_whitespace(c);
        if(c.json[0] == ':'){
            ++(c.json);
        }else{
            throw;
        }

        parse_whitespace(c);

        if(c.json[0] == '{'||c.json[0] == '['){
            //不需要++depth
            //不需要vec_p.push_back

            _except_obj_or_arr(c);
        }else if(c.json[0] == '-' ||ISDIGIT(c.json[0])){
            char *temp1 = c.json;
            parse_number(c);
            char *temp2 = c.json - 1;
            ptrdiff_t num = (temp2 - temp1);
            JSON_value jv;
            jv.type = JSON_NUMBER;
            jv.n = strtod(std::string(temp1,num).c_str(),NULL);

            vec_x.push_back(std::make_pair(no[no.size() - 1],jv));


        }else if(c.json[0] == '"'){
            ++(c.json);
            char *temp1 = c.json;
            parse_string(c);
            char *temp2 =(c.json - 1);
            ptrdiff_t num = (temp2 - temp1);
            JSON_value jv;
            jv.type = JSON_STRING;
            jv.s = std::string(temp1,num);


            vec_x.push_back(std::make_pair(no[no.size() - 1],jv));

        }else if(c.json[0] == 'f'||c.json[0] == 't'){
            bool b;
            if(c.json[0] == 't'){
                b = true;
            }else b = false;

            parse_bool(c);
            JSON_value jv;

            if(b){
                jv.type = JSON_TRUE;
            }else{
                jv.type = JSON_FALSE;
            }


            vec_x.push_back(std::make_pair(no[no.size() - 1],jv));


        }else if(c.json[0] == 'n'){
            parse_null(c);
            JSON_value jv;
            jv.type = JSON_NULL;
            vec_x.push_back(std::make_pair(no[no.size() - 1],jv));

        }else{
            throw;
        }

        parse_whitespace(c);

        if(c.json[0] == ','){
            ++(c.json);
            parse_whitespace(c);
            if(c.json[0] == '"'){
                ++(c.json);
                parse_keystring(c);
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
    void parse_object(JSON_context &c){
        parse_whitespace(c);
        if(c.json[0] == '"'){
            ++c.json;
            parse_keystring(c);
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

    void parse_value(JSON_context &c,int pos){
        parse_whitespace(c);
        JSON_status status;
        status.depth = depth;
        status.is_object = false;
        no.push_back(std::make_pair(std::string(ITOA(pos)),status));
        if(c.json[0] == '{'||c.json[0] == '['){
            //不需要++depth
            _except_obj_or_arr(c);
        }else if(c.json[0] == '-' ||ISDIGIT(c.json[0])){
            char *temp1 = c.json;
            parse_number(c);
            char *temp2 = c.json - 1;
            ptrdiff_t num = (temp2 - temp1);
            JSON_value jv;
            jv.type = JSON_NUMBER;
            jv.n = strtod(std::string(temp1,num).c_str(),NULL);

            vec_x.push_back(std::make_pair(no[no.size() - 1],jv));
        }else if(c.json[0] == '"'){
            ++(c.json);
            char *temp1 = c.json;
            parse_string(c);
            char *temp2 =(c.json - 1);
            ptrdiff_t num = (temp2 - temp1);
            JSON_value jv;
            jv.type = JSON_STRING;
            jv.s = std::string(temp1,num);
            vec_x.push_back(std::make_pair(no[no.size() - 1],jv));
        }else if(c.json[0] == 'f'||c.json[0] == 't'){
            bool b;
            if(c.json[0] == 't'){
                b = true;
            }else b = false;

            parse_bool(c);
            JSON_value jv;

            if(b){
                jv.type = JSON_TRUE;
            }else{
                jv.type = JSON_FALSE;
            }

            vec_x.push_back(std::make_pair(no[no.size() - 1],jv));

        }else if(c.json[0] == 'n'){
            parse_null(c);
            JSON_value jv;
            jv.type = JSON_NULL;
            vec_x.push_back(std::make_pair(no[no.size() - 1],jv));
        }else{
            throw;
        }

        parse_whitespace(c);
        if(c.json[0] == ','){
            ++(c.json);
            ++pos;
            parse_value(c,pos);
        }else if(c.json[0] == ']'){
            ++(c.json);


            --depth;
        }else{
            throw;
        }
    }

    void parse_array(JSON_context &c){
        parse_whitespace(c);
        if(c.json[0] == ']'){
            ++c.json;


            JSON_status status;
            status.depth = depth;
            status.is_object = false;
            no.push_back(std::make_pair(std::string(ITOA(0)),status));
            JSON_value jv;
            jv.type = JSON_NULL;
            vec_x.push_back(std::make_pair(no[no.size() - 1],jv));

            --depth;
            //直接return
        }else{
            parse_value(c,0);
        }



    }


    void _except_obj_or_arr(JSON_context &c){
        if(c.json[0] == '{'){
            JSON_value jv;jv.type = JSON_OBJECT;
            if(no.size() != 0){
                vec_x.push_back(std::make_pair(no[no.size() - 1],jv));
            }else{
                    JSON_status js;
                    js.depth = 0;
                    js.is_object = true;

                    std::pair<std::string,JSON_status> p = std::make_pair(std::string(""),js);

                    jv.type = JSON_OBJECT;
                    std::pair<std::pair<std::string,JSON_status>,JSON_value> pp =std::make_pair(p,jv);

                    vec_x.push_back(pp);

            }

            ++depth;
            ++(c.json);
            parse_object(c);
        }else if(c.json[0] == '['){
            JSON_value jv;jv.type = JSON_ARRAY;
            if(no.size() != 0){
                vec_x.push_back(std::make_pair(no[no.size() - 1],jv));
            }else{
                JSON_status js;
                js.depth = 0;
                js.is_object = false;

                std::pair<std::string,JSON_status> p = std::make_pair(std::string(""),js);

                jv.type = JSON_ARRAY;
                std::pair<std::pair<std::string,JSON_status>,JSON_value> pp =std::make_pair(p,jv);

                vec_x.push_back(pp);

            }


            ++depth;
            ++(c.json);
            parse_array(c);
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
    Value parse(const char *json){
        Value v;
        JSON_context c;
        init_context(c,json);
        parse_whitespace(c);

        _except_obj_or_arr(c);


        parse_whitespace(c);
        if(c.json[0] != '\0'){
            throw;
        }
        v.set_vec(vec_x);
        return v;
    }
};

std::string strS(EasyJSON::JSONTYPE t){
    if(t == EasyJSON::JSON_NULL){
        return std::string("NULL");
    }
    if(t == EasyJSON::JSON_FALSE){
        return std::string("FALSE");
    }
    if(t == EasyJSON::JSON_TRUE){
        return std::string("TRUE");
    }
    if(t == EasyJSON::JSON_ARRAY){
        return std::string("ARRAY");
    }
    if(t == EasyJSON::JSON_OBJECT){
        return std::string("OBJECT");
    }
    if(t == EasyJSON::JSON_NUMBER){
        return std::string("NUMBER");
    }
    return std::string("STRING");
}

int main(){
    FILE *f = fopen("C:\\Users\\42937\\Desktop\\test\\pass1.json","r");
    char str[1100] = {0};
    fgets(str,1100,f);

    EasyJSON::Value v;
    v = EasyJSON::parse(str);

    for(auto x:EasyJSON::vec_x){
        std::cout << x.first.first <<' '<<x.first.second.depth <<' ' << strS(x.second.type) << '\n'  ;
    }

    return 0;
}
