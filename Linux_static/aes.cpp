/*
 * Title:  spider aes.cpp (Linux static)
 * Author: Shuichiro Endo
 */

/*
 * Reference:
 * https://github.com/kokke/tiny-AES-c
 */

#include "spider.hpp"
#include "encryption.hpp"
#include "aes.hpp"


namespace spider
{
    Aes::Aes(bool flag,
             std::string aes_key_hex_string,
             std::string aes_iv_hex_string)
    : Encryption(flag)
    {
        this->aes_key_hex_string = aes_key_hex_string;
        this->aes_key_hex_string_size = aes_key_hex_string.size();
        this->aes_iv_hex_string = aes_iv_hex_string;
        this->aes_iv_hex_string_size = aes_iv_hex_string.size();

        this->key = (unsigned char *)calloc(AES_KEY_LEN,
                                            sizeof(unsigned char));

        this->iv = (unsigned char *)calloc(AES_BLOCK_LEN,
                                           sizeof(unsigned char));

        this->round_key = (unsigned char *)calloc(AES_KEY_EXP_SIZE,
                                                  sizeof(unsigned char));

        if(flag == true
           && this->aes_key_hex_string_size == AES_KEY_LEN * 2
           && this->aes_iv_hex_string_size == AES_BLOCK_LEN * 2)
        {
            hex_string_to_array(this->aes_key_hex_string.c_str(),
                                this->aes_key_hex_string_size,
                                this->key,
                                AES_KEY_LEN);

            hex_string_to_array(this->aes_iv_hex_string.c_str(),
                                this->aes_iv_hex_string_size,
                                this->iv,
                                AES_BLOCK_LEN);

            aes_init(round_key,
                     key);
        }else
        {
            std::printf("[-] aes key or iv size error: key(%d):%d iv(%d):%d\n",
                        AES_KEY_LEN * 2,
                        aes_key_hex_string_size,
                        AES_BLOCK_LEN * 2,
                        aes_iv_hex_string_size);

            this->flag = false;
        }
    }

    Aes::~Aes()
    {
        free(key);
        free(iv);
        free(round_key);
    }

    void Aes::set_aes_key_hex_string(std::string aes_key_hex_string)
    {
        this->aes_key_hex_string = aes_key_hex_string;
    }

    std::string Aes::get_aes_key_hex_string()
    {
        return aes_key_hex_string;
    }

    void Aes::set_aes_iv_hex_string(std::string aes_iv_hex_string)
    {
        this->aes_iv_hex_string = aes_iv_hex_string;
    }

    std::string Aes::get_aes_iv_hex_string()
    {
        return aes_iv_hex_string;
    }

    void Aes::key_expansion(unsigned char *round_key,
                            const unsigned char *key)
    {
        uint8_t i, j, k;
        unsigned char tmp[4];


        for(i = 0; i < Nk; ++i)
        {
            round_key[(i * 4) + 0] = key[(i * 4) + 0];
            round_key[(i * 4) + 1] = key[(i * 4) + 1];
            round_key[(i * 4) + 2] = key[(i * 4) + 2];
            round_key[(i * 4) + 3] = key[(i * 4) + 3];
        }

        for(i = Nk; i< Nb * (Nr + 1); ++i)
        {
            k = (i - 1) * 4;
            tmp[0] = round_key[k + 0];
            tmp[1] = round_key[k + 1];
            tmp[2] = round_key[k + 2];
            tmp[3] = round_key[k + 3];

            if(i % Nk == 0)
            {
                const unsigned char u8_tmp = tmp[0];
                tmp[0] = tmp[1];
                tmp[1] = tmp[2];
                tmp[2] = tmp[3];
                tmp[3] = u8_tmp;

                tmp[0] = get_sbox_value(tmp[0]);
                tmp[1] = get_sbox_value(tmp[1]);
                tmp[2] = get_sbox_value(tmp[2]);
                tmp[3] = get_sbox_value(tmp[3]);

                tmp[0] = tmp[0] ^ rcon[i / Nk];
            }

            if(i % Nk == 4)
            {
                tmp[0] = get_sbox_value(tmp[0]);
                tmp[1] = get_sbox_value(tmp[1]);
                tmp[2] = get_sbox_value(tmp[2]);
                tmp[3] = get_sbox_value(tmp[3]);
            }

            j = i * 4;
            k = (i - Nk) * 4;

            round_key[j + 0] = round_key[k + 0] ^ tmp[0];
            round_key[j + 1] = round_key[k + 1] ^ tmp[1];
            round_key[j + 2] = round_key[k + 2] ^ tmp[2];
            round_key[j + 3] = round_key[k + 3] ^ tmp[3];
        }
    }

    void Aes::aes_init(unsigned char *round_key,
                       const unsigned char *key)
    {
        key_expansion(round_key,
                      key);
    }

    unsigned char Aes::get_sbox_value(uint8_t num) const
    {
        return sbox[num];
    }

    void Aes::add_round_key(uint8_t round,
                            state_t *state,
                            const unsigned char *round_key) const
    {
        uint8_t i, j;


        for(i = 0; i < 4; ++i)
        {
            for(j = 0; j < 4; ++j)
            {
                (*state)[i][j] ^= round_key[(round * Nb * 4) + (i * Nb) + j];
            }
        }
    }

    void Aes::sub_bytes(state_t *state) const
    {
        uint8_t i, j;


        for(i = 0; i < 4; ++i)
        {
            for(j = 0; j < 4; ++j)
            {
                (*state)[j][i] = get_sbox_value((*state)[j][i]);
            }
        }
    }

    void Aes::shift_rows(state_t* state) const
    {
        unsigned char tmp;


        tmp            = (*state)[0][1];
        (*state)[0][1] = (*state)[1][1];
        (*state)[1][1] = (*state)[2][1];
        (*state)[2][1] = (*state)[3][1];
        (*state)[3][1] = tmp;

        tmp            = (*state)[0][2];
        (*state)[0][2] = (*state)[2][2];
        (*state)[2][2] = tmp;

        tmp            = (*state)[1][2];
        (*state)[1][2] = (*state)[3][2];
        (*state)[3][2] = tmp;

        tmp            = (*state)[0][3];
        (*state)[0][3] = (*state)[3][3];
        (*state)[3][3] = (*state)[2][3];
        (*state)[2][3] = (*state)[1][3];
        (*state)[1][3] = tmp;
    }

    unsigned char Aes::xtime(unsigned char x) const
    {
        return ((x << 1) ^ (((x >> 7) & 1) * 0x1b));
    }

    void Aes::mix_columns(state_t *state) const
    {
        uint8_t i;
        unsigned char t1, t2, t3;


        for(i = 0; i < 4; ++i)
        {
            t1 = (*state)[i][0];
            t2 = (*state)[i][0] ^ (*state)[i][1] ^ (*state)[i][2] ^ (*state)[i][3];

            t3 = (*state)[i][0] ^ (*state)[i][1];
            t3 = xtime(t3);
            (*state)[i][0] ^= t3 ^ t2;

            t3 = (*state)[i][1] ^ (*state)[i][2];
            t3 = xtime(t3);
            (*state)[i][1] ^= t3 ^ t2;

            t3 = (*state)[i][2] ^ (*state)[i][3];
            t3 = xtime(t3);
            (*state)[i][2] ^= t3 ^ t2;

            t3 = (*state)[i][3] ^ t1;
            t3 = xtime(t3);
            (*state)[i][3] ^= t3 ^ t2;
        }
    }

    unsigned char Aes::multiply(unsigned char x,
                                unsigned char y) const
    {
        return (((y & 1) * x) ^
                ((y >> 1 & 1) * xtime(x)) ^
                ((y >> 2 & 1) * xtime(xtime(x))) ^
                ((y >> 3 & 1) * xtime(xtime(xtime(x)))) ^
                ((y >> 4 & 1) * xtime(xtime(xtime(xtime(x))))));
    }

    unsigned char Aes::get_sbox_invert(uint8_t num) const
    {
        return rsbox[num];
    }

    void Aes::inv_mix_columns(state_t *state) const
    {
        uint8_t i;
        unsigned char a, b, c, d;


        for(i = 0; i < 4; ++i)
        {
            a = (*state)[i][0];
            b = (*state)[i][1];
            c = (*state)[i][2];
            d = (*state)[i][3];

            (*state)[i][0] = multiply(a, 0x0e) ^ multiply(b, 0x0b) ^ multiply(c, 0x0d) ^ multiply(d, 0x09);
            (*state)[i][1] = multiply(a, 0x09) ^ multiply(b, 0x0e) ^ multiply(c, 0x0b) ^ multiply(d, 0x0d);
            (*state)[i][2] = multiply(a, 0x0d) ^ multiply(b, 0x09) ^ multiply(c, 0x0e) ^ multiply(d, 0x0b);
            (*state)[i][3] = multiply(a, 0x0b) ^ multiply(b, 0x0d) ^ multiply(c, 0x09) ^ multiply(d, 0x0e);
        }
    }

    void Aes::inv_sub_bytes(state_t *state) const
    {
        uint8_t i, j;


        for(i = 0; i < 4; ++i)
        {
            for(j = 0; j < 4; ++j)
            {
                (*state)[j][i] = get_sbox_invert((*state)[j][i]);
            }
        }
    }

    void Aes::inv_shift_rows(state_t *state) const
    {
        unsigned char tmp;


        tmp            = (*state)[3][1];
        (*state)[3][1] = (*state)[2][1];
        (*state)[2][1] = (*state)[1][1];
        (*state)[1][1] = (*state)[0][1];
        (*state)[0][1] = tmp;

        tmp            = (*state)[0][2];
        (*state)[0][2] = (*state)[2][2];
        (*state)[2][2] = tmp;

        tmp            = (*state)[1][2];
        (*state)[1][2] = (*state)[3][2];
        (*state)[3][2] = tmp;

        tmp            = (*state)[0][3];
        (*state)[0][3] = (*state)[1][3];
        (*state)[1][3] = (*state)[2][3];
        (*state)[2][3] = (*state)[3][3];
        (*state)[3][3] = tmp;
    }

    void Aes::cipher(state_t *state,
                     const unsigned char *round_key) const
    {
        uint8_t round = 0;


        add_round_key(0,
                      state,
                      round_key);

        for(round = 1; ; ++round)
        {
            sub_bytes(state);
            shift_rows(state);

            if(round == Nr)
            {
                break;
            }

            mix_columns(state);
            add_round_key(round,
                          state,
                          round_key);
        }

        add_round_key(Nr, state, round_key);
    }

    void Aes::inv_cipher(state_t *state,
                         const unsigned char *round_key) const
    {
        uint8_t round = 0;


        add_round_key(Nr,
                      state,
                      round_key);

        for(round = (Nr - 1); ; --round)
        {
            inv_shift_rows(state);
            inv_sub_bytes(state);
            add_round_key(round,
                          state,
                          round_key);

            if(round == 0)
            {
                break;
            }

            inv_mix_columns(state);
        }
    }

    void Aes::xor_with_iv(unsigned char *buf,
                          const unsigned char *iv) const
    {
        uint8_t i;


        for(i = 0; i < AES_BLOCK_LEN; ++i)
        {
            buf[i] ^= iv[i];
        }
    }

    int32_t Aes::add_padding(unsigned char *data,
                             int32_t data_size) const
    {
        int32_t i;
        unsigned char pad[AES_BLOCK_LEN];
        int32_t p;


        for(i = 0; i < AES_BLOCK_LEN; i++)
        {
            pad[i] = AES_BLOCK_LEN - i;
        }

        p = data_size % AES_BLOCK_LEN;
        if(p == 0)
        {
            for(i = 0; i <= AES_BLOCK_LEN; i++)
            {
                *(data + data_size + i) = pad[0];
            }

            data_size += AES_BLOCK_LEN;
        }else
        {
            for(i = 0; i <= AES_BLOCK_LEN - p; i++)
            {
                *(data+data_size + i) = pad[p];
            }

            data_size += pad[p];
        }

        return data_size;
    }

    int32_t Aes::delete_padding(unsigned char *data,
                                int32_t data_size) const
    {
        int32_t i;
        unsigned char pad = *(data + data_size - 1);


        for(i = pad; i >= 0; i--)
        {
            *(data + data_size - pad) = 0x0;
        }

        data_size -= pad;

        return data_size;
    }

    int32_t Aes::encrypt(char *data,
                         int32_t data_size,
                         int32_t buffer_size) const
    {
        int32_t i;
        unsigned char *iv_tmp = iv;


        if(data_size + AES_BLOCK_LEN > buffer_size)
        {
#ifdef _DEBUG
            std::printf("[-] aes encrypt buffer size error: %d\n", data_size);
#endif
            return -1;
        }

#ifdef _DEBUG
//        print_bytes(data, data_size);
#endif

        data_size = add_padding((unsigned char *)data,
                                data_size);

        for(i = 0; i < data_size; i += AES_BLOCK_LEN)
        {
            xor_with_iv((unsigned char *)data,
                        iv_tmp);

            cipher((state_t *)data,
                   round_key);

            iv_tmp = (unsigned char *)data;
            data += AES_BLOCK_LEN;
        }

        return data_size;
    }

    int32_t Aes::decrypt(char *data,
                         int32_t data_size,
                         int32_t buffer_size) const
    {
        int32_t i;
        unsigned char *p;
        unsigned char *iv_tmp;


        if(data_size % AES_BLOCK_LEN)
        {
#ifdef _DEBUG
            std::printf("[-] aes decrypt data size error: %d\n", data_size);
#endif
            return -1;
        }

        p = (unsigned char *)data + data_size - AES_BLOCK_LEN;
        iv_tmp = (unsigned char *)data + data_size - AES_BLOCK_LEN * 2;

        for(i = data_size - AES_BLOCK_LEN; i >= 0; i -= AES_BLOCK_LEN)
        {
            inv_cipher((state_t *)p,
                       round_key);

            if(p == (unsigned char *)data)
            {
                xor_with_iv((unsigned char *)p,
                            iv);
            }else
            {
                xor_with_iv((unsigned char *)p,
                            iv_tmp);
            }

            p -= AES_BLOCK_LEN;
            iv_tmp -= AES_BLOCK_LEN;
        }

        data_size = delete_padding((unsigned char *)data,
                                   data_size);

#ifdef _DEBUG
//        print_bytes(data, data_size);
#endif

        return data_size;
    }
}

