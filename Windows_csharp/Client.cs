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
        private const int SOCKS5_MESSAGE_DATA_SIZE = 60000;
        private const int SHELL_UPLOAD_DOWNLOAD_DATA_SIZE = 50000;
        private const int SHOW_NODE_INFORMATION_WORKER_TV_SEC = 10;
        private const int SHOW_NODE_INFORMATION_WORKER_TV_USEC = 0;
        private const int SHOW_NODE_INFORMATION_WORKER_FORWARDER_TV_SEC = 10;
        private const int SHOW_NODE_INFORMATION_WORKER_FORWARDER_TV_USEC = 0;
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

            if(dataSize > SOCKS5_MESSAGE_DATA_SIZE)
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
                                         SOCKS5_MESSAGE_DATA_SIZE);
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
                                              (ushort)dataSize,
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

        private void ForwarderRecvData()
        {
            int rec = 0;
            int sen = 0;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            int bufferMaxLength = NODE_BUFFER_SIZE;
            int socks5MessageDataMaxSize = SOCKS5_MESSAGE_DATA_SIZE;


            while(true)
            {
                try
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] [client -> client] Read");
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
                        Console.WriteLine("[+] [client -> server] SendMessage message_id:{0}",
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

                        sendMessageId++;
                    }else{
#if DEBUGPRINT
                        Console.WriteLine("[-] [client -> client] Read error rec: {0}",
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
            int rec = 0;
            int sen = 0;
            int length = 0;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            int bufferMaxLength = NODE_BUFFER_SIZE;
            Dictionary<uint, (int, byte[])> msgsMap = new Dictionary<uint, (int, byte[])>();
            ValueTuple<int, byte[]> msg;
            recvMessageId = 0;


            while(true)
            {
                try
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] [client <- server] RecvMessage");
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
                            length = rec;
#if DEBUGPRINT
                            Console.WriteLine("[+] [client <- client] Write message_id:{0}",
                                             nextRecvMessageId);
#endif
                            stream.Write(buffer,
                                         0,
                                         length);

                            nextRecvMessageId++;
                        }else
                        {
                            msg = (rec,
                                   buffer);
                            msgsMap.Add(recvMessageId,
                                        msg);

                            while(msgsMap.ContainsKey(nextRecvMessageId))
                            {
                                msg = msgsMap[nextRecvMessageId];
                                msgsMap.Remove(nextRecvMessageId);

                                length = msg.Item1;
                                buffer = msg.Item2;

#if DEBUGPRINT
                                Console.WriteLine("[+] [client <- client] Write message_id:{0}",
                                                 nextRecvMessageId);
#endif
                                stream.Write(buffer,
                                             0,
                                             length);
                                sen = length;
                                nextRecvMessageId++;
                            }
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
            int socks5MessageDataMaxSize = SOCKS5_MESSAGE_DATA_SIZE;
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

        private string[] SplitInput(string[] input)
        {
            return null;
        }

        private int ForwarderShellRecvData()
        {
            return 0;
        }

        private int ForwarderShellSendData()
        {
            return 0;
        }

        private int ForwarderShell()
        {
            return 0;
        }

        public int DoSocks5ConnectionShell()
        {
            return 0;
        }

        private int ForwarderAddNode(string config)
        {
            return 0;
        }

        public int DoSocks5ConnectionAddNode(string config)
        {
            return 0;
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
                    Console.WriteLine("[+] [client <- server] RecvMessage message_id:{0}",
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
                    Console.WriteLine("[-] [client <- server] RecvMessage error message_id:{0}",
                                      recvMessageId);
#endif
                    Console.WriteLine("[-] RecvMessage error");
                }
            }else
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client <- server] RecvMessage error");
#endif
                Console.WriteLine("[-] RecvMessage error");
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

        private int ForwarderUdpRecvSendData(int addrLength)
        {
            return 0;
        }

        private int ForwarderUdp(int addrLength)
        {
            return 0;
        }

        public int DoSocks5ConnectionUdp()
        {
            return 0;
        }
    }
}

