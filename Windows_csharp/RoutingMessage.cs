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
        private const int ROUTING_MESSAGE_DATA_SIZE = 60000;

        private uint pipeId;
        private ushort dataSize;
        private byte[] data;


        public RoutingMessage()
        {
            this.pipeId = 0;
            this.messageType = 'r';
            this.dataSize = 0;
            this.data = new byte[ROUTING_MESSAGE_DATA_SIZE];
        }

        public RoutingMessage(uint pipeId,
                              byte[] array)
        {
            this.pipeId = pipeId;
            try
            {
                this.messageType = (char)array[0];
                this.dataSize = NetworkToHostOrderUShort(BitConverter.ToUInt16(array, 2));
                if(dataSize <= ROUTING_MESSAGE_DATA_SIZE)
                {
                    this.data = new byte[dataSize];
                    for(int i = 0; i < (int)dataSize; i++)
                    {
                        this.data[i] = array[4 + i];
                    }
                }else
                {
                    this.data = new byte[ROUTING_MESSAGE_DATA_SIZE];
                    for(int i = 0; i < ROUTING_MESSAGE_DATA_SIZE; i++)
                    {
                        this.data[i] = array[4 + i];
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

        public ushort DataSize
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
            BitConverter.GetBytes(HostToNetworkOrderUShort(this.dataSize)).CopyTo(buffer, 2);
            for(int i = 0; i < (int)this.dataSize; i++)
            {
                buffer[4 + i] = this.data[i];
            }

            length = RoutingMessageDataHeader.ROUTING_MESSAGE_DATA_HEADER_SIZE
                   + this.dataSize;

            return length;
        }
    }
}

