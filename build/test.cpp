#include "../include/json.h"
#include <iostream>
#include <cstdio>



int main(){
    JSON::Value v,v2,v3;
    JSON::Value temp,temp2;

    //FILE *fp = fopen("/home/wang/桌面/EasyJSON-master/test/json/test.json","r");
    FILE *fp = fopen("../test/json/test1.json","r");
    FILE *fp2 = fopen("../test/json/test2.json","r");
    FILE *fp3 = fopen("../test/json/test3.json","r");
	char p[1400], p2[200], p3[200];
	fgets(p, 1400, fp);
	fgets(p2, 200, fp2);
	fgets(p3, 200, fp3);
	JSON::Parse(temp, "{\"ss\":123,\"zz\":{},\"pp\":[1,2,3,4,5,{},[]]}");
	JSON::Parse(v, p); JSON::Parse(v2, p2); JSON::Parse(v3, p3);
	JSON::SetValue(temp, JSON::Value("123"), "pp", 6, 0);
	std::cout << v.isArray() << "\n";
	std::cout << v3.isObject() << "\n";
	std::cout << JSON::Value(true).isBool() << "\n";
	std::cout << JSON::Value(false).toBool() << "\n";
	std::cout << JSON::Value().isNull() << "\n";
	std::cout << JSON::Value("123").isString() << "\n";
	std::cout << JSON::Value("456").toString() << "\n";
	std::cout << JSON::Value(33333).isNumber() << "\n";
	std::cout << JSON::Value(444.5).toNumber() << "\n";
	std::cout << temp.format() << v.format();
    return 0;
}
