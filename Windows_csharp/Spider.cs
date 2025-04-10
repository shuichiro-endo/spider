/*
 * Title:  spider Spider.cs (Windows csharp)
 * Author: Shuichiro Endo
 * Ver:    1.24
 */

using System;
using System.Net;
using System.Net.NetworkInformation;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

namespace spider
{
    public class Spider
    {
        [DllImport("kernel32.dll")]
        private static extern IntPtr GetConsoleWindow();

        [DllImport("user32.dll")]
        private static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

        private const string SPIDER_COMMAND_ADD_NODE_SPIDER_PIPE                         = "1";
        private const string SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT                       = "2";
        private const string SPIDER_COMMAND_SHOW_NODE_INFORMATION                        = "3";
        private const string SPIDER_COMMAND_SHOW_ROUTING_TABLE                           = "4";
        private const string SPIDER_COMMAND_EDIT_ROUTING_TABLE                           = "5";
        private const string SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_TCP                   = "6";
        private const string SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_UDP                   = "7";
        private const string SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_SHELL                 = "8";
        private const string SPIDER_COMMAND_CLOSE_CLIENT_LISTENER_TCP                    = "9";
        private const string SPIDER_COMMAND_EXIT                                         = "0";
        private const int SW_HIDE = 0;


        private static void PrintTitle()
        {
            Console.WriteLine("");
            Console.WriteLine("                 -.                                 _//                   ");
            Console.WriteLine("           .=  :*=--::                       _/     _//                   ");
            Console.WriteLine("      ....  =:::::-==++-::.   _//// _/ _//          _//   _//    _/ _///  ");
            Console.WriteLine("      .:..:--::::..--:..     _//    _/  _// _// _// _// _/   _//  _//     ");
            Console.WriteLine("    .:::==-::.:-===.           _/// _/   _//_//_/   _//_///// _// _//     ");
            Console.WriteLine("  :-.  ::+=-:--=:=*-             _//_// _// _//_/   _//_/         _//     ");
            Console.WriteLine("         -+: ++-  -*-        _// _//_//     _// _// _//  _////   _///     ");
            Console.WriteLine("        :*-  :*-   .:.              _//                                   ");
            Console.WriteLine("        =-    -:       Windows csharp Ver: 1.24   Author: Shuichiro Endo  ");
            Console.WriteLine("");
        }

        private static void Usage(string fileName)
        {
            Console.WriteLine("");
            Console.WriteLine("usage   : {0}", fileName);
            Console.WriteLine("        : [-4 spider_ipv4] [-6 spider_ipv6_global] [-u spider_ipv6_unique_local] [-l spider_ipv6_link_local]");
            Console.WriteLine("        : [-f config_file]");
            Console.WriteLine("        : [-d (hide)] [-i pipe_destination_ip] [-p pipe_destination_port]");
            Console.WriteLine("        : [-r routing_mode(auto:a self:s)]");
            Console.WriteLine("        : [-e x(xor encryption)] [-k key(hexstring)]");
            Console.WriteLine("        : [-e a(aes-256-cbc encryption)] [-k key(hexstring)] [-v iv(hexstring)]");
            Console.WriteLine("        : [-s (prevent spider server startup)]");
            Console.WriteLine("example : {0}", fileName);
            Console.WriteLine("        : {0} -4 192.168.0.10", fileName);
            Console.WriteLine("        : {0} -6 2001::xxxx:xxxx:xxxx:xxxx", fileName);
            Console.WriteLine("        : {0} -u fd00::xxxx:xxxx:xxxx:xxxx", fileName);
            Console.WriteLine("        : {0} -l fe80::xxxx:xxxx:xxxx:xxxx%14", fileName);
            Console.WriteLine("        : {0} -4 192.168.0.10 -6 2001::xxxx:xxxx:xxxx:xxxx -u fd00::xxxx:xxxx:xxxx:xxxx -l fe80::xxxx:xxxx:xxxx:xxxx%14", fileName);
            Console.WriteLine("        : {0} -f config_sample.txt", fileName);
            Console.WriteLine("        : {0} -d -i 192.168.0.25 -p 1025", fileName);
            Console.WriteLine("        : {0} -4 192.168.0.10 -r s", fileName);
            Console.WriteLine("        : {0} -4 192.168.0.10 -e x -k deadbeef", fileName);
            Console.WriteLine("        : {0} -4 192.168.0.10 -e a -k 47a2baa1e39fa16752a2ea8e8e3e24256b3c360f382b9782e2e57d4affb19f8c -v c87114c8b36088074c7ec1398f5c168a", fileName);
            Console.WriteLine("        : {0} -s", fileName);
            Console.WriteLine("");
        }

        static void Main(string[] args)
        {
            int ret = 0;
            string fileName = "spider.exe";
            string spiderIpv4 = "";
            string spiderIpv6Global = "";
            string spiderIpv6UniqueLocal = "";
            string spiderIpv6LinkLocal = "";
            string scopeId = "";
            string ipv6GlobalPrefix = "2001:";
            string ipv6UniqueLocalPrefix = "fd00:";
            string ipv6LinkLocalPrefix = "fe80:";
            NetworkInterface[] networkInterfaces;
            string configFile = "";
            bool hideFlag = false;
            IntPtr handleWindow;
            char mode = 'c';
            string pipeIp = "";
            string pipeIpScopeId = "";
            string pipeDestinationIp = "";
            string pipeDestinationPort = "";
            string routingMode = "a";
            string encryptionType = "";
            string key = "";
            string iv = "";
            bool xorFlag = false;
            bool aesFlag = false;
            string xorKeyHexString = "";
            string aesKeyHexString = "";
            string aesIvHexString = "";
            bool preventSpiderServerStartupFlag = false;
            object[] parameters;


            for(int i = 0; i < args.Length; i++)
            {
                switch(args[i])
                {
                    case "-h":
                        PrintTitle();
                        Usage(fileName);
                        Environment.Exit(-1);
                        break;

                    case "-4":
                        if(i + 1 < args.Length)
                        {
                            spiderIpv4 = args[i + 1];
                            i++;
                        }
                        break;

                    case "-6":
                        if(i + 1 < args.Length)
                        {
                            spiderIpv6Global = args[i + 1];
                            i++;
                        }
                        break;

                    case "-u":
                        if(i + 1 < args.Length)
                        {
                            spiderIpv6UniqueLocal = args[i + 1];
                            i++;
                        }
                        break;

                    case "-l":
                        if(i + 1 < args.Length)
                        {
                            spiderIpv6LinkLocal = args[i + 1];
                            i++;
                        }
                        break;

                    case "-f":
                        if(i + 1 < args.Length)
                        {
                            configFile = args[i + 1];
                            i++;
                        }
                        break;

                    case "-d":
                        hideFlag = true;
                        break;

                    case "-i":
                        if(i + 1 < args.Length)
                        {
                            pipeDestinationIp = args[i + 1];
                            i++;
                        }
                        break;

                    case "-p":
                        if(i + 1 < args.Length)
                        {
                            pipeDestinationPort = args[i + 1];
                            i++;
                        }
                        break;

                    case "-r":
                        if(i + 1 < args.Length)
                        {
                            routingMode = args[i + 1];
                            i++;
                        }
                        break;

                    case "-e":
                        if(i + 1 < args.Length)
                        {
                            encryptionType = args[i + 1];
                            i++;
                        }
                        break;

                    case "-k":
                        if(i + 1 < args.Length)
                        {
                            key = args[i + 1];
                            i++;
                        }
                        break;

                    case "-v":
                        if(i + 1 < args.Length)
                        {
                            iv = args[i + 1];
                            i++;
                        }
                        break;

                    case "-s":
                        preventSpiderServerStartupFlag = true;
                        break;

                    default:
                        PrintTitle();
                        Usage(fileName);
                        Environment.Exit(-1);
                        break;
                }
            }

            // hide
            if(hideFlag == true)
            {
                handleWindow = GetConsoleWindow();

                if(handleWindow != null)
                {
                    ShowWindow(handleWindow,
                               SW_HIDE);
                }
            }

            PrintTitle();

            if(string.IsNullOrEmpty(spiderIpv4) &&
               string.IsNullOrEmpty(spiderIpv6Global) &&
               string.IsNullOrEmpty(spiderIpv6UniqueLocal) &&
               string.IsNullOrEmpty(spiderIpv6LinkLocal))
            {

                IPGlobalProperties.GetIPGlobalProperties();
                networkInterfaces = NetworkInterface.GetAllNetworkInterfaces();

                foreach(NetworkInterface networkInterface in networkInterfaces)
                {
                    IPInterfaceProperties ipProperties = networkInterface.GetIPProperties();
                    foreach(UnicastIPAddressInformation ip in ipProperties.UnicastAddresses)
                    {
                        if(ip.Address.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork)
                        {
                            if(string.IsNullOrEmpty(spiderIpv4))
                            {
                                spiderIpv4 = ip.Address.ToString();
                            }
                        }else if(ip.Address.AddressFamily == System.Net.Sockets.AddressFamily.InterNetworkV6)
                        {
                            if(ip.Address.ToString().StartsWith(ipv6GlobalPrefix, StringComparison.OrdinalIgnoreCase))
                            {
                                if(string.IsNullOrEmpty(spiderIpv6Global))
                                {
                                    spiderIpv6Global = ip.Address.ToString();
                                }
                            }else if(ip.Address.ToString().StartsWith(ipv6UniqueLocalPrefix, StringComparison.OrdinalIgnoreCase))
                            {
                                if(string.IsNullOrEmpty(spiderIpv6UniqueLocal))
                                {
                                    spiderIpv6UniqueLocal = ip.Address.ToString();
                                }
                            }else if(ip.Address.ToString().StartsWith(ipv6LinkLocalPrefix, StringComparison.OrdinalIgnoreCase))
                            {
                                if(string.IsNullOrEmpty(spiderIpv6LinkLocal))
                                {
                                    spiderIpv6LinkLocal = ip.Address.ToString();
                                    int percentIndex = spiderIpv6LinkLocal.IndexOf('%');
                                    if (percentIndex != -1)
                                    {
                                        scopeId = spiderIpv6LinkLocal.Substring(percentIndex + 1);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            SpiderIp spiderIp = new SpiderIp(spiderIpv4,
                                             spiderIpv6Global,
                                             spiderIpv6UniqueLocal,
                                             spiderIpv6LinkLocal);
            if(spiderIp.InitFlag == false)
            {
                Usage(fileName);
                Environment.Exit(-1);
            }

            Encryption encryption;
            XorEncryption xorEncryption;
            AesEncryption aesEncryption;

            if(encryptionType == "x")
            {
                xorFlag = true;
                xorKeyHexString = key;
                xorEncryption = new XorEncryption(xorFlag,
                                                  xorKeyHexString);
                encryption = xorEncryption;
            }else if(encryptionType == "a")
            {
                aesFlag = true;
                aesKeyHexString = key;
                aesIvHexString = iv;
                aesEncryption = new AesEncryption(aesFlag,
                                                  aesKeyHexString,
                                                  aesIvHexString);
                if(aesEncryption.Flag == false)
                {
                    Environment.Exit(-1);
                }

                encryption = aesEncryption;
            }else
            {
                encryption = null;
            }

            ClientManager clientManager = new ClientManager();
            ServerManager serverManager = new ServerManager();
            PipeManager pipeManager = new PipeManager();
            RoutingManager routingManager;
            MessageManager messageManager;

            routingManager = new RoutingManager(spiderIp,
                                                pipeManager);

            messageManager = new MessageManager(spiderIp,
                                                clientManager,
                                                serverManager,
                                                pipeManager,
                                                routingManager);

            routingManager.MessageManager = messageManager;

            SpiderCommand spiderCommand = new SpiderCommand(spiderIp,
                                                            encryption,
                                                            clientManager,
                                                            serverManager,
                                                            pipeManager,
                                                            routingManager,
                                                            messageManager);

            if(routingMode != "s")  // auto
            {
                Thread routingManagerThread = new Thread(new ThreadStart(spiderCommand.RoutingManagerWorker));
                routingManagerThread.Start();
            }

            Thread messageManagerThread = new Thread(new ParameterizedThreadStart(spiderCommand.MessageManagerWorker));
            messageManagerThread.Start(preventSpiderServerStartupFlag);

            if(!string.IsNullOrEmpty(configFile))
            {
                ret = spiderCommand.ReadConfigFile(configFile);
                if(ret < 0)
                {
                    Console.WriteLine("[-] read config file error");
                    Environment.Exit(-1);
                }
            }

            if(!string.IsNullOrEmpty(pipeDestinationIp) &&
               !string.IsNullOrEmpty(pipeDestinationPort))
            {
                if(!pipeDestinationIp.Contains(":"))    // ipv4
                {
                    if(string.IsNullOrEmpty(spiderIp.SpiderIpv4))
                    {
                        Console.WriteLine("[-] spider_ipv4 empty");
                        Environment.Exit(-1);
                    }

                    pipeIp = spiderIp.SpiderIpv4;
                }else if(pipeDestinationIp.StartsWith(ipv6GlobalPrefix, StringComparison.OrdinalIgnoreCase))    // ipv6 global
                {
                    if(string.IsNullOrEmpty(spiderIp.SpiderIpv6Global))
                    {
                        Console.WriteLine("[-] spider_ipv6_global empty");
                        Environment.Exit(-1);
                    }

                    pipeIp = spiderIp.SpiderIpv6Global;
                }else if(pipeDestinationIp.StartsWith(ipv6UniqueLocalPrefix, StringComparison.OrdinalIgnoreCase))    // ipv6 unique local
                {
                    if(string.IsNullOrEmpty(spiderIp.SpiderIpv6UniqueLocal))
                    {
                        Console.WriteLine("[-] spider_ipv6_unique_local empty");
                        Environment.Exit(-1);
                    }

                    pipeIp = spiderIp.SpiderIpv6UniqueLocal;
                }else if(pipeDestinationIp.StartsWith(ipv6LinkLocalPrefix, StringComparison.OrdinalIgnoreCase))    // ipv6 link local
                {
                    if(string.IsNullOrEmpty(spiderIp.SpiderIpv6LinkLocal))
                    {
                        Console.WriteLine("[-] spider_ipv6_link_local empty");
                        Environment.Exit(-1);
                    }

                    if(!pipeDestinationIp.Contains("%"))
                    {
                        Console.WriteLine("[-] pipe_destination_ip includes a scope id: {0}",
                                          pipeDestinationIp);
                        Environment.Exit(-1);
                    }

                    pipeIp = spiderIp.SpiderIpv6LinkLocal;
                    pipeIpScopeId = spiderIp.SpiderIpv6LinkLocalScopeId;
                }else
                {
                    Console.WriteLine("[-] pipe_destination_ip error: {0}",
                                      pipeDestinationIp);
                    Environment.Exit(-1);
                }

                parameters = new object[] {mode,
                                           pipeIp,
                                           pipeIpScopeId,
                                           pipeDestinationIp,
                                           pipeDestinationPort};
                Thread thread = new Thread(new ParameterizedThreadStart(spiderCommand.ConnectPipe));
                thread.Start(parameters);

                Thread.Sleep(5000); // 5s
            }


            string spiderCommandInput = "";

            while(true)
            {
                Console.WriteLine("");
                Console.WriteLine("--------------------------------- spider ---------------------------------");
                if(!string.IsNullOrEmpty(spiderIp.SpiderIpv4))
                {
                    Console.WriteLine(" spider ipv4                     : {0}", spiderIp.SpiderIpv4);
                }
                if(!string.IsNullOrEmpty(spiderIp.SpiderIpv6Global))
                {
                    Console.WriteLine(" spider ipv6 global              : {0}", spiderIp.SpiderIpv6Global);
                }
                if(!string.IsNullOrEmpty(spiderIp.SpiderIpv6UniqueLocal))
                {
                    Console.WriteLine(" spider ipv6 unique local        : {0}", spiderIp.SpiderIpv6UniqueLocal);
                }
                if(!string.IsNullOrEmpty(spiderIp.SpiderIpv6LinkLocal))
                {
                    Console.WriteLine(" spider ipv6 link local          : {0}", spiderIp.SpiderIpv6LinkLocal);
                    if(!string.IsNullOrEmpty(spiderIp.SpiderIpv6LinkLocalScopeId))
                    {
                        Console.WriteLine(" spider ipv6 link local scope id : {0}", spiderIp.SpiderIpv6LinkLocalScopeId);
                    }
                }
                Console.WriteLine(" config file                     : {0}", configFile);
                Console.WriteLine(" routing mode                    : {0}", (routingMode == "s" ? "self" : "auto"));
                Console.WriteLine(" xor encryption                  : {0}", (xorFlag ? "on" : "off"));
                Console.WriteLine(" xor key hex string              : {0}", xorKeyHexString);
                Console.WriteLine(" aes encryption                  : {0}", (aesFlag ? "on" : "off"));
                Console.WriteLine(" aes key hex string              : {0}", aesKeyHexString);
                Console.WriteLine(" aes iv hex string               : {0}", aesIvHexString);
                Console.WriteLine(" prevent spider server startup   : {0}", (preventSpiderServerStartupFlag ? "on" : "off"));
                Console.WriteLine("----------------------------- spider command -----------------------------");
                Console.WriteLine(" {0}: add node (spider pipe)", SPIDER_COMMAND_ADD_NODE_SPIDER_PIPE);
                Console.WriteLine(" {0}: add node (spider client)", SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT);
                Console.WriteLine(" {0}: show node information", SPIDER_COMMAND_SHOW_NODE_INFORMATION);
                Console.WriteLine(" {0}: show routing table", SPIDER_COMMAND_SHOW_ROUTING_TABLE);
                Console.WriteLine(" {0}: edit routing table", SPIDER_COMMAND_EDIT_ROUTING_TABLE);
                Console.WriteLine(" {0}: add node (spider client tcp)", SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_TCP);
                Console.WriteLine(" {0}: add node (spider client udp)", SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_UDP);
                Console.WriteLine(" {0}: add node (spider client shell)", SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_SHELL);
                Console.WriteLine(" {0}: close client listener (tcp)", SPIDER_COMMAND_CLOSE_CLIENT_LISTENER_TCP);
                Console.WriteLine(" {0}: exit", SPIDER_COMMAND_EXIT);
                Console.WriteLine("--------------------------------------------------------------------------");
                Console.WriteLine("");
                Console.Write("command > ");

                spiderCommandInput = Console.ReadLine();

                switch(spiderCommandInput)
                {
                    case SPIDER_COMMAND_ADD_NODE_SPIDER_PIPE:
                        Console.WriteLine("[+] add node (spider pipe)");
                        spiderCommand.AddNodeSpiderPipe();
                        break;

                    case SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT:
                        Console.WriteLine("[+] add node (spider client)");
                        spiderCommand.AddNodeSpiderClient();
                        break;

                    case SPIDER_COMMAND_SHOW_NODE_INFORMATION:
                        Console.WriteLine("[+] show node information");
                        spiderCommand.ShowNodeInformation();
                        break;

                    case SPIDER_COMMAND_SHOW_ROUTING_TABLE:
                        Console.WriteLine("[+] show routing table");
                        spiderCommand.ShowRoutingTable();
                        break;

                    case SPIDER_COMMAND_EDIT_ROUTING_TABLE:
                        Console.WriteLine("[+] edit routing table");
                        spiderCommand.EditRoutingTable();
                        break;

                    case SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_TCP:
                        Console.WriteLine("[+] add node (spider client tcp)");
                        Console.WriteLine("[!] This is not SOCKS5 connection. (Something like TCP over TCP)");
                        spiderCommand.AddNodeSpiderClientTcp();
                        break;

                    case SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_UDP:
                        Console.WriteLine("[+] add node (spider client udp)");
                        Console.WriteLine("[!] This is not SOCKS5 connection. (Something like UDP over TCP)");
                        spiderCommand.AddNodeSpiderClientUdp();
                        break;

                    case SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_SHELL:
                        Console.WriteLine("[+] add node (spider client shell)");
                        Console.WriteLine("[!] This is not SOCKS5 connection.");
                        spiderCommand.AddNodeSpiderClientShell();
                        break;

                    case SPIDER_COMMAND_CLOSE_CLIENT_LISTENER_TCP:
                        Console.WriteLine("[+] close client listener (tcp)");
                        spiderCommand.CloseClientListenerTcp();
                        break;

                    case SPIDER_COMMAND_EXIT:
                        Console.WriteLine("[+] exit");
                        Environment.Exit(0);
                        break;

                    default:
                        break;
                }
            }
        }
    }
}

