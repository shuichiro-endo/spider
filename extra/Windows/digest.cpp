/*
 * Title:  spider digest.cpp (Windows)
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

    int Digest::rand_bytes(unsigned char *buffer,
                           int32_t rand_bytes_size)
    {
        BOOL ret_b = false;
        HCRYPTPROV h_provider = NULL;


        ret_b = CryptAcquireContext(&h_provider,
                                    NULL,
                                    NULL,
                                    PROV_RSA_AES,
                                    CRYPT_VERIFYCONTEXT);
        if(ret_b == false)
        {
#ifdef DEBUGPRINT
            std::printf("[-] rand_bytes CryptBinaryToStringA error\n");
#endif
            goto error;
        }

        ret_b = CryptGenRandom(h_provider,
                               rand_bytes_size,
                               buffer);
        if(ret_b == false)
        {
#ifdef DEBUGPRINT
            std::printf("[-] rand_bytes CryptGenRandom error\n");
#endif
            goto error;
        }

        CryptReleaseContext(h_provider,
                            0);
        return rand_bytes_size;

error:
        if(h_provider != NULL)
        {
            CryptReleaseContext(h_provider,
                                0);
        }
        return -1;
    }

    int Digest::encode_base64(const unsigned char *input,
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

    int Digest::get_md5_hash(const unsigned char *input,
                             int input_length,
                             unsigned char *output,
                             int output_size)
    {
        int ret = 0;
        BOOL ret_b = false;
        DWORD required_size = 0;
        DWORD size = sizeof(required_size);
        unsigned char *digest = NULL;
        int output_length = 0;
        HCRYPTPROV h_prov = NULL;
        HCRYPTHASH h_hash = NULL;


        ret_b = CryptAcquireContext(&h_prov,
                                    NULL,
                                    NULL,
                                    PROV_RSA_AES,
                                    CRYPT_VERIFYCONTEXT);
        if(ret_b == false)
        {
#ifdef DEBUGPRINT
            std::printf("[-] get_md5_hash CryptAcquireContext error\n");
#endif
            goto error;
        }


        ret_b = CryptCreateHash(h_prov,
                                CALG_MD5,
                                0,
                                0,
                                &h_hash);
        if(ret_b == false)
        {
#ifdef DEBUGPRINT
            std::printf("[-] get_md5_hash CryptCreateHash error\n");
#endif
            goto error;
        }

        ret_b = CryptHashData(h_hash,
                              input,
                              input_length,
                              0);
        if(ret_b == false)
        {
#ifdef DEBUGPRINT
            std::printf("[-] get_md5_hash CryptHashData error\n");
#endif
            goto error;
        }

        ret_b = CryptGetHashParam(h_hash,
                                  HP_HASHSIZE,
                                  (BYTE *)&required_size,
                                  &size,
                                  0);
        if(ret_b == false)
        {
#ifdef DEBUGPRINT
            std::printf("[-] get_md5_hash CryptGetHashParam error\n");
#endif
            goto error;
        }

        digest = (unsigned char *)calloc(required_size,
                                  sizeof(char));

        ret_b = CryptGetHashParam(h_hash,
                                  HP_HASHVAL,
                                  digest,
                                  &required_size,
                                  0);
        if(ret_b == false)
        {
#ifdef DEBUGPRINT
            std::printf("[-] get_md5_hash CryptGetHashParam error\n");
#endif
            goto error;
        }

        if(output_size < required_size * 2)
        {
#ifdef DEBUGPRINT
            std::printf("[-] get_md5_hash output_size error required_size * 2 : %d output_size: %d\n",
                        required_size * 2,
                        output_size);
#endif
            goto error;
        }

        for(int i=0; i*8<required_size; i++)
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

        output_length = required_size * 2;

        free(digest);
        CryptDestroyHash(h_hash);
        CryptReleaseContext(h_prov,
                            0);
        return output_length;

error:
        if(digest != NULL)
        {
            free(digest);
        }

        if(h_hash != NULL)
        {
            CryptDestroyHash(h_hash);
        }

        if(h_prov != NULL)
        {
            CryptReleaseContext(h_prov,
                                0);
        }

        return -1;
    }

    int Digest::get_sha_256_hash(const unsigned char *input,
                                 int input_length,
                                 unsigned char *output,
                                 int output_size)
    {
        int ret = 0;
        BOOL ret_b = false;
        DWORD required_size = 0;
        DWORD size = sizeof(required_size);
        unsigned char *digest = NULL;
        int output_length = 0;
        HCRYPTPROV h_prov = NULL;
        HCRYPTHASH h_hash = NULL;


        ret_b = CryptAcquireContext(&h_prov,
                                    NULL,
                                    NULL,
                                    PROV_RSA_AES,
                                    CRYPT_VERIFYCONTEXT);
        if(ret_b == false)
        {
#ifdef DEBUGPRINT
            std::printf("[-] get_sha_256_hash CryptAcquireContext error\n");
#endif
            goto error;
        }


        ret_b = CryptCreateHash(h_prov,
                                CALG_SHA_256,
                                0,
                                0,
                                &h_hash);
        if(ret_b == false)
        {
#ifdef DEBUGPRINT
            std::printf("[-] get_sha_256_hash CryptCreateHash error\n");
#endif
            goto error;
        }

        ret_b = CryptHashData(h_hash,
                              input,
                              input_length,
                              0);
        if(ret_b == false)
        {
#ifdef DEBUGPRINT
            std::printf("[-] get_sha_256_hash CryptHashData error\n");
#endif
            goto error;
        }

        ret_b = CryptGetHashParam(h_hash,
                                  HP_HASHSIZE,
                                  (BYTE *)&required_size,
                                  &size,
                                  0);
        if(ret_b == false)
        {
#ifdef DEBUGPRINT
            std::printf("[-] get_sha_256_hash CryptGetHashParam error\n");
#endif
            goto error;
        }

        digest = (unsigned char *)calloc(required_size,
                                         sizeof(char));

        ret_b = CryptGetHashParam(h_hash,
                                  HP_HASHVAL,
                                  digest,
                                  &required_size,
                                  0);
        if(ret_b == false)
        {
#ifdef DEBUGPRINT
            std::printf("[-] get_sha_256_hash CryptGetHashParam error\n");
#endif
            goto error;
        }

        if(output_size < required_size * 2)
        {
#ifdef DEBUGPRINT
            std::printf("[-] get_md5_hash output_size error required_size * 2 : %d output_size: %d\n",
                        required_size * 2,
                        output_size);
#endif
            goto error;
        }

        for(int i=0; i*8<required_size; i++)
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

        output_length = required_size * 2;

        free(digest);
        CryptDestroyHash(h_hash);
        CryptReleaseContext(h_prov,
                            0);
        return output_length;

error:
        if(digest != NULL)
        {
            free(digest);
        }

        if(h_hash != NULL)
        {
            CryptDestroyHash(h_hash);
        }

        if(h_prov != NULL)
        {
            CryptReleaseContext(h_prov,
                                0);
        }

        return -1;
    }

    int Digest::get_sha_512_256_hash(const unsigned char *input,
                                     int input_length,
                                     unsigned char *output,
                                     int output_size)
    {
#ifdef DEBUGPRINT
        std::printf("[-] Not implemented\n");
#endif
        return -1;
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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

#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
            std::printf("[-] get_http_header start is NULL\n");
#endif
            return -1;
        }

        end = strstr(start,
                     "\r\n");
        if(end == NULL)
        {
#ifdef DEBUGPRINT
            std::printf("[-] get_http_header end is NULL\n");
#endif
            return -1;
        }

        d = end - start;
        if(d <= 0 ||
           d >= output_size)
        {
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
            std::printf("[-] get_digest_response get_http_header error\n");
#endif
            goto error;
        }

        ret = get_digest_values(digest_http_header_data);
        if(ret < 0)
        {
#ifdef DEBUGPRINT
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
        ret = rand_bytes((unsigned char *)tmp1,
                         16);
        if(ret < 0)
        {
#ifdef DEBUGPRINT
            std::printf("[-] get_digest_response rand_bytes error\n");
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

            ret = rand_bytes((unsigned char *)tmp1,
                             16);
            if(ret < 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] get_digest_response rand_bytes error\n");
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
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
#ifdef DEBUGPRINT
                std::printf("[-] get_digest_response response get_sha_512_256_hash error\n");
#endif
                goto error;
            }
        }else
        {
#ifdef DEBUGPRINT
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

