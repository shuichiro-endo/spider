/*
 * Title:  spider Pipe.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Threading;

namespace spider
{
    public class Pipe : Node
    {
        private SpiderIp spiderIp;
        private uint pipeId;
        private char mode;
        private string pipeIp;
        private string pipeIpScopeId;
        private string pipeListenPort;
        private string pipeDestinationIp;
        private string pipeDestinationIpScopeId;
        private string pipeDestinationPort;
        private RoutingManager routingManager;
        private RoutingMessageQueue routingMessagesQueue;

        public Pipe(SpiderIp spiderIp,
                    uint pipeId,
                    char mode,
                    string pipeIp,
                    string pipeIpScopeId,
                    string pipeDestinationIp,
                    string pipeDestinationIpScopeId,
                    string pipeDestinationPort,
                    TcpClient tcpClient,
                    RoutingManager routingManager,
                    MessageManager messageManager)
        : base(null,
               tcpClient,
               messageManager)
        {
            this.spiderIp = spiderIp;
            this.pipeId = pipeId;
            this.mode = mode;
            this.pipeIp = pipeIp;
            this.pipeIpScopeId = pipeIpScopeId;
            this.pipeDestinationIp = pipeDestinationIp;
            this.pipeDestinationIpScopeId = pipeDestinationIpScopeId;
            this.pipeDestinationPort = pipeDestinationPort;
            this.routingManager = routingManager;
            this.routingMessagesQueue = new RoutingMessageQueue();

            this.tcpClient.ReceiveTimeout = 3600 * 1000;
            this.tcpClient.SendTimeout = 3600 * 1000;
        }

        public Pipe(SpiderIp spiderIp,
                    uint pipeId,
                    char mode,
                    string pipeIp,
                    string pipeIpScopeId,
                    string pipeListenPort,
                    TcpListener tcpListener,
                    RoutingManager routingManager,
                    MessageManager messageManager)
        : base(tcpListener,
               null,
               messageManager)
        {
            this.spiderIp = spiderIp;
            this.pipeId = pipeId;
            this.mode = mode;
            this.pipeIp = pipeIp;
            this.pipeIpScopeId = pipeIpScopeId;
            this.pipeListenPort = pipeListenPort;
            this.routingManager = routingManager;
            this.routingMessagesQueue = new RoutingMessageQueue();
        }

        ~Pipe()
        {

        }

        public SpiderIp SpiderIp
        {
            get { return spiderIp; }
            set { spiderIp = value; }
        }

        public uint PipeId
        {
            get { return pipeId; }
            set { pipeId = value; }
        }

        public char Mode
        {
            get { return mode; }
            set { mode = value; }
        }

        public string PipeIp
        {
            get { return pipeIp; }
            set { pipeIp = value; }
        }

        public string PipeIpScopeId
        {
            get { return pipeIpScopeId; }
            set { pipeIpScopeId = value; }
        }

        public string PipeListenPort
        {
            get { return pipeListenPort; }
            set { pipeListenPort = value; }
        }

        public string PipeDestinationIp
        {
            get { return pipeDestinationIp; }
            set { pipeDestinationIp = value; }
        }

        public string PipeDestinationIpScopeId
        {
            get { return pipeDestinationIpScopeId; }
            set { pipeDestinationIpScopeId = value; }
        }

        public string PipeDestinationPort
        {
            get { return pipeDestinationPort; }
            set { pipeDestinationPort = value; }
        }

        public void PushRoutingMessage(RoutingMessage routingMessage)
        {
            routingMessagesQueue.Push(routingMessage);

            return;
        }

        private RoutingMessage PopRoutingMessage()
        {
            RoutingMessage routingMessage = routingMessagesQueue.Pop();

            return routingMessage;
        }

        public int RecvMessage()
        {
            int rec = 0;
            int tmprec = 0;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            byte[] tmp = new byte[NODE_BUFFER_SIZE];

            RoutingMessage routingMessage;
            RoutingMessageDataHeader routingMessageDataHeader = new RoutingMessageDataHeader(buffer);
            ushort routingMessageHeaderSize = (ushort)RoutingMessageDataHeader.ROUTING_MESSAGE_DATA_HEADER_SIZE;

            Socks5Message socks5Message;
            Socks5MessageDataHeader socks5MessageDataHeader = new Socks5MessageDataHeader(buffer);
            ushort socks5MessageHeaderSize = (ushort)Socks5MessageDataHeader.SOCKS5_MESSAGE_DATA_HEADER_SIZE;

            bool recvMessageTypeFlag = false;
            bool recvHeaderFlag = false;
            int recvMessageTypeSize = 1;    // byte
            int recvDataSize = 0;
            int remainingSize = 0;

            Pipe pipe;


            while(true)
            {
                try
                {
                    if(recvMessageTypeFlag == false)
                    {

                        tmprec = stream.Read(buffer,
                                             rec,
                                             recvMessageTypeSize);
                    }else
                    {
                        tmprec = stream.Read(buffer,
                                             rec,
                                             remainingSize);
                    }

                    if(tmprec > 0)
                    {
                        rec += tmprec;
                        tmprec = 0;

#if DEBUGPRINT
//                        Console.WriteLine("rec: {0}", rec);
//                        PrintBytes(buffer, rec);
#endif

                        if(recvMessageTypeFlag == false)
                        {
                            recvMessageTypeFlag = true;

                            if(buffer[0] == 0x72)   // 'r'
                            {
                                remainingSize = routingMessageHeaderSize - rec;
                            }else if(buffer[0] == 0x73 )    // 's'
                            {
                                remainingSize = socks5MessageHeaderSize - rec;
                            }else
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] RecvMessage message type error: {0}",
                                                  (char)buffer[0]);
#endif
                                return 0;
                            }

                            continue;
                        }else if(recvHeaderFlag == false)
                        {
                            if(buffer[0] == 0x72)   // 'r'
                            {
                                if(rec < routingMessageHeaderSize)
                                {
                                    remainingSize = routingMessageHeaderSize - rec;
                                }else
                                {
                                    recvHeaderFlag = true;

                                    routingMessageDataHeader = new RoutingMessageDataHeader(buffer);

                                    recvDataSize = NetworkToHostOrderUShort(routingMessageDataHeader.DataSize);

                                    remainingSize = recvDataSize;
                                }

                                continue;
                            }else if(buffer[0] == 0x73) // 's'
                            {
                                if(rec < socks5MessageHeaderSize)
                                {
                                    remainingSize = socks5MessageHeaderSize - rec;
                                }else
                                {
                                    recvHeaderFlag = true;

                                    socks5MessageDataHeader = new Socks5MessageDataHeader(buffer);

                                    recvDataSize = NetworkToHostOrderUShort(socks5MessageDataHeader.DataSize);

                                    remainingSize = recvDataSize;
                                }

                                continue;
                            }else
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] RecvMessage message type error: {0}",
                                                  (char)buffer[0]);
#endif
                                return 0;
                            }
                        }else
                        {
                            if(buffer[0] == 0x72)   // 'r'
                            {
                                if(rec < routingMessageHeaderSize + recvDataSize)
                                {
                                    remainingSize = routingMessageHeaderSize + recvDataSize - rec;
                                    continue;
                                }else
                                {
                                    routingMessageDataHeader = new RoutingMessageDataHeader(buffer);
                                    if(rec < routingMessageHeaderSize + NetworkToHostOrderUShort(routingMessageDataHeader.DataSize))
                                    {
                                        continue;
                                    }else if(rec == routingMessageHeaderSize + NetworkToHostOrderUShort(routingMessageDataHeader.DataSize))
                                    {
                                        routingMessage = new RoutingMessage(this.PipeId,
                                                                            buffer);

                                        Thread threadMessageManager = new Thread(new ParameterizedThreadStart(messageManager.PushRoutingMessage));
                                        threadMessageManager.Start(routingMessage);

                                        recvMessageTypeFlag = false;
                                        recvHeaderFlag = false;
                                        Array.Clear(buffer,
                                                    0,
                                                    NODE_BUFFER_SIZE);
                                    }
                                }
                            }else if(buffer[0] == 0x73) // 's'
                            {
                                if(rec < socks5MessageHeaderSize + recvDataSize)
                                {
                                    remainingSize = socks5MessageHeaderSize + recvDataSize - rec;
                                    continue;
                                }else
                                {
                                    socks5MessageDataHeader = new Socks5MessageDataHeader(buffer);
                                    if(rec < socks5MessageHeaderSize + NetworkToHostOrderUShort(socks5MessageDataHeader.DataSize))
                                    {
                                        continue;
                                    }else if(rec == socks5MessageHeaderSize + NetworkToHostOrderUShort(socks5MessageDataHeader.DataSize))
                                    {
                                        socks5Message = new Socks5Message(buffer);

                                        if((String.Compare(spiderIp.SpiderIpv4, socks5Message.DestinationIp) == 0) ||
                                           (String.Compare(spiderIp.SpiderIpv6Global, socks5Message.DestinationIp) == 0) ||
                                           (String.Compare(spiderIp.SpiderIpv6UniqueLocal, socks5Message.DestinationIp) == 0) ||
                                           (String.Compare(spiderIp.SpiderIpv6LinkLocal, socks5Message.DestinationIp) == 0))
                                        {
                                            Thread threadMessageManager = new Thread(new ParameterizedThreadStart(messageManager.PushSocks5Message));
                                            threadMessageManager.Start(socks5Message);

                                            recvMessageTypeFlag = false;
                                            recvHeaderFlag = false;
                                            Array.Clear(buffer,
                                                        0,
                                                        NODE_BUFFER_SIZE);
                                        }else
                                        {
                                            pipe = routingManager.GetDestinationPipe(socks5Message.DestinationIp);
                                            if(pipe != null)
                                            {
                                                Thread threadPipe = new Thread(new ParameterizedThreadStart(pipe.PushSocks5Message));
                                                threadPipe.Start(socks5Message);

                                                recvMessageTypeFlag = false;
                                                recvHeaderFlag = false;
                                                Array.Clear(buffer,
                                                            0,
                                                            NODE_BUFFER_SIZE);
                                            }else
                                            {
#if DEBUGPRINT
                                                Console.WriteLine("[-] cannot transfer pipe message");
#endif

                                                return 0;
                                            }
                                        }
                                    }
                                }
                            }else
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] recv_message message type error: {0}",
                                                  (char)buffer[0]);
#endif
                                return 0;
                            }
                        }

                        recvMessageTypeFlag = false;
                        recvHeaderFlag = false;
                        Array.Clear(buffer,
                                    0,
                                    NODE_BUFFER_SIZE);

                        break;
                    }
                }catch(IOException ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] RecvMessage error: {0}",
                                      ex.Message);
#endif
                    return -1;
                }catch (Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] RecvMessage error: {0}",
                                      ex.Message);
#endif
                    return -1;
                }
            }

            return rec;
        }

        public int SendRoutingMessage()
        {
            int sen = 0;
            int length = 0;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];

            RoutingMessage routingMessage;
            routingMessage = PopRoutingMessage();
            if(routingMessage == null)
            {
                return 0;
            }

            length = routingMessage.CopyToBuffer(ref buffer);

#if DEBUGPRINT
//            Console.WriteLine("length: {0}", length);
//            PrintBytes(buffer, length);
#endif

            try
            {
                stream.Write(buffer,
                             0,
                             length);
            }catch(IOException ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] SendRoutingMessage error: {0}",
                                  ex.Message);
#endif
                return -1;
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] SendRoutingMessage error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            sen = length;

            return sen;
        }

        public int SendSocks5Message()
        {
            int sen = 0;
            int length = 0;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];

            Socks5Message socks5Message;
            socks5Message = PopSocks5Message();
            length = socks5Message.CopyToBuffer(ref buffer);

#if DEBUGPRINT
//            Console.WriteLine("length: {0}", length);
//            PrintBytes(buffer, length);
#endif

            try
            {
                stream.Write(buffer,
                             0,
                             length);
            }catch(IOException ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] SengSocks5Message error: {0}",
                                  ex.Message);
#endif
                return -1;
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] SengSocks5Message error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            sen = length;

            return sen;
        }
    }
}

