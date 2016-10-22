# EasyJSON
[![Build Status](https://travis-ci.org/wangxiaozhi123/EasyJSON.svg?branch=master)](https://travis-ci.org/wangxiaozhi123/EasyJSON)

This is a JSON Parser written in C++11. It is a header-only C++ library. Just copy `include/json.h`and you can use it.
###Platforms
* Linux (gcc 4.8+,clang 3.5+) 
* Windows(Vistual Studio 2015,MinGW 4.9.2+) 
* Mac OSX (clang 3.5+)


###demo
```cpp
#include "json.h"
#include <iostream>

int main(){

    JSON::Value v,v2,v3;
    JSON::Value temp;

    JSON::Parse(temp, "{\"ss\":123,\"zz\":{},\"pp\":[1,2,3,4,5,{},[]]}");
    JSON::Parse(v, "{}"); JSON::Parse(v2, "[]"); JSON::Parse(v3, "{\"123456\":12345,\"zz\":[1,2,3,4,5,6]}");
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
    std::cout << temp.format() << v3.format();
    return 0;
}
```
output
<pre><code>0
1
1
0
1
1
456
1
444.5
{
    "ss": 123.000000,
    "zz":
    {

    },
    "pp":
    [
        1.000000,
        2.000000,
        3.000000,
        4.000000,
        5.000000,
        {

        },
        [
            "123"
        ]
    ]
}
{
    "123456": 12345.000000,
    "zz":
    [
        1.000000,
        2.000000,
        3.000000,
        4.000000,
        5.000000,
        6.000000
    ]
}
</code></pre>
