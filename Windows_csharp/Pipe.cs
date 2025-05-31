/*
 * Title:  spider Pipe.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

namespace spider
{
    public class Pipe : Node
    {
        private const string HTTP_REQUEST_HEADER_USER_AGENT_VALUE = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/127.0.0.0 Safari/537.36 Edg/127.0.2651.74";

        private SpiderIp spiderIp;
        private uint pipeId;
        private char mode;
        private char messageMode;
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
                    char messageMode,
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
            this.messageMode = messageMode;
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
                    char messageMode,
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
            this.messageMode = messageMode;
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

        public char MessageMode
        {
            get { return messageMode; }
            set { messageMode = value; }
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
            RoutingMessage routingMessageOld = null;

            routingMessageOld = routingMessagesQueue.PopLatestMessage();
            routingMessagesQueue.Push(routingMessage);

            return;
        }

        private RoutingMessage PopRoutingMessage()
        {
            RoutingMessage routingMessage = routingMessagesQueue.Pop();

            return routingMessage;
        }

        private RoutingMessage PopLatestRoutingMessage()
        {
            RoutingMessage routingMessage = routingMessagesQueue.PopLatestMessage();

            return routingMessage;
        }

        public int RecvMessage()
        {
            int rec = 0;
            int tmprec = 0;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            byte[] tmp = new byte[NODE_BUFFER_SIZE];

            RoutingMessage routingMessage;
            Socks5Message socks5Message;
            SpiderMessageHeader spiderMessageHeader = null;
            int spiderMessageHeaderSize = (int)SpiderMessageHeader.SPIDER_MESSAGE_HEADER_SIZE;
            bool recvHeaderFlag = false;
            int recvDataSize = 0;
            int remainingSize = 0;

            Pipe pipe;


            while(true)
            {
                try
                {
                    if(recvHeaderFlag == false)
                    {

                        tmprec = stream.Read(buffer,
                                             rec,
                                             spiderMessageHeaderSize);
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

                        if(recvHeaderFlag == false)
                        {
                            if(rec < spiderMessageHeaderSize)
                            {
                                remainingSize = spiderMessageHeaderSize - rec;
                                continue;
                            }else
                            {
                                recvHeaderFlag = true;

                                spiderMessageHeader = new SpiderMessageHeader(buffer);

                                recvDataSize = IPAddress.NetworkToHostOrder(spiderMessageHeader.DataSize);

                                remainingSize = recvDataSize;
                                if(remainingSize > 0)
                                {
                                    continue;
                                }
                            }
                        }

                        if(recvHeaderFlag == true)
                        {
                            if(rec < spiderMessageHeaderSize + recvDataSize)
                            {
                                remainingSize = spiderMessageHeaderSize + recvDataSize - rec;
                                continue;
                            }else
                            {
                                spiderMessageHeader = new SpiderMessageHeader(buffer);
                                if(spiderMessageHeader.MessageType == 'r')
                                {
                                    routingMessage = new RoutingMessage(this.PipeId,
                                                                        buffer);

                                    Thread threadMessageManager = new Thread(new ParameterizedThreadStart(messageManager.PushRoutingMessage));
                                    threadMessageManager.Start(routingMessage);

                                    recvHeaderFlag = false;
                                    Array.Clear(buffer,
                                                0,
                                                NODE_BUFFER_SIZE);
                                }else if(spiderMessageHeader.MessageType == 's')
                                {
                                    socks5Message = new Socks5Message(buffer);

                                    if((String.Compare(spiderIp.SpiderIpv4, socks5Message.DestinationIp) == 0) ||
                                       (String.Compare(spiderIp.SpiderIpv6Global, socks5Message.DestinationIp) == 0) ||
                                       (String.Compare(spiderIp.SpiderIpv6UniqueLocal, socks5Message.DestinationIp) == 0) ||
                                       (String.Compare(spiderIp.SpiderIpv6LinkLocal, socks5Message.DestinationIp) == 0))
                                    {
                                        Thread threadMessageManager = new Thread(new ParameterizedThreadStart(messageManager.PushSocks5Message));
                                        threadMessageManager.Start(socks5Message);

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
                                }else
                                {
#if DEBUGPRINT
                                    Console.WriteLine("[-] recv_message message type error: {0}",
                                                      (char)buffer[0]);
#endif
                                    return 0;
                                }
                            }
                        }

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
                }catch(Exception ex)
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

        private int GetHttpHeaderLength(byte[] buffer)
        {
            byte[] crlf = new byte[] {13, 10};
            int headerEndIndex = 0;

            for (int i=0; i<buffer.Length-3; i++)
            {
                if (buffer[i] == crlf[0] &&
                    buffer[i + 1] == crlf[1] &&
                    buffer[i + 2] == crlf[0] &&
                    buffer[i + 3] == crlf[1])
                {
                    headerEndIndex = i;
                    break;
                }
            }

            if(headerEndIndex == 0)
            {
                return 0;
            }

            return headerEndIndex + 4;
        }

        private int GetContentLength(byte[] buffer)
        {
            string httpHeader = Encoding.UTF8.GetString(buffer);
            string crlf = "\r\n";
            string[] lines = httpHeader.Split(new string[] {crlf},
                                              StringSplitOptions.None);

            foreach(string line in lines)
            {
                if(line.StartsWith("Content-Length:", StringComparison.OrdinalIgnoreCase))
                {
                    string[] parts = line.Split(':');
                    if(parts.Length > 1 &&
                       int.TryParse(parts[1].Trim(), out int contentLength))
                    {
                        return contentLength;
                    }
                }
            }

            return 0;
        }

        public int DoHttpConnectionClient()
        {
            int rec = 0;
            int tmprec = 0;
            int len = 0;

            int timeout = 30 * 1000;    // 30s
            this.tcpClient.ReceiveTimeout = timeout;
            this.tcpClient.SendTimeout = timeout;

            int bufferSize = NODE_BUFFER_SIZE * 12;
            int tmpSize = NODE_BUFFER_SIZE;
            int bufferHttpHeaderSize = NODE_BUFFER_SIZE;
            int bufferHttpBodySize = NODE_BUFFER_SIZE * 11;

            byte[] buffer = new byte[bufferSize];
            byte[] tmp = new byte[tmpSize];
            byte[] bufferHttpBody = new byte[bufferHttpBodySize];

            int httpHeaderLength = 0;   // start line + headers + empty line
            int httpBodyLength = 0;     // body
            int tmpLength = 0;
            string httpHeaderString = "";
            byte[] httpHeaderByteArray = null;
            int routingMessageCount = 0;
            int socks5MessageCount = 0;
            RoutingMessage routingMessage;
            Socks5Message socks5Message;
            bool recvHttpHeaderFlag = false;
            int totalLength = 0;
            int remainingSize = 0;
            int pos = 0;
            SpiderMessageHeader spiderMessageHeader = null;
            int spiderMessageHeaderSize = (int)SpiderMessageHeader.SPIDER_MESSAGE_HEADER_SIZE;
            int dataSize = 0;
            Pipe pipe = null;


            try
            {
                routingMessage = PopLatestRoutingMessage();
                if(routingMessage != null)
                {
                    httpBodyLength = routingMessage.CopyToBuffer(ref bufferHttpBody);
                    routingMessageCount++;
                }

                socks5MessageCount = this.socks5MessagesQueue.GetCount();
                if(socks5MessageCount > 10)
                {
                    socks5MessageCount = 10;
                }

                for(int i=socks5MessageCount; i>0; i--)
                {
                    Array.Clear(tmp,
                                0,
                                tmpSize);

                    socks5Message = this.PopSocks5Message();

                    tmpLength = socks5Message.CopyToBuffer(ref tmp);

                    Buffer.BlockCopy(tmp,
                                     0,
                                     bufferHttpBody,
                                     httpBodyLength,
                                     tmpLength);

                    httpBodyLength += tmpLength;
                }

                if(pipeDestinationIp.Contains(":")) // ipv6
                {
                    httpHeaderString = String.Format("POST / HTTP/1.1\r\nHost: [{0}]:{1}\r\nUser-Agent: {2}\r\nContent-Type: application/octet-stream\r\nContent-Length: {3}\r\n\r\n",
                                                     pipeDestinationIp,
                                                     pipeDestinationPort,
                                                     HTTP_REQUEST_HEADER_USER_AGENT_VALUE,
                                                     httpBodyLength);
                }else
                {
                    httpHeaderString = String.Format("POST / HTTP/1.1\r\nHost: {0}:{1}\r\nUser-Agent: {2}\r\nContent-Type: application/octet-stream\r\nContent-Length: {3}\r\n\r\n",
                                                     pipeDestinationIp,
                                                     pipeDestinationPort,
                                                     HTTP_REQUEST_HEADER_USER_AGENT_VALUE,
                                                     httpBodyLength);
                }

                httpHeaderByteArray = Encoding.UTF8.GetBytes(httpHeaderString);
                httpHeaderLength = httpHeaderByteArray.Length;

                Buffer.BlockCopy(httpHeaderByteArray,
                                 0,
                                 buffer,
                                 0,
                                 httpHeaderLength);

                Buffer.BlockCopy(bufferHttpBody,
                                 0,
                                 buffer,
                                 httpHeaderLength,
                                 httpBodyLength);

                len = httpHeaderLength + httpBodyLength;

#if DEBUGPRINT
//                Console.WriteLine("len: {0}", len);
//                PrintBytes(buffer, len);
#endif

                stream.Write(buffer,
                             0,
                             len);


                Array.Clear(buffer,
                            0,
                            bufferSize);

                httpHeaderLength = 0;
                httpBodyLength = 0;


                while(true)
                {
                    if(recvHttpHeaderFlag == false)
                    {
                        tmprec = stream.Read(buffer,
                                             rec,
                                             bufferHttpHeaderSize);
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

                        if(recvHttpHeaderFlag == false)
                        {
                            recvHttpHeaderFlag = true;

                            httpHeaderLength = GetHttpHeaderLength(buffer);
                            httpBodyLength = GetContentLength(buffer);
                            totalLength = httpHeaderLength + httpBodyLength;

#if DEBUGPRINT
                            Console.WriteLine("[+] DoHttpConnectionClient httpHeaderLength: {0}, httpBodyLength: {1}, totalLength: {2}",
                                              httpHeaderLength,
                                              httpBodyLength,
                                              totalLength);
#endif

                            if(totalLength == 0)
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] DoHttpConnectionClient error totalLength: {0}",
                                                  totalLength);
#endif
                                return -1;
                            }else if(totalLength > bufferSize)
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] DoHttpConnectionClient http size error bufferSize: {0} totalLength: {1}",
                                                  bufferSize,
                                                  totalLength);
#endif
                                return -1;
                            }

                            remainingSize = totalLength - rec;
                            if(remainingSize > 0)
                            {
                                continue;
                            }
                        }else
                        {
                            remainingSize = totalLength - rec;
                            if(remainingSize > 0)
                            {
                                continue;
                            }
                        }

                        pos = httpHeaderLength;

                        while(pos < totalLength)
                        {
                            Array.Clear(tmp,
                                        0,
                                        tmpSize);

                            Buffer.BlockCopy(buffer,
                                             pos,
                                             tmp,
                                             0,
                                             spiderMessageHeaderSize);

                            pos += spiderMessageHeaderSize;
                            spiderMessageHeader = new SpiderMessageHeader(tmp);
                            dataSize = IPAddress.NetworkToHostOrder(spiderMessageHeader.DataSize);
                            if(dataSize > SPIDER_MESSAGE_DATA_MAX_SIZE)
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] DoHttpConnectionClient spider message data size error: {0}",
                                                  dataSize);
#endif
                                return -1;
                            }

                            if(dataSize > 0)
                            {
                                Buffer.BlockCopy(buffer,
                                                 pos,
                                                 tmp,
                                                 spiderMessageHeaderSize,
                                                 dataSize);

                                pos += dataSize;
                            }

                            if(spiderMessageHeader.MessageType == 'r')
                            {
                                routingMessage = new RoutingMessage(this.PipeId,
                                                                    tmp);

                                Thread threadMessageManager = new Thread(new ParameterizedThreadStart(messageManager.PushRoutingMessage));
                                threadMessageManager.Start(routingMessage);
                            }else if(spiderMessageHeader.MessageType == 's')
                            {
                                socks5Message = new Socks5Message(tmp);

                                if((String.Compare(spiderIp.SpiderIpv4, socks5Message.DestinationIp) == 0) ||
                                   (String.Compare(spiderIp.SpiderIpv6Global, socks5Message.DestinationIp) == 0) ||
                                   (String.Compare(spiderIp.SpiderIpv6UniqueLocal, socks5Message.DestinationIp) == 0) ||
                                   (String.Compare(spiderIp.SpiderIpv6LinkLocal, socks5Message.DestinationIp) == 0))
                                {
                                    Thread threadMessageManager = new Thread(new ParameterizedThreadStart(messageManager.PushSocks5Message));
                                    threadMessageManager.Start(socks5Message);
                                }else
                                {
                                    pipe = routingManager.GetDestinationPipe(socks5Message.DestinationIp);
                                    if(pipe != null)
                                    {
                                        Thread threadPipe = new Thread(new ParameterizedThreadStart(pipe.PushSocks5Message));
                                        threadPipe.Start(socks5Message);
                                    }else
                                    {
#if DEBUGPRINT
                                        Console.WriteLine("[-] DoHttpConnectionClient cannot transfer pipe message");
#endif
                                    }
                                }
                            }else
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] DoHttpConnectionClient message type error: {0}",
                                                  (char)tmp[0]);
#endif
                                break;
                            }
                        }

                        break;
                    }else
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] DoHttpConnectionClient error tmprec: {0}",
                                          tmprec);
#endif
                        return -1;
                    }
                }
            }catch(IOException ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] DoHttpConnectionClient error: {0}",
                                      ex.Message);
#endif
                return -1;

            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] DoHttpConnectionClient error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            return 0;
        }

        public int DoHttpConnectionServer()
        {
            int rec = 0;
            int tmprec = 0;
            int len = 0;

            int timeout = 30 * 1000;    // 30s
            this.tcpClient.ReceiveTimeout = timeout;
            this.tcpClient.SendTimeout = timeout;

            int bufferSize = NODE_BUFFER_SIZE * 12;
            int tmpSize = NODE_BUFFER_SIZE;
            int bufferHttpHeaderSize = NODE_BUFFER_SIZE;
            int bufferHttpBodySize = NODE_BUFFER_SIZE * 11;

            byte[] buffer = new byte[bufferSize];
            byte[] tmp = new byte[tmpSize];
//            byte[] bufferHttpHeader = new byte[bufferHttpHeaderSize];
            byte[] bufferHttpBody = new byte[bufferHttpBodySize];

            int httpHeaderLength = 0;   // start line + headers + empty line
            int httpBodyLength = 0;     // body
            int tmpLength = 0;
            string httpHeaderString = "";
            byte[] httpHeaderByteArray = null;
            int routingMessageCount = 0;
            int socks5MessageCount = 0;
            RoutingMessage routingMessage;
            Socks5Message socks5Message;
            bool recvHttpHeaderFlag = false;
            int totalLength = 0;
            int remainingSize = 0;
            int pos = 0;
            SpiderMessageHeader spiderMessageHeader = null;
            int spiderMessageHeaderSize = (int)SpiderMessageHeader.SPIDER_MESSAGE_HEADER_SIZE;
            int dataSize = 0;
            Pipe pipe = null;


            try
            {
                while(true)
                {
                    if(recvHttpHeaderFlag == false)
                    {
                        tmprec = stream.Read(buffer,
                                             rec,
                                             bufferHttpHeaderSize);
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

                        if(recvHttpHeaderFlag == false)
                        {
                            recvHttpHeaderFlag = true;

                            httpHeaderLength = GetHttpHeaderLength(buffer);
                            httpBodyLength = GetContentLength(buffer);
                            totalLength = httpHeaderLength + httpBodyLength;

#if DEBUGPRINT
                            Console.WriteLine("[+] DoHttpConnectionServer httpHeaderLength: {0}, httpBodyLength: {1}, totalLength: {2}",
                                              httpHeaderLength,
                                              httpBodyLength,
                                              totalLength);
#endif

                            if(totalLength == 0)
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] DoHttpConnectionServer error totalLength: {0}",
                                                  totalLength);
#endif
                                return -1;
                            }else if(totalLength > bufferSize)
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] DoHttpConnectionServer http size error bufferSize: {0} totalLength: {1}",
                                                  bufferSize,
                                                  totalLength);
#endif
                                return -1;
                            }

                            remainingSize = totalLength - rec;
                            if(remainingSize > 0)
                            {
                                continue;
                            }
                        }else
                        {
                            remainingSize = totalLength - rec;
                            if(remainingSize > 0)
                            {
                                continue;
                            }
                        }

                        pos = httpHeaderLength;

                        while(pos < totalLength)
                        {
                            Array.Clear(tmp,
                                        0,
                                        tmpSize);

                            Buffer.BlockCopy(buffer,
                                             pos,
                                             tmp,
                                             0,
                                             spiderMessageHeaderSize);

                            pos += spiderMessageHeaderSize;
                            spiderMessageHeader = new SpiderMessageHeader(tmp);
                            dataSize = IPAddress.NetworkToHostOrder(spiderMessageHeader.DataSize);
                            if(dataSize > SPIDER_MESSAGE_DATA_MAX_SIZE)
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] DoHttpConnectionClient spider message data size error: {0}",
                                                  dataSize);
#endif
                                return -1;
                            }

                            if(dataSize > 0)
                            {
                                Buffer.BlockCopy(buffer,
                                                 pos,
                                                 tmp,
                                                 spiderMessageHeaderSize,
                                                 dataSize);

                                pos += dataSize;
                            }

                            if(spiderMessageHeader.MessageType == 'r')
                            {
                                routingMessage = new RoutingMessage(this.PipeId,
                                                                    tmp);

                                Thread threadMessageManager = new Thread(new ParameterizedThreadStart(messageManager.PushRoutingMessage));
                                threadMessageManager.Start(routingMessage);
                            }else if(spiderMessageHeader.MessageType == 's')
                            {
                                socks5Message = new Socks5Message(tmp);

                                if((String.Compare(spiderIp.SpiderIpv4, socks5Message.DestinationIp) == 0) ||
                                   (String.Compare(spiderIp.SpiderIpv6Global, socks5Message.DestinationIp) == 0) ||
                                   (String.Compare(spiderIp.SpiderIpv6UniqueLocal, socks5Message.DestinationIp) == 0) ||
                                   (String.Compare(spiderIp.SpiderIpv6LinkLocal, socks5Message.DestinationIp) == 0))
                                {
                                    Thread threadMessageManager = new Thread(new ParameterizedThreadStart(messageManager.PushSocks5Message));
                                    threadMessageManager.Start(socks5Message);
                                }else
                                {
                                    pipe = routingManager.GetDestinationPipe(socks5Message.DestinationIp);
                                    if(pipe != null)
                                    {
                                        Thread threadPipe = new Thread(new ParameterizedThreadStart(pipe.PushSocks5Message));
                                        threadPipe.Start(socks5Message);
                                    }else
                                    {
#if DEBUGPRINT
                                        Console.WriteLine("[-] DoHttpConnectionServer cannot transfer pipe message");
#endif
                                    }
                                }
                            }else
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] DoHttpConnectionServer message type error: {0}",
                                                  (char)tmp[0]);
#endif
                                break;
                            }
                        }

                        break;
                    }
                }


                Array.Clear(buffer,
                            0,
                            bufferSize);

                httpHeaderLength = 0;
                httpBodyLength = 0;


                routingMessage = PopLatestRoutingMessage();
                if(routingMessage != null)
                {
                    httpBodyLength = routingMessage.CopyToBuffer(ref bufferHttpBody);
                    routingMessageCount++;
                }

                socks5MessageCount = this.socks5MessagesQueue.GetCount();
                if(socks5MessageCount > 10)
                {
                    socks5MessageCount = 10;
                }

                for(int i=socks5MessageCount; i>0; i--)
                {
                    Array.Clear(tmp,
                                0,
                                tmpSize);

                    socks5Message = this.PopSocks5Message();

                    tmpLength = socks5Message.CopyToBuffer(ref tmp);

                    Buffer.BlockCopy(tmp,
                                     0,
                                     bufferHttpBody,
                                     httpBodyLength,
                                     tmpLength);

                    httpBodyLength += tmpLength;
                }

                httpHeaderString = String.Format("HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: {0}\r\n\r\n",
                                                     httpBodyLength);

                httpHeaderByteArray = Encoding.UTF8.GetBytes(httpHeaderString);
                httpHeaderLength = httpHeaderByteArray.Length;

                Buffer.BlockCopy(httpHeaderByteArray,
                                 0,
                                 buffer,
                                 0,
                                 httpHeaderLength);

                Buffer.BlockCopy(bufferHttpBody,
                                 0,
                                 buffer,
                                 httpHeaderLength,
                                 httpBodyLength);

                len = httpHeaderLength + httpBodyLength;

#if DEBUGPRINT
//                Console.WriteLine("len: {0}", len);
//                PrintBytes(buffer, len);
#endif

                stream.Write(buffer,
                             0,
                             len);

            }catch(IOException ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] DoHttpConnectionServer error: {0}",
                                      ex.Message);
#endif
                return -1;

            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] DoHttpConnectionServer error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            return 0;
        }
    }
}

