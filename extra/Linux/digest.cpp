/*
 * Title:  spider digest.cpp (Linux)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "digest.hpp"


namespace spider
{
    Digest::Digest(std::string username,
                   std::string password,
                   std::string ip,
                   std::string port)
    {
        this->username = username;
        this->password = password;
        this->ip = ip;
        this->port = port;
        digest_param = (struct digest_parameters *)calloc(1,
                                                          sizeof(struct digest_parameters));
        std::memcpy(&(digest_param->username),
                    username.c_str(),
                    strlen(username.c_str()));

        std::memcpy(&(digest_param->password),
                    password.c_str(),
                    strlen(password.c_str()));

        std::memcpy(&(digest_param->nc),
                    "00000001",
                    strlen("00000001"));

        std::memcpy(&(digest_param->method),
                    "CONNECT",
                    strlen("CONNECT"));

        this->length = snprintf(digest_param->uri,
                                500,
                                "%s:%s",
                                ip.c_str(),
                                port.c_str());
    }

    Digest::~Digest()
    {
        if(digest_param != NULL)
        {
            free(digest_param);
        }
    }

    void Digest::set_username(std::string username)
    {
        this->username = username;
    }

    std::string Digest::get_username()
    {
        return username;
    }

    void Digest::set_password(std::string password)
    {
        this->password = password;
    }

    std::string Digest::get_password()
    {
        return password;
    }

    void Digest::set_ip(std::string ip)
    {
        this->ip = ip;
    }

    std::string Digest::get_ip()
    {
        return ip;
    }

    void Digest::set_port(std::string port)
    {
        this->port = port;
    }

    std::string Digest::get_port()
    {
        return port;
    }

    void Digest::set_digest_param(struct digest_parameters *digest_param)
    {
        this->digest_param = digest_param;
    }

    struct digest_parameters *Digest::get_digest_param()
    {
        return digest_param;
    }

    int Digest::encode_base64(const unsigned char *input,
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

    int Digest::get_md5_hash(const unsigned char *input,
                             int input_length,
                             unsigned char *output,
                             int output_size)
    {
        EVP_MD_CTX *ctx = NULL;
        int ret = 0;
        unsigned char *digest = NULL;
        unsigned int length = 0;

        ctx = EVP_MD_CTX_new();
        if(ctx == NULL)
        {
#ifdef _DEBUG
            std::printf("[-] get_md5_hash EVP_MD_CTX_new error\n");
#endif
            return -1;
        }

        ret = EVP_DigestInit_ex(ctx,
                                EVP_md5(),
                                NULL);
        if(ret != 1)
        {
#ifdef _DEBUG
            std::printf("[-] get_md5_hash EVP_DigestInit_ex error\n");
#endif
            EVP_MD_CTX_free(ctx);
            return -1;
        }

        ret = EVP_DigestUpdate(ctx,
                               input,
                               input_length);
        if(ret != 1)
        {
#ifdef _DEBUG
            std::printf("[-] get_md5_hash EVP_DigestUpdate error\n");
#endif
            EVP_MD_CTX_free(ctx);
            return -1;
        }

        if(EVP_MD_size(EVP_md5()) > output_size)
        {
#ifdef _DEBUG
            std::printf("[-] get_md5_hash output_size error\n");
#endif
            EVP_MD_CTX_free(ctx);
            return -1;
        }

        digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_md5()));
        if(digest == NULL)
        {
#ifdef _DEBUG
            std::printf("[-] get_md5_hash OPENSSL_malloc error\n");
#endif
            EVP_MD_CTX_free(ctx);
            return -1;
        }

        ret = EVP_DigestFinal_ex(ctx,
                                 digest,
                                 &length);
        if(ret != 1)
        {
#ifdef _DEBUG
            std::printf("[-] get_md5_hash EVP_DigestFinal_ex error\n");
#endif
            OPENSSL_free(digest);
            EVP_MD_CTX_free(ctx);
            return -1;
        }

        for(int i=0; i*8<length; i++)
        {
            ret = snprintf((char *)output+i*16,
                           17,
                           "%02x%02x%02x%02x%02x%02x%02x%02x\n",
                           digest[i*8+0],
                           digest[i*8+1],
                           digest[i*8+2],
                           digest[i*8+3],
                           digest[i*8+4],
                           digest[i*8+5],
                           digest[i*8+6],
                           digest[i*8+7]);
        }

        OPENSSL_free(digest);
        EVP_MD_CTX_free(ctx);

        return length * 2;
    }

    int Digest::get_sha_256_hash(const unsigned char *input,
                                 int input_length,
                                 unsigned char *output,
                                 int output_size)
    {
        EVP_MD_CTX *ctx = NULL;
        int ret = 0;
        unsigned char *digest = NULL;
        unsigned int length = 0;

        ctx = EVP_MD_CTX_new();
        if(ctx == NULL)
        {
#ifdef _DEBUG
            std::printf("[-] get_sha_256_hash EVP_MD_CTX_new error\n");
#endif
            return -1;
        }

        ret = EVP_DigestInit_ex(ctx,
                                EVP_sha256(),
                                NULL);
        if(ret != 1)
        {
#ifdef _DEBUG
            std::printf("[-] get_sha_256_hash EVP_DigestInit_ex error\n");
#endif
            EVP_MD_CTX_free(ctx);
            return -1;
        }

        ret = EVP_DigestUpdate(ctx,
                               input,
                               input_length);
        if(ret != 1)
        {
#ifdef _DEBUG
            std::printf("[-] get_sha_256_hash EVP_DigestUpdate error\n");
#endif
            EVP_MD_CTX_free(ctx);
            return -1;
        }

        if(EVP_MD_size(EVP_sha256()) > output_size)
        {
#ifdef _DEBUG
            std::printf("[-] get_sha_256_hash output_size error\n");
#endif
            EVP_MD_CTX_free(ctx);
            return -1;
        }

        digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha256()));
        if(digest == NULL)
        {
#ifdef _DEBUG
            std::printf("[-] get_sha_256_hash OPENSSL_malloc error\n");
#endif
            EVP_MD_CTX_free(ctx);
            return -1;
        }

        ret = EVP_DigestFinal_ex(ctx,
                                 digest,
                                 &length);
        if(ret != 1)
        {
#ifdef _DEBUG
            std::printf("[-] get_sha_256_hash EVP_DigestFinal_ex error\n");
#endif
            OPENSSL_free(digest);
            EVP_MD_CTX_free(ctx);
            return -1;
        }

        for(int i=0; i*8<length; i++)
        {
            ret = snprintf((char *)output+i*16,
                           17,
                           "%02x%02x%02x%02x%02x%02x%02x%02x\n",
                           digest[i*8+0],
                           digest[i*8+1],
                           digest[i*8+2],
                           digest[i*8+3],
                           digest[i*8+4],
                           digest[i*8+5],
                           digest[i*8+6],
                           digest[i*8+7]);
        }

        OPENSSL_free(digest);
        EVP_MD_CTX_free(ctx);

        return length * 2;
    }

    int Digest::get_sha_512_256_hash(const unsigned char *input,
                                     int input_length,
                                     unsigned char *output,
                                     int output_size)
    {
        EVP_MD_CTX *ctx = NULL;
        int ret = 0;
        unsigned char *digest = NULL;
        unsigned int length = 0;

        ctx = EVP_MD_CTX_new();
        if(ctx == NULL)
        {
#ifdef _DEBUG
            std::printf("[-] get_sha_512_256_hash EVP_MD_CTX_new error\n");
#endif
            return -1;
        }

        ret = EVP_DigestInit_ex(ctx,
                                EVP_sha512_256(),
                                NULL);
        if(ret != 1)
        {
#ifdef _DEBUG
            std::printf("[-] get_sha_512_256_hash EVP_DigestInit_ex error\n");
#endif
            EVP_MD_CTX_free(ctx);
            return -1;
        }

        ret = EVP_DigestUpdate(ctx,
                               input,
                               input_length);
        if(ret != 1)
        {
#ifdef _DEBUG
            std::printf("[-] get_sha_512_256_hash EVP_DigestUpdate error\n");
#endif
            EVP_MD_CTX_free(ctx);
            return -1;
        }

        if(EVP_MD_size(EVP_sha512_256()) > output_size)
        {
#ifdef _DEBUG
            std::printf("[-] get_sha_512_256_hash output_size error\n");
#endif
            EVP_MD_CTX_free(ctx);
            return -1;
        }

        digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha512_256()));
        if(digest == NULL)
        {
#ifdef _DEBUG
            std::printf("[-] get_sha_512_256_hash OPENSSL_malloc error\n");
#endif
            EVP_MD_CTX_free(ctx);
            return -1;
        }

        ret = EVP_DigestFinal_ex(ctx,
                                 digest,
                                 &length);
        if(ret != 1)
        {
#ifdef _DEBUG
            std::printf("[-] get_sha_512_256_hash EVP_DigestFinal_ex error\n");
#endif
            OPENSSL_free(digest);
            EVP_MD_CTX_free(ctx);
            return -1;
        }

        for(int i=0; i*8<length; i++)
        {
            ret = snprintf((char *)output+i*16,
                           17,
                           "%02x%02x%02x%02x%02x%02x%02x%02x\n",
                           digest[i*8+0],
                           digest[i*8+1],
                           digest[i*8+2],
                           digest[i*8+3],
                           digest[i*8+4],
                           digest[i*8+5],
                           digest[i*8+6],
                           digest[i*8+7]);
        }

        OPENSSL_free(digest);
        EVP_MD_CTX_free(ctx);

        return length * 2;
    }

    int Digest::get_digest_values(char *input)
    {
        char *start = NULL;
        char *end = NULL;
        long d = 0;

        // realm
        start = strstr(input,
                       "realm=\"");
        if(start == NULL)
        {
#ifdef _DEBUG
            std::printf("[-] get_digest_values realm error\n");
#endif
            return -1;
        }

        start += strlen("realm=\"");
        end = strstr(start,
                     "\"");
        d = end - start;
        if(d <= 0 ||
           d >= 100)
        {
#ifdef _DEBUG
            printf("[-] get_digest_values realm error: %d\n",
                   d);
#endif
            return -1;
        }

        std::memcpy(&(digest_param->realm),
                    start,
                    d);

        // nonce
        start = strstr(input,
                       "nonce=\"");
        if(start == NULL)
        {
#ifdef _DEBUG
            std::printf("[-] get_digest_values nonce error\n");
#endif
            return -1;
        }

        start += strlen("nonce=\"");
        end = strstr(start,
                     "\"");
        d = end - start;
        if(d <= 0 ||
           d >= 200)
        {
#ifdef _DEBUG
            std::printf("[-] get_digest_values nonce error: %d\n",
                        d);
#endif
            return -1;
        }

        std::memcpy(&(digest_param->nonce),
                    start,
                    d);

        // nonce-prime
        start = strstr(input,
                       "nonce-prime=\"");
        if(start != NULL)
        {
            start += strlen("nonce-prime=\"");
            end = strstr(start,
                         "\"");
            d = end - start;
            if(d <= 0 ||
               d >= 200)
            {
#ifdef _DEBUG
                std::printf("[-] get_digest_values nonce-prime error: %d\n",
                            d);
#endif
                return -1;
            }

            std::memcpy(&(digest_param->nonce_prime),
                        start,
                        d);
        }

        // qop
        start = strstr(input,
                       "qop=\"");
        if(start == NULL)
        {
#ifdef _DEBUG
            std::printf("[-] get_digest_values qop error\n");
#endif
            return -1;
        }

        start += strlen("qop=\"");
        end = strstr(start,
                     "\"");
        d = end - start;
        if(d <= 0 ||
           d >= 10)
        {
#ifdef _DEBUG
            std::printf("[-] get_digest_values qop error: %d\n",
                        d);
#endif
            return -1;
        }

        if(!strncmp(start, "auth-int", strlen("auth-int")))
        {
            std::memcpy(&(digest_param->qop),
                        "auth-int",
                        strlen("auth-int"));
        }else
        {
            std::memcpy(&(digest_param->qop),
                        "auth",
                        strlen("auth"));
        }

        // algorithm
        start = strstr(input,
                       "algorithm=");
        if(start == NULL)
        {
            std::memcpy(&(digest_param->algorithm),
                        "MD5",
                        strlen("MD5"));
        }else
        {
            start += strlen("algorithm=");
            end = strstr(start,
                         " ");
            d = end - start;
            if(d < 0 ||
               d >= 100)
            {
#ifdef _DEBUG
                std::printf("[-] get_digest_values algorithm error: %d\n",
                            d);
#endif
                return -1;
            }

            std::memcpy(&(digest_param->algorithm),
                        start,
                        d);
        }

        // stale
        start = strstr(input,
                       "stale=");
        if(start == NULL)
        {
#ifdef _DEBUG
            std::printf("[-] get_digest_values stale error\n");
#endif
            return -1;
        }
        start += strlen("stale=");
        if(!strncmp(start, "false", strlen("false")))
        {
            std::memcpy(&(digest_param->stale),
                        "false",
                        strlen("false"));
        }else{
            std::memcpy(&(digest_param->stale),
                        "true",
                        strlen("true"));
        }

#ifdef _DEBUG
//        std::printf("[+] get_digest_values realm:%s nonce:%s, nonce-prime:%s qop:%s, algorithm:%s stale:%s\n", digest_param->realm, digest_param->nonce, digest_param->nonce_prime, digest_param->qop, digest_param->algorithm, digest_param->stale);
#endif

        return 0;
    }

    int Digest::get_http_header(const char *input,
                                const char *key,
                                char *output,
                                int output_size)
    {
        char *start = NULL;
        char *end = NULL;
        long d = 0;
        int length = 0;

        start = strstr((char *)input,
                       key);
        if(start == NULL)
        {
#ifdef _DEBUG
            std::printf("[-] get_http_header start is NULL\n");
#endif
            return -1;
        }

        end = strstr(start,
                     "\r\n");
        if(end == NULL)
        {
#ifdef _DEBUG
            std::printf("[-] get_http_header end is NULL\n");
#endif
            return -1;
        }

        d = end - start;
        if(d <= 0 ||
           d >= output_size)
        {
#ifdef _DEBUG
            std::printf("[-] get_http_header error: %d\n",
                        d);
#endif
            return -1;
        }

        std::memset(output,
                    0,
                    output_size);

        std::memcpy(output,
                    start,
                    d);

        length = strlen(output);

        return length;
    }

    int Digest::get_digest_response(char *http_header_data)
    {
        int ret = 0;
        int length = 0;
        char *pos = NULL;

        const char *digest_http_header_key = "Proxy-Authenticate:";

        char *digest_http_header_data = (char *)calloc(NODE_BUFFER_SIZE,
                                                       sizeof(char));
        unsigned char *tmp1 = (unsigned char *)calloc(17, sizeof(char));
        unsigned char *tmp2 = (unsigned char *)calloc(33, sizeof(char));
        unsigned char *tmp3 = (unsigned char *)calloc(1000, sizeof(char));
        unsigned char *tmp4 = (unsigned char *)calloc(150, sizeof(char));


        ret = get_http_header((const char *)http_header_data,
                              (const char *)digest_http_header_key,
                              (char *)digest_http_header_data,
                              NODE_BUFFER_SIZE);
        if(ret < 0)
        {
#ifdef _DEBUG
            std::printf("[-] get_digest_response get_http_header error\n");
#endif
            goto error;
        }

        ret = get_digest_values(digest_http_header_data);
        if(ret < 0)
        {
#ifdef _DEBUG
            std::printf("[-] get_digest_response get_digest_values error\n");
#endif
            goto error;
        }

        if(!strncmp(digest_param->qop, "auth-int", strlen("auth-int")))
        {
            pos = strstr(http_header_data,
                         "\r\n\r\n");

            length = snprintf(digest_param->entity_body,
                              ENTITY_BODY_SIZE,
                              "%s",
                              pos+4);
        }


        // cnonce
        ret = RAND_bytes((unsigned char *)tmp1,
                         16);
        if(ret != 1)
        {
#ifdef _DEBUG
            std::printf("[-] get_digest_response RAND_bytes error: %s\n",
                        ERR_error_string(ERR_peek_last_error(), NULL));
#endif
            goto error;
        }

        for(int i=0; i*8<16; i++)
        {
            ret = snprintf((char *)tmp2+i*16,
                           17,
                           "%02x%02x%02x%02x%02x%02x%02x%02x\n",
                           tmp1[i*8+0],
                           tmp1[i*8+1],
                           tmp1[i*8+2],
                           tmp1[i*8+3],
                           tmp1[i*8+4],
                           tmp1[i*8+5],
                           tmp1[i*8+6],
                           tmp1[i*8+7]);
        }

        ret = encode_base64(tmp2,
                            32,
                            (unsigned char *)digest_param->cnonce,
                            200);

        // cnonce-prime
        if(digest_param->nonce_prime != NULL)
        {
            std::memset(tmp1,
                        0,
                        17);

            std::memset(tmp2,
                        0,
                        33);

            ret = RAND_bytes((unsigned char *)tmp1,
                             16);
            if(ret != 1)
            {
#ifdef _DEBUG
                std::printf("[-] get_digest_response RAND_bytes error: %s\n",
                            ERR_error_string(ERR_peek_last_error(), NULL));
#endif
                goto error;
            }

            for(int i=0; i*8<16; i++)
            {
                ret = snprintf((char *)tmp2+i*16,
                               17,
                               "%02x%02x%02x%02x%02x%02x%02x%02x\n",
                               tmp1[i*8+0],
                               tmp1[i*8+1],
                               tmp1[i*8+2],
                               tmp1[i*8+3],
                               tmp1[i*8+4],
                               tmp1[i*8+5],
                               tmp1[i*8+6],
                               tmp1[i*8+7]);
            }

            ret = encode_base64(tmp2,
                                32,
                                (unsigned char *)digest_param->cnonce_prime,
                                200);
        }


        if(!strncmp(digest_param->algorithm, "MD5-sess", strlen("MD5-sess")))
        {
            // A1 MD5(username:realm:password):nonce-prime:cnonce-prime
            length = strlen(digest_param->username) + strlen(digest_param->realm) + strlen(digest_param->password) + 2;  // 2 colon
            ret = snprintf((char *)tmp3,
                           length+1,
                           "%s:%s:%s",
                           digest_param->username,
                           digest_param->realm,
                           digest_param->password);
            ret = get_md5_hash((const unsigned char *)tmp3,
                               length,
                               tmp4,
                               150);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] get_digest_response A1-1 get_md5_hash error\n");
#endif
                goto error;
            }

            length = strlen((char *)tmp4) + strlen(digest_param->nonce_prime) + strlen(digest_param->cnonce_prime) + 2;   // 2 colon
            ret = snprintf(digest_param->a1,
                           length+1,
                           "%s:%s:%s",
                           tmp4,
                           digest_param->nonce_prime,
                           digest_param->cnonce_prime);
            ret = get_md5_hash((const unsigned char *)&(digest_param->a1),
                               length,
                               (unsigned char *)&(digest_param->a1_hash),
                               150);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] get_digest_response A1-2 get_md5_hash error\n");
#endif
                goto error;
            }

            if(!strncmp(digest_param->qop, "auth-int", strlen("auth-int")))    // auth-int
            {
                // A2 method:uri:MD5(entity-body)
                length = strlen(digest_param->entity_body);
                ret = get_md5_hash((const unsigned char *)&(digest_param->entity_body),
                                   length,
                                   (unsigned char *)&(digest_param->entity_body_hash),
                                   150);
                if(ret == -1)
                {
#ifdef _DEBUG
                    std::printf("[-] get_digest_response A2-1 get_md5_hash error\n");
#endif
                    goto error;
                }

                length = strlen(digest_param->method) + strlen(digest_param->uri) + strlen(digest_param->entity_body_hash) + 2;  // 2 colon
                ret = snprintf(digest_param->a2,
                               length+1,
                               "%s:%s:%s",
                               digest_param->method,
                               digest_param->uri,
                               digest_param->entity_body_hash);
                ret = get_md5_hash((const unsigned char *)&(digest_param->a2),
                                   length,
                                   (unsigned char *)&(digest_param->a2_hash),
                                   150);
                if(ret == -1)
                {
#ifdef _DEBUG
                    std::printf("[-] get_digest_response A2-2 get_md5_hash error\n");
#endif
                    goto error;
                }
            }else   // auth
            {
                // A2 method:uri
                length = strlen(digest_param->method) + strlen(digest_param->uri) + 1;    // 1 colon
                ret = snprintf(digest_param->a2,
                               length+1,
                               "%s:%s",
                               digest_param->method,
                               digest_param->uri);
                ret = get_md5_hash((const unsigned char *)&(digest_param->a2),
                                   length,
                                   (unsigned char *)&(digest_param->a2_hash),
                                   150);
                if(ret == -1)
                {
#ifdef _DEBUG
                    std::printf("[-] get_digest_response A2 get_md5_hash error\n");
#endif
                    goto error;
                }
            }

            // response MD5(A1):nonce:nc:cnonce:qop:MD5(A2)
            length = strlen(digest_param->a1_hash) + strlen(digest_param->nonce) + strlen(digest_param->nc) + strlen(digest_param->cnonce) + strlen(digest_param->qop) + strlen(digest_param->a2_hash) + 5;   // 5 colon
            ret = snprintf(digest_param->response,
                           length+1,
                           "%s:%s:%s:%s:%s:%s",
                           digest_param->a1_hash,
                           digest_param->nonce,
                           digest_param->nc,
                           digest_param->cnonce,
                           digest_param->qop,
                           digest_param->a2_hash);
            ret = get_md5_hash((const unsigned char *)&(digest_param->response),
                               length,
                               (unsigned char *)&(digest_param->response_hash),
                               150);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] get_digest_response response get_md5_hash error\n");
#endif
                goto error;
            }
        }else if(!strncmp(digest_param->algorithm, "MD5", strlen("MD5")))
        {
            // A1 username:realm:password
            length = strlen(digest_param->username) + strlen(digest_param->realm) + strlen(digest_param->password) + 2;  // 2 colon
            ret = snprintf(digest_param->a1,
                           length+1,
                           "%s:%s:%s",
                           digest_param->username,
                           digest_param->realm,
                           digest_param->password);
            ret = get_md5_hash((const unsigned char *)&(digest_param->a1),
                               length,
                               (unsigned char *)&(digest_param->a1_hash),
                               150);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] get_digest_response A1 get_md5_hash error\n");
#endif
                goto error;
            }

            if(!strncmp(digest_param->qop, "auth-int", strlen("auth-int")))    // auth-int
            {
                // A2 method:uri:MD5(entity-body)
                length = strlen(digest_param->entity_body);
                ret = get_md5_hash((const unsigned char *)&(digest_param->entity_body),
                                   length,
                                   (unsigned char *)&(digest_param->entity_body_hash),
                                   150);
                if(ret == -1)
                {
#ifdef _DEBUG
                    std::printf("[-] get_digest_response A2-1 get_md5_hash error\n");
#endif
                    goto error;
                }

                length = strlen(digest_param->method) + strlen(digest_param->uri) + strlen(digest_param->entity_body_hash) + 2;  // 2 colon
                ret = snprintf(digest_param->a2,
                               length+1,
                               "%s:%s:%s",
                               digest_param->method,
                               digest_param->uri,
                               digest_param->entity_body_hash);
                ret = get_md5_hash((const unsigned char *)&(digest_param->a2),
                                   length,
                                   (unsigned char *)&(digest_param->a2_hash),
                                   150);
                if(ret == -1)
                {
#ifdef _DEBUG
                    std::printf("[-] get_digest_response A2-2 get_md5_hash error\n");
#endif
                    goto error;
                }
            }else   // auth
            {
                // A2 method:uri
                length = strlen(digest_param->method) + strlen(digest_param->uri) + 1;    // 1 colon
                ret = snprintf(digest_param->a2,
                               length+1,
                               "%s:%s",
                               digest_param->method,
                               digest_param->uri);
                ret = get_md5_hash((const unsigned char *)&(digest_param->a2),
                                   length,
                                   (unsigned char *)&(digest_param->a2_hash),
                                   150);
                if(ret == -1)
                {
#ifdef _DEBUG
                    std::printf("[-] get_digest_response A2 get_md5_hash error\n");
#endif
                    goto error;
                }
            }

            // response MD5(A1):nonce:nc:cnonce:qop:MD5(A2)
            length = strlen(digest_param->a1_hash) + strlen(digest_param->nonce) + strlen(digest_param->nc) + strlen(digest_param->cnonce) + strlen(digest_param->qop) + strlen(digest_param->a2_hash) + 5;   // 5 colon
            ret = snprintf(digest_param->response,
                           length+1,
                           "%s:%s:%s:%s:%s:%s",
                           digest_param->a1_hash,
                           digest_param->nonce,
                           digest_param->nc,
                           digest_param->cnonce,
                           digest_param->qop,
                           digest_param->a2_hash);
            ret = get_md5_hash((const unsigned char *)&(digest_param->response),
                               length,
                               (unsigned char *)&(digest_param->response_hash),
                               150);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] get_digest_response response get_md5_hash error\n");
#endif
                goto error;
            }
        }else if(!strncmp(digest_param->algorithm, "SHA-256-sess", strlen("SHA-256-sess")))
        {
            // A1 SHA-256(username:realm:password):nonce-prime:cnonce-prime
            length = strlen(digest_param->username) + strlen(digest_param->realm) + strlen(digest_param->password) + 2;  // 2 colon
            ret = snprintf((char *)tmp3,
                           length+1,
                           "%s:%s:%s",
                           digest_param->username,
                           digest_param->realm,
                           digest_param->password);
            ret = get_sha_256_hash((const unsigned char *)tmp3,
                                   length,
                                   tmp4,
                                   150);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] get_digest_response A1-1 get_sha_256_hash error\n");
#endif
                goto error;
            }

            length = strlen((char *)tmp4) + strlen(digest_param->nonce_prime) + strlen(digest_param->cnonce_prime) + 2;   // 2 colon
            ret = snprintf(digest_param->a1,
                           length+1,
                           "%s:%s:%s",
                           tmp4,
                           digest_param->nonce_prime,
                           digest_param->cnonce_prime);
            ret = get_sha_256_hash((const unsigned char *)&(digest_param->a1),
                                   length,
                                   (unsigned char *)&(digest_param->a1_hash),
                                   150);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] get_digest_response A1-2 get_sha_256_hash error\n");
#endif
                goto error;
            }

            if(!strncmp(digest_param->qop, "auth-int", strlen("auth-int")))    // auth-int
            {
                // A2 method:uri:SHA-256(entity-body)
                length = strlen(digest_param->entity_body);
                ret = get_sha_256_hash((const unsigned char *)&(digest_param->entity_body),
                                       length,
                                       (unsigned char *)&(digest_param->entity_body_hash),
                                       150);
                if(ret == -1)
                {
#ifdef _DEBUG
                    std::printf("[-] get_digest_response A2-1 get_sha_256_hash error\n");
#endif
                    goto error;
                }

                length = strlen(digest_param->method) + strlen(digest_param->uri) + strlen(digest_param->entity_body_hash) + 2;  // 2 colon
                ret = snprintf(digest_param->a2,
                               length+1,
                               "%s:%s:%s",
                               digest_param->method,
                               digest_param->uri,
                               digest_param->entity_body_hash);
                ret = get_sha_256_hash((const unsigned char *)&(digest_param->a2),
                                       length,
                                       (unsigned char *)&(digest_param->a2_hash),
                                       150);
                if(ret == -1)
                {
#ifdef _DEBUG
                    std::printf("[-] get_digest_response A2-2 get_sha_256_hash error\n");
#endif
                    goto error;
                }
            }else   // auth
            {
                // A2 method:uri
                length = strlen(digest_param->method) + strlen(digest_param->uri) + 1;    // 1 colon
                ret = snprintf(digest_param->a2,
                               length+1,
                               "%s:%s",
                               digest_param->method,
                               digest_param->uri);
                ret = get_sha_256_hash((const unsigned char *)&(digest_param->a2),
                                       length,
                                       (unsigned char *)&(digest_param->a2_hash),
                                       150);
                if(ret == -1)
                {
#ifdef _DEBUG
                    std::printf("[-] get_digest_response A2 get_sha_256_hash error\n");
#endif
                    goto error;
                }
            }

            // response SHA-256(A1):nonce:nc:cnonce:qop:SHA-256(A2)
            length = strlen(digest_param->a1_hash) + strlen(digest_param->nonce) + strlen(digest_param->nc) + strlen(digest_param->cnonce) + strlen(digest_param->qop) + strlen(digest_param->a2_hash) + 5;   // 5 colon
            ret = snprintf(digest_param->response,
                           length+1,
                           "%s:%s:%s:%s:%s:%s",
                           digest_param->a1_hash,
                           digest_param->nonce,
                           digest_param->nc,
                           digest_param->cnonce,
                           digest_param->qop,
                           digest_param->a2_hash);
            ret = get_sha_256_hash((const unsigned char *)&(digest_param->response),
                                   length,
                                   (unsigned char *)&(digest_param->response_hash),
                                   150);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] get_digest_response response get_sha_256_hash error\n");
#endif
                goto error;
            }

        }else if(!strncmp(digest_param->algorithm, "SHA-256", strlen("SHA-256")))
        {
            // A1 username:realm:password
            length = strlen(digest_param->username) + strlen(digest_param->realm) + strlen(digest_param->password) + 2;  // 2 colon
            ret = snprintf(digest_param->a1,
                           length+1,
                           "%s:%s:%s",
                           digest_param->username,
                           digest_param->realm,
                           digest_param->password);
            ret = get_sha_256_hash((const unsigned char *)&(digest_param->a1),
                                   length,
                                   (unsigned char *)&(digest_param->a1_hash),
                                   150);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] get_digest_response A1 get_sha_256_hash error\n");
#endif
                goto error;
            }

            if(!strncmp(digest_param->qop, "auth-int", strlen("auth-int")))    // auth-int
            {
                // A2 method:uri:SHA-256(entity-body)
                length = strlen(digest_param->entity_body);
                ret = get_sha_256_hash((const unsigned char *)&(digest_param->entity_body),
                                       length,
                                       (unsigned char *)&(digest_param->entity_body_hash),
                                       150);
                if(ret == -1)
                {
#ifdef _DEBUG
                    std::printf("[-] get_digest_response A2-1 get_sha_256_hash error\n");
#endif
                    goto error;
                }

                length = strlen(digest_param->method) + strlen(digest_param->uri) + strlen(digest_param->entity_body_hash) + 2;  // 2 colon
                ret = snprintf(digest_param->a2,
                               length+1,
                               "%s:%s:%s",
                               digest_param->method,
                               digest_param->uri,
                               digest_param->entity_body_hash);
                ret = get_sha_256_hash((const unsigned char *)&(digest_param->a2),
                                       length,
                                       (unsigned char *)&(digest_param->a2_hash),
                                       150);
                if(ret == -1)
                {
#ifdef _DEBUG
                    std::printf("[-] get_digest_response A2-2 get_sha_256_hash error\n");
#endif
                    goto error;
                }
            }else   // auth
            {
                // A2 method:uri
                length = strlen(digest_param->method) + strlen(digest_param->uri) + 1;    // 1 colon
                ret = snprintf(digest_param->a2,
                               length+1,
                               "%s:%s",
                               digest_param->method,
                               digest_param->uri);
                ret = get_sha_256_hash((const unsigned char *)&(digest_param->a2),
                                       length,
                                       (unsigned char *)&(digest_param->a2_hash),
                                       150);
                if(ret == -1)
                {
#ifdef _DEBUG
                    std::printf("[-] get_digest_response A2 get_sha_256_hash error\n");
#endif
                    goto error;
                }
            }

            // response SHA-256(A1):nonce:nc:cnonce:qop:SHA-256(A2)
            length = strlen(digest_param->a1_hash) + strlen(digest_param->nonce) + strlen(digest_param->nc) + strlen(digest_param->cnonce) + strlen(digest_param->qop) + strlen(digest_param->a2_hash) + 5;   // 5 colon
            ret = snprintf(digest_param->response,
                           length+1,
                           "%s:%s:%s:%s:%s:%s",
                           digest_param->a1_hash,
                           digest_param->nonce,
                           digest_param->nc,
                           digest_param->cnonce,
                           digest_param->qop,
                           digest_param->a2_hash);
            ret = get_sha_256_hash((const unsigned char *)&(digest_param->response),
                                   length,
                                   (unsigned char *)&(digest_param->response_hash),
                                   150);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] get_digest_response response get_sha_256_hash error\n");
#endif
                goto error;
            }
        }else if(!strncmp(digest_param->algorithm, "SHA-512-256-sess", strlen("SHA-512-256-sess")))
        {
            // A1 SHA-512-256(username:realm:password):nonce-prime:cnonce-prime
            length = strlen(digest_param->username) + strlen(digest_param->realm) + strlen(digest_param->password) + 2;  // 2 colon
            ret = snprintf((char *)tmp3,
                           length+1,
                           "%s:%s:%s",
                           digest_param->username,
                           digest_param->realm,
                           digest_param->password);
            ret = get_sha_512_256_hash((const unsigned char *)tmp3,
                                       length,
                                       tmp4,
                                       150);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] get_digest_response A1-1 get_sha_512_256_hash error\n");
#endif
                goto error;
            }

            length = strlen((char *)tmp4) + strlen(digest_param->nonce_prime) + strlen(digest_param->cnonce_prime) + 2;   // 2 colon
            ret = snprintf(digest_param->a1,
                           length+1,
                           "%s:%s:%s",
                           tmp4,
                           digest_param->nonce_prime,
                           digest_param->cnonce_prime);
            ret = get_sha_512_256_hash((const unsigned char *)&(digest_param->a1),
                                       length,
                                       (unsigned char *)&(digest_param->a1_hash),
                                       150);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] get_digest_response A1-2 get_sha_512_256_hash error\n");
#endif
                goto error;
            }

            if(!strncmp(digest_param->qop, "auth-int", strlen("auth-int"))){	// auth-int
                // A2 method:uri:SHA-512-256(entity-body)
                length = strlen(digest_param->entity_body);
                ret = get_sha_512_256_hash((const unsigned char *)&(digest_param->entity_body),
                                           length,
                                           (unsigned char *)&(digest_param->entity_body_hash),
                                           150);
                if(ret == -1)
                {
#ifdef _DEBUG
                    std::printf("[-] get_digest_response A2-1 get_sha_512_256_hash error\n");
#endif
                    goto error;
                }

                length = strlen(digest_param->method) + strlen(digest_param->uri) + strlen(digest_param->entity_body_hash) + 2;  // 2 colon
                ret = snprintf(digest_param->a2,
                               length+1,
                               "%s:%s:%s",
                               digest_param->method,
                               digest_param->uri,
                               digest_param->entity_body_hash);
                ret = get_sha_512_256_hash((const unsigned char *)&(digest_param->a2),
                                           length,
                                           (unsigned char *)&(digest_param->a2_hash),
                                           150);
                if(ret == -1)
                {
#ifdef _DEBUG
                    std::printf("[-] get_digest_response A2-2 get_sha_512_256_hash error\n");
#endif
                    goto error;
                }
            }else   // auth
            {
                // A2 method:uri
                length = strlen(digest_param->method) + strlen(digest_param->uri) + 1;    // 1 colon
                ret = snprintf(digest_param->a2,
                               length+1,
                               "%s:%s",
                               digest_param->method,
                               digest_param->uri);
                ret = get_sha_512_256_hash((const unsigned char *)&(digest_param->a2),
                                           length,
                                           (unsigned char *)&(digest_param->a2_hash),
                                           150);
                if(ret == -1)
                {
#ifdef _DEBUG
                    std::printf("[-] get_digest_response A2 get_sha_512_256_hash error\n");
#endif
                    goto error;
                }
            }

            // response SHA-512-256(A1):nonce:nc:cnonce:qop:SHA-512-256(A2)
            length = strlen(digest_param->a1_hash) + strlen(digest_param->nonce) + strlen(digest_param->nc) + strlen(digest_param->cnonce) + strlen(digest_param->qop) + strlen(digest_param->a2_hash) + 5;   // 5 colon
            ret = snprintf(digest_param->response,
                           length+1,
                           "%s:%s:%s:%s:%s:%s",
                           digest_param->a1_hash,
                           digest_param->nonce,
                           digest_param->nc,
                           digest_param->cnonce,
                           digest_param->qop,
                           digest_param->a2_hash);
            ret = get_sha_512_256_hash((const unsigned char *)&(digest_param->response),
                                       length,
                                       (unsigned char *)&(digest_param->response_hash),
                                       150);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] get_digest_response response get_sha_512_256_hash error\n");
#endif
                goto error;
            }
        }else if(!strncmp(digest_param->algorithm, "SHA-512-256", strlen("SHA-512-256")))
        {
            // A1 username:realm:password
            length = strlen(digest_param->username) + strlen(digest_param->realm) + strlen(digest_param->password) + 2;  // 2 colon
            ret = snprintf(digest_param->a1,
                           length+1,
                           "%s:%s:%s",
                           digest_param->username,
                           digest_param->realm,
                           digest_param->password);
            ret = get_sha_512_256_hash((const unsigned char *)&(digest_param->a1),
                                       length,
                                       (unsigned char *)&(digest_param->a1_hash),
                                       150);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] get_digest_response A1 get_sha_512_256_hash error\n");
#endif
                goto error;
            }

            if(!strncmp(digest_param->qop, "auth-int", strlen("auth-int")))    // auth-int
            {
                // A2 method:uri:SHA-512-256(entity-body)
                length = strlen(digest_param->entity_body);
                ret = get_sha_512_256_hash((const unsigned char *)&(digest_param->entity_body),
                                           length,
                                           (unsigned char *)&(digest_param->entity_body_hash),
                                           150);
                if(ret == -1)
                {
#ifdef _DEBUG
                    std::printf("[-] get_digest_response A2-1 get_sha_512_256_hash error\n");
#endif
                    goto error;
                }

                length = strlen(digest_param->method) + strlen(digest_param->uri) + strlen(digest_param->entity_body_hash) + 2;  // 2 colon
                ret = snprintf(digest_param->a2,
                               length+1,
                               "%s:%s:%s",
                               digest_param->method,
                               digest_param->uri,
                               digest_param->entity_body_hash);
                ret = get_sha_512_256_hash((const unsigned char *)&(digest_param->a2),
                                           length,
                                           (unsigned char *)&(digest_param->a2_hash),
                                           150);
                if(ret == -1)
                {
#ifdef _DEBUG
                    std::printf("[-] get_digest_response A2-2 get_sha_512_256_hash error\n");
#endif
                    goto error;
                }
            }else   // auth
            {
                // A2 method:uri
                length = strlen(digest_param->method) + strlen(digest_param->uri) + 1;    // 1 colon
                ret = snprintf(digest_param->a2,
                               length+1,
                               "%s:%s",
                               digest_param->method,
                               digest_param->uri);
                ret = get_sha_512_256_hash((const unsigned char *)&(digest_param->a2),
                                           length,
                                           (unsigned char *)&(digest_param->a2_hash),
                                           150);
                if(ret == -1)
                {
#ifdef _DEBUG
                    std::printf("[-] get_digest_response A2 get_sha_512_256_hash error\n");
#endif
                    goto error;
                }
            }

            // response SHA-512-256(A1):nonce:nc:cnonce:qop:SHA-512-256(A2)
            length = strlen(digest_param->a1_hash) + strlen(digest_param->nonce) + strlen(digest_param->nc) + strlen(digest_param->cnonce) + strlen(digest_param->qop) + strlen(digest_param->a2_hash) + 5;   // 5 colon
            ret = snprintf(digest_param->response,
                           length+1,
                           "%s:%s:%s:%s:%s:%s",
                           digest_param->a1_hash,
                           digest_param->nonce,
                           digest_param->nc,
                           digest_param->cnonce,
                           digest_param->qop,
                           digest_param->a2_hash);
            ret = get_sha_512_256_hash((const unsigned char *)&(digest_param->response),
                                       length,
                                       (unsigned char *)&(digest_param->response_hash),
                                       150);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] get_digest_response response get_sha_512_256_hash error\n");
#endif
                goto error;
            }
        }else
        {
#ifdef _DEBUG
            std::printf("[-] Not implemented\n");
#endif
            goto error;
        }

        free(digest_http_header_data);
        free(tmp1);
        free(tmp2);
        free(tmp3);
        free(tmp4);
        return 0;

error:
        free(digest_http_header_data);
        free(tmp1);
        free(tmp2);
        free(tmp3);
        free(tmp4);
        return -1;
    }
}

