/*
 * Title:  spider AesEncryption.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;
using System.IO;
using System.Security.Cryptography;
using System.Text;

namespace spider
{
    public class AesEncryption : Encryption
    {
        private const int AES_BLOCK_LEN = 16;
        private const int AES_KEY_LEN = 32;

        private string aesKeyHexString;
        private int aesKeyHexStringSize;
        private string aesIvHexString;
        private int aesIvHexStringSize;
        private byte[] key;
        private byte[] iv;
        private Aes aes = null;
        ICryptoTransform encryptor = null;
        ICryptoTransform decryptor = null;

        public AesEncryption(bool flag,
                             string aesKeyHexString,
                             string aesIvHexString)
        : base(flag)
        {
            this.aesKeyHexString = aesKeyHexString;
            this.aesKeyHexStringSize = aesKeyHexString.Length;
            this.aesIvHexString = aesIvHexString;
            this.aesIvHexStringSize = aesIvHexString.Length;

            this.key = new byte[AES_KEY_LEN];
            this.iv = new byte[AES_BLOCK_LEN];

            if(flag == true &&
               this.aesKeyHexStringSize == AES_KEY_LEN * 2 &&
               this.aesIvHexStringSize == AES_BLOCK_LEN * 2)
            {
                HexStringToArray(Encoding.UTF8.GetBytes(this.aesKeyHexString),
                                 this.aesKeyHexStringSize,
                                 ref this.key,
                                 AES_KEY_LEN);

                HexStringToArray(Encoding.UTF8.GetBytes(this.aesIvHexString),
                                 this.aesIvHexStringSize,
                                 ref this.iv,
                                 AES_BLOCK_LEN);

                this.aes = Aes.Create();
                this.aes.Key = key;
                this.aes.IV = iv;
                this.aes.Mode = CipherMode.CBC;

                encryptor = aes.CreateEncryptor(aes.Key, aes.IV);
                decryptor = aes.CreateDecryptor(aes.Key, aes.IV);
            }else
            {
                Console.WriteLine("[-] aes key or iv size error: key({0}):{1} iv({2}):{3}",
                                  AES_KEY_LEN * 2,
                                  this.aesKeyHexStringSize,
                                  AES_BLOCK_LEN * 2,
                                  this.aesIvHexStringSize);

                this.flag = false;
            }
        }

        ~AesEncryption()
        {

        }

        public string AesKeyHexString
        {
            get { return aesKeyHexString; }
            set { aesKeyHexString = value; }
        }

        public int AesKeyHexStringSize
        {
            get { return aesKeyHexStringSize; }
            set { aesKeyHexStringSize = value; }
        }

        public string AesIvHexString
        {
            get { return aesIvHexString; }
            set { aesIvHexString = value; }
        }

        public int AesIvHexStringSize
        {
            get { return aesIvHexStringSize; }
            set { aesIvHexStringSize = value; }
        }

        public override int Encrypt(ref byte[] data,
                                    int dataSize,
                                    int bufferSize)
        {
            if(dataSize + AES_BLOCK_LEN > bufferSize)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] aes encrypt buffer size error: {0}",
                                  dataSize);
#endif
                return -1;
            }

#if DEBUGPRINT
//            PrintBytes(data, dataSize);
#endif

            using(var ms = new MemoryStream())
            {
                using(var cs = new CryptoStream(ms, encryptor, CryptoStreamMode.Write))
                {
                    cs.Write(data, 0, data.Length);
                    cs.FlushFinalBlock();
                }

                byte[] encryptedData = ms.ToArray();

                data = encryptedData;

                return encryptedData.Length;
            }
        }

        public override int Decrypt(ref byte[] data,
                                    int dataSize,
                                    int bufferSize)
        {
            if(dataSize % AES_BLOCK_LEN != 0)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] aes decrypt data size error: {0}",
                                  dataSize);
#endif
                return -1;
            }

            using(var ms = new MemoryStream(data))
            using(var cs = new CryptoStream(ms, decryptor, CryptoStreamMode.Read))
            using(var resultStream = new MemoryStream())
            {
                cs.CopyTo(resultStream);
                byte[] decryptedData = resultStream.ToArray();

                data = decryptedData;

#if DEBUGPRINT
//                PrintBytes(data, decryptedData.Length);
#endif

                return decryptedData.Length;
            }
        }
    }
}

