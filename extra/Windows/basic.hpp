/*
 * Title:  spider basic.hpp (Windows)
 * Author: Shuichiro Endo
 */

#ifndef BASIC_H_
#define BASIC_H_

#include "spider.hpp"


namespace spider
{
    class Basic
    {
    private:
        std::string username;
        std::string password;

    public:

    private:
        int encode_base64(const unsigned char *input,
                          int32_t input_length,
                          unsigned char *output,
                          int32_t output_size);

    public:
        Basic(std::string username,
              std::string password);

        ~Basic();

        void set_username(std::string username);
        std::string get_username();

        void set_password(std::string password);
        std::string get_password();

        int get_base64_credential(char *output,
                                  int32_t output_size);
    };
}

#endif /* BASIC_H_ */
