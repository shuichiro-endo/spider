/*
 * Title:  spider SpiderCommand.cs (Windows csharp)
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
    public class SpiderCommand
    {
        [DllImport("iphlpapi.dll", SetLastError = true)]
        public static extern uint if_nametoindex(string ifname);

        private const int METRIC_MAX = 20;   // 0 < METRIC_MAX <= UINT8_MAX(255), UINT8_MAX(255) < delete route
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


        private SpiderIp spiderIp;
        private Encryption encryption;
        private ClientManager clientManager;
        private ServerManager serverManager;
        private PipeManager pipeManager;
        private RoutingManager routingManager;
        private MessageManager messageManager;

        public SpiderCommand(SpiderIp spiderIp,
                             Encryption encryption,
                             ClientManager clientManager,
                             ServerManager serverManager,
                             PipeManager pipeManager,
                             RoutingManager routingManager,
                             MessageManager messageManager)
        {
            this.spiderIp = spiderIp;
            this.encryption = encryption;
            this.clientManager = clientManager;
            this.serverManager = serverManager;
            this.pipeManager = pipeManager;
            this.routingManager = routingManager;
            this.messageManager = messageManager;
        }

        ~SpiderCommand()
        {

        }

        private uint GenerateRandomId()
        {
            Random random = new Random();
            ulong randomValue = 0;

            do
            {
                randomValue = (ulong)random.Next(1, int.MaxValue) + (ulong)random.Next(0, int.MaxValue);
            }while(randomValue >= (ulong)uint.MaxValue);

            return (uint)randomValue;
        }

        private void RoutingManagerSendRoutingTable()
        {
            while(true)
            {
                Thread.Sleep(1000); // 1s

                routingManager.SendRoutingTable();
            }
        }

        private void RoutingManagerUpdateRoutingTable()
        {
            while(true)
            {
                Thread.Sleep(5); // 5ms

                routingManager.UpdateRoutingTable();
            }
        }

        private void RoutingManagerDeleteRoutingTable()
        {
            while(true)
            {
                Thread.Sleep(500); // 500ms

                routingManager.DeleteRoutingTable();
            }
        }

        public void RoutingManagerWorker()
        {
            Thread routingManagerSendRoutingTableThread = new Thread(new ThreadStart(RoutingManagerSendRoutingTable));
            routingManagerSendRoutingTableThread.Start();

            Thread routingManagerUpdateRoutingTableThread = new Thread(new ThreadStart(RoutingManagerUpdateRoutingTable));
            routingManagerUpdateRoutingTableThread.Start();

            Thread routingManagerDeleteRoutingTableThread = new Thread(new ThreadStart(RoutingManagerDeleteRoutingTable));
            routingManagerDeleteRoutingTableThread.Start();

            return;
        }

        private void MessageManagerTransferRoutingMessage()
        {
            while(true)
            {
                Thread.Sleep(5); // 5ms

                messageManager.TransferRoutingMessage();
            }
        }

        private void MessageManagerTransferSocks5Message()
        {
            int ret = 0;
            Socks5Message socks5Message = null;
            Server server;
            uint connectionId = 0;
            uint clientId = 0;
            uint serverId = 0;
            object[] parameters;

            while(true)
            {
                socks5Message = messageManager.TransferSocks5Message();
                if(socks5Message != null)
                {
                    connectionId = socks5Message.ConnectionId;
                    clientId = socks5Message.ClientId;
                    server = new Server(spiderIp,
                                        connectionId,
                                        clientId,
                                        0,
                                        socks5Message.DestinationIp,
                                        "0",
                                        socks5Message.SourceIp,
                                        socks5Message.TvSec,
                                        socks5Message.TvUsec,
                                        socks5Message.ForwarderTvSec,
                                        socks5Message.ForwarderTvUsec,
                                        encryption,
                                        clientManager,
                                        serverManager,
                                        pipeManager,
                                        routingManager,
                                        messageManager,
                                        this);

                    do
                    {
                        serverId = GenerateRandomId();
                        ret = serverManager.InsertServer(connectionId,
                                                         serverId,
                                                         server);
                    }while(ret != 0);

#if DEBUGPRINT
                    Console.WriteLine("[+] generate server : {0}",
                                      serverId);
#endif
                    server.ServerId = serverId;

                    parameters = new object[] {server,
                                               socks5Message};

                    Thread thread = new Thread(new ParameterizedThreadStart(ServerWorker));
                    thread.Start(parameters);
                }
            }
        }

        public void MessageManagerWorker()
        {
            Thread messageManagerTransferRoutingMessageThread = new Thread(new ThreadStart(MessageManagerTransferRoutingMessage));
            messageManagerTransferRoutingMessageThread.Start();

            Thread messageManagerTransferSocks5MessageThread = new Thread(new ThreadStart(MessageManagerTransferSocks5Message));
            messageManagerTransferSocks5MessageThread.Start();

            return;
        }

        private void ClientWorker(object obj)
        {
            int ret = 0;
            Client client = obj as Client;


            if(client != null)
            {
                int timeout = client.TvSec * 1000 + client.TvUsec / 1000;
                client.TcpClient.ReceiveTimeout = timeout;
                client.TcpClient.SendTimeout = timeout;

                ret = client.DoSocks5Connection();
                if(ret == -1)
                {
                    if(clientManager.EraseClient(client.ConnectionId,
                                                 client.ClientId))
                    {
                        if(client.TcpClient != null)
                        {
                            client.TcpClient.Dispose();
                        }
                    }
                }
            }

            return;
        }

        private int ListenClient(string clientListenIp,
                                 string clientListenIpScopeId,
                                 string clientListenPort,
                                 string destinationSpiderIp,
                                 int tvSec,
                                 int tvUsec,
                                 int forwarderTvSec,
                                 int forwarderTvUsec)
        {
            int ret = 0;
            string type = "socks5";
            uint connectionId = 0;
            TcpListener clientTcpListener = null;
            IPAddress clientListenerIpAddress = null;
            string clientListenIpTmp = "";
            TcpClient clientTcpClient = null;
            Client clientListen;
            string ipv6LinkLocalPrefix = "fe80:";


            try
            {
                if(clientListenIp.Contains(":") &&  // ipv6 link local
                   clientListenIp.StartsWith(ipv6LinkLocalPrefix, StringComparison.OrdinalIgnoreCase))
                {
                    clientListenIpTmp = clientListenIp + "%" + clientListenIpScopeId;
                    clientListenerIpAddress = IPAddress.Parse(clientListenIpTmp);

                    clientTcpListener = new TcpListener(clientListenerIpAddress,
                                                        int.Parse(clientListenPort));

                    clientTcpListener.Start();

                    Console.WriteLine("[+] listening port {0} on {1}",
                                      clientListenPort,
                                      clientListenIpTmp);

                    clientListen = new Client(type,
                                              0,
                                              0,
                                              0,
                                              clientListenIp,
                                              clientListenIpScopeId,
                                              clientListenPort,
                                              "",
                                              destinationSpiderIp,
                                              clientTcpListener,
                                              null,
                                              tvSec,
                                              tvUsec,
                                              forwarderTvSec,
                                              forwarderTvUsec,
                                              encryption,
                                              messageManager);

                    do
                    {
                        connectionId = GenerateRandomId();
                        ret = clientManager.InsertClient(connectionId,
                                                         0,
                                                         clientListen);
                    }while(ret != 0);

                    while(true)
                    {
                        clientTcpClient = clientTcpListener.AcceptTcpClient();

                        uint clientId = 0;
                        IPEndPoint clientRemoteEndPoint = clientTcpClient.Client.RemoteEndPoint as IPEndPoint;
                        IPAddress clientClientIpAddress = null;
                        int port;
                        long scopeId;
                        string clientIp = "";
                        string clientIpScopeId = "";
                        string clientPort = "";
                        int percentIndex = -1;

                        if(clientRemoteEndPoint != null)
                        {
                            clientClientIpAddress = clientRemoteEndPoint.Address;
                            port = clientRemoteEndPoint.Port;
                            scopeId = clientClientIpAddress.ScopeId;

                            clientIp = clientClientIpAddress.ToString();
                            percentIndex = clientIp.IndexOf('%');
                            if(percentIndex != -1)
                            {
                                clientIp = clientIp.Substring(0, percentIndex);
                            }
                            clientIpScopeId = scopeId.ToString();
                            clientPort = port.ToString();

#if DEBUGPRINT
                            Console.WriteLine("[+] connected from ip:{0}%{1} port:{2}",
                                              clientIp,
                                              clientIpScopeId,
                                              clientPort);
#endif
                            Client client = new Client(type,
                                                       connectionId,
                                                       0,
                                                       0,
                                                       clientIp,
                                                       clientIpScopeId,
                                                       "",
                                                       clientPort,
                                                       destinationSpiderIp,
                                                       null,
                                                       clientTcpClient,
                                                       tvSec,
                                                       tvUsec,
                                                       forwarderTvSec,
                                                       forwarderTvUsec,
                                                       encryption,
                                                       messageManager);

                            do
                            {
                                clientId = GenerateRandomId();
                                ret = clientManager.InsertClient(connectionId,
                                                                 clientId,
                                                                 client);
                            }while(ret != 0);

                            Thread thread = new Thread(new ParameterizedThreadStart(ClientWorker));
                            thread.Start(client);
                        }else
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] clientRemoteEndPoint is null");
#endif
                            clientTcpClient.Dispose();
                            continue;
                        }
                    }
                }else
                {
                    clientListenerIpAddress = IPAddress.Parse(clientListenIp);

                    clientTcpListener = new TcpListener(clientListenerIpAddress,
                                                        int.Parse(clientListenPort));

                    clientTcpListener.Start();

                    Console.WriteLine("[+] listening port {0} on {1}",
                                      clientListenPort,
                                      clientListenIp);

                    clientListen = new Client(type,
                                              0,
                                              0,
                                              0,
                                              clientListenIp,
                                              "",
                                              clientListenPort,
                                              "",
                                              destinationSpiderIp,
                                              clientTcpListener,
                                              null,
                                              tvSec,
                                              tvUsec,
                                              forwarderTvSec,
                                              forwarderTvUsec,
                                              encryption,
                                              messageManager);

                    do
                    {
                        connectionId = GenerateRandomId();
                        ret = clientManager.InsertClient(connectionId,
                                                         0,
                                                         clientListen);
                    }while(ret != 0);

                    while(true)
                    {
                        clientTcpClient = clientTcpListener.AcceptTcpClient();

                        uint clientId = 0;
                        IPEndPoint clientRemoteEndPoint = clientTcpClient.Client.RemoteEndPoint as IPEndPoint;
                        IPAddress clientClientIpAddress = null;
                        int port;
                        string clientIp = "";
                        string clientPort = "";

                        if(clientRemoteEndPoint != null)
                        {
                            clientClientIpAddress = clientRemoteEndPoint.Address;
                            port = clientRemoteEndPoint.Port;

                            clientIp = clientClientIpAddress.ToString();
                            clientPort = port.ToString();

#if DEBUGPRINT
                            Console.WriteLine("[+] connected from ip:{0} port:{1}",
                                              clientIp,
                                              clientPort);
#endif
                            Client client = new Client(type,
                                                       connectionId,
                                                       0,
                                                       0,
                                                       clientIp,
                                                       "",
                                                       "",
                                                       clientPort,
                                                       destinationSpiderIp,
                                                       null,
                                                       clientTcpClient,
                                                       tvSec,
                                                       tvUsec,
                                                       forwarderTvSec,
                                                       forwarderTvUsec,
                                                       encryption,
                                                       messageManager);

                            do
                            {
                                clientId = GenerateRandomId();
                                ret = clientManager.InsertClient(connectionId,
                                                                 clientId,
                                                                 client);
                            }while(ret != 0);

                            Thread thread = new Thread(new ParameterizedThreadStart(ClientWorker));
                            thread.Start(client);
                        }else
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] clientRemoteEndPoint is null");
#endif
                            clientTcpClient.Dispose();
                            continue;
                        }
                    }
                }
            }catch(Exception ex)
            {
                Console.WriteLine("[-] ListenClient error: {0}",
                                  ex.Message);
            }

            clientManager.EraseClient(connectionId,
                                      0);

            if(clientTcpListener != null)
            {
                clientTcpListener.Stop();
            }

            return 0;
        }

        private void ListenClient(object obj)
        {
            object[] parameters = obj as object[];

            string clientListenIp = parameters[0] as string;
            string clientListenIpScopeId = parameters[1] as string;
            string clientListenPort = parameters[2] as string;
            string destinationSpiderIp = parameters[3] as string;
            int tvSec = (int)parameters[4];
            int tvUsec = (int)parameters[5];
            int forwarderTvSec = (int)parameters[6];
            int forwarderTvUsec = (int)parameters[7];


            if(clientListenIp != null &&
               clientListenIpScopeId != null &&
               clientListenPort != null &&
               destinationSpiderIp != null)
            {
                int ret = ListenClient(clientListenIp,
                                       clientListenIpScopeId,
                                       clientListenPort,
                                       destinationSpiderIp,
                                       tvSec,
                                       tvUsec,
                                       forwarderTvSec,
                                       forwarderTvUsec);
            }

            return;
        }

        public void AddNodeSpiderClient()
        {
            string config = "";
            char mode;  // self:s other:o
            string sourceSpiderIp = "";
            string sourceSpiderIpScopeId = "";
            string destinationSpiderIp = "";
            string clientListenIp = "";
            string clientListenIpScopeId = "";
            string clientListenPort = "";
            string clientDestinationSpiderIp = "";
            int tvSec = 0;
            int tvUsec = 0;
            int forwarderTvSec = 0;
            int forwarderTvUsec = 0;
            string input = "";
            byte[] tmp;
            char check = 'n';
            object[] parameters;


            while(true)
            {
                routingManager.ShowRoutingTable();
                Console.WriteLine("");

                Console.Write("mode (self:s other:o)                          > ");
                input = Console.ReadLine();
                input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                mode = input[0];
                if(mode == 's')   // self
                {
                    Console.Write("client listen ip                               > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientListenIp = Encoding.UTF8.GetString(tmp);

                    if((String.Compare(clientListenIp, spiderIp.SpiderIpv4) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6Global) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                    {
                        Console.WriteLine("[-] please input spider ipv4 or ipv6");
                        continue;
                    }

                    if(String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                    {
                        clientListenIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                    }

                    Console.Write("client listen port                             > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientListenPort = Encoding.UTF8.GetString(tmp);

                    Console.Write("client destination spider ip                   > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientDestinationSpiderIp = Encoding.UTF8.GetString(tmp);

                    Console.Write("recv/send tv_sec  (timeout 0-60 sec)           > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        tvSec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        tvSec = 3;
                    }

                    if(tvSec < 0 || tvSec > 60)
                    {
                        tvSec = 3;
                    }

                    Console.Write("recv/send tv_usec (timeout 0-1000000 microsec) > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        tvUsec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        tvUsec = 3;
                    }

                    if(tvUsec < 0 || tvUsec > 1000000)
                    {
                        tvUsec = 0;
                    }

                    if(tvSec == 0 && tvUsec == 0){
                        tvSec = 3;
                        tvUsec = 0;
                    }

                    Console.Write("forwarder tv_sec  (timeout 0-3600 sec)         > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        forwarderTvSec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        forwarderTvSec = 30;
                    }

                    if(forwarderTvSec < 0 || forwarderTvSec > 3600)
                    {
                        forwarderTvSec = 30;
                    }

                    Console.Write("forwarder tv_usec (timeout 0-1000000 microsec) > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        forwarderTvUsec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvUsec < 0 || forwarderTvUsec > 1000000)
                    {
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvSec == 0 && forwarderTvUsec == 0)
                    {
                        forwarderTvSec = 30;
                        forwarderTvUsec = 0;
                    }

                    Console.WriteLine("");
                    Console.WriteLine("client listen ip             : {0}", clientListenIp);
                    if(!string.IsNullOrEmpty(clientListenIpScopeId))
                    {
                        Console.WriteLine("client listen ip scope id    : {0}", clientListenIpScopeId);
                    }
                    Console.WriteLine("client listen port           : {0}", clientListenPort);
                    Console.WriteLine("client destination spider ip : {0}", clientDestinationSpiderIp);
                    Console.WriteLine("recv/send tv_sec             : {0,7} sec", tvSec);
                    Console.WriteLine("recv/send tv_usec            : {0,7} microsec", tvUsec);
                    Console.WriteLine("forwarder_tv_sec             : {0,7} sec", forwarderTvSec);
                    Console.WriteLine("forwarder_tv_usec            : {0,7} microsec", forwarderTvUsec);
                    Console.WriteLine("");

                    Console.Write("ok? (yes:y no:n quit:q)                        > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    check = input[0];
                    if(check == 'y')
                    {
                        parameters = new object[] {clientListenIp,
                                                   clientListenIpScopeId,
                                                   clientListenPort,
                                                   clientDestinationSpiderIp,
                                                   tvSec,
                                                   tvUsec,
                                                   forwarderTvSec,
                                                   forwarderTvUsec};

                        Thread thread = new Thread(new ParameterizedThreadStart(ListenClient));
                        thread.Start(parameters);

                        Thread.Sleep(2000); // 2s

                        break;
                    }else if(check == 'n')
                    {
                        continue;
                    }else if(check == 'q')
                    {
                        return;
                    }else
                    {
                        return;
                    }
                }else if(mode == 'o')   // other
                {
                    Console.Write("source spider ip                               > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    sourceSpiderIp = Encoding.UTF8.GetString(tmp);

                    if((String.Compare(sourceSpiderIp, spiderIp.SpiderIpv4) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6Global) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                    {
                        Console.WriteLine("[-] please input spider ipv4 or ipv6");
                        continue;
                    }

                    if(String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                    {
                        sourceSpiderIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                    }

                    Console.Write("destination spider ip                          > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    destinationSpiderIp = Encoding.UTF8.GetString(tmp);

                    Console.Write("client listen ip                               > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientListenIp = Encoding.UTF8.GetString(tmp);

                    Console.Write("client listen port                             > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientListenPort = Encoding.UTF8.GetString(tmp);

                    Console.Write("client destination spider ip                   > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientDestinationSpiderIp = Encoding.UTF8.GetString(tmp);

                    Console.Write("recv/send tv_sec  (timeout 0-60 sec)           > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        tvSec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        tvSec = 3;
                    }

                    if(tvSec < 0 || tvSec > 60)
                    {
                        tvSec = 3;
                    }

                    Console.Write("recv/send tv_usec (timeout 0-1000000 microsec) > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        tvUsec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        tvUsec = 3;
                    }

                    if(tvUsec < 0 || tvUsec > 1000000)
                    {
                        tvUsec = 0;
                    }

                    if(tvSec == 0 && tvUsec == 0){
                        tvSec = 3;
                        tvUsec = 0;
                    }

                    Console.Write("forwarder tv_sec  (timeout 0-3600 sec)         > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        forwarderTvSec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        forwarderTvSec = 30;
                    }

                    if(forwarderTvSec < 0 || forwarderTvSec > 3600)
                    {
                        forwarderTvSec = 30;
                    }

                    Console.Write("forwarder tv_usec (timeout 0-1000000 microsec) > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        forwarderTvUsec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvUsec < 0 || forwarderTvUsec > 1000000)
                    {
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvSec == 0 && forwarderTvUsec == 0)
                    {
                        forwarderTvSec = 30;
                        forwarderTvUsec = 0;
                    }

                    Console.WriteLine("");
                    Console.WriteLine("source spider ip             : {0}", sourceSpiderIp);
                    if(!string.IsNullOrEmpty(sourceSpiderIpScopeId))
                    {
                        Console.WriteLine("source spider ip scope id    : {0} ({1})", sourceSpiderIpScopeId, if_nametoindex(sourceSpiderIpScopeId));
                    }
                    Console.WriteLine("destination spider ip        : {0}", destinationSpiderIp);
                    Console.WriteLine("client listen ip             : {0}", clientListenIp);
                    Console.WriteLine("client listen port           : {0}", clientListenPort);
                    Console.WriteLine("client destination spider ip : {0}", clientDestinationSpiderIp);
                    Console.WriteLine("recv/send tv_sec             : {0,7} sec", tvSec);
                    Console.WriteLine("recv/send tv_usec            : {0,7} microsec", tvUsec);
                    Console.WriteLine("forwarder_tv_sec             : {0,7} sec", forwarderTvSec);
                    Console.WriteLine("forwarder_tv_usec            : {0,7} microsec", forwarderTvUsec);
                    Console.WriteLine("");

                    Console.Write("ok? (yes:y no:n quit:q)                        > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    check = input[0];
                    if(check == 'y')
                    {
                        config = "";
                        config += "[client]\n";

                        config += "client_listen_ip:";
                        config += clientListenIp;
                        config += "\n";

                        config += "client_listen_port:";
                        config += clientListenPort;
                        config += "\n";

                        config += "client_destination_spider_ip:";
                        config += clientDestinationSpiderIp;
                        config += "\n";

                        config += "tv_sec:";
                        config += tvSec.ToString();
                        config += "\n";

                        config += "tv_usec:";
                        config += tvUsec.ToString();
                        config += "\n";

                        config += "forwarder_tv_sec:";
                        config += forwarderTvSec.ToString();
                        config += "\n";

                        config += "forwarder_tv_usec:";
                        config += forwarderTvUsec.ToString();
                        config += "\n";

                        parameters = new object[] {config,
                                                   sourceSpiderIp,
                                                   sourceSpiderIpScopeId,
                                                   destinationSpiderIp};

                        Thread thread = new Thread(new ParameterizedThreadStart(AddNodeToDestinationSpiderWorker));
                        thread.Start(parameters);

                        break;
                    }else if(check == 'n')
                    {
                        continue;
                    }else if(check == 'q')
                    {
                        return;
                    }else
                    {
                        return;
                    }
                }else
                {
                    return;
                }
            }

            return;
        }

        private void ServerWorker(object obj)
        {
            int ret = 0;
            object[] parameters = obj as object[];
            Server server = parameters[0] as Server;
            Socks5Message socks5Message = parameters[1] as Socks5Message;


            if(server != null &&
               socks5Message != null)
            {
                ret = server.DoSocks5Connection(socks5Message);
                if(ret == -1)
                {
                    serverManager.EraseServer(server.ConnectionId,
                                              server.ServerId);
                }
            }

            return;
        }

        private void PipeRecvMessage(object obj)
        {
            int ret = 0;
            uint pipeKey;
            Pipe pipe = obj as Pipe;


            if(pipe != null)
            {
                pipeKey = pipe.PipeId;

                while(true)
                {
                    ret = pipe.RecvMessage();
                    if(ret == -1)
                    {
                        if(pipeManager.ErasePipe(pipeKey))
                        {
                            if(pipe.TcpClient != null)
                            {
                                pipe.TcpClient.Dispose();
                            }
                        }
                        break;
                    }
                }
            }

            return;
        }

        private void PipeSendRoutingMessage(object obj)
        {
            int ret = 0;
            uint pipeKey;
            Pipe pipe = obj as Pipe;


            if(pipe != null)
            {
                pipeKey = pipe.PipeId;

                while(true)
                {
                    ret = pipe.SendRoutingMessage();
                    if(ret == -1)
                    {
                        if(pipeManager.ErasePipe(pipeKey))
                        {
                            if(pipe.TcpClient != null)
                            {
                                pipe.TcpClient.Dispose();
                            }
                        }
                        break;
                    }
                }
            }

            return;
        }

        private void PipeSendSocks5Message(object obj)
        {
            int ret = 0;
            uint pipeKey;
            Pipe pipe = obj as Pipe;


            if(pipe != null)
            {
                pipeKey = pipe.PipeId;

                while(true)
                {
                    ret = pipe.SendSocks5Message();
                    if(ret == -1)
                    {
                        if(pipeManager.ErasePipe(pipeKey))
                        {
                            if(pipe.TcpClient != null)
                            {
                                pipe.TcpClient.Dispose();
                            }
                        }
                        break;
                    }
                }
            }

            return;
        }

        private void PipeWorker(object obj)
        {
            Pipe pipe = obj as Pipe;


            if(pipe != null)
            {
                Thread pipeRecvMessageThread = new Thread(new ParameterizedThreadStart(PipeRecvMessage));
                pipeRecvMessageThread.Start(pipe);

                Thread pipeSendRoutingMessageThread = new Thread(new ParameterizedThreadStart(PipeSendRoutingMessage));
                pipeSendRoutingMessageThread.Start(pipe);

                Thread pipeSendSocks5MessageThread = new Thread(new ParameterizedThreadStart(PipeSendSocks5Message));
                pipeSendSocks5MessageThread.Start(pipe);
            }

            return;
        }

        private int ConnectPipe(char mode,
                                string pipeIp,
                                string pipeIpScopeId,
                                string pipeDestinationIp,
                                string pipeDestinationPort)
        {
            int ret = 0;
            TcpClient pipeTcpClient = null;
            string pipeDestinationIpTmp = "";
            string pipeDestinationIpScodeId = "";
            Pipe pipe;
            uint pipeKey;
            string ipv6LinkLocalPrefix = "fe80:";


            try
            {
                if(pipeDestinationIp.Contains(":") &&   // ipv6 link local
                   pipeDestinationIp.StartsWith(ipv6LinkLocalPrefix, StringComparison.OrdinalIgnoreCase))
                {
                    pipeDestinationIpScodeId = pipeIpScopeId;
                    pipeDestinationIpTmp = pipeDestinationIp + "%" + pipeDestinationIpScodeId;

                    Console.WriteLine("[+] connecting to ip:{0} port:{1}",
                                      pipeDestinationIpTmp,
                                      pipeDestinationPort);

                    pipeTcpClient = new TcpClient(pipeDestinationIpTmp,
                                                  int.Parse(pipeDestinationPort));

                    if(pipeTcpClient != null)
                    {
                        Console.WriteLine("[+] connected to ip:{0} port:{1}",
                                          pipeDestinationIpTmp,
                                          pipeDestinationPort);
                    }
                }else
                {
                    Console.WriteLine("[+] connecting to ip:{0} port:{1}",
                                      pipeDestinationIp,
                                      pipeDestinationPort);

                    pipeTcpClient = new TcpClient(pipeDestinationIp,
                                                  int.Parse(pipeDestinationPort));

                    if(pipeTcpClient != null)
                    {
                        Console.WriteLine("[+] connected to ip:{0} port:{1}",
                                          pipeDestinationIp,
                                          pipeDestinationPort);
                    }
                }
            }catch(Exception ex)
            {
                Console.WriteLine("[-] ConnectPipe error: {0}",
                                  ex.Message);
                return -1;
            }

            if(pipeTcpClient != null)
            {
                pipe = new Pipe(spiderIp,
                                0,
                                mode,
                                pipeIp,
                                pipeIpScopeId,
                                pipeDestinationIp,
                                pipeDestinationIpScodeId,
                                pipeDestinationPort,
                                pipeTcpClient,
                                routingManager,
                                messageManager);

                do
                {
                    pipeKey = GenerateRandomId();
                    ret = pipeManager.InsertPipe(pipeKey,
                                                 pipe);
                }while(ret != 0);

                Thread thread = new Thread(new ParameterizedThreadStart(PipeWorker));
                thread.Start(pipe);
            }else
            {
                Console.WriteLine("[-] pipeTcpClient is null");
                return -1;
            }

            return 0;
        }

        public void ConnectPipe(object obj)
        {
            object[] parameters = obj as object[];

            char mode = (char)parameters[0];
            string pipeIp = parameters[1] as string;
            string pipeIpScopeId = parameters[2] as string;
            string pipeDestinationIp = parameters[3] as string;
            string pipeDestinationPort = parameters[4] as string;


            if(pipeIp != null &&
               pipeIpScopeId != null &&
               pipeDestinationIp != null &&
               pipeDestinationPort != null)
            {
                int ret = ConnectPipe(mode,
                                      pipeIp,
                                      pipeIpScopeId,
                                      pipeDestinationIp,
                                      pipeDestinationPort);
            }

            return;
        }

        private int ListenPipe(char mode,
                               string pipeListenIp,
                               string pipeListenIpScopeId,
                               string pipeListenPort)
        {
            int ret = 0;
            uint pipeId = 0;
            TcpListener pipeTcpListener = null;
            IPAddress pipeListenerIpAddress = null;
            string pipeListenIpTmp = "";
            TcpClient pipeTcpClient = null;
            Pipe pipeListen;
            uint pipeListenKey = 0;
            string ipv6LinkLocalPrefix = "fe80:";


            try
            {
                if(pipeListenIp.Contains(":") &&   // ipv6 link local
                   pipeListenIp.StartsWith(ipv6LinkLocalPrefix, StringComparison.OrdinalIgnoreCase))
                {
                    pipeListenIpTmp = pipeListenIp + "%" + pipeListenIpScopeId;
                    pipeListenerIpAddress = IPAddress.Parse(pipeListenIpTmp);

                    pipeTcpListener = new TcpListener(pipeListenerIpAddress,
                                                      int.Parse(pipeListenPort));
                    pipeTcpListener.Start();

                    Console.WriteLine("[+] listening port {0} on {1}",
                                      pipeListenPort,
                                      pipeListenIpTmp);

                    pipeListen = new Pipe(spiderIp,
                                          0,
                                          mode,
                                          pipeListenIp,
                                          pipeListenIpScopeId,
                                          pipeListenPort,
                                          pipeTcpListener,
                                          routingManager,
                                          messageManager);

                    do
                    {
                        pipeListenKey = GenerateRandomId();
                        ret = pipeManager.InsertPipe(pipeListenKey,
                                                     pipeListen);
                    }while(ret != 0);

                    while(true)
                    {
                        pipeTcpClient = pipeTcpListener.AcceptTcpClient();

                        if(pipeId != 0 &&
                           pipeManager.CheckPipe(pipeId))
                        {
                            pipeTcpClient.Dispose();
                            continue;
                        }

                        IPEndPoint pipeRemoteEndPoint = pipeTcpClient.Client.RemoteEndPoint as IPEndPoint;
                        IPAddress pipeClientIpAddress = null;
                        int port;
                        long scopeId;
                        string pipeDestinationIp = "";
                        string pipeDestinationIpScopeId = "";
                        string pipeDestinationPort = "";
                        int percentIndex = -1;

                        if(pipeRemoteEndPoint != null)
                        {
                            pipeClientIpAddress = pipeRemoteEndPoint.Address;
                            port = pipeRemoteEndPoint.Port;
                            scopeId = pipeClientIpAddress.ScopeId;

                            pipeDestinationIp = pipeClientIpAddress.ToString();
                            percentIndex = pipeDestinationIp.IndexOf('%');
                            if(percentIndex != -1)
                            {
                                pipeDestinationIp = pipeDestinationIp.Substring(0, percentIndex);
                            }
                            pipeDestinationIpScopeId = scopeId.ToString();
                            pipeDestinationPort = port.ToString();

#if DEBUGPRINT
                            Console.WriteLine("[+] connected from ip:{0}%{1} port:{2}",
                                              pipeDestinationIp,
                                              pipeDestinationIpScopeId,
                                              pipeDestinationPort);
#endif
                            Pipe pipe = new Pipe(spiderIp,
                                                 0,
                                                 '-',
                                                 pipeListenIp,
                                                 pipeListenIpScopeId,
                                                 pipeDestinationIp,
                                                 pipeDestinationIpScopeId,
                                                 pipeDestinationPort,
                                                 pipeTcpClient,
                                                 routingManager,
                                                 messageManager);

                            do
                            {
                                pipeId = GenerateRandomId();
                                ret = pipeManager.InsertPipe(pipeId,
                                                             pipe);
                            }while(ret != 0);

                            Thread thread = new Thread(new ParameterizedThreadStart(PipeWorker));
                            thread.Start(pipe);
                        }else
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] pipeRemoteEndPoint is null");
#endif
                            pipeTcpClient.Dispose();
                            continue;
                        }
                    }
                }else
                {
                    pipeListenerIpAddress = IPAddress.Parse(pipeListenIp);

                    pipeTcpListener = new TcpListener(pipeListenerIpAddress,
                                                      int.Parse(pipeListenPort));
                    pipeTcpListener.Start();

                    Console.WriteLine("[+] listening port {0} on {1}",
                                      pipeListenPort,
                                      pipeListenIp);

                    pipeListen = new Pipe(spiderIp,
                                          0,
                                          mode,
                                          pipeListenIp,
                                          "",
                                          pipeListenPort,
                                          pipeTcpListener,
                                          routingManager,
                                          messageManager);

                    do
                    {
                        pipeListenKey = GenerateRandomId();
                        ret = pipeManager.InsertPipe(pipeListenKey,
                                                     pipeListen);
                    }while(ret != 0);

                    while(true)
                    {
                        pipeTcpClient = pipeTcpListener.AcceptTcpClient();

                        if(pipeId != 0 &&
                           pipeManager.CheckPipe(pipeId))
                        {
                            pipeTcpClient.Dispose();
                            continue;
                        }

                        IPEndPoint pipeRemoteEndPoint = pipeTcpClient.Client.RemoteEndPoint as IPEndPoint;
                        IPAddress pipeClientIpAddress = null;
                        int port;
                        string pipeDestinationIp = "";
                        string pipeDestinationPort = "";

                        if(pipeRemoteEndPoint != null)
                        {
                            pipeClientIpAddress = pipeRemoteEndPoint.Address;
                            port = pipeRemoteEndPoint.Port;

                            pipeDestinationIp = pipeClientIpAddress.ToString();
                            pipeDestinationPort = port.ToString();

#if DEBUGPRINT
                            Console.WriteLine("[+] connected from ip:{0} port:{1}",
                                              pipeDestinationIp,
                                              pipeDestinationPort);
#endif
                            Pipe pipe = new Pipe(spiderIp,
                                                 0,
                                                 '-',
                                                 pipeListenIp,
                                                 "",
                                                 pipeDestinationIp,
                                                 "",
                                                 pipeDestinationPort,
                                                 pipeTcpClient,
                                                 routingManager,
                                                 messageManager);

                            do
                            {
                                pipeId = GenerateRandomId();
                                ret = pipeManager.InsertPipe(pipeId,
                                                             pipe);
                            }while(ret != 0);

                            Thread thread = new Thread(new ParameterizedThreadStart(PipeWorker));
                            thread.Start(pipe);
                        }else
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] pipeRemoteEndPoint is null");
#endif
                            pipeTcpClient.Dispose();
                            continue;
                        }
                    }
                }
            }catch(Exception ex)
            {
                Console.WriteLine("[-] ListenPipe error: {0}",
                                  ex.Message);
            }

            pipeManager.ErasePipe(pipeListenKey);

            if(pipeTcpListener != null)
            {
                pipeTcpListener.Stop();
            }

            return 0;
        }

        private void ListenPipe(object obj)
        {
            object[] parameters = obj as object[];

            char mode = (char)parameters[0];
            string pipeListenIp = parameters[1] as string;
            string pipeListenIpScopeId = parameters[2] as string;
            string pipeListenPort = parameters[3] as string;


            if(pipeListenIp != null &&
               pipeListenIpScopeId != null &&
               pipeListenPort != null)
            {
                int ret = ListenPipe(mode,
                                     pipeListenIp,
                                     pipeListenIpScopeId,
                                     pipeListenPort);
            }

            return;
        }

        public void AddNodeSpiderPipe()
        {
            string config = "";
            char mode;  // self:s other:o
            char pipeMode;  // client:c server:s
            string sourceSpiderIp = "";
            string sourceSpiderIpScopeId = "";
            string destinationSpiderIp = "";
            string pipeIp = "";
            string pipeIpScopeId = "";
            string pipeDestinationIp = "";
            string pipeDestinationPort = "";
            string pipeListenPort = "";
            string input = "";
            byte[] tmp;
            char check = 'n';
            object[] parameters;


            while(true)
            {
                routingManager.ShowRoutingTable();
                Console.WriteLine("");

                Console.Write("mode (self:s other:o)                          > ");
                input = Console.ReadLine();
                input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                mode = input[0];
                if(mode == 's')   // self
                {
                    Console.Write("pipe mode (client:c server:s)                  > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    pipeMode = input[0];
                    if(pipeMode == 'c')
                    {
                        Console.Write("pipe ip                                        > ");
                        input = Console.ReadLine();
                        input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                        tmp = Encoding.UTF8.GetBytes(input.Trim());
                        pipeIp = Encoding.UTF8.GetString(tmp);

                        if((String.Compare(pipeIp, spiderIp.SpiderIpv4) != 0) &&
                           (String.Compare(pipeIp, spiderIp.SpiderIpv6Global) != 0) &&
                           (String.Compare(pipeIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                           (String.Compare(pipeIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                        {
                            Console.WriteLine("[-] please input spider ipv4 or ipv6");
                            continue;
                        }

                        if(String.Compare(pipeIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                        {
                            pipeIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                        }

                        Console.Write("pipe destination ip                            > ");
                        input = Console.ReadLine();
                        input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                        tmp = Encoding.UTF8.GetBytes(input.Trim());
                        pipeDestinationIp = Encoding.UTF8.GetString(tmp);

                        Console.Write("pipe destination port                          > ");
                        input = Console.ReadLine();
                        input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                        tmp = Encoding.UTF8.GetBytes(input.Trim());
                        pipeDestinationPort = Encoding.UTF8.GetString(tmp);

                        Console.WriteLine("");
                        Console.WriteLine("pipe mode                 : {0}", pipeMode);
                        Console.WriteLine("pipe ip                   : {0}", pipeIp);
                        if(!string.IsNullOrEmpty(pipeIpScopeId))
                        {
                            Console.WriteLine("pipe ip scope id          : {0}", pipeIpScopeId);
                        }
                        Console.WriteLine("pipe destination ip       : {0}", pipeDestinationIp);
                        Console.WriteLine("pipe destination port     : {0}", pipeDestinationPort);
                        Console.WriteLine("");

                        Console.Write("ok? (yes:y no:n quit:q)                        > ");
                        input = Console.ReadLine();
                        input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                        check = input[0];
                        if(check == 'y')
                        {
                            parameters = new object[] {pipeMode,
                                                       pipeIp,
                                                       pipeIpScopeId,
                                                       pipeDestinationIp,
                                                       pipeDestinationPort};

                            Thread thread = new Thread(new ParameterizedThreadStart(ConnectPipe));
                            thread.Start(parameters);

                            Thread.Sleep(5000); // 5s

                            break;
                        }else if(check == 'n')
                        {
                            continue;
                        }else if(check == 'q')
                        {
                            return;
                        }else
                        {
                            return;
                        }
                    }else if(pipeMode == 's')
                    {
                        Console.Write("pipe listen ip                                 > ");
                        input = Console.ReadLine();
                        input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                        tmp = Encoding.UTF8.GetBytes(input.Trim());
                        pipeIp = Encoding.UTF8.GetString(tmp);

                        if((String.Compare(pipeIp, spiderIp.SpiderIpv4) != 0) &&
                           (String.Compare(pipeIp, spiderIp.SpiderIpv6Global) != 0) &&
                           (String.Compare(pipeIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                           (String.Compare(pipeIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                        {
                            Console.WriteLine("[-] please input spider ipv4 or ipv6");
                            continue;
                        }

                        if(String.Compare(pipeIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                        {
                            pipeIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                        }

                        Console.Write("pipe listen port                               > ");
                        input = Console.ReadLine();
                        input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                        tmp = Encoding.UTF8.GetBytes(input.Trim());
                        pipeListenPort = Encoding.UTF8.GetString(tmp);

                        Console.WriteLine("");
                        Console.WriteLine("pipe mode                 : {0}", pipeMode);
                        Console.WriteLine("pipe listen ip            : {0}", pipeIp);
                        if(!string.IsNullOrEmpty(pipeIpScopeId))
                        {
                            Console.WriteLine("pipe listen ip scope id   : {0}", pipeIpScopeId);
                        }
                        Console.WriteLine("pipe listen port          : {0}", pipeListenPort);
                        Console.WriteLine("");

                        Console.Write("ok? (yes:y no:n quit:q)                        > ");
                        input = Console.ReadLine();
                        input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                        check = input[0];
                        if(check == 'y')
                        {
                            parameters = new object[] {pipeMode,
                                                       pipeIp,
                                                       pipeIpScopeId,
                                                       pipeListenPort};

                            Thread thread = new Thread(new ParameterizedThreadStart(ListenPipe));
                            thread.Start(parameters);

                            Thread.Sleep(2000); // 2s

                            break;
                        }else if(check == 'n')
                        {
                            continue;
                        }else if(check == 'q')
                        {
                            return;
                        }else
                        {
                            return;
                        }
                    }else
                    {
                        return;
                    }
                }else if(mode == 'o')   // other
                {
                    Console.Write("pipe mode (client:c server:s)                  > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    pipeMode = input[0];
                    if(pipeMode == 'c')
                    {
                        Console.Write("source spider ip                               > ");
                        input = Console.ReadLine();
                        input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                        tmp = Encoding.UTF8.GetBytes(input.Trim());
                        sourceSpiderIp = Encoding.UTF8.GetString(tmp);

                        if((String.Compare(sourceSpiderIp, spiderIp.SpiderIpv4) != 0) &&
                           (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6Global) != 0) &&
                           (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                           (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                        {
                            Console.WriteLine("[-] please input spider ipv4 or ipv6");
                            continue;
                        }

                        if(String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                        {
                            sourceSpiderIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                        }

                        Console.Write("destination spider ip                          > ");
                        input = Console.ReadLine();
                        input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                        tmp = Encoding.UTF8.GetBytes(input.Trim());
                        destinationSpiderIp = Encoding.UTF8.GetString(tmp);

                        Console.Write("pipe ip                                        > ");
                        input = Console.ReadLine();
                        input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                        tmp = Encoding.UTF8.GetBytes(input.Trim());
                        pipeIp = Encoding.UTF8.GetString(tmp);

                        Console.Write("pipe destination ip                            > ");
                        input = Console.ReadLine();
                        input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                        tmp = Encoding.UTF8.GetBytes(input.Trim());
                        pipeDestinationIp = Encoding.UTF8.GetString(tmp);

                        Console.Write("pipe destination port                          > ");
                        input = Console.ReadLine();
                        input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                        tmp = Encoding.UTF8.GetBytes(input.Trim());
                        pipeDestinationPort = Encoding.UTF8.GetString(tmp);

                        Console.WriteLine("");
                        Console.WriteLine("pipe mode                 : {0}", pipeMode);
                        Console.WriteLine("source spider ip          : {0}", sourceSpiderIp);
                        if(!string.IsNullOrEmpty(sourceSpiderIpScopeId))
                        {
                            Console.WriteLine("source spider ip scope id : {0} ({1})", sourceSpiderIpScopeId, if_nametoindex(sourceSpiderIpScopeId));
                        }
                        Console.WriteLine("destination spider ip     : {0}", destinationSpiderIp);
                        Console.WriteLine("pipe ip                   : {0}", pipeIp);
                        Console.WriteLine("pipe destination ip       : {0}", pipeDestinationIp);
                        Console.WriteLine("pipe destination port     : {0}", pipeDestinationPort);
                        Console.WriteLine("");

                        Console.Write("ok? (yes:y no:n quit:q)                        > ");
                        input = Console.ReadLine();
                        input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                        check = input[0];
                        if(check == 'y')
                        {
                            config = "";
                            config += "[pipe_client]\n";

                            config += "pipe_ip:";
                            config += pipeIp;
                            config += "\n";

                            config += "pipe_destination_ip:";
                            config += pipeDestinationIp;
                            config += "\n";

                            config += "pipe_destination_port:";
                            config += pipeDestinationPort;
                            config += "\n";

                            parameters = new object[] {config,
                                                       sourceSpiderIp,
                                                       sourceSpiderIpScopeId,
                                                       destinationSpiderIp};

                            Thread thread = new Thread(new ParameterizedThreadStart(AddNodeToDestinationSpiderWorker));
                            thread.Start(parameters);

                            break;
                        }else if(check == 'n')
                        {
                            continue;
                        }else if(check == 'q')
                        {
                            return;
                        }else
                        {
                            return;
                        }
                    }else if(pipeMode == 's')
                    {
                        Console.Write("source spider ip                               > ");
                        input = Console.ReadLine();
                        input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                        tmp = Encoding.UTF8.GetBytes(input.Trim());
                        sourceSpiderIp = Encoding.UTF8.GetString(tmp);

                        if((String.Compare(sourceSpiderIp, spiderIp.SpiderIpv4) != 0) &&
                           (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6Global) != 0) &&
                           (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                           (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                        {
                            Console.WriteLine("[-] please input spider ipv4 or ipv6");
                            continue;
                        }

                        if(String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                        {
                            sourceSpiderIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                        }

                        Console.Write("destination spider ip                          > ");
                        input = Console.ReadLine();
                        input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                        tmp = Encoding.UTF8.GetBytes(input.Trim());
                        destinationSpiderIp = Encoding.UTF8.GetString(tmp);

                        Console.Write("pipe listen ip                                 > ");
                        input = Console.ReadLine();
                        input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                        tmp = Encoding.UTF8.GetBytes(input.Trim());
                        pipeIp = Encoding.UTF8.GetString(tmp);

                        Console.Write("pipe listen port                               > ");
                        input = Console.ReadLine();
                        input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                        tmp = Encoding.UTF8.GetBytes(input.Trim());
                        pipeListenPort = Encoding.UTF8.GetString(tmp);

                        Console.WriteLine("");
                        Console.WriteLine("pipe mode                 : {0}", pipeMode);
                        Console.WriteLine("source spider ip          : {0}", sourceSpiderIp);
                        if(!string.IsNullOrEmpty(sourceSpiderIpScopeId))
                        {
                            Console.WriteLine("source spider ip scope id : {0} {1})", sourceSpiderIpScopeId, if_nametoindex(sourceSpiderIpScopeId));
                        }
                        Console.WriteLine("destination spider ip     : {0}", destinationSpiderIp);
                        Console.WriteLine("pipe listen ip            : {0}", pipeIp);
                        Console.WriteLine("pipe listen port          : {0}", pipeListenPort);
                        Console.WriteLine("");

                        Console.Write("ok? (yes:y no:n quit:q)                        > ");
                        input = Console.ReadLine();
                        input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                        check = input[0];
                        if(check == 'y')
                        {
                            config = "";
                            config += "[pipe_server]\n";

                            config += "pipe_listen_ip:";
                            config += pipeIp;
                            config += "\n";

                            config += "pipe_listen_port:";
                            config += pipeListenPort;
                            config += "\n";

                            parameters = new object[] {config,
                                                       sourceSpiderIp,
                                                       sourceSpiderIpScopeId,
                                                       destinationSpiderIp};

                            Thread thread = new Thread(new ParameterizedThreadStart(AddNodeToDestinationSpiderWorker));
                            thread.Start(parameters);

                            break;
                        }else if(check == 'n')
                        {
                            continue;
                        }else if(check == 'q')
                        {
                            return;
                        }else
                        {
                            return;
                        }
                    }else
                    {
                        return;
                    }
                }else
                {
                    return;
                }
            }

            return;
        }

        private void ShowNodeInformationWorker(string sourceSpiderIp,
                                               string sourceSpiderIpScopeId,
                                               string destinationSpiderIp)
        {
            int ret = 0;
            uint connectionId = 0;
            uint clientId = 0;
            Client client;


            client = new Client("show n",
                                0,
                                0,
                                0,
                                sourceSpiderIp,
                                sourceSpiderIpScopeId,
                                "",
                                "",
                                destinationSpiderIp,
                                null,
                                null,
                                SHOW_NODE_INFORMATION_WORKER_TV_SEC,
                                SHOW_NODE_INFORMATION_WORKER_TV_USEC,
                                SHOW_NODE_INFORMATION_WORKER_FORWARDER_TV_SEC,
                                SHOW_NODE_INFORMATION_WORKER_FORWARDER_TV_USEC,
                                encryption,
                                messageManager);

            do
            {
                connectionId = GenerateRandomId();
                clientId = GenerateRandomId();
                ret = clientManager.InsertClient(connectionId,
                                                 clientId,
                                                 client);
            }while(ret != 0);

            ret = client.DoSocks5ConnectionShowNode();
            if(ret == -1)
            {
                clientManager.EraseClient(client.ConnectionId,
                                          client.ClientId);
            }

            return;
        }


        private void ShowNodeInformationWorker(object obj)
        {
            object[] parameters = obj as object[];

            string sourceSpiderIp = parameters[0] as string;
            string sourceSpiderIpScopeId = parameters[1] as string;
            string destinationSpiderIp = parameters[2] as string;


            if(sourceSpiderIp != null &&
               sourceSpiderIpScopeId != null &&
               destinationSpiderIp != null)
            {
                ShowNodeInformationWorker(sourceSpiderIp,
                                          sourceSpiderIpScopeId,
                                          destinationSpiderIp);
            }

            return;
        }

        public void ShowNodeInformation()
        {
            char mode;  // self:s other:o
            string sourceSpiderIp = "";
            string sourceSpiderIpScopeId = "";
            string destinationSpiderIp = "";
            string input = "";
            byte[] tmp;
            char check = 'n';
            object[] parameters;


            while(true)
            {
                Console.Write("mode (self:s other:o)                          > ");
                input = Console.ReadLine();
                input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                mode = input[0];
                if(mode == 's')   // self
                {
                    // client
                    clientManager.ShowClientsMap();

                    // server
                    serverManager.ShowServersMap();

                    // pipe
                    pipeManager.ShowPipesMap();

                    break;
                }else if(mode == 'o')   // other
                {
                    routingManager.ShowRoutingTable();
                    Console.WriteLine("");

                    Console.Write("source spider ip                               > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    sourceSpiderIp = Encoding.UTF8.GetString(tmp);

                    if((String.Compare(sourceSpiderIp, spiderIp.SpiderIpv4) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6Global) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                    {
                        Console.WriteLine("[-] please input spider ipv4 or ipv6");
                        continue;
                    }

                    if(String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                    {
                        sourceSpiderIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                    }

                    Console.Write("destination spider ip                          > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    destinationSpiderIp = Encoding.UTF8.GetString(tmp);

                    Console.WriteLine("");
                    Console.WriteLine("mode                      : {0}", mode);
                    Console.WriteLine("source spider ip          : {0}", sourceSpiderIp);
                    if(!string.IsNullOrEmpty(sourceSpiderIpScopeId))
                    {
                        Console.WriteLine("source spider ip scope id : {0} ({1})", sourceSpiderIpScopeId, if_nametoindex(sourceSpiderIpScopeId));
                    }
                    Console.WriteLine("destination spider ip     : {0}", destinationSpiderIp);
                    Console.WriteLine("");

                    Console.Write("ok? (yes:y no:n quit:q)                        > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    check = input[0];
                    if(check == 'y')
                    {
                        parameters = new object[] {sourceSpiderIp,
                                                   sourceSpiderIpScopeId,
                                                   destinationSpiderIp};

                        Thread thread = new Thread(new ParameterizedThreadStart(ShowNodeInformationWorker));
                        thread.Start(parameters);

                        Thread.Sleep(20000);    // 20s

                        break;
                    }else if(check == 'n')
                    {
                        continue;
                    }else if(check == 'q'){
                        return;
                    }else{
                        return;
                    }
                }else{
                    return;
                }
            }

            return;
        }

        private void ShowRoutingTableWorker(string sourceSpiderIp,
                                            string sourceSpiderIpScopeId,
                                            string destinationSpiderIp)
        {
            int ret = 0;
            uint connectionId = 0;
            uint clientId = 0;
            Client client;


            client = new Client("show r",
                                0,
                                0,
                                0,
                                sourceSpiderIp,
                                sourceSpiderIpScopeId,
                                "",
                                "",
                                destinationSpiderIp,
                                null,
                                null,
                                SHOW_ROUTING_TABLE_WORKER_TV_SEC,
                                SHOW_ROUTING_TABLE_WORKER_TV_USEC,
                                SHOW_ROUTING_TABLE_WORKER_FORWARDER_TV_SEC,
                                SHOW_ROUTING_TABLE_WORKER_FORWARDER_TV_USEC,
                                encryption,
                                messageManager);

            do
            {
                connectionId = GenerateRandomId();
                clientId = GenerateRandomId();
                ret = clientManager.InsertClient(connectionId,
                                                 clientId,
                                                 client);
            }while(ret != 0);

            ret = client.DoSocks5ConnectionShowRoute();
            if(ret == -1)
            {
                clientManager.EraseClient(client.ConnectionId,
                                          client.ClientId);
            }

            return;
        }


        private void ShowRoutingTableWorker(object obj)
        {
            object[] parameters = obj as object[];

            string sourceSpiderIp = parameters[0] as string;
            string sourceSpiderIpScopeId = parameters[1] as string;
            string destinationSpiderIp = parameters[2] as string;


            if(sourceSpiderIp != null &&
               sourceSpiderIpScopeId != null &&
               destinationSpiderIp != null)
            {
                ShowRoutingTableWorker(sourceSpiderIp,
                                       sourceSpiderIpScopeId,
                                       destinationSpiderIp);
            }

            return;
        }

        public void ShowRoutingTable()
        {
            char mode;  // self:s other:o
            string sourceSpiderIp = "";
            string sourceSpiderIpScopeId = "";
            string destinationSpiderIp = "";
            string input = "";
            byte[] tmp;
            char check = 'n';
            object[] parameters;


            while(true)
            {
                Console.Write("mode (self:s other:o)                          > ");
                input = Console.ReadLine();
                input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                mode = input[0];
                if(mode == 's')   // self
                {
                    routingManager.ShowRoutingTable();

                    break;
                }else if(mode == 'o')   // other
                {
                    routingManager.ShowRoutingTable();
                    Console.WriteLine("");

                    Console.Write("source spider ip                               > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    sourceSpiderIp = Encoding.UTF8.GetString(tmp);

                    if((String.Compare(sourceSpiderIp, spiderIp.SpiderIpv4) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6Global) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                    {
                        Console.WriteLine("[-] please input spider ipv4 or ipv6");
                        continue;
                    }

                    if(String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                    {
                        sourceSpiderIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                    }

                    Console.Write("destination spider ip                          > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    destinationSpiderIp = Encoding.UTF8.GetString(tmp);

                    Console.WriteLine("");
                    Console.WriteLine("mode                      : {0}", mode);
                    Console.WriteLine("source spider ip          : {0}", sourceSpiderIp);
                    if(!string.IsNullOrEmpty(sourceSpiderIpScopeId))
                    {
                        Console.WriteLine("source spider ip scope id : {0} ({1})", sourceSpiderIpScopeId, if_nametoindex(sourceSpiderIpScopeId));
                    }
                    Console.WriteLine("destination spider ip     : {0}", destinationSpiderIp);
                    Console.WriteLine("");

                    Console.Write("ok? (yes:y no:n quit:q)                        > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    check = input[0];
                    if(check == 'y')
                    {
                        parameters = new object[] {sourceSpiderIp,
                                                   sourceSpiderIpScopeId,
                                                   destinationSpiderIp};

                        Thread thread = new Thread(new ParameterizedThreadStart(ShowRoutingTableWorker));
                        thread.Start(parameters);

                        Thread.Sleep(20000);    // 20s

                        break;
                    }else if(check == 'n')
                    {
                        continue;
                    }else if(check == 'q'){
                        return;
                    }else{
                        return;
                    }
                }else{
                    return;
                }
            }

            return;
        }

        public void EditRoutingTable()
        {
            char editMode;  // add:a delete:d
            string ip = "";
            byte metric = 0;
            uint pipeId = 0;
            string input = "";
            byte[] tmp;
            char check = 'n';


            while(true)
            {
                routingManager.ShowRoutingTable();
                Console.WriteLine("");

                Console.Write("edit mode (add:a delete:d quit:q)              > ");
                input = Console.ReadLine();
                input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                editMode = input[0];
                if(editMode == 'a')
                {
                    Console.Write("ip address                                     > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    ip = Encoding.UTF8.GetString(tmp);

                    if((String.Compare(ip, spiderIp.SpiderIpv4) == 0) ||
                       (String.Compare(ip, spiderIp.SpiderIpv6Global) == 0) ||
                       (String.Compare(ip, spiderIp.SpiderIpv6UniqueLocal) == 0) ||
                       (String.Compare(ip, spiderIp.SpiderIpv6LinkLocal) == 0))
                    {
                        Console.WriteLine("[-] cannot input spider ipv4 and ipv6");
                        continue;
                    }

                    Console.Write("metric (0 < metric <= {0,3})                     > ", METRIC_MAX);
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        metric = byte.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        continue;
                    }

                    if(metric == 0 || metric > METRIC_MAX)
                    {
                        Console.WriteLine("[-] input error");
                        continue;
                    }

                    pipeManager.ShowPipesMap();

                    Console.Write("pipe id                                        > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        pipeId = uint.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        continue;
                    }

                    Console.WriteLine("");
                    Console.WriteLine("edit mode                 : {0}", editMode);
                    Console.WriteLine("ip address                : {0}", ip);
                    Console.WriteLine("metric                    : {0}", metric);
                    Console.WriteLine("pipe id                   : {0}", pipeId);
                    Console.WriteLine("");

                    Console.Write("ok? (yes:y no:n quit:q)                        > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    check = input[0];;
                    if(check == 'y')
                    {
                        Route route = new Route('s',
                                                ip,
                                                metric,
                                                pipeId);

                        routingManager.AddRoute(route);

                        continue;
                    }else if(check == 'n')
                    {
                        continue;
                    }else if(check == 'q'){
                        return;
                    }else{
                        break;
                    }
                }else if(editMode == 'd')
                {
                    Console.Write("ip address                                     > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    ip = Encoding.UTF8.GetString(tmp);

                    if((String.Compare(ip, spiderIp.SpiderIpv4) == 0) ||
                       (String.Compare(ip, spiderIp.SpiderIpv6Global) == 0) ||
                       (String.Compare(ip, spiderIp.SpiderIpv6UniqueLocal) == 0) ||
                       (String.Compare(ip, spiderIp.SpiderIpv6LinkLocal) == 0))
                    {
                        Console.WriteLine("[-] cannot input spider ipv4 and ipv6");
                        continue;
                    }

                    Console.WriteLine("");
                    Console.WriteLine("edit mode                 : {0}", editMode);
                    Console.WriteLine("ip address                : {0}", ip);
                    Console.WriteLine("");

                    Console.Write("ok? (yes:y no:n quit:q)                        > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    check = input[0];
                    if(check == 'y')
                    {
                        routingManager.DeleteRoute(ip);

                        continue;
                    }else if(check == 'n')
                    {
                        continue;
                    }else if(check == 'q')
                    {
                        return;
                    }else{
                        break;
                    }
                }else if(check == 'q')
                {
                    return;
                }else{
                    return;
                }
            }

            return;
        }

        private void ClientTcpWorker(object obj)
        {
            int ret = 0;
            Client client = obj as Client;


            if(client != null)
            {
                int timeout = client.TvSec * 1000 + client.TvUsec / 1000;
                client.TcpClient.ReceiveTimeout = timeout;
                client.TcpClient.SendTimeout = timeout;

                ret = client.DoSocks5ConnectionTcp();
                if(ret == -1)
                {
                    if(clientManager.EraseClient(client.ConnectionId,
                                                 client.ClientId))
                    {
                        if(client.TcpClient != null)
                        {
                            client.TcpClient.Dispose();
                        }
                    }
                }
            }

            return;
        }

        private int ListenClientTcp(string clientListenIp,
                                    string clientListenIpScopeId,
                                    string clientListenPort,
                                    string destinationSpiderIp,
                                    string targetIp,
                                    string targetPort,
                                    int tvSec,
                                    int tvUsec,
                                    int forwarderTvSec,
                                    int forwarderTvUsec)
        {
            int ret = 0;
            string type = "tcp";
            uint connectionId = 0;
            TcpListener clientTcpListener = null;
            IPAddress clientListenerIpAddress = null;
            string clientListenIpTmp = "";
            TcpClient clientTcpClient = null;
            Client clientListen;
            string ipv6LinkLocalPrefix = "fe80:";


            try
            {
                if(clientListenIp.Contains(":") &&  // ipv6 link local
                   clientListenIp.StartsWith(ipv6LinkLocalPrefix, StringComparison.OrdinalIgnoreCase))
                {
                    clientListenIpTmp = clientListenIp + "%" + clientListenIpScopeId;
                    clientListenerIpAddress = IPAddress.Parse(clientListenIpTmp);

                    clientTcpListener = new TcpListener(clientListenerIpAddress,
                                                        int.Parse(clientListenPort));

                    clientTcpListener.Start();

                    Console.WriteLine("[+] listening port {0} on {1}",
                                      clientListenPort,
                                      clientListenIpTmp);

                    clientListen = new Client(type,
                                              0,
                                              0,
                                              0,
                                              clientListenIp,
                                              clientListenIpScopeId,
                                              clientListenPort,
                                              "",
                                              destinationSpiderIp,
                                              targetIp,
                                              targetPort,
                                              clientTcpListener,
                                              null,
                                              tvSec,
                                              tvUsec,
                                              forwarderTvSec,
                                              forwarderTvUsec,
                                              encryption,
                                              messageManager);

                    do
                    {
                        connectionId = GenerateRandomId();
                        ret = clientManager.InsertClient(connectionId,
                                                         0,
                                                         clientListen);
                    }while(ret != 0);

                    while(true)
                    {
                        clientTcpClient = clientTcpListener.AcceptTcpClient();

                        uint clientId = 0;
                        IPEndPoint clientRemoteEndPoint = clientTcpClient.Client.RemoteEndPoint as IPEndPoint;
                        IPAddress clientClientIpAddress = null;
                        int port;
                        long scopeId;
                        string clientIp = "";
                        string clientIpScopeId = "";
                        string clientPort = "";
                        int percentIndex = -1;

                        if(clientRemoteEndPoint != null)
                        {
                            clientClientIpAddress = clientRemoteEndPoint.Address;
                            port = clientRemoteEndPoint.Port;
                            scopeId = clientClientIpAddress.ScopeId;

                            clientIp = clientClientIpAddress.ToString();
                            percentIndex = clientIp.IndexOf('%');
                            if(percentIndex != -1)
                            {
                                clientIp = clientIp.Substring(0, percentIndex);
                            }
                            clientIpScopeId = scopeId.ToString();
                            clientPort = port.ToString();

#if DEBUGPRINT
                            Console.WriteLine("[+] connected from ip:{0}%{1} port:{2}",
                                              clientIp,
                                              clientIpScopeId,
                                              clientPort);
#endif
                            Client client = new Client(type,
                                                       connectionId,
                                                       0,
                                                       0,
                                                       clientIp,
                                                       clientIpScopeId,
                                                       "",
                                                       clientPort,
                                                       destinationSpiderIp,
                                                       targetIp,
                                                       targetPort,
                                                       null,
                                                       clientTcpClient,
                                                       tvSec,
                                                       tvUsec,
                                                       forwarderTvSec,
                                                       forwarderTvUsec,
                                                       encryption,
                                                       messageManager);

                            do
                            {
                                clientId = GenerateRandomId();
                                ret = clientManager.InsertClient(connectionId,
                                                                 clientId,
                                                                 client);
                            }while(ret != 0);

                            Thread thread = new Thread(new ParameterizedThreadStart(ClientTcpWorker));
                            thread.Start(client);
                        }else
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] clientRemoteEndPoint is null");
#endif
                            clientTcpClient.Dispose();
                            continue;
                        }
                    }
                }else
                {
                    clientListenerIpAddress = IPAddress.Parse(clientListenIp);

                    clientTcpListener = new TcpListener(clientListenerIpAddress,
                                                        int.Parse(clientListenPort));

                    clientTcpListener.Start();

                    Console.WriteLine("[+] listening port {0} on {1}",
                                      clientListenPort,
                                      clientListenIp);

                    clientListen = new Client(type,
                                              0,
                                              0,
                                              0,
                                              clientListenIp,
                                              "",
                                              clientListenPort,
                                              "",
                                              destinationSpiderIp,
                                              targetIp,
                                              targetPort,
                                              clientTcpListener,
                                              null,
                                              tvSec,
                                              tvUsec,
                                              forwarderTvSec,
                                              forwarderTvUsec,
                                              encryption,
                                              messageManager);

                    do
                    {
                        connectionId = GenerateRandomId();
                        ret = clientManager.InsertClient(connectionId,
                                                         0,
                                                         clientListen);
                    }while(ret != 0);

                    while(true)
                    {
                        clientTcpClient = clientTcpListener.AcceptTcpClient();

                        uint clientId = 0;
                        IPEndPoint clientRemoteEndPoint = clientTcpClient.Client.RemoteEndPoint as IPEndPoint;
                        IPAddress clientClientIpAddress = null;
                        int port;
                        string clientIp = "";
                        string clientPort = "";

                        if(clientRemoteEndPoint != null)
                        {
                            clientClientIpAddress = clientRemoteEndPoint.Address;
                            port = clientRemoteEndPoint.Port;

                            clientIp = clientClientIpAddress.ToString();
                            clientPort = port.ToString();

#if DEBUGPRINT
                            Console.WriteLine("[+] connected from ip:{0} port:{1}",
                                              clientIp,
                                              clientPort);
#endif
                            Client client = new Client(type,
                                                       connectionId,
                                                       0,
                                                       0,
                                                       clientIp,
                                                       "",
                                                       "",
                                                       clientPort,
                                                       destinationSpiderIp,
                                                       targetIp,
                                                       targetPort,
                                                       null,
                                                       clientTcpClient,
                                                       tvSec,
                                                       tvUsec,
                                                       forwarderTvSec,
                                                       forwarderTvUsec,
                                                       encryption,
                                                       messageManager);

                            do
                            {
                                clientId = GenerateRandomId();
                                ret = clientManager.InsertClient(connectionId,
                                                                 clientId,
                                                                 client);
                            }while(ret != 0);

                            Thread thread = new Thread(new ParameterizedThreadStart(ClientTcpWorker));
                            thread.Start(client);
                        }else
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] clientRemoteEndPoint is null");
#endif
                            clientTcpClient.Dispose();
                            continue;
                        }
                    }
                }
            }catch(Exception ex)
            {
                Console.WriteLine("[-] ListenClient error: {0}",
                                  ex.Message);
            }

            clientManager.EraseClient(connectionId,
                                      0);

            if(clientTcpListener != null)
            {
                clientTcpListener.Stop();
            }

            return 0;
        }

        private void ListenClientTcp(object obj)
        {
            object[] parameters = obj as object[];

            string clientListenIp = parameters[0] as string;
            string clientListenIpScopeId = parameters[1] as string;
            string clientListenPort = parameters[2] as string;
            string destinationSpiderIp = parameters[3] as string;
            string targetIp = parameters[4] as string;
            string targetPort = parameters[5] as string;
            int tvSec = (int)parameters[6];
            int tvUsec = (int)parameters[7];
            int forwarderTvSec = (int)parameters[8];
            int forwarderTvUsec = (int)parameters[9];


            if(clientListenIp != null &&
               clientListenIpScopeId != null &&
               clientListenPort != null &&
               destinationSpiderIp != null &&
               targetIp != null &&
               targetPort != null)
            {
                int ret = ListenClientTcp(clientListenIp,
                                          clientListenIpScopeId,
                                          clientListenPort,
                                          destinationSpiderIp,
                                          targetIp,
                                          targetPort,
                                          tvSec,
                                          tvUsec,
                                          forwarderTvSec,
                                          forwarderTvUsec);
            }

            return;
        }

        public void AddNodeSpiderClientTcp()
        {
            string config = "";
            char mode;  // self:s other:o
            string sourceSpiderIp = "";
            string sourceSpiderIpScopeId = "";
            string destinationSpiderIp = "";
            string clientListenIp = "";
            string clientListenIpScopeId = "";
            string clientListenPort = "";
            string clientDestinationSpiderIp = "";
            string targetIp = "";      // ipv4, domainname, ipv6
            string targetPort = "";
            int tvSec = 0;
            int tvUsec = 0;
            int forwarderTvSec = 0;
            int forwarderTvUsec = 0;
            string input = "";
            byte[] tmp;
            char check = 'n';
            object[] parameters;


            while(true)
            {
                routingManager.ShowRoutingTable();
                Console.WriteLine("");

                Console.Write("mode (self:s other:o)                          > ");
                input = Console.ReadLine();
                input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                mode = input[0];
                if(mode == 's')   // self
                {
                    Console.Write("client listen ip                               > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientListenIp = Encoding.UTF8.GetString(tmp);

                    if((String.Compare(clientListenIp, spiderIp.SpiderIpv4) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6Global) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                    {
                        Console.WriteLine("[-] please input spider ipv4 or ipv6");
                        continue;
                    }

                    if(String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                    {
                        clientListenIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                    }

                    Console.Write("client listen port                             > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientListenPort = Encoding.UTF8.GetString(tmp);

                    Console.Write("client destination spider ip                   > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientDestinationSpiderIp = Encoding.UTF8.GetString(tmp);

                    Console.Write("target ip (ipv4<16, domainname<256, ipv6<46)   > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    targetIp = Encoding.UTF8.GetString(tmp);

                    if(targetIp.Length >= 256)
                    {
                        Console.WriteLine("[-] input error");
                        continue;
                    }

                    Console.Write("target port                                    > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    targetPort = Encoding.UTF8.GetString(tmp);

                    Console.Write("recv/send tv_sec  (timeout 0-60 sec)           > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        tvSec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        tvSec = 3;
                    }

                    if(tvSec < 0 || tvSec > 60)
                    {
                        tvSec = 3;
                    }

                    Console.Write("recv/send tv_usec (timeout 0-1000000 microsec) > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        tvUsec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        tvUsec = 3;
                    }

                    if(tvUsec < 0 || tvUsec > 1000000)
                    {
                        tvUsec = 0;
                    }

                    if(tvSec == 0 && tvUsec == 0){
                        tvSec = 3;
                        tvUsec = 0;
                    }

                    Console.Write("forwarder tv_sec  (timeout 0-3600 sec)         > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        forwarderTvSec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        forwarderTvSec = 30;
                    }

                    if(forwarderTvSec < 0 || forwarderTvSec > 3600)
                    {
                        forwarderTvSec = 30;
                    }

                    Console.Write("forwarder tv_usec (timeout 0-1000000 microsec) > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        forwarderTvUsec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvUsec < 0 || forwarderTvUsec > 1000000)
                    {
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvSec == 0 && forwarderTvUsec == 0)
                    {
                        forwarderTvSec = 30;
                        forwarderTvUsec = 0;
                    }

                    Console.WriteLine("");
                    Console.WriteLine("client listen ip             : {0}", clientListenIp);
                    if(!string.IsNullOrEmpty(clientListenIpScopeId))
                    {
                        Console.WriteLine("client listen ip scope id    : {0}", clientListenIpScopeId);
                    }
                    Console.WriteLine("client listen port           : {0}", clientListenPort);
                    Console.WriteLine("client destination spider ip : {0}", clientDestinationSpiderIp);
                    Console.WriteLine("target ip                    : {0}", targetIp);
                    Console.WriteLine("target port                  : {0}", targetPort);
                    Console.WriteLine("recv/send tv_sec             : {0,7} sec", tvSec);
                    Console.WriteLine("recv/send tv_usec            : {0,7} microsec", tvUsec);
                    Console.WriteLine("forwarder_tv_sec             : {0,7} sec", forwarderTvSec);
                    Console.WriteLine("forwarder_tv_usec            : {0,7} microsec", forwarderTvUsec);
                    Console.WriteLine("");

                    Console.Write("ok? (yes:y no:n quit:q)                        > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    check = input[0];
                    if(check == 'y')
                    {
                        parameters = new object[] {clientListenIp,
                                                   clientListenIpScopeId,
                                                   clientListenPort,
                                                   clientDestinationSpiderIp,
                                                   targetIp,
                                                   targetPort,
                                                   tvSec,
                                                   tvUsec,
                                                   forwarderTvSec,
                                                   forwarderTvUsec};

                        Thread thread = new Thread(new ParameterizedThreadStart(ListenClientTcp));
                        thread.Start(parameters);

                        Thread.Sleep(2000); // 2s

                        break;
                    }else if(check == 'n')
                    {
                        continue;
                    }else if(check == 'q')
                    {
                        return;
                    }else
                    {
                        return;
                    }
                }else if(mode == 'o')   // other
                {
                    Console.Write("source spider ip                               > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    sourceSpiderIp = Encoding.UTF8.GetString(tmp);

                    if((String.Compare(sourceSpiderIp, spiderIp.SpiderIpv4) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6Global) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                    {
                        Console.WriteLine("[-] please input spider ipv4 or ipv6");
                        continue;
                    }

                    if(String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                    {
                        sourceSpiderIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                    }

                    Console.Write("destination spider ip                          > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    destinationSpiderIp = Encoding.UTF8.GetString(tmp);

                    Console.Write("client listen ip                               > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientListenIp = Encoding.UTF8.GetString(tmp);

                    Console.Write("client listen port                             > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientListenPort = Encoding.UTF8.GetString(tmp);

                    Console.Write("client destination spider ip                   > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientDestinationSpiderIp = Encoding.UTF8.GetString(tmp);

                    Console.Write("target ip (ipv4<16, domainname<256, ipv6<46)   > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    targetIp = Encoding.UTF8.GetString(tmp);

                    if(targetIp.Length >= 256)
                    {
                        Console.WriteLine("[-] input error");
                        continue;
                    }

                    Console.Write("target port                                    > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    targetPort = Encoding.UTF8.GetString(tmp);

                    Console.Write("recv/send tv_sec  (timeout 0-60 sec)           > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        tvSec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        tvSec = 3;
                    }

                    if(tvSec < 0 || tvSec > 60)
                    {
                        tvSec = 3;
                    }

                    Console.Write("recv/send tv_usec (timeout 0-1000000 microsec) > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        tvUsec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        tvUsec = 3;
                    }

                    if(tvUsec < 0 || tvUsec > 1000000)
                    {
                        tvUsec = 0;
                    }

                    if(tvSec == 0 && tvUsec == 0){
                        tvSec = 3;
                        tvUsec = 0;
                    }

                    Console.Write("forwarder tv_sec  (timeout 0-3600 sec)         > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        forwarderTvSec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        forwarderTvSec = 30;
                    }

                    if(forwarderTvSec < 0 || forwarderTvSec > 3600)
                    {
                        forwarderTvSec = 30;
                    }

                    Console.Write("forwarder tv_usec (timeout 0-1000000 microsec) > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        forwarderTvUsec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvUsec < 0 || forwarderTvUsec > 1000000)
                    {
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvSec == 0 && forwarderTvUsec == 0)
                    {
                        forwarderTvSec = 30;
                        forwarderTvUsec = 0;
                    }

                    Console.WriteLine("");
                    Console.WriteLine("source spider ip             : {0}", sourceSpiderIp);
                    if(!string.IsNullOrEmpty(sourceSpiderIpScopeId))
                    {
                        Console.WriteLine("source spider ip scope id    : {0} ({1})", sourceSpiderIpScopeId, if_nametoindex(sourceSpiderIpScopeId));
                    }
                    Console.WriteLine("destination spider ip        : {0}", destinationSpiderIp);
                    Console.WriteLine("client listen ip             : {0}", clientListenIp);
                    Console.WriteLine("client listen port           : {0}", clientListenPort);
                    Console.WriteLine("client destination spider ip : {0}", clientDestinationSpiderIp);
                    Console.WriteLine("target ip                    : {0}", targetIp);
                    Console.WriteLine("target port                  : {0}", targetPort);
                    Console.WriteLine("recv/send tv_sec             : {0,7} sec", tvSec);
                    Console.WriteLine("recv/send tv_usec            : {0,7} microsec", tvUsec);
                    Console.WriteLine("forwarder_tv_sec             : {0,7} sec", forwarderTvSec);
                    Console.WriteLine("forwarder_tv_usec            : {0,7} microsec", forwarderTvUsec);
                    Console.WriteLine("");

                    Console.Write("ok? (yes:y no:n quit:q)                        > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    check = input[0];
                    if(check == 'y')
                    {
                        config = "";
                        config += "[client_tcp]\n";

                        config += "client_listen_ip:";
                        config += clientListenIp;
                        config += "\n";

                        config += "client_listen_port:";
                        config += clientListenPort;
                        config += "\n";

                        config += "client_destination_spider_ip:";
                        config += clientDestinationSpiderIp;
                        config += "\n";

                        config += "target_ip:";
                        config += targetIp;
                        config += "\n";

                        config += "target_port:";
                        config += targetPort;
                        config += "\n";

                        config += "tv_sec:";
                        config += tvSec.ToString();
                        config += "\n";

                        config += "tv_usec:";
                        config += tvUsec.ToString();
                        config += "\n";

                        config += "forwarder_tv_sec:";
                        config += forwarderTvSec.ToString();
                        config += "\n";

                        config += "forwarder_tv_usec:";
                        config += forwarderTvUsec.ToString();
                        config += "\n";

                        parameters = new object[] {config,
                                                   sourceSpiderIp,
                                                   sourceSpiderIpScopeId,
                                                   destinationSpiderIp};

                        Thread thread = new Thread(new ParameterizedThreadStart(AddNodeToDestinationSpiderWorker));
                        thread.Start(parameters);

                        break;
                    }else if(check == 'n')
                    {
                        continue;
                    }else if(check == 'q')
                    {
                        return;
                    }else
                    {
                        return;
                    }
                }else
                {
                    return;
                }
            }

            return;
        }

        private void ClientUdpWorker(string clientListenIp,
                                     string clientListenIpScopeId,
                                     string clientListenPort,
                                     string destinationSpiderIp,
                                     string targetIp,
                                     string targetPort,
                                     int tvSec,
                                     int tvUsec,
                                     int forwarderTvSec,
                                     int forwarderTvUsec)
        {
            int ret = 0;
            uint connectionId = 0;
            uint clientId = 0;
            Client clientUdp;


            clientUdp = new Client("udp",
                                   0,
                                   0,
                                   0,
                                   clientListenIp,
                                   clientListenIpScopeId,
                                   clientListenPort,
                                   "",
                                   destinationSpiderIp,
                                   targetIp,
                                   targetPort,
                                   null,
                                   tvSec,
                                   tvUsec,
                                   forwarderTvSec,
                                   forwarderTvUsec,
                                   encryption,
                                   messageManager);

            do
            {
                connectionId = GenerateRandomId();
                clientId = GenerateRandomId();
                ret = clientManager.InsertClient(connectionId,
                                                 clientId,
                                                 clientUdp);
            }while(ret != 0);

            ret = clientUdp.DoSocks5ConnectionUdp();
            if(ret == -1)
            {
                clientManager.EraseClient(clientUdp.ConnectionId,
                                          clientUdp.ClientId);
            }

            return;
        }

        private void ClientUdpWorker(object obj)
        {
            object[] parameters = obj as object[];

            string clientListenIp = parameters[0] as string;
            string clientListenIpScopeId = parameters[1] as string;
            string clientListenPort = parameters[2] as string;
            string destinationSpiderIp = parameters[3] as string;
            string targetIp = parameters[4] as string;
            string targetPort = parameters[5] as string;
            int tvSec = (int)parameters[6];
            int tvUsec = (int)parameters[7];
            int forwarderTvSec = (int)parameters[8];
            int forwarderTvUsec = (int)parameters[9];


            if(clientListenIp != null &&
               clientListenIpScopeId != null &&
               clientListenPort != null &&
               destinationSpiderIp != null &&
               targetIp != null &&
               targetPort != null)
            {
                ClientUdpWorker(clientListenIp,
                                clientListenIpScopeId,
                                clientListenPort,
                                destinationSpiderIp,
                                targetIp,
                                targetPort,
                                tvSec,
                                tvUsec,
                                forwarderTvSec,
                                forwarderTvUsec);
            }

            return;
        }

        public void AddNodeSpiderClientUdp()
        {
            string config = "";
            char mode;  // self:s other:o
            string sourceSpiderIp = "";
            string sourceSpiderIpScopeId = "";
            string destinationSpiderIp = "";
            string clientListenIp = "";
            string clientListenIpScopeId = "";
            string clientListenPort = "";
            string clientDestinationSpiderIp = "";
            string targetIp = "";      // ipv4, domainname, ipv6
            string targetPort = "";
            int tvSec = 0;
            int tvUsec = 0;
            int forwarderTvSec = 0;
            int forwarderTvUsec = 0;
            string input = "";
            byte[] tmp;
            char check = 'n';
            object[] parameters;


            while(true)
            {
                routingManager.ShowRoutingTable();
                Console.WriteLine("");

                Console.Write("mode (self:s other:o)                          > ");
                input = Console.ReadLine();
                input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                mode = input[0];
                if(mode == 's')   // self
                {
                    Console.Write("client listen ip                               > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientListenIp = Encoding.UTF8.GetString(tmp);

                    if((String.Compare(clientListenIp, spiderIp.SpiderIpv4) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6Global) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                    {
                        Console.WriteLine("[-] please input spider ipv4 or ipv6");
                        continue;
                    }

                    if(String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                    {
                        clientListenIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                    }

                    Console.Write("client listen port                             > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientListenPort = Encoding.UTF8.GetString(tmp);

                    Console.Write("client destination spider ip                   > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientDestinationSpiderIp = Encoding.UTF8.GetString(tmp);

                    Console.Write("target ip (ipv4<16, domainname<256, ipv6<46)   > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    targetIp = Encoding.UTF8.GetString(tmp);

                    if(targetIp.Length >= 256)
                    {
                        Console.WriteLine("[-] input error");
                        continue;
                    }

                    Console.Write("target port                                    > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    targetPort = Encoding.UTF8.GetString(tmp);

                    Console.Write("recv/send tv_sec  (timeout 0-60 sec)           > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        tvSec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        tvSec = 3;
                    }

                    if(tvSec < 0 || tvSec > 60)
                    {
                        tvSec = 3;
                    }

                    Console.Write("recv/send tv_usec (timeout 0-1000000 microsec) > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        tvUsec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        tvUsec = 3;
                    }

                    if(tvUsec < 0 || tvUsec > 1000000)
                    {
                        tvUsec = 0;
                    }

                    if(tvSec == 0 && tvUsec == 0){
                        tvSec = 3;
                        tvUsec = 0;
                    }

                    Console.Write("forwarder tv_sec  (timeout 0-3600 sec)         > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        forwarderTvSec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        forwarderTvSec = 30;
                    }

                    if(forwarderTvSec < 0 || forwarderTvSec > 3600)
                    {
                        forwarderTvSec = 30;
                    }

                    Console.Write("forwarder tv_usec (timeout 0-1000000 microsec) > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        forwarderTvUsec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvUsec < 0 || forwarderTvUsec > 1000000)
                    {
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvSec == 0 && forwarderTvUsec == 0)
                    {
                        forwarderTvSec = 30;
                        forwarderTvUsec = 0;
                    }

                    Console.WriteLine("");
                    Console.WriteLine("client listen ip             : {0}", clientListenIp);
                    if(!string.IsNullOrEmpty(clientListenIpScopeId))
                    {
                        Console.WriteLine("client listen ip scope id    : {0}", clientListenIpScopeId);
                    }
                    Console.WriteLine("client listen port           : {0}", clientListenPort);
                    Console.WriteLine("client destination spider ip : {0}", clientDestinationSpiderIp);
                    Console.WriteLine("target ip                    : {0}", targetIp);
                    Console.WriteLine("target port                  : {0}", targetPort);
                    Console.WriteLine("recv/send tv_sec             : {0,7} sec", tvSec);
                    Console.WriteLine("recv/send tv_usec            : {0,7} microsec", tvUsec);
                    Console.WriteLine("forwarder_tv_sec             : {0,7} sec", forwarderTvSec);
                    Console.WriteLine("forwarder_tv_usec            : {0,7} microsec", forwarderTvUsec);
                    Console.WriteLine("FORWARDER_UDP_TIMEOUT        : {0,7} sec", FORWARDER_UDP_TIMEOUT);
                    Console.WriteLine("");

                    Console.Write("ok? (yes:y no:n quit:q)                        > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    check = input[0];
                    if(check == 'y')
                    {
                        parameters = new object[] {clientListenIp,
                                                   clientListenIpScopeId,
                                                   clientListenPort,
                                                   clientDestinationSpiderIp,
                                                   targetIp,
                                                   targetPort,
                                                   tvSec,
                                                   tvUsec,
                                                   forwarderTvSec,
                                                   forwarderTvUsec};

                        Thread thread = new Thread(new ParameterizedThreadStart(ClientUdpWorker));
                        thread.Start(parameters);

                        break;
                    }else if(check == 'n')
                    {
                        continue;
                    }else if(check == 'q')
                    {
                        return;
                    }else
                    {
                        return;
                    }
                }else if(mode == 'o')   // other
                {
                    Console.Write("source spider ip                               > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    sourceSpiderIp = Encoding.UTF8.GetString(tmp);

                    if((String.Compare(sourceSpiderIp, spiderIp.SpiderIpv4) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6Global) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                    {
                        Console.WriteLine("[-] please input spider ipv4 or ipv6");
                        continue;
                    }

                    if(String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                    {
                        sourceSpiderIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                    }

                    Console.Write("destination spider ip                          > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    destinationSpiderIp = Encoding.UTF8.GetString(tmp);

                    Console.Write("client listen ip                               > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientListenIp = Encoding.UTF8.GetString(tmp);

                    Console.Write("client listen port                             > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientListenPort = Encoding.UTF8.GetString(tmp);

                    Console.Write("client destination spider ip                   > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientDestinationSpiderIp = Encoding.UTF8.GetString(tmp);

                    Console.Write("target ip (ipv4<16, domainname<256, ipv6<46)   > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    targetIp = Encoding.UTF8.GetString(tmp);

                    if(targetIp.Length >= 256)
                    {
                        Console.WriteLine("[-] input error");
                        continue;
                    }

                    Console.Write("target port                                    > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    targetPort = Encoding.UTF8.GetString(tmp);

                    Console.Write("recv/send tv_sec  (timeout 0-60 sec)           > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        tvSec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        tvSec = 3;
                    }

                    if(tvSec < 0 || tvSec > 60)
                    {
                        tvSec = 3;
                    }

                    Console.Write("recv/send tv_usec (timeout 0-1000000 microsec) > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        tvUsec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        tvUsec = 3;
                    }

                    if(tvUsec < 0 || tvUsec > 1000000)
                    {
                        tvUsec = 0;
                    }

                    if(tvSec == 0 && tvUsec == 0){
                        tvSec = 3;
                        tvUsec = 0;
                    }

                    Console.Write("forwarder tv_sec  (timeout 0-3600 sec)         > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        forwarderTvSec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        forwarderTvSec = 30;
                    }

                    if(forwarderTvSec < 0 || forwarderTvSec > 3600)
                    {
                        forwarderTvSec = 30;
                    }

                    Console.Write("forwarder tv_usec (timeout 0-1000000 microsec) > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        forwarderTvUsec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvUsec < 0 || forwarderTvUsec > 1000000)
                    {
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvSec == 0 && forwarderTvUsec == 0)
                    {
                        forwarderTvSec = 30;
                        forwarderTvUsec = 0;
                    }

                    Console.WriteLine("");
                    Console.WriteLine("source spider ip             : {0}", sourceSpiderIp);
                    if(!string.IsNullOrEmpty(sourceSpiderIpScopeId))
                    {
                        Console.WriteLine("source spider ip scope id    : {0} ({1})", sourceSpiderIpScopeId, if_nametoindex(sourceSpiderIpScopeId));
                    }
                    Console.WriteLine("destination spider ip        : {0}", destinationSpiderIp);
                    Console.WriteLine("client listen ip             : {0}", clientListenIp);
                    Console.WriteLine("client listen port           : {0}", clientListenPort);
                    Console.WriteLine("client destination spider ip : {0}", clientDestinationSpiderIp);
                    Console.WriteLine("target ip                    : {0}", targetIp);
                    Console.WriteLine("target port                  : {0}", targetPort);
                    Console.WriteLine("recv/send tv_sec             : {0,7} sec", tvSec);
                    Console.WriteLine("recv/send tv_usec            : {0,7} microsec", tvUsec);
                    Console.WriteLine("forwarder_tv_sec             : {0,7} sec", forwarderTvSec);
                    Console.WriteLine("forwarder_tv_usec            : {0,7} microsec", forwarderTvUsec);
                    Console.WriteLine("FORWARDER_UDP_TIMEOUT        : {0,7} sec", FORWARDER_UDP_TIMEOUT);
                    Console.WriteLine("");

                    Console.Write("ok? (yes:y no:n quit:q)                        > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    check = input[0];
                    if(check == 'y')
                    {
                        config = "";
                        config += "[client_udp]\n";

                        config += "client_listen_ip:";
                        config += clientListenIp;
                        config += "\n";

                        config += "client_listen_port:";
                        config += clientListenPort;
                        config += "\n";

                        config += "client_destination_spider_ip:";
                        config += clientDestinationSpiderIp;
                        config += "\n";

                        config += "target_ip:";
                        config += targetIp;
                        config += "\n";

                        config += "target_port:";
                        config += targetPort;
                        config += "\n";

                        config += "tv_sec:";
                        config += tvSec.ToString();
                        config += "\n";

                        config += "tv_usec:";
                        config += tvUsec.ToString();
                        config += "\n";

                        config += "forwarder_tv_sec:";
                        config += forwarderTvSec.ToString();
                        config += "\n";

                        config += "forwarder_tv_usec:";
                        config += forwarderTvUsec.ToString();
                        config += "\n";

                        parameters = new object[] {config,
                                                   sourceSpiderIp,
                                                   sourceSpiderIpScopeId,
                                                   destinationSpiderIp};

                        Thread thread = new Thread(new ParameterizedThreadStart(AddNodeToDestinationSpiderWorker));
                        thread.Start(parameters);

                        break;
                    }else if(check == 'n')
                    {
                        continue;
                    }else if(check == 'q')
                    {
                        return;
                    }else
                    {
                        return;
                    }
                }else
                {
                    return;
                }
            }

            return;
        }

        private void ClientShellWorker(object obj)
        {
            int ret = 0;
            Client client = obj as Client;


            if(client != null)
            {
                int timeout = client.TvSec * 1000 + client.TvUsec / 1000;
                client.TcpClient.ReceiveTimeout = timeout;
                client.TcpClient.SendTimeout = timeout;

                ret = client.DoSocks5ConnectionShell();
                if(ret == -1)
                {
                    if(clientManager.EraseClient(client.ConnectionId,
                                                 client.ClientId))
                    {
                        if(client.TcpClient != null)
                        {
                            client.TcpClient.Dispose();
                        }
                    }
                }
            }

            return;
        }

        private int ListenClientShell(string clientListenIp,
                                      string clientListenIpScopeId,
                                      string clientListenPort,
                                      string destinationSpiderIp,
                                      int tvSec,
                                      int tvUsec,
                                      int forwarderTvSec,
                                      int forwarderTvUsec)
        {
            int ret = 0;
            string type = "shell";
            uint connectionId = 0;
            TcpListener clientTcpListener = null;
            IPAddress clientListenerIpAddress = null;
            string clientListenIpTmp = "";
            TcpClient clientTcpClient = null;
            Client clientListen;
            string ipv6LinkLocalPrefix = "fe80:";


            try
            {
                if(clientListenIp.Contains(":") &&  // ipv6 link local
                   clientListenIp.StartsWith(ipv6LinkLocalPrefix, StringComparison.OrdinalIgnoreCase))
                {
                    clientListenIpTmp = clientListenIp + "%" + clientListenIpScopeId;
                    clientListenerIpAddress = IPAddress.Parse(clientListenIpTmp);

                    clientTcpListener = new TcpListener(clientListenerIpAddress,
                                                        int.Parse(clientListenPort));

                    clientTcpListener.Start();

                    Console.WriteLine("[+] listening port {0} on {1}",
                                      clientListenPort,
                                      clientListenIpTmp);

                    clientListen = new Client(type,
                                              0,
                                              0,
                                              0,
                                              clientListenIp,
                                              clientListenIpScopeId,
                                              clientListenPort,
                                              "",
                                              destinationSpiderIp,
                                              clientTcpListener,
                                              null,
                                              tvSec,
                                              tvUsec,
                                              forwarderTvSec,
                                              forwarderTvUsec,
                                              encryption,
                                              messageManager);

                    do
                    {
                        connectionId = GenerateRandomId();
                        ret = clientManager.InsertClient(connectionId,
                                                         0,
                                                         clientListen);
                    }while(ret != 0);

                    while(true)
                    {
                        clientTcpClient = clientTcpListener.AcceptTcpClient();

                        uint clientId = 0;
                        IPEndPoint clientRemoteEndPoint = clientTcpClient.Client.RemoteEndPoint as IPEndPoint;
                        IPAddress clientClientIpAddress = null;
                        int port;
                        long scopeId;
                        string clientIp = "";
                        string clientIpScopeId = "";
                        string clientPort = "";
                        int percentIndex = -1;

                        if(clientRemoteEndPoint != null)
                        {
                            clientClientIpAddress = clientRemoteEndPoint.Address;
                            port = clientRemoteEndPoint.Port;
                            scopeId = clientClientIpAddress.ScopeId;

                            clientIp = clientClientIpAddress.ToString();
                            percentIndex = clientIp.IndexOf('%');
                            if(percentIndex != -1)
                            {
                                clientIp = clientIp.Substring(0, percentIndex);
                            }
                            clientIpScopeId = scopeId.ToString();
                            clientPort = port.ToString();

#if DEBUGPRINT
                            Console.WriteLine("[+] connected from ip:{0}%{1} port:{2}",
                                              clientIp,
                                              clientIpScopeId,
                                              clientPort);
#endif
                            Client client = new Client(type,
                                                       connectionId,
                                                       0,
                                                       0,
                                                       clientIp,
                                                       clientIpScopeId,
                                                       "",
                                                       clientPort,
                                                       destinationSpiderIp,
                                                       null,
                                                       clientTcpClient,
                                                       tvSec,
                                                       tvUsec,
                                                       forwarderTvSec,
                                                       forwarderTvUsec,
                                                       encryption,
                                                       messageManager);

                            do
                            {
                                clientId = GenerateRandomId();
                                ret = clientManager.InsertClient(connectionId,
                                                                 clientId,
                                                                 client);
                            }while(ret != 0);

                            Thread thread = new Thread(new ParameterizedThreadStart(ClientShellWorker));
                            thread.Start(client);
                        }else
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] clientRemoteEndPoint is null");
#endif
                            clientTcpClient.Dispose();
                            continue;
                        }
                    }
                }else
                {
                    clientListenerIpAddress = IPAddress.Parse(clientListenIp);

                    clientTcpListener = new TcpListener(clientListenerIpAddress,
                                                        int.Parse(clientListenPort));

                    clientTcpListener.Start();

                    Console.WriteLine("[+] listening port {0} on {1}",
                                      clientListenPort,
                                      clientListenIp);

                    clientListen = new Client(type,
                                              0,
                                              0,
                                              0,
                                              clientListenIp,
                                              "",
                                              clientListenPort,
                                              "",
                                              destinationSpiderIp,
                                              clientTcpListener,
                                              null,
                                              tvSec,
                                              tvUsec,
                                              forwarderTvSec,
                                              forwarderTvUsec,
                                              encryption,
                                              messageManager);

                    do
                    {
                        connectionId = GenerateRandomId();
                        ret = clientManager.InsertClient(connectionId,
                                                         0,
                                                         clientListen);
                    }while(ret != 0);

                    while(true)
                    {
                        clientTcpClient = clientTcpListener.AcceptTcpClient();

                        uint clientId = 0;
                        IPEndPoint clientRemoteEndPoint = clientTcpClient.Client.RemoteEndPoint as IPEndPoint;
                        IPAddress clientClientIpAddress = null;
                        int port;
                        string clientIp = "";
                        string clientPort = "";

                        if(clientRemoteEndPoint != null)
                        {
                            clientClientIpAddress = clientRemoteEndPoint.Address;
                            port = clientRemoteEndPoint.Port;

                            clientIp = clientClientIpAddress.ToString();
                            clientPort = port.ToString();

#if DEBUGPRINT
                            Console.WriteLine("[+] connected from ip:{0} port:{1}",
                                              clientIp,
                                              clientPort);
#endif
                            Client client = new Client(type,
                                                       connectionId,
                                                       0,
                                                       0,
                                                       clientIp,
                                                       "",
                                                       "",
                                                       clientPort,
                                                       destinationSpiderIp,
                                                       null,
                                                       clientTcpClient,
                                                       tvSec,
                                                       tvUsec,
                                                       forwarderTvSec,
                                                       forwarderTvUsec,
                                                       encryption,
                                                       messageManager);

                            do
                            {
                                clientId = GenerateRandomId();
                                ret = clientManager.InsertClient(connectionId,
                                                                 clientId,
                                                                 client);
                            }while(ret != 0);

                            Thread thread = new Thread(new ParameterizedThreadStart(ClientShellWorker));
                            thread.Start(client);
                        }else
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] clientRemoteEndPoint is null");
#endif
                            clientTcpClient.Dispose();
                            continue;
                        }
                    }
                }
            }catch(Exception ex)
            {
                Console.WriteLine("[-] ListenClient error: {0}",
                                  ex.Message);
            }

            clientManager.EraseClient(connectionId,
                                      0);

            if(clientTcpListener != null)
            {
                clientTcpListener.Stop();
            }

            return 0;
        }

        private void ListenClientShell(object obj)
        {
            object[] parameters = obj as object[];

            string clientListenIp = parameters[0] as string;
            string clientListenIpScopeId = parameters[1] as string;
            string clientListenPort = parameters[2] as string;
            string destinationSpiderIp = parameters[3] as string;
            int tvSec = (int)parameters[4];
            int tvUsec = (int)parameters[5];
            int forwarderTvSec = (int)parameters[6];
            int forwarderTvUsec = (int)parameters[7];


            if(clientListenIp != null &&
                clientListenIpScopeId != null &&
                clientListenPort != null &&
                destinationSpiderIp != null)
            {
                int ret = ListenClientShell(clientListenIp,
                                            clientListenIpScopeId,
                                            clientListenPort,
                                            destinationSpiderIp,
                                            tvSec,
                                            tvUsec,
                                            forwarderTvSec,
                                            forwarderTvUsec);
            }

            return;
        }

        public void AddNodeSpiderClientShell()
        {
            string config = "";
            char mode;  // self:s other:o
            string sourceSpiderIp = "";
            string sourceSpiderIpScopeId = "";
            string destinationSpiderIp = "";
            string clientListenIp = "";
            string clientListenIpScopeId = "";
            string clientListenPort = "";
            string clientDestinationSpiderIp = "";
            int tvSec = 0;
            int tvUsec = 0;
            int forwarderTvSec = 0;
            int forwarderTvUsec = 0;
            string input = "";
            byte[] tmp;
            char check = 'n';
            object[] parameters;


            while(true)
            {
                routingManager.ShowRoutingTable();
                Console.WriteLine("");

                Console.Write("mode (self:s other:o)                          > ");
                input = Console.ReadLine();
                input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                mode = input[0];
                if(mode == 's')   // self
                {
                    Console.Write("client listen ip                               > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientListenIp = Encoding.UTF8.GetString(tmp);

                    if((String.Compare(clientListenIp, spiderIp.SpiderIpv4) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6Global) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                    {
                        Console.WriteLine("[-] please input spider ipv4 or ipv6");
                        continue;
                    }

                    if(String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                    {
                        clientListenIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                    }

                    Console.Write("client listen port                             > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientListenPort = Encoding.UTF8.GetString(tmp);

                    Console.Write("client destination spider ip                   > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientDestinationSpiderIp = Encoding.UTF8.GetString(tmp);

                    Console.Write("recv/send tv_sec  (timeout 0-60 sec)           > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        tvSec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        tvSec = 3;
                    }

                    if(tvSec < 0 || tvSec > 60)
                    {
                        tvSec = 3;
                    }

                    Console.Write("recv/send tv_usec (timeout 0-1000000 microsec) > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        tvUsec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        tvUsec = 3;
                    }

                    if(tvUsec < 0 || tvUsec > 1000000)
                    {
                        tvUsec = 0;
                    }

                    if(tvSec == 0 && tvUsec == 0){
                        tvSec = 3;
                        tvUsec = 0;
                    }

                    Console.Write("forwarder tv_sec  (timeout 0-3600 sec)         > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        forwarderTvSec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        forwarderTvSec = 300;
                    }

                    if(forwarderTvSec < 0 || forwarderTvSec > 3600)
                    {
                        forwarderTvSec = 300;
                    }

                    Console.Write("forwarder tv_usec (timeout 0-1000000 microsec) > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        forwarderTvUsec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvUsec < 0 || forwarderTvUsec > 1000000)
                    {
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvSec == 0 && forwarderTvUsec == 0)
                    {
                        forwarderTvSec = 300;
                        forwarderTvUsec = 0;
                    }

                    Console.WriteLine("");
                    Console.WriteLine("client listen ip             : {0}", clientListenIp);
                    if(!string.IsNullOrEmpty(clientListenIpScopeId))
                    {
                        Console.WriteLine("client listen ip scope id    : {0}", clientListenIpScopeId);
                    }
                    Console.WriteLine("client listen port           : {0}", clientListenPort);
                    Console.WriteLine("client destination spider ip : {0}", clientDestinationSpiderIp);
                    Console.WriteLine("recv/send tv_sec             : {0,7} sec", tvSec);
                    Console.WriteLine("recv/send tv_usec            : {0,7} microsec", tvUsec);
                    Console.WriteLine("forwarder_tv_sec             : {0,7} sec", forwarderTvSec);
                    Console.WriteLine("forwarder_tv_usec            : {0,7} microsec", forwarderTvUsec);
                    Console.WriteLine("");

                    Console.Write("ok? (yes:y no:n quit:q)                        > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    check = input[0];
                    if(check == 'y')
                    {
                        parameters = new object[] {clientListenIp,
                                                   clientListenIpScopeId,
                                                   clientListenPort,
                                                   clientDestinationSpiderIp,
                                                   tvSec,
                                                   tvUsec,
                                                   forwarderTvSec,
                                                   forwarderTvUsec};

                        Thread thread = new Thread(new ParameterizedThreadStart(ListenClientShell));
                        thread.Start(parameters);

                        Thread.Sleep(2000); // 2s

                        break;
                    }else if(check == 'n')
                    {
                        continue;
                    }else if(check == 'q')
                    {
                        return;
                    }else
                    {
                        return;
                    }
                }else if(mode == 'o')   // other
                {
                    Console.Write("source spider ip                               > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    sourceSpiderIp = Encoding.UTF8.GetString(tmp);

                    if((String.Compare(sourceSpiderIp, spiderIp.SpiderIpv4) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6Global) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                    {
                        Console.WriteLine("[-] please input spider ipv4 or ipv6");
                        continue;
                    }

                    if(String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                    {
                        sourceSpiderIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                    }

                    Console.Write("destination spider ip                          > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    destinationSpiderIp = Encoding.UTF8.GetString(tmp);

                    Console.Write("client listen ip                               > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientListenIp = Encoding.UTF8.GetString(tmp);

                    Console.Write("client listen port                             > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientListenPort = Encoding.UTF8.GetString(tmp);

                    Console.Write("client destination spider ip                   > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    clientDestinationSpiderIp = Encoding.UTF8.GetString(tmp);

                    Console.Write("recv/send tv_sec  (timeout 0-60 sec)           > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        tvSec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        tvSec = 3;
                    }

                    if(tvSec < 0 || tvSec > 60)
                    {
                        tvSec = 3;
                    }

                    Console.Write("recv/send tv_usec (timeout 0-1000000 microsec) > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        tvUsec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        tvUsec = 3;
                    }

                    if(tvUsec < 0 || tvUsec > 1000000)
                    {
                        tvUsec = 0;
                    }

                    if(tvSec == 0 && tvUsec == 0){
                        tvSec = 3;
                        tvUsec = 0;
                    }

                    Console.Write("forwarder tv_sec  (timeout 0-3600 sec)         > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        forwarderTvSec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        forwarderTvSec = 300;
                    }

                    if(forwarderTvSec < 0 || forwarderTvSec > 3600)
                    {
                        forwarderTvSec = 300;
                    }

                    Console.Write("forwarder tv_usec (timeout 0-1000000 microsec) > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    try
                    {
                        forwarderTvUsec = int.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvUsec < 0 || forwarderTvUsec > 1000000)
                    {
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvSec == 0 && forwarderTvUsec == 0)
                    {
                        forwarderTvSec = 300;
                        forwarderTvUsec = 0;
                    }

                    Console.WriteLine("");
                    Console.WriteLine("source spider ip             : {0}", sourceSpiderIp);
                    if(!string.IsNullOrEmpty(sourceSpiderIpScopeId))
                    {
                        Console.WriteLine("source spider ip scope id    : {0} ({1})", sourceSpiderIpScopeId, if_nametoindex(sourceSpiderIpScopeId));
                    }
                    Console.WriteLine("destination spider ip        : {0}", destinationSpiderIp);
                    Console.WriteLine("client listen ip             : {0}", clientListenIp);
                    Console.WriteLine("client listen port           : {0}", clientListenPort);
                    Console.WriteLine("client destination spider ip : {0}", clientDestinationSpiderIp);
                    Console.WriteLine("recv/send tv_sec             : {0,7} sec", tvSec);
                    Console.WriteLine("recv/send tv_usec            : {0,7} microsec", tvUsec);
                    Console.WriteLine("forwarder_tv_sec             : {0,7} sec", forwarderTvSec);
                    Console.WriteLine("forwarder_tv_usec            : {0,7} microsec", forwarderTvUsec);
                    Console.WriteLine("");

                    Console.Write("ok? (yes:y no:n quit:q)                        > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    check = input[0];
                    if(check == 'y')
                    {
                        config = "";
                        config += "[client_shell]\n";

                        config += "client_listen_ip:";
                        config += clientListenIp;
                        config += "\n";

                        config += "client_listen_port:";
                        config += clientListenPort;
                        config += "\n";

                        config += "client_destination_spider_ip:";
                        config += clientDestinationSpiderIp;
                        config += "\n";

                        config += "tv_sec:";
                        config += tvSec.ToString();
                        config += "\n";

                        config += "tv_usec:";
                        config += tvUsec.ToString();
                        config += "\n";

                        config += "forwarder_tv_sec:";
                        config += forwarderTvSec.ToString();
                        config += "\n";

                        config += "forwarder_tv_usec:";
                        config += forwarderTvUsec.ToString();
                        config += "\n";

                        parameters = new object[] {config,
                                                   sourceSpiderIp,
                                                   sourceSpiderIpScopeId,
                                                   destinationSpiderIp};

                        Thread thread = new Thread(new ParameterizedThreadStart(AddNodeToDestinationSpiderWorker));
                        thread.Start(parameters);

                        break;
                    }else if(check == 'n')
                    {
                        continue;
                    }else if(check == 'q')
                    {
                        return;
                    }else
                    {
                        return;
                    }
                }else
                {
                    return;
                }
            }

            return;
        }

        private void AddNodeToDestinationSpiderWorker(string config,
                                                      string sourceSpiderIp,
                                                      string sourceSpiderIpScopeId,
                                                      string destinationSpiderIp)
        {
            int ret = 0;
            uint connectionId = 0;
            uint clientId = 0;
            Client client;


            client = new Client("add",
                                0,
                                0,
                                0,
                                sourceSpiderIp,
                                sourceSpiderIpScopeId,
                                "",
                                "",
                                destinationSpiderIp,
                                null,
                                null,
                                ADD_NODE_TO_DESTINATION_SPIDER_WORKER_TV_SEC,
                                ADD_NODE_TO_DESTINATION_SPIDER_WORKER_TV_USEC,
                                ADD_NODE_TO_DESTINATION_SPIDER_WORKER_FORWARDER_TV_SEC,
                                ADD_NODE_TO_DESTINATION_SPIDER_WORKER_FORWARDER_TV_USEC,
                                encryption,
                                messageManager);

            do
            {
                connectionId = GenerateRandomId();
                clientId = GenerateRandomId();
                ret = clientManager.InsertClient(connectionId,
                                                 clientId,
                                                 client);
            }while(ret != 0);

            ret = client.DoSocks5ConnectionAddNode(config);
            if(ret == -1)
            {
                clientManager.EraseClient(client.ConnectionId,
                                          client.ClientId);
            }

            return;
        }

        private void AddNodeToDestinationSpiderWorker(object obj)
        {
            object[] parameters = obj as object[];

            string config = parameters[0] as string;
            string sourceSpiderIp = parameters[1] as string;
            string sourceSpiderIpScopeId = parameters[2] as string;
            string destinationSpiderIp = parameters[3] as string;

            if(config != null &&
               sourceSpiderIp != null &&
               sourceSpiderIpScopeId != null &&
               destinationSpiderIp != null)
            {
                AddNodeToDestinationSpiderWorker(config,
                                                 sourceSpiderIp,
                                                 sourceSpiderIpScopeId,
                                                 destinationSpiderIp);
            }

            return;
        }

        private string getLine(IEnumerator<string> lineEnumerator)
        {
            string line = null;
            byte[] tmp;

            while(lineEnumerator.MoveNext())
            {
                line = lineEnumerator.Current;
                line = new string(line.Where(c => !char.IsWhiteSpace(c)).ToArray());
                tmp = Encoding.UTF8.GetBytes(line.Trim());
                line = Encoding.UTF8.GetString(tmp);

                if(line.StartsWith("//", StringComparison.OrdinalIgnoreCase) ||
                   line.StartsWith("\r\n", StringComparison.OrdinalIgnoreCase) ||
                   line.StartsWith("\n", StringComparison.OrdinalIgnoreCase) ||
                   line.StartsWith("\r", StringComparison.OrdinalIgnoreCase))
                {
                    continue;
                }else
                {
                    return line;
                }
            }

            return null;
        }

        private IEnumerator<string> ReadLinesFromFile(string configFile)
        {
            using(StreamReader reader = new StreamReader(configFile))
            {
                string line;
                while((line = reader.ReadLine()) != null)
                {
                    yield return line;
                }
            }
        }

        public int ReadConfig(string config)
        {
            string[] lines;
            string line;
            int index = 0;

            lines = config.Split(new[] {"\r\n", "\r", "\n"}, StringSplitOptions.RemoveEmptyEntries);
            if(lines.Length == 0)
            {
                return -1;
            }

            try
            {
                line = lines[index];
                index++;
                if(String.Compare(lines[0], "[client]") == 0)
                {
                    string clientListenIp = "";
                    string clientListenIpScopeId = "";
                    string clientListenPort = "";
                    string clientDestinationSpiderIp = "";
                    string tvSecString = "";
                    string tvUsecString = "";
                    string forwarderTvSecString = "";
                    string forwarderTvUsecString = "";
                    int tvSec = 0;
                    int tvUsec = 0;
                    int forwarderTvSec = 0;
                    int forwarderTvUsec = 0;
                    object[] parameters;


                    // client_listen_ip
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("client_listen_ip:", StringComparison.Ordinal))
                    {
                        clientListenIp = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(clientListenIp))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client] [client_listen_ip] error");
#endif
                        return -1;
                    }

                    if((String.Compare(clientListenIp, spiderIp.SpiderIpv4) != 0) &&
                        (String.Compare(clientListenIp, spiderIp.SpiderIpv6Global) != 0) &&
                        (String.Compare(clientListenIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                        (String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client] [client_listen_ip] please input spider ipv4 or ipv6: {0}",
                                          clientListenIp);
#endif
                        return -1;
                    }

                    if(String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                    {
                        clientListenIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                    }


                    // client_listen_port
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("client_listen_port:", StringComparison.Ordinal))
                    {
                        clientListenPort = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(clientListenPort))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client] [client_listen_port] error");
#endif
                        return -1;
                    }


                    // client_destination_spider_ip
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("client_destination_spider_ip:", StringComparison.Ordinal))
                    {
                        clientDestinationSpiderIp = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(clientDestinationSpiderIp))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client] [client_destination_spider_ip] error");
#endif
                        return -1;
                    }


                    // tv_sec
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("tv_sec:", StringComparison.Ordinal))
                    {
                        tvSecString = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(tvSecString))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client] [tv_sec] error");
#endif
                        return -1;
                    }


                    // tv_usec
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("tv_usec:", StringComparison.Ordinal))
                    {
                        tvUsecString = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(tvUsecString))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client] [tv_usec] error");
#endif
                        return -1;
                    }


                    // forwarder_tv_sec
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("forwarder_tv_sec:", StringComparison.Ordinal))
                    {
                        forwarderTvSecString = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(forwarderTvSecString))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client] [forwarder_tv_sec] error");
#endif
                        return -1;
                    }


                    // forwarder_tv_usec
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("forwarder_tv_usec:", StringComparison.Ordinal))
                    {
                        forwarderTvUsecString = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(forwarderTvUsecString))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client] [forwarder_tv_usec] error");
#endif
                        return -1;
                    }


                    tvSec = int.Parse(tvSecString);
                    tvUsec = int.Parse(tvUsecString);
                    forwarderTvSec = int.Parse(forwarderTvSecString);
                    forwarderTvUsec = int.Parse(forwarderTvUsecString);

                    if(tvSec < 0 || tvSec > 60)
                    {
                        tvSec = 3;
                    }

                    if(tvUsec < 0 || tvUsec > 1000000)
                    {
                        tvUsec = 0;
                    }

                    if(tvSec == 0 && tvUsec == 0){
                        tvSec = 3;
                        tvUsec = 0;
                    }

                    if(forwarderTvSec < 0 || forwarderTvSec > 3600)
                    {
                        forwarderTvSec = 30;
                    }

                    if(forwarderTvUsec < 0 || forwarderTvUsec > 1000000)
                    {
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvSec == 0 && forwarderTvUsec == 0)
                    {
                        forwarderTvSec = 30;
                        forwarderTvUsec = 0;
                    }


                    parameters = new object[] {clientListenIp,
                                               clientListenIpScopeId,
                                               clientListenPort,
                                               clientDestinationSpiderIp,
                                               tvSec,
                                               tvUsec,
                                               forwarderTvSec,
                                               forwarderTvUsec};

                    Thread thread = new Thread(new ParameterizedThreadStart(ListenClient));
                    thread.Start(parameters);

                }else if(String.Compare(line, "[pipe_client]") == 0)
                {
                    char mode = 'c';
                    string pipeIp = "";
                    string pipeIpScopeId = "";
                    string pipeDestinationIp = "";
                    string pipeDestinationPort = "";
                    object[] parameters;


                    // pipe_ip
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [pipe_client] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("pipe_ip:", StringComparison.Ordinal))
                    {
                        pipeIp = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(pipeIp))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [pipe_client] [pipe_ip] error");
#endif
                        return -1;
                    }

                    if((String.Compare(pipeIp, spiderIp.SpiderIpv4) != 0) &&
                       (String.Compare(pipeIp, spiderIp.SpiderIpv6Global) != 0) &&
                       (String.Compare(pipeIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                       (String.Compare(pipeIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [pipe_client] [pipe_ip] please input spider ipv4 or ipv6: {0}",
                                          pipeIp);
#endif
                        return -1;
                    }

                    if(String.Compare(pipeIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                    {
                        pipeIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                    }


                    // pipe_destination_ip
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [pipe_client] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("pipe_destination_ip:"))
                    {
                        pipeDestinationIp = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(pipeDestinationIp))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [pipe_client] [pipe_destination_ip] error");
#endif
                        return -1;
                    }


                    // pipe_destination_port
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [pipe_client] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("pipe_destination_port:"))
                    {
                        pipeDestinationPort = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(pipeDestinationPort))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [pipe_client] [pipe_destination_port] error");
#endif
                        return -1;
                    }


                    parameters = new object[] {mode,
                                               pipeIp,
                                               pipeIpScopeId,
                                               pipeDestinationIp,
                                               pipeDestinationPort};

                    Thread thread = new Thread(new ParameterizedThreadStart(ConnectPipe));
                    thread.Start(parameters);

                }else if(String.Compare(line, "[pipe_server]") == 0)
                {
                    char mode = 's';
                    string pipeListenIp = "";
                    string pipeListenIpScopeId = "";
                    string pipeListenPort = "";
                    object[] parameters;


                    // pipe_listen_ip
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [pipe_server] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("pipe_listen_ip:"))
                    {
                        pipeListenIp = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(pipeListenIp))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [pipe_server] [pipe_listen_ip] error");
#endif
                        return -1;
                    }

                    if((String.Compare(pipeListenIp, spiderIp.SpiderIpv4) != 0) &&
                       (String.Compare(pipeListenIp, spiderIp.SpiderIpv6Global) != 0) &&
                       (String.Compare(pipeListenIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                       (String.Compare(pipeListenIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [pipe_server] [pipe_listen_ip] please input spider ipv4 or ipv6: {0}",
                                          pipeListenIp);
#endif
                        return -1;
                    }

                    if(String.Compare(pipeListenIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                    {
                        pipeListenIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                    }


                    // pipe_listen_port
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [pipe_server] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("pipe_listen_port:"))
                    {
                        pipeListenPort = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(pipeListenPort))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [pipe_server] [pipe_listen_port] error");
#endif
                        return -1;
                    }


                    parameters = new object[] {mode,
                                               pipeListenIp,
                                               pipeListenIpScopeId,
                                               pipeListenPort};

                    Thread thread = new Thread(new ParameterizedThreadStart(ListenPipe));
                    thread.Start(parameters);

                }else if(String.Compare(line, "[client_tcp]") == 0)
                {
                    string clientListenIp = "";
                    string clientListenIpScopeId = "";
                    string clientListenPort = "";
                    string clientDestinationSpiderIp = "";
                    string targetIp = "";
                    string targetPort = "";
                    string tvSecString = "";
                    string tvUsecString = "";
                    string forwarderTvSecString = "";
                    string forwarderTvUsecString = "";
                    int tvSec = 0;
                    int tvUsec = 0;
                    int forwarderTvSec = 0;
                    int forwarderTvUsec = 0;
                    object[] parameters;


                    // client_listen_ip
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("client_listen_ip:", StringComparison.Ordinal))
                    {
                        clientListenIp = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(clientListenIp))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] [client_listen_ip] error");
#endif
                        return -1;
                    }

                    if((String.Compare(clientListenIp, spiderIp.SpiderIpv4) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6Global) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] [client_listen_ip] please input spider ipv4 or ipv6: {0}",
                                          clientListenIp);
#endif
                        return -1;
                    }

                    if(String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                    {
                        clientListenIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                    }


                    // client_listen_port
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("client_listen_port:", StringComparison.Ordinal))
                    {
                        clientListenPort = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(clientListenPort))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] [client_listen_port] error");
#endif
                        return -1;
                    }


                    // client_destination_spider_ip
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("client_destination_spider_ip:", StringComparison.Ordinal))
                    {
                        clientDestinationSpiderIp = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(clientDestinationSpiderIp))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] [client_destination_spider_ip] error");
#endif
                        return -1;
                    }


                    // target_ip
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("target_ip:"))
                    {
                        targetIp = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(targetIp))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] [target_ip] error");
#endif
                        return -1;
                    }

                    if(targetIp.Length >= 256)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] [target_ip] size error");
#endif
                        return -1;
                    }


                    // target_port
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("target_port:"))
                    {
                        targetPort = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(targetPort))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] [target_port] error");
#endif
                        return -1;
                    }


                    // tv_sec
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("tv_sec:", StringComparison.Ordinal))
                    {
                        tvSecString = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(tvSecString))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] [tv_sec] error");
#endif
                        return -1;
                    }


                    // tv_usec
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("tv_usec:", StringComparison.Ordinal))
                    {
                        tvUsecString = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(tvUsecString))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] [tv_usec] error");
#endif
                        return -1;
                    }


                    // forwarder_tv_sec
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("forwarder_tv_sec:", StringComparison.Ordinal))
                    {
                        forwarderTvSecString = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(forwarderTvSecString))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] [forwarder_tv_sec] error");
#endif
                        return -1;
                    }


                    // forwarder_tv_usec
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("forwarder_tv_usec:", StringComparison.Ordinal))
                    {
                        forwarderTvUsecString = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(forwarderTvUsecString))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_tcp] [forwarder_tv_usec] error");
#endif
                        return -1;
                    }


                    tvSec = int.Parse(tvSecString);
                    tvUsec = int.Parse(tvUsecString);
                    forwarderTvSec = int.Parse(forwarderTvSecString);
                    forwarderTvUsec = int.Parse(forwarderTvUsecString);

                    if(tvSec < 0 || tvSec > 60)
                    {
                        tvSec = 3;
                    }

                    if(tvUsec < 0 || tvUsec > 1000000)
                    {
                        tvUsec = 0;
                    }

                    if(tvSec == 0 && tvUsec == 0){
                        tvSec = 3;
                        tvUsec = 0;
                    }

                    if(forwarderTvSec < 0 || forwarderTvSec > 3600)
                    {
                        forwarderTvSec = 30;
                    }

                    if(forwarderTvUsec < 0 || forwarderTvUsec > 1000000)
                    {
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvSec == 0 && forwarderTvUsec == 0)
                    {
                        forwarderTvSec = 30;
                        forwarderTvUsec = 0;
                    }


                    parameters = new object[] {clientListenIp,
                                               clientListenIpScopeId,
                                               clientListenPort,
                                               clientDestinationSpiderIp,
                                               targetIp,
                                               targetPort,
                                               tvSec,
                                               tvUsec,
                                               forwarderTvSec,
                                               forwarderTvUsec};

                    Thread thread = new Thread(new ParameterizedThreadStart(ListenClientTcp));
                    thread.Start(parameters);

                }else if(String.Compare(line, "[client_udp]") == 0)
                {
                    string clientListenIp = "";
                    string clientListenIpScopeId = "";
                    string clientListenPort = "";
                    string clientDestinationSpiderIp = "";
                    string targetIp = "";
                    string targetPort = "";
                    string tvSecString = "";
                    string tvUsecString = "";
                    string forwarderTvSecString = "";
                    string forwarderTvUsecString = "";
                    int tvSec = 0;
                    int tvUsec = 0;
                    int forwarderTvSec = 0;
                    int forwarderTvUsec = 0;
                    object[] parameters;


                    // client_listen_ip
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("client_listen_ip:", StringComparison.Ordinal))
                    {
                        clientListenIp = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(clientListenIp))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] [client_listen_ip] error");
#endif
                        return -1;
                    }

                    if((String.Compare(clientListenIp, spiderIp.SpiderIpv4) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6Global) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] [client_listen_ip] please input spider ipv4 or ipv6: {0}",
                                          clientListenIp);
#endif
                        return -1;
                    }

                    if(String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                    {
                        clientListenIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                    }


                    // client_listen_port
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("client_listen_port:", StringComparison.Ordinal))
                    {
                        clientListenPort = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(clientListenPort))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] [client_listen_port] error");
#endif
                        return -1;
                    }


                    // client_destination_spider_ip
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("client_destination_spider_ip:", StringComparison.Ordinal))
                    {
                        clientDestinationSpiderIp = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(clientDestinationSpiderIp))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] [client_destination_spider_ip] error");
#endif
                        return -1;
                    }


                    // target_ip
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("target_ip:"))
                    {
                        targetIp = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(targetIp))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] [target_ip] error");
#endif
                        return -1;
                    }

                    if(targetIp.Length >= 256)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] [target_ip] size error");
#endif
                        return -1;
                    }


                    // target_port
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("target_port:"))
                    {
                        targetPort = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(targetPort))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] [target_port] error");
#endif
                        return -1;
                    }


                    // tv_sec
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("tv_sec:", StringComparison.Ordinal))
                    {
                        tvSecString = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(tvSecString))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] [tv_sec] error");
#endif
                        return -1;
                    }


                    // tv_usec
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("tv_usec:", StringComparison.Ordinal))
                    {
                        tvUsecString = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(tvUsecString))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] [tv_usec] error");
#endif
                        return -1;
                    }


                    // forwarder_tv_sec
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("forwarder_tv_sec:", StringComparison.Ordinal))
                    {
                        forwarderTvSecString = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(forwarderTvSecString))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] [forwarder_tv_sec] error");
#endif
                        return -1;
                    }


                    // forwarder_tv_usec
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("forwarder_tv_usec:", StringComparison.Ordinal))
                    {
                        forwarderTvUsecString = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(forwarderTvUsecString))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_udp] [forwarder_tv_usec] error");
#endif
                        return -1;
                    }


                    tvSec = int.Parse(tvSecString);
                    tvUsec = int.Parse(tvUsecString);
                    forwarderTvSec = int.Parse(forwarderTvSecString);
                    forwarderTvUsec = int.Parse(forwarderTvUsecString);

                    if(tvSec < 0 || tvSec > 60)
                    {
                        tvSec = 3;
                    }

                    if(tvUsec < 0 || tvUsec > 1000000)
                    {
                        tvUsec = 0;
                    }

                    if(tvSec == 0 && tvUsec == 0){
                        tvSec = 3;
                        tvUsec = 0;
                    }

                    if(forwarderTvSec < 0 || forwarderTvSec > 3600)
                    {
                        forwarderTvSec = 30;
                    }

                    if(forwarderTvUsec < 0 || forwarderTvUsec > 1000000)
                    {
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvSec == 0 && forwarderTvUsec == 0)
                    {
                        forwarderTvSec = 30;
                        forwarderTvUsec = 0;
                    }


                    parameters = new object[] {clientListenIp,
                                               clientListenIpScopeId,
                                               clientListenPort,
                                               clientDestinationSpiderIp,
                                               targetIp,
                                               targetPort,
                                               tvSec,
                                               tvUsec,
                                               forwarderTvSec,
                                               forwarderTvUsec};

                    Thread thread = new Thread(new ParameterizedThreadStart(ClientUdpWorker));
                    thread.Start(parameters);

                }else if(String.Compare(line, "[client_shell]") == 0)
                {
                    string clientListenIp = "";
                    string clientListenIpScopeId = "";
                    string clientListenPort = "";
                    string clientDestinationSpiderIp = "";
                    string tvSecString = "";
                    string tvUsecString = "";
                    string forwarderTvSecString = "";
                    string forwarderTvUsecString = "";
                    int tvSec = 0;
                    int tvUsec = 0;
                    int forwarderTvSec = 0;
                    int forwarderTvUsec = 0;
                    object[] parameters;


                    // client_listen_ip
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_shell] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("client_listen_ip:", StringComparison.Ordinal))
                    {
                        clientListenIp = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(clientListenIp))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_shell] [client_listen_ip] error");
#endif
                        return -1;
                    }

                    if((String.Compare(clientListenIp, spiderIp.SpiderIpv4) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6Global) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                       (String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_shell] [client_listen_ip] please input spider ipv4 or ipv6: {0}",
                                          clientListenIp);
#endif
                        return -1;
                    }

                    if(String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                    {
                        clientListenIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                    }


                    // client_listen_port
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_shell] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("client_listen_port:", StringComparison.Ordinal))
                    {
                        clientListenPort = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(clientListenPort))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_shell] [client_listen_port] error");
#endif
                        return -1;
                    }


                    // client_destination_spider_ip
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_shell] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("client_destination_spider_ip:", StringComparison.Ordinal))
                    {
                        clientDestinationSpiderIp = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(clientDestinationSpiderIp))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_shell] [client_destination_spider_ip] error");
#endif
                        return -1;
                    }


                    // tv_sec
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_shell] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("tv_sec:", StringComparison.Ordinal))
                    {
                        tvSecString = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(tvSecString))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_shell] [tv_sec] error");
#endif
                        return -1;
                    }


                    // tv_usec
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_shell] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("tv_usec:", StringComparison.Ordinal))
                    {
                        tvUsecString = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(tvUsecString))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_shell] [tv_usec] error");
#endif
                        return -1;
                    }


                    // forwarder_tv_sec
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_shell] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("forwarder_tv_sec:", StringComparison.Ordinal))
                    {
                        forwarderTvSecString = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(forwarderTvSecString))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_shell] [forwarder_tv_sec] error");
#endif
                        return -1;
                    }


                    // forwarder_tv_usec
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_shell] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("forwarder_tv_usec:", StringComparison.Ordinal))
                    {
                        forwarderTvUsecString = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(forwarderTvUsecString))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client_shell] [forwarder_tv_usec] error");
#endif
                        return -1;
                    }


                    tvSec = int.Parse(tvSecString);
                    tvUsec = int.Parse(tvUsecString);
                    forwarderTvSec = int.Parse(forwarderTvSecString);
                    forwarderTvUsec = int.Parse(forwarderTvUsecString);

                    if(tvSec < 0 || tvSec > 60)
                    {
                        tvSec = 3;
                    }

                    if(tvUsec < 0 || tvUsec > 1000000)
                    {
                        tvUsec = 0;
                    }

                    if(tvSec == 0 && tvUsec == 0){
                        tvSec = 3;
                        tvUsec = 0;
                    }

                    if(forwarderTvSec < 0 || forwarderTvSec > 3600)
                    {
                        forwarderTvSec = 30;
                    }

                    if(forwarderTvUsec < 0 || forwarderTvUsec > 1000000)
                    {
                        forwarderTvUsec = 0;
                    }

                    if(forwarderTvSec == 0 && forwarderTvUsec == 0)
                    {
                        forwarderTvSec = 30;
                        forwarderTvUsec = 0;
                    }


                    parameters = new object[] {clientListenIp,
                                               clientListenIpScopeId,
                                               clientListenPort,
                                               clientDestinationSpiderIp,
                                               tvSec,
                                               tvUsec,
                                               forwarderTvSec,
                                               forwarderTvUsec};

                    Thread thread = new Thread(new ParameterizedThreadStart(ListenClientShell));
                    thread.Start(parameters);

                }
            }catch(Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] read config error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            return 0;
        }

        public int ReadConfigFile(string configFile)
        {
            string line = "";
            IEnumerator<string> lineEnumerator;


            if(!File.Exists(configFile))
            {
                Console.WriteLine("[-] cannot find config file");
                return -1;
            }

            lineEnumerator = ReadLinesFromFile(configFile);

            while(true)
            {
                try
                {
                    line = getLine(lineEnumerator);
                    if(line == null)
                    {
                        break;
                    }

                    if(String.Compare(line, "[client]") == 0)
                    {
                        string clientListenIp = "";
                        string clientListenIpScopeId = "";
                        string clientListenPort = "";
                        string clientDestinationSpiderIp = "";
                        string tvSecString = "";
                        string tvUsecString = "";
                        string forwarderTvSecString = "";
                        string forwarderTvUsecString = "";
                        int tvSec = 0;
                        int tvUsec = 0;
                        int forwarderTvSec = 0;
                        int forwarderTvUsec = 0;
                        object[] parameters;


                        // client_listen_ip
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client] error");
                            break;
                        }

                        if(line.StartsWith("client_listen_ip:", StringComparison.Ordinal))
                        {
                            clientListenIp = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(clientListenIp))
                        {
                            Console.WriteLine("[-] [client] [client_listen_ip] error");
                            break;
                        }

                        if((String.Compare(clientListenIp, spiderIp.SpiderIpv4) != 0) &&
                           (String.Compare(clientListenIp, spiderIp.SpiderIpv6Global) != 0) &&
                           (String.Compare(clientListenIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                           (String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                        {
                            Console.WriteLine("[-] [client] [client_listen_ip] please input spider ipv4 or ipv6: {0}",
                                              clientListenIp);
                            break;
                        }

                        if(String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                        {
                            clientListenIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                        }


                        // client_listen_port
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client] error");
                            break;
                        }

                        if(line.StartsWith("client_listen_port:", StringComparison.Ordinal))
                        {
                            clientListenPort = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(clientListenPort))
                        {
                            Console.WriteLine("[-] [client] [client_listen_port] error");
                            break;
                        }


                        // client_destination_spider_ip
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client] error");
                            break;
                        }

                        if(line.StartsWith("client_destination_spider_ip:", StringComparison.Ordinal))
                        {
                            clientDestinationSpiderIp = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(clientDestinationSpiderIp))
                        {
                            Console.WriteLine("[-] [client] [client_destination_spider_ip] error");
                            break;
                        }


                        // tv_sec
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client] error");
                            break;
                        }

                        if(line.StartsWith("tv_sec:", StringComparison.Ordinal))
                        {
                            tvSecString = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(tvSecString))
                        {
                            Console.WriteLine("[-] [client] [tv_sec] error");
                            break;
                        }


                        // tv_usec
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client] error");
                            break;
                        }

                        if(line.StartsWith("tv_usec:", StringComparison.Ordinal))
                        {
                            tvUsecString = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(tvUsecString))
                        {
                            Console.WriteLine("[-] [client] [tv_usec] error");
                            break;
                        }


                        // forwarder_tv_sec
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client] error");
                            break;
                        }

                        if(line.StartsWith("forwarder_tv_sec:", StringComparison.Ordinal))
                        {
                            forwarderTvSecString = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(forwarderTvSecString))
                        {
                            Console.WriteLine("[-] [client] [forwarder_tv_sec] error");
                            break;
                        }


                        // forwarder_tv_usec
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client] error");
                            break;
                        }

                        if(line.StartsWith("forwarder_tv_usec:", StringComparison.Ordinal))
                        {
                            forwarderTvUsecString = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(forwarderTvUsecString))
                        {
                            Console.WriteLine("[-] [client] [forwarder_tv_usec] error");
                            break;
                        }


                        tvSec = int.Parse(tvSecString);
                        tvUsec = int.Parse(tvUsecString);
                        forwarderTvSec = int.Parse(forwarderTvSecString);
                        forwarderTvUsec = int.Parse(forwarderTvUsecString);

                        if(tvSec < 0 || tvSec > 60)
                        {
                            tvSec = 3;
                        }

                        if(tvUsec < 0 || tvUsec > 1000000)
                        {
                            tvUsec = 0;
                        }

                        if(tvSec == 0 && tvUsec == 0){
                            tvSec = 3;
                            tvUsec = 0;
                        }

                        if(forwarderTvSec < 0 || forwarderTvSec > 3600)
                        {
                            forwarderTvSec = 30;
                        }

                        if(forwarderTvUsec < 0 || forwarderTvUsec > 1000000)
                        {
                            forwarderTvUsec = 0;
                        }

                        if(forwarderTvSec == 0 && forwarderTvUsec == 0)
                        {
                            forwarderTvSec = 30;
                            forwarderTvUsec = 0;
                        }


                        parameters = new object[] {clientListenIp,
                                                   clientListenIpScopeId,
                                                   clientListenPort,
                                                   clientDestinationSpiderIp,
                                                   tvSec,
                                                   tvUsec,
                                                   forwarderTvSec,
                                                   forwarderTvUsec};

                        Thread thread = new Thread(new ParameterizedThreadStart(ListenClient));
                        thread.Start(parameters);

                    }else if(String.Compare(line, "[pipe_client]") == 0)
                    {
                        char mode = 'c';
                        string pipeIp = "";
                        string pipeIpScopeId = "";
                        string pipeDestinationIp = "";
                        string pipeDestinationPort = "";
                        object[] parameters;


                        // pipe_ip
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [pipe_client] error");
                            break;
                        }

                        if(line.StartsWith("pipe_ip:", StringComparison.Ordinal))
                        {
                            pipeIp = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(pipeIp))
                        {
                            Console.WriteLine("[-] [pipe_client] [pipe_ip] error");
                            break;
                        }

                        if((String.Compare(pipeIp, spiderIp.SpiderIpv4) != 0) &&
                           (String.Compare(pipeIp, spiderIp.SpiderIpv6Global) != 0) &&
                           (String.Compare(pipeIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                           (String.Compare(pipeIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                        {
                            Console.WriteLine("[-] [pipe_client] [pipe_ip] please input spider ipv4 or ipv6: {0}",
                                              pipeIp);
                            break;
                        }

                        if(String.Compare(pipeIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                        {
                            pipeIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                        }


                        // pipe_destination_ip
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [pipe_client] error");
                            break;
                        }

                        if(line.StartsWith("pipe_destination_ip:"))
                        {
                            pipeDestinationIp = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(pipeDestinationIp))
                        {
                            Console.WriteLine("[-] [pipe_client] [pipe_destination_ip] error");
                            break;
                        }


                        // pipe_destination_port
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [pipe_client] error");
                            break;
                        }

                        if(line.StartsWith("pipe_destination_port:"))
                        {
                            pipeDestinationPort = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(pipeDestinationPort))
                        {
                            Console.WriteLine("[-] [pipe_client] [pipe_destination_port] error");
                            break;
                        }


                        parameters = new object[] {mode,
                                                   pipeIp,
                                                   pipeIpScopeId,
                                                   pipeDestinationIp,
                                                   pipeDestinationPort};

                        Thread thread = new Thread(new ParameterizedThreadStart(ConnectPipe));
                        thread.Start(parameters);

                    }else if(String.Compare(line, "[pipe_server]") == 0)
                    {
                        char mode = 's';
                        string pipeListenIp = "";
                        string pipeListenIpScopeId = "";
                        string pipeListenPort = "";
                        object[] parameters;


                        // pipe_listen_ip
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [pipe_server] error");
                            break;
                        }

                        if(line.StartsWith("pipe_listen_ip:"))
                        {
                            pipeListenIp = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(pipeListenIp))
                        {
                            Console.WriteLine("[-] [pipe_server] [pipe_listen_ip] error");
                            break;
                        }

                        if((String.Compare(pipeListenIp, spiderIp.SpiderIpv4) != 0) &&
                           (String.Compare(pipeListenIp, spiderIp.SpiderIpv6Global) != 0) &&
                           (String.Compare(pipeListenIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                           (String.Compare(pipeListenIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                        {
                            Console.WriteLine("[-] [pipe_server] [pipe_listen_ip] please input spider ipv4 or ipv6: {0}",
                                              pipeListenIp);
                            break;
                        }

                        if(String.Compare(pipeListenIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                        {
                            pipeListenIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                        }


                        // pipe_listen_port
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [pipe_server] error");
                            break;
                        }

                        if(line.StartsWith("pipe_listen_port:"))
                        {
                            pipeListenPort = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(pipeListenPort))
                        {
                            Console.WriteLine("[-] [pipe_server] [pipe_listen_port] error");
                            break;
                        }


                        parameters = new object[] {mode,
                                                   pipeListenIp,
                                                   pipeListenIpScopeId,
                                                   pipeListenPort};

                        Thread thread = new Thread(new ParameterizedThreadStart(ListenPipe));
                        thread.Start(parameters);

                    }else if(String.Compare(line, "[client_tcp]") == 0)
                    {
                        string clientListenIp = "";
                        string clientListenIpScopeId = "";
                        string clientListenPort = "";
                        string clientDestinationSpiderIp = "";
                        string targetIp = "";
                        string targetPort = "";
                        string tvSecString = "";
                        string tvUsecString = "";
                        string forwarderTvSecString = "";
                        string forwarderTvUsecString = "";
                        int tvSec = 0;
                        int tvUsec = 0;
                        int forwarderTvSec = 0;
                        int forwarderTvUsec = 0;
                        object[] parameters;


                        // client_listen_ip
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_tcp] error");
                            break;
                        }

                        if(line.StartsWith("client_listen_ip:", StringComparison.Ordinal))
                        {
                            clientListenIp = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(clientListenIp))
                        {
                            Console.WriteLine("[-] [client_tcp] [client_listen_ip] error");
                            break;
                        }

                        if((String.Compare(clientListenIp, spiderIp.SpiderIpv4) != 0) &&
                           (String.Compare(clientListenIp, spiderIp.SpiderIpv6Global) != 0) &&
                           (String.Compare(clientListenIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                           (String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                        {
                            Console.WriteLine("[-] [client_tcp] [client_listen_ip] please input spider ipv4 or ipv6: {0}",
                                              clientListenIp);
                            break;
                        }

                        if(String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                        {
                            clientListenIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                        }


                        // client_listen_port
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_tcp] error");
                            break;
                        }

                        if(line.StartsWith("client_listen_port:", StringComparison.Ordinal))
                        {
                            clientListenPort = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(clientListenPort))
                        {
                            Console.WriteLine("[-] [client_tcp] [client_listen_port] error");
                            break;
                        }


                        // client_destination_spider_ip
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_tcp] error");
                            break;
                        }

                        if(line.StartsWith("client_destination_spider_ip:", StringComparison.Ordinal))
                        {
                            clientDestinationSpiderIp = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(clientDestinationSpiderIp))
                        {
                            Console.WriteLine("[-] [client_tcp] [client_destination_spider_ip] error");
                            break;
                        }


                        // target_ip
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_tcp] error");
                            break;
                        }

                        if(line.StartsWith("target_ip:"))
                        {
                            targetIp = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(targetIp))
                        {
                            Console.WriteLine("[-] [client_tcp] [target_ip] error");
                            break;
                        }

                        if(targetIp.Length >= 256)
                        {
                            Console.WriteLine("[-] [client_tcp] [target_ip] size error");
                            break;
                        }


                        // target_port
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_tcp] error");
                            break;
                        }

                        if(line.StartsWith("target_port:"))
                        {
                            targetPort = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(targetPort))
                        {
                            Console.WriteLine("[-] [client_tcp] [target_port] error");
                            break;
                        }


                        // tv_sec
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_tcp] error");
                            break;
                        }

                        if(line.StartsWith("tv_sec:", StringComparison.Ordinal))
                        {
                            tvSecString = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(tvSecString))
                        {
                            Console.WriteLine("[-] [client_tcp] [tv_sec] error");
                            break;
                        }


                        // tv_usec
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_tcp] error");
                            break;
                        }

                        if(line.StartsWith("tv_usec:", StringComparison.Ordinal))
                        {
                            tvUsecString = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(tvUsecString))
                        {
                            Console.WriteLine("[-] [client_tcp] [tv_usec] error");
                            break;
                        }


                        // forwarder_tv_sec
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_tcp] error");
                            break;
                        }

                        if(line.StartsWith("forwarder_tv_sec:", StringComparison.Ordinal))
                        {
                            forwarderTvSecString = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(forwarderTvSecString))
                        {
                            Console.WriteLine("[-] [client_tcp] [forwarder_tv_sec] error");
                            break;
                        }


                        // forwarder_tv_usec
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_tcp] error");
                            break;
                        }

                        if(line.StartsWith("forwarder_tv_usec:", StringComparison.Ordinal))
                        {
                            forwarderTvUsecString = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(forwarderTvUsecString))
                        {
                            Console.WriteLine("[-] [client_tcp] [forwarder_tv_usec] error");
                            break;
                        }


                        tvSec = int.Parse(tvSecString);
                        tvUsec = int.Parse(tvUsecString);
                        forwarderTvSec = int.Parse(forwarderTvSecString);
                        forwarderTvUsec = int.Parse(forwarderTvUsecString);

                        if(tvSec < 0 || tvSec > 60)
                        {
                            tvSec = 3;
                        }

                        if(tvUsec < 0 || tvUsec > 1000000)
                        {
                            tvUsec = 0;
                        }

                        if(tvSec == 0 && tvUsec == 0){
                            tvSec = 3;
                            tvUsec = 0;
                        }

                        if(forwarderTvSec < 0 || forwarderTvSec > 3600)
                        {
                            forwarderTvSec = 30;
                        }

                        if(forwarderTvUsec < 0 || forwarderTvUsec > 1000000)
                        {
                            forwarderTvUsec = 0;
                        }

                        if(forwarderTvSec == 0 && forwarderTvUsec == 0)
                        {
                            forwarderTvSec = 30;
                            forwarderTvUsec = 0;
                        }


                        parameters = new object[] {clientListenIp,
                                                   clientListenIpScopeId,
                                                   clientListenPort,
                                                   clientDestinationSpiderIp,
                                                   targetIp,
                                                   targetPort,
                                                   tvSec,
                                                   tvUsec,
                                                   forwarderTvSec,
                                                   forwarderTvUsec};

                        Thread thread = new Thread(new ParameterizedThreadStart(ListenClientTcp));
                        thread.Start(parameters);

                    }else if(String.Compare(line, "[client_udp]") == 0)
                    {
                        string clientListenIp = "";
                        string clientListenIpScopeId = "";
                        string clientListenPort = "";
                        string clientDestinationSpiderIp = "";
                        string targetIp = "";
                        string targetPort = "";
                        string tvSecString = "";
                        string tvUsecString = "";
                        string forwarderTvSecString = "";
                        string forwarderTvUsecString = "";
                        int tvSec = 0;
                        int tvUsec = 0;
                        int forwarderTvSec = 0;
                        int forwarderTvUsec = 0;
                        object[] parameters;


                        // client_listen_ip
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_udp] error");
                            break;
                        }

                        if(line.StartsWith("client_listen_ip:", StringComparison.Ordinal))
                        {
                            clientListenIp = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(clientListenIp))
                        {
                            Console.WriteLine("[-] [client_udp] [client_listen_ip] error");
                            break;
                        }

                        if((String.Compare(clientListenIp, spiderIp.SpiderIpv4) != 0) &&
                           (String.Compare(clientListenIp, spiderIp.SpiderIpv6Global) != 0) &&
                           (String.Compare(clientListenIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                           (String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                        {
                            Console.WriteLine("[-] [client_udp] [client_listen_ip] please input spider ipv4 or ipv6: {0}",
                                              clientListenIp);
                            break;
                        }

                        if(String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                        {
                            clientListenIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                        }


                        // client_listen_port
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_udp] error");
                            break;
                        }

                        if(line.StartsWith("client_listen_port:", StringComparison.Ordinal))
                        {
                            clientListenPort = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(clientListenPort))
                        {
                            Console.WriteLine("[-] [client_udp] [client_listen_port] error");
                            break;
                        }


                        // client_destination_spider_ip
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_udp] error");
                            break;
                        }

                        if(line.StartsWith("client_destination_spider_ip:", StringComparison.Ordinal))
                        {
                            clientDestinationSpiderIp = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(clientDestinationSpiderIp))
                        {
                            Console.WriteLine("[-] [client_udp] [client_destination_spider_ip] error");
                            break;
                        }


                        // target_ip
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_udp] error");
                            break;
                        }

                        if(line.StartsWith("target_ip:"))
                        {
                            targetIp = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(targetIp))
                        {
                            Console.WriteLine("[-] [client_udp] [target_ip] error");
                            break;
                        }

                        if(targetIp.Length >= 256)
                        {
                            Console.WriteLine("[-] [client_udp] [target_ip] size error");
                            break;
                        }


                        // target_port
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_udp] error");
                            break;
                        }

                        if(line.StartsWith("target_port:"))
                        {
                            targetPort = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(targetPort))
                        {
                            Console.WriteLine("[-] [client_udp] [target_port] error");
                            break;
                        }


                        // tv_sec
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_udp] error");
                            break;
                        }

                        if(line.StartsWith("tv_sec:", StringComparison.Ordinal))
                        {
                            tvSecString = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(tvSecString))
                        {
                            Console.WriteLine("[-] [client_udp] [tv_sec] error");
                            break;
                        }


                        // tv_usec
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_udp] error");
                            break;
                        }

                        if(line.StartsWith("tv_usec:", StringComparison.Ordinal))
                        {
                            tvUsecString = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(tvUsecString))
                        {
                            Console.WriteLine("[-] [client_udp] [tv_usec] error");
                            break;
                        }


                        // forwarder_tv_sec
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_udp] error");
                            break;
                        }

                        if(line.StartsWith("forwarder_tv_sec:", StringComparison.Ordinal))
                        {
                            forwarderTvSecString = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(forwarderTvSecString))
                        {
                            Console.WriteLine("[-] [client_udp] [forwarder_tv_sec] error");
                            break;
                        }


                        // forwarder_tv_usec
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_udp] error");
                            break;
                        }

                        if(line.StartsWith("forwarder_tv_usec:", StringComparison.Ordinal))
                        {
                            forwarderTvUsecString = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(forwarderTvUsecString))
                        {
                            Console.WriteLine("[-] [client_udp] [forwarder_tv_usec] error");
                            break;
                        }


                        tvSec = int.Parse(tvSecString);
                        tvUsec = int.Parse(tvUsecString);
                        forwarderTvSec = int.Parse(forwarderTvSecString);
                        forwarderTvUsec = int.Parse(forwarderTvUsecString);

                        if(tvSec < 0 || tvSec > 60)
                        {
                            tvSec = 3;
                        }

                        if(tvUsec < 0 || tvUsec > 1000000)
                        {
                            tvUsec = 0;
                        }

                        if(tvSec == 0 && tvUsec == 0){
                            tvSec = 3;
                            tvUsec = 0;
                        }

                        if(forwarderTvSec < 0 || forwarderTvSec > 3600)
                        {
                            forwarderTvSec = 30;
                        }

                        if(forwarderTvUsec < 0 || forwarderTvUsec > 1000000)
                        {
                            forwarderTvUsec = 0;
                        }

                        if(forwarderTvSec == 0 && forwarderTvUsec == 0)
                        {
                            forwarderTvSec = 30;
                            forwarderTvUsec = 0;
                        }


                        parameters = new object[] {clientListenIp,
                                                   clientListenIpScopeId,
                                                   clientListenPort,
                                                   clientDestinationSpiderIp,
                                                   targetIp,
                                                   targetPort,
                                                   tvSec,
                                                   tvUsec,
                                                   forwarderTvSec,
                                                   forwarderTvUsec};

                        Thread thread = new Thread(new ParameterizedThreadStart(ClientUdpWorker));
                        thread.Start(parameters);

                    }else if(String.Compare(line, "[client_shell]") == 0)
                    {
                        string clientListenIp = "";
                        string clientListenIpScopeId = "";
                        string clientListenPort = "";
                        string clientDestinationSpiderIp = "";
                        string tvSecString = "";
                        string tvUsecString = "";
                        string forwarderTvSecString = "";
                        string forwarderTvUsecString = "";
                        int tvSec = 0;
                        int tvUsec = 0;
                        int forwarderTvSec = 0;
                        int forwarderTvUsec = 0;
                        object[] parameters;


                        // client_listen_ip
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_shell] error");
                            break;
                        }

                        if(line.StartsWith("client_listen_ip:", StringComparison.Ordinal))
                        {
                            clientListenIp = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(clientListenIp))
                        {
                            Console.WriteLine("[-] [client_shell] [client_listen_ip] error");
                            break;
                        }

                        if((String.Compare(clientListenIp, spiderIp.SpiderIpv4) != 0) &&
                           (String.Compare(clientListenIp, spiderIp.SpiderIpv6Global) != 0) &&
                           (String.Compare(clientListenIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                           (String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                        {
                            Console.WriteLine("[-] [client_shell] [client_listen_ip] please input spider ipv4 or ipv6: {0}",
                                              clientListenIp);
                            break;
                        }

                        if(String.Compare(clientListenIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                        {
                            clientListenIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                        }


                        // client_listen_port
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_shell] error");
                            break;
                        }

                        if(line.StartsWith("client_listen_port:", StringComparison.Ordinal))
                        {
                            clientListenPort = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(clientListenPort))
                        {
                            Console.WriteLine("[-] [client_shell] [client_listen_port] error");
                            break;
                        }


                        // client_destination_spider_ip
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_shell] error");
                            break;
                        }

                        if(line.StartsWith("client_destination_spider_ip:", StringComparison.Ordinal))
                        {
                            clientDestinationSpiderIp = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(clientDestinationSpiderIp))
                        {
                            Console.WriteLine("[-] [client_shell] [client_destination_spider_ip] error");
                            break;
                        }


                        // tv_sec
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_shell] error");
                            break;
                        }

                        if(line.StartsWith("tv_sec:", StringComparison.Ordinal))
                        {
                            tvSecString = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(tvSecString))
                        {
                            Console.WriteLine("[-] [client_shell] [tv_sec] error");
                            break;
                        }


                        // tv_usec
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_shell] error");
                            break;
                        }

                        if(line.StartsWith("tv_usec:", StringComparison.Ordinal))
                        {
                            tvUsecString = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(tvUsecString))
                        {
                            Console.WriteLine("[-] [client_shell] [tv_usec] error");
                            break;
                        }


                        // forwarder_tv_sec
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_shell] error");
                            break;
                        }

                        if(line.StartsWith("forwarder_tv_sec:", StringComparison.Ordinal))
                        {
                            forwarderTvSecString = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(forwarderTvSecString))
                        {
                            Console.WriteLine("[-] [client_shell] [forwarder_tv_sec] error");
                            break;
                        }


                        // forwarder_tv_usec
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_shell] error");
                            break;
                        }

                        if(line.StartsWith("forwarder_tv_usec:", StringComparison.Ordinal))
                        {
                            forwarderTvUsecString = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(forwarderTvUsecString))
                        {
                            Console.WriteLine("[-] [client_shell] [forwarder_tv_usec] error");
                            break;
                        }


                        tvSec = int.Parse(tvSecString);
                        tvUsec = int.Parse(tvUsecString);
                        forwarderTvSec = int.Parse(forwarderTvSecString);
                        forwarderTvUsec = int.Parse(forwarderTvUsecString);

                        if(tvSec < 0 || tvSec > 60)
                        {
                            tvSec = 3;
                        }

                        if(tvUsec < 0 || tvUsec > 1000000)
                        {
                            tvUsec = 0;
                        }

                        if(tvSec == 0 && tvUsec == 0){
                            tvSec = 3;
                            tvUsec = 0;
                        }

                        if(forwarderTvSec < 0 || forwarderTvSec > 3600)
                        {
                            forwarderTvSec = 30;
                        }

                        if(forwarderTvUsec < 0 || forwarderTvUsec > 1000000)
                        {
                            forwarderTvUsec = 0;
                        }

                        if(forwarderTvSec == 0 && forwarderTvUsec == 0)
                        {
                            forwarderTvSec = 30;
                            forwarderTvUsec = 0;
                        }


                        parameters = new object[] {clientListenIp,
                                                   clientListenIpScopeId,
                                                   clientListenPort,
                                                   clientDestinationSpiderIp,
                                                   tvSec,
                                                   tvUsec,
                                                   forwarderTvSec,
                                                   forwarderTvUsec};

                        Thread thread = new Thread(new ParameterizedThreadStart(ListenClientShell));
                        thread.Start(parameters);

                    }else if(String.Compare(line, "[sleep]") == 0)
                    {
                        string sleepString = "";
                        int s;


                        // sleep
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [sleep] error");
                            break;
                        }

                        if(line.StartsWith("sleep:", StringComparison.Ordinal))
                        {
                            sleepString = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(sleepString))
                        {
                            Console.WriteLine("[-] [sleep] [sleep] error");
                            break;
                        }


                        s = int.Parse(sleepString) * 1000;
                        Thread.Sleep(s);

                    }
                }catch(Exception ex)
                {
                    Console.WriteLine("[-] read config file error: {0}",
                                      ex.Message);
                    return -1;
                }
            }

            Thread.Sleep(5000);

            return 0;
        }

        public void CloseClientListenerTcp()
        {
            char mode;  // self:s other:o
//            string sourceSpiderIp = "";
//            string sourceSpiderIpScopeId = "";
//            string destinationSpiderIp = "";
            string input = "";
            byte[] tmp;
            uint connectionId = 0;
            uint clientId = 0;
            Client client = null;
            TcpListener tcpListener = null;
            char check = 'n';
            object[] parameters;


            while(true)
            {
                Console.Write("mode (self:s other:o)                          > ");
                input = Console.ReadLine();
                input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                mode = input[0];
                if(mode == 's')   // self
                {

                    clientManager.ShowClientListenerTcp();

                    Console.Write("connection id                                  > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());

                    try
                    {
                        connectionId = uint.Parse(input);
                    }catch(Exception ex)
                    {
                        Console.WriteLine("[-] input error: {0}",
                                          ex.Message);
                        continue;
                    }

                    Console.WriteLine("");
                    Console.WriteLine("connection id             : {0}", connectionId);
                    Console.WriteLine("");

                    Console.Write("ok? (yes:y no:n quit:q)                        > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    check = input[0];
                    if(check == 'y')
                    {
                        clientId = 0;
                        client = clientManager.GetClient(connectionId,
                                                         clientId);
                        if(client != null)
                        {
                            tcpListener = client.TcpListener;
                            if(tcpListener == null)
                            {
                                Console.WriteLine("[-] close client listener error");
                                return;
                            }

                            tcpListener.Stop();

                            Console.WriteLine("[+] close client listener connection_id: {0,10}",
                                              client.ConnectionId);
                        }else
                        {
                            Console.WriteLine("[-] close client listener error");
                            return;
                        }

                        break;
                    }else if(check == 'n')
                    {
                        continue;
                    }else if(check == 'q'){
                        return;
                    }else{
                        return;
                    }
                }else if(mode == 'o')   // other
                {

                    Console.WriteLine("[-] not implemented");

/*
                    routingManager.ShowRoutingTable();
                    Console.WriteLine("");

                    Console.Write("source spider ip                               > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    sourceSpiderIp = Encoding.UTF8.GetString(tmp);

                    if((String.Compare(sourceSpiderIp, spiderIp.SpiderIpv4) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6Global) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6UniqueLocal) != 0) &&
                       (String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6LinkLocal) != 0))
                    {
                        Console.WriteLine("[-] please input spider ipv4 or ipv6");
                        continue;
                    }

                    if(String.Compare(sourceSpiderIp, spiderIp.SpiderIpv6LinkLocal) == 0)
                    {
                        sourceSpiderIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                    }

                    Console.Write("destination spider ip                          > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    tmp = Encoding.UTF8.GetBytes(input.Trim());
                    destinationSpiderIp = Encoding.UTF8.GetString(tmp);

                    Console.WriteLine("");
                    Console.WriteLine("mode                      : {0}", mode);
                    Console.WriteLine("source spider ip          : {0}", sourceSpiderIp);
                    if(!string.IsNullOrEmpty(sourceSpiderIpScopeId))
                    {
                        Console.WriteLine("source spider ip scope id : {0} ({1})", sourceSpiderIpScopeId, if_nametoindex(sourceSpiderIpScopeId));
                    }
                    Console.WriteLine("destination spider ip     : {0}", destinationSpiderIp);
                    Console.WriteLine("");

                    Console.Write("ok? (yes:y no:n quit:q)                        > ");
                    input = Console.ReadLine();
                    input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                    check = input[0];
                    if(check == 'y')
                    {
                        parameters = new object[] {sourceSpiderIp,
                                                   sourceSpiderIpScopeId,
                                                   destinationSpiderIp};

                        Thread thread = new Thread(new ParameterizedThreadStart(ShowRoutingTableWorker));
                        thread.Start(parameters);

                        Thread.Sleep(20000);    // 20s

                        break;
                    }else if(check == 'n')
                    {
                        continue;
                    }else if(check == 'q'){
                        return;
                    }else{
                        return;
                    }
*/
                }else{
                    return;
                }
            }

            return;
        }
    }
}

