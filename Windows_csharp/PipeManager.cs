/*
 * Title:  spider PipeManager.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace spider
{
    public class PipeManager
    {
        [DllImport("iphlpapi.dll", SetLastError = true)]
        public static extern uint if_nametoindex(string ifname);

        private Dictionary<uint, Pipe> pipesMap = new Dictionary<uint, Pipe>();
        private static readonly object pipesMapLock = new object();

        public PipeManager()
        {

        }

        ~PipeManager()
        {

        }

        public Pipe GetPipe(uint pipeId)
        {
            Pipe pipe = null;

            lock(pipesMapLock)
            {
                if(pipesMap.ContainsKey(pipeId))
                {
                    pipe = pipesMap[pipeId];
                }
            }

            return pipe;
        }

        public bool CheckPipe(uint pipeId)
        {
            bool ret = false;

            lock(pipesMapLock)
            {
                ret = pipesMap.ContainsKey(pipeId);
            }

            return ret;
        }

        public int InsertPipe(uint pipeId,
                              Pipe pipe)
        {
            int ret = -1;

            lock(pipesMapLock)
            {
                if(!pipesMap.ContainsKey(pipeId))
                {
                    pipe.PipeId = pipeId;
                    pipesMap.Add(pipeId,
                                 pipe);
                    ret = 0;
                }
            }

            return ret;
        }

        public bool ErasePipe(uint pipeId)
        {
            bool ret = false;

            lock(pipesMapLock)
            {
                if(pipesMap.ContainsKey(pipeId))
                {
                    pipesMap.Remove(pipeId);
                    ret = true;
                }
            }

            return ret;
        }

        public void ShowPipesMap()
        {
            Console.WriteLine("--------------------------------------------------------------------------------------------------------- pipe ---------------------------------------------------------------------------------------------------------");
            Console.WriteLine("|pipe id   |mode|message|pipe ip                                       |pipe ip scope id|pipe listen port|pipe destination ip                           |pipe destination ip scope id|pipe destination port|pipe socket|");
            Console.WriteLine("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");

            lock(pipesMapLock)
            {
                foreach(var kvp in pipesMap)
                {
                    Console.WriteLine("|{0,10}|{1}   |{2}      |{3,-46}|{4,-10}      |           {5,5}|{6,-46}|{7,-10}                  |                {8,5}|      {9,5}|",
                                      kvp.Value.PipeId,
                                      kvp.Value.Mode,
                                      kvp.Value.MessageMode,
                                      kvp.Value.PipeIp,
                                      kvp.Value.PipeIpScopeId,
                                      kvp.Value.PipeListenPort,
                                      kvp.Value.PipeDestinationIp,
                                      kvp.Value.PipeDestinationIpScopeId,
                                      kvp.Value.PipeDestinationPort,
                                      kvp.Value.Sock);
                }
            }

            Console.WriteLine("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
            Console.WriteLine("");

            return;
        }

        public string ShowPipesMapString()
        {
            string result = "";

            result += "--------------------------------------------------------------------------------------------------------- pipe ---------------------------------------------------------------------------------------------------------\n";

            result += "|pipe id   |mode|message|pipe ip                                       |pipe ip scope id|pipe listen port|pipe destination ip                           |pipe destination ip scope id|pipe destination port|pipe socket|\n";

            result += "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";

            lock(pipesMapLock)
            {
                foreach(var kvp in pipesMap)
                {
                    string pipeIpScopeId = "";
                    if(if_nametoindex(kvp.Value.PipeIpScopeId) > 0)
                    {
                        pipeIpScopeId = if_nametoindex(kvp.Value.PipeIpScopeId).ToString();
                    }else
                    {
                        pipeIpScopeId = "";
                    }

                    string pipeDestinationIpScopeId = "";
                    if(if_nametoindex(kvp.Value.PipeDestinationIpScopeId) > 0)
                    {
                        pipeDestinationIpScopeId = if_nametoindex(kvp.Value.PipeDestinationIpScopeId).ToString();
                    }else
                    {
                        pipeDestinationIpScopeId = "";
                    }

                    result += string.Format("|{0,10}|{1}   |{2}      |{3,-46}|{4,-10} ({5,3})|           {6,5}|{7,-46}|{8,-10}             ({9,3})|                {10,5}|      {11,5}|\n",
                                      kvp.Value.PipeId,
                                      kvp.Value.Mode,
                                      kvp.Value.MessageMode,
                                      kvp.Value.PipeIp,
                                      kvp.Value.PipeIpScopeId,
                                      pipeIpScopeId,
                                      kvp.Value.PipeListenPort,
                                      kvp.Value.PipeDestinationIp,
                                      kvp.Value.PipeDestinationIpScopeId,
                                      pipeDestinationIpScopeId,
                                      kvp.Value.PipeDestinationPort,
                                      kvp.Value.Sock);
                }
            }

            result += "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\n";

            return result;
        }

        public void SendRoutingMessage(RoutingMessage routingMessage)
        {
            lock(pipesMapLock)
            {
                foreach(var kvp in pipesMap)
                {
                    Pipe pipe = kvp.Value;
                    if(pipe.PipeId != 0 && pipe.Mode != 's')
                    {
                        pipe.PushRoutingMessage(routingMessage);
                    }
                }
            }

            return;
        }
    }
}

