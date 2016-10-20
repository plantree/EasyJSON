#ifndef _JSON_H_
#define _JSON_H_
#include <string>
#include <utility>
#include <vector>
#include <stack>
#include <algorithm>
#include <functional>
#include <cstddef>
#include "jsonnumber.h"
#include "utf.h"
namespace JSON {
    enum JSONTYPE {
        JSON_NULL = 0, JSON_FALSE, JSON_TRUE, JSON_NUMBER, JSON_STRING, JSON_ARRAY, JSON_OBJECT
    };

    struct JSON_value {
        std::string s;
        double n;
        JSONTYPE type;
    };

    struct JSON_context {
        char *json;
    };

    struct JSON_status {
        int depth = 0;
        bool is_object = true;
        bool is_realnull = false;
    };

    struct JSON_node {
        std::string node;
    };
    using _Type = std::vector<std::pair<std::pair<std::string, JSON_status>, JSON_value>>;

    void __distinct(_Type::iterator _it, int depth, std::vector<int> &vec_pos,_Type &vec_x) {
        std::string str;
        if (_it != vec_x.end())
            str = (*_it).first.first;
        for (_Type::iterator it = _it; it != vec_x.end() && (*it).first.second.depth >= depth; ++it) {
            if ((*it).first.second.depth == depth && it != _it && str == (*it).first.first) {
                int pos = _it - vec_x.begin();
                vec_pos.push_back(pos);
                //todo
                _Type::iterator temp_it = _it + 1;
                for (int i = 1; temp_it != vec_x.end() && (*temp_it).first.second.depth > depth; ++temp_it, ++i) {
                    vec_pos.push_back(pos + i);
                }
                __distinct(temp_it, depth, vec_pos,vec_x);
                break;//test
            }
        }
    }

    void distinct(_Type &vec_x) {
        std::vector<int> vec_pos;
        for (_Type::iterator it = vec_x.begin(); it != vec_x.end(); ++it) {
            if ((*it).second.type == JSON_OBJECT && it + 1 != vec_x.end() && (*(it + 1)).first.second.depth == (*it).first.second.depth + 1) {
                //todo
                __distinct(it + 1, (*it).first.second.depth + 1, vec_pos,vec_x);
            }
        }
        std::sort(vec_pos.begin(), vec_pos.end(), std::greater<int>());
        std::vector<int>::iterator iter = std::unique(vec_pos.begin(), vec_pos.end());
        vec_pos.erase(iter, vec_pos.end());

        for (std::size_t i = 0; i < vec_pos.size(); ++i) {
            vec_x.erase(vec_x.begin() + vec_pos[i]);
        }
    }


    class Value {
    public:
        explicit Value() :type(JSON_NULL), vec_x(),is_arr(false) ,s(),_format(""){
            vec_x.push_back(std::pair<std::pair<std::string, JSON_status>, JSON_value>());
            vec_x[0].first.second.depth = 0;
            vec_x[0].first.second.is_object = false;
            vec_x[0].second.type = JSON_NULL;


        }

        explicit Value(int i) :type(JSON_NUMBER), vec_x(),is_arr(false),s() ,_format(std::to_string(i)){
            vec_x.push_back(std::pair<std::pair<std::string, JSON_status>, JSON_value>());
            vec_x[0].first.second.depth = 0;
            vec_x[0].first.second.is_object = false;
            vec_x[0].second.n = i;
            vec_x[0].second.type = JSON_NUMBER;

        }

        explicit Value(double d) :type(JSON_NUMBER), vec_x(),is_arr(false),s() ,_format(std::to_string(d)){
            vec_x.push_back(std::pair<std::pair<std::string, JSON_status>, JSON_value>());
            vec_x[0].first.second.depth = 0;
            vec_x[0].first.second.is_object = false;
            vec_x[0].second.n = d;
            vec_x[0].second.type = JSON_NUMBER;

        }

        explicit Value(bool b) :type(), vec_x() ,is_arr(false),s(),_format(){
            vec_x.push_back(std::pair<std::pair<std::string, JSON_status>, JSON_value>());
            vec_x[0].first.second.depth = 0;
            vec_x[0].first.second.is_object = false;
            if (b){
                vec_x[0].second.type = JSON_TRUE;
                _format = "true";
            }
            else{
                vec_x[0].second.type = JSON_FALSE;
                _format = "false";
            }


        }

        explicit Value(const char *str) :type(JSON_STRING), vec_x() ,is_arr(false),s(),_format("\"" + std::string(str)+"\""){
            vec_x.push_back(std::pair<std::pair<std::string, JSON_status>, JSON_value>());
            vec_x[0].first.second.depth = 0;
            vec_x[0].first.second.is_object = false;
            vec_x[0].second.s = str;
            vec_x[0].second.type = JSON_STRING;

        }
        Value(const Value& _v) :type(_v.type), vec_x(_v.vec_x),is_arr(false),s() ,_format(_v._format){}


        Value& operator =(Value _rh) {
            type = _rh.type;
            vec_x = _rh.vec_x;
            s = _rh.s;
            _format = _rh._format;

            if(is_arr){
                throw Exception("error");
            }


            return *this;
        }


        Value operator[](const char *s) {
            auto tempvec_x = vec_x;
            auto temp_type = type;
            _Type::iterator tempit = vec_x.end();
            _Type::iterator tempit2;
            for (_Type::iterator it = vec_x.begin(); it != vec_x.end(); ++it) {
                if ((*it).first.second.depth == 1 && (*it).first.first == std::string(s) && (*it).first.second.is_object == true) {
                    tempit = it;
                }
            }
            if (tempit == vec_x.end()) {
                throw;
            }
            tempit2 = tempit;
            for (_Type::iterator it = tempit + 1; it != vec_x.end(); ++it) {
                if ((*it).first.second.depth > 1) {
                    tempit2 = it;
                }
                else {
                    break;
                }
            }
            ++tempit2;



            vec_x = _Type(tempit, tempit2);
            Reduce_Depth();
            type = (*vec_x.begin()).second.type;
            distinct(vec_x);

            Value temp_v(type,vec_x);
            temp_v.set_array(true);


            //huanyuan
            vec_x = tempvec_x;
            type = temp_type;
            return temp_v;
        }
        Value operator[](int pos) {
            auto tempvec_x = vec_x;
            auto temp_type = type;
            _Type::iterator tempit = vec_x.end();
            _Type::iterator tempit2;
            for (_Type::iterator it = vec_x.begin(); it != vec_x.end(); ++it) {
                if ((*it).first.second.depth == 1 && (*it).first.first == std::string(ITOA(pos)) && (*it).first.second.is_object == false) {
                    tempit = it;
                }
            }
            if (tempit == vec_x.end()) {
                throw;
            }
            tempit2 = tempit;
            for (_Type::iterator it = tempit + 1; it != vec_x.end(); ++it) {
                if ((*it).first.second.depth> 1) {
                    tempit2 = it;
                }
                else {

                    break;
                }
            }
            ++tempit2;

            vec_x = _Type(tempit, tempit2);
            Reduce_Depth();
            //fix bug
            type = (*vec_x.begin()).second.type;
            //distinct
            distinct(vec_x);

            Value temp_v(type,vec_x);
            temp_v.set_array(true);



            //huanyuan
            vec_x = tempvec_x;
            type = temp_type;
            return temp_v;
        }



        bool isObject() {
            if (type == JSON_OBJECT)
                return true;
            return false;
        }

        bool isArray() {
            if (type == JSON_ARRAY)
                return true;
            return false;
        }

        bool isNumber() {
            if (type == JSON_NUMBER)
                return true;
            return false;
        }
        bool isBool() {
            if (type == JSON_TRUE || type == JSON_FALSE)
                return true;
            return false;
        }



        bool isNull() {
            if (type == JSON_NULL)
                return true;
            return false;
        }

        bool isString() {
            if (type == JSON_STRING)
                return true;
            return false;
        }

        double toNumber() {
            if (type == JSON_NUMBER)
                return vec_x[0].second.n;
            else Exception("Error: This is not a number!");
            return 0.0;
        }




        const char * toString() {
            if (type == JSON_STRING)
                return vec_x[0].second.s.c_str();
            else throw Exception("Error: This is not a string!");
            return "";
        }

        const char *format(){
            _format = "";

            if(vec_x.size() == 1){
                if(vec_x[0].second.type == JSON_NULL &&vec_x[0].first.second.is_realnull == true){
                    _format += "null";
                }else if(vec_x[0].second.type == JSON_TRUE){
                    _format += "true";
                }else if(vec_x[0].second.type == JSON_FALSE){
                    _format += "false";
                }else if(vec_x[0].second.type == JSON_NUMBER){
                    _format += std::to_string(vec_x[0].second.n);
                }else if(vec_x[0].second.type == JSON_STRING){
                    _format += "\""+ vec_x[0].second.s+"\"";
                }else if(vec_x[0].second.type == JSON_OBJECT){
                    _format += "{}";
                }
                return _format.c_str();
            }else if(vec_x.size() == 2&&vec_x[0].second.type == JSON_ARRAY&&vec_x[1].second.type == JSON_NULL){
                if(vec_x[1].first.second.is_realnull){
                    return "[null]";
                }else
                    return "[]";
            }

            std::stack<bool> obj_or_arr;
            if(vec_x[0].second.type == JSON_ARRAY){
                _format += "[";
            }else if(vec_x[0].second.type == JSON_OBJECT){
                _format += "{";
            }
            for(size_t i = 1;i < vec_x.size(); ++i){
                if(vec_x[i].first.second.depth <= vec_x[i - 1 ].first.second.depth&&vec_x[i -1].second.type == JSON_OBJECT){
                    _format += "}";
                    obj_or_arr.pop();
                }
                if(vec_x[i].first.second.depth < vec_x[i - 1 ].first.second.depth){
                    for(int j = 0;j < vec_x[i-1].first.second.depth -vec_x[i].first.second.depth;++j){
                        if(obj_or_arr.top()) _format += "}";
                        else _format += "]";

                        obj_or_arr.pop();
                    }
                }

                if(vec_x[i].first.second.depth <= vec_x[i - 1 ].first.second.depth){
                    _format += ",";
                }



                if(vec_x[i].first.second.is_object == true){
                    _format += "\""+vec_x[i].first.first +"\":";
                }


                if(vec_x[i].second.type == JSON_NULL &&vec_x[i].first.second.is_realnull == true){
                    _format += "null";
                }else if(vec_x[i].second.type == JSON_TRUE){
                    _format += "true";
                }else if(vec_x[i].second.type == JSON_FALSE){
                    _format += "false";
                }else if(vec_x[i].second.type == JSON_NUMBER){
                    _format += std::to_string(vec_x[i].second.n);
                }else if(vec_x[i].second.type == JSON_STRING){
                    _format += "\""+ vec_x[i].second.s+"\"";
                }else if(vec_x[i].second.type == JSON_ARRAY){
                    _format += "[";
                    obj_or_arr.push(false);
                }else if(vec_x[i].second.type == JSON_OBJECT){
                    _format += "{";
                    obj_or_arr.push(true);
                }


            }
            while(!obj_or_arr.empty()){
                if(obj_or_arr.top()) _format += "}";
                else _format += "]";

                obj_or_arr.pop();
            }
            if(vec_x[0].second.type == JSON_ARRAY){
                _format += "]";
            }if(vec_x[0].second.type == JSON_OBJECT){
                _format += "}";
            }

            process_fp(_format);

            return _format.c_str();
        }


        ~Value(){}

    //private:
        Value(JSONTYPE _T,const _Type &_v) :type(_T), vec_x(_v),is_arr(true),s() {}



        void set_vec(const std::vector<std::pair<std::pair<std::string, JSON_status>, JSON_value>> &v) {
            vec_x = v;
        }
        void set_type(JSONTYPE _type) {
            type = _type;
        }

        void Reduce_Depth() {
            for (_Type::iterator it = vec_x.begin(); it != vec_x.end(); ++it) {
                --((*it).first.second.depth);
            }
        }
        void set_array(bool _b){
            is_arr = _b;
        }

        bool is_array(){
            return is_arr;
        }

        std::pair<int,int> get_pos(const char *s){
            _Type::iterator tempit = vec_x.end();
            int pos = 0;
            _Type::iterator tempit2;
            int pos2;
            for (_Type::iterator it = vec_x.begin(); it != vec_x.end(); ++it,++pos) {
                if ((*it).first.second.depth == 1 && (*it).first.first == std::string(s) && (*it).first.second.is_object == true) {
                    tempit = it;
                    break;
                }
            }
            if (tempit == vec_x.end()) {
                throw;
            }
            tempit2 = tempit;
            pos2 = pos + 1;
            for (_Type::iterator it = tempit + 1; it != vec_x.end(); ++it,++pos2) {
                if ((*it).first.second.depth > 1) {
                    tempit2 = it;
                }
                else {
                    break;
                }
            }
            //++tempit2;

            return std::pair<int,int>(pos,pos2);

        }
        std::pair<int,int> get_pos(int i){

            _Type::iterator tempit = vec_x.end();
            int pos = 0;
            _Type::iterator tempit2;
            int pos2;
            for (_Type::iterator it = vec_x.begin(); it != vec_x.end(); ++it,++pos) {
                if ((*it).first.second.depth == 1 && (*it).first.first == std::string(ITOA(i)) && (*it).first.second.is_object == false) {
                    tempit = it;
                    break;
                }
            }
            if (tempit == vec_x.end()) {
                throw;
            }
            tempit2 = tempit;
            pos2 = pos + 1;
            for (_Type::iterator it = tempit + 1; it != vec_x.end(); ++it,++pos2) {
                if ((*it).first.second.depth> 1) {
                    tempit2 = it;
                }
                else {

                    break;
                }
            }
            //++tempit2;
            return std::pair<int,int>(pos,pos2);
        }

        int get_size(){
            return static_cast<int>(vec_x.size());
        }

        _Type&get_vec(){
            return vec_x;
        }

        void padLine(int level,std::string &temp){
            int i;
            //if(temp != std::string(""))
            temp+= "\n";
            for(i=0; i < level; i++)
                temp += "    ";
        }

        void process_fp(std::string&str) {

            int level=0;
            int last=-1;
            int c;
            int instr=0;
            int inesc=0;
            std::string temp;

            for(size_t i =0;i < str.size(); ++i){
                c = static_cast<int> (str[i]);
                if(instr && c == '\\'){    /* escape */
                    inesc=!inesc;
                }else if (inesc){
                    inesc = 0;
                    if ('"' == c){
                        temp += "\"";
                        last = '1';
                        continue;
                    }
                }else{
                    if(last =='"')
                        instr=!instr;    /* quote */

                    if(!instr && last==':')
                        temp += " ";

                    if(!instr) {
                        if (last == '{' || last == '[')
                            padLine(++level,temp);
                        else if (last == ',')
                            padLine(level,temp);

                        if (c == '}' || c == ']') {
                            if (level > 0)
                                level--;
                            padLine(level,temp);
                        }else if (c == '{' || c == '[') {
                            if (last != ',' && last != '[' && last != '{')
                                padLine(level,temp);
                        }
                    }
                }


                last=c;
                if (instr || !(' ' == c || '\t' == c || '\r' == c || '\n' == c)){
                    temp += static_cast<char>(c);
                }


            }
            std::string::iterator it = temp.begin();
            for(;it != temp.end();++it){
                if(*it != '\n'&&*it != ' '){
                    break;
                }

            }
            str = std::string(it,temp.end()) + "\n";
        }


        JSONTYPE type;
        std::vector<std::pair<std::pair<std::string, JSON_status>, JSON_value>> vec_x;
        bool is_arr;
        std::string s;
        std::string _format;

        friend Value _parse(const char *json);
        template<typename... _Args>
        friend int __SetValue(Value v,int &pos,const char *s,_Args&&... __args);
        template<typename... _Args>
        friend int __SetValue(Value v,int &pos,int i,_Args&&... __args);

        template<typename... _Args>
        friend void SetValue(Value &v,const Value &__v, _Args&&... __args);
        friend int __SetValue(Value v,int &pos);


    };


    void parse_whitespace(JSON_context &c);
    void _except_obj_or_arr(JSON_context &c, std::vector<std::pair<std::string, JSON_status>> &no,
        std::vector<std::pair<std::pair<std::string, JSON_status>, JSON_value>> &vec_x, int &depth);







    void parse_string(JSON_context &c) {
        //不需要消去空格
        char *p = c.json;
        while (*p) {

            if (*p == '\"') {
                ++p;
                break;
            }
            if (*p == '\\') {
                if (*(p + 1) == 'n' || *(p + 1) == 't' || *(p + 1) == 'r'
                    || *(p + 1) == 'b' || *(p + 1) == 'f' || *(p + 1) == '\\'
                    || *(p + 1) == '/' || *(p + 1) == '"') {
                    ++p;
                }
                else if (*(p + 1) == 'u'&&ISHEX(*(p + 2)) && ISHEX(*(p + 3)) && ISHEX(*(p + 4)) && ISHEX(*(p + 5))) {
                    p += 5;
                }
                else {
                    throw Exception("Error: This is not string!");
                }
            }
            ++p;
        }
        c.json = p;
    }

    void parse_number(JSON_context &c) {
        std::size_t i;
        for (i = 0; 1; ++i) {
            if (ISSPACE_OR_ZERO_OR_COMMA(c.json[i])) {
                break;
            }
        }
        if (is_JSONNumber(std::string(c.json, i))) {
            c.json += i;
        }
        else {
            throw Exception("Error: This is not number!");
        }
    }

    void parse_bool(JSON_context &c) {
        if (c.json[0] == 't' || c.json[1] == 'r' || c.json[2] == 'u' || c.json[3] == 'e') {
            c.json += 4;
        }
        else if (c.json[0] == 'f' || c.json[1] == 'a' || c.json[2] == 'l' || c.json[3] == 's' || c.json[4] == 'e') {
            c.json += 5;
        }
        else {
            throw Exception("Error: This is not bool!");
        }
    }
    void parse_null(JSON_context &c) {
        if (c.json[0] == 'n' || c.json[1] == 'u' || c.json[2] == 'l' || c.json[3] == 'l') {
            c.json += 4;
        }
        else {
            throw Exception("Error: This is not null!");
        }
    }

    void parse_keystring(JSON_context &c, std::vector<std::pair<std::string, JSON_status>> &no,
        std::vector<std::pair<std::pair<std::string, JSON_status>, JSON_value>> &vec_x, int &depth) {
        //不需要消去空格
        char *temp1 = c.json;
        parse_string(c);
        char *temp2 = c.json;
        ptrdiff_t num = (temp2 - temp1) - 1;
        JSON_status status;
        status.depth = depth;
        status.is_object = true;
        no.push_back(std::make_pair(std::string(temp1, num), status));
        parse_whitespace(c);
        if (c.json[0] == ':') {
            ++(c.json);
        }
        else {
            throw Exception("Error: This is not colon!");
        }

        parse_whitespace(c);

        if (c.json[0] == '{' || c.json[0] == '[') {
            //不需要++depth
            //不需要vec_p.push_back

            _except_obj_or_arr(c, no, vec_x, depth);
        }
        else if (c.json[0] == '-' || ISDIGIT(c.json[0])) {
            char *temp1 = c.json;
            parse_number(c);
            char *temp2 = c.json;
            ptrdiff_t num = (temp2 - temp1);
            JSON_value jv;
            jv.type = JSON_NUMBER;
            jv.n = strtod(std::string(temp1, num).c_str(), NULL);

            vec_x.push_back(std::make_pair(no[no.size() - 1], jv));


        }
        else if (c.json[0] == '"') {
            ++(c.json);
            char *temp1 = c.json;
            parse_string(c);
            char *temp2 = (c.json - 1);
            ptrdiff_t num = (temp2 - temp1);
            JSON_value jv;
            jv.type = JSON_STRING;
            jv.s = std::string(temp1, num);


            vec_x.push_back(std::make_pair(no[no.size() - 1], jv));

        }
        else if (c.json[0] == 'f' || c.json[0] == 't') {
            bool b;
            if (c.json[0] == 't') {
                b = true;
            }
            else b = false;

            parse_bool(c);
            JSON_value jv;

            if (b) {
                jv.type = JSON_TRUE;
            }
            else {
                jv.type = JSON_FALSE;
            }


            vec_x.push_back(std::make_pair(no[no.size() - 1], jv));


        }
        else if (c.json[0] == 'n') {
            parse_null(c);
            JSON_value jv;
            jv.type = JSON_NULL;
            no[no.size() - 1].second.is_realnull = true;
            vec_x.push_back(std::make_pair(no[no.size() - 1], jv));

        }
        else {
            throw Exception("Error: This is not value!");
        }

        parse_whitespace(c);

        if (c.json[0] == ',') {
            ++(c.json);
            parse_whitespace(c);
            if (c.json[0] == '"') {
                ++(c.json);
                parse_keystring(c, no, vec_x, depth);
            }
            else {
                throw Exception("Error: This is not string!");
            }
        }
        else if (c.json[0] == '}') {
            --depth;
            ++c.json;
        }
        else {
            throw Exception("Error: This is not comma or }!");
        }


    }
    void parse_object(JSON_context &c, std::vector<std::pair<std::string, JSON_status>> &no,
        std::vector<std::pair<std::pair<std::string, JSON_status>, JSON_value>> &vec_x, int &depth) {
        parse_whitespace(c);
        if (c.json[0] == '"') {
            ++c.json;
            parse_keystring(c, no, vec_x, depth);
        }
        else if (c.json[0] == '}') {
            ++c.json;
            --depth;
        }
        else {
            throw Exception("Error: This is not string!");
        }
    }




    void parse_value(JSON_context &c, int pos, std::vector<std::pair<std::string, JSON_status>> &no,
        std::vector<std::pair<std::pair<std::string, JSON_status>, JSON_value>> &vec_x, int &depth) {
        parse_whitespace(c);
        JSON_status status;
        status.depth = depth;
        status.is_object = false;
        no.push_back(std::make_pair(std::string(ITOA(pos)), status));
        if (c.json[0] == '{' || c.json[0] == '[') {
            //不需要++depth
            _except_obj_or_arr(c, no, vec_x, depth);
        }
        else if (c.json[0] == '-' || ISDIGIT(c.json[0])) {
            char *temp1 = c.json;
            parse_number(c);
            char *temp2 = c.json;
            ptrdiff_t num = (temp2 - temp1);
            JSON_value jv;
            jv.type = JSON_NUMBER;
            jv.n = strtod(std::string(temp1, num).c_str(), NULL);

            vec_x.push_back(std::make_pair(no[no.size() - 1], jv));
        }
        else if (c.json[0] == '"') {
            ++(c.json);
            char *temp1 = c.json;
            parse_string(c);
            char *temp2 = (c.json - 1);
            ptrdiff_t num = (temp2 - temp1);
            JSON_value jv;
            jv.type = JSON_STRING;
            jv.s = std::string(temp1, num);
            vec_x.push_back(std::make_pair(no[no.size() - 1], jv));
        }
        else if (c.json[0] == 'f' || c.json[0] == 't') {
            bool b;
            if (c.json[0] == 't') {
                b = true;
            }
            else b = false;

            parse_bool(c);
            JSON_value jv;

            if (b) {
                jv.type = JSON_TRUE;
            }
            else {
                jv.type = JSON_FALSE;
            }

            vec_x.push_back(std::make_pair(no[no.size() - 1], jv));

        }
        else if (c.json[0] == 'n') {
            parse_null(c);
            JSON_value jv;
            jv.type = JSON_NULL;
            no[no.size() - 1].second.is_realnull = true;
            vec_x.push_back(std::make_pair(no[no.size() - 1], jv));
        }
        else {
            throw Exception("Error: This is not value!");
        }

        parse_whitespace(c);
        if (c.json[0] == ',') {
            ++(c.json);
            ++pos;
            parse_value(c, pos, no, vec_x, depth);
        }
        else if (c.json[0] == ']') {
            ++(c.json);


            --depth;
        }
        else {
            throw Exception("Error: This is not comma or ]!");
        }
    }

    void parse_array(JSON_context &c, std::vector<std::pair<std::string, JSON_status>> &no,
        std::vector<std::pair<std::pair<std::string, JSON_status>, JSON_value>> &vec_x, int & depth) {
        parse_whitespace(c);
        if (c.json[0] == ']') {
            ++c.json;


            JSON_status status;
            status.depth = depth;
            status.is_object = false;
            no.push_back(std::make_pair(std::string(ITOA(0)), status));
            JSON_value jv;
            jv.type = JSON_NULL;
            vec_x.push_back(std::make_pair(no[no.size() - 1], jv));

            --depth;
            //直接return
        }
        else {
            parse_value(c, 0, no, vec_x, depth);
        }



    }


    void _except_obj_or_arr(JSON_context &c, std::vector<std::pair<std::string, JSON_status>> &no,
        std::vector<std::pair<std::pair<std::string, JSON_status>, JSON_value>> &vec_x, int &depth) {
        if (c.json[0] == '{') {
            JSON_value jv; jv.type = JSON_OBJECT;
            if (no.size() != 0) {
                vec_x.push_back(std::make_pair(no[no.size() - 1], jv));
            }
            else {
                JSON_status js;
                js.depth = 0;
                js.is_object = true;

                std::pair<std::string, JSON_status> p = std::make_pair(std::string(""), js);

                jv.type = JSON_OBJECT;
                std::pair<std::pair<std::string, JSON_status>, JSON_value> pp = std::make_pair(p, jv);

                vec_x.push_back(pp);

            }

            ++depth;
            ++(c.json);
            parse_object(c, no, vec_x, depth);
        }
        else if (c.json[0] == '[') {
            JSON_value jv; jv.type = JSON_ARRAY;
            if (no.size() != 0) {
                vec_x.push_back(std::make_pair(no[no.size() - 1], jv));
            }
            else {
                JSON_status js;
                js.depth = 0;
                js.is_object = false;

                std::pair<std::string, JSON_status> p = std::make_pair(std::string(""), js);

                jv.type = JSON_ARRAY;
                std::pair<std::pair<std::string, JSON_status>, JSON_value> pp = std::make_pair(p, jv);

                vec_x.push_back(pp);

            }


            ++depth;
            ++(c.json);
            parse_array(c, no, vec_x, depth);
        }
        else {
            throw Exception("Error: This is not object or array!");
        }
    }


    void parse_whitespace(JSON_context &c) {
        char *p = c.json;
        while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
            p++;
        c.json = p;
    }



    void init_context(JSON_context &c, const  char *json) {
        c.json = const_cast<char*>  (json);
    }



    Value _parse(const char *json) {
        Value v;
        JSON_context c;
        std::vector<std::pair<std::string, JSON_status>> no;
        std::vector<std::pair<std::pair<std::string, JSON_status>, JSON_value>> vec_x;
        int depth = 0;
        init_context(c, json);
        parse_whitespace(c);

        _except_obj_or_arr(c, no, vec_x, depth);
        parse_whitespace(c);
        if (c.json[0] != '\0') {
            throw Exception("Error: Additional characters!");
        }

        //panduanchongfu
        distinct(vec_x);

        v.set_vec(vec_x);
        v.set_type(vec_x[0].second.type);
        v.set_array(false);

        return v;
    }

    bool Parse(Value &v, const std::string &str) {
        bool b = true;
        try {
            v = _parse(str.c_str());
        }
        catch (...) {
            v = Value();
            b = false;
        }
        return b;
    }
    bool Parse(Value &v, UTFstring ustr) {
        bool b = true;
        ____parse_valid(ustr.get_string());
        try {
            v = _parse(ustr.get_string());
        }
        catch (...) {
            v = Value();
            b = false;
        }
        return b;
    }

    template<typename... _Args>
    int __SetValue(Value v,int &pos,const char *s,_Args&&... __args);
    template<typename... _Args>
    int __SetValue(Value v,int &pos,int i,_Args&&... __args);


    int __SetValue(Value v,int &pos){
        pos = pos;
        return static_cast<int>(v.get_size());
    }

    template<typename... _Args>
    int __SetValue(Value v,int &pos,int i,_Args&&... __args){
        pos += v.get_pos(i).first;
        return __SetValue(v[i],pos,__args...);


    }
    template<typename... _Args>
    int __SetValue(Value v,int &pos,const char *s,_Args&&... __args){
        pos += v.get_pos(s).first;
        return __SetValue(v[s],pos,__args...);

    }


    template<typename... _Args>
    int _SetValue(Value &v,int &pos,_Args&&... __args){
        return __SetValue(v,pos,__args...);
    }

    void __add_vec_depth(_Type &vec,int depth){
        size_t s = vec.size();
        for(size_t i= 0;i < s; ++i){
            vec[i].first.second.depth += (depth);
        }

    }

    template<typename... _Args>
    void SetValue(Value &v,const Value &__v, _Args&&... __args){
        /*if(v.is_array()){
            throw;
        }*/

        int pos = 0;
        int _size;
        if(sizeof...(__args) != 0)
            _size = _SetValue(v,pos,__args...);
        else{
            v = __v;
            return;
        }

        Value tempv = __v;
        int _depth = (v.get_vec().begin() + (pos))->first.second.depth;
        __add_vec_depth( tempv.get_vec(),_depth);


        _Type temp1(v.get_vec().begin(),(v.get_vec().begin() + (pos + 1)));
        _Type temp2(v.get_vec().begin() + (pos + _size),v.get_vec().end());

        (temp1.end() - 1)->second.type = tempv.get_vec().begin()->second.type;
        if(tempv.get_vec().size() != 1){
            temp1.insert(temp1.end(),tempv.get_vec().begin() + 1,tempv.get_vec().end());
        }else{
            if (tempv.get_vec().begin()->second.type== JSON_NUMBER)
                (temp1.end()-1)->second.n =  tempv.get_vec().begin()->second.n;
            else if(tempv.get_vec().begin()->second.type== JSON_STRING)
                (temp1.end()-1)->second.s =  tempv.get_vec().begin()->second.s;
        }
        temp1.insert(temp1.end(),temp2.begin(),temp2.end());


        v.set_array(false);
        v.set_type(temp1[0].second.type);
        v.set_vec(temp1);

    }

}

#endif
