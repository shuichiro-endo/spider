/*
 * Title:  spider digest.hpp (Linux)
 * Author: Shuichiro Endo
 */

#ifndef DIGEST_H_
#define DIGEST_H_

#include "spider.hpp"

#define ENTITY_BODY_SIZE 8192

#pragma pack(push, 1)
struct digest_parameters
{
    char algorithm[20];         // MD5 or MD5-sess or SHA-256 or SHA-256-sess or SHA-512-256 or SHA-512-256-sess
    char username[256];         // forward proxy username
    char realm[100];
    char password[256];         // forward proxy password
    char a1[1000];              // username:realm:password or H(username:realm:password):nonce:cnonce
    char a1_hash[150];          // H(a1)
    char nonce[200];
    char nonce_prime[200];
    char nc[10];                // 00000001
    char cnonce[200];
    char cnonce_prime[200];
    char qop[10];               // auth or auth-int
    char entity_body[ENTITY_BODY_SIZE];
    char entity_body_hash[150];
    char stale[10];             // true or false
    char method[10];            // CONNECT
    char uri[500];
    char a2[1000];              // method:uri or method:uri:H(entity_body)
    char a2_hash[150];          // H(a2)
    char response[1000];        // H(A1):nonce:nc:cnonce:qop:H(A2)
    char response_hash[150];    // H(H(A1):nonce:nc:cnonce:qop:H(A2))
};
#pragma pack(pop)


namespace spider
{
    class Digest
    {
    private:
        std::string username;
        std::string password;
        std::string ip;
        std::string port;
        struct digest_parameters *digest_param = NULL;
        int length = 0;

    public:

    private:
        int encode_base64(const unsigned char *input,
                          int32_t input_length,
                          unsigned char *output,
                          int32_t output_size);

        int get_md5_hash(const unsigned char *input,
                         int input_length,
                         unsigned char *output,
                         int output_size);

        int get_sha_256_hash(const unsigned char *input,
                             int input_length,
                             unsigned char *output,
                             int output_size);

        int get_sha_512_256_hash(const unsigned char *input,
                                 int input_length,
                                 unsigned char *output,
                                 int output_size);

        int get_http_header(const char *input,
                            const char *key,
                            char *output,
                            int output_size);

        int get_digest_values(char *input);

    public:
        Digest(std::string username,
               std::string password,
               std::string ip,
               std::string port);

        ~Digest();

        void set_username(std::string username);
        std::string get_username();

        void set_password(std::string password);
        std::string get_password();

        void set_ip(std::string ip);
        std::string get_ip();

        void set_port(std::string port);
        std::string get_port();

        void set_digest_param(struct digest_parameters *digest_param);
        struct digest_parameters *get_digest_param();

        int get_digest_response(char *http_header_data);
    };
}

#endif /* DIGEST_H_ */

