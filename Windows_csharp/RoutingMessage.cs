/*
 * Title:  spider RoutingMessage.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;
using System.Net;
using System.Runtime.InteropServices;

namespace spider
{
    public class RoutingMessage : Message
    {
        private uint pipeId;


        public RoutingMessage()
        {
            this.pipeId = 0;
            this.messageType = 'r';
            this.dataSize = 0;
            this.data = new byte[SPIDER_MESSAGE_DATA_MAX_SIZE];
        }

        public RoutingMessage(uint pipeId,
                              byte[] array)
        {
            this.pipeId = pipeId;
            try
            {
                this.messageType = (char)array[0];
                this.dataSize = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(array, 136));

                if(this.dataSize > 0)
                {
                    this.data = new byte[SPIDER_MESSAGE_DATA_MAX_SIZE];
                }

                if(this.dataSize > 0 &&
                   this.dataSize <= SPIDER_MESSAGE_DATA_MAX_SIZE)
                {
                    for(int i = 0; i < this.dataSize; i++)
                    {
                        this.data[i] = array[144 + i];
                    }
                }else if(this.dataSize > SPIDER_MESSAGE_DATA_MAX_SIZE)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] routing message data size error: {0}",
                                      this.dataSize);
#endif
                    this.dataSize = SPIDER_MESSAGE_DATA_MAX_SIZE;
                    for(int i = 0; i < SPIDER_MESSAGE_DATA_MAX_SIZE; i++)
                    {
                        this.data[i] = array[144 + i];
                    }
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        ~RoutingMessage()
        {

        }

        public uint PipeId
        {
            get { return pipeId; }
            set { pipeId = value; }
        }

        public ushort NetworkToHostOrderUShort(ushort value)
        {
            if(BitConverter.IsLittleEndian)
            {
                byte[] bytes = BitConverter.GetBytes(value);
                Array.Reverse(bytes);
                return BitConverter.ToUInt16(bytes, 0);
            }

            return value;
        }

        public ushort HostToNetworkOrderUShort(ushort value)
        {
            if(BitConverter.IsLittleEndian)
            {
                byte[] bytes = BitConverter.GetBytes(value);
                Array.Reverse(bytes);
                return BitConverter.ToUInt16(bytes, 0);
            }

            return value;
        }

        public override int CopyToBuffer(ref byte[] buffer)
        {
            int length = 0;

            buffer[0] = (byte)this.messageType;
            BitConverter.GetBytes(IPAddress.HostToNetworkOrder(this.dataSize)).CopyTo(buffer, 136);
            for(int i = 0; i < this.dataSize; i++)
            {
                buffer[144 + i] = this.data[i];
            }

            length = SpiderMessageHeader.SPIDER_MESSAGE_HEADER_SIZE
                   + this.dataSize;

            return length;
        }
    }
}

