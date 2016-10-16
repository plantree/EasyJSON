#ifndef UTF_H
#define UTF_H

#include <string>
#include <exception>
#include <cstdio>
#include "jsonnumber.h"

#ifdef _WIN32
#include <windows.h>
#include <cstdlib>
#include <bitset>
#else
#include <codecvt>
#include <locale>
#endif


namespace JSON {


class  Exception : public std::exception {
public:
    Exception(std::string const& msg);
    ~Exception() throw() override;
    char const* what() const throw() override;
private:
    std::string msg_;
};
Exception::Exception(std::string const& msg)
    : msg_(msg)
{}
Exception::~Exception() throw()
{}
char const* Exception::what() const throw()
{
    return msg_.c_str();
}

inline std::string ITOA(int i){
    char string[16] = {0};
    sprintf(string, "%d", i);
    return std::string(string);
}


inline bool ISHEX(char c){
     if((c >= '0' && c<= '9')||(c == 'a'||c == 'A')||(c == 'b'||c == 'B')||(c == 'c'||c == 'C')||(c == 'd'||c == 'D')
             ||(c == 'e'||c == 'E') || (c == 'f'||c == 'F')){
         return true;
     }

     return false;
}
inline bool U_ISHEX(char16_t c){
     if((c >= u'0' && c<= u'9')||(c == u'a'||c == u'A')||(c == u'b'||c == u'B')||(c == u'c'||c == u'C')||(c == u'd'||c == u'D')
             ||(c == u'e'||c == u'E') || (c == u'f'||c == u'F')){
         return true;
     }

     return false;
}

inline bool ISSPACE_OR_ZERO_OR_COMMA(char &c){
    char *p = &c;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p=='\0'||*p==',' ||*p=='}'||*p==']')
        return true;
    return false;

}

inline bool ISSPACE_OR_ZERO_OR_COMMA(char16_t &c){
    char16_t *p = &c;
    while (*p == u' ' || *p == u'\t' || *p == u'\n' || *p == u'\r' || *p==u'\0'||*p==u',' ||*p==u'}'||*p==u']')
        return true;
    return false;

}

inline bool ISDIGIT(char c){
     if(c>= '0'&& c<='9'){
         return true;
     }
     return false;
}

inline bool ISDIGIT(char16_t c){
     if(c>= u'0'&& c<=u'9'){
         return true;
     }
     return false;
}



using UTF8string = std::string;
using UTF16string = std::u16string;
using UTF32string = std::u32string;

#ifdef _WIN32
using ANSIstring = std::string;

ANSIstring UTF16ToANSI(const UTF16string &str);
UTF8string UTF16ToUTF8(const UTF16string &str);
UTF16string UTF8ToUTF16(const UTF8string &str);
UTF16string ANSIToUTF16(const ANSIstring &str);


UTF16string UTF32ToUTF16(const UTF32string &utf32s) {
    wchar_t *wc = (wchar_t *)malloc((utf32s.size() + 1) * sizeof(char32_t)); //分配比较多量的内存
    memset(wc, 0, (utf32s.size() + 1) * sizeof(char32_t));
    size_t pos = 0;
    std::bitset<20> bit;
    for (size_t i = 0; i < utf32s.size(); ++i, ++pos) {
        if (utf32s[i] < 65535) {
            wc[pos] = static_cast<wchar_t>(utf32s[i]);
        }
        else {
            bit = utf32s[i] - 0x10000;
            wc[pos++] = static_cast<wchar_t>((bit >> 10).to_ulong() + 0xD800);
            wc[pos] = static_cast<wchar_t>(((bit << 10) >> 10).to_ulong() + 0xDC00);
            // todo
        }
    }
    UTF16string u16s(reinterpret_cast<char16_t *>(wc));
    free(wc);
    return u16s;
}
UTF8string UTF32ToUTF8(const UTF32string &str) {
    return UTF16ToUTF8(UTF32ToUTF16(str));
}

ANSIstring UTF32ToANSI(const UTF32string &str) {
    return UTF16ToANSI(UTF32ToUTF16(str));
}




UTF32string UTF16ToUTF32(const UTF16string &utf16s) {
    char32_t *utf32str = (char32_t *)malloc((utf16s.size() + 1) * sizeof(char32_t));
    char32_t temp = 0;
    size_t pos = 0;
    for (size_t i = 0; i < utf16s.size(); ++i, ++pos) {
        if (utf16s[i] >> 10 == 54 && utf16s[i + 1] >> 10 == 55) {
            temp = (utf16s[i] - 0xD800);
            temp <<= 10;
            temp += (utf16s[i + 1] - 0xDC00);
            utf32str[pos] = temp + 0x10000;
            ++i;

        }
        else {
            utf32str[pos] = utf16s[i];

        }

    }
    utf32str[pos] = 0;
    UTF32string u32string(utf32str);
    free(utf32str);
    return u32string;
}

UTF8string UTF16ToUTF8(const UTF16string &str) {
    char* result;
    int textlen;
    textlen = WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<const wchar_t *>(str.c_str()), -1, NULL, 0, NULL, NULL);
    result = (char *)malloc((textlen + 1) * sizeof(char));
    memset(result, 0, sizeof(char) * (textlen + 1));
    WideCharToMultiByte(CP_UTF8, 0,reinterpret_cast<const wchar_t *>( str.c_str()), -1, result, textlen, NULL, NULL);
    ANSIstring temp(result);
    free(result);
    return temp;
}



ANSIstring UTF16ToANSI(const UTF16string &str) {
    char* result;
    int textlen;
    textlen = WideCharToMultiByte(CP_ACP, 0,reinterpret_cast<const wchar_t *>( str.c_str()), -1, NULL, 0, NULL, NULL);
    result = (char *)malloc((textlen + 1) * sizeof(char));
    memset(result, 0, sizeof(char) * (textlen + 1));
    WideCharToMultiByte(CP_ACP, 0,reinterpret_cast<const wchar_t *>( str.c_str()), -1, result, textlen, NULL, NULL);
    ANSIstring temp(result);
    free(result);
    return temp;
}




UTF32string UTF8ToUTF32(const UTF8string &str) {
    return UTF16ToUTF32(UTF8ToUTF16(str));
}


UTF16string UTF8ToUTF16(const UTF8string &str) {
    int textlen;
    wchar_t * result;
    textlen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    result = (wchar_t *)malloc((textlen + 1) * sizeof(wchar_t));
    memset(result, 0, (textlen + 1) * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, (LPWSTR)result, textlen);
    UTF16string temp(reinterpret_cast<char16_t *>(result));
    free(result);
    return temp;
}

ANSIstring UTF8ToANSI(const UTF8string &str) {
    return UTF16ToANSI(UTF8ToUTF16(str));
}

UTF32string ANSIToUTF32(const ANSIstring &str) {
    return UTF16ToUTF32(ANSIToUTF16(str));
}

UTF16string ANSIToUTF16(const ANSIstring &str) {
    wchar_t * result;
    int textlen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    result = (wchar_t *)malloc((textlen + 1) * sizeof(wchar_t));
    memset(result, 0, (textlen + 1) * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, (LPWSTR)result, textlen);
    UTF16string temp(reinterpret_cast<char16_t *>(result));
    free(result);
    return temp;
}

UTF8string ANSIToUTF8(const ANSIstring &str) {
    return UTF16ToUTF8(ANSIToUTF16(str));
}

#else

std::string UTF32ToUTF8(const std::u32string&u32s) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf8_ucs4_cvt;
    std::string u8_str_from_ucs4 = utf8_ucs4_cvt.to_bytes(std::u32string(u32s.c_str())); // ucs4 to utf-8
    return u8_str_from_ucs4;
}


std::u16string UTF8ToUTF16(const std::string &u8s);
std::u16string UTF32ToUTF16(const std::u32string &u32s) {
    return UTF8ToUTF16(UTF32ToUTF8(u32s));
}



std::string UTF16ToUTF8(const std::u16string &u16s) {
    std::u16string ucs2_cvt_str = u16s.c_str();
    std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> utf8_ucs2_cvt;
    std::string u8_str_from_ucs2 = utf8_ucs2_cvt.to_bytes(ucs2_cvt_str);
    return u8_str_from_ucs2;
}



std::u32string UTF8ToUTF32(const std::string &u8s);
std::u32string UTF16ToUTF32(const std::u16string &u16s) {

    return UTF8ToUTF32(UTF16ToUTF8(u16s));
}




std::u32string UTF8ToUTF32(const std::string &u8s) {
    std::string u8_source_str = u8s.c_str();
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf8_ucs4_cvt;
    std::u32string ucs4_cvt_str = utf8_ucs4_cvt.from_bytes(u8_source_str); // utf-8 to ucs4
    return std::u32string(ucs4_cvt_str);
}




std::u16string UTF8ToUTF16(const std::string &u8s) {
    std::string u8_source_str = u8s;
    std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> utf8_ucs2_cvt;
    std::u16string ucs2_cvt_str = utf8_ucs2_cvt.from_bytes(u8_source_str);
    return std::u16string(ucs2_cvt_str);
}

#endif



struct __JSON_context{
     char16_t *json;
 };





void __parse_whitespace(__JSON_context &c);
void ___except_obj_or_arr(__JSON_context &,int &);


void __parse_string(__JSON_context &c){
     //不需要消去空格
     char16_t *p = c.json;
     while(*p){

         if(*p == u'\"'){
             ++p;
             break;
         }
         if(*p == u'\\'){
             if(*(p+1) == u'n'||*(p+1) == u't' ||*(p+1) == u'r'
              ||*(p+1) == u'b'||*(p+1) == u'f'||*(p+1) == u'\\'
              ||*(p+1) == u'/'||*(p+1) == u'"'){
                 ++p;
             }else if(*(p+1) == u'u'&&U_ISHEX(*(p+2))&&U_ISHEX(*(p+3))&&U_ISHEX(*(p+4))&&U_ISHEX(*(p+5))){
                 p+=5;
             }else{
                 throw;
             }
         }
         ++p;
     }
     c.json = p;
}




  void __parse_number(__JSON_context &c){
     std::size_t i;
     for(i = 0;1;++i){
         if(ISSPACE_OR_ZERO_OR_COMMA(c.json[i])){
             break;
         }
     }
#ifdef _WIN32
     ANSIstring s = UTF16ToANSI(UTF16string(c.json,i));
#else
     std::string s =  UTF16ToUTF8(std::u16string(c.json,i));
#endif
     if(is_JSONNumber(s)){
         c.json +=i;
     }else{
         throw;
     }
  }

 void __parse_bool(__JSON_context &c){
     if(c.json[0] == u't'||c.json[1] == u'r'||c.json[2] == u'u'||c.json[3] == u'e'){
         c.json += 4;
     }else if(c.json[0] == u'f'||c.json[1] == u'a'||c.json[2] == u'l'||c.json[3] == u's'||c.json[4] == u'e'){
         c.json += 5;
     }else {
         throw;
     }
 }
 void __parse_null(__JSON_context &c){
     if(c.json[0] == u'n'||c.json[1] == u'u'||c.json[2] == u'l'||c.json[3] == u'l'){
         c.json += 4;
     }else {
         throw;
     }
 }

 void __parse_keystring(__JSON_context &c,int&depth){
     //不需要消去空格
     __parse_string(c);


     __parse_whitespace(c);
     if(c.json[0] == u':'){
         ++(c.json);
     }else{
         throw;
     }

     __parse_whitespace(c);

     if(c.json[0] == u'{'||c.json[0] == u'['){
         //不需要++depth
         ___except_obj_or_arr(c,depth);
     }else if(c.json[0] == u'-' ||ISDIGIT(c.json[0])){
         __parse_number(c);
     }else if(c.json[0] == u'"'){
         ++(c.json);
         __parse_string(c);
     }else if(c.json[0] == u'f'||c.json[0] == u't'){
         __parse_bool(c);
     }else if(c.json[0] == u'n'){
         __parse_null(c);
     }else{
         throw;
     }

     __parse_whitespace(c);

     if(c.json[0] == u','){
         ++(c.json);
         __parse_whitespace(c);
         if(c.json[0] == u'"'){
             ++(c.json);
             __parse_keystring(c,depth);
         }else{
             throw;
         }
     }else if(c.json[0] == u'}'){
         --depth;
         ++c.json;
     }else{
         throw;
     }


 }
 void __parse_object(__JSON_context &c,int &depth){
     __parse_whitespace(c);
     if(c.json[0] == u'"'){
         ++c.json;
         __parse_keystring(c,depth);
     }else if(c.json[0] == u'}'){
         ++c.json;
         --depth;
     }else{
         throw;
     }
 }


 void __parse_value(__JSON_context &c,int pos,int&depth){
     __parse_whitespace(c);

     if(c.json[0] == u'{'||c.json[0] == u'['){
         //不需要++depth
         ___except_obj_or_arr(c,depth);
     }else if(c.json[0] == u'-' ||ISDIGIT(c.json[0])){
         __parse_number(c);
     }else if(c.json[0] == u'"'){
         ++(c.json);
         __parse_string(c);
     }else if(c.json[0] == u'f'||c.json[0] == u't'){
         __parse_bool(c);
     }else if(c.json[0] == u'n'){
         __parse_null(c);
     }else{
         throw;
     }

     __parse_whitespace(c);
     if(c.json[0] == u','){
         ++(c.json);
         ++pos;
         __parse_value(c,pos,depth);
     }else if(c.json[0] == u']'){
         ++(c.json);


         --depth;
     }else{
         throw;
     }
 }

 void __parse_array(__JSON_context &c,int &depth){
     __parse_whitespace(c);
     if(c.json[0] == u']'){
         ++c.json;
         --depth;
         //直接return
     }else{
         __parse_value(c,0,depth);
     }



 }


 void ___except_obj_or_arr(__JSON_context &c,int &depth){
     if(c.json[0] == u'{'){
         ++depth;
         ++(c.json);
         __parse_object(c,depth);
     }else if(c.json[0] == u'['){
         ++depth;
         ++(c.json);
         __parse_array(c,depth);
     }else{
         throw;
     }
 }


 void __parse_whitespace(__JSON_context &c) {
     char16_t *p = c.json;
     while (*p == u' ' || *p == u'\t' || *p == u'\n' || *p == u'\r')
         p++;
     c.json = p;
 }



 void __init_context(__JSON_context &c,const  char16_t *json){
     c.json = const_cast<char16_t *>  (json);
 }
 void ____parse_valid(const char *json){
     __JSON_context c;
     int depth = 0;
#ifdef _WIN32
     UTF16string u16s = ANSIToUTF16(json);
#else
     std::u16string u16s = UTF8ToUTF16(json);
#endif
     __init_context(c,u16s.c_str());
     __parse_whitespace(c);

     ___except_obj_or_arr(c ,depth);
     __parse_whitespace(c);
     if(c.json[0] != u'\0'){
         throw;
    }

 }

#define UTF8 1
#define UTF16 2
#define UTF32 3
class UTFstring{
public:
    UTFstring(const char *ustr,int type){
        _type = type;
        _base_str = ustr;
        if(type == UTF8){
#ifdef _WIN32
            _u8s = ANSIToUTF8(ustr);
#else
            _u8s = ustr;
#endif
        }else if(type == UTF16){
#ifdef _WIN32
            _u16s = ANSIToUTF16(ustr);
#else
            _u16s = UTF8ToUTF16(ustr);
#endif

        }else if(type == UTF32){
#ifdef _WIN32
            _u32s = ANSIToUTF32(ustr);
#else
            _u32s = UTF8ToUTF32(ustr);
#endif
        }

    }


    UTFstring(const UTF8string &str){
        _type = 1;
#ifdef _WIN32
        _u8s = ANSIToUTF8(str);
        _base_str = str;
#else
        _u8s = str;
        _base_str = str;
#endif
    }

    UTFstring(const UTF16string &str){
        _type = 2;
        _u16s = str;
#ifdef _WIN32
        _base_str = UTF16ToANSI(str);
#else
        _base_str = UTF16ToUTF8(str);
#endif
    }

    UTFstring(const UTF32string &str){
        _type = 2;
        _u32s = str;
#ifdef _WIN32
        _base_str = UTF32ToANSI(str);
#else
        _base_str = UTF32ToUTF8(str);
#endif
    }

    int get_type(){
        return _type;
    }

    const char *get_string(){
        return _base_str.c_str();
    }

    UTF16string get_u16string(){
        UTF16string u16s;
        if(_type == 1){
            u16s = UTF8ToUTF16(_u8s);
        }else if(_type == 2){
            u16s = _u16s;
        }else if(_type == 3){
            u16s = UTF32ToUTF16(_u32s);
        }
        return u16s;
    }

private:
    int _type;
    std::string _base_str;
    UTF8string _u8s;
    UTF16string _u16s;
    UTF32string _u32s;
};

}
#endif // UTF_H
