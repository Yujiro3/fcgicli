/**
 * fcgi_client.h
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

#ifndef __FCGI_CLIENT_H__
#define __FCGI_CLIENT_H__

#include <string>
#include <map>

namespace fcgi {
    /**
     * 2次元配列コンテナの型定義
     * @typedef std::map<std::string, std::string>
     */
    typedef std::map<std::string, std::string> array_t;

    /**
     * 文字の ASCII 値を返す
     */
    inline unsigned int ord(int val) {
        return ((unsigned int) val & 0xff);
    }

    /**
     * ヘッダー
     * @typedef struct header_t
     */
    typedef struct {
        unsigned int version;
        unsigned int type;
        unsigned int requestId;
        unsigned int contentLength;
        unsigned int paddingLength;
        unsigned int reserved;
        unsigned int flag;
    } header_t;

    /**
     * バージョン
     * @const unsigned int
     */
    const unsigned int VERSION = 1;

    /**
     * タイプ：開始リクエスト
     * @const unsigned int
     */
    const unsigned int BEGIN_REQUEST = 1;

    /**
     * タイプ：中断リクエスト
     * @const unsigned int
     */
    const unsigned int ABORT_REQUEST = 2;

    /**
     * タイプ：終了リクエスト
     * @const unsigned int
     */
    const unsigned int END_REQUEST = 3;

    /**
     * タイプ：引数
     * @const unsigned int
     */
    const unsigned int PARAMS = 4;

    /**
     * タイプ：標準入力
     * @const unsigned int
     */
    const unsigned int STDIN = 5;

    /**
     * タイプ：標準出力
     * @const unsigned int
     */
    const unsigned int STDOUT = 6;

    /**
     * タイプ：エラー
     * @const unsigned int
     */
    const unsigned int STDERR = 7;

    /**
     * タイプ：データ
     * @const unsigned char
     */
    const int DATA                 = 8;
    const int GET_VALUES           = 9;
    const int GET_VALUES_RESULT    = 10;
    const int UNKNOWN_TYPE         = 11;
    const int MAXTYPE              = 11;

    const int RESPONDER            = 1;
    const int AUTHORIZER           = 2;
    const int FILTER               = 3;

    /**
     * フラグ：コンプリート
     * @const unsigned char
     */
    const unsigned char REQUEST_COMPLETE = 0;
    const unsigned char CANT_MPX_CONN    = 1;
    const unsigned char OVERLOADED       = 2;
    const unsigned char UNKNOWN_ROLE     = 3;

    /**
     * メッセージ
     * @const char*
     */
    const char MAX_CONNS[]         = "MAX_CONNS";
    const char MAX_REQS[]          = "MAX_REQS";
    const char MPXS_CONNS[]        = "MPXS_CONNS";

    /**
     * ヘッダーサイズ
     * @const integer
     */
    const int HEADER_LEN = 8;

    /**
     * 読込サイズ
     * @const integer
     */
    const int READ_LEN = 1024;

    /**
     * 最大値
     * @const integer
     */
    const int MAX_LENGTH = 0xffff;

    /**
     * FCGIプロトコル開始リクエスト本文クラス
     *
     * @package     FCGIClient
     * @author      Yujiro Takahashi <yujiro3@gmail.com>
     */
    class client {
    public:
        /**
         * FCGI用引数
         * @var array_t
         */
        array_t params;

        /**
         * レコード
         * @var string
         */
        std::string record;

        /**
         * Response
         * @var string
         */
        std::string response;

        /**
         * ヘッダー
         * @var struct
         */
        header_t header;

        /**
         * ソケット
         * @var integer
         */
        int sock;

        /**
         * 接続先アドレス
         * @var sockaddr
         */
        struct sockaddr *address;

        /**
         * 接続先アドレスサイズ
         * @var integer
         */
        int addrlen;

    public:
        /**
         * コンストラクタ
         *
         * @access public
         * @param string listen
         * @param int port
         */
        client(std::string listen, int port);

        /**
         * コンストラクタ
         *
         * @access public
         * @param string listen
         */
        client(std::string listen);

        /**
         * デストラクタ
         *
         * @access public
         */
        ~client();

        /**
         * Execute a request to the FastCGI application
         *
         * @access public
         * @param String stdin Content
         * @return String
         */
        std::string request(std::string stdin);
    
    private:
        /**
         * 送信パケット作成
         *
         * @access private
         * @param  int type
         * @param  string content
         * @param  int requestId
         */
        void _buildPacket(int type, std::string content, int requestId);

        /**
         * Build an FastCGI Name value pair
         *
         * @access private
         * @param string name Name
         * @param string value Value
         * @return string FastCGI Name value pair
         */
        std::string _buildNvpair(std::string name, std::string value);

        /**
         * ヘッダーパケットの読込
         *
         * @access private
         * @return void
         */
        void _readPacketHeader();

        /**
         * Responseの受取
         *
         * @access private
         * @return void
         */
        void _readPacket();
    
        /**
         * Create a connection to the FastCGI application
         *
         * @access private
         * @return boolean
         */
        bool _connect();
    };
}

#endif // #ifndef __FCGI_CLIENT_H__
