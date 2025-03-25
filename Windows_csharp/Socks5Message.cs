/*
 * Title:  spider Socks5Message.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;

namespace spider
{
    public class Socks5Message : Message
    {
        private const int SOCKS5_MESSAGE_DATA_SIZE = 60000;
        private const int INET6_ADDR_STRING_LENGTH = 46;

        private uint messageId;
        private uint connectionId;
        private uint clientId;
        private uint serverId;
        private char sourceNodeType;
        private string sourceIp;
        private char destinationNodeType;
        private string destinationIp;
        private int tvSec;
        private int tvUsec;
        private int forwarderTvSec;
        private int forwarderTvUsec;
        private ushort dataSize;
        private byte[] data;

        public Socks5Message(char messageType,
                             uint messageId,
                             uint connectionId,
                             uint clientId,
                             uint serverId,
                             char sourceNodeType,
                             string sourceIp,
                             char destinationNodeType,
                             string destinationIp,
                             int tvSec,
                             int tvUsec,
                             int forwarderTvSec,
                             int forwarderTvUsec,
                             ushort dataSize,
                             byte[] data)
        {
            this.messageType = messageType;
            this.messageId = messageId;
            this.connectionId = connectionId;
            this.clientId = clientId;
            this.serverId = serverId;
            this.sourceNodeType = sourceNodeType;
            this.sourceIp = sourceIp;
            this.destinationNodeType = destinationNodeType;
            this.destinationIp = destinationIp;
            this.tvSec = tvSec;
            this.tvUsec = tvUsec;
            this.forwarderTvSec = forwarderTvSec;
            this.forwarderTvUsec = forwarderTvUsec;
            this.dataSize = dataSize;
            this.data = new byte[SOCKS5_MESSAGE_DATA_SIZE];
            if(this.dataSize <= SOCKS5_MESSAGE_DATA_SIZE)
            {
                for(int i = 0; i < this.dataSize; i++)
                {
                    this.data[i] = data[i];
                }
            }else
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socks5 message data size error: {0}",
                                  this.dataSize);
#endif
                for(int i = 0; i < SOCKS5_MESSAGE_DATA_SIZE; i++)
                {
                    this.data[i] = data[i];
                }
            }
        }

        public Socks5Message(byte[] array)
        {
            byte[] tmp;

            messageType = (char)array[0];
            messageId = (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(array, 4));
            connectionId = (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(array, 8));
            clientId = (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(array, 12));
            serverId = (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(array, 16));
            sourceNodeType = (char)array[20];

            tmp = new byte[INET6_ADDR_STRING_LENGTH + 1];
            for(int i = 0; i < INET6_ADDR_STRING_LENGTH + 1; i++)
            {
                tmp[i] = array[22 + i];
            }
            tmp = tmp.Where(b => b != 0x00).ToArray();
            sourceIp = Encoding.UTF8.GetString(tmp);
            sourceIp = sourceIp.Trim();

            destinationNodeType = (char)array[70];

            tmp = new byte[INET6_ADDR_STRING_LENGTH + 1];
            for(int i = 0; i < INET6_ADDR_STRING_LENGTH + 1; i++)
            {
                tmp[i] = array[72 + i];
            }
            tmp = tmp.Where(b => b != 0x00).ToArray();
            destinationIp = Encoding.UTF8.GetString(tmp);
            destinationIp = destinationIp.Trim();

            tvSec = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(array, 120));
            tvUsec = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(array, 124));
            forwarderTvSec = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(array, 128));
            forwarderTvUsec = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(array, 132));
            dataSize = NetworkToHostOrderUShort(BitConverter.ToUInt16(array, 136));
            if(dataSize <= SOCKS5_MESSAGE_DATA_SIZE)
            {
                this.data = new byte[dataSize];
                for(int i = 0; i < dataSize; i++)
                {
                    this.data[i] = array[144 + i];
                }
            }else
            {
                this.data = new byte[SOCKS5_MESSAGE_DATA_SIZE];
                for(int i = 0; i < SOCKS5_MESSAGE_DATA_SIZE; i++)
                {
                    this.data[i] = array[144 + i];
                }
            }
        }

        public uint MessageId
        {
            get { return messageId; }
            set { messageId = value; }
        }

        public uint ConnectionId
        {
            get { return connectionId; }
            set { connectionId = value; }
        }

        public uint ClientId
        {
            get { return clientId; }
            set { clientId = value; }
        }

        public uint ServerId
        {
            get { return serverId; }
            set { serverId = value; }
        }

        public char SourceNodeType
        {
            get { return sourceNodeType; }
            set { sourceNodeType = value; }
        }

        public string SourceIp
        {
            get { return sourceIp; }
            set { sourceIp = value; }
        }

        public char DestinationNodeType
        {
            get { return destinationNodeType; }
            set { destinationNodeType = value; }
        }

        public string DestinationIp
        {
            get { return destinationIp; }
            set { destinationIp = value; }
        }

        public int TvSec
        {
            get { return tvSec; }
            set { tvSec = value; }
        }

        public int TvUsec
        {
            get { return tvUsec; }
            set { tvUsec = value; }
        }

        public int ForwarderTvSec
        {
            get { return forwarderTvSec; }
            set { forwarderTvSec = value; }
        }

        public int ForwarderTvUsec
        {
            get { return forwarderTvUsec; }
            set { forwarderTvUsec = value; }
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
            byte[] tmp;

            buffer[0] = (byte)messageType;
            BitConverter.GetBytes((uint)IPAddress.HostToNetworkOrder((int)this.messageId)).CopyTo(buffer, 4);
            BitConverter.GetBytes((uint)IPAddress.HostToNetworkOrder((int)this.connectionId)).CopyTo(buffer, 8);
            BitConverter.GetBytes((uint)IPAddress.HostToNetworkOrder((int)this.clientId)).CopyTo(buffer, 12);
            BitConverter.GetBytes((uint)IPAddress.HostToNetworkOrder((int)this.serverId)).CopyTo(buffer, 16);
            buffer[20] = (byte)sourceNodeType;
            tmp = Encoding.UTF8.GetBytes(sourceIp);
            for(int i = 0; i < tmp.Length; i++)
            {
                buffer[22 + i] = tmp[i];
            }
            buffer[70] = (byte)destinationNodeType;
            tmp = Encoding.UTF8.GetBytes(destinationIp);
            for(int i = 0; i < tmp.Length; i++)
            {
                buffer[72 + i] = tmp[i];
            }
            BitConverter.GetBytes(IPAddress.HostToNetworkOrder(this.tvSec)).CopyTo(buffer, 120);
            BitConverter.GetBytes(IPAddress.HostToNetworkOrder(this.tvUsec)).CopyTo(buffer, 124);
            BitConverter.GetBytes(IPAddress.HostToNetworkOrder(this.forwarderTvSec)).CopyTo(buffer, 128);
            BitConverter.GetBytes(IPAddress.HostToNetworkOrder(this.forwarderTvUsec)).CopyTo(buffer, 132);
            BitConverter.GetBytes(HostToNetworkOrderUShort(this.dataSize)).CopyTo(buffer, 136);
            for(int i = 0; i < this.dataSize; i++)
            {
                buffer[144 + i] = this.data[i];
            }

            length = Socks5MessageDataHeader.SOCKS5_MESSAGE_DATA_HEADER_SIZE
                   + this.dataSize;

            return length;
        }
    }
}

