/*
 * Title:  spider Structs.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;

namespace spider
{
    public class RouteData
    {
        public static int ROUTE_DATA_SIZE = 48;
        const int INET6_ADDR_STRING_LENGTH = 46;

        byte[] ip = new byte[INET6_ADDR_STRING_LENGTH + 1];
        byte metric;


        public RouteData(byte[] ip,
                         byte metric)
        {
            try
            {
                for(int i = 0; i < INET6_ADDR_STRING_LENGTH + 1; i++)
                {
                    this.ip[i] = ip[i];
                }
                this.metric = metric;
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public RouteData(byte[] array)
        {
            try
            {
                for(int i = 0; i < INET6_ADDR_STRING_LENGTH + 1; i++)
                {
                    ip[i] = array[i];
                }
                metric = array[INET6_ADDR_STRING_LENGTH + 2];
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte[] Ip
        {
            get { return ip; }
            set { ip = value; }
        }

        public byte Metric
        {
            get { return metric; }
            set { metric = value;}
        }
    }

    public class RoutingMessageDataHeader
    {
        public static int ROUTING_MESSAGE_DATA_HEADER_SIZE = 4;

        byte messageType;
        byte reserved;
        ushort dataSize;    // network byte order

        public RoutingMessageDataHeader(byte[] array)
        {
            try
            {
                messageType = array[0];
                reserved = array[1];
                dataSize = BitConverter.ToUInt16(array, 2);     // network byte order
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte MessageType
        {
            get { return messageType; }
            set { messageType = value; }
        }

        public byte Reserved
        {
            get { return reserved; }
            set { reserved = value; }
        }

        public ushort DataSize
        {
            get { return dataSize; }
            set { dataSize = value; }
        }
    }

    public class RoutingMessageData
    {
        const int ROUTING_MESSAGE_DATA_SIZE = 60000;

        byte messageType;
        byte reserved;
        ushort dataSize;    // host byte order
        byte[] data;

        public RoutingMessageData(byte messageType,
                                  byte reserved,
                                  ushort dataSize,
                                  byte[] data)
        {
            this.messageType = messageType;
            this.reserved = reserved;
            this.dataSize = dataSize;
            if(dataSize <= ROUTING_MESSAGE_DATA_SIZE)
            {
                this.data = new byte[dataSize];
                for(int i = 0; i < dataSize; i++)
                {
                    this.data[i] = data[i];
                }
            }else
            {
                this.data = new byte[ROUTING_MESSAGE_DATA_SIZE];
                for(int i = 0; i < ROUTING_MESSAGE_DATA_SIZE; i++)
                {
                    this.data[i] = data[i];
                }
            }
        }

        public RoutingMessageData(byte[] array)
        {
            try
            {
                messageType = array[0];
                reserved = array[1];
                dataSize = NetworkToHostOrderUShort(BitConverter.ToUInt16(array, 2));   // host byte order
                if(dataSize <= ROUTING_MESSAGE_DATA_SIZE)
                {
                    this.data = new byte[dataSize];
                    for(int i = 0; i < dataSize; i++)
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

        public byte MessageType
        {
            get { return messageType; }
            set { messageType = value; }
        }

        public byte Reserved
        {
            get { return reserved; }
            set { reserved = value; }
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
    }

    public class Socks5MessageDataHeader
    {
        public static int SOCKS5_MESSAGE_DATA_HEADER_SIZE = 144;
        const int INET6_ADDR_STRING_LENGTH = 46;

        byte messageType;
//      byte reserved1;
//      byte reserved2;
//      byte reserved3;
        uint messageId;         // network byte order
        uint connectionId;      // network byte order
        uint clientId;          // network byte order
        uint serverId;          // network byte order
        byte sourceNodeType;
//      byte reserved4;
        byte[] sourceIp = new byte[INET6_ADDR_STRING_LENGTH + 1];
//      byte reserved5;
        byte destinationNodeType;
//      byte reserved6;
        byte[] destinationIp = new byte[INET6_ADDR_STRING_LENGTH + 1];
//      byte reserved7;
        int tvSec;              // network byte order
        int tvUsec;             // network byte order
        int forwarderTvSec;     // network byte order
        int forwarderTvUsec;    // network byte order
        ushort dataSize;        // network byte order
//      byte reserved8;
//      byte reserved9;
//      byte reserved10;
//      byte reserved11;
//      byte reserved12;
//      byte reserved13;

        public Socks5MessageDataHeader(byte[] array)
        {
            try
            {
                messageType = array[0];
                messageId = BitConverter.ToUInt32(array, 4);
                connectionId = BitConverter.ToUInt32(array, 8);
                clientId = BitConverter.ToUInt32(array, 12);
                serverId = BitConverter.ToUInt32(array, 16);
                sourceNodeType = array[20];
                for(int i = 0; i < INET6_ADDR_STRING_LENGTH + 1; i++)
                {
                    sourceIp[i] = array[22 + i];
                }
                destinationNodeType = array[70];
                for(int i = 0; i < INET6_ADDR_STRING_LENGTH + 1; i++)
                {
                    destinationIp[i] = array[72 + i];
                }
                tvSec = BitConverter.ToInt32(array, 120);
                tvUsec = BitConverter.ToInt32(array, 124);
                forwarderTvSec = BitConverter.ToInt32(array, 128);
                forwarderTvUsec = BitConverter.ToInt32(array, 132);
                dataSize = BitConverter.ToUInt16(array, 136);

            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte MessageType
        {
            get { return messageType; }
            set { messageType = value; }
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

        public byte SourceNodeType
        {
            get { return sourceNodeType; }
            set { sourceNodeType = value; }
        }

        public byte[] SourceIp
        {
            get { return sourceIp; }
            set { sourceIp = value; }
        }

        public byte DestinationNodeType
        {
            get { return destinationNodeType; }
            set { destinationNodeType = value; }
        }

        public byte[] DestinationIp
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
    }

    public class Socks5MessageData
    {
        const int SOCKS5_MESSAGE_DATA_SIZE = 60000;
        const int INET6_ADDR_STRING_LENGTH = 46;

        byte messageType;
//      byte reserved1;
//      byte reserved2;
//      byte reserved3;
        uint messageId;         // host byte order
        uint connectionId;      // host byte order
        uint clientId;          // host byte order
        uint serverId;          // host byte order
        byte sourceNodeType;
//      byte reserved4;
        byte[] sourceIp = new byte[INET6_ADDR_STRING_LENGTH + 1];
//      byte reserved5;
        byte destinationNodeType;
//      byte reserved6;
        byte[] destinationIp = new byte[INET6_ADDR_STRING_LENGTH + 1];
//      byte reserved7;
        int tvSec;              // host byte order
        int tvUsec;             // host byte order
        int forwarderTvSec;     // host byte order
        int forwarderTvUsec;    // host byte order
        ushort dataSize;        // host byte order
//      byte reserved8;
//      byte reserved9;
//      byte reserved10;
//      byte reserved11;
//      byte reserved12;
//      byte reserved13;
        byte[] data;

        public Socks5MessageData(byte[] array)
        {
            try
            {
                messageType = array[0];
                messageId = (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(array, 4));
                connectionId = (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(array, 8));
                clientId = (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(array, 12));
                serverId = (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(array, 16));
                sourceNodeType = array[20];
                for(int i = 0; i < INET6_ADDR_STRING_LENGTH + 1; i++)
                {
                    sourceIp[i] = array[22 + i];
                }
                destinationNodeType = array[70];
                for(int i = 0; i < INET6_ADDR_STRING_LENGTH + 1; i++)
                {
                    destinationIp[i] = array[72 + i];
                }
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
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte MessageType
        {
            get { return messageType; }
            set { messageType = value; }
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

        public byte SourceNodeType
        {
            get { return sourceNodeType; }
            set { sourceNodeType = value; }
        }

        public byte[] SourceIp
        {
            get { return sourceIp; }
            set { sourceIp = value; }
        }

        public byte DestinationNodeType
        {
            get { return destinationNodeType; }
            set { destinationNodeType = value; }
        }

        public byte[] DestinationIp
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
    }

    public class UploadDownloadDataHeader
    {
        public static int UPLOAD_DOWNLOAD_DATA_HEADER_SIZE = 544;

        byte[] command = new byte[16];
        byte[] fileName = new byte[256];
        byte[] filePath = new byte[256];
        ulong fileSize;
        ulong dataSize;

        public UploadDownloadDataHeader(byte[] command,
                                        byte[] fileName,
                                        byte[] filePath,
                                        ulong fileSize,
                                        ulong dataSize)
        {
            try
            {
                for(int i = 0; i < 16; i++)
                {
                    this.command[i] = command[i];
                }
                for(int i = 0; i < 256; i++)
                {
                    this.fileName[i] = fileName[i];
                }
                for(int i = 0; i < 256; i++)
                {
                    this.filePath[i] = filePath[i];
                }
                this.fileSize = fileSize;
                this.dataSize = dataSize;
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public UploadDownloadDataHeader(byte[] array)
        {
            int index = 0;

            try
            {
                for(int i = 0; i < 16; i++)
                {
                    this.command[i] = array[i];
                }
                index += 16;

                for(int i = 0; i < 256; i++)
                {
                    this.fileName[i] = array[index + i];
                }
                index += 256;

                for(int i = 0; i < 256; i++)
                {
                    this.filePath[i] = array[index + i];
                }
                index += 256;

                this.fileSize = (ulong)IPAddress.NetworkToHostOrder((long)BitConverter.ToUInt64(array, index));
                index += 8;

                this.dataSize = (ulong)IPAddress.NetworkToHostOrder((long)BitConverter.ToUInt64(array, index));
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte[] Command
        {
            get { return command; }
            set { command = value; }
        }

        public byte[] FileName
        {
            get { return fileName; }
            set { fileName = value; }
        }

        public byte[] FilePath
        {
            get { return filePath; }
            set { filePath = value; }
        }

        public ulong FileSize
        {
            get { return fileSize; }
            set { fileSize = value; }
        }

        public ulong DataSize
        {
            get {return dataSize; }
            set { dataSize = value; }
        }

        public int CopyToBuffer(ref byte[] buffer)
        {
            int index = 0;
            byte[] tmp;

            try
            {
                for(int i = 0; i < 16; i++)
                {
                    buffer[i] = command[i];
                }
                index += 16;

                for(int i = 0; i < 256; i++)
                {
                    buffer[index + i] = fileName[i];
                }
                index += 256;

                for(int i = 0; i < 256; i++)
                {
                    buffer[index + i] = filePath[i];
                }
                index += 256;

                tmp = BitConverter.GetBytes((ulong)IPAddress.HostToNetworkOrder((long)fileSize));
                for(int i = 0; i < 8; i++)
                {
                    buffer[index + i] = tmp[i];
                }
                index += 8;

                tmp = BitConverter.GetBytes((ulong)IPAddress.HostToNetworkOrder((long)dataSize));
                for(int i = 0; i < 8; i++)
                {
                    buffer[index + i] = tmp[i];
                }
                index += 8;
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }

            return index;
        }
    }

    public class UploadDownloadData
    {
        const int SHELL_UPLOAD_DOWNLOAD_DATA_SIZE = 50000;

        byte[] command = new byte[16];
        byte[] fileName = new byte[256];
        byte[] filePath = new byte[256];
        ulong fileSize;
        ulong dataSize;
        byte[] data = new byte[SHELL_UPLOAD_DOWNLOAD_DATA_SIZE];

        public UploadDownloadData(string command,
                                  string fileName,
                                  string filePath,
                                  ulong fileSize,
                                  ulong dataSize,
                                  byte[] data)
        {
            byte[] tmp;

            try
            {
                tmp = Encoding.UTF8.GetBytes(command);
                if(tmp.Length <= 16)
                {
                    for(int i = 0; i < tmp.Length; i++)
                    {
                        this.command[i] = tmp[i];
                    }
                }else
                {
                    for(int i = 0; i < 16; i++)
                    {
                        this.command[i] = tmp[i];
                    }
                }

                tmp = Encoding.UTF8.GetBytes(fileName);
                if(tmp.Length <= 256)
                {
                    for(int i = 0; i < tmp.Length; i++)
                    {
                        this.fileName[i] = tmp[i];
                    }
                }else
                {
                    for(int i = 0; i < 256; i++)
                    {
                        this.fileName[i] = tmp[i];
                    }
                }

                tmp = Encoding.UTF8.GetBytes(filePath);
                if(tmp.Length <= 256)
                {
                    for(int i = 0; i < tmp.Length; i++)
                    {
                        this.filePath[i] = tmp[i];
                    }
                }else
                {
                    for(int i = 0; i < 256; i++)
                    {
                        this.filePath[i] = tmp[i];
                    }
                }

                this.fileSize = fileSize;
                this.dataSize = dataSize;
                for(ulong i = 0; i < dataSize; i++)
                {
                    this.data[i] = data[i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public UploadDownloadData(byte[] array)
        {
            try
            {
                int index = 0;

                for(int i = 0; i < 16; i++)
                {
                    this.command[i] = array[i];
                }
                index += 16;

                for(int i = 0; i < 256; i++)
                {
                    this.fileName[i] = array[index + i];
                }
                index += 256;

                for(int i = 0; i < 256; i++)
                {
                    this.filePath[i] = array[index + i];
                }
                index += 256;

                this.fileSize = (ulong)IPAddress.NetworkToHostOrder((long)BitConverter.ToUInt64(array, index));
                index += 8;

                this.dataSize = (ulong)IPAddress.NetworkToHostOrder((long)BitConverter.ToUInt64(array, index));
                index += 8;

                for(ulong i = 0; i < this.dataSize; i++)
                {
                    this.data[i] = array[(ulong)index + i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte[] Command
        {
            get { return command; }
            set { command = value; }
        }

        public byte[] FileName
        {
            get { return fileName; }
            set { fileName = value; }
        }

        public byte[] FilePath
        {
            get { return filePath; }
            set { filePath = value; }
        }

        public ulong FileSize
        {
            get { return fileSize; }
            set { fileSize = value; }
        }

        public ulong DataSize
        {
            get {return dataSize; }
            set { dataSize = value; }
        }

        public byte[] Data
        {
            get { return data; }
            set { data = value; }
        }

        public int CopyToBuffer(ref byte[] buffer)
        {
            int index = 0;
            byte[] tmp;

            try
            {
                for(int i = 0; i < 16; i++)
                {
                    buffer[i] = command[i];
                }
                index += 16;

                for(int i = 0; i < 256; i++)
                {
                    buffer[index + i] = fileName[i];
                }
                index += 256;

                for(int i = 0; i < 256; i++)
                {
                    buffer[index + i] = filePath[i];
                }
                index += 256;

                tmp = BitConverter.GetBytes((ulong)IPAddress.HostToNetworkOrder((long)fileSize));
                for(int i = 0; i < 8; i++)
                {
                    buffer[index + i] = tmp[i];
                }
                index += 8;

                tmp = BitConverter.GetBytes((ulong)IPAddress.HostToNetworkOrder((long)dataSize));
                for(int i = 0; i < 8; i++)
                {
                    buffer[index + i] = tmp[i];
                }
                index += 8;

                for(ulong i = 0; i < dataSize; i++)
                {
                    buffer[(ulong)index + i] = data[i];
                }
                index += (int)dataSize;
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }

            return index;
        }
    }
}

