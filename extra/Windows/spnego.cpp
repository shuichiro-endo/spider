/*
 * Title:  spider spnego.cpp (Windows)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "spnego.hpp"


namespace spider
{
    Spnego::Spnego(std::string service_principal_name)
    {
        this->service_principal_name = service_principal_name;
    }

    Spnego::~Spnego()
    {

    }

    void Spnego::set_service_principal_name(std::string service_principal_name)
    {
        this->service_principal_name = service_principal_name;
    }

    std::string Spnego::get_service_principal_name()
    {
        return service_principal_name;
    }

    int Spnego::encode_base64(const unsigned char *input,
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

    int Spnego::get_http_header(const char *input,
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

    int Spnego::generate_base64_kerberos_token(char *b64_kerberos_token,
                                               int32_t b64_kerberos_token_size)
    {
        SECURITY_STATUS security_status;
        char pname[] = "Kerberos";
        LPSTR package_name = (LPSTR)&pname;
        CredHandle cred_handle;
        TimeStamp life_time;
        PBYTE p_in_buf = NULL;
        PBYTE p_out_buf = NULL;
        SecBufferDesc out_buff_desc;
        SecBuffer out_sec_buff;
        SecBufferDesc in_buffer_desc;
        SecBuffer in_sec_buff;
        ULONG f_context_req = ISC_REQ_REPLAY_DETECT | ISC_REQ_SEQUENCE_DETECT | ISC_REQ_MUTUAL_AUTH;
        struct _SecHandle context;
        ULONG context_attributes;
        int ret = 0;
        int b64_kerberos_token_length = 0;


        security_status = AcquireCredentialsHandleA(NULL,
                                                    package_name,
                                                    SECPKG_CRED_OUTBOUND,
                                                    NULL,
                                                    NULL,
                                                    NULL,
                                                    NULL,
                                                    &cred_handle,
                                                    &life_time);
        if(!(SEC_SUCCESS(security_status)))
        {
#ifdef DEBUGPRINT
            std::printf("[-] generate_base64_kerberos_token AcquireCredentialsHandleA error: %d\n",
                        security_status);
#endif
            return -1;
        }

        p_in_buf = (PBYTE)calloc(CB_MAX_SIZE,
                                 sizeof(unsigned char));
        p_out_buf = (PBYTE)calloc(CB_MAX_SIZE,
                                  sizeof(unsigned char));

        out_buff_desc.ulVersion = 0;
        out_buff_desc.cBuffers = 1;
        out_buff_desc.pBuffers = &out_sec_buff;
        out_sec_buff.cbBuffer = CB_MAX_SIZE;
        out_sec_buff.BufferType = SECBUFFER_TOKEN;
        out_sec_buff.pvBuffer = p_out_buf;

        security_status = InitializeSecurityContextA(&cred_handle,
                                                     NULL,
                                                     (SEC_CHAR *)service_principal_name.c_str(),
                                                     f_context_req,
                                                     0,
                                                     SECURITY_NATIVE_DREP,
                                                     NULL,
                                                     0,
                                                     &context,
                                                     &out_buff_desc,
                                                     &context_attributes,
                                                     &life_time);
        if(!SEC_SUCCESS(security_status))
        {
#ifdef DEBUGPRINT
            std::printf("[-] generate_base64_kerberos_token InitializeSecurityContext error: %d\n",
                        security_status);
#endif
            goto error;
        }

        if(security_status == SEC_I_COMPLETE_AND_CONTINUE ||
           security_status == SEC_I_COMPLETE_NEEDED)
        {
            security_status = CompleteAuthToken(&context,
                                                &out_buff_desc);
            if(!SEC_SUCCESS(security_status))
            {
#ifdef DEBUGPRINT
                std::printf("[-] generate_base64_kerberos_token CompleteAuthToken error: %d\n",
                            security_status);
#endif
                goto error;
            }
        }

        if(out_sec_buff.cbBuffer != 0)
        {
            ret = encode_base64((const unsigned char *)out_sec_buff.pvBuffer,
                                (int)out_sec_buff.cbBuffer,
                                (unsigned char *)b64_kerberos_token,
                                b64_kerberos_token_size);
            if(ret == -1)
            {
#ifdef DEBUGPRINT
                std::printf("[-] generate_base64_kerberos_token encode_base64 error\n");
#endif
                goto error;
            }
            b64_kerberos_token_length = ret;
        }

        free(p_out_buf);
        free(p_in_buf);
        return b64_kerberos_token_length;

error:
        free(p_out_buf);
        free(p_in_buf);
        return -1;
    }

    int Spnego::check_http_header(char *http_data)
    {
        int ret = 0;
        const char *spnego_http_header_key = "Proxy-Authenticate:";
        char *pos = NULL;
        char *http_header_data = (char *)calloc(NODE_BUFFER_SIZE,
                                                sizeof(char));


        ret = get_http_header((const char *)http_data,
                              (const char *)spnego_http_header_key,
                              (char *)http_header_data,
                              NODE_BUFFER_SIZE);
        if(ret < 0)
        {
#ifdef DEBUGPRINT
            std::printf("[-] check_http_header get_http_header error\n");
#endif
            goto error;
        }

        pos = strstr(http_header_data,
                     "Proxy-Authenticate: Negotiate");
        if(pos == NULL)
        {
#ifdef DEBUGPRINT
            std::printf("[-] check_http_header strstr error\n");
#endif
            goto error;
        }

        free(http_header_data);
        return 0;

error:
        free(http_header_data);
        return -1;
    }
}

