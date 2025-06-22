/*
 * Title:  spider spnego.hpp (Linux)
 * Author: Shuichiro Endo
 */

#ifndef SPNEGO_H_
#define SPNEGO_H_

#include "spider.hpp"


namespace spider
{
    class Spnego
    {
    private:
        std::string service_principal_name;


    public:

    private:
        int encode_base64(const unsigned char *input,
                          int32_t input_length,
                          unsigned char *output,
                          int32_t output_size);

        int display_gss_error(OM_uint32 status_value,
                              int status_type,
                              gss_OID mechanism_type,
                              char *buffer,
                              int32_t buffer_size);

        gss_name_t get_spn(char *spn);


        int get_http_header(const char *input,
                            const char *key,
                            char *output,
                            int output_size);

    public:
        Spnego(std::string service_principal_name);

        ~Spnego();

        void set_service_principal_name(std::string service_principal_name);
        std::string get_service_principal_name();


        int generate_base64_kerberos_token(char *b64_kerberos_token,
                                           int32_t b64_kerberos_token_size);

        int check_http_header(char *http_data);

    };
}

#endif /* SPNEGO_H_ */
