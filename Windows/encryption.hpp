/*
 * Title:  spider encryption.hpp (Windows)
 * Author: Shuichiro Endo
 */

#ifndef ENCRYPTION_H_
#define ENCRYPTION_H_

#include "spider.hpp"


namespace spider
{
    class Encryption
    {
    private:

    protected:
        BOOL flag;

    public:

    private:

    public:
        Encryption(BOOL flag);
        ~Encryption();

        void set_flag(BOOL flag);
        BOOL get_flag();

        virtual int32_t encrypt(char *data,
                                int32_t data_size,
                                int32_t buffer_size) const = 0;
        virtual int32_t decrypt(char *data,
                                int32_t data_size,
                                int32_t buffer_size) const = 0;
    };
}

#endif /* ENCRYPTION_H_ */

