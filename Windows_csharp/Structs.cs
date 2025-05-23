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
        private const int INET6_ADDR_STRING_LENGTH = 46;

        private byte[] ip = new byte[INET6_ADDR_STRING_LENGTH + 1];
        private byte metric;


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

    public class SpiderMessageHeader
    {
        public static int SPIDER_MESSAGE_HEADER_SIZE = 144;
        private const int INET6_ADDR_STRING_LENGTH = 46;

        private byte messageType;
        private byte receiveFlag;
        private byte receiveResult;
        private byte commandResult;
        private uint messageId;         // network byte order
        private uint connectionId;      // network byte order
        private uint clientId;          // network byte order
        private uint serverId;          // network byte order
        private byte sourceNodeType;
//      private byte reserved1;
        private byte[] sourceIp = new byte[INET6_ADDR_STRING_LENGTH + 1];
//      private byte reserved2;
        private byte destinationNodeType;
//      private byte reserved3;
        private byte[] destinationIp = new byte[INET6_ADDR_STRING_LENGTH + 1];
//      private byte reserved4;
        private int tvSec;              // network byte order
        private int tvUsec;             // network byte order
        private int forwarderTvSec;     // network byte order
        private int forwarderTvUsec;    // network byte order
        private int dataSize;           // network byte order
//      private byte reserved5;
//      private byte reserved6;
//      private byte reserved7;
//      private byte reserved8;

        public SpiderMessageHeader(byte[] array)
        {
            try
            {
                messageType = array[0];
                receiveFlag = array[1];
                receiveResult = array[2];
                commandResult = array[3];
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
                dataSize = BitConverter.ToInt32(array, 136);

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

        public byte ReceiveFlag
        {
            get { return receiveFlag; }
            set { receiveFlag = value; }
        }

        public byte ReceiveResult
        {
            get { return receiveResult; }
            set { receiveResult = value; }
        }

        public byte CommandResult
        {
            get { return commandResult; }
            set { commandResult = value; }
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

        public int DataSize
        {
            get { return dataSize; }
            set { dataSize = value; }
        }
    }

    public class UploadDownloadDataHeader
    {
        public static int UPLOAD_DOWNLOAD_DATA_HEADER_SIZE = 544;

        private byte[] command = new byte[16];
        private byte[] fileName = new byte[256];
        private byte[] filePath = new byte[256];
        private ulong fileSize;
        private ulong dataSize;

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
        private const int SHELL_UPLOAD_DOWNLOAD_DATA_SIZE = 50000;

        private byte[] command = new byte[16];
        private byte[] fileName = new byte[256];
        private byte[] filePath = new byte[256];
        private ulong fileSize;
        private ulong dataSize;
        private byte[] data = new byte[SHELL_UPLOAD_DOWNLOAD_DATA_SIZE];

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

