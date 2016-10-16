#ifndef JSONNUMBER_H
#define JSONNUMBER_H
#include <string>
#include <cctype>
namespace JSON {


using Type = std::size_t;
bool is_Digit(const std::string &str){
    if(str.size() == 0){
        return false;
    }

    for(Type i = 0;i < str.size(); ++i){
        if(!isdigit(str[i])){
            return false;
        }
    }

    return true;
}

//0000e+0000 0000e0000 0000e-0000
bool parse__1(const std::string &str){
    int number_e = 0;
    std::size_t pos = 0;
    for(Type i = 0;i < str.size(); ++i){
        if(str[i] == 'e'||str[i] == 'E'){
            ++number_e;
            pos = i;
        }
    }

    if(number_e != 1 || (pos == 0 || pos == str.size() - 1 )){
        return false;
    }

    if(is_Digit(std::string(str,0,pos)) && is_Digit(std::string(str,pos+1))){
        return true;
    }else if( is_Digit(std::string(str,0,pos)) && (str[pos+1] == '-'||str[pos+1]=='+') && is_Digit(std::string(str,pos+2))){
        return true;
    }


    return false;
}

//123.456
bool parse__2(const std::string &str){
    int number_point = 0;
    std::size_t pos = 0;
    for(Type i = 0;i < str.size(); ++i){
        if(str[i] == '.'){
            ++number_point;
            pos = i;
        }
    }

    if(number_point != 1 || (pos == 0 || pos == str.size() - 1 )){
        return false;
    }

    if(is_Digit(std::string(str,0,pos)) && is_Digit(std::string(str,pos+1))){
        return true;
    }

    return false;


}

bool parse__3(const std::string &str){
    int number_e = 0;
    std::size_t pos = 0;
    for(Type i = 0;i < str.size(); ++i){
        if(str[i] == 'e'||str[i] == 'E'){
            ++number_e;
            pos = i;
        }
    }

    if(number_e != 1 || (pos == 0 || pos == str.size() - 1 )){
        return false;
    }

    if(parse__2(std::string(str,0,pos) ) && is_Digit(std::string(str,pos+1))){
        return true;
    }else if( parse__2(std::string(str,0,pos) ) && (str[pos+1] == '-'||str[pos+1]=='+') && is_Digit(std::string(str,pos+2))){
        return true;
    }

    return false;

}

bool is_JSONNumber1(const std::string &str){
    if (str.size() == 0){
        return false;
    }

    if(str.size() == 1 && str[0] == '0'){
        return true;
    }else if(str.size() >= 4 && str[0] == '0' && (str[1] == 'e' || str[1] == 'E' )
                             && (str[2] == '+' || str[2] == '-') &&
                             is_Digit( std::string(str,3) )){
        return true;
    }else if(str.size() >= 3 && str[0] == '0' && (str[1] == 'e' || str[1] == 'E' )
                             && is_Digit( std::string(str,2) )){
        return true;
    }else if(str.size() >= 3 && str[0] == '0' && str[1] == '.' && is_Digit(std::string(str,2))){
        return true;
    }else if(str.size() >= 3 && str[0] == '0' && str[1] == '.' && parse__1(std::string(str,2))){
        return true;
    }else if(str[0] != '0' && is_Digit(str)){
        return true;
    }else if(str[0] != '0' && parse__1(str)){
        return true;
    }else if(str[0] != '0' && parse__2(str)){
        return true;
    }else if(str[0] != '0' && parse__3(str)){
        return true;
    }

    return false;
}


bool is_JSONNumber(const std::string &str){
    if(str.size() == 0){
        return false;
    }
    if(str.size()>=2 && str[0] == '-' && is_JSONNumber1(std::string(str,1))){
        return true;
    }else if(is_JSONNumber1(str)){
        return true;
    }

    return false;
}
}
#endif // JSONNUMBER_H
