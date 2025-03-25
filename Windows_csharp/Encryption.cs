/*
 * Title:  spider Encryption.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;

namespace spider
{
    public class Encryption
    {
        protected bool flag;

        public Encryption(bool flag)
        {
            this.flag = flag;
        }

        ~Encryption()
        {

        }

        public bool Flag
        {
            get { return flag; }
            set { flag = value; }
        }

        protected byte HexByteToInt(byte c)
        {
            if((c >= 0x30) && (c <= 0x39))
            {
                c = (byte)(c - 0x30);
            }else if((c >= 0x61) && (c <= 0x66))
            {
                c = (byte)(c + (byte)0xa - (byte)0x61);
            }else if((c >= 0x41) && (c <= 0x46))
            {
                c = (byte)(c + (byte)0xa - (byte)0x41);
            }else
            {
                c = 0;
            }

            return c;
        }

        protected void HexStringToArray(byte[] hexString,
                                        int hexStringLength,
                                        ref byte[] buffer,
                                        int bufferSize)
        {
            byte tmp1 = 0;
            byte tmp2 = 0;
            int length = 0;

            for(int i = 0; i < hexStringLength && length < bufferSize; i += 2)
            {
                tmp1 = HexByteToInt(hexString[i]);
                tmp2 = HexByteToInt(hexString[i + 1]);

                tmp1 = (byte)(tmp1 << 4);
                buffer[length] = (byte)(tmp1 + tmp2);
                length++;
            }
        }

        public void PrintBytes(byte[] buffer,
                               int bufferLength)
        {
            for(int i = 0; i < bufferLength; i++)
            {
                if(i != 0 && i % 16 == 0){
                    Console.WriteLine("");
                }else if(i % 16 == 8)
                {
                    Console.Write(" ");
                }
                Console.Write($"{buffer[i]:X2} ");
            }
            Console.WriteLine("");
        }

        public virtual int Encrypt(ref byte[] data,
                                   int dataSize,
                                   int bufferSize)
        {
            return 0;
        }

        public virtual int Decrypt(ref byte[] data,
                                   int dataSize,
                                   int bufferSize)
        {
            return 0;
        }
    }
}

