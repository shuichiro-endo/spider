/*
 * Title:  spider MessageManager.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;
using System.Threading;

namespace spider
{
    public class MessageManager
    {
        private SpiderIp spiderIp;
        private ClientManager clientManager;
        private ServerManager serverManager;
        private PipeManager pipeManager;
        private RoutingManager routingManager;
        private RoutingMessageQueue routingMessagesQueue;
        private Socks5MessageQueue socks5MessagesQueue;

        public MessageManager(SpiderIp spiderIp,
                              ClientManager clientManager,
                              ServerManager serverManager,
                              PipeManager pipeManager,
                              RoutingManager routingManager)
        {
            this.spiderIp = spiderIp;
            this.clientManager = clientManager;
            this.serverManager = serverManager;
            this.pipeManager = pipeManager;
            this.routingManager = routingManager;
            this.routingMessagesQueue = new RoutingMessageQueue();
            this.socks5MessagesQueue = new Socks5MessageQueue();
        }

        ~MessageManager()
        {

        }

        public SpiderIp SpiderIp
        {
            get { return spiderIp; }
            set { spiderIp = value; }
        }

        public void PushRoutingMessage(RoutingMessage routingMessage)
        {
            routingMessagesQueue.Push(routingMessage);

            return;
        }

        public void PushRoutingMessage(Object obj)
        {
            RoutingMessage routingMessage = obj as RoutingMessage;


            if(routingMessage != null)
            {
                routingMessagesQueue.Push(routingMessage);
            }

            return;
        }

        public int PushTimeoutRoutingMessage(RoutingMessage routingMessage,
                                             int tvSec,
                                             int tvUsec)
        {
            int ret = 0;

            ret = routingMessagesQueue.PushTimeout(routingMessage,
                                                   tvSec,
                                                   tvUsec);

            return ret;
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

        private RoutingMessage PopRoutingMessage()
        {
            RoutingMessage routingMessage = routingMessagesQueue.Pop();

            return routingMessage;
        }

        private Socks5Message PopSocks5Message()
        {
            Socks5Message socks5Message = socks5MessagesQueue.Pop();

            return socks5Message;
        }

        public void TransferRoutingMessage()
        {
            char messageType;
            RoutingMessage routingMessage;

            routingMessage = PopRoutingMessage();
            if(routingMessage != null)
            {
                messageType = routingMessage.MessageType;
                if(messageType == 'r')  // routing message
                {
                    routingManager.PushRoutingMessage(routingMessage);
                }else
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] unknown message type: {0}",
                                      messageType);
#endif
                }
            }

            return;
        }

        public Socks5Message TransferSocks5Message()
        {
            char messageType;
            Socks5Message socks5Message;
            char destinationNodeType;
            Client client;
            Server server;
            Pipe pipe;
            uint connectionId = 0;
            uint clientId = 0;
            uint serverId = 0;


            socks5Message = PopSocks5Message();
            if(socks5Message != null)
            {
                messageType = socks5Message.MessageType;
                if(messageType == 's')
                {
                    if((String.Compare(spiderIp.SpiderIpv4, socks5Message.DestinationIp) == 0) ||
                       (String.Compare(spiderIp.SpiderIpv6Global, socks5Message.DestinationIp) == 0) ||
                       (String.Compare(spiderIp.SpiderIpv6UniqueLocal, socks5Message.DestinationIp) == 0) ||
                       (String.Compare(spiderIp.SpiderIpv6LinkLocal, socks5Message.DestinationIp) == 0))
                    {
                        connectionId = socks5Message.ConnectionId;
                        clientId = socks5Message.ClientId;
                        serverId = socks5Message.ServerId;
                        destinationNodeType = socks5Message.DestinationNodeType;
                        if(destinationNodeType == 'c')  // client
                        {
                            client = clientManager.GetClient(connectionId,
                                                             clientId);
                            if(client != null)
                            {
                                Thread threadClient = new Thread(new ParameterizedThreadStart(client.PushSocks5Message));
                                threadClient.Start(socks5Message);
                            }else
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] cannot transfer client message");
#endif
                            }
                        }else if(destinationNodeType == 's')    // server
                        {
                            server = serverManager.GetServer(connectionId,
                                                             serverId);
                            if(server != null)
                            {
                                Thread threadServer = new Thread(new ParameterizedThreadStart(server.PushSocks5Message));
                                threadServer.Start(socks5Message);
                            }else
                            {
                                // generate server
                                return socks5Message;
                            }
                        }else
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] cannot transfer socks5 message");
#endif
                        }
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
                            Console.WriteLine("[-] cannot transfer pipe message");
#endif
                        }
                    }
                }else
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] unknown message type: {0}",
                                      messageType);
#endif
                }
            }

            return null;
        }
    }
}

