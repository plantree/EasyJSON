#include "json.h"

int main(){
    JSON::Value v;
    JSON::Reader r;
    r.parse("[123.4,5]",v);
    JSON::Value z = v[0];
    printf("%lf",z.asNumber());
//    FILE *fp = fopen("C:\\Users\\42937\\Desktop\\test\\pass3.json","r");
//    char p[1400];
//    fgets(p,1400,fp);
//    printf("%s\n",p);
//    JSON::_parse_valid(p);
    JSON::____parse_valid("{\"\":\"嘀嘀嘀\"}");
    return 0;
}
