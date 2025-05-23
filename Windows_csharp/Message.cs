/*
 * Title:  spider Message.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;

namespace spider
{
    public class Message
    {
        protected const int SPIDER_MESSAGE_DATA_SIZE = 65536;
        protected const int SPIDER_MESSAGE_DATA_MAX_SIZE = 65552;     // 65536 + 16 (AES padding)

        protected char messageType;
        protected int dataSize = 0;
        protected byte[] data;

        public Message()
        {

        }

        ~Message()
        {

        }

        public char MessageType
        {
            get { return messageType; }
            set { messageType = value; }
        }

        public int DataSize
        {
            get { return dataSize; }
            set { dataSize = value; }
        }

        public byte[] Data
        {
            get { return data; }
            set { data = value; }
        }

        public void PrintBytes()
        {
            for(int i = 0; i < dataSize; i++)
            {
                if(i != 0 && i % 16 == 0){
                    Console.WriteLine("");
                }else if(i % 16 == 8)
                {
                    Console.Write(" ");
                }
                Console.Write($"{data[i]:X2} ");
            }
            Console.WriteLine("");
        }

        public virtual int CopyToBuffer(ref byte[] buffer)
        {
            return 0;
        }
    }
}

