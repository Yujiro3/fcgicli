C++のFastCGIクライアントサンプル
======================
https://github.com/adoy/PHP-FastCGI-Client

PHP-FastCGI-ClientのC++移植

利用方法
------

### コンパイル ###
    
    $ g++ -o fcgicli main.cpp fcg_client.cpp
        
### コマンド ###

```cpp
fcgcli.params["GATEWAY_INTERFACE"] = "FastCGI/1.0";
fcgcli.params["REQUEST_METHOD"]    = "PUT";
fcgcli.params["SCRIPT_FILENAME"]   = dir;
fcgcli.params["SCRIPT_FILENAME"].append("/sample.php");
fcgcli.params["SERVER_SOFTWARE"]   = "cpp/fcgi_client";
fcgcli.params["SERVER_PROTOCOL"]   = "HTTP/1.1";
fcgcli.params["CONTENT_TYPE"]      = "application/x-www-form-urlencoded";
fcgcli.params["CONTENT_LENGTH"]    = conlength.str();

fcgcli.params["Original-Key"] = "originkey";

response = fcgcli.request(contents);

cout << response << endl;
```

### テスト実行 ###
```   
$ ./fcgicli
X-Powered-By: PHP/5.4.19-1~dotdeb.0
Set-Cookie: sheepsid=tlr88p6vsrva4c4njjomi0ajl6; path=/
Expires: Thu, 19 Nov 1981 08:52:00 GMT
Cache-Control: no-store, no-cache, must-revalidate, post-check=0, pre-check=0
Pragma: no-cache
Content-type: text/html

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
values
_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
Array
(
    [USER] => www-data
    [HOME] => /var/www
    [FCGI_ROLE] => RESPONDER
    [CONTENT_LENGTH] => 6
    [CONTENT_TYPE] => application/x-www-form-urlencoded
    [GATEWAY_INTERFACE] => FastCGI/1.0
    [Original-Key] => originkey
    [REQUEST_METHOD] => PUT
    [SCRIPT_FILENAME] => /home/yujiro3/fcgicli/sample.php
    [SERVER_PROTOCOL] => HTTP/1.1
    [SERVER_SOFTWARE] => cpp/fcgi_client
    [PHP_SELF] =>
    [REQUEST_TIME_FLOAT] => 1384176807.6554
    [REQUEST_TIME] => 1384176807
)
```

ライセンス
----------
Copyright &copy; 2013 Yujiro Takahashi  
Licensed under the [MIT License][MIT].  
Distributed under the [MIT License][MIT].  

[MIT]: http://www.opensource.org/licenses/mit-license.php
