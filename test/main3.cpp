#include <string>
#include <utility>
#include <vector>
#include <cstddef>
#include <cstdio>
#include "jsonnumber.h"
namespace JSON{

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



    std::string ITOA(int i);

    class Value{ 
    public:
        using _Type= std::vector<std::pair<std::pair<std::string,JSON_status>,JSON_value>>;
        Value():type(JSON_NULL),vec_x(){}

        Value(int i):type(JSON_NUMBER),vec_x(){
            vec_x.push_back(std::pair<std::pair<std::string,JSON_status>,JSON_value>());
            vec_x[0].first.second.depth = 0;
            vec_x[0].first.second.is_object = false;
            vec_x[0].second.n = i;
            vec_x[0].second.type = JSON_NUMBER;
        }
        Value(const Value& _v):type(_v.type),vec_x(_v.vec_x){}

        Value&operator =(const Value& _rh){
            type = _rh.type;
            vec_x = _rh.vec_x;
            return *this;
        }


        Value &operator[](const char *s){
            _Type::iterator tempit = vec_x.end();
            _Type::iterator tempit2;
            for(_Type::iterator it = vec_x.begin();it !=vec_x.end(); ++it){
                if((*it).first.second.depth== 1 &&(*it).first.first == std::string(s) &&(*it).first.second.is_object == true){
                    tempit = it;
                }
            }
            if(tempit == vec_x.end()){
                throw;
            }
            tempit2 = tempit;
            for(_Type::iterator it = tempit + 1;it !=vec_x.end(); ++it){
                if((*it).first.second.depth> 1 ){
                    tempit2 = it;
                }else{

                    break;
                }
            }
            ++tempit2;



            vec_x = _Type(tempit,tempit2);
            Reduce_Depth();
            type = (*tempit).second.type;

            return *this;
        }
        Value &operator[](int pos){
            _Type::iterator tempit = vec_x.end();
            _Type::iterator tempit2;
            for(_Type::iterator it = vec_x.begin();it !=vec_x.end(); ++it){
                if((*it).first.second.depth== 1 &&(*it).first.first == std::string(ITOA(pos)) &&(*it).first.second.is_object == false){
                    tempit = it;
                }
            }
            if(tempit == vec_x.end()){
                throw;
            }
            tempit2 = tempit;
            for(_Type::iterator it = tempit + 1;it !=vec_x.end(); ++it){
                if((*it).first.second.depth> 1 ){
                    tempit2 = it;
                }else{

                    break;
                }
            }
            ++tempit2;



            vec_x = _Type(tempit,tempit2);
            Reduce_Depth();
            type = (*tempit).second.type;

            return *this;
        }


        bool isObject(){
            if(type == JSON_OBJECT)
                return true;
            return false;
        }

        bool isArray(){
            if(type == JSON_ARRAY)
                return true;
            return false;
        }

        bool isNumber(){
            if(type == JSON_NUMBER)
                return true;
            return false;
        }
        bool isTrue(){
            if(type == JSON_TRUE)
                return true;
            return false;
        }

        bool isFalse(){
            if(type == JSON_FALSE)
                return true;
            return false;
        }

        bool isNull(){
            if(type == JSON_NULL)
                return true;
            return false;
        }

        bool isString(){
            if(type == JSON_STRING)
                return true;
            return false;
        }

        double asNumber(){
            if(type == JSON_NUMBER)
                return vec_x[0].second.n;
            else throw;
            return 0.0;
        }

        bool asTrue(){
            if(type == JSON_TRUE)
                return true;

            else throw;
            return true;
        }

        bool asFalse(){
            if(type == JSON_FALSE)
                return false;
            else throw;
            return false;
        }
        void* asNull(){
            if(type == JSON_NULL)
                return NULL;
            else throw;
            return NULL;
        }

        std::string asString(){
            if(type == JSON_STRING)
                return vec_x[0].second.s;
            else throw;
            return "";
        }



    private:

        void set_vec(const std::vector<std::pair<std::pair<std::string,JSON_status>,JSON_value>> &v){
            vec_x = v;
        }
        void Reduce_Depth(){
            for(_Type::iterator it = vec_x.begin();it != vec_x.end(); ++it){
                --((*it).first.second.depth);
            }
        }

        JSONTYPE type;
        std::vector<std::pair<std::pair<std::string,JSON_status>,JSON_value>> vec_x;

        friend Value _parse(const char *json);
    };


    void parse_whitespace(JSON_context &c);
    void _except_obj_or_arr(JSON_context &c,std::vector<std::pair<std::string,JSON_status>> &no,
                            std::vector<std::pair<std::pair<std::string,JSON_status>,JSON_value>> &vec_x,int &depth);




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

    void parse_keystring(JSON_context &c    ,std::vector<std::pair<std::string,JSON_status>> &no,
    std::vector<std::pair<std::pair<std::string,JSON_status>,JSON_value>> &vec_x,int &depth){
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

            _except_obj_or_arr(c,no,vec_x,depth);
        }else if(c.json[0] == '-' ||ISDIGIT(c.json[0])){
            char *temp1 = c.json;
            parse_number(c);
            char *temp2 = c.json;
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
                parse_keystring(c,no,vec_x,depth);
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
    void parse_object(JSON_context &c,std::vector<std::pair<std::string,JSON_status>> &no,
                      std::vector<std::pair<std::pair<std::string,JSON_status>,JSON_value>> &vec_x,int &depth){
        parse_whitespace(c);
        if(c.json[0] == '"'){
            ++c.json;
            parse_keystring(c,no,vec_x,depth);
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

    void parse_value(JSON_context &c,int pos,std::vector<std::pair<std::string,JSON_status>> &no,
                     std::vector<std::pair<std::pair<std::string,JSON_status>,JSON_value>> &vec_x,int &depth){
        parse_whitespace(c);
        JSON_status status;
        status.depth = depth;
        status.is_object = false;
        no.push_back(std::make_pair(std::string(ITOA(pos)),status));
        if(c.json[0] == '{'||c.json[0] == '['){
            //不需要++depth
            _except_obj_or_arr(c,no,vec_x,depth);
        }else if(c.json[0] == '-' ||ISDIGIT(c.json[0])){
            char *temp1 = c.json;
            parse_number(c);
            char *temp2 = c.json;
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
            parse_value(c,pos,no,vec_x,depth);
        }else if(c.json[0] == ']'){
            ++(c.json);


            --depth;
        }else{
            throw;
        }
    }

    void parse_array(JSON_context &c,std::vector<std::pair<std::string,JSON_status>> &no,
                     std::vector<std::pair<std::pair<std::string,JSON_status>,JSON_value>> &vec_x,int & depth){
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
            parse_value(c,0,no,vec_x,depth);
        }



    }


    void _except_obj_or_arr(JSON_context &c,std::vector<std::pair<std::string,JSON_status>> &no,
                            std::vector<std::pair<std::pair<std::string,JSON_status>,JSON_value>> &vec_x,int &depth){
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
            parse_object(c,no,vec_x,depth);
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
            parse_array(c,no,vec_x,depth);
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



    Value _parse(const char *json){
        Value v;
        JSON_context c;
        std::vector<std::pair<std::string,JSON_status>> no;
        std::vector<std::pair<std::pair<std::string,JSON_status>,JSON_value>> vec_x;
        int depth = 0;
        init_context(c,json);
        parse_whitespace(c);

        _except_obj_or_arr(c,no,vec_x,depth);


        parse_whitespace(c);
        if(c.json[0] != '\0'){
            throw;
        }

        //panduanchongfu
        v.set_vec(vec_x);
        return v;
    }


    class Reader{
    public:
        Reader(){}

        bool parse(const std::string str,Value &v){
            bool b = true;
            try{
                v = _parse(str.c_str());
            }catch(...){

                v = Value();
                b = false;
            }
            return b;
        }

    };
};


int main(){

    return 0;
}
