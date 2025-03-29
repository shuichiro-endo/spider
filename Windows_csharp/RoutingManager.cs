/*
 * Title:  spider RoutingManager.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;

namespace spider
{
    public class RoutingManager
    {
        private const int ROUTING_MESSAGE_DATA_SIZE = 60000;
        private const int INET6_ADDR_STRING_LENGTH = 46;
        private const int KEYS_MAP_SIZE = 200;
        private const int METRIC_MAX = 20;   // 0 < METRIC_MAX <= UINT8_MAX(255), UINT8_MAX(255) < delete route
        private const int DELETE_ROUTE_TIME = 5;    // 5s

        private SpiderIp spiderIp;
        private PipeManager pipeManager;
        private MessageManager messageManager;
        private Dictionary<string, Route> routesMap = new Dictionary<string, Route>();
        private static readonly object routesMapLock = new object();
        private RoutingMessageQueue routingMessagesQueue;


        public RoutingManager(SpiderIp spiderIp,
                              PipeManager pipeManager)
        {
            this.spiderIp = spiderIp;
            this.pipeManager = pipeManager;
            this.routingMessagesQueue = new RoutingMessageQueue();

            this.InitRoutingTable();
        }

        ~RoutingManager()
        {

        }

        public SpiderIp SpiderIp
        {
            get { return spiderIp; }
            set { spiderIp = value; }
        }

        public MessageManager MessageManager
        {
            get { return messageManager; }
            set { messageManager = value; }
        }

        public void InitRoutingTable()
        {
            // self
            if(!string.IsNullOrEmpty(spiderIp.SpiderIpv4))
            {
                Route route = new Route('-',
                                        spiderIp.SpiderIpv4,
                                        0,
                                        0);
                this.AddRoute(route);
            }

            if(!string.IsNullOrEmpty(spiderIp.SpiderIpv6Global))
            {
                Route route = new Route('-',
                                        spiderIp.SpiderIpv6Global,
                                        0,
                                        0);
                this.AddRoute(route);
            }

            if(!string.IsNullOrEmpty(spiderIp.SpiderIpv6UniqueLocal))
            {
                Route route = new Route('-',
                                        spiderIp.SpiderIpv6UniqueLocal,
                                        0,
                                        0);
                this.AddRoute(route);
            }

            if(!string.IsNullOrEmpty(spiderIp.SpiderIpv6LinkLocal))
            {
                Route route = new Route('-',
                                        spiderIp.SpiderIpv6LinkLocal,
                                        0,
                                        0);
                this.AddRoute(route);
            }
        }

        public void ShowRoutingTable()
        {
            Console.WriteLine("---------------------------------------- routing  table ----------------------------------------");
            Console.WriteLine("|mode|ip address                                    |metric|pipe id   |time                    |");
            Console.WriteLine("------------------------------------------------------------------------------------------------");

            lock(routesMapLock)
            {
                foreach(var kvp in routesMap)
                {

                    Console.WriteLine("|{0}   |{1,-46}|   {2,3}|{3,10}|{4,-24}|",
                                      kvp.Value.Mode,
                                      kvp.Value.Ip,
                                      kvp.Value.Metric,
                                      kvp.Value.PipeId,
                                      kvp.Value.Time);
                }
            }

            Console.WriteLine("------------------------------------------------------------------------------------------------");

            return;
        }

        public string ShowRoutingTableString()
        {
            string result = "";

            result += "---------------------------------------- routing  table ----------------------------------------\n";

            result += "|mode|ip address                                    |metric|pipe id   |time                    |\n";

            result += "------------------------------------------------------------------------------------------------\n";

            lock(routesMapLock)
            {
                foreach(var kvp in routesMap)
                {
                    result += string.Format("|{0}   |{1,-46}|   {2,3}|{3,10}|{4,-24}|\n",
                                            kvp.Value.Mode,
                                            kvp.Value.Ip,
                                            kvp.Value.Metric,
                                            kvp.Value.PipeId,
                                            kvp.Value.Time);
                }
            }

            result += "------------------------------------------------------------------------------------------------\n";

            return result;
        }

        public void SendRoutingTable()
        {
            RoutingMessage routingMessage = new RoutingMessage();
            ushort dataSize = 0;
            ushort routeDataSize = (ushort)RouteData.ROUTE_DATA_SIZE;
            byte[] data = routingMessage.Data;
            ushort p = 0;


            lock(routesMapLock)
            {
                foreach(var kvp in routesMap)
                {
                    byte[] ip = Encoding.UTF8.GetBytes(kvp.Value.Ip);
                    for(int i = 0; i < ip.Length; i++)
                    {
                        data[p + i] = ip[i];
                    }
                    p += INET6_ADDR_STRING_LENGTH + 1;

                    data[p] = kvp.Value.Metric;
                    p += 1;

                    if(p + routeDataSize > ROUTING_MESSAGE_DATA_SIZE)
                    {
                        break;
                    }
                }
            }

            dataSize = p;
            routingMessage.PipeId = 0;
            routingMessage.DataSize = dataSize;

#if DEBUGPRINT
//            routingMessage.PringBytes();
#endif
            pipeManager.SendRoutingMessage(routingMessage);

            return;
        }

        public void UpdateRoutingTable()
        {
            int ret = 0;
            char messageType;
            RoutingMessage routingMessage;
            Route routeNew;
            uint pipeId;
            char mode;
            string ip;
            byte[] ipTmp;
            byte metric;
            int dataSize;
            byte[] data;
            ushort p = 0;
            int routeDataSize = RouteData.ROUTE_DATA_SIZE;


            routingMessage = PopRoutingMessage();
            if(routingMessage != null)
            {
                messageType = routingMessage.MessageType;
                if(messageType == 'r')  // routing message
                {
                    pipeId = routingMessage.PipeId;
                    dataSize = routingMessage.DataSize;
                    data = routingMessage.Data;
                    p = 0;

                    for(int i = 0; i + routeDataSize <= (int)dataSize; p += (ushort)routeDataSize, i += routeDataSize)
                    {
                        mode = 'a';
                        ipTmp = new byte[INET6_ADDR_STRING_LENGTH + 1];
                        for(int j = 0; j < INET6_ADDR_STRING_LENGTH + 1; j++)
                        {
                            ipTmp[j] = data[p + j];
                        }
                        ipTmp = ipTmp.Where(b => b != 0x00).ToArray();
                        ip = Encoding.UTF8.GetString(ipTmp);
                        ip = ip.Trim();

                        if((String.Compare(ip, spiderIp.SpiderIpv4) == 0) ||
                           (String.Compare(ip, spiderIp.SpiderIpv6Global) == 0) ||
                           (String.Compare(ip, spiderIp.SpiderIpv6UniqueLocal) == 0) ||
                           (String.Compare(ip, spiderIp.SpiderIpv6LinkLocal) == 0))
                        {
                            continue;
                        }

                        metric = data[p + INET6_ADDR_STRING_LENGTH + 1];
                        if(metric < byte.MaxValue)
                        {
                            metric += 1;    // +1
                        }else
                        {
                            metric = byte.MaxValue;
                        }

                        routeNew = new Route(mode,
                                             ip,
                                             metric,
                                             pipeId);

                        ret = this.UpdateRoute(routeNew);
                    }

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

        public void DeleteRoutingTable()
        {
            DateTime now = DateTime.Now;
            TimeSpan d;
            int count = 0;
            string[] keys = new string[KEYS_MAP_SIZE];


            lock(routesMapLock)
            {
                foreach(var kvp in routesMap)
                {
                    if(kvp.Value.Mode != 's' &&
                       ((String.Compare(kvp.Value.Ip, spiderIp.SpiderIpv4) != 0) ||
                        (String.Compare(kvp.Value.Ip, spiderIp.SpiderIpv6Global) != 0) ||
                        (String.Compare(kvp.Value.Ip, spiderIp.SpiderIpv6UniqueLocal) != 0) ||
                        (String.Compare(kvp.Value.Ip, spiderIp.SpiderIpv6LinkLocal) != 0)) &&
                       kvp.Value.Metric != 0)
                    {
                        DateTime t = kvp.Value.Time;
                        d = now - t;

                        if (d.TotalSeconds >= DELETE_ROUTE_TIME)
                        {
                            if(count < KEYS_MAP_SIZE)
                            {
                                string routeKey = kvp.Value.Ip;
                                keys[count] = routeKey;
                                count++;
                            }
                        }else if(kvp.Value.Metric > METRIC_MAX)
                        {
                            if(count < KEYS_MAP_SIZE)
                            {
                                string routeKey = kvp.Value.Ip;
                                keys[count] = routeKey;
                                count++;
                            }
                        }
                    }
                }

                for(int i = 0; i < count; i++)
                {
                    routesMap.Remove(keys[i]);   // delete route
                }
            }

            return;
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

        public Route GetRoute(string ip)
        {
            Route route = null;
            byte[] tmp = Encoding.UTF8.GetBytes(ip.Trim());
            string routeKey = Encoding.UTF8.GetString(tmp);


            lock(routesMapLock)
            {
                if(routesMap.ContainsKey(routeKey))
                {
                    route = routesMap[routeKey];
                }
            }

            return route;
        }

        public void AddRoute(Route route)
        {
            string routeKey = route.Ip;


            lock(routesMapLock)
            {
                routesMap.Add(routeKey,
                              route);
            }

            return;
        }

        public int UpdateRoute(Route routeNew)
        {
            int ret = -1;
            Route route = null;
            string routeNewKey = routeNew.Ip;


            lock(routesMapLock)
            {
                if(routesMap.ContainsKey(routeNewKey))
                {
                    route = routesMap[routeNewKey];

                    if(routeNew.Metric < route.Metric)
                    {
                        routesMap[routeNewKey] = routeNew;
                        ret = 0;
                    }else if((routeNew.Metric == route.Metric)
                              && (routeNew.PipeId == route.PipeId))
                    {
                        routesMap[routeNewKey] = routeNew;
                        ret = 0;
                    }
                }else
                {
                    routesMap[routeNewKey] = routeNew;
                    ret = 0;
                }
            }

            return ret;
        }

        public void DeleteRoute(string ip)
        {
            byte[] tmp = Encoding.UTF8.GetBytes(ip.Trim());
            string routeKey = Encoding.UTF8.GetString(tmp);

            lock(routesMapLock)
            {
                if(routesMap.ContainsKey(routeKey))
                {
                    routesMap.Remove(routeKey);
                }
            }

            return;
        }

        public Pipe GetDestinationPipe(string ip)
        {
            Route route = null;
            uint pipeId = 0;
            Pipe pipe = null;

            route = GetRoute(ip);
            if(route != null)
            {
                pipeId = route.PipeId;
                pipe = pipeManager.GetPipe(pipeId);
            }

            return pipe;
        }
    }
}

