/*
 * Title:  spider basic.cpp (Windows)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "basic.hpp"


namespace spider
{
    Basic::Basic(std::string username,
                 std::string password)
    {
        this->username = username;
        this->password = password;
    }

    Basic::~Basic()
    {

    }

    int Basic::encode_base64(const unsigned char *input,
                             int32_t input_length,
                             unsigned char *output,
                             int32_t output_size)
    {
        BOOL ret_b = false;
        DWORD required_size = 0;
        int32_t output_length = 0;


        ret_b = CryptBinaryToStringA(input,
                                     input_length,
                                     CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
                                     NULL,
                                     &required_size);
        if(ret_b == false)
        {
#ifdef DEBUGPRINT
            std::printf("[-] encode_base64 CryptBinaryToStringA error\n");
#endif
            goto error;
        }

        if(required_size > output_size - 1)
        {
#ifdef DEBUGPRINT
            std::printf("[-] encode_base64 required_size error: %d\n",
                        required_size);
#endif
            goto error;
        }

        ret_b = CryptBinaryToStringA(input,
                                     input_length,
                                     CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
                                     (char *)output,
                                     &required_size);
        if(ret_b == false)
        {
#ifdef DEBUGPRINT
            std::printf("[-] encode_base64 CryptBinaryToStringA error\n");
#endif
            goto error;
        }

        output_length = required_size;
        return output_length;

error:
        return -1;
    }

    int Basic::get_base64_credential(char *output,
                                     int32_t output_size)
    {
        int ret = 0;
        int32_t proxy_credential_length = 0;

        char *proxy_credential = (char *)calloc(NODE_BUFFER_SIZE,
                                                sizeof(char));


        proxy_credential_length = snprintf(proxy_credential,
                                           NODE_BUFFER_SIZE,
                                           "%s:%s",
                                           username.c_str(),
                                           password.c_str());
        if(proxy_credential_length > NODE_BUFFER_SIZE / 2)
        {
#ifdef DEBUGPRINT
            std::printf("[-] get_base64_credential proxy_credential_length error\n");
#endif
            goto error;
        }

        ret = encode_base64((const unsigned char *)proxy_credential,
                            proxy_credential_length,
                            (unsigned char *)output,
                            output_size);
        if(ret < 0)
        {
#ifdef DEBUGPRINT
            std::printf("[-] get_base64_credential encode_base64 error\n");
#endif
            goto error;
        }

        free(proxy_credential);
        return 0;

error:
        free(proxy_credential);
        return -1;
    }

}
