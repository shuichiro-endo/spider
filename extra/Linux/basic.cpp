/*
 * Title:  spider basic.cpp (Linux)
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
        BIO *b64 = BIO_new(BIO_f_base64());
        BIO_set_flags(b64,
                      BIO_FLAGS_BASE64_NO_NL);
        BIO *mem = BIO_new(BIO_s_mem());
        char *ptr = NULL;
        long len = 0;
        int32_t output_length = 0;
        int ret = 0;
        BIO *bio = BIO_push(b64,
                            mem);

        ret = BIO_write(bio,
                        input,
                        input_length);
        if(ret <= 0)
        {
#ifdef _DEBUG
            std::printf("[-] encode_base64 BIO_write error\n");
#endif
            goto error;
        }

        ret = BIO_flush(bio);
        if(ret <= 0)
{
#ifdef _DEBUG
            std::printf("[-] encode_base64 BIO_flush error\n");
#endif
            goto error;
        }

        len = BIO_get_mem_data(mem,
                               &ptr);
        if(len <= 0)
        {
#ifdef _DEBUG
            std::printf("[-] encode_base64 BIO_get_mem_data error\n");
#endif
            goto error;
        }

        if(len > output_size)
        {
#ifdef _DEBUG
            std::printf("[-] encode_base64 output_size error\n");
#endif
            goto error;
        }

        std::memcpy(output,
                    ptr,
                    (int)len);
        output_length = strlen((const char *)output);

        BIO_free_all(bio);
        return output_length;

error:
        BIO_free_all(bio);
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
#ifdef _DEBUG
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
#ifdef _DEBUG
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
