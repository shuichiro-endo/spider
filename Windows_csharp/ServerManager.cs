/*
 * Title:  spider ServerManager.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace spider
{
    public class ServerManager
    {
        [DllImport("iphlpapi.dll", SetLastError = true)]
        public static extern uint if_nametoindex(string ifname);

        private Dictionary<(uint, uint), Server> serversMap = new Dictionary<(uint, uint), Server>();
        private static readonly object serversMapLock = new object();

        public ServerManager()
        {

        }

        ~ServerManager()
        {

        }

        public Server GetServer(uint connectionId,
                                uint serverId)
        {
            Server server = null;
            ValueTuple<uint, uint> serverKey = (connectionId,
                                                serverId);

            lock(serversMapLock)
            {
                if(serversMap.ContainsKey(serverKey))
                {
                    server = serversMap[serverKey];
                }
            }

            return server;
        }

        public int InsertServer(uint connectionId,
                                uint serverId,
                                Server server)
        {
            int ret = -1;
            ValueTuple<uint, uint> serverKey = (connectionId,
                                                serverId);

            lock(serversMapLock)
            {
                if(!serversMap.ContainsKey(serverKey))
                {
                    server.ConnectionId = connectionId;
                    server.ServerId = serverId;
                    serversMap.Add(serverKey,
                                   server);
                    ret = 0;
                }
            }

            return ret;
        }

        public bool EraseServer(uint connectionId,
                                uint serverId)
        {
            bool ret = false;
            ValueTuple<uint, uint> serverKey = (connectionId,
                                                serverId);

            lock(serversMapLock)
            {
                if(serversMap.ContainsKey(serverKey))
                {
                    ret = serversMap.Remove(serverKey);
                }
            }

            return ret;
        }

        public void ShowServersMap()
        {
            Console.WriteLine("---------------------------------------------------------------------------------------------------------------------------------------- server -----------------------------------------------------------------------------------------------------------------------------------------");
            Console.WriteLine("|connection id|client id |server id |server ip                                     |server port|client destination ip                         |server socket|target ip                                     |target port|target socket|tv_sec |tv_usec|forwarder_tv_sec|forwarder_tv_usec|");
            Console.WriteLine("-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");

            lock(serversMapLock)
            {
                foreach(var kvp in serversMap)
                {
                    Console.WriteLine("|   {0,10}|{1,10}|{2,10}|{3,-46}|      {4,5}|{5,-46}|        {6,5}|{7,-46}|      {8,5}|        {9,5}|{10,7}|{11,7}|         {12,7}|          {13,7}|",
                                      kvp.Value.ConnectionId,
                                      kvp.Value.ClientId,
                                      kvp.Value.ServerId,
                                      kvp.Value.ServerIp,
                                      kvp.Value.ServerPort,
                                      kvp.Value.ClientDestinationIp,
                                      kvp.Value.Sock,
                                      kvp.Value.TargetIp,
                                      kvp.Value.TargetPort,
                                      kvp.Value.TargetSock,
                                      kvp.Value.TvSec,
                                      kvp.Value.TvUsec,
                                      kvp.Value.ForwarderTvSec,
                                      kvp.Value.ForwarderTvUsec);
                }
            }

            Console.WriteLine("-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
            Console.WriteLine("");

            return;
        }

        public string ShowServersMapString()
        {
            string result = "";

            result += "---------------------------------------------------------------------------------------------------------------------------------------- server -----------------------------------------------------------------------------------------------------------------------------------------\n";

            result += "|connection id|client id |server id |server ip                                     |server port|client destination ip                         |server socket|target ip                                     |target port|target socket|tv_sec |tv_usec|forwarder_tv_sec|forwarder_tv_usec|\n";

            result += "-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";

            lock(serversMapLock)
            {
                foreach(var kvp in serversMap)
                {
                    result += string.Format("|   {0,10}|{1,10}|{2,10}|{3,-46}|      {4,5}|{5,-46}|        {6,5}|{7,-46}|      {8,5}|        {9,5}|{10,7}|{11,7}|         {12,7}|          {13,7}|\n",
                                            kvp.Value.ConnectionId,
                                            kvp.Value.ClientId,
                                            kvp.Value.ServerId,
                                            kvp.Value.ServerIp,
                                            kvp.Value.ServerPort,
                                            kvp.Value.ClientDestinationIp,
                                            kvp.Value.Sock,
                                            kvp.Value.TargetIp,
                                            kvp.Value.TargetPort,
                                            kvp.Value.TargetSock,
                                            kvp.Value.TvSec,
                                            kvp.Value.TvUsec,
                                            kvp.Value.ForwarderTvSec,
                                            kvp.Value.ForwarderTvUsec);

                }
            }

            result += "-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\n";

            return result;
        }
    }
}

