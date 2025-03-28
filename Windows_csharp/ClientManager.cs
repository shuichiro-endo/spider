/*
 * Title:  spider ClientManager.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace spider
{
    public class ClientManager
    {
        [DllImport("iphlpapi.dll", SetLastError = true)]
        public static extern uint if_nametoindex(string ifname);

        private Dictionary<(uint, uint), Client> clientsMap = new Dictionary<(uint, uint), Client>();
        private static readonly object clientsMapLock = new object();

        public ClientManager()
        {

        }

        ~ClientManager()
        {

        }

        public Client GetClient(uint connectionId,
                                uint clientId)
        {
            Client client = null;
            ValueTuple<uint, uint> clientKey = (connectionId,
                                                clientId);

            lock(clientsMapLock)
            {
                if(clientsMap.ContainsKey(clientKey))
                {
                    client = clientsMap[clientKey];
                }
            }

            return client;
        }

        public int InsertClient(uint connectionId,
                                uint clientId,
                                Client client)
        {
            int ret = -1;
            ValueTuple<uint, uint> clientKey = (connectionId,
                                                clientId);

            lock(clientsMapLock)
            {
                if(!clientsMap.ContainsKey(clientKey))
                {
                    client.ConnectionId = connectionId;
                    client.ClientId = clientId;
                    clientsMap.Add(clientKey,
                                   client);
                    ret = 0;
                }
            }

            return ret;
        }

        public bool EraseClient(uint connectionId,
                                uint clientId)
        {
            bool ret = false;
            ValueTuple<uint, uint> clientKey = (connectionId,
                                                clientId);

            lock(clientsMapLock)
            {
                if(clientsMap.ContainsKey(clientKey))
                {
                    ret = clientsMap.Remove(clientKey);
                }
            }

            return ret;
        }

        public void ShowClientsMap()
        {
            Console.WriteLine("-------------------------------------------------------------------------------------------------------------------------------------------------------- client --------------------------------------------------------------------------------------------------------------------------------------------------------");
            Console.WriteLine("|type  |connection id|client id |server id |client ip                                     |client ip scope id|client listen port|client port|destination spider ip                         |target ip                                     |target port|client socket|tv_sec |tv_usec|forwarder_tv_sec|forwarder_tv_usec|");
            Console.WriteLine("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");

            lock(clientsMapLock)
            {
                foreach(var kvp in clientsMap)
                {
                    Console.WriteLine("|{0,-6}|   {1,10}|{2,10}|{3,10}|{4,-46}|{5,-10}        |             {6,5}|      {7,5}|{8,-46}|{9,-46}|      {10,5}|        {11,5}|{12,7}|{13,7}|         {14,7}|          {15,7}|",
                                      kvp.Value.Type,
                                      kvp.Value.ConnectionId,
                                      kvp.Value.ClientId,
                                      kvp.Value.ServerId,
                                      kvp.Value.ClientIp,
                                      kvp.Value.ClientIpScopeId,
                                      kvp.Value.ClientListenPort,
                                      kvp.Value.ClientPort,
                                      kvp.Value.DestinationSpiderIp,
                                      kvp.Value.TargetIp,
                                      kvp.Value.TargetPort,
                                      kvp.Value.Sock,
                                      kvp.Value.TvSec,
                                      kvp.Value.TvUsec,
                                      kvp.Value.ForwarderTvSec,
                                      kvp.Value.ForwarderTvUsec);
                }
            }

            Console.WriteLine("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
            Console.WriteLine("");

            return;
        }

        public string ShowClientsMapString()
        {
            string result = "";

            result += "-------------------------------------------------------------------------------------------------------------------------------------------------------- client --------------------------------------------------------------------------------------------------------------------------------------------------------\n";

            result += "|type  |connection id|client id |server id |client ip                                     |client ip scope id|client listen port|client port|destination spider ip                         |target ip                                     |target port|client socket|tv_sec |tv_usec|forwarder_tv_sec|forwarder_tv_usec|\n";

            result += "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";

            lock(clientsMapLock)
            {
                foreach(var kvp in clientsMap)
                {
                    string clientIpScopeId = "";
                    if(if_nametoindex(kvp.Value.ClientIpScopeId) > 0)
                    {
                        clientIpScopeId = if_nametoindex(kvp.Value.ClientIpScopeId).ToString();
                    }else
                    {
                        clientIpScopeId = "";
                    }

                    result += string.Format("|{0,-6}|   {1,10}|{2,10}|{3,10}|{4,-46}|{5,-10}   ({6,3})|             {7,5}|      {8,5}|{9,-46}|{10,-46}|      {11,5}|        {12,5}|{13,7}|{14,7}|         {15,7}|          {16,7}|\n",
                                            kvp.Value.Type,
                                            kvp.Value.ConnectionId,
                                            kvp.Value.ClientId,
                                            kvp.Value.ServerId,
                                            kvp.Value.ClientIp,
                                            kvp.Value.ClientIpScopeId,
                                            clientIpScopeId,
                                            kvp.Value.ClientListenPort,
                                            kvp.Value.ClientPort,
                                            kvp.Value.DestinationSpiderIp,
                                            kvp.Value.TargetIp,
                                            kvp.Value.TargetPort,
                                            kvp.Value.Sock,
                                            kvp.Value.TvSec,
                                            kvp.Value.TvUsec,
                                            kvp.Value.ForwarderTvSec,
                                            kvp.Value.ForwarderTvUsec);
                }
            }

            result += "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\n";

            return result;
        }
    }
}

