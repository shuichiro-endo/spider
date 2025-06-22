/*
 * Title:  spider ntlmv2.hpp (Linux)
 * Author: Shuichiro Endo
 */

#ifndef NTLMV2_H_
#define NTLMV2_H_

#include "spider.hpp"
#include "ntlm.hpp"

namespace spider
{

    class Ntlmv2
    {
    private:
        std::string username;
        std::string password;
        std::string nthash_hexstring;
        std::string domainname;
        std::string workstationname;

    public:

    private:
        void print_bytes(unsigned char *input,
                         int input_length);

        int encode_base64(const unsigned char *input,
                          int32_t input_length,
                          unsigned char *output,
                          int32_t output_size);

        int decode_base64(const unsigned char *input,
                          int32_t input_length,
                          unsigned char *output,
                          int32_t output_size);

        int get_md4_hash(const unsigned char *input,
                         int input_length,
                         unsigned char *output,
                         int output_size);

        int get_hmac_md5(const unsigned char *input,
                         int input_length,
                         const unsigned char *key,
                         int key_length,
                         unsigned char *output,
                         int output_size);

        int get_upper_string(const char *input,
                             int input_length,
                             char *output);

        int get_number_of_bytes_of_utf16_string(char *input);

        int convert_utf8_to_utf16(const char *input,
                                  char *output,
                                  size_t output_size);

        int get_av_pair_value(struct challenge_message *challenge_message,
                              uint16_t av_id,
                              unsigned char *data,
                              int data_size);

        char hex_char_to_int(char c);

        int hexstring_to_array(const char *hexstring,
                               int hexstring_length,
                               unsigned char *output,
                               int output_size);

        int ntowfv2(const char *user,
                    const char *password,
                    const char *nthash,
                    const char *userdom,
                    unsigned char *output,
                    int output_size);

        int lmowfv2(const char *user,
                    const char *password,
                    const char *nthash,
                    const char *userdom,
                    unsigned char *output,
                    int output_size);

        int get_http_header(const char *input,
                            const char *key,
                            char *output,
                            int output_size);

    public:
        Ntlmv2(std::string username,
               std::string password,
               std::string nthash_hexstring,
               std::string domainname,
               std::string workstationname);
        ~Ntlmv2();

        void set_username(std::string username);
        std::string get_username();

        void set_password(std::string password);
        std::string get_password();

        void set_nthash_hexstring(std::string nthash_hexstring);
        std::string get_nthash_hexstring();

        void set_domainname(std::string domainname);
        std::string get_domainname();

        void set_workstationname(std::string workstationname);
        std::string get_workstationname();

        int generate_base64_negotiate_message(char *ntlm_b64,
                                              int32_t ntlm_b64_size);

        int generate_response_ntlmv2(struct challenge_message *challenge_message,
                                     struct authenticate_message *authenticate_message);

        int generate_base64_authenticate_message(char *http_header_data,
                                                 char *ntlm_b64,
                                                 int32_t ntlm_b64_size);

    };
}

#endif /* NTLMV2_H_ */

