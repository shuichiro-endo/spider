/*
 * Title:  spider ntlmv2.cpp (Linux)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "ntlm.hpp"
#include "ntlmv2.hpp"


namespace spider
{
    Ntlmv2::Ntlmv2(std::string username,
                   std::string password,
                   std::string nthash_hexstring,
                   std::string domainname,
                   std::string workstationname)
    {
        this->username = username;
        this->password = password;
        this->nthash_hexstring = nthash_hexstring;
        this->domainname = domainname;
        this->workstationname = workstationname;
    }

    Ntlmv2::~Ntlmv2()
    {

    }

    void Ntlmv2::print_bytes(unsigned char *input,
                             int input_length)
    {
        for(int i=0; i<input_length; i++)
        {
            if(i != 0 && i%16 == 0)
            {
                std::printf("\n");
            }else if(i%16 == 8)
            {
                std::printf(" ");
            }
            std::printf("%02x ", input[i]);
        }
        std::printf("\n");

        return;
    }

    int Ntlmv2::encode_base64(const unsigned char *input,
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

    int Ntlmv2::decode_base64(const unsigned char *input,
                              int32_t input_length,
                              unsigned char *output,
                              int32_t output_size)
    {
        BIO *b64 = BIO_new(BIO_f_base64());
        BIO_set_flags(b64,
                      BIO_FLAGS_BASE64_NO_NL);
        BIO *mem = BIO_new_mem_buf((char *)input,
                                   -1);
        int output_length = 0;
        int ret = 0;
        BIO *bio = BIO_push(b64,
                            mem);

        if(input_length > output_size)
        {
#ifdef _DEBUG
            std::printf("[-] decode_base64 output_size error\n");
#endif
            goto error;
        }

        output_length = BIO_read(bio,
                                 output,
                                 input_length);
        if(output_length <= 0)
        {
#ifdef _DEBUG
            std::printf("[-] decode_base64 BIO_read error\n");
#endif
            goto error;
        }

        ret = BIO_flush(bio);
        if(ret <= 0)
        {
#ifdef _DEBUG
            std::printf("[-] decode_base64 BIO_flush error\n");
#endif
            goto error;
        }

        BIO_free_all(bio);
        return output_length;

error:
        BIO_free_all(bio);
        return -1;
    }

    int Ntlmv2::get_md4_hash(const unsigned char *input,
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
            std::printf("[-] get_md4_hash EVP_MD_CTX_new error\n");
#endif
            return -1;
        }

        ret = EVP_DigestInit_ex(ctx,
                                EVP_md4(),
                                NULL);
        if(ret != 1)
        {
#ifdef _DEBUG
            std::printf("[-] get_md4_hash EVP_DigestInit_ex error\n");
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
            std::printf("[-] get_md4_hash EVP_DigestUpdate error\n");
#endif
            EVP_MD_CTX_free(ctx);
            return -1;
        }

        if(EVP_MD_size(EVP_md4()) > output_size)
        {
#ifdef _DEBUG
            std::printf("[-] get_md4_hash output_size error\n");
#endif
            EVP_MD_CTX_free(ctx);
            return -1;
        }

        digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_md4()));
        if(digest == NULL)
        {
#ifdef _DEBUG
            std::printf("[-] get_md4_hash OPENSSL_malloc error\n");
#endif
            EVP_MD_CTX_free(ctx);
            return -1;
        }

        ret = EVP_DigestFinal_ex(ctx,
                                 (unsigned char *)digest,
                                 &length);
        if(ret != 1)
        {
#ifdef _DEBUG
            std::printf("[-] get_md4_hash EVP_DigestFinal_ex error\n");
#endif
            OPENSSL_free(digest);
            EVP_MD_CTX_free(ctx);
            return -1;
        }

        std::memcpy(output,
                    digest,
                    length);

        OPENSSL_free(digest);
        EVP_MD_CTX_free(ctx);
        return length;
    }

    int Ntlmv2::get_hmac_md5(const unsigned char *input,
                             int input_length,
                             const unsigned char *key,
                             int key_length,
                             unsigned char *output,
                             int output_size)
    {	EVP_MAC *mac = NULL;
        EVP_MAC_CTX *ctx = NULL;
        const char digest[] = "MD5";
        OSSL_PARAM params[] =
        {
            OSSL_PARAM_construct_utf8_string("digest",
                                             (char *)&digest,
                                             0),
            OSSL_PARAM_construct_end()
        };
        int ret = 0;
        size_t length = 0;

        mac = EVP_MAC_fetch(NULL, "HMAC", NULL);
        if(mac == NULL)
        {
#ifdef _DEBUG
            std::printf("[-] get_hmac_md5 EVP_MAC_fetch error\n");
#endif
            return -1;
        }

        ctx = EVP_MAC_CTX_new(mac);
        if(ctx == NULL)
        {
#ifdef _DEBUG
            std::printf("[-] get_hmac_md5 EVP_MAC_CTX_new error\n");
#endif
            EVP_MAC_free(mac);
            return -1;
        }

        ret = EVP_MAC_init(ctx,
                           key,
                           key_length,
                           params);
        if(ret != 1)
        {
#ifdef _DEBUG
            std::printf("[-] get_hmac_md5 EVP_MAC_init error\n");
#endif
            EVP_MAC_CTX_free(ctx);
            EVP_MAC_free(mac);
            return -1;
        }

        ret = EVP_MAC_update(ctx,
                             input,
                             input_length);
        if(ret != 1)
        {
#ifdef _DEBUG
            std::printf("[-] get_hmac_md5 EVP_MAC_update error\n");
#endif
            EVP_MAC_CTX_free(ctx);
            EVP_MAC_free(mac);
            return -1;
        }

        ret = EVP_MAC_final(ctx,
                            output,
                            (size_t *)&length,
                            output_size);
        if(ret != 1)
        {
#ifdef _DEBUG
            std::printf("[-] get_hmac_md5 EVP_MAC_final error\n");
#endif
            EVP_MAC_CTX_free(ctx);
            EVP_MAC_free(mac);
            return -1;
        }

        EVP_MAC_CTX_free(ctx);
        EVP_MAC_free(mac);
        return length;
    }

    int Ntlmv2::get_upper_string(const char *input,
                                 int input_length,
                                 char *output)
    {
        for(int i=0; i<input_length; i++)
        {
            output[i] = toupper(input[i]);
        }

        return 0;
    }

    int Ntlmv2::get_number_of_bytes_of_utf16_string(char *input)
    {
        int i = 0;

        while(!(input[i] == '\0' &&
                input[i+1] == '\0'))
        {
            i += 2;
        }

        return i;
    }

    int Ntlmv2::convert_utf8_to_utf16(const char *input,
                                      char *output,
                                      size_t output_size)
    {
        iconv_t conv;
        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        char *input_buffer = buffer;
        char *output_buffer = output;
        size_t input_length = strlen(input);
        int output_length = (int)input_length*2;
        size_t size = output_size-2;
        int ret = 0;

        if(input_length*2 > NODE_BUFFER_SIZE)
        {
#ifdef _DEBUG
            std::printf("[-] convert_utf8_to_utf16 input_length error\n");
#endif
            goto error;
        }

        strncpy(buffer,
                input,
                input_length);

        conv = iconv_open("UTF-16LE",
                          "UTF-8");
        if(conv == (iconv_t)-1)
        {
#ifdef _DEBUG
            std::printf("[-] convert_utf8_to_utf16 iconv_open error\n");
#endif
            goto error;
        }

        ret = iconv(conv,
                    &input_buffer,
                    &input_length,
                    &output_buffer,
                    &size);
        if(ret == -1)
        {
#ifdef _DEBUG
            std::printf("[-] convert_utf8_to_utf16 iconv error\n");
#endif
            goto error;
        }

        ret = iconv_close(conv);
        if(ret == -1)
        {
#ifdef _DEBUG
            std::printf("[-] convert_utf8_to_utf16 iconv_close error\n");
#endif
            goto error;
        }

        output_length = get_number_of_bytes_of_utf16_string(output);

#ifdef _DEBUG
//      std::printf("input:%s, input_length:%d\n", input, strlen(input));
//      std::printf("output:%d\n", output_length);
//      print_bytes((unsigned char *)output, output_length);
#endif

        free(buffer);
        return output_length;

error:
        free(buffer);
        return -1;
    }

    int Ntlmv2::get_av_pair_value(struct challenge_message *challenge_message,
                                  uint16_t av_id,
                                  unsigned char *data,
                                  int data_size)
    {
        uint16_t target_info_len = 0;
        uint16_t target_info_max_len = 0;
        uint32_t target_info_buffer_offset = 0;
        unsigned char *pos = NULL;
        struct av_pair *av_pair = NULL;
        int length = 0;
        int data_length = 0;

        target_info_len = challenge_message->target_info_fields.target_info_len;
        target_info_max_len = challenge_message->target_info_fields.target_info_max_len;
        target_info_buffer_offset = challenge_message->target_info_fields.target_info_buffer_offset;
        pos = (unsigned char *)challenge_message+target_info_buffer_offset;

#ifdef _DEBUG
//      std::printf("target_info_len:%d\n", target_info_len);
//      std::printf("target_info_max_len:%d\n", target_info_max_len);
//      std::printf("target_info_buffer_offset:%d\n", target_info_buffer_offset);
#endif

        while(length < target_info_max_len)
        {
            av_pair = (struct av_pair *)pos;

#ifdef _DEBUG
//          std::printf("av_id:%d\n", av_pair->av_id);
//          std::printf("av_len:%d\n", av_pair->av_len);
#endif

            if(av_id == av_pair->av_id)
            {
                if(av_pair->av_len > data_size)
                {
#ifdef _DEBUG
                    std::printf("[-] get_av_pair_value data_size error\n");
#endif
                    break;
                }else
                {
                    data_length = av_pair->av_len;
                    std::memcpy(data,
                                &av_pair->value,
                                av_pair->av_len);
                }
            }

            length += 4 + av_pair->av_len;
            pos += length;
        }

        return data_length;
    }

    char Ntlmv2::hex_char_to_int(char c)
    {
        char ret = 0;

        if(c >= '0' &&
           c <= '9')
        {
            ret = c - '0';
        }else if(c >= 'a' &&
                 c <= 'f')
        {
            ret = c + 10 - 'a';
        }else if(c >= 'A' &&
                 c <= 'F')
        {
            ret = c + 10 - 'A';
        }else
        {
            ret = -1;
        }

        return ret;
    }

    int Ntlmv2::hexstring_to_array(const char *hexstring,
                                   int hexstring_length,
                                   unsigned char *output,
                                   int output_size)
    {
        char tmp1 = 0;
        char tmp2 = 0;
        int output_length = 0;

        if(hexstring_length % 2 != 0)
        {
#ifdef _DEBUG
            std::printf("[-] hexstring_to_array hexstring_length error\n");
#endif
            return -1;
        }

        if(hexstring_length / 2 > output_size)
        {
#ifdef _DEBUG
            std::printf("[-] hexstring_to_array hexstring_length error\n");
#endif
            return -1;
        }

        for(int i=0; i<hexstring_length; i+=2)
        {
            tmp1 = hex_char_to_int(hexstring[i]);
            tmp2 = hex_char_to_int(hexstring[i+1]);

            if(tmp1 == -1 ||
               tmp2 == -1)
            {
#ifdef _DEBUG
                std::printf("[-] hexstring_to_array hex_char_to_int error\n");
#endif
                return -1;
            }

            tmp1 = tmp1 << 4;
            output[output_length] = (unsigned char)(tmp1 + tmp2);
            output_length++;
        }

        return output_length;
    }

    /*
     * Reference:
     * https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-nlmp/5e550938-91d4-459f-b67d-75d70009e3f3
     */
    int Ntlmv2::ntowfv2(const char *user,
                        const char *password,
                        const char *nthash,
                        const char *userdom,
                        unsigned char *output,
                        int output_size)
    {
        int ret = 0;
        int password_length = 0;
        int password_utf16le_length = 0;
        int password_utf16le_md4_length = 0;
        unsigned char *password_utf16le = (unsigned char *)calloc(1000,
                                                                  sizeof(unsigned char));
        unsigned char *password_utf16le_md4 = (unsigned char *)calloc(100,
                                                                      sizeof(unsigned char));

        int user_length = strlen(user);
        int userdom_length = strlen(userdom);
        int user_upper_userdom_length = 0;
        int user_upper_userdom_utf16le_length = 0;
        char *user_upper = (char *)calloc(256,
                                          sizeof(char));
        char *user_upper_userdom = (char *)calloc(1000,
                                                  sizeof(char));
        unsigned char *user_upper_userdom_utf16le = (unsigned char *)calloc(2000,
                                                                            sizeof(unsigned char));
        char *pos = NULL;
        int response_key_length = 0;
        unsigned char *response_key = (unsigned char *)calloc(16,
                                                              sizeof(unsigned char));


        if(nthash == NULL)
        {
            password_length = strlen(password);

            // UNICODE(Passwd)
            ret = convert_utf8_to_utf16(password,
                                        (char *)password_utf16le,
                                        1000);
            if(ret < 0)
            {
#ifdef _DEBUG
                std::printf("[-] ntowfv2 convert_utf8_to_utf16 error\n");
#endif
                goto error;
            }
            password_utf16le_length = ret;

#ifdef _DEBUG
//          std::printf("password_utf16le:%d\n", password_utf16le_length);
//          print_bytes(password_utf16le, password_utf16le_length);
#endif

            // MD4(UNICODE(Passwd))
            ret = get_md4_hash((const unsigned char *)password_utf16le,
                               password_utf16le_length,
                               (unsigned char *)password_utf16le_md4,
                               16);
            if(ret < 0)
            {
#ifdef _DEBUG
                std::printf("[-] ntowfv2 get_md4_hash error\n");
#endif
                goto error;
            }
            password_utf16le_md4_length = ret;
        }else   // NTHash
        {
            ret = hexstring_to_array(nthash,
                                     strlen(nthash),
                                     (unsigned char *)password_utf16le_md4,
                                     16);
            if(ret != 16)
            {
#ifdef _DEBUG
                std::printf("[-] ntowfv2 hexstring_to_array error\n");
#endif
                goto error;
            }
            password_utf16le_md4_length = ret;
        }

#ifdef _DEBUG
//      std::printf("password_utf16le_md4:%d\n", password_utf16le_md4_length);
//      print_bytes(password_utf16le_md4, password_utf16le_md4_length);
#endif


        // Uppercase(user)
        ret = get_upper_string(user,
                               strlen(user),
                               (char *)user_upper);

        // ConcatenationOf(Uppercase(User), UserDom)
        user_upper_userdom_length = 0;
        pos = (char *)user_upper_userdom;

        std::memcpy(pos,
                    user_upper,
                    user_length);
        user_upper_userdom_length += user_length;

        std::memcpy(pos+user_upper_userdom_length,
                    userdom,
                    userdom_length);
        user_upper_userdom_length += userdom_length;

        // UNICODE(ConcatenationOf(Uppercase(User), UserDom))
        ret = convert_utf8_to_utf16((const char *)user_upper_userdom,
                                    (char *)user_upper_userdom_utf16le,
                                    2000);
        if(ret < 0)
        {
#ifdef _DEBUG
            std::printf("[-] ntowfv2 convert_utf8_to_utf16 error\n");
#endif
            goto error;
        }
        user_upper_userdom_utf16le_length = ret;

#ifdef _DEBUG
//      std::printf("user_upper_userdom_utf16le:%d\n", user_upper_userdom_utf16le_length);
//      print_bytes(user_upper_userdom_utf16le, user_upper_userdom_utf16le_length);
#endif

        // HMAC_MD5(K, M)	Indicates the computation of a 16-byte HMAC-keyed MD5 message digest of the byte string M using the key K.
        // HMAC_MD5(MD4(UNICODE(Passwd)), UNICODE(ConcatenationOf(Uppercase(User), UserDom)))
        ret = get_hmac_md5((const unsigned char *)user_upper_userdom_utf16le,
                           user_upper_userdom_utf16le_length,
                           (const unsigned char *)password_utf16le_md4,
                           password_utf16le_md4_length,
                           (unsigned char *)response_key,
                           16);
        if(ret < 0)
        {
#ifdef _DEBUG
            std::printf("[-] ntowfv2 get_hmac_md5 error\n");
#endif
            goto error;
        }
        response_key_length = ret;

#ifdef _DEBUG
//      std::printf("response_key:%d\n", response_key_length);
//      print_bytes(response_key, response_key_length);
#endif

        if(output_size > response_key_length)
        {
#ifdef _DEBUG
            std::printf("[-] ntowfv2 output_size error\n");
#endif
            goto error;
        }

        std::memcpy(output,
                    response_key,
                    response_key_length);

        free(password_utf16le);
        free(password_utf16le_md4);
        free(user_upper);
        free(user_upper_userdom);
        free(user_upper_userdom_utf16le);
        free(response_key);
        return response_key_length;

error:
        free(password_utf16le);
        free(password_utf16le_md4);
        free(user_upper);
        free(user_upper_userdom);
        free(user_upper_userdom_utf16le);
        free(response_key);
        return -1;
    }

    /*
     * Reference:
     * https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-nlmp/5e550938-91d4-459f-b67d-75d70009e3f3
     */
    int Ntlmv2::lmowfv2(const char *user,
                        const char *password,
                        const char *nthash,
                        const char *userdom,
                        unsigned char *output,
                        int output_size)
    {
        int ret = 0;
        int response_key_length = 0;

        ret = ntowfv2(user,
                      password,
                      nthash,
                      userdom,
                      output,
                      output_size);
        if(ret < 0)
        {
#ifdef _DEBUG
            std::printf("[-] lmowfv2 ntowfv2 error\n");
#endif
            return -1;
        }
        response_key_length = ret;

        return response_key_length;
    }

    int Ntlmv2::get_http_header(const char *input,
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

    void Ntlmv2::set_username(std::string username)
    {
        this->username = username;
    }

    std::string Ntlmv2::get_username()
    {
        return username;
    }

    void Ntlmv2::set_password(std::string password)
    {
        this->password = password;
    }

    std::string Ntlmv2::get_password()
    {
        return password;
    }

    void Ntlmv2::set_nthash_hexstring(std::string nthash_hexstring)
    {
        this->nthash_hexstring = nthash_hexstring;
    }

    std::string Ntlmv2::get_nthash_hexstring()
    {
        return nthash_hexstring;
    }

    void Ntlmv2::set_domainname(std::string domainname)
    {
        this->domainname = domainname;
    }

    std::string Ntlmv2::get_domainname()
    {
        return domainname;
    }

    void Ntlmv2::set_workstationname(std::string workstationname)
    {
        this->workstationname = workstationname;
    }

    std::string Ntlmv2::get_workstationname()
    {
        return workstationname;
    }


    int Ntlmv2::generate_base64_negotiate_message(char *ntlm_b64,
                                                  int32_t ntlm_b64_size)
    {
        int ret = 0;
        int ntlm_negotiate_message_length = 0;
        char *ntlm = (char *)calloc(NODE_BUFFER_SIZE,
                                    sizeof(char));
        struct negotiate_message *negotiate_message = (struct negotiate_message *)ntlm;


        // negotiate message
        std::memcpy(&negotiate_message->signature,
                    "NTLMSSP\0",
                    8);

        ntlm_negotiate_message_length += 8;

        negotiate_message->message_type = NtLmNegotiate;
        ntlm_negotiate_message_length += 4;

        negotiate_message->negotiate_flags.negotiate_unicode                  = 0;
        negotiate_message->negotiate_flags.negotiate_oem                      = 1;
        negotiate_message->negotiate_flags.request_target                     = 1;
        negotiate_message->negotiate_flags.request_0x00000008                 = 0;
        negotiate_message->negotiate_flags.negotiate_sign                     = 0;
        negotiate_message->negotiate_flags.negotiate_seal                     = 0;
        negotiate_message->negotiate_flags.negotiate_datagram                 = 0;
        negotiate_message->negotiate_flags.negotiate_lan_manager_key          = 0;
        negotiate_message->negotiate_flags.negotiate_0x00000100               = 0;
        negotiate_message->negotiate_flags.negotiate_ntlm_key                 = 1;
        negotiate_message->negotiate_flags.negotiate_nt_only                  = 0;
        negotiate_message->negotiate_flags.negotiate_anonymous                = 0;
        negotiate_message->negotiate_flags.negotiate_oem_domain_supplied      = 0;
        negotiate_message->negotiate_flags.negotiate_oem_workstation_supplied = 0;
        negotiate_message->negotiate_flags.negotiate_0x00004000               = 0;
        negotiate_message->negotiate_flags.negotiate_always_sign              = 1;
        negotiate_message->negotiate_flags.target_type_domain                 = 0;
        negotiate_message->negotiate_flags.target_type_server                 = 0;
        negotiate_message->negotiate_flags.target_type_share                  = 0;
        negotiate_message->negotiate_flags.negotiate_extended_security        = 1;
        negotiate_message->negotiate_flags.negotiate_identify                 = 0;
        negotiate_message->negotiate_flags.negotiate_0x00200000               = 0;
        negotiate_message->negotiate_flags.request_non_nt_session             = 0;
        negotiate_message->negotiate_flags.negotiate_target_info              = 0;
        negotiate_message->negotiate_flags.negotiate_0x01000000               = 0;
        negotiate_message->negotiate_flags.negotiate_version                  = 0;
        negotiate_message->negotiate_flags.negotiate_0x04000000               = 0;
        negotiate_message->negotiate_flags.negotiate_0x08000000               = 0;
        negotiate_message->negotiate_flags.negotiate_0x10000000               = 0;
        negotiate_message->negotiate_flags.negotiate_128                      = 0;
        negotiate_message->negotiate_flags.negotiate_key_exchange             = 0;
        negotiate_message->negotiate_flags.negotiate_56                       = 0;
        ntlm_negotiate_message_length += 4;

        negotiate_message->domain_name_fields.domain_name_len = 0;
        negotiate_message->domain_name_fields.domain_name_max_len = 0;
        negotiate_message->domain_name_fields.domain_name_buffer_offset = 0;
        ntlm_negotiate_message_length += 8;

        negotiate_message->workstation_fields.workstation_len = 0;
        negotiate_message->workstation_fields.workstation_max_len = 0;
        negotiate_message->workstation_fields.workstation_buffer_offset = 0;
        ntlm_negotiate_message_length += 8;

        ret = encode_base64((const unsigned char *)negotiate_message,
                            ntlm_negotiate_message_length,
                            (unsigned char *)ntlm_b64,
                            ntlm_b64_size);
        if(ret < 0)
        {
#ifdef _DEBUG
            std::printf("[-] get_base64_negotiate_message encode_base64 error\n");
#endif
            goto error;
        }

        free(ntlm);
        return 0;

error:
        free(ntlm);
        return -1;
    }

    int Ntlmv2::generate_response_ntlmv2(struct challenge_message *challenge_message,
                                         struct authenticate_message *authenticate_message)
    {
        int ret = 0;

        unsigned char *response_key_nt = (unsigned char *)calloc(16,
                                                                 sizeof(unsigned char));
        unsigned char *response_key_lm = (unsigned char *)calloc(16,
                                                                 sizeof(unsigned char));
        int response_key_nt_length = 0;
        int response_key_lm_length = 0;
        unsigned char *server_challenge = (unsigned char *)calloc(8,
                                                                  sizeof(unsigned char));
        unsigned char *client_challenge = (unsigned char *)calloc(8,
                                                                  sizeof(unsigned char));

        unsigned char responser_version = 1;
        unsigned char hi_responser_version = 1;
        int64_t timestamp = 0;
        unsigned char *server_name = (unsigned char *)calloc(1000,
                                                             sizeof(unsigned char));
        int server_name_length = 0;

        unsigned char *temp = (unsigned char *)calloc(2000,
                                                      sizeof(unsigned char));
        int temp_length = 0;
        unsigned char *pos = NULL;

        unsigned char *nt_proof_str = (unsigned char *)calloc(16,
                                                              sizeof(unsigned char));
        int nt_proof_str_length = 0;
        unsigned char *tmp1 = (unsigned char *)calloc(3000,
                                                      sizeof(unsigned char));
        int tmp1_length = 0;

        unsigned char *nt_challenge_response = (unsigned char *)calloc(2016,
                                                                       sizeof(unsigned char));
        int nt_challenge_response_length = 0;

        unsigned char *lm_challenge_response = (unsigned char *)calloc(24,
                                                                       sizeof(unsigned char));
        int lm_challenge_response_length = 0;
        unsigned char *server_challenge_client_challenge = (unsigned char *)calloc(16,
                                                                                   sizeof(unsigned char));
        unsigned char *tmp2 = (unsigned char *)calloc(16,
                                                      sizeof(unsigned char));
        int tmp2_length = 0;

        unsigned char *session_base_key = (unsigned char *)calloc(16,
                                                                  sizeof(unsigned char));
        int session_base_key_length = 0;

        int authenticate_message_length = 0;
        int32_t offset = 0;
        const char *domainname = NULL;
        const char *username = NULL;
        const char *password = NULL;
        const char *nthash = NULL;
        const char *workstationname = NULL;
        int domainname_length = 0;
        int username_length = 0;
        int workstationname_length = 0;

        if(!this->domainname.empty())
        {
            domainname = this->domainname.c_str();
            domainname_length = strlen(domainname);
        }

        if(!this->username.empty())
        {
            username = this->username.c_str();
            username_length = strlen(username);

        }

        if(!this->password.empty())
        {
            password = this->password.c_str();
        }

        if(!this->nthash_hexstring.empty())
        {
            nthash = this->nthash_hexstring.c_str();
        }

        if(!this->workstationname.empty())
        {
            workstationname = this->workstationname.c_str();
            workstationname_length = strlen(workstationname);
        }

        if(username == NULL &&
           password == NULL)
        {
            // Special case for anonymous authentication
            // Set NtChallengeResponseLen to 0
            // Set NtChallengeResponseMaxLen to 0
            // Set NtChallengeResponseBufferOffset to 0
            // Set LmChallengeResponse to Z(1)
#ifdef _DEBUG
            std::printf("[-] Not implemented\n");
#endif
            goto error;
        }else
        {
            // ResponseKeyNT
            ret = ntowfv2(username,
                          password,
                          nthash,
                          domainname,
                          (unsigned char *)response_key_nt,
                          16);
            if(ret < 0)
            {
#ifdef _DEBUG
                std::printf("[-] generate_response_ntlmv2 ntowfv2 error\n");
#endif
                goto error;
            }
            response_key_nt_length = ret;


            // ResponseKeyLM
            ret = lmowfv2(username,
                          password,
                          nthash,
                          domainname,
                          (unsigned char *)response_key_lm,
                          16);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] generate_response_ntlmv2 lmowfv2 error\n");
#endif
                goto error;
            }
            response_key_lm_length = ret;


            // ServerChallenge
            std::memcpy(server_challenge,
                        &challenge_message->server_challenge,
                        8);

#ifdef _DEBUG
//          std::printf("server_challenge: %d\n", 8);
//          print_bytes(server_challenge, 8);
#endif


            // ClientChallenge
            ret = RAND_bytes((unsigned char *)client_challenge,
                             8);
            if(ret != 1)
            {
#ifdef _DEBUG
                std::printf("[-] generate_response_ntlmv2 client_challenge generate error: %s\n",
                            ERR_error_string(ERR_peek_last_error(), NULL));
#endif
                goto error;
            }

#ifdef _DEBUG
//          std::printf("client_challenge: %d\n", 8);
//          print_bytes(client_challenge, 8);
#endif


            // TIME
            timestamp = (time(NULL) + 11644473600) * 10000000;

#ifdef _DEBUG
//          std::printf("time: %d\n", 8);
//          print_bytes((unsigned char *)&timestamp, 8);
#endif


            // ServerName
            // The NtChallengeResponseFields.NTLMv2_RESPONSE.NTLMv2_CLIENT_CHALLENGE.AvPairs field structure of the AUTHENTICATE_MESSAGE payload.
            server_name_length = challenge_message->target_info_fields.target_info_len;
            pos = (unsigned char *)challenge_message;
            pos += challenge_message->target_info_fields.target_info_buffer_offset;

            if(server_name_length > 1000)
            {
#ifdef _DEBUG
                std::printf("[-] generate_response_ntlmv2 server_name_length error\n");
#endif
                goto error;
            }
            std::memcpy(server_name,
                        pos,
                        server_name_length);

#ifdef _DEBUG
//          std::printf("server_name: %d\n", server_name_length);
//          print_bytes((unsigned char *)server_name, server_name_length);
#endif


            // temp
            // ConcatenationOf(Responserversion, HiResponserversion, Z(6), Time, ClientChallenge, Z(4), ServerName, Z(4))
            pos = (unsigned char *)temp;
            temp_length = 0;

            std::memcpy(pos+temp_length,
                        &responser_version,
                        1);
            temp_length += 1;

            std::memcpy(pos+temp_length,
                        &hi_responser_version,
                        1);
            temp_length += 1;

            temp_length += 6;   // Z(6)

            std::memcpy(pos+temp_length,
                        &timestamp,
                        8);
            temp_length += 8;

            std::memcpy(pos+temp_length,
                        client_challenge,
                        8);
            temp_length += 8;

            temp_length += 4;   // Z(4)

            std::memcpy(pos+temp_length,
                        server_name,
                        server_name_length);
            temp_length += server_name_length;

            temp_length += 4;   // Z(4)

#ifdef _DEBUG
//          std::printf("temp: %d\n", temp_length);
//          print_bytes((unsigned char *)temp, temp_length);
#endif


            // NTProofStr
            // ConcatenationOf(CHALLENGE_MESSAGE.ServerChallenge,temp)
            pos = (unsigned char *)tmp1;
            tmp1_length = 0;

            std::memcpy(pos+tmp1_length,
                        server_challenge,
                        8);
            tmp1_length += 8;

            std::memcpy(pos+tmp1_length,
                        temp,
                        temp_length);
            tmp1_length += temp_length;

#ifdef _DEBUG
//          std::printf("tmp1: %d\n", tmp1_length);
//          print_bytes((unsigned char *)tmp1, tmp1_length);
#endif

            // HMAC_MD5(ResponseKeyNT, ConcatenationOf(CHALLENGE_MESSAGE.ServerChallenge,temp))
            ret = get_hmac_md5((unsigned char *)tmp1,
                               tmp1_length,
                               (unsigned char *)response_key_nt,
                               response_key_nt_length,
                               (unsigned char *)nt_proof_str,
                               16);
            if(ret < 0)
            {
#ifdef _DEBUG
                std::printf("[-] generate_response_ntlmv2 get_hmac_md5 error\n");
#endif
                goto error;
            }
            nt_proof_str_length = ret;

#ifdef _DEBUG
//          std::printf("nt_proof_str: %d\n", nt_proof_str_length);
//          print_bytes((unsigned char *)nt_proof_str, nt_proof_str_length);
#endif


            // NtChallengeResponse
            // ConcatenationOf(NTProofStr, temp)
            pos = (unsigned char *)nt_challenge_response;
            nt_challenge_response_length = 0;

            std::memcpy(pos,
                        nt_proof_str,
                        nt_proof_str_length);
            nt_challenge_response_length += nt_proof_str_length;

            std::memcpy(pos+nt_proof_str_length,
                        temp,
                        temp_length);
            nt_challenge_response_length += temp_length;

#ifdef _DEBUG
//          std::printf("nt_challenge_response: %d\n", nt_challenge_response_length);
//          print_bytes((unsigned char *)nt_challenge_response, nt_challenge_response_length);
#endif


            // LmChallengeResponse
            // ConcatenationOf(CHALLENGE_MESSAGE.ServerChallenge, ClientChallenge)
            pos = (unsigned char *)server_challenge_client_challenge;

            std::memcpy(pos,
                        server_challenge,
                        8);
            std::memcpy(pos+8,
                        client_challenge,
                        8);

#ifdef _DEBUG
//          std::printf("server_challenge_client_challenge: %d\n", 16);
//          print_bytes((unsigned char *)server_challenge_client_challenge, 16);
#endif

            // HMAC_MD5(ResponseKeyLM, ConcatenationOf(CHALLENGE_MESSAGE.ServerChallenge, ClientChallenge))
            ret = get_hmac_md5((unsigned char *)server_challenge_client_challenge,
                               16,
                               (unsigned char *)response_key_lm,
                               response_key_lm_length,
                               (unsigned char *)tmp2,
                               16);
            if(ret < 0)
            {
#ifdef _DEBUG
                std::printf("[-] generate_response_ntlmv2 get_hmac_md5 error\n");
#endif
                goto error;
            }
            tmp2_length = ret;

#ifdef _DEBUG
//          std::printf("tmp2: %d\n", tmp2_length);
//          print_bytes((unsigned char *)tmp2, tmp2_length);
#endif

            // ConcatenationOf(HMAC_MD5(ResponseKeyLM, ConcatenationOf(CHALLENGE_MESSAGE.ServerChallenge, ClientChallenge)), ClientChallenge)
            pos = (unsigned char *)lm_challenge_response;
            lm_challenge_response_length = 0;

            std::memcpy(pos,
                        tmp2,
                        tmp2_length);
            lm_challenge_response_length += tmp2_length;

            std::memcpy(pos+tmp2_length,
                        client_challenge,
                        8);
            lm_challenge_response_length += 8;

#ifdef _DEBUG
//          std::printf("lm_challenge_response: %d\n", lm_challenge_response_length);
//          print_bytes((unsigned char *)lm_challenge_response, lm_challenge_response_length);
#endif


            // SessionBaseKey
            // HMAC_MD5(ResponseKeyNT, NTProofStr)
            ret = get_hmac_md5((unsigned char *)nt_proof_str,
                               nt_proof_str_length,
                               (unsigned char *)response_key_nt,
                               response_key_nt_length,
                               (unsigned char *)session_base_key,
                               16);
            if(ret < 0)
            {
#ifdef _DEBUG
                std::printf("[-] generate_response_ntlmv2 get_hmac_md5 error\n");
#endif
                goto error;
            }
            session_base_key_length = ret;

#ifdef _DEBUG
//          std::printf("session_base_key: %d\n", session_base_key_length);
//          print_bytes((unsigned char *)session_base_key, session_base_key_length);
#endif


            // authenticate_message
            pos = (unsigned char *)authenticate_message;
            authenticate_message_length = 0;
            offset = 0x40;  // start buffer offset

            // authenticate_message Signature
            std::memcpy(&authenticate_message->signature,
                        "NTLMSSP\0",
                        8);

            // authenticate_message MessageType
            authenticate_message->message_type = NtLmAuthenticate;

            // authenticate_message LmChallengeResponseFields
            authenticate_message->lm_challenge_response_fields.lm_challenge_response_len = lm_challenge_response_length;
            authenticate_message->lm_challenge_response_fields.lm_challenge_response_max_len = lm_challenge_response_length;
            authenticate_message->lm_challenge_response_fields.lm_challenge_response_buffer_offset = 0x40;

            std::memcpy(pos+offset,
                        lm_challenge_response,
                        lm_challenge_response_length);
            offset += lm_challenge_response_length;

            // authenticate_message NtChallengeResponseFields
            authenticate_message->nt_challenge_response_fields.nt_challenge_response_len = nt_challenge_response_length;
            authenticate_message->nt_challenge_response_fields.nt_challenge_response_max_len = nt_challenge_response_length;
            authenticate_message->nt_challenge_response_fields.nt_challenge_response_buffer_offset = offset;

            std::memcpy(pos+offset,
                        nt_challenge_response,
                        nt_challenge_response_length);
            offset += nt_challenge_response_length;

            // authenticate_message DomainNameFields
            authenticate_message->domain_name_fields.domain_name_len = domainname_length;
            authenticate_message->domain_name_fields.domain_name_max_len = domainname_length;
            authenticate_message->domain_name_fields.domain_name_buffer_offset = offset;

            std::memcpy(pos+offset,
                        domainname,
                        domainname_length);
            offset += domainname_length;

            // authenticate_message UserNameFields
            authenticate_message->user_name_fields.user_name_len = username_length;
            authenticate_message->user_name_fields.user_name_max_len = username_length;
            authenticate_message->user_name_fields.user_name_buffer_offset = offset;

            std::memcpy(pos+offset,
                        username,
                        username_length);
            offset += username_length;

            // authenticate_message WorkstationFields
            authenticate_message->workstation_fields.workstation_len = workstationname_length;
            authenticate_message->workstation_fields.workstation_max_len = workstationname_length;
            authenticate_message->workstation_fields.workstation_buffer_offset = offset;

            std::memcpy(pos+offset,
                        workstationname,
                        workstationname_length);
            offset += workstationname_length;

            // authenticate_message EncryptedRandomSessionKeyFields
            authenticate_message->encrypted_random_session_key_fields.encrypted_random_session_key_len = 0;
            authenticate_message->encrypted_random_session_key_fields.encrypted_random_session_key_max_len = 0;
            authenticate_message->encrypted_random_session_key_fields.encrypted_random_session_key_buffer_offset = 0;

            authenticate_message_length = offset;

            // authenticate_message NegotiateFlags
            authenticate_message->negotiate_flags.negotiate_unicode                  = 0;
            authenticate_message->negotiate_flags.negotiate_oem                      = 1;
            authenticate_message->negotiate_flags.request_target                     = 1;
            authenticate_message->negotiate_flags.request_0x00000008                 = 0;
            authenticate_message->negotiate_flags.negotiate_sign                     = 0;
            authenticate_message->negotiate_flags.negotiate_seal                     = 0;
            authenticate_message->negotiate_flags.negotiate_datagram                 = 0;
            authenticate_message->negotiate_flags.negotiate_lan_manager_key          = 0;
            authenticate_message->negotiate_flags.negotiate_0x00000100               = 0;
            authenticate_message->negotiate_flags.negotiate_ntlm_key                 = 1;
            authenticate_message->negotiate_flags.negotiate_nt_only                  = 0;
            authenticate_message->negotiate_flags.negotiate_anonymous                = 0;
            authenticate_message->negotiate_flags.negotiate_oem_domain_supplied      = 0;
            authenticate_message->negotiate_flags.negotiate_oem_workstation_supplied = 0;
            authenticate_message->negotiate_flags.negotiate_0x00004000               = 0;
            authenticate_message->negotiate_flags.negotiate_always_sign              = 1;
            authenticate_message->negotiate_flags.target_type_domain                 = 1;
            authenticate_message->negotiate_flags.target_type_server                 = 0;
            authenticate_message->negotiate_flags.target_type_share                  = 0;
            authenticate_message->negotiate_flags.negotiate_extended_security        = 1;
            authenticate_message->negotiate_flags.negotiate_identify                 = 0;
            authenticate_message->negotiate_flags.negotiate_0x00200000               = 0;
            authenticate_message->negotiate_flags.request_non_nt_session             = 0;
            authenticate_message->negotiate_flags.negotiate_target_info              = 1;
            authenticate_message->negotiate_flags.negotiate_0x01000000               = 0;
            authenticate_message->negotiate_flags.negotiate_version                  = 1;
            authenticate_message->negotiate_flags.negotiate_0x04000000               = 0;
            authenticate_message->negotiate_flags.negotiate_0x08000000               = 0;
            authenticate_message->negotiate_flags.negotiate_0x10000000               = 0;
            authenticate_message->negotiate_flags.negotiate_128                      = 0;
            authenticate_message->negotiate_flags.negotiate_key_exchange             = 0;
            authenticate_message->negotiate_flags.negotiate_56                       = 0;

#ifdef _DEBUG
//          std::printf("authenticate_message: %d\n", authenticate_message_length);
//          print_bytes((unsigned char *)authenticate_message, authenticate_message_length);
#endif
        }

        free(response_key_nt);
        free(response_key_lm);
        free(server_challenge);
        free(client_challenge);
        free(server_name);
        free(temp);
        free(nt_proof_str);
        free(tmp1);
        free(nt_challenge_response);
        free(lm_challenge_response);
        free(server_challenge_client_challenge);
        free(tmp2);
        free(session_base_key);
        return authenticate_message_length;

error:
        free(response_key_nt);
        free(response_key_lm);
        free(server_challenge);
        free(client_challenge);
        free(server_name);
        free(temp);
        free(nt_proof_str);
        free(tmp1);
        free(nt_challenge_response);
        free(lm_challenge_response);
        free(server_challenge_client_challenge);
        free(tmp2);
        free(session_base_key);
        return -1;
    }

    int Ntlmv2::generate_base64_authenticate_message(char *http_header_data,
                                                     char *ntlm_b64,
                                                     int32_t ntlm_b64_size)
    {
        int ret = 0;
        int ntlm_challenge_message_length = 0;
        int ntlm_authenticate_message_length = 0;
        int length = 0;
        char *pos = NULL;
        const char * ntlm_http_header_key = "Proxy-Authenticate:";
        char *ntlm_http_header_data = (char *)calloc(NODE_BUFFER_SIZE,
                                                     sizeof(char));
        char *ntlm_challenge_message = (char *)calloc(NODE_BUFFER_SIZE,
                                                      sizeof(char));
        char *ntlm = (char *)calloc(NODE_BUFFER_SIZE,
                                    sizeof(char));

        struct challenge_message *challenge_message = NULL;
        struct authenticate_message *authenticate_message = NULL;


        // challenge message
        ret = get_http_header((const char *)http_header_data,
                              (const char *)ntlm_http_header_key,
                              (char *)ntlm_http_header_data,
                              NODE_BUFFER_SIZE);
        if(ret < 0)
        {
#ifdef _DEBUG
            std::printf("[-] generate_base64_authenticate_message get_http_header error\n");
#endif
            goto error;
        }

        pos = strstr((char *)ntlm_http_header_data,
                     "Proxy-Authenticate: NTLM ");
        if(pos == NULL)
        {
#ifdef _DEBUG
            std::printf("[-] generate_base64_authenticate_message Proxy-Authenticate: NTLM error\n");
#endif
            goto error;
        }

        pos += strlen("Proxy-Authenticate: NTLM ");
        length = strlen(pos);
        ret = decode_base64((const unsigned char *)pos,
                            length,
                            (unsigned char *)ntlm_challenge_message,
                            NODE_BUFFER_SIZE);
        if(ret < 0)
        {
#ifdef _DEBUG
            std::printf("[-] generate_base64_authenticate_message decode_base64 error\n");
#endif
            goto error;
        }

        challenge_message = (struct challenge_message *)ntlm_challenge_message;
        if(challenge_message->message_type != NtLmChallenge)
        {
#ifdef _DEBUG
            std::printf("[-] generate_base64_authenticate_message ntlm challenge message message_type error: %04x\n",
                        challenge_message->message_type);
#endif
            goto error;
        }


        // authenticate_message
        authenticate_message = (struct authenticate_message *)ntlm;
        ntlm_authenticate_message_length = 0;

        ret = generate_response_ntlmv2(challenge_message,
                                       authenticate_message);
        if(ret < 0)
        {
#ifdef _DEBUG
            std::printf("[-] generate_base64_authenticate_message generate_response_ntlmv2 error\n");
#endif
            goto error;
        }
        ntlm_authenticate_message_length = ret;

        ret = encode_base64((const unsigned char *)authenticate_message,
                            ntlm_authenticate_message_length,
                            (unsigned char *)ntlm_b64,
                            ntlm_b64_size);
        if(ret < 0)
        {
#ifdef _DEBUG
            std::printf("[-] generate_base64_authenticate_message encode_base64 error\n");
#endif
            goto error;
        }

        free(ntlm_http_header_data);
        free(ntlm_challenge_message);
        free(ntlm);
        return 0;

error:
        free(ntlm_http_header_data);
        free(ntlm_challenge_message);
        free(ntlm);
        return -1;
    }
}

