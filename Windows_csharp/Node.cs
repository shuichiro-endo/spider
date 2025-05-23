/*
 * Title:  spider Node.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Net.Sockets;

namespace spider
{
    public class Node
    {
        protected const int NODE_BUFFER_SIZE = 72000;
        protected const int SPIDER_MESSAGE_DATA_SIZE = 65536;
        protected const int SPIDER_MESSAGE_DATA_MAX_SIZE = 65552;     // 65536 + 16 (AES padding)
        protected const int INET6_ADDR_STRING_LENGTH = 46;

        protected TcpListener tcpListener = null;
        protected TcpClient tcpClient = null;
        protected UdpClient udpClient = null;
        protected IntPtr sock = IntPtr.Zero;
        protected NetworkStream stream = null;
        protected MessageManager messageManager;
        protected Socks5MessageQueue socks5MessagesQueue;
        protected Dictionary<uint, Socks5Message> socks5ReceiveMessagesMap = new Dictionary<uint, Socks5Message>();
        protected static readonly object socks5ReceiveMessagesMapLock = new object();

        public Node(TcpListener tcpListener,
                    TcpClient tcpClient,
                    MessageManager messageManager)
        {
            this.tcpListener = tcpListener;
            if(this.tcpListener != null)
            {
                this.sock = this.tcpListener.Server.Handle;
            }
            this.tcpClient = tcpClient;
            if(this.tcpClient != null)
            {
                this.sock = this.tcpClient.Client.Handle;
                this.stream = this.tcpClient.GetStream();
            }
            this.MessageManager = messageManager;
            this.socks5MessagesQueue = new Socks5MessageQueue();
        }

        public Node(UdpClient udpClient,
                    MessageManager messageManager)
        {
            this.udpClient = udpClient;
            if(this.udpClient != null)
            {
                this.sock = this.udpClient.Client.Handle;
            }
            this.MessageManager = messageManager;
            this.socks5MessagesQueue = new Socks5MessageQueue();
        }

        ~Node()
        {

        }

        public TcpListener TcpListener
        {
            get { return tcpListener; }
            set { tcpListener = value; }
        }

        public TcpClient TcpClient
        {
            get { return tcpClient; }
            set { tcpClient = value; }
        }

        public UdpClient UdpClient
        {
            get { return udpClient; }
            set { udpClient = value; }
        }

        public IntPtr Sock
        {
            get { return sock; }
            set { sock = value; }
        }

        public NetworkStream Stream
        {
            get { return stream; }
            set { stream = value; }
        }

        public MessageManager MessageManager
        {
            get { return messageManager; }
            set { messageManager = value; }
        }

        public void PushSocks5Message(Socks5Message socks5Message)
        {
            socks5MessagesQueue.Push(socks5Message);

            return;
        }

        public void PushSocks5Message(Object obj)
        {
            Socks5Message socks5Message = obj as Socks5Message;

            if(socks5Message != null)
            {
                socks5MessagesQueue.Push(socks5Message);
            }

            return;
        }

        public int PushTimeoutSocks5Message(Socks5Message socks5Message,
                                            int tvSec,
                                            int tvUsec)
        {
            int ret = 0;

            ret = socks5MessagesQueue.PushTimeout(socks5Message,
                                                  tvSec,
                                                  tvUsec);

            return ret;
        }

        protected Socks5Message PopSocks5Message()
        {
            Socks5Message socks5Message = socks5MessagesQueue.Pop();

            return socks5Message;
        }

        protected Socks5Message PopTimeoutSocks5Message(int tvSec,
                                                        int tvUsec)
        {
            Socks5Message socks5Message = socks5MessagesQueue.PopTimeout(tvSec,
                                                                         tvUsec);

            return socks5Message;
        }

        public void AddSocks5ReceiveMessageToMap(Object obj)
        {
            Socks5Message socks5Message = obj as Socks5Message;
            uint message_id = 0;

            if(socks5Message != null)
            {
                message_id = socks5Message.MessageId;

                lock(socks5ReceiveMessagesMapLock)
                {
                    socks5ReceiveMessagesMap.Add(message_id,
                                                 socks5Message);
                }
            }

            return;
        }

        protected Socks5Message GetSocks5ReceiveMessageFromMap(uint messageId)
        {
            Socks5Message socks5Message = null;

            lock(socks5ReceiveMessagesMapLock)
            {
                if(socks5ReceiveMessagesMap.ContainsKey(messageId))
                {
                    socks5Message = socks5ReceiveMessagesMap[messageId];
                    socks5ReceiveMessagesMap.Remove(messageId);
                }
            }

            return socks5Message;
        }

        public int RecvData(byte[] buffer,
                            int bufferSize,
                            int tvSec,
                            int tvUsec)
        {
            int rec = 0;
            tcpClient.ReceiveTimeout = tvSec * 1000 + tvUsec / 1000;

            if(bufferSize > NODE_BUFFER_SIZE)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] buffer size error");
#endif
                return -1;
            }

            try
            {
                rec = stream.Read(buffer,
                                  0,
                                  NODE_BUFFER_SIZE);
            }catch(IOException ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] RecvData timeout: {0}",
                                  ex.Message);
#endif
                return -1;
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] RecvData error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            return rec;
        }

        public int SendData(byte[] buffer,
                            int dataSize,
                            int tvSec,
                            int tvUsec)
        {
            int sen = 0;
            this.tcpClient.SendTimeout = tvSec * 1000 + tvUsec / 1000;

            if(dataSize > NODE_BUFFER_SIZE)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] data size error");
#endif
                return -1;
            }

            try
            {
                stream.Write(buffer,
                             0,
                             dataSize);
            }catch(IOException ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] SendData timeout: {0}",
                                  ex.Message);
#endif
                return -1;
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] SendData error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            sen = dataSize;

            return sen;
        }

        public uint GenerateRandomId()
        {
            Random random = new Random();
            ulong randomValue = 0;

            do
            {
                randomValue = (ulong)random.Next(1, int.MaxValue) + (ulong)random.Next(0, int.MaxValue);
            }while(randomValue >= (ulong)uint.MaxValue);

            return (uint)randomValue;
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
    }
}

