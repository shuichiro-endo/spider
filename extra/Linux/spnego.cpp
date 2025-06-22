/*
 * Title:  spider spnego.cpp (Linux)
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




    int Spnego::display_gss_error(OM_uint32 status_value,
                                  int status_type,
                                  gss_OID mechanism_type,
                                  char *buffer,
                                  int buffer_size)
    {
        OM_uint32 major_status;
        OM_uint32 minor_status;
        OM_uint32 message_context = 0;
        gss_buffer_desc status_string = GSS_C_EMPTY_BUFFER;
        int length = 0;


        do
        {
            major_status = gss_display_status(&minor_status,
                                              status_value,
                                              status_type,
                                              mechanism_type,
                                              &message_context,
                                              &status_string);
            if(major_status == GSS_S_COMPLETE &&
               status_string.length > 0)
            {
                if(buffer_size > length + status_string.length + 3)
                {
                    length += snprintf(buffer+length,
                                       buffer_size-length,
                                       "%.*s.\n",
                                       (int)status_string.length,
                                       (char *)status_string.value);
                }
            }

            gss_release_buffer(&minor_status,
                               &status_string);

        }while(!GSS_ERROR(major_status) && message_context);

        return length;
    }


    gss_name_t Spnego::get_spn(char *spn)
    {
        int ret = 0;
        OM_uint32 major_status;
        OM_uint32 minor_status;
        gss_buffer_desc spn_token = GSS_C_EMPTY_BUFFER;
        gss_name_t spn_gss_name = GSS_C_NO_NAME;
        char *error_buffer = (char *)calloc(1000,
                                            sizeof(char));


        spn_token.value = spn;
        spn_token.length = strlen((char *)spn_token.value) + 1;

        major_status = gss_import_name(&minor_status,
                                       &spn_token,
                                       GSS_KRB5_NT_PRINCIPAL_NAME,
                                       &spn_gss_name);
        if(GSS_ERROR(major_status))
        {
#ifdef _DEBUG
            ret = display_gss_error(minor_status,
                                    GSS_C_GSS_CODE,
                                    GSS_C_NO_OID,
                                    (char *)error_buffer,
                                    1000);
            std::printf("[-] get_spn gss_import_name error:\n%s",
                        error_buffer);
#endif
            goto error;
        }

        free(error_buffer);
        return spn_gss_name;

error:
        free(error_buffer);
        return GSS_C_NO_NAME;
    }

    int Spnego::generate_base64_kerberos_token(char *b64_kerberos_token,
                                               int32_t b64_kerberos_token_size)
    {
        OM_uint32 major_status;
        OM_uint32 minor_status;
        gss_OID_desc spnego_mechanism_oid = {6, (char *)"\x2b\x06\x01\x05\x05\x02"};
        gss_OID_set_desc mechanism;
        mechanism.elements = &spnego_mechanism_oid;
        mechanism.count = 1;
        OM_uint32 req_flags = GSS_C_REPLAY_FLAG | GSS_C_SEQUENCE_FLAG | GSS_C_MUTUAL_FLAG;
        gss_ctx_id_t context = GSS_C_NO_CONTEXT;
        gss_name_t spn_gss_name = get_spn((char *)service_principal_name.c_str());
        gss_buffer_desc output_token;
        int ret = 0;
        int b64_kerberos_token_length = 0;
        char *error_buffer = (char *)calloc(1000,
                                            sizeof(char));


        major_status = gss_init_sec_context(&minor_status,
                                            GSS_C_NO_CREDENTIAL,
                                            &context,
                                            spn_gss_name,
                                            mechanism.elements,
                                            req_flags,
                                            0,
                                            GSS_C_NO_CHANNEL_BINDINGS,
                                            GSS_C_NO_BUFFER,
                                            NULL,
                                            &output_token,
                                            NULL,
                                            NULL);
        if(GSS_ERROR(major_status))
        {
#ifdef _DEBUG
            ret = display_gss_error(minor_status,
                                    GSS_C_GSS_CODE,
                                    mechanism.elements,
                                    (char *)error_buffer,
                                    1000);
            std::printf("[-] generate_base64_kerberos_token gss_init_sec_context error:\n%s",
                        error_buffer);
#endif

            if(context != GSS_C_NO_CONTEXT)
            {
                gss_delete_sec_context(&minor_status,
                                       &context,
                                       GSS_C_NO_BUFFER);
            }

            if(spn_gss_name != GSS_C_NO_NAME)
            {
                gss_release_name(&minor_status,
                                 &spn_gss_name);
            }

            gss_release_buffer(&minor_status,
                               &output_token);

            goto error;
        }else if(output_token.length != 0)
        {
            ret = encode_base64((const unsigned char *)output_token.value,
                                output_token.length,
                                (unsigned char *)b64_kerberos_token,
                                b64_kerberos_token_size);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] generate_base64_kerberos_token encode_base64 error\n");
#endif

                if(context != GSS_C_NO_CONTEXT)
                {
                    gss_delete_sec_context(&minor_status,
                                           &context,
                                           GSS_C_NO_BUFFER);
                }

                if(spn_gss_name != GSS_C_NO_NAME)
                {
                    gss_release_name(&minor_status,
                                     &spn_gss_name);
                }

                gss_release_buffer(&minor_status,
                                   &output_token);

                goto error;
            }
            b64_kerberos_token_length = ret;
        }

        if(context != GSS_C_NO_CONTEXT)
        {
            gss_delete_sec_context(&minor_status,
                                   &context,
                                   GSS_C_NO_BUFFER);
        }

        if(spn_gss_name != GSS_C_NO_NAME)
        {
            gss_release_name(&minor_status,
                             &spn_gss_name);
        }

        gss_release_buffer(&minor_status,
                           &output_token);

        free(error_buffer);
        return b64_kerberos_token_length;

error:
        free(error_buffer);
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
#ifdef _DEBUG
            std::printf("[-] check_http_header get_http_header error\n");
#endif
            goto error;
        }

        pos = strstr(http_header_data,
                     "Proxy-Authenticate: Negotiate");
        if(pos == NULL)
        {
#ifdef _DEBUG
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

