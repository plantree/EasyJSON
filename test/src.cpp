#include "json.h"
#include <iostream>



int main(){
    JSON::Value v;
    JSON::Value temp;

    JSON::Parse(v,JSON::UTFstring("{\"aa\":{\"ss\":777}}",UTF8));
    JSON::_Type &t = v.get_vec();
    for(size_t i = 0;i < t.size(); ++i){
        printf("%d %s \n",t[i].first.second.depth,t[i].first.first.c_str());
    }
    printf("\n");
    JSON::Parse(temp,"[\"dd\",123]");
    JSON::SetValue(v,temp[1],"aa");

    for(size_t i = 0;i < t.size(); ++i){
        printf("%d %s \n",t[i].first.second.depth,t[i].first.first.c_str());
    }


\
    std::cout << v["aa"].toNumber();
//    FILE *fp = fopen("C:\\Users\\42937\\Desktop\\test\\pass3.json","r");
//    char p[1400];
//    fgets(p,1400,fp);
//    printf("%s\n",p);
//    JSON::_parse_valid(p);
    JSON::____parse_valid("{\"\":\"嘀嘀嘀\"}");

    return 0;
}

