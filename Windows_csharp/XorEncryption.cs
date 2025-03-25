/*
 * Title:  spider XorEncryption.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System.Text;

namespace spider
{
    public class XorEncryption : Encryption
    {
        private string xorKeyHexString;
        int xorKeyHexStringSize;
        byte[] key;
        int keyLength;

        public XorEncryption(bool flag,
                             string xorKeyHexString)
        : base(flag)
        {
            this.xorKeyHexString = xorKeyHexString;
            this.xorKeyHexStringSize = xorKeyHexString.Length;

            if(this.xorKeyHexStringSize > 0){
                if(this.xorKeyHexStringSize % 2 == 0){
                    this.keyLength = this.xorKeyHexStringSize / 2;
                }else{
                    this.keyLength = this.xorKeyHexStringSize / 2 + 1;
                }

                this.key = new byte[this.xorKeyHexStringSize];

                HexStringToArray(Encoding.UTF8.GetBytes(this.xorKeyHexString),
                                 this.xorKeyHexStringSize,
                                 ref this.key,
                                 this.keyLength);
            }else{
                this.keyLength = 0;
                this.key = null;
            }
        }

        ~XorEncryption()
        {

        }

        public string XorKeyHexString
        {
            get { return xorKeyHexString; }
            set { xorKeyHexString = value; }
        }

        public int XorKeyHexStringSize
        {
            get { return xorKeyHexStringSize; }
            set { xorKeyHexStringSize = value; }
        }

        public override int Encrypt(ref byte[] data,
                                    int dataSize,
                                    int bufferSize)
        {
            if(keyLength == 0)
            {
                return 0;
            }else if(keyLength > 0)
            {
                for(int i = 0; i < dataSize; i++)
                {
                    data[i] = (byte)(data[i] ^ key[i % keyLength]);
                }
            }else
            {
                return -1;
            }

            return dataSize;
        }

        public override int Decrypt(ref byte[] data,
                                    int dataSize,
                                    int bufferSize)
        {
            if(keyLength == 0)
            {
                return 0;
            }else if(keyLength > 0)
            {
                for(int i = 0; i < dataSize; i++)
                {
                    data[i] = (byte)(data[i] ^ key[i % keyLength]);
                }
            }else
            {
                return -1;
            }

            return dataSize;
        }
    }
}

