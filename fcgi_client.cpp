/**
 * fcgi_client.cpp
 *
 * C++ versions 4.4.5
 *
 *      fcgi_client : https://github.com/Yujiro3/fcgicli
 *      Copyright (c) 2011-2013 sheeps.me All Rights Reserved.
 *
 * @package         fcgi_client
 * @copyright       Copyright (c) 2011-2013 sheeps.me
 * @author          Yujiro Takahashi <yujiro3@gmail.com>
 * @filesource
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <cstring>
#include <cassert>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "fcgi_client.h"

namespace fcgi {
    /**
     * コンストラクタ
     *
     * @access public
     * @param string listen
     * @param int port
     */
    client::client(std::string listen, int port) {
        bzero(&header, sizeof(header));

        struct sockaddr_in *sin;
        sin = new struct sockaddr_in;
        bzero(sin, sizeof(struct sockaddr_in));
        sin->sin_family = AF_INET;
        sin->sin_addr.s_addr = inet_addr(listen.c_str());
        sin->sin_port = htons(port);

        sock = socket(AF_INET, SOCK_STREAM, 0);
        address = (struct sockaddr *)sin;
        addrlen  = sizeof(*sin);
    }

    /**
     * コンストラクタ
     *
     * @access public
     * @param string listen
     */
    client::client(std::string listen) {
        bzero(&header, sizeof(header));
        
        struct sockaddr_un *sun;
        sun = new struct sockaddr_un;

        bzero(sun, sizeof(struct sockaddr_un));
        sun->sun_family = AF_LOCAL;
        strcpy(sun->sun_path, listen.c_str());
        sock = socket(PF_UNIX, SOCK_STREAM, 0);
        address = (struct sockaddr *)sun;
        addrlen  = sizeof(*sun);
    }

    /**
     * デストラクタ
     *
     * @access public
     */
    client::~client() {
        delete address;
    }

    /**
     * Execute a request to the FastCGI application
     *
     * @access public
     * @param String stdin Content
     * @return String
     */
    std::string client::request(std::string stdin) {
        std::stringstream conlength;
        conlength << stdin.length();
        
        params["CONTENT_LENGTH"] = conlength.str();
        record.clear();

        std::string head("");
        head += char(0);
        head += char(1);    // Responder
        head += char(0);    // Keep alive
        head += char(0); head += char(0); head += char(0); head += char(0); head += char(0);
        _buildPacket(BEGIN_REQUEST, head, 1);
        
        std::string paramsRequest("");
        array_t::iterator rows;
        for (rows = params.begin(); rows != params.end(); rows++) {
            paramsRequest += _buildNvpair((*rows).first, (*rows).second);
        }

        if (paramsRequest.size() > 0) {
            _buildPacket(PARAMS, paramsRequest, 1);
        }
        _buildPacket(PARAMS, "", 1);

        if (stdin.size() > 0) {
            _buildPacket(STDIN, stdin, 1);
        }
        _buildPacket(STDIN, "", 1);

        if (!_connect()) {
            return std::string("Unable to connect to FastCGI application.");
        }
        write(sock, record.c_str(), record.size());

        do {
            _readPacket();
        } while (header.type != END_REQUEST);

        if (sock > 0) {
            close(sock);
        }

        /* 終了チェック */
        if (header.flag == REQUEST_COMPLETE) {
            return response;
        } else if (header.flag == CANT_MPX_CONN) {
            return std::string("This app can't multiplex [CANT_MPX_CONN]");
        } else if (header.flag == OVERLOADED) {
            return std::string("New request rejected; too busy [OVERLOADED]");
        } else if (header.flag == UNKNOWN_ROLE) {
            return std::string("Role value not known [UNKNOWN_ROLE]");
        }
        return response;
    }

    /**
     * パケット作成
     *
     * @access private
     * @param  int type
     * @param  string content
     * @param  int requestId
     */
    void client::_buildPacket(int type, std::string content, int requestId) {
        int contentLength = content.size();

        assert(contentLength >= 0 && contentLength <= MAX_LENGTH);
        
        record += char(VERSION);                        // version
        record += char(type);                           // type
        record += char((requestId     >> 8) & 0xff);    // requestIdB1
        record += char((requestId         ) & 0xff);    // requestIdB0
        record += char((contentLength >> 8) & 0xff);    // contentLengthB1
        record += char((contentLength     ) & 0xff);    // contentLengthB0
        record += char(0);                              // paddingLength
        record += char(0);                              // reserved
        record += content;                              // content
    }

    /**
     * Build an FastCGI Name value pair
     *
     * @access private
     * @param string name Name
     * @param string value Value
     * @return string FastCGI Name value pair
     */
    std::string client::_buildNvpair(std::string name, std::string value) {
        std::string nvpair("");
        
        int nlen = name.size();
        if (nlen < 128) {
            nvpair  = (unsigned char) nlen;                     // valueLengthB0
        } else {
            nvpair  = (unsigned char) ((nlen >> 24) | 0x80);    // valueLengthB3
            nvpair += (unsigned char) ((nlen >> 16) & 0xff);    // valueLengthB2
            nvpair += (unsigned char) ((nlen >>  8) & 0xff);    // valueLengthB1
            nvpair += (unsigned char) (nlen & 0xff);            // valueLengthB0
        }
    
        int vlen = value.size();
        if (vlen < 128) {
            nvpair += (unsigned char) vlen;                     // valueLengthB0
        } else {
            nvpair += (unsigned char) ((vlen >> 24) | 0x80);    // valueLengthB3
            nvpair += (unsigned char) ((vlen >> 16) & 0xff);    // valueLengthB2
            nvpair += (unsigned char) ((vlen >>  8) & 0xff);    // valueLengthB1
            nvpair += (unsigned char) (vlen & 0xff);            // valueLengthB0
        }
        nvpair += name + value;
    
        /* nameData & valueData */
        return nvpair;
    }

    /**
     * ヘッダーパケットの読込
     *
     * @access private
     * @return void
     */
    void client::_readPacketHeader() {
        char pack[HEADER_LEN];

        int len = read(sock, pack, sizeof(pack));
        if (len < 0) {
            throw "Unable to read response header.";
        }
        header.version       = ord(pack[0]);
        header.type          = ord(pack[1]);
        header.requestId     = (ord(pack[2]) << 8) + ord(pack[3]);
        header.contentLength = (ord(pack[4]) << 8) + ord(pack[5]);
        header.paddingLength = ord(pack[6]);
        header.reserved      = ord(pack[7]);
    }

    /**
     * Read a FastCGI Packet
     *
     * @access private
     * @return void
     */
    void client::_readPacket() {
        /* ヘッダーデータの読込 */
        _readPacketHeader();

        if (header.contentLength > 0) {
            char* buff = new char[header.contentLength];
            int length = read(sock, buff, header.contentLength);
            if (length < 0) {
                throw "Unable to read content buffer.";
            }

            if (header.type == STDOUT || header.type == STDERR) {
                response.append(buff);
            } else if (header.type == END_REQUEST) {
                header.flag = buff[4];
            }
            delete [] buff;
        }

        /* Paddingデータの読込 */
        if (header.paddingLength > 0) {
            char* padding = new char[header.paddingLength];
            int len = read(sock, padding, header.paddingLength);
            delete [] padding;
            if (len < 0) {
                throw "Unable to read padding buffer.";
            }
        } // if (header.paddingLength > 0)
    }

    /**
     * Create a connection to the FastCGI application
     *
     * @access private
     * @return void
     */
    bool client::_connect() {
        if (connect(sock, address, addrlen)) {
            return false;
        }
        return true;
    }
} // namespace fcgi 
