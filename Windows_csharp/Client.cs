/*
 * Title:  spider Client.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

namespace spider
{
    public class Client : Node
    {
        private const int SHELL_UPLOAD_DOWNLOAD_DATA_SIZE = 50000;
        private const int FORWARDER_UDP_TIMEOUT = 300;
        private const int SHOW_NODE_INFORMATION_WORKER_TV_SEC = 10;
        private const int SHOW_NODE_INFORMATION_WORKER_TV_USEC = 0;
        private const int SHOW_NODE_INFORMATION_WORKER_FORWARDER_TV_SEC = 10;
        private const int SHOW_NODE_INFORMATION_WORKER_FORWARDER_TV_USEC = 0;
        private const int SHOW_ROUTING_TABLE_WORKER_TV_SEC = 10;
        private const int SHOW_ROUTING_TABLE_WORKER_TV_USEC = 0;
        private const int SHOW_ROUTING_TABLE_WORKER_FORWARDER_TV_SEC = 10;
        private const int SHOW_ROUTING_TABLE_WORKER_FORWARDER_TV_USEC = 0;
        private const int ADD_NODE_TO_DESTINATION_SPIDER_WORKER_TV_SEC = 10;
        private const int ADD_NODE_TO_DESTINATION_SPIDER_WORKER_TV_USEC = 0;
        private const int ADD_NODE_TO_DESTINATION_SPIDER_WORKER_FORWARDER_TV_SEC = 60;
        private const int ADD_NODE_TO_DESTINATION_SPIDER_WORKER_FORWARDER_TV_USEC = 0;
        private const int SOCKS5_AUTHENTICATION_METHOD = 0x0;   // 0x0:No Authentication Required  0x2:Username/Password Authentication
        private const string SOCKS5_USERNAME = "socks5user";
        private const string SOCKS5_PASSWORD = "supersecretpassword";

        private string type;
        private uint connectionId;
        private uint clientId;
        private uint serverId;
        private string clientIp;
        private string clientIpScopeId;
        private string clientListenPort;
        private string clientPort;
        private string destinationSpiderIp;
        private string targetIp;
        private string targetPort;
        private int tvSec;
        private int tvUsec;
        private int forwarderTvSec;
        private int forwarderTvUsec;
        private Encryption encryption;
        private uint recvMessageId = 0;
        private uint nextRecvMessageId = 0;
        private uint sendMessageId = 0;

        public Client(string type,
                      uint connectionId,
                      uint clientId,
                      uint serverId,
                      string clientIp,
                      string clientIpScopeId,
                      string clientListenPort,
                      string clientPort,
                      string destinationSpiderIp,
                      TcpListener tcpListener,
                      TcpClient tcpClient,
                      int tvSec,
                      int tvUsec,
                      int forwarderTvSec,
                      int forwarderTvUsec,
                      Encryption encryption,
                      MessageManager messageManager)
        : base(tcpListener,
               tcpClient,
               messageManager)
        {
            this.type = type;
            this.connectionId = connectionId;
            this.clientId = clientId;
            this.serverId = serverId;
            this.clientIp = clientIp;
            this.clientIpScopeId = clientIpScopeId;
            this.clientListenPort = clientListenPort;
            this.clientPort = clientPort;
            this.destinationSpiderIp = destinationSpiderIp;
            this.tvSec = tvSec;
            this.tvUsec = tvUsec;
            this.forwarderTvSec = forwarderTvSec;
            this.forwarderTvUsec = forwarderTvUsec;
            this.encryption = encryption;
        }

        public Client(string type,
                      uint connectionId,
                      uint clientId,
                      uint serverId,
                      string clientIp,
                      string clientIpScopeId,
                      string clientListenPort,
                      string clientPort,
                      string destinationSpiderIp,
                      string targetIp,
                      string targetPort,
                      TcpListener tcpListener,
                      TcpClient tcpClient,
                      int tvSec,
                      int tvUsec,
                      int forwarderTvSec,
                      int forwarderTvUsec,
                      Encryption encryption,
                      MessageManager messageManager)
        : base(tcpListener,
               tcpClient,
               messageManager)
        {
            this.type = type;
            this.connectionId = connectionId;
            this.clientId = clientId;
            this.serverId = serverId;
            this.clientIp = clientIp;
            this.clientIpScopeId = clientIpScopeId;
            this.clientListenPort = clientListenPort;
            this.clientPort = clientPort;
            this.destinationSpiderIp = destinationSpiderIp;
            this.targetIp = targetIp;
            this.targetPort = targetPort;
            this.tvSec = tvSec;
            this.tvUsec = tvUsec;
            this.forwarderTvSec = forwarderTvSec;
            this.forwarderTvUsec = forwarderTvUsec;
            this.encryption = encryption;
        }

        public Client(string type,
                      uint connectionId,
                      uint clientId,
                      uint serverId,
                      string clientIp,
                      string clientIpScopeId,
                      string clientListenPort,
                      string clientPort,
                      string destinationSpiderIp,
                      string targetIp,
                      string targetPort,
                      UdpClient udpClient,
                      int tvSec,
                      int tvUsec,
                      int forwarderTvSec,
                      int forwarderTvUsec,
                      Encryption encryption,
                      MessageManager messageManager)
        : base(udpClient,
               messageManager)
        {
            this.type = type;
            this.connectionId = connectionId;
            this.clientId = clientId;
            this.serverId = serverId;
            this.clientIp = clientIp;
            this.clientIpScopeId = clientIpScopeId;
            this.clientListenPort = clientListenPort;
            this.clientPort = clientPort;
            this.destinationSpiderIp = destinationSpiderIp;
            this.targetIp = targetIp;
            this.targetPort = targetPort;
            this.tvSec = tvSec;
            this.tvUsec = tvUsec;
            this.forwarderTvSec = forwarderTvSec;
            this.forwarderTvUsec = forwarderTvUsec;
            this.encryption = encryption;
        }

        ~Client()
        {

        }

        public string Type
        {
            get { return type; }
            set { type = value; }
        }

        public uint ConnectionId
        {
            get { return connectionId; }
            set { connectionId = value; }
        }

        public uint ClientId
        {
            get { return clientId; }
            set { clientId = value;}
        }

        public uint ServerId
        {
            get { return serverId; }
            set { serverId = value; }
        }

        public string ClientIp
        {
            get { return clientIp; }
            set { clientIp = value; }
        }

        public string ClientIpScopeId
        {
            get { return clientIpScopeId; }
            set { clientIpScopeId = value; }
        }

        public string ClientListenPort
        {
            get { return clientListenPort; }
            set { clientListenPort = value; }
        }

        public string ClientPort
        {
            get { return clientPort; }
            set { clientPort = value; }
        }

        public string DestinationSpiderIp
        {
            get { return destinationSpiderIp; }
            set { destinationSpiderIp = value; }
        }

        public string TargetIp
        {
            get { return targetIp; }
            set { targetIp = value; }
        }

        public string TargetPort
        {
            get { return targetPort; }
            set { targetPort = value; }
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

        public Encryption Encryption
        {
            get { return encryption; }
            set { encryption = value; }
        }

        private int RecvMessage(byte[] buffer,
                                int bufferSize,
                                int tvSec,
                                int tvUsec,
                                bool registerServerIdFlag)
        {
            int ret = 0;
            char messageType;
            int rec = 0;
            Socks5Message socks5Message;
            byte[] data;


            Array.Clear(buffer,
                        0,
                        bufferSize);

            socks5Message = PopTimeoutSocks5Message(tvSec,
                                                    tvUsec);
            if(socks5Message != null)
            {
                messageType = socks5Message.MessageType;
                if(messageType == 's')  // socks5 message
                {
                    rec = socks5Message.DataSize;
                    if(rec >= 0)
                    {
                        if(registerServerIdFlag == true)
                        {
                            serverId = socks5Message.ServerId;
                        }

                        recvMessageId = socks5Message.MessageId;

                        data = socks5Message.Data;
                        for(int i = 0; i < socks5Message.DataSize; i++)
                        {
                            buffer[i] = data[i];
                        }

#if DEBUGPRINT
                            PrintBytes(buffer,
                                       rec);
#endif

                            if(encryption != null &&
                               encryption.Flag)
                            {
                                ret = encryption.Decrypt(ref buffer,
                                                         rec,
                                                         bufferSize);
                                if(ret <= 0)
                                {
#if DEBUGPRINT
                                    Console.WriteLine("[-] RecvMessage decrypt error: {0}",
                                                      ret);
#endif
                                    return -1;
                                }

                                rec = ret;
                            }
                    }else
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] RecvMessage recv error: {0}",
                                          rec);
#endif
                        return -1;
                    }
                }else
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] RecvMessage unknown message type: {0}",
                                      messageType);
#endif
                    return -1;
                }
            }else
            {
#if DEBUGPRINT
                Console.WriteLine("[-] RecvMessage error");
#endif
                return -1;
            }

            return rec;
        }

        private int SendMessage(byte[] buffer,
                                int dataSize,
                                int tvSec,
                                int tvUsec)
        {
            int ret = 0;
            int sen = 0;
            Socks5Message socks5Message;

            if(dataSize > SPIDER_MESSAGE_DATA_SIZE)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] SendMessage data size error: {0}",
                                  dataSize);
#endif
                return -1;
            }

            if(encryption != null &&
               encryption.Flag)
            {
                ret = encryption.Encrypt(ref buffer,
                                         dataSize,
                                         SPIDER_MESSAGE_DATA_MAX_SIZE);
                if(ret <= 0)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] SendMessage encrypt error: {0}",
                                      ret);
#endif
                    return -1;
                }

                dataSize = ret;
            }

#if DEBUGPRINT
            PrintBytes(buffer,
                       dataSize);
#endif

            socks5Message = new Socks5Message('s',
                                              this.sendMessageId,
                                              this.connectionId,
                                              this.clientId,
                                              this.serverId,
                                              'c',
                                              this.clientIp,
                                              's',
                                              this.destinationSpiderIp,
                                              this.tvSec,
                                              this.tvUsec,
                                              this.forwarderTvSec,
                                              this.forwarderTvUsec,
                                              dataSize,
                                              buffer);

            ret = messageManager.PushTimeoutSocks5Message(socks5Message,
                                                          tvSec,
                                                          tvUsec);
            if(ret < 0)
            {
                sen = -1;
            }else
            {
                sen = dataSize;
            }

            return sen;
        }

        private int RecvReceiveMessage(uint messageId,
                                       int tvSec,
                                       int tvUsec)
        {
            Socks5Message socks5Message;
            DateTime start = DateTime.Now;
            DateTime end;
            TimeSpan d;
            long timeout = (long)tvSec * 1000000 + (long)tvUsec;


            while(true)
            {
                socks5Message = GetSocks5ReceiveMessageFromMap(messageId);
                if(socks5Message != null)
                {
                    if(socks5Message.ReceiveFlag == 1 &&
                       socks5Message.ReceiveResult == 0 &&     // ok
                       socks5Message.CommandResult == 0)       // command ok
                    {
                        break;
                    }else if(socks5Message.ReceiveFlag == 1 &&
                             socks5Message.ReceiveResult == 1 &&     // ng
                             socks5Message.CommandResult == 0)       // command ok
                    {
                        return 1;
                    }else if(socks5Message.ReceiveFlag == 1 &&
                             socks5Message.ReceiveResult == 0 &&     // ok
                             socks5Message.CommandResult == 1)       // command ng
                    {
                        return 2;
                    }else if(socks5Message.ReceiveFlag == 1 &&
                             socks5Message.ReceiveResult == 1 &&     // ng
                             socks5Message.CommandResult == 1)       // command ng
                    {
                        return 3;
                    }else
                    {
                        return -1;
                    }
                }else
                {
                    end = DateTime.Now;
                    d = end - start;
                    if((long)(d.TotalMilliseconds * 1000) >= timeout)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[+] RecvReceiveMessage timeout");
#endif
                        return 5;
                    }

                    Thread.Sleep(5); // 5ms
                }
            }

            return 0;
        }

        private int SendReceiveMessage(uint messageId,
                                       byte receiveFlag,
                                       byte receiveResult,
                                       byte commandResult,
                                       int tvSec,
                                       int tvUsec)
        {
            int ret = 0;
            Socks5Message socks5Message = new Socks5Message('s',
                                                            receiveFlag,
                                                            receiveResult,
                                                            commandResult,
                                                            messageId,
                                                            this.connectionId,
                                                            this.clientId,
                                                            this.serverId,
                                                            'c',
                                                            this.clientIp,
                                                            's',
                                                            this.destinationSpiderIp);


            ret = messageManager.PushTimeoutSocks5Message(socks5Message,
                                                          tvSec,
                                                          tvUsec);
            if(ret < 0)
            {
                return -1;
            }

            return ret;
        }

        private void ForwarderRecvData()
        {
            int ret = 0;
            int rec = 0;
            int sen = 0;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            int bufferMaxLength = NODE_BUFFER_SIZE;
            int socks5MessageDataMaxSize = SPIDER_MESSAGE_DATA_SIZE;


            while(true)
            {
                try
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] [client -> client] ForwarderRecvData Read");
#endif
                    Array.Clear(buffer,
                                0,
                                bufferMaxLength);

                    rec = stream.Read(buffer,
                                      0,
                                      socks5MessageDataMaxSize);
                    if(rec > 0)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[+] [client -> server] ForwarderRecvData SendMessage({0})",
                                          sendMessageId);
#endif
                        sen = SendMessage(buffer,
                                          rec,
                                          forwarderTvSec,
                                          forwarderTvUsec);
                        if(sen <= 0)
                        {
                            break;
                        }

#if DEBUGPRINT
                        Console.WriteLine("[+] [client <- server] ForwarderRecvData RecvReceiveMessage({0})",
                                          sendMessageId);
#endif
                        ret = RecvReceiveMessage(sendMessageId,
                                                 forwarderTvSec,
                                                 forwarderTvUsec);
                        if(ret == 0)    // ok
                        {
#if DEBUGPRINT
                            Console.WriteLine("[+] [client <- server] ForwarderRecvData RecvReceiveMessage({0}) ok",
                                              sendMessageId);
#endif
                        }else if(ret == 1)    // ng
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] [client <- server] ForwarderRecvData RecvReceiveMessage({0}) ng",
                                              sendMessageId);
#endif
                            break;
                        }else
                        {
                            break;
                        }

                        sendMessageId++;
                    }else{
#if DEBUGPRINT
                        Console.WriteLine("[-] [client -> client] ForwarderRecvData Read error rec: {0}",
                                          rec);
#endif
                        break;
                    }
                }catch(IOException ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderRecvData timeout: {0}",
                                      ex.Message);
#endif
                    break;
                }catch (Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderRecvData error: {0}",
                                      ex.Message);
#endif
                    break;
                }
            }

            return;
        }

        private void ForwarderSendData()
        {
            int ret = 0;
            int rec = 0;
//          int sen = 0;
            int length = 0;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            int bufferMaxLength = NODE_BUFFER_SIZE;
            recvMessageId = 0;


            while(true)
            {
                try
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] [client <- server] ForwarderSendData RecvMessage");
#endif
                    Array.Clear(buffer,
                                0,
                                bufferMaxLength);

                    rec = RecvMessage(buffer,
                                      bufferMaxLength,
                                      forwarderTvSec,
                                      forwarderTvUsec,
                                      false);
                    if(rec > 0)
                    {
                        if(recvMessageId == nextRecvMessageId)
                        {
#if DEBUGPRINT
                            Console.WriteLine("[+] [client -> server] ForwarderSendData SendReceiveMessage({0}) ok",
                                              recvMessageId);
#endif
                            ret = SendReceiveMessage(recvMessageId,
                                                     1,   // received
                                                     0,   // ok
                                                     0,
                                                     forwarderTvSec,
                                                     forwarderTvUsec);
                            if(ret < 0)
                            {
                                break;
                            }

                            length = rec;
#if DEBUGPRINT
                            Console.WriteLine("[+] [client <- client] ForwarderSendData Write({0})",
                                              recvMessageId);
#endif
                            stream.Write(buffer,
                                         0,
                                         length);

                            nextRecvMessageId++;
                        }else
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] [client -> server] ForwarderSendData SendReceiveMessage({0}) ng",
                                              recvMessageId);
#endif
                            ret = SendReceiveMessage(recvMessageId,
                                                     1,   // received
                                                     1,   // ng
                                                     0,
                                                     forwarderTvSec,
                                                     forwarderTvUsec);
                            break;
                        }
                    }else
                    {
                        break;
                    }
                }catch(IOException ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderSendData timeout: {0}",
                                      ex.Message);
#endif
                    break;
                }catch (Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderSendData error: {0}",
                                      ex.Message);
#endif
                    break;
                }
            }

            return;
        }

        private int Forwarder()
        {
            int timeout = forwarderTvSec * 1000 + ForwarderTvUsec / 1000;
            this.tcpClient.ReceiveTimeout = timeout;
            this.tcpClient.SendTimeout = timeout;

            Thread thread1 = new Thread(new ThreadStart(ForwarderRecvData));
            Thread thread2 = new Thread(new ThreadStart(ForwarderSendData));

            thread1.Start();
            thread2.Start();

            thread1.Join();
            thread2.Join();

            return 0;
        }

        public int DoSocks5Connection()
        {
            int rec;
            int sen;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            int bufferMaxLength = NODE_BUFFER_SIZE;
            int socks5MessageDataMaxSize = SPIDER_MESSAGE_DATA_SIZE;
            recvMessageId = 0;
            nextRecvMessageId = 0;
            sendMessageId = GenerateRandomId();

            SelectionResponse selectionResponse;


            // socks SELECTION_REQUEST [client -> client]
#if DEBUGPRINT
            Console.WriteLine("[+] [client -> client] recv selection request");
#endif
            rec = RecvData(buffer,
                           socks5MessageDataMaxSize,
                           tvSec,
                           tvUsec);
            if(rec <= 0)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client -> client] recv selection request error");
#endif
                return -1;
            }
#if DEBUGPRINT
                Console.WriteLine("[+] [client -> client] recv selection request: {0} bytes",
                                  rec);
#endif


            // socks SELECTION_REQUEST [client -> server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send selection request\n");
#endif
            sendMessageId++;
            sen = SendMessage(buffer,
                              rec,
                              tvSec,
                              tvUsec);

#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send selection request: {0} bytes",
                              sen);
#endif


            // socks SELECTION_RESPONSE [client <- server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv selection response");
#endif

            rec = RecvMessage(buffer,
                              bufferMaxLength,
                              tvSec,
                              tvUsec,
                              true);
            if(rec != SelectionResponse.SELECTION_RESPONSE_SIZE)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client <- server] recv selection response error");
#endif
                return -1;
            }

            nextRecvMessageId = recvMessageId + 1;

#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv selection response: {0} bytes",
                              rec);
#endif


            // socks SELECTION_RESPONSE [client <- client]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <- client] send selection response");
#endif
            sen = SendData(buffer,
                           rec,
                           tvSec,
                           tvUsec);
#if DEBUGPRINT
            Console.WriteLine("[+] [client <- client] send selection response: {0} bytes",
                              sen);
#endif
            try
            {
                selectionResponse = new SelectionResponse(buffer);
            }catch(Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] SelectionResponse error: {0}",
                                  ex.Message);
#endif
                return -1;
            }
            if(selectionResponse.Method == 0xFF)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socks5server authentication method error");
#endif
            }

            if(selectionResponse.Method  == 0x2)    // USERNAME_PASSWORD_AUTHENTICATION
            {
                // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST client -> client
#if DEBUGPRINT
                Console.WriteLine("[+] [client -> client] recv username password authentication request");
#endif
                rec = RecvData(buffer,
                               socks5MessageDataMaxSize,
                               tvSec,
                               tvUsec);
                if(rec <= 0)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] [client -> client] recv username password authentication request error");
#endif
                    return -1;
                }
#if DEBUGPRINT
                Console.WriteLine("[+] [client -> client] recv username password authentication request: {0} bytes",
                                  rec);
#endif


                // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST [client -> server]
#if DEBUGPRINT
                Console.WriteLine("[+] [client -> server] send username password authentication request");
#endif

                sendMessageId++;
                sen = SendMessage(buffer,
                                  rec,
                                  tvSec,
                                  tvUsec);

#if DEBUGPRINT
                Console.WriteLine("[+] [client -> server] send username password authentication request: {0} bytes",
                            sen);
#endif


                // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- server]
#if DEBUGPRINT
                Console.WriteLine("[+] [client <- server] recv username password authentication response");
#endif

                rec = RecvMessage(buffer,
                                  bufferMaxLength,
                                  tvSec,
                                  tvUsec,
                                  false);
                if(rec <= 0 ||
                   nextRecvMessageId != recvMessageId)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] [client <- server] recv username password authentication response error");
#endif
                    return -1;
                }

                nextRecvMessageId++;

#if DEBUGPRINT
                Console.WriteLine("[+] [client <- server] recv username password authentication response: {0} bytes",
                                  rec);
#endif


                // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- client]
#if DEBUGPRINT
                Console.WriteLine("[+] [client <- client] send username password authentication response");
#endif
                sen = SendData(buffer,
                               rec,
                               tvSec,
                               tvUsec);
#if DEBUGPRINT
                Console.WriteLine("[+] [client <- client] send username password authentication response: {0} bytes",
                                  sen);
#endif
            }


            // socks SOCKS_REQUEST [client -> client]
#if DEBUGPRINT
            Console.WriteLine("[+] [client -> client] recv socks request");
#endif
            rec = RecvData(buffer,
                           socks5MessageDataMaxSize,
                           tvSec,
                           tvUsec);
            if(rec <= 0)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client -> client] recv socks request error");
#endif
                return -1;
            }
#if DEBUGPRINT
            Console.WriteLine("[+] [client -> client] recv socks request: {0} bytes",
                              rec);
#endif


            // socks SOCKS_REQUEST [client -> server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send socks request");
#endif

            sendMessageId++;
            sen = SendMessage(buffer,
                              rec,
                              tvSec,
                              tvUsec);

#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send socks request: {0} bytes",
                              sen);
#endif


            // socks SOCKS_RESPONSE [client <- server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv socks response");
#endif
            rec = RecvMessage(buffer,
                              bufferMaxLength,
                              tvSec,
                              tvUsec,
                              false);
            if(rec <= 0 ||
               nextRecvMessageId != recvMessageId)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client <- server] recv socks response error");
#endif
                return -1;
            }

            nextRecvMessageId++;

#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv socks response: {0} bytes",
                              rec);
#endif


            // socks SOCKS_RESPONSE [client <- client]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <- client] send socks response");
#endif
            sen = SendData(buffer,
                           rec,
                           tvSec,
                           tvUsec);
#if DEBUGPRINT
            Console.WriteLine("[+] [client <- client] send socks response: {0} bytes",
                              sen);
#endif


            // forwarder [client <> client <> server <> target]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <> client <> server <> target] forwarder");
#endif

            sendMessageId++;
            Forwarder();

#if DEBUGPRINT
            Console.WriteLine("[+] worker exit");
#endif

            return -1;
        }

        private string[] SplitInput(byte[] input)
        {
            string inputString;
            string[] tokens;


            inputString = Encoding.UTF8.GetString(input);
            tokens = inputString.Split(' ');

            return tokens;
        }

        private void ForwarderShellRecvData()
        {
            int ret = 0;
            int rec = 0;
            int sen = 0;
            int length = 0;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            byte[] data = new byte[NODE_BUFFER_SIZE];
            byte[] tmp;
            int bufferMaxLength = NODE_BUFFER_SIZE;
            int socks5MessageDataMaxSize = SPIDER_MESSAGE_DATA_SIZE;

            string result = "";
            string prompt = "\ncommand >";
            byte[] input;
            string[] tokens;
            string uploadCommand = "upload";
            FileStream fileStream = null;
            string uploadFileName = "";
            string uploadFilePath = "";
            ulong uploadFileSize = 0;
            ulong uploadFileRemainingSize = 0;
            ulong readBytes = 0;
            ulong dataSize = 0;
            UploadDownloadData uploadDownloadData;
            bool errorFlag = false;


            try
            {
                // output prompt
                Array.Clear(buffer,
                            0,
                            bufferMaxLength);
                tmp = Encoding.UTF8.GetBytes(prompt);
                length = tmp.Length;
                for(int i = 0; i < length; i++)
                {
                    buffer[i] = tmp[i];
                }

#if DEBUGPRINT
                Console.WriteLine("[+] [client <- client] ForwarderShellRecvData Write");
#endif
                stream.Write(buffer,
                             0,
                             length);
                sen = length;
            }catch(IOException ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] ForwarderShellRecvData timeout: {0}",
                                  ex.Message);
#endif
                return;
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] ForwarderShellRecvData error: {0}",
                                  ex.Message);
#endif
                return;
            }

            while(true)
            {
                try
                {
                    if(errorFlag == true)
                    {
                        break;
                    }

#if DEBUGPRINT
                    Console.WriteLine("[+] [client -> client] ForwarderShellRecvData Read");
#endif
                    Array.Clear(buffer,
                                0,
                                bufferMaxLength);

                    rec = stream.Read(buffer,
                                      0,
                                      socks5MessageDataMaxSize);
                    if(rec > 0)
                    {
                        input = buffer.Where(b => b != 0x00).ToArray();;
                        tokens = SplitInput(input);

                        if(tokens.Length == 3 &&
                           (String.Compare(tokens[0].Replace("\r\n", "").Replace("\n", "").Replace("\r", ""), uploadCommand) == 0))
                        {
                            fileStream = null;

                            try
                            {
                                fileStream = new FileStream(tokens[1],
                                                            FileMode.Open,
                                                            FileAccess.Read);

                                uploadFileSize = (ulong)fileStream.Length;
                                uploadFileRemainingSize = uploadFileSize;

                                string path = Path.GetDirectoryName(tokens[1]);
                                uploadFileName = Path.GetFileName(tokens[1]);
                                uploadFilePath = tokens[2].Replace("\r\n", "").Replace("\n", "").Replace("\r", "");

                                while(uploadFileRemainingSize > 0)
                                {
                                    Array.Clear(buffer,
                                                0,
                                                bufferMaxLength);

                                    Array.Clear(data,
                                                0,
                                                bufferMaxLength);

                                    readBytes = 0;
                                    dataSize = 0;

                                    if(uploadFileRemainingSize <= SHELL_UPLOAD_DOWNLOAD_DATA_SIZE)
                                    {
                                        readBytes = (ulong)fileStream.Read(data,
                                                                           0,
                                                                           (int)uploadFileRemainingSize);
                                        dataSize = uploadFileRemainingSize;
                                    }else
                                    {
                                        readBytes = (ulong)fileStream.Read(data,
                                                                           0,
                                                                           SHELL_UPLOAD_DOWNLOAD_DATA_SIZE);
                                        dataSize = SHELL_UPLOAD_DOWNLOAD_DATA_SIZE;
                                    }

                                    uploadDownloadData = new UploadDownloadData(uploadCommand,
                                                                                uploadFileName,
                                                                                uploadFilePath,
                                                                                uploadFileSize,
                                                                                dataSize,
                                                                                data);

                                    length = uploadDownloadData.CopyToBuffer(ref buffer);

#if DEBUGPRINT
                                    Console.WriteLine("[+] [client -> server] ForwarderShellRecvData SendMessage({0})",
                                                      sendMessageId);
#endif

                                    sen = SendMessage(buffer,
                                                      length,
                                                      forwarderTvSec,
                                                      forwarderTvUsec);
                                    if(sen <= 0)
                                    {
                                        errorFlag = true;
                                        break;
                                    }

#if DEBUGPRINT
                                    Console.WriteLine("[+] [client <- server] ForwarderShellRecvData RecvReceiveMessage({0})",
                                            sendMessageId);
#endif
                                    ret = RecvReceiveMessage(sendMessageId,
                                                             forwarderTvSec,
                                                             forwarderTvUsec);
                                    if(ret == 0)    // ok
                                    {
#if DEBUGPRINT
                                        Console.WriteLine("[+] [client <- server] ForwarderShellRecvData RecvReceiveMessage({0}) ok",
                                                          sendMessageId);
#endif
                                    }else if(ret == 1 ||
                                             ret == 3)    // ng
                                    {
#if DEBUGPRINT
                                        Console.WriteLine("[-] [client <- server] ForwarderShellRecvData RecvReceiveMessage({0}) ng",
                                                          sendMessageId);
#endif
                                        Array.Clear(buffer,
                                                    0,
                                                    bufferMaxLength);

                                        result = "[-] ForwarderShellRecvData upload error";
                                        result += prompt;

                                        tmp = Encoding.UTF8.GetBytes(result);
                                        length = tmp.Length;
                                        for(int i = 0; i < length; i++)
                                        {
                                            buffer[i] = tmp[i];
                                        }

#if DEBUGPRINT
                                        Console.WriteLine("[+] [client <- client] ForwarderShellRecvData Write");
#endif
                                        stream.Write(buffer,
                                                     0,
                                                     length);
                                        sen = length;

                                        errorFlag = true;
                                        break;
                                    }else if(ret == 2)  // command ng
                                    {
#if DEBUGPRINT
                                        Console.WriteLine("[-] [client <- server] ForwarderShellRecvData RecvReceiveMessage({0}) command ng",
                                                          sendMessageId);
#endif
                                        Array.Clear(buffer,
                                                    0,
                                                    bufferMaxLength);

                                        result = "[-] ForwarderShellRecvData upload error";
                                        result += prompt;

                                        tmp = Encoding.UTF8.GetBytes(result);
                                        length = tmp.Length;
                                        for(int i = 0; i < length; i++)
                                        {
                                            buffer[i] = tmp[i];
                                        }

#if DEBUGPRINT
                                        Console.WriteLine("[+] [client <- client] ForwarderShellRecvData Write");
#endif
                                        stream.Write(buffer,
                                                     0,
                                                     length);
                                        sen = length;

                                        uploadFileName = "";
                                        uploadFilePath = "";
                                        uploadFileSize = 0;
                                        uploadFileRemainingSize = 0;
                                        readBytes = 0;

                                        break;
                                    }else
                                    {
#if DEBUGPRINT
                                        Console.WriteLine("[-] [client <- server] ForwarderShellRecvData RecvReceiveMessage({0}) ret: {1}",
                                                          sendMessageId,
                                                          ret);
#endif

                                        Array.Clear(buffer,
                                                    0,
                                                    bufferMaxLength);

                                        result = "[-] ForwarderShellRecvData upload error";
                                        result += prompt;

                                        tmp = Encoding.UTF8.GetBytes(result);
                                        length = tmp.Length;
                                        for(int i = 0; i < length; i++)
                                        {
                                            buffer[i] = tmp[i];
                                        }

#if DEBUGPRINT
                                        Console.WriteLine("[+] [client <- client] ForwarderShellRecvData Write");
#endif
                                        stream.Write(buffer,
                                                     0,
                                                     length);
                                        sen = length;

                                        errorFlag = true;
                                        break;
                                    }

                                    sendMessageId++;
                                    uploadFileRemainingSize -= readBytes;

//                                    Thread.Sleep(1);    // 1ms
                                }

                                if(sen <= 0)
                                {
                                    errorFlag = true;
                                    break;
                                }
                            }catch(Exception ex)
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] ForwarderShellRecvData upload file error: {0}",
                                                  ex.Message);
#endif
                                Array.Clear(buffer,
                                            0,
                                            bufferMaxLength);

                                result = "[-] ForwarderShellRecvData upload file error";
                                result += prompt;

                                tmp = Encoding.UTF8.GetBytes(result);
                                length = tmp.Length;
                                for(int i = 0; i < length; i++)
                                {
                                    buffer[i] = tmp[i];
                                }

#if DEBUGPRINT
                                Console.WriteLine("[+] [client <- client] ForwarderShellRecvData Write");
#endif
                                stream.Write(buffer,
                                             0,
                                             length);
                                sen = length;

                                errorFlag = true;
                                break;
                            }finally
                            {
                                if(fileStream != null)
                                {
                                    fileStream.Close();
                                    fileStream = null;
                                }
                            }
                        }else
                        {
                            length = rec;

#if DEBUGPRINT
                            Console.WriteLine("[+] [client -> server] ForwarderShellRecvData SendMessage({0})",
                                               sendMessageId);
#endif
                            sen = SendMessage(buffer,
                                              length,
                                              forwarderTvSec,
                                              forwarderTvUsec);
                            if(sen <= 0)
                            {
                                break;
                            }

#if DEBUGPRINT
                            Console.WriteLine("[+] [client <- server] ForwarderShellRecvData RecvReceiveMessage({0})",
                                              sendMessageId);
#endif
                            ret = RecvReceiveMessage(sendMessageId,
                                                     forwarderTvSec,
                                                     forwarderTvUsec);
                            if(ret == 0)    // ok
                            {
#if DEBUGPRINT
                                Console.WriteLine("[+] [client <- server] ForwarderShellRecvData RecvReceiveMessage({0}) ok",
                                                   sendMessageId);
#endif
                            }else if(ret == 1 ||
                                     ret == 3)  // ng
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] [client <- server] ForwarderShellRecvData RecvReceiveMessage({0}) ng",
                                                  sendMessageId);
#endif

                                Array.Clear(buffer,
                                            0,
                                            bufferMaxLength);

                                result = "[-] ForwarderShellRecvData error";
                                result += prompt;

                                tmp = Encoding.UTF8.GetBytes(result);
                                length = tmp.Length;
                                for(int i = 0; i < length; i++)
                                {
                                    buffer[i] = tmp[i];
                                }

#if DEBUGPRINT
                                Console.WriteLine("[+] [client <- client] ForwarderShellRecvData Write");
#endif
                                stream.Write(buffer,
                                             0,
                                             length);
                                sen = length;

                                errorFlag = true;
                                break;
                            }else if(ret == 2)  // command ng
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] [client <- server] ForwarderShellRecvData RecvReceiveMessage({0}) command ng",
                                                  sendMessageId);
#endif

                                Array.Clear(buffer,
                                            0,
                                            bufferMaxLength);

                                result = "[-] ForwarderShellRecvData command error";
                                result += prompt;

                                tmp = Encoding.UTF8.GetBytes(result);
                                length = tmp.Length;
                                for(int i = 0; i < length; i++)
                                {
                                    buffer[i] = tmp[i];
                                }

#if DEBUGPRINT
                                Console.WriteLine("[+] [client <- client] ForwarderShellRecvData Write");
#endif
                                stream.Write(buffer,
                                             0,
                                             length);
                                sen = length;
                            }else
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] [client <- server] ForwarderShellRecvData RecvReceiveMessage({0}) ret: {1}",
                                                  sendMessageId,
                                                  ret);
#endif

                                Array.Clear(buffer,
                                            0,
                                            bufferMaxLength);

                                result = "[-] ForwarderShellRecvData error";
                                result += prompt;

                                tmp = Encoding.UTF8.GetBytes(result);
                                length = tmp.Length;
                                for(int i = 0; i < length; i++)
                                {
                                    buffer[i] = tmp[i];
                                }

#if DEBUGPRINT
                                Console.WriteLine("[+] [client <- client] ForwarderShellRecvData Write");
#endif
                                stream.Write(buffer,
                                             0,
                                             length);
                                sen = length;

                                errorFlag = true;
                                break;
                            }

                            sendMessageId++;
                        }
                    }else
                    {
                        errorFlag = true;
                        break;
                    }
                }catch(IOException ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderShellRecvData timeout: {0}",
                                      ex.Message);
#endif
                    break;
                }catch (Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderShellRecvData error: {0}",
                                      ex.Message);
#endif
                    break;
                }
            }

            return;
        }

        private void ForwarderShellSendData()
        {
            int ret = 0;
            int rec = 0;
            int sen = 0;
            int length = 0;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            byte[] data;
            byte[] tmp;
            int bufferMaxLength = NODE_BUFFER_SIZE;
            recvMessageId = 0;

            string result = "";
            string prompt = "\ncommand >";
            byte[] commandTmp;
            string command = "";
            string downloadCommand = "download";
            FileStream fileStream = null;
            string downloadFileName = "";
            string downloadFilePath = "";
            ulong downloadFileSize = 0;
            ulong recvDownloadFileDataSize = 0;
            ulong downloadFileRemainingSize = 0;
            bool downloadCommandFlag = false;
            bool downloadFileFlag = false;
            UploadDownloadDataHeader uploadDownloadDataHeader;
            UploadDownloadData uploadDownloadData;
            bool errorFlag = false;


            while(true)
            {
                if(errorFlag == true)
                {
                    break;
                }

                try
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] [client <- server] ForwarderShellSendData RecvMessage");
#endif
                    Array.Clear(buffer,
                                0,
                                bufferMaxLength);

                    rec = RecvMessage(buffer,
                                      bufferMaxLength,
                                      forwarderTvSec,
                                      forwarderTvUsec,
                                      false);
                    if(rec > 0)
                    {
                        if(recvMessageId == nextRecvMessageId)
                        {
                            downloadCommandFlag = false;

                            try
                            {
                                uploadDownloadDataHeader = new UploadDownloadDataHeader(buffer);
                                commandTmp = uploadDownloadDataHeader.Command.Where(b => b != 0x00).ToArray();
                                command = Encoding.UTF8.GetString(commandTmp);
                                if(String.Compare(command, downloadCommand) == 0)
                                {
                                    downloadCommandFlag = true;
                                }else
                                {
                                    downloadCommandFlag = false;
                                }
                            }catch(Exception ex)
                            {
                                downloadCommandFlag = false;
                            }

                            if(downloadCommandFlag == true)
                            {
                                try
                                {
                                    if(downloadFileFlag == false)
                                    {
                                        downloadFileFlag = true;
                                        fileStream = null;
                                        uploadDownloadData = new UploadDownloadData(buffer);

                                        tmp = uploadDownloadData.FilePath.Where(b => b != 0x00).ToArray();
                                        downloadFilePath = Encoding.UTF8.GetString(tmp);
                                        downloadFilePath += "\\";

                                        tmp = uploadDownloadData.FileName.Where(b => b != 0x00).ToArray();
                                        downloadFileName = Encoding.UTF8.GetString(tmp);
                                        downloadFileName = downloadFilePath + downloadFileName;
                                        downloadFileSize = uploadDownloadData.FileSize;

                                        fileStream = new FileStream(downloadFileName,
                                                                    FileMode.Create,
                                                                    FileAccess.Write);

                                        recvDownloadFileDataSize = uploadDownloadData.DataSize;
                                        data = uploadDownloadData.Data;

                                        fileStream.Write(data,
                                                         0,
                                                         (int)recvDownloadFileDataSize);

                                        downloadFileRemainingSize = downloadFileSize - recvDownloadFileDataSize;
                                        if(downloadFileRemainingSize > 0)
                                        {
#if DEBUGPRINT
                                            Console.WriteLine("[+] [client -> server] ForwarderShellSendData SendReceiveMessage({0}) ok",
                                                              recvMessageId);
#endif
                                            ret = SendReceiveMessage(recvMessageId,
                                                                     1,   // received
                                                                     0,   // ok
                                                                     0,
                                                                     forwarderTvSec,
                                                                     forwarderTvUsec);
                                            if(ret < 0)
                                            {
                                                fileStream.Close();
                                                errorFlag = true;
                                                break;
                                            }

                                            nextRecvMessageId++;
                                            continue;
                                        }
                                    }else
                                    {
                                        uploadDownloadData = new UploadDownloadData(buffer);

                                        recvDownloadFileDataSize = uploadDownloadData.DataSize;
                                        data = uploadDownloadData.Data;

                                        fileStream.Write(data,
                                                         0,
                                                         (int)recvDownloadFileDataSize);

                                        downloadFileRemainingSize -= recvDownloadFileDataSize;
                                        if(downloadFileRemainingSize > 0)
                                        {
#if DEBUGPRINT
                                            Console.WriteLine("[+] [client -> server] ForwarderShellSendData SendReceiveMessage({0}) ok",
                                                              recvMessageId);
#endif
                                            ret = SendReceiveMessage(recvMessageId,
                                                                     1,   // received
                                                                     0,   // ok
                                                                     0,
                                                                     forwarderTvSec,
                                                                     forwarderTvUsec);
                                            if(ret < 0)
                                            {
                                                fileStream.Close();
                                                errorFlag = true;
                                                break;
                                            }

                                            nextRecvMessageId++;
                                            continue;
                                        }
                                    }
                                }catch(Exception ex)
                                {
#if DEBUGPRINT
                                    Console.WriteLine("[-] [client -> server] ForwarderShellSendData SendReceiveMessage({0}) command ng",
                                                      recvMessageId);
#endif
                                    ret = SendReceiveMessage(recvMessageId,
                                                             1,   // received
                                                             0,   // ok
                                                             1,   // command ng
                                                             forwarderTvSec,
                                                             forwarderTvUsec);
                                    if(ret < 0)
                                    {
                                        errorFlag = true;
                                        break;
                                    }

                                    nextRecvMessageId++;

#if DEBUGPRINT
                                    Console.WriteLine("[-] ForwarderShellSendData download file error: {0}",
                                                      ex.Message);
#endif
                                    Array.Clear(buffer,
                                                0,
                                                bufferMaxLength);

                                    result = "[-] ForwarderShellSendData download file error";
                                    result += prompt;

                                    tmp = Encoding.UTF8.GetBytes(result);
                                    length = tmp.Length;
                                    for(int i = 0; i < length; i++)
                                    {
                                        buffer[i] = tmp[i];
                                    }

#if DEBUGPRINT
                                    Console.WriteLine("[+] [client <- client] ForwarderShellSendData Write");
#endif
                                    stream.Write(buffer,
                                                 0,
                                                 length);
                                    sen = length;

                                    downloadCommandFlag = false;
                                    downloadFileFlag = false;
                                    downloadFileName = "";
                                    downloadFilePath = "";
                                    downloadFileSize = 0;
                                    recvDownloadFileDataSize = 0;
                                    downloadFileRemainingSize = 0;

                                    continue;
                                }

                                fileStream.Close();
                                fileStream = null;

                                Array.Clear(buffer,
                                            0,
                                            bufferMaxLength);

                                result = "[+] download file: ";
                                result += downloadFileName;
                                result += prompt;

                                tmp = Encoding.UTF8.GetBytes(result);
                                length = tmp.Length;
                                for(int i = 0; i < length; i++)
                                {
                                    buffer[i] = tmp[i];
                                }

#if DEBUGPRINT
                                Console.WriteLine("[+] [client <- client] ForwarderShellSendData Write");
#endif
                                stream.Write(buffer,
                                             0,
                                             length);
                                sen = length;

#if DEBUGPRINT
                                Console.WriteLine("[+] [client -> server] ForwarderShellSendData SendReceiveMessage({0}) ok",
                                                   recvMessageId);
#endif
                                ret = SendReceiveMessage(recvMessageId,
                                                         1,   // received
                                                         0,   // ok
                                                         0,
                                                         forwarderTvSec,
                                                         forwarderTvUsec);
                                if(ret < 0)
                                {
                                    errorFlag = true;
                                    break;
                                }

                                nextRecvMessageId++;


                                downloadCommandFlag = false;
                                downloadFileFlag = false;
                                downloadFileName = "";
                                downloadFilePath = "";
                                downloadFileSize = 0;
                                recvDownloadFileDataSize = 0;
                                downloadFileRemainingSize = 0;
                            }else
                            {
                                length = rec;
#if DEBUGPRINT
                                Console.WriteLine("[+] [client <- client] ForwarderShellSendData Write({0})",
                                                  recvMessageId);
#endif
                                stream.Write(buffer,
                                             0,
                                             length);
                                sen = length;

#if DEBUGPRINT
                                Console.WriteLine("[+] [client -> server] ForwarderShellSendData SendReceiveMessage({0}) ok",
                                                   recvMessageId);
#endif
                                ret = SendReceiveMessage(recvMessageId,
                                                         1,   // received
                                                         0,   // ok
                                                         0,
                                                         forwarderTvSec,
                                                         forwarderTvUsec);
                                if(ret < 0)
                                {
                                    errorFlag = true;
                                    break;
                                }

                                nextRecvMessageId++;
                            }
                        }else
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] [client -> server] ForwarderShellSendData SendReceiveMessage({0}) ng",
                                              recvMessageId);
#endif
                            ret = SendReceiveMessage(recvMessageId,
                                                     1,   // received
                                                     1,   // ng
                                                     0,
                                                     forwarderTvSec,
                                                     forwarderTvUsec);

                            errorFlag = true;
                            break;
                        }
                    }else
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client -> server] ForwarderShellSendData SendReceiveMessage({0}) ng",
                                          nextRecvMessageId);
#endif
                        ret = SendReceiveMessage(nextRecvMessageId,
                                                 1,   // received
                                                 1,   // ng
                                                 0,
                                                 forwarderTvSec,
                                                 forwarderTvUsec);

                        errorFlag = true;
                        break;
                    }
                }catch(IOException ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderShellSendData timeout: {0}",
                                      ex.Message);
#endif
                    break;
                }catch (Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderShellSendData error: {0}",
                                      ex.Message);
#endif
                    break;
                }
            }

            if(fileStream != null)
            {
                fileStream.Close();
                fileStream = null;
            }

            return;
        }

        private int ForwarderShell()
        {
            int timeout = forwarderTvSec * 1000 + ForwarderTvUsec / 1000;
            this.tcpClient.ReceiveTimeout = timeout;
            this.tcpClient.SendTimeout = timeout;

            Thread thread1 = new Thread(new ThreadStart(ForwarderShellRecvData));
            Thread thread2 = new Thread(new ThreadStart(ForwarderShellSendData));

            thread1.Start();
            thread2.Start();

            thread1.Join();
            thread2.Join();

            return 0;
        }

        public int DoSocks5ConnectionShell()
        {
            byte authenticationMethod = SOCKS5_AUTHENTICATION_METHOD;   // 0x0:No Authentication Required  0x2:Username/Password Authentication
            byte[] username = Encoding.UTF8.GetBytes(SOCKS5_USERNAME);
            byte[] password = Encoding.UTF8.GetBytes(SOCKS5_PASSWORD);

            int rec;
            int sen;
            int size;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            int bufferMaxLength = NODE_BUFFER_SIZE;

            recvMessageId = 0;
            nextRecvMessageId = 0;
            sendMessageId = GenerateRandomId();

            SelectionRequest selectionRequest;
            SelectionResponse selectionResponse;
            UsernamePasswordAuthenticationRequest usernamePasswordAuthenticationRequest;
            UsernamePasswordAuthenticationResponse usernamePasswordAuthenticationResponse;
            SocksRequestIpv4 socksRequestIpv4;
            SocksResponseIpv4 socksResponseIpv4;

            byte[] methods;
            byte[] dstAddr;
            byte[] dstPort;


            // socks SELECTION_REQUEST [client -> server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send selection request\n");
#endif
            Array.Clear(buffer,
                        0,
                        bufferMaxLength);

            try
            {
                methods = new byte[1];
                methods[0] = authenticationMethod;
                selectionRequest = new SelectionRequest(0x5,
                                                        0x1,
                                                        methods);
                size = selectionRequest.CopyToBuffer(ref buffer);
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] selectionRequest error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            sendMessageId++;
            sen = SendMessage(buffer,
                              size,
                              tvSec,
                              tvUsec);

#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send selection request: {0} bytes",
                              sen);
#endif


            // socks SELECTION_RESPONSE [client <- server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv selection response");
#endif

            rec = RecvMessage(buffer,
                              bufferMaxLength,
                              tvSec,
                              tvUsec,
                              true);
            if(rec != SelectionResponse.SELECTION_RESPONSE_SIZE)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client <- server] recv selection response error");
#endif
                return -1;
            }

            nextRecvMessageId = recvMessageId + 1;

#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv selection response: {0} bytes",
                              rec);
#endif

            try
            {
                selectionResponse = new SelectionResponse(buffer);
            }catch(Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] SelectionResponse error: {0}",
                                  ex.Message);
#endif
                return -1;
            }
            if(selectionResponse.Method == 0xFF)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socks5server authentication method error");
#endif
            }

            if(selectionResponse.Method  == 0x2)    // USERNAME_PASSWORD_AUTHENTICATION
            {
                // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST [client -> server]
#if DEBUGPRINT
                Console.WriteLine("[+] [client -> server] send username password authentication request");
#endif
                Array.Clear(buffer,
                            0,
                            bufferMaxLength);

                try
                {
                    usernamePasswordAuthenticationRequest = new UsernamePasswordAuthenticationRequest(0x1,
                                                            (byte)username.Length,
                                                            username,
                                                            (byte)password.Length,
                                                            password);
                    size = usernamePasswordAuthenticationRequest.CopyToBuffer(ref buffer);
                }catch (Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] usernamePasswordAuthenticationRequest error: {0}",
                                      ex.Message);
#endif
                    return -1;
                }

                sendMessageId++;
                sen = SendMessage(buffer,
                                  size,
                                  tvSec,
                                  tvUsec);

#if DEBUGPRINT
                Console.WriteLine("[+] [client -> server] send username password authentication request: {0} bytes",
                            sen);
#endif


                // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- server]
#if DEBUGPRINT
                Console.WriteLine("[+] [client <- server] recv username password authentication response");
#endif

                rec = RecvMessage(buffer,
                                  bufferMaxLength,
                                  tvSec,
                                  tvUsec,
                                  false);
                if(rec <= 0 ||
                   nextRecvMessageId != recvMessageId)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] [client <- server] recv username password authentication response error");
#endif
                    return -1;
                }

                nextRecvMessageId++;

#if DEBUGPRINT
                Console.WriteLine("[+] [client <- server] recv username password authentication response: {0} bytes",
                                  rec);
#endif

                try
                {
                    usernamePasswordAuthenticationResponse = new UsernamePasswordAuthenticationResponse(buffer);
                }catch(Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] usernamePasswordAuthenticationResponse error: {0}",
                                      ex.Message);
#endif
                    return -1;
                }

                if(usernamePasswordAuthenticationResponse.Status != 0x0)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] username password authentication error: 0x%02x",
                                      usernamePasswordAuthenticationResponse.Status);
#endif
                    return -1;
                }
            }


            // socks SOCKS_REQUEST [client -> server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send socks request");
#endif
            Array.Clear(buffer,
                        0,
                        bufferMaxLength);

            try
            {
                dstAddr = new byte[4];
                dstPort = new byte[2];
                socksRequestIpv4 = new SocksRequestIpv4(0x5,
                                                        0x9,    // SHELL (0x9, original command)
                                                        0x0,
                                                        0x0,    // none
                                                        dstAddr,
                                                        dstPort);
                size = socksRequestIpv4.CopyToBuffer(ref buffer);
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socksRequest error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            sendMessageId++;
            sen = SendMessage(buffer,
                              size,
                              tvSec,
                              tvUsec);

#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send socks request: {0} bytes",
                              sen);
#endif


            // socks SOCKS_RESPONSE [client <- server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv socks response");
#endif
            rec = RecvMessage(buffer,
                              bufferMaxLength,
                              tvSec,
                              tvUsec,
                              false);
            if(rec <= 0 ||
               nextRecvMessageId != recvMessageId)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client <- server] recv socks response error");
#endif
                return -1;
            }

            nextRecvMessageId++;

#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv socks response: {0} bytes",
                              rec);
#endif

            try
            {
                socksResponseIpv4 = new SocksResponseIpv4(buffer);
            }catch(Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] socksResponse error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            if(socksResponseIpv4.Ver != 0x5 &&
               socksResponseIpv4.Rep != 0x0 &&
               socksResponseIpv4.Atyp != 0x0)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socks response error: 0x%02x",
                                  socksResponseIpv4.Rep);
#endif
                return -1;
            }


            // forwarder [client <> client <> server <> target]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <> client <> server <> target] ForwarderShell");
#endif

            sendMessageId++;
            ForwarderShell();

#if DEBUGPRINT
            Console.WriteLine("[+] worker exit");
#endif

            return -1;
        }

        private int ForwarderAddNode(string config)
        {
            int rec = 0;
            int sen = 0;

            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            byte[] data;
            int bufferMaxLength = NODE_BUFFER_SIZE;
            recvMessageId = 0;

            int configSize = 0;
            string result = "";


            data = Encoding.UTF8.GetBytes(config);
            configSize = data.Length;

            for(int i = 0; i < configSize; i++)
            {
                buffer[i] = data[i];
            }

#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] ForwarderAddNode SendMessage ({0})",
                              sendMessageId);
#endif
            sen = SendMessage(buffer,
                              configSize,
                              forwarderTvSec,
                              forwarderTvUsec);
            if(sen > 0)
            {
                sendMessageId++;

                Array.Clear(buffer,
                            0,
                            bufferMaxLength);

                rec = RecvMessage(buffer,
                                  bufferMaxLength,
                                  forwarderTvSec,
                                  forwarderTvUsec,
                                  false);
                if(rec > 0)
                {
                    if(recvMessageId == nextRecvMessageId)
                    {
                        data = buffer.Where(b => b != 0x00).ToArray();
                        result = Encoding.UTF8.GetString(data);
#if DEBUGPRINT
                    Console.WriteLine("[+] [client <- server] ForwarderAddNode RecvMessage({0}): {1}",
                                      recvMessageId,
                                      result);
#endif
                    }else
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client <- server] ForwarderAddNode RecvMessage({0}) error",
                                          recvMessageId);
#endif
                    }
                }else
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] [client <- server] ForwarderAddNode RecvMessage error");
#endif
                }
            }

            return 0;
        }

        public int DoSocks5ConnectionAddNode(string config)
        {
            byte authenticationMethod = SOCKS5_AUTHENTICATION_METHOD;   // 0x0:No Authentication Required  0x2:Username/Password Authentication
            byte[] username = Encoding.UTF8.GetBytes(SOCKS5_USERNAME);
            byte[] password = Encoding.UTF8.GetBytes(SOCKS5_PASSWORD);

            int rec;
            int sen;
            int size;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            int bufferMaxLength = NODE_BUFFER_SIZE;

            recvMessageId = 0;
            nextRecvMessageId = 0;
            sendMessageId = GenerateRandomId();

            SelectionRequest selectionRequest;
            SelectionResponse selectionResponse;
            UsernamePasswordAuthenticationRequest usernamePasswordAuthenticationRequest;
            UsernamePasswordAuthenticationResponse usernamePasswordAuthenticationResponse;
            SocksRequestIpv4 socksRequestIpv4;
            SocksResponseIpv4 socksResponseIpv4;

            byte[] methods;
            byte[] dstAddr;
            byte[] dstPort;


            // socks SELECTION_REQUEST [client -> server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send selection request\n");
#endif
            Array.Clear(buffer,
                        0,
                        bufferMaxLength);

            try
            {
                methods = new byte[1];
                methods[0] = authenticationMethod;
                selectionRequest = new SelectionRequest(0x5,
                                                        0x1,
                                                        methods);
                size = selectionRequest.CopyToBuffer(ref buffer);
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] selectionRequest error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            sendMessageId++;
            sen = SendMessage(buffer,
                              size,
                              tvSec,
                              tvUsec);

#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send selection request: {0} bytes",
                              sen);
#endif


            // socks SELECTION_RESPONSE [client <- server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv selection response");
#endif

            rec = RecvMessage(buffer,
                              bufferMaxLength,
                              tvSec,
                              tvUsec,
                              true);
            if(rec != SelectionResponse.SELECTION_RESPONSE_SIZE)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client <- server] recv selection response error");
#endif
                return -1;
            }

            nextRecvMessageId = recvMessageId + 1;

#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv selection response: {0} bytes",
                              rec);
#endif

            try
            {
                selectionResponse = new SelectionResponse(buffer);
            }catch(Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] SelectionResponse error: {0}",
                                  ex.Message);
#endif
                return -1;
            }
            if(selectionResponse.Method == 0xFF)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socks5server authentication method error");
#endif
            }

            if(selectionResponse.Method  == 0x2)    // USERNAME_PASSWORD_AUTHENTICATION
            {
                // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST [client -> server]
#if DEBUGPRINT
                Console.WriteLine("[+] [client -> server] send username password authentication request");
#endif
                Array.Clear(buffer,
                            0,
                            bufferMaxLength);

                try
                {
                    usernamePasswordAuthenticationRequest = new UsernamePasswordAuthenticationRequest(0x1,
                                                            (byte)username.Length,
                                                            username,
                                                            (byte)password.Length,
                                                            password);
                    size = usernamePasswordAuthenticationRequest.CopyToBuffer(ref buffer);
                }catch (Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] usernamePasswordAuthenticationRequest error: {0}",
                                      ex.Message);
#endif
                    return -1;
                }

                sendMessageId++;
                sen = SendMessage(buffer,
                                  size,
                                  tvSec,
                                  tvUsec);

#if DEBUGPRINT
                Console.WriteLine("[+] [client -> server] send username password authentication request: {0} bytes",
                            sen);
#endif


                // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- server]
#if DEBUGPRINT
                Console.WriteLine("[+] [client <- server] recv username password authentication response");
#endif

                rec = RecvMessage(buffer,
                                  bufferMaxLength,
                                  tvSec,
                                  tvUsec,
                                  false);
                if(rec <= 0 ||
                   nextRecvMessageId != recvMessageId)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] [client <- server] recv username password authentication response error");
#endif
                    return -1;
                }

                nextRecvMessageId++;

#if DEBUGPRINT
                Console.WriteLine("[+] [client <- server] recv username password authentication response: {0} bytes",
                                  rec);
#endif

                try
                {
                    usernamePasswordAuthenticationResponse = new UsernamePasswordAuthenticationResponse(buffer);
                }catch(Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] usernamePasswordAuthenticationResponse error: {0}",
                                      ex.Message);
#endif
                    return -1;
                }

                if(usernamePasswordAuthenticationResponse.Status != 0x0)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] username password authentication error: 0x%02x",
                                      usernamePasswordAuthenticationResponse.Status);
#endif
                    return -1;
                }
            }


            // socks SOCKS_REQUEST [client -> server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send socks request");
#endif
            Array.Clear(buffer,
                        0,
                        bufferMaxLength);

            try
            {
                dstAddr = new byte[4];
                dstPort = new byte[2];
                socksRequestIpv4 = new SocksRequestIpv4(0x5,
                                                        0xa,    // ADD NODE (0xa, original command)
                                                        0x0,
                                                        0x0,    // none
                                                        dstAddr,
                                                        dstPort);
                size = socksRequestIpv4.CopyToBuffer(ref buffer);
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socksRequest error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            sendMessageId++;
            sen = SendMessage(buffer,
                              size,
                              tvSec,
                              tvUsec);

#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send socks request: {0} bytes",
                              sen);
#endif


            // socks SOCKS_RESPONSE [client <- server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv socks response");
#endif
            rec = RecvMessage(buffer,
                              bufferMaxLength,
                              tvSec,
                              tvUsec,
                              false);
            if(rec <= 0 ||
               nextRecvMessageId != recvMessageId)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client <- server] recv socks response error");
#endif
                return -1;
            }

            nextRecvMessageId++;

#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv socks response: {0} bytes",
                              rec);
#endif

            try
            {
                socksResponseIpv4 = new SocksResponseIpv4(buffer);
            }catch(Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] socksResponse error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            if(socksResponseIpv4.Ver != 0x5 &&
               socksResponseIpv4.Rep != 0x0 &&
               socksResponseIpv4.Atyp != 0x0)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socks response error: 0x%02x",
                                  socksResponseIpv4.Rep);
#endif
                return -1;
            }


            // forwarder [client <> client <> server <> target]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <> client <> server <> target] ForwarderAddNode");
#endif

            sendMessageId++;
            ForwarderAddNode(config);

#if DEBUGPRINT
            Console.WriteLine("[+] worker exit");
#endif

            return -1;
        }

        private int ForwarderShowNode()
        {
            int rec = 0;
            string result = "";
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            int bufferMaxLength = NODE_BUFFER_SIZE;
            recvMessageId = 0;


            rec = RecvMessage(buffer,
                              bufferMaxLength,
                              forwarderTvSec,
                              forwarderTvUsec,
                              false);
            if(rec > 0)
            {
                if(recvMessageId == nextRecvMessageId)
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] [client <- server] ForwarderShowNode RecvMessage({0})",
                                      recvMessageId);
#endif

                    buffer = buffer.Where(b => b != 0x00).ToArray();
                    result = Encoding.UTF8.GetString(buffer);
                    result = result.Trim();
                    Console.WriteLine("{0}",
                                      result);
                }else
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] [client <- server] ForwarderShowNode RecvMessage({0}) error",
                                      recvMessageId);
#endif
                }
            }else
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client <- server] ForwarderShowNode RecvMessage error");
#endif
            }

            return 0;
        }

        public int DoSocks5ConnectionShowNode()
        {
            byte authenticationMethod = SOCKS5_AUTHENTICATION_METHOD;   // 0x0:No Authentication Required  0x2:Username/Password Authentication
            byte[] username = Encoding.UTF8.GetBytes(SOCKS5_USERNAME);
            byte[] password = Encoding.UTF8.GetBytes(SOCKS5_PASSWORD);

            int rec;
            int sen;
            int size;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            int bufferMaxLength = NODE_BUFFER_SIZE;

            recvMessageId = 0;
            nextRecvMessageId = 0;
            sendMessageId = GenerateRandomId();

            SelectionRequest selectionRequest;
            SelectionResponse selectionResponse;
            UsernamePasswordAuthenticationRequest usernamePasswordAuthenticationRequest;
            UsernamePasswordAuthenticationResponse usernamePasswordAuthenticationResponse;
            SocksRequestIpv4 socksRequestIpv4;
            SocksResponseIpv4 socksResponseIpv4;

            byte[] methods;
            byte[] dstAddr;
            byte[] dstPort;


            // socks SELECTION_REQUEST [client -> server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send selection request\n");
#endif
            Array.Clear(buffer,
                        0,
                        bufferMaxLength);

            try
            {
                methods = new byte[1];
                methods[0] = authenticationMethod;
                selectionRequest = new SelectionRequest(0x5,
                                                        0x1,
                                                        methods);
                size = selectionRequest.CopyToBuffer(ref buffer);
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] selectionRequest error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            sendMessageId++;
            sen = SendMessage(buffer,
                              size,
                              tvSec,
                              tvUsec);

#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send selection request: {0} bytes",
                              sen);
#endif


            // socks SELECTION_RESPONSE [client <- server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv selection response");
#endif

            rec = RecvMessage(buffer,
                              bufferMaxLength,
                              tvSec,
                              tvUsec,
                              true);
            if(rec != SelectionResponse.SELECTION_RESPONSE_SIZE)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client <- server] recv selection response error");
#endif
                return -1;
            }

            nextRecvMessageId = recvMessageId + 1;

#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv selection response: {0} bytes",
                              rec);
#endif

            try
            {
                selectionResponse = new SelectionResponse(buffer);
            }catch(Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] SelectionResponse error: {0}",
                                  ex.Message);
#endif
                return -1;
            }
            if(selectionResponse.Method == 0xFF)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socks5server authentication method error");
#endif
            }

            if(selectionResponse.Method  == 0x2)    // USERNAME_PASSWORD_AUTHENTICATION
            {
                // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST [client -> server]
#if DEBUGPRINT
                Console.WriteLine("[+] [client -> server] send username password authentication request");
#endif
                Array.Clear(buffer,
                            0,
                            bufferMaxLength);

                try
                {
                    usernamePasswordAuthenticationRequest = new UsernamePasswordAuthenticationRequest(0x1,
                                                            (byte)username.Length,
                                                            username,
                                                            (byte)password.Length,
                                                            password);
                    size = usernamePasswordAuthenticationRequest.CopyToBuffer(ref buffer);
                }catch (Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] usernamePasswordAuthenticationRequest error: {0}",
                                      ex.Message);
#endif
                    return -1;
                }

                sendMessageId++;
                sen = SendMessage(buffer,
                                  size,
                                  tvSec,
                                  tvUsec);

#if DEBUGPRINT
                Console.WriteLine("[+] [client -> server] send username password authentication request: {0} bytes",
                            sen);
#endif


                // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- server]
#if DEBUGPRINT
                Console.WriteLine("[+] [client <- server] recv username password authentication response");
#endif

                rec = RecvMessage(buffer,
                                  bufferMaxLength,
                                  tvSec,
                                  tvUsec,
                                  false);
                if(rec <= 0 ||
                   nextRecvMessageId != recvMessageId)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] [client <- server] recv username password authentication response error");
#endif
                    return -1;
                }

                nextRecvMessageId++;

#if DEBUGPRINT
                Console.WriteLine("[+] [client <- server] recv username password authentication response: {0} bytes",
                                  rec);
#endif

                try
                {
                    usernamePasswordAuthenticationResponse = new UsernamePasswordAuthenticationResponse(buffer);
                }catch(Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] usernamePasswordAuthenticationResponse error: {0}",
                                      ex.Message);
#endif
                    return -1;
                }

                if(usernamePasswordAuthenticationResponse.Status != 0x0)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] username password authentication error: 0x%02x",
                                      usernamePasswordAuthenticationResponse.Status);
#endif
                    return -1;
                }
            }


            // socks SOCKS_REQUEST [client -> server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send socks request");
#endif
            Array.Clear(buffer,
                        0,
                        bufferMaxLength);

            try
            {
                dstAddr = new byte[4];
                dstPort = new byte[2];
                socksRequestIpv4 = new SocksRequestIpv4(0x5,
                                                        0xb,    // SHOW NODE (0xb, original command)
                                                        0x0,
                                                        0x0,    // none
                                                        dstAddr,
                                                        dstPort);
                size = socksRequestIpv4.CopyToBuffer(ref buffer);
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socksRequest error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            sendMessageId++;
            sen = SendMessage(buffer,
                              size,
                              tvSec,
                              tvUsec);

#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send socks request: {0} bytes",
                              sen);
#endif


            // socks SOCKS_RESPONSE [client <- server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv socks response");
#endif
            rec = RecvMessage(buffer,
                              bufferMaxLength,
                              tvSec,
                              tvUsec,
                              false);
            if(rec <= 0 ||
               nextRecvMessageId != recvMessageId)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client <- server] recv socks response error");
#endif
                return -1;
            }

            nextRecvMessageId++;

#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv socks response: {0} bytes",
                              rec);
#endif

            try
            {
                socksResponseIpv4 = new SocksResponseIpv4(buffer);
            }catch(Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] socksResponse error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            if(socksResponseIpv4.Ver != 0x5 &&
               socksResponseIpv4.Rep != 0x0 &&
               socksResponseIpv4.Atyp != 0x0)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socks response error: 0x%02x",
                                  socksResponseIpv4.Rep);
#endif
                return -1;
            }


            // forwarder [client <> client <> server <> target]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <> client <> server <> target] ForwarderShowNode");
#endif

            sendMessageId++;
            ForwarderShowNode();

#if DEBUGPRINT
            Console.WriteLine("[+] worker exit");
#endif

            return -1;
        }

        private int ForwarderShowRoute()
        {
            int rec = 0;
            string result = "";
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            int bufferMaxLength = NODE_BUFFER_SIZE;
            recvMessageId = 0;


            rec = RecvMessage(buffer,
                              bufferMaxLength,
                              forwarderTvSec,
                              forwarderTvUsec,
                              false);

            if(rec > 0)
            {
                if(recvMessageId == nextRecvMessageId)
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] [client <- server] ForwarderShowRoute RecvMessage({0})",
                                      recvMessageId);
#endif

                    buffer = buffer.Where(b => b != 0x00).ToArray();
                    result = Encoding.UTF8.GetString(buffer);
                    result = result.Trim();
                    Console.WriteLine("{0}",
                                      result);
                }else
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] [client <- server] ForwarderShowRoute RecvMessage({0}) error",
                                      recvMessageId);
#endif
                }
            }else
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client <- server] ForwarderShowRoute RecvMessage error");
#endif
            }

            return 0;
        }

        public int DoSocks5ConnectionShowRoute()
        {
            byte authenticationMethod = SOCKS5_AUTHENTICATION_METHOD;   // 0x0:No Authentication Required  0x2:Username/Password Authentication
            byte[] username = Encoding.UTF8.GetBytes(SOCKS5_USERNAME);
            byte[] password = Encoding.UTF8.GetBytes(SOCKS5_PASSWORD);

            int rec;
            int sen;
            int size;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            int bufferMaxLength = NODE_BUFFER_SIZE;

            recvMessageId = 0;
            nextRecvMessageId = 0;
            sendMessageId = GenerateRandomId();

            SelectionRequest selectionRequest;
            SelectionResponse selectionResponse;
            UsernamePasswordAuthenticationRequest usernamePasswordAuthenticationRequest;
            UsernamePasswordAuthenticationResponse usernamePasswordAuthenticationResponse;
            SocksRequestIpv4 socksRequestIpv4;
            SocksResponseIpv4 socksResponseIpv4;

            byte[] methods;
            byte[] dstAddr;
            byte[] dstPort;


            // socks SELECTION_REQUEST [client -> server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send selection request\n");
#endif
            Array.Clear(buffer,
                        0,
                        bufferMaxLength);

            try
            {
                methods = new byte[1];
                methods[0] = authenticationMethod;
                selectionRequest = new SelectionRequest(0x5,
                                                        0x1,
                                                        methods);
                size = selectionRequest.CopyToBuffer(ref buffer);
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] selectionRequest error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            sendMessageId++;
            sen = SendMessage(buffer,
                              size,
                              tvSec,
                              tvUsec);

#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send selection request: {0} bytes",
                              sen);
#endif


            // socks SELECTION_RESPONSE [client <- server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv selection response");
#endif

            rec = RecvMessage(buffer,
                              bufferMaxLength,
                              tvSec,
                              tvUsec,
                              true);
            if(rec != SelectionResponse.SELECTION_RESPONSE_SIZE)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client <- server] recv selection response error");
#endif
                return -1;
            }

            nextRecvMessageId = recvMessageId + 1;

#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv selection response: {0} bytes",
                              rec);
#endif

            try
            {
                selectionResponse = new SelectionResponse(buffer);
            }catch(Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] SelectionResponse error: {0}",
                                  ex.Message);
#endif
                return -1;
            }
            if(selectionResponse.Method == 0xFF)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socks5server authentication method error");
#endif
            }

            if(selectionResponse.Method  == 0x2)    // USERNAME_PASSWORD_AUTHENTICATION
            {
                // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST [client -> server]
#if DEBUGPRINT
                Console.WriteLine("[+] [client -> server] send username password authentication request");
#endif
                Array.Clear(buffer,
                            0,
                            bufferMaxLength);

                try
                {
                    usernamePasswordAuthenticationRequest = new UsernamePasswordAuthenticationRequest(0x1,
                                                            (byte)username.Length,
                                                            username,
                                                            (byte)password.Length,
                                                            password);
                    size = usernamePasswordAuthenticationRequest.CopyToBuffer(ref buffer);
                }catch (Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] usernamePasswordAuthenticationRequest error: {0}",
                                      ex.Message);
#endif
                    return -1;
                }

                sendMessageId++;
                sen = SendMessage(buffer,
                                  size,
                                  tvSec,
                                  tvUsec);

#if DEBUGPRINT
                Console.WriteLine("[+] [client -> server] send username password authentication request: {0} bytes",
                            sen);
#endif


                // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- server]
#if DEBUGPRINT
                Console.WriteLine("[+] [client <- server] recv username password authentication response");
#endif

                rec = RecvMessage(buffer,
                                  bufferMaxLength,
                                  tvSec,
                                  tvUsec,
                                  false);
                if(rec <= 0 ||
                   nextRecvMessageId != recvMessageId)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] [client <- server] recv username password authentication response error");
#endif
                    return -1;
                }

                nextRecvMessageId++;

#if DEBUGPRINT
                Console.WriteLine("[+] [client <- server] recv username password authentication response: {0} bytes",
                                  rec);
#endif

                try
                {
                    usernamePasswordAuthenticationResponse = new UsernamePasswordAuthenticationResponse(buffer);
                }catch(Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] usernamePasswordAuthenticationResponse error: {0}",
                                      ex.Message);
#endif
                    return -1;
                }

                if(usernamePasswordAuthenticationResponse.Status != 0x0)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] username password authentication error: 0x%02x",
                                      usernamePasswordAuthenticationResponse.Status);
#endif
                    return -1;
                }
            }


            // socks SOCKS_REQUEST [client -> server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send socks request");
#endif
            Array.Clear(buffer,
                        0,
                        bufferMaxLength);

            try
            {
                dstAddr = new byte[4];
                dstPort = new byte[2];
                socksRequestIpv4 = new SocksRequestIpv4(0x5,
                                                        0xc,    // SHOW ROUTE (0xc, original command)
                                                        0x0,
                                                        0x0,    // none
                                                        dstAddr,
                                                        dstPort);
                size = socksRequestIpv4.CopyToBuffer(ref buffer);
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socksRequest error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            sendMessageId++;
            sen = SendMessage(buffer,
                              size,
                              tvSec,
                              tvUsec);

#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send socks request: {0} bytes",
                              sen);
#endif


            // socks SOCKS_RESPONSE [client <- server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv socks response");
#endif
            rec = RecvMessage(buffer,
                              bufferMaxLength,
                              tvSec,
                              tvUsec,
                              false);
            if(rec <= 0 ||
               nextRecvMessageId != recvMessageId)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client <- server] recv socks response error");
#endif
                return -1;
            }

            nextRecvMessageId++;

#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv socks response: {0} bytes",
                              rec);
#endif

            try
            {
                socksResponseIpv4 = new SocksResponseIpv4(buffer);
            }catch(Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] socksResponse error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            if(socksResponseIpv4.Ver != 0x5 &&
               socksResponseIpv4.Rep != 0x0 &&
               socksResponseIpv4.Atyp != 0x0)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socks response error: 0x%02x",
                                  socksResponseIpv4.Rep);
#endif
                return -1;
            }


            // forwarder [client <> client <> server <> target]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <> client <> server <> target] ForwarderShowRoute");
#endif

            sendMessageId++;
            ForwarderShowRoute();

#if DEBUGPRINT
            Console.WriteLine("[+] worker exit");
#endif

            return -1;
        }

        private void ForwarderUdpRecvSendData()
        {
            int ret = 0;
            int rec = 0;
            int sen = 0;
            int length = 0;

            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            byte[] data;
            int bufferMaxLength = NODE_BUFFER_SIZE;
            int socks5MessageDataMaxSize = SPIDER_MESSAGE_DATA_SIZE;
            recvMessageId = 0;


            while(true)
            {
                try
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] [client -> client] ForwarderUdpRecvSendData Receive");
#endif
                    Array.Clear(buffer,
                                0,
                                bufferMaxLength);

                    IPEndPoint ipEndPoint = new IPEndPoint(IPAddress.Any,
                                                           0);

                    data = udpClient.Receive(ref ipEndPoint);
                    rec = data.Length;

                    if(rec > socks5MessageDataMaxSize)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] ForwarderUdpRecvSendData Receive error: {0}",
                                          rec);
#endif
                        break;
                    }

                    if(rec > 0)
                    {
                        for(int i = 0; i < data.Length; i++)
                        {
                            buffer[i] = data[i];
                        }

#if DEBUGPRINT
                        Console.WriteLine("[+] [client -> server] ForwarderUdpRecvSendData SendMessage({0})",
                                          sendMessageId);
#endif
                        sen = SendMessage(buffer,
                                          rec,
                                          forwarderTvSec,
                                          forwarderTvUsec);
                        if(sen <= 0)
                        {
                            break;
                        }

#if DEBUGPRINT
                        Console.WriteLine("[+] [client <- server] ForwarderUdpRecvSendData RecvReceiveMessage({0})",
                                          sendMessageId);
#endif
                        ret = RecvReceiveMessage(sendMessageId,
                                                 forwarderTvSec,
                                                 forwarderTvUsec);
                        if(ret == 0)    // ok
                        {
#if DEBUGPRINT
                            Console.WriteLine("[+] [client <- server] ForwarderUdpRecvSendData RecvReceiveMessage({0}) ok",
                                              sendMessageId);
#endif
                        }else if(ret == 1)    // ng
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] [client <- server] ForwarderUdpRecvSendData RecvReceiveMessage({0}) ng",
                                              sendMessageId);
#endif
                            break;
                        }else
                        {
                            break;
                        }

                        sendMessageId++;
                    }else
                    {
                        break;
                    }


#if DEBUGPRINT
                    Console.WriteLine("[+] [client <- server] ForwarderUdpRecvSendData RecvMessage");
#endif
                    Array.Clear(buffer,
                                0,
                                bufferMaxLength);

                    rec = RecvMessage(buffer,
                                      bufferMaxLength,
                                      forwarderTvSec,
                                      forwarderTvUsec,
                                      false);
                    if(rec > 0)
                    {
                        if(recvMessageId == nextRecvMessageId)
                        {
#if DEBUGPRINT
                            Console.WriteLine("[+] [client -> server] ForwarderUdpRecvSendData SendReceiveMessage({0}) ok",
                                              recvMessageId);
#endif
                            ret = SendReceiveMessage(recvMessageId,
                                                     1,   // received
                                                     0,   // ok
                                                     0,
                                                     forwarderTvSec,
                                                     forwarderTvUsec);
                            if(ret < 0)
                            {
                                break;
                            }

                            length = rec;

#if DEBUGPRINT
                            Console.WriteLine("[+] [client <- client] ForwarderUdpRecvSendData Send({0})",
                                              recvMessageId);
#endif
                            udpClient.Send(buffer,
                                           length,
                                           ipEndPoint);

                            sen = length;
                            nextRecvMessageId++;
                        }else
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] [client -> server] ForwarderUdpRecvSendData SendReceiveMessage({0}) ng",
                                              recvMessageId);
#endif
                            ret = SendReceiveMessage(recvMessageId,
                                                     1,   // received
                                                     1,   // ng
                                                     0,
                                                     forwarderTvSec,
                                                     forwarderTvUsec);
                            break;
                        }
                    }else
                    {
                        break;
                    }
                }catch(IOException ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderUdpRecvSendData error: {0}",
                                      ex.Message);
#endif
                    break;
                }catch (Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderUdpRecvSendData error: {0}",
                                      ex.Message);
#endif
                    break;
                }
            }

            return;
        }

        private int ForwarderUdp()
        {
            int timeout = forwarderTvSec * 1000 + forwarderTvUsec / 1000;
            this.udpClient.Client.ReceiveTimeout = FORWARDER_UDP_TIMEOUT * 1000;
            this.udpClient.Client.SendTimeout = timeout;

            Thread thread = new Thread(new ThreadStart(ForwarderUdpRecvSendData));
            thread.Start();

            thread.Join();

            return 0;
        }

        public int DoSocks5ConnectionTcp()
        {
            byte authenticationMethod = SOCKS5_AUTHENTICATION_METHOD;   // 0x0:No Authentication Required  0x2:Username/Password Authentication
            byte[] username = Encoding.UTF8.GetBytes(SOCKS5_USERNAME);
            byte[] password = Encoding.UTF8.GetBytes(SOCKS5_PASSWORD);

            int rec;
            int sen;
            int size;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            int bufferMaxLength = NODE_BUFFER_SIZE;

            recvMessageId = 0;
            nextRecvMessageId = 0;
            sendMessageId = GenerateRandomId();

            SelectionRequest selectionRequest;
            SelectionResponse selectionResponse;
            UsernamePasswordAuthenticationRequest usernamePasswordAuthenticationRequest;
            UsernamePasswordAuthenticationResponse usernamePasswordAuthenticationResponse;
            SocksRequestDomainname socksRequestDomainname;
            SocksResponseTmp socksResponseTmp;

            byte[] methods;


            // socks SELECTION_REQUEST [client -> server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send selection request\n");
#endif
            Array.Clear(buffer,
                        0,
                        bufferMaxLength);

            try
            {
                methods = new byte[1];
                methods[0] = authenticationMethod;
                selectionRequest = new SelectionRequest(0x5,
                                                        0x1,
                                                        methods);
                size = selectionRequest.CopyToBuffer(ref buffer);
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] selectionRequest error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            sendMessageId++;
            sen = SendMessage(buffer,
                              size,
                              tvSec,
                              tvUsec);

#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send selection request: {0} bytes",
                              sen);
#endif


            // socks SELECTION_RESPONSE [client <- server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv selection response");
#endif

            rec = RecvMessage(buffer,
                              bufferMaxLength,
                              tvSec,
                              tvUsec,
                              true);
            if(rec != SelectionResponse.SELECTION_RESPONSE_SIZE)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client <- server] recv selection response error");
#endif
                return -1;
            }

            nextRecvMessageId = recvMessageId + 1;

#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv selection response: {0} bytes",
                              rec);
#endif

            try
            {
                selectionResponse = new SelectionResponse(buffer);
            }catch(Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] SelectionResponse error: {0}",
                                  ex.Message);
#endif
                return -1;
            }
            if(selectionResponse.Method == 0xFF)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socks5server authentication method error");
#endif
            }

            if(selectionResponse.Method  == 0x2)    // USERNAME_PASSWORD_AUTHENTICATION
            {
                // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST [client -> server]
#if DEBUGPRINT
                Console.WriteLine("[+] [client -> server] send username password authentication request");
#endif
                Array.Clear(buffer,
                            0,
                            bufferMaxLength);

                try
                {
                    usernamePasswordAuthenticationRequest = new UsernamePasswordAuthenticationRequest(0x1,
                                                            (byte)username.Length,
                                                            username,
                                                            (byte)password.Length,
                                                            password);
                    size = usernamePasswordAuthenticationRequest.CopyToBuffer(ref buffer);
                }catch (Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] usernamePasswordAuthenticationRequest error: {0}",
                                      ex.Message);
#endif
                    return -1;
                }

                sendMessageId++;
                sen = SendMessage(buffer,
                                  size,
                                  tvSec,
                                  tvUsec);

#if DEBUGPRINT
                Console.WriteLine("[+] [client -> server] send username password authentication request: {0} bytes",
                            sen);
#endif


                // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- server]
#if DEBUGPRINT
                Console.WriteLine("[+] [client <- server] recv username password authentication response");
#endif

                rec = RecvMessage(buffer,
                                  bufferMaxLength,
                                  tvSec,
                                  tvUsec,
                                  false);
                if(rec <= 0 ||
                   nextRecvMessageId != recvMessageId)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] [client <- server] recv username password authentication response error");
#endif
                    return -1;
                }

                nextRecvMessageId++;

#if DEBUGPRINT
                Console.WriteLine("[+] [client <- server] recv username password authentication response: {0} bytes",
                                  rec);
#endif

                try
                {
                    usernamePasswordAuthenticationResponse = new UsernamePasswordAuthenticationResponse(buffer);
                }catch(Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] usernamePasswordAuthenticationResponse error: {0}",
                                      ex.Message);
#endif
                    return -1;
                }

                if(usernamePasswordAuthenticationResponse.Status != 0x0)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] username password authentication error: 0x%02x",
                                      usernamePasswordAuthenticationResponse.Status);
#endif
                    return -1;
                }
            }


            // socks SOCKS_REQUEST [client -> server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send socks request");
#endif
            Array.Clear(buffer,
                        0,
                        bufferMaxLength);

            try
            {
                socksRequestDomainname = new SocksRequestDomainname(0x5,
                                                                    0x1,    // CONNECT (0x1)
                                                                    0x0,
                                                                    0x3,    // domainname
                                                                    targetIp,
                                                                    targetPort);
                size = socksRequestDomainname.CopyToBuffer(ref buffer);
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socksRequest error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            sendMessageId++;
            sen = SendMessage(buffer,
                              size,
                              tvSec,
                              tvUsec);

#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send socks request: {0} bytes",
                              sen);
#endif


            // socks SOCKS_RESPONSE [client <- server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv socks response");
#endif
            rec = RecvMessage(buffer,
                              bufferMaxLength,
                              tvSec,
                              tvUsec,
                              false);
            if(rec <= 0 ||
               nextRecvMessageId != recvMessageId)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client <- server] recv socks response error");
#endif
                return -1;
            }

            nextRecvMessageId++;

#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv socks response: {0} bytes",
                              rec);
#endif

            try
            {
                socksResponseTmp = new SocksResponseTmp(buffer);
            }catch(Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] socksResponse error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            if(socksResponseTmp.Ver != 0x5 &&
               socksResponseTmp.Rep != 0x0)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socks response error: 0x%02x",
                                  socksResponseTmp.Rep);
#endif
                return -1;
            }


            // forwarder [client <> client <> server <> target]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <> client <> server <> target] Forwarder");
#endif

            sendMessageId++;
            Forwarder();

#if DEBUGPRINT
            Console.WriteLine("[+] worker exit");
#endif

            return -1;
        }

        public int DoSocks5ConnectionUdp()
        {
            byte authenticationMethod = SOCKS5_AUTHENTICATION_METHOD;   // 0x0:No Authentication Required  0x2:Username/Password Authentication
            byte[] username = Encoding.UTF8.GetBytes(SOCKS5_USERNAME);
            byte[] password = Encoding.UTF8.GetBytes(SOCKS5_PASSWORD);

            int rec;
            int sen;
            int size;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            int bufferMaxLength = NODE_BUFFER_SIZE;

            recvMessageId = 0;
            nextRecvMessageId = 0;
            sendMessageId = GenerateRandomId();

            SelectionRequest selectionRequest;
            SelectionResponse selectionResponse;
            UsernamePasswordAuthenticationRequest usernamePasswordAuthenticationRequest;
            UsernamePasswordAuthenticationResponse usernamePasswordAuthenticationResponse;
            SocksRequestDomainname socksRequestDomainname;
            SocksResponseTmp socksResponseTmp;

            byte[] methods;
            IPEndPoint ipEndPoint;
            string clientIpTmp = "";
            string ipv6LinkLocalPrefix = "fe80:";


            // socks SELECTION_REQUEST [client -> server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send selection request\n");
#endif
            Array.Clear(buffer,
                        0,
                        bufferMaxLength);

            try
            {
                methods = new byte[1];
                methods[0] = authenticationMethod;
                selectionRequest = new SelectionRequest(0x5,
                                                        0x1,
                                                        methods);
                size = selectionRequest.CopyToBuffer(ref buffer);
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] selectionRequest error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            sendMessageId++;
            sen = SendMessage(buffer,
                              size,
                              tvSec,
                              tvUsec);

#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send selection request: {0} bytes",
                              sen);
#endif


            // socks SELECTION_RESPONSE [client <- server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv selection response");
#endif

            rec = RecvMessage(buffer,
                              bufferMaxLength,
                              tvSec,
                              tvUsec,
                              true);
            if(rec != SelectionResponse.SELECTION_RESPONSE_SIZE)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client <- server] recv selection response error");
#endif
                return -1;
            }

            nextRecvMessageId = recvMessageId + 1;

#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv selection response: {0} bytes",
                              rec);
#endif

            try
            {
                selectionResponse = new SelectionResponse(buffer);
            }catch(Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] SelectionResponse error: {0}",
                                  ex.Message);
#endif
                return -1;
            }
            if(selectionResponse.Method == 0xFF)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socks5server authentication method error");
#endif
            }

            if(selectionResponse.Method  == 0x2)    // USERNAME_PASSWORD_AUTHENTICATION
            {
                // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST [client -> server]
#if DEBUGPRINT
                Console.WriteLine("[+] [client -> server] send username password authentication request");
#endif
                Array.Clear(buffer,
                            0,
                            bufferMaxLength);

                try
                {
                    usernamePasswordAuthenticationRequest = new UsernamePasswordAuthenticationRequest(0x1,
                                                            (byte)username.Length,
                                                            username,
                                                            (byte)password.Length,
                                                            password);
                    size = usernamePasswordAuthenticationRequest.CopyToBuffer(ref buffer);
                }catch (Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] usernamePasswordAuthenticationRequest error: {0}",
                                      ex.Message);
#endif
                    return -1;
                }

                sendMessageId++;
                sen = SendMessage(buffer,
                                  size,
                                  tvSec,
                                  tvUsec);

#if DEBUGPRINT
                Console.WriteLine("[+] [client -> server] send username password authentication request: {0} bytes",
                            sen);
#endif


                // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- server]
#if DEBUGPRINT
                Console.WriteLine("[+] [client <- server] recv username password authentication response");
#endif

                rec = RecvMessage(buffer,
                                  bufferMaxLength,
                                  tvSec,
                                  tvUsec,
                                  false);
                if(rec <= 0 ||
                   nextRecvMessageId != recvMessageId)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] [client <- server] recv username password authentication response error");
#endif
                    return -1;
                }

                nextRecvMessageId++;

#if DEBUGPRINT
                Console.WriteLine("[+] [client <- server] recv username password authentication response: {0} bytes",
                                  rec);
#endif

                try
                {
                    usernamePasswordAuthenticationResponse = new UsernamePasswordAuthenticationResponse(buffer);
                }catch(Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] usernamePasswordAuthenticationResponse error: {0}",
                                      ex.Message);
#endif
                    return -1;
                }

                if(usernamePasswordAuthenticationResponse.Status != 0x0)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] username password authentication error: 0x%02x",
                                      usernamePasswordAuthenticationResponse.Status);
#endif
                    return -1;
                }
            }


            // socks SOCKS_REQUEST [client -> server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send socks request");
#endif
            Array.Clear(buffer,
                        0,
                        bufferMaxLength);

            try
            {
                socksRequestDomainname = new SocksRequestDomainname(0x5,
                                                                    0x8,    // CONNECT UDP (0x8, UDP over TCP, original command)
                                                                    0x0,
                                                                    0x3,    // domainname
                                                                    targetIp,
                                                                    targetPort);
                size = socksRequestDomainname.CopyToBuffer(ref buffer);
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socksRequest error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            sendMessageId++;
            sen = SendMessage(buffer,
                              size,
                              tvSec,
                              tvUsec);

#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] send socks request: {0} bytes",
                              sen);
#endif


            // socks SOCKS_RESPONSE [client <- server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv socks response");
#endif
            rec = RecvMessage(buffer,
                              bufferMaxLength,
                              tvSec,
                              tvUsec,
                              false);
            if(rec <= 0 ||
               nextRecvMessageId != recvMessageId)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client <- server] recv socks response error");
#endif
                return -1;
            }

            nextRecvMessageId++;

#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] recv socks response: {0} bytes",
                              rec);
#endif

            try
            {
                socksResponseTmp = new SocksResponseTmp(buffer);
            }catch(Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] socksResponse error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            if(socksResponseTmp.Ver != 0x5 &&
               socksResponseTmp.Rep != 0x0)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socks response error: 0x%02x",
                                  socksResponseTmp.Rep);
#endif
                return -1;
            }


            try
            {
                if(clientIp.Contains(":") &&    // ipv6 link local
                   clientIp.StartsWith(ipv6LinkLocalPrefix, StringComparison.OrdinalIgnoreCase))
                {
                    clientIpTmp = clientIp + "%" + clientIpScopeId;
                    ipEndPoint = new IPEndPoint(IPAddress.Parse(clientIpTmp),
                                                int.Parse(clientListenPort));
                    udpClient = new UdpClient(ipEndPoint);
#if DEBUGPRINT
                    Console.WriteLine("[+] bind port {0} on {1}",
                                      clientListenPort,
                                      clientIpTmp);
#endif
                }else
                {
                    ipEndPoint = new IPEndPoint(IPAddress.Parse(clientIp),
                                                int.Parse(clientListenPort));
                    udpClient = new UdpClient(ipEndPoint);
#if DEBUGPRINT
                    Console.WriteLine("[+] bind port {0} on {1}",
                                      clientListenPort,
                                      clientIp);
#endif
                }
            }catch(Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] udpClient error: {0}",
                                  ex.Message);
#endif
                return -1;
            }


            // forwarder [client <> client <> server <> target]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <> client <> server <> target] ForwarderUdp");
#endif

            sendMessageId++;
            ForwarderUdp();

#if DEBUGPRINT
            Console.WriteLine("[+] worker exit");
#endif

            return -1;
        }
    }
}

