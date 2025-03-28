/*
 * Title:  spider Server.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

namespace spider
{
    public class Server : Node
    {
        private const int SOCKS5_MESSAGE_DATA_SIZE = 60000;
        private const int SHELL_UPLOAD_DOWNLOAD_DATA_SIZE = 50000;
        private const int FORWARDER_UDP_TIMEOUT = 300;
        private const int SHOW_NODE_INFORMATION_WORKER_TV_SEC = 10;
        private const int SHOW_NODE_INFORMATION_WORKER_TV_USEC = 0;
        private const int SHOW_NODE_INFORMATION_WORKER_FORWARDER_TV_SEC = 10;
        private const int SHOW_NODE_INFORMATION_WORKER_FORWARDER_TV_USEC = 0;
        private const int ADD_NODE_TO_DESTINATION_SPIDER_WORKER_TV_SEC = 10;
        private const int ADD_NODE_TO_DESTINATION_SPIDER_WORKER_TV_USEC = 0;
        private const int ADD_NODE_TO_DESTINATION_SPIDER_WORKER_FORWARDER_TV_SEC = 60;
        private const int ADD_NODE_TO_DESTINATION_SPIDER_WORKER_FORWARDER_TV_USEC = 0;
        private const int SOCKS5_AUTHENTICATION_METHOD = 0x0; // 0x0:No Authentication Required  0x2:Username/Password Authentication
        private const string SOCKS5_USERNAME = "socks5user";
        private const string SOCKS5_PASSWORD = "supersecretpassword";

        private SpiderIp spiderIp;
        private uint connectionId;
        private uint clientId;
        private uint serverId;
        private string serverIp;
        private string serverPort;
        private string clientDestinationIp;
        private string targetIp;
        private string targetPort;
        private TcpClient targetTcpClient = null;
        private NetworkStream targetStream = null;
        private UdpClient targetUdpClient = null;
        private IPEndPoint targetIpEndPoint = null;
        private IntPtr targetSock = IntPtr.Zero;
        private int tvSec;
        private int tvUsec;
        private int forwarderTvSec;
        private int forwarderTvUsec;
        private Encryption encryption;
        private uint recvMessageId = 0;
        private uint nextRecvMessageId = 0;
        private uint sendMessageId = 0;
        private ClientManager clientManager;
        private ServerManager serverManager;
        private PipeManager pipeManager;
        private SpiderCommand spiderCommand;

        public Server(SpiderIp spiderIp,
                      uint connectionId,
                      uint clientId,
                      uint serverId,
                      string serverIp,
                      string serverPort,
                      string clientDestinationIp,
                      int tvSec,
                      int tvUsec,
                      int forwarderTvSec,
                      int forwarderTvUsec,
                      Encryption encryption,
                      ClientManager clientManager,
                      ServerManager serverManager,
                      PipeManager pipeManager,
                      MessageManager messageManager,
                      SpiderCommand spiderCommand)
        : base(null,
               null,
               messageManager)
        {
            this.spiderIp = spiderIp;
            this.connectionId = connectionId;
            this.clientId = clientId;
            this.serverId = serverId;
            this.serverIp = serverIp;
            this.serverPort = serverPort;
            this.clientDestinationIp = clientDestinationIp;
            this.tvSec = tvSec;
            this.tvUsec = tvUsec;
            this.forwarderTvSec = forwarderTvSec;
            this.forwarderTvUsec = forwarderTvUsec;
            this.encryption = encryption;
            this.clientManager = clientManager;
            this.serverManager = serverManager;
            this.pipeManager = pipeManager;
            this.messageManager = messageManager;
            this.spiderCommand = spiderCommand;

            this.sock = IntPtr.Zero;
            this.targetSock = IntPtr.Zero;
        }

        ~Server()
        {

        }

        public SpiderIp SpiderIp
        {
            get { return spiderIp; }
            set { spiderIp = value; }
        }

        public uint ConnectionId
        {
            get { return connectionId; }
            set { connectionId = value; }
        }

        public uint ClientId
        {
            get { return clientId; }
            set { clientId = value; }
        }

        public uint ServerId
        {
            get { return serverId; }
            set { serverId = value; }
        }

        public string ServerIp
        {
            get { return serverIp; }
            set { serverIp = value; }
        }

        public string ServerPort
        {
            get { return serverPort; }
            set { serverPort = value; }
        }

        public string ClientDestinationIp
        {
            get { return clientDestinationIp; }
            set { clientDestinationIp = value; }
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

        public IntPtr TargetSock
        {
            get { return targetSock; }
            set { targetSock = value; }
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

        public int RecvMessage(byte[] buffer,
                               int bufferSize,
                               int tvSec,
                               int tvUsec)
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

                            if(encryption != null
                               && encryption.Flag)
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

        public int SendMessage(byte[] buffer,
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
                                              's',
                                              this.serverIp,
                                              'c',
                                              this.clientDestinationIp,
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
                    Console.WriteLine("[+] [client -> server] RecvMessage");
#endif
                    Array.Clear(buffer,
                                0,
                                bufferMaxLength);

                    rec = RecvMessage(buffer,
                                      bufferMaxLength,
                                      forwarderTvSec,
                                      forwarderTvUsec);
                    if(rec > 0)
                    {
                        if(recvMessageId == nextRecvMessageId)
                        {
                            length = rec;
#if DEBUGPRINT
                            Console.WriteLine("[+] [server -> target] Write message_id:{0}",
                                             nextRecvMessageId);
#endif
                            targetStream.Write(buffer,
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
                                Console.WriteLine("[+] [server -> target] Write message_id:{0}",
                                                 nextRecvMessageId);
#endif
                                targetStream.Write(buffer,
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
                    Console.WriteLine("[-] ForwarderRecvData error: {0}",
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
        }

        private void ForwarderSendData()
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
                    Console.WriteLine("[+] [server <- target] Read");
#endif
                    Array.Clear(buffer,
                                0,
                                bufferMaxLength);

                    rec = targetStream.Read(buffer,
                                            0,
                                            socks5MessageDataMaxSize);
                    if(rec > 0)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[+] [client <- server] SendMessage message_id:{0}",
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
                    }else
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [server <- target] Read error rec: {0}",
                                          rec);
#endif
                        break;
                    }
                }catch(IOException ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderSendData error: {0}",
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
        }

        private int Forwarder()
        {
            int timeout = forwarderTvSec * 1000 + forwarderTvUsec / 1000;
            this.targetTcpClient.ReceiveTimeout = timeout;
            this.targetTcpClient.SendTimeout = timeout;

            Thread thread1 = new Thread(new ThreadStart(ForwarderRecvData));
            Thread thread2 = new Thread(new ThreadStart(ForwarderSendData));

            thread1.Start();
            thread2.Start();

            thread1.Join();
            thread2.Join();

            return 0;
        }

        private int SendSocksResponseIpv4(byte[] buffer,
                                          int bufferSize,
                                          byte ver,
                                          byte rep,
                                          byte rsv,
                                          byte atyp)
        {
            int sen = 0;
            int size = 0;
            byte[] bndAddr = new byte[4];
            byte[] bndPort = new byte[2];


            Array.Clear(buffer,
                        0,
                        bufferSize);

            try
            {
                SocksResponseIpv4 socksResponseIvp4 = new SocksResponseIpv4(ver,
                                                                            rep,
                                                                            rsv,
                                                                            atyp,
                                                                            bndAddr,
                                                                            bndPort);
                size = socksResponseIvp4.CopyToBuffer(ref buffer);
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socksResponseIvp4 error: {0}",
                                  ex.Message);
#endif
                return 0;
            }

            sendMessageId++;
            sen = SendMessage(buffer,
                              size,
                              tvSec,
                              tvUsec);

            return sen;
        }

        private int SendSocksResponseIpv6(byte[] buffer,
                                          int bufferSize,
                                          byte ver,
                                          byte rep,
                                          byte rsv,
                                          byte atyp)
        {
            int sen = 0;
            int size = 0;
            byte[] bndAddr = new byte[16];
            byte[] bndPort = new byte[2];


            Array.Clear(buffer,
                        0,
                        bufferSize);

            try
            {
                SocksResponseIpv6 socksResponseIvp6 = new SocksResponseIpv6(ver,
                                                                            rep,
                                                                            rsv,
                                                                            atyp,
                                                                            bndAddr,
                                                                            bndPort);
                size = socksResponseIvp6.CopyToBuffer(ref buffer);
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] SocksResponseIpv6 error: {0}",
                                  ex.Message);
#endif
                return 0;
            }

            sendMessageId++;
            sen = SendMessage(buffer,
                              size,
                              tvSec,
                              tvUsec);

            return sen;
        }

        private string ExecuteCommand(string command)
        {
            string result = "";
            ProcessStartInfo processStartInfo = new ProcessStartInfo("cmd.exe",
                                                                     $"/c {command}")
            {
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true
            };

            using(Process process = new Process())
            {
                process.StartInfo = processStartInfo;
                process.Start();

                result += process.StandardError.ReadToEnd();
                result += process.StandardOutput.ReadToEnd();

                process.WaitForExit();
            }

            return result;
        }

        private string[] SplitInput(byte[] input)
        {
            string inputString;
            string[] tokens;


            inputString = Encoding.UTF8.GetString(input);
            tokens = inputString.Split(' ');

            return tokens;
        }

        private int ForwarderShell()
        {
            int rec = 0;
            int sen = 0;
            int length = 0;

            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            byte[] data = new byte[NODE_BUFFER_SIZE];
            byte[] tmp;
            int bufferMaxLength = NODE_BUFFER_SIZE;
            int socks5MessageDataMaxSize = SOCKS5_MESSAGE_DATA_SIZE;
            Dictionary<uint, (int, byte[])> msgsMap = new Dictionary<uint, (int, byte[])>();
            ValueTuple<int, byte[]> msg;
            recvMessageId = 0;

            string result = "";
            int resultSize = 0;
            string prompt = "\ncommand >";
            byte[] promptBytes = Encoding.UTF8.GetBytes(prompt);
            int promptSize = promptBytes.Length;
            byte[] input;
            string inputString = "";
            string[] tokens;
            byte[] commandTmp;
            string command = "";
            string exitCommand = "exit";
            bool exitFlag = false;
            string cdCommand = "cd";
            string uploadCommand = "upload";
            FileStream uploadFileStream = null;
            string uploadFileName = "";
            string uploadFilePath = "";
            ulong uploadFileSize = 0;
            ulong recvUploadFileDataSize = 0;
            ulong uploadFileRemainingSize = 0;
            bool uploadCommandFlag = false;
            bool uploadFileFlag = false;
            string downloadCommand = "download";
            FileStream downloadFileStream = null;
            string downloadFileName = "";
            string downloadFilePath = "";
            ulong downloadFileSize = 0;
            ulong downloadFileRemainingSize = 0;
            ulong readBytes = 0;
            ulong dataSize = 0;
            UploadDownloadData uploadDownloadData;


            while(true)
            {
                if(exitFlag == true)
                {
                    break;
                }

                try
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] [client -> server] RecvMessage");
#endif
                    Array.Clear(buffer,
                                0,
                                bufferMaxLength);

                    rec = RecvMessage(buffer,
                                      bufferMaxLength,
                                      forwarderTvSec,
                                      forwarderTvUsec);
                    if(rec > 0)
                    {
                        if(recvMessageId == nextRecvMessageId)
                        {
                            nextRecvMessageId++;
                            uploadCommandFlag = false;
                            result = "";

                            try{
                                commandTmp = new byte[16];
                                for(int i = 0; i < 16; i++)
                                {
                                    commandTmp[i] = buffer[i];
                                }
                                commandTmp = commandTmp.Where(b => b != 0x00).ToArray();
                                command = Encoding.UTF8.GetString(commandTmp);
                                if(String.Compare(command, uploadCommand) == 0)
                                {
                                    uploadCommandFlag = true;
                                }else
                                {
                                    uploadCommandFlag = false;
                                }
                            }catch(Exception ex)
                            {
                                uploadCommandFlag = false;
                            }

                            if(uploadCommandFlag == true)
                            {
                                try
                                {
                                    if(uploadFileFlag == false)
                                    {
                                        uploadFileFlag = true;
                                        uploadFileStream = null;
                                        uploadDownloadData = new UploadDownloadData(buffer);

                                        tmp = uploadDownloadData.FilePath.Where(b => b != 0x00).ToArray();
                                        uploadFilePath = Encoding.UTF8.GetString(tmp);
                                        uploadFilePath += "\\";

                                        tmp = uploadDownloadData.FileName.Where(b => b != 0x00).ToArray();
                                        uploadFileName = Encoding.UTF8.GetString(tmp);
                                        uploadFileName = uploadFilePath + uploadFileName;
                                        uploadFileSize = uploadDownloadData.FileSize;

                                        uploadFileStream = new FileStream(uploadFileName,
                                                                          FileMode.Create,
                                                                          FileAccess.Write);

                                        recvUploadFileDataSize = uploadDownloadData.DataSize;
                                        data = uploadDownloadData.Data;

                                        uploadFileStream.Write(data,
                                                               0,
                                                               (int)recvUploadFileDataSize);

                                        uploadFileRemainingSize = uploadFileSize - recvUploadFileDataSize;
                                        if(uploadFileRemainingSize > 0)
                                        {
                                            continue;
                                        }
                                    }else
                                    {
                                        uploadDownloadData = new UploadDownloadData(buffer);

                                        recvUploadFileDataSize = uploadDownloadData.DataSize;
                                        data = uploadDownloadData.Data;

                                        uploadFileStream.Write(data,
                                                               0,
                                                               (int)recvUploadFileDataSize);

                                        uploadFileRemainingSize -= recvUploadFileDataSize;
                                        if(uploadFileRemainingSize > 0)
                                        {
                                            continue;
                                        }
                                    }
                                }catch(Exception ex)
                                {
#if DEBUGPRINT
                                    Console.WriteLine("[-] upload file error: {0}",
                                                      ex.Message);
#endif
                                    Array.Clear(buffer,
                                                0,
                                                bufferMaxLength);

                                    result = "[-] upload file error";
                                    result += prompt;

                                    tmp = Encoding.UTF8.GetBytes(result);
                                    length = tmp.Length;
                                    for(int i = 0; i < length; i++)
                                    {
                                        buffer[i] = tmp[i];
                                    }

#if DEBUGPRINT
                                    Console.WriteLine("[+] [client <- server] SendMessage message_id:{0}",
                                                      sendMessageId);
#endif
                                    sen = SendMessage(buffer,
                                                      length,
                                                      forwarderTvSec,
                                                      forwarderTvUsec);
                                    if(sen > 0)
                                    {
                                        sendMessageId++;
                                    }

                                    if(uploadFileStream != null)
                                    {
                                        uploadFileStream.Close();
                                        uploadFileStream = null;
                                    }

                                    uploadCommandFlag = false;
                                    uploadFileFlag = false;
                                    uploadFileName = "";
                                    uploadFilePath = "";
                                    uploadFileSize = 0;
                                    recvUploadFileDataSize = 0;
                                    uploadFileRemainingSize = 0;

                                    continue;
                                }

                                uploadFileStream.Close();
                                uploadFileStream = null;

                                Array.Clear(buffer,
                                            0,
                                            bufferMaxLength);

                                result = "[+] upload file: ";
                                result += uploadFileName;
                                result += prompt;

                                tmp = Encoding.UTF8.GetBytes(result);
                                length = tmp.Length;
                                for(int i = 0; i < length; i++)
                                {
                                    buffer[i] = tmp[i];
                                }

#if DEBUGPRINT
                                Console.WriteLine("[+] [client <- server] SendMessage message_id:{0}",
                                                  sendMessageId);
#endif
                                sen = SendMessage(buffer,
                                                  length,
                                                  forwarderTvSec,
                                                  forwarderTvUsec);
                                if(sen > 0)
                                {
                                    sendMessageId++;
                                }

                                uploadCommandFlag = false;
                                uploadFileFlag = false;
                                uploadFileName = "";
                                uploadFilePath = "";
                                uploadFileSize = 0;
                                recvUploadFileDataSize = 0;
                                uploadFileRemainingSize = 0;

                                continue;
                            }else
                            {
                                input = buffer.Where(b => b != 0x00).ToArray();
                                tokens = SplitInput(input);
                                if(tokens.Length == 0)
                                {
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
                                    Console.WriteLine("[+] [client <- server] SendMessage message_id:{0}",
                                                      sendMessageId);
#endif
                                    sen = SendMessage(buffer,
                                                      length,
                                                      forwarderTvSec,
                                                      forwarderTvUsec);
                                    if(sen > 0)
                                    {
                                        sendMessageId++;
                                    }

                                    continue;
                                }else if(String.Compare(tokens[0].Replace("\r\n", "").Replace("\n", "").Replace("\r", ""), exitCommand) == 0)
                                {
                                    exitFlag = true;
                                    break;
                                }else if(String.Compare(tokens[0], cdCommand) == 0)
                                {
                                    if(tokens.Length > 1)
                                    {
                                        try
                                        {
                                            Directory.SetCurrentDirectory(tokens[1]);

                                            Array.Clear(buffer,
                                                        0,
                                                        bufferMaxLength);

                                            tmp = Encoding.UTF8.GetBytes(prompt);
                                            length = tmp.Length;
                                            for(int i = 0; i < length; i++)
                                            {
                                                buffer[i] = tmp[i];
                                            }
                                        }catch(Exception ex)
                                        {
#if DEBUGPRINT
                                            Console.WriteLine("[-] change directory error: {0}, {1}",
                                                              tokens[1],
                                                              ex.Message);
#endif
                                            result = "[-] change directory error";
                                            result += prompt;

                                            tmp = Encoding.UTF8.GetBytes(result);
                                            length = tmp.Length;
                                            for(int i = 0; i < length; i++)
                                            {
                                                buffer[i] = tmp[i];
                                            }
                                        }
                                    }else
                                    {
                                        inputString = Encoding.UTF8.GetString(input);
                                        result = ExecuteCommand(inputString);
                                        result += prompt;

                                        tmp = Encoding.UTF8.GetBytes(result);
                                        length = tmp.Length;
                                        for(int i = 0; i < length; i++)
                                        {
                                            buffer[i] = tmp[i];
                                        }
                                    }
#if DEBUGPRINT
                                    Console.WriteLine("[+] [client <- server] SendMessage message_id:{0}",
                                                      sendMessageId);
#endif
                                    sen = SendMessage(buffer,
                                                      length,
                                                      forwarderTvSec,
                                                      forwarderTvUsec);
                                   if(sen > 0)
                                   {
                                       sendMessageId++;
                                   }

                                   continue;
                                }else if(tokens.Length == 3 &&
                                         (String.Compare(tokens[0], downloadCommand) == 0))
                                {
                                    downloadFileStream = null;

                                    try
                                    {
                                        downloadFileStream = new FileStream(tokens[1],
                                                                            FileMode.Open,
                                                                            FileAccess.Read);

                                        downloadFileSize = (ulong)downloadFileStream.Length;
                                        downloadFileRemainingSize = downloadFileSize;

                                        string path = Path.GetDirectoryName(tokens[1]);
                                        downloadFileName = Path.GetFileName(tokens[1]);
                                        downloadFilePath = tokens[2].Replace("\r\n", "").Replace("\n", "").Replace("\r", "");

                                        while(downloadFileRemainingSize > 0)
                                        {
                                            Array.Clear(buffer,
                                                        0,
                                                        bufferMaxLength);

                                            Array.Clear(data,
                                                        0,
                                                        bufferMaxLength);

                                            readBytes = 0;
                                            dataSize = 0;

                                            if(downloadFileRemainingSize <= SHELL_UPLOAD_DOWNLOAD_DATA_SIZE)
                                            {
                                                readBytes = (ulong)downloadFileStream.Read(data,
                                                                                           0,
                                                                                           (int)downloadFileRemainingSize);
                                                dataSize = downloadFileRemainingSize;
                                            }else
                                            {
                                                readBytes = (ulong)downloadFileStream.Read(data,
                                                                                           0,
                                                                                           SHELL_UPLOAD_DOWNLOAD_DATA_SIZE);
                                                dataSize = SHELL_UPLOAD_DOWNLOAD_DATA_SIZE;
                                            }

                                            uploadDownloadData = new UploadDownloadData(downloadCommand,
                                                                                        downloadFileName,
                                                                                        downloadFilePath,
                                                                                        downloadFileSize,
                                                                                        dataSize,
                                                                                        data);

                                            length = uploadDownloadData.CopyToBuffer(ref buffer);

#if DEBUGPRINT
                                            Console.WriteLine("[+] [client <- server] SendMessage message_id:{0}",
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

                                            sendMessageId++;
                                            downloadFileRemainingSize -= readBytes;
                                        }
                                    }catch(Exception ex)
                                    {
#if DEBUGPRINT
                                        Console.WriteLine("[-] download file error: {0}",
                                                          ex.Message);
#endif
                                        Array.Clear(buffer,
                                                    0,
                                                    bufferMaxLength);

                                        result = "[-] download file error";
                                        result += prompt;

                                        tmp = Encoding.UTF8.GetBytes(result);
                                        length = tmp.Length;
                                        for(int i = 0; i < length; i++)
                                        {
                                            buffer[i] = tmp[i];
                                        }

#if DEBUGPRINT
                                        Console.WriteLine("[+] [client <- server] SendMessage message_id:{0}",
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

                                        sendMessageId++;
                                    }

                                    continue;
                                }else
                                {
                                    inputString = Encoding.UTF8.GetString(input);
                                    result = ExecuteCommand(inputString);
                                }

                                Array.Clear(buffer,
                                            0,
                                            bufferMaxLength);

                                tmp = Encoding.UTF8.GetBytes(result);
                                resultSize = tmp.Length;
                                if(resultSize > socks5MessageDataMaxSize - 0x10 - promptSize)
                                {
                                    resultSize = socks5MessageDataMaxSize - 0x10 - promptSize;
                                }

                                for(int i = 0; i < resultSize; i++)
                                {
                                    buffer[i] = tmp[i];
                                }

                                for(int i = 0; i < promptSize; i++)
                                {
                                    buffer[resultSize + i] = promptBytes[i];
                                }

                                length = resultSize + promptSize;
#if DEBUGPRINT
                                Console.WriteLine("[+] [client <- server] SendMessage message_id:{0}",
                                                  sendMessageId);
#endif
                                sen = SendMessage(buffer,
                                                  length,
                                                  forwarderTvSec,
                                                  forwarderTvUsec);
                                if(sen > 0)
                                {
                                    sendMessageId++;
                                }

                                continue;
                            }
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

                                rec = msg.Item1;
                                buffer = msg.Item2;

                                nextRecvMessageId++;
                                uploadCommandFlag = false;
                                result = "";

                                try{
                                    commandTmp = new byte[16];
                                    for(int i = 0; i < 16; i++)
                                    {
                                        commandTmp[i] = buffer[i];
                                    }
                                    commandTmp = commandTmp.Where(b => b != 0x00).ToArray();
                                    command = Encoding.UTF8.GetString(commandTmp);
                                    if(String.Compare(command, uploadCommand) == 0)
                                    {
                                        uploadCommandFlag = true;
                                    }else
                                    {
                                        uploadCommandFlag = false;
                                    }
                                }catch(Exception ex)
                                {
                                    uploadCommandFlag = false;
                                }

                                if(uploadCommandFlag == true)
                                {
                                    try
                                    {
                                        if(uploadFileFlag == false)
                                        {
                                            uploadFileFlag = true;
                                            uploadFileStream = null;

                                            uploadDownloadData = new UploadDownloadData(buffer);

                                            tmp = uploadDownloadData.FilePath.Where(b => b != 0x00).ToArray();
                                            uploadFilePath = Encoding.UTF8.GetString(tmp);
                                            uploadFilePath += "\\";

                                            tmp = uploadDownloadData.FileName.Where(b => b != 0x00).ToArray();
                                            uploadFileName = Encoding.UTF8.GetString(tmp);
                                            uploadFileName = uploadFilePath + uploadFileName;
                                            uploadFileSize = uploadDownloadData.FileSize;

                                            uploadFileStream = new FileStream(uploadFileName,
                                                                              FileMode.Create,
                                                                              FileAccess.Write);

                                            recvUploadFileDataSize = uploadDownloadData.DataSize;
                                            data = uploadDownloadData.Data;

                                            uploadFileStream.Write(data,
                                                                   0,
                                                                   (int)recvUploadFileDataSize);

                                            uploadFileRemainingSize = uploadFileSize - recvUploadFileDataSize;
                                            if(uploadFileRemainingSize > 0)
                                            {
                                                continue;
                                            }
                                        }else
                                        {
                                            uploadDownloadData = new UploadDownloadData(buffer);

                                            recvUploadFileDataSize = uploadDownloadData.DataSize;
                                            data = uploadDownloadData.Data;

                                            uploadFileStream.Write(data,
                                                                   0,
                                                                   (int)recvUploadFileDataSize);

                                            uploadFileRemainingSize -= recvUploadFileDataSize;
                                            if(uploadFileRemainingSize > 0)
                                            {
                                                continue;
                                            }
                                        }
                                    }catch(Exception ex)
                                    {
#if DEBUGPRINT
                                        Console.WriteLine("[-] upload file error: {0}",
                                                          ex.Message);
#endif
                                        Array.Clear(buffer,
                                                    0,
                                                    bufferMaxLength);

                                        result = "[-] upload file error";
                                        result += prompt;

                                        tmp = Encoding.UTF8.GetBytes(result);
                                        length = tmp.Length;
                                        for(int i = 0; i < length; i++)
                                        {
                                            buffer[i] = tmp[i];
                                        }

#if DEBUGPRINT
                                        Console.WriteLine("[+] [client <- server] SendMessage message_id:{0}",
                                                          sendMessageId);
#endif
                                        sen = SendMessage(buffer,
                                                          length,
                                                          forwarderTvSec,
                                                          forwarderTvUsec);
                                        if(sen > 0)
                                        {
                                            sendMessageId++;
                                        }

                                        if(uploadFileStream != null)
                                        {
                                            uploadFileStream.Close();
                                            uploadFileStream = null;
                                        }

                                        uploadCommandFlag = false;
                                        uploadFileFlag = false;
                                        uploadFileName = "";
                                        uploadFilePath = "";
                                        uploadFileSize = 0;
                                        recvUploadFileDataSize = 0;
                                        uploadFileRemainingSize = 0;

                                        continue;
                                    }

                                    uploadFileStream.Close();
                                    uploadFileStream = null;

                                    Array.Clear(buffer,
                                                0,
                                                bufferMaxLength);

                                    result = "[+] upload file: ";
                                    result += uploadFileName;
                                    result += prompt;

                                    tmp = Encoding.UTF8.GetBytes(result);
                                    length = tmp.Length;
                                    for(int i = 0; i < length; i++)
                                    {
                                        buffer[i] = tmp[i];
                                    }

#if DEBUGPRINT
                                    Console.WriteLine("[+] [client <- server] SendMessage message_id:{0}",
                                                      sendMessageId);
#endif
                                    sen = SendMessage(buffer,
                                                      length,
                                                      forwarderTvSec,
                                                      forwarderTvUsec);
                                   if(sen > 0)
                                   {
                                       sendMessageId++;
                                   }

                                   uploadCommandFlag = false;
                                   uploadFileFlag = false;
                                   uploadFileName = "";
                                   uploadFilePath = "";
                                   uploadFileSize = 0;
                                   recvUploadFileDataSize = 0;
                                   uploadFileRemainingSize = 0;

                                   continue;
                                }else
                                {
                                    input = buffer.Where(b => b != 0x00).ToArray();
                                    tokens = SplitInput(input);
                                    if(tokens.Length == 0)
                                    {
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
                                        Console.WriteLine("[+] [client <- server] SendMessage message_id:{0}",
                                                          sendMessageId);
#endif
                                        sen = SendMessage(buffer,
                                                          length,
                                                          forwarderTvSec,
                                                          forwarderTvUsec);
                                        if(sen > 0)
                                        {
                                            sendMessageId++;
                                        }

                                        continue;
                                    }else if(String.Compare(tokens[0].Replace("\r\n", "").Replace("\n", "").Replace("\r", ""), exitCommand) == 0)
                                    {
                                        exitFlag = true;
                                        break;
                                    }else if(String.Compare(tokens[0], cdCommand) == 0)
                                    {
                                        if(tokens.Length > 1)
                                        {
                                            try
                                            {
                                                Directory.SetCurrentDirectory(tokens[1]);

                                                Array.Clear(buffer,
                                                            0,
                                                            bufferMaxLength);

                                                tmp = Encoding.UTF8.GetBytes(prompt);
                                                length = tmp.Length;
                                                for(int i = 0; i < length; i++)
                                                {
                                                    buffer[i] = tmp[i];
                                                }
                                            }catch(Exception ex)
                                            {
#if DEBUGPRINT
                                                Console.WriteLine("[-] change directory error: {0}, {1}",
                                                                  tokens[1],
                                                                  ex.Message);
#endif
                                                result = "[-] change directory error";
                                                result += prompt;

                                                tmp = Encoding.UTF8.GetBytes(result);
                                                length = tmp.Length;
                                                for(int i = 0; i < length; i++)
                                                {
                                                    buffer[i] = tmp[i];
                                                }
                                            }
                                        }else
                                        {
                                            inputString = Encoding.UTF8.GetString(input);
                                            result = ExecuteCommand(inputString);
                                            result += prompt;

                                            tmp = Encoding.UTF8.GetBytes(result);
                                            length = tmp.Length;
                                            for(int i = 0; i < length; i++)
                                            {
                                                buffer[i] = tmp[i];
                                            }
                                        }

#if DEBUGPRINT
                                        Console.WriteLine("[+] [client <- server] SendMessage message_id:{0}",
                                                          sendMessageId);
#endif
                                        sen = SendMessage(buffer,
                                                          length,
                                                          forwarderTvSec,
                                                          forwarderTvUsec);
                                       if(sen > 0)
                                       {
                                           sendMessageId++;
                                       }

                                       continue;
                                    }else if(tokens.Length == 3 &&
                                             (String.Compare(tokens[0], downloadCommand) == 0))
                                    {
                                        downloadFileStream = null;

                                        try
                                        {
                                            downloadFileStream = new FileStream(tokens[1],
                                                                                FileMode.Open,
                                                                                FileAccess.Read);

                                            downloadFileSize = (ulong)downloadFileStream.Length;
                                            downloadFileRemainingSize = downloadFileSize;

                                            string path = Path.GetDirectoryName(tokens[1]);
                                            downloadFileName = Path.GetFileName(tokens[1]);
                                            downloadFilePath = tokens[2].Replace("\r\n", "").Replace("\n", "").Replace("\r", "");

                                            while(downloadFileRemainingSize > 0)
                                            {
                                                Array.Clear(buffer,
                                                            0,
                                                            bufferMaxLength);

                                                Array.Clear(data,
                                                            0,
                                                            bufferMaxLength);

                                                readBytes = 0;
                                                dataSize = 0;

                                                if(downloadFileRemainingSize <= SHELL_UPLOAD_DOWNLOAD_DATA_SIZE)
                                                {
                                                    readBytes = (ulong)downloadFileStream.Read(data,
                                                                                               0,
                                                                                               (int)downloadFileRemainingSize);
                                                    dataSize = downloadFileRemainingSize;
                                                }else
                                                {
                                                    readBytes = (ulong)downloadFileStream.Read(data,
                                                                                               0,
                                                                                               SHELL_UPLOAD_DOWNLOAD_DATA_SIZE);
                                                    dataSize = SHELL_UPLOAD_DOWNLOAD_DATA_SIZE;
                                                }

                                                uploadDownloadData = new UploadDownloadData(downloadCommand,
                                                                                            downloadFileName,
                                                                                            downloadFilePath,
                                                                                            downloadFileSize,
                                                                                            dataSize,
                                                                                            data);

                                                length = uploadDownloadData.CopyToBuffer(ref buffer);

#if DEBUGPRINT
                                                Console.WriteLine("[+] [client <- server] SendMessage message_id:{0}",
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

                                                                  sendMessageId++;
                                                                  downloadFileRemainingSize -= readBytes;
                                            }
                                        }catch(Exception ex)
                                        {
#if DEBUGPRINT
                                            Console.WriteLine("[-] download file error: {0}",
                                                              ex.Message);
#endif
                                            Array.Clear(buffer,
                                                        0,
                                                        bufferMaxLength);

                                            result = "[-] download file error";
                                            result += prompt;

                                            tmp = Encoding.UTF8.GetBytes(result);
                                            length = tmp.Length;
                                            for(int i = 0; i < length; i++)
                                            {
                                                buffer[i] = tmp[i];
                                            }

#if DEBUGPRINT
                                            Console.WriteLine("[+] [client <- server] SendMessage message_id:{0}",
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

                                            sendMessageId++;
                                        }

                                        continue;
                                    }else
                                    {
                                        inputString = Encoding.UTF8.GetString(input);
                                        result = ExecuteCommand(inputString);
                                        result += prompt;
                                    }

                                    Array.Clear(buffer,
                                                0,
                                                bufferMaxLength);

                                    tmp = Encoding.UTF8.GetBytes(result);
                                    resultSize = tmp.Length;
                                    if(resultSize > socks5MessageDataMaxSize - 0x10 - promptSize)
                                    {
                                        resultSize = socks5MessageDataMaxSize - 0x10 - promptSize;
                                    }

                                    for(int i = 0; i < resultSize; i++)
                                    {
                                        buffer[i] = tmp[i];
                                    }

                                    for(int i = 0; i < promptSize; i++)
                                    {
                                        buffer[resultSize + i] = promptBytes[i];
                                    }

                                    length = resultSize + promptSize;
#if DEBUGPRINT
                                    Console.WriteLine("[+] [client <- server] SendMessage message_id:{0}",
                                                      sendMessageId);
#endif
                                    sen = SendMessage(buffer,
                                                      length,
                                                      forwarderTvSec,
                                                      forwarderTvUsec);
                                    if(sen > 0)
                                    {
                                        sendMessageId++;
                                    }

                                    continue;
                                }
                            }

                            buffer = new byte[NODE_BUFFER_SIZE];
                        }
                    }else
                    {
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

            return 0;
        }

        private int ForwarderAddNode()
        {
            return 0;
        }

        private int ForwarderShowNode()
        {
            int sen = 0;

            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            byte[] data;
            int socks5MessageDataMaxSize = SOCKS5_MESSAGE_DATA_SIZE;
            recvMessageId = 0;

            string result = "";
            int resultSize = 0;


            // client
            result += clientManager.ShowClientsMapString();

            // server
            result += serverManager.ShowServersMapString();

            // pipe
            result += pipeManager.ShowPipesMapString();

            data = Encoding.UTF8.GetBytes(result);
            resultSize = data.Length;

            if(resultSize <= socks5MessageDataMaxSize)
            {
                for(int i = 0; i < resultSize; i++)
                {
                    buffer[i] = data[i];
                }
            }else
            {
                for(int i = 0; i < socks5MessageDataMaxSize; i++)
                {
                    buffer[i] = data[i];
                }

                resultSize = socks5MessageDataMaxSize;
            }

#if DEBUGPRINT
            Console.WriteLine("[+] [client <- server] SendMessage message_id:{0}",
                              sendMessageId);
#endif
            sen = SendMessage(buffer,
                              resultSize,
                              forwarderTvSec,
                              forwarderTvUsec);
            if(sen > 0)
            {
                sendMessageId++;
            }

            return 0;
        }

        private void ForwarderUdpRecvSendData()
        {
            int rec = 0;
            int sen = 0;
            int length = 0;

            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            byte[] data;
            int bufferMaxLength = NODE_BUFFER_SIZE;
            int socks5MessageDataMaxSize = SOCKS5_MESSAGE_DATA_SIZE;
            Dictionary<uint, (int, byte[])> msgsMap = new Dictionary<uint, (int, byte[])>();
            ValueTuple<int, byte[]> msg;
            recvMessageId = 0;


            while(true)
            {
                try
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] [client -> server] RecvMessage");
#endif
                    Array.Clear(buffer,
                                0,
                                bufferMaxLength);

                    rec = RecvMessage(buffer,
                                      bufferMaxLength,
                                      FORWARDER_UDP_TIMEOUT,
                                      0);
                    if(rec > 0)
                    {
                        if(recvMessageId == nextRecvMessageId)
                        {
                            length = rec;
#if DEBUGPRINT
                            Console.WriteLine("[+] [server -> target] Send message_id:{0}",
                                              nextRecvMessageId);
#endif
                            targetUdpClient.Send(buffer,
                                                 length,
                                                 targetIpEndPoint);
                            sen = length;
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
                                Console.WriteLine("[+] [server <- target] Send message_id:{0}",
                                                  nextRecvMessageId);
#endif
                                targetUdpClient.Send(buffer,
                                                     length,
                                                     targetIpEndPoint);
                                sen = length;
                                nextRecvMessageId++;
                            }

                            buffer = new byte[NODE_BUFFER_SIZE];
                        }
                    }else
                    {
                        break;
                    }


#if DEBUGPRINT
                    Console.WriteLine("[+] [server <- target] Receive");
#endif
                    Array.Clear(buffer,
                                0,
                                bufferMaxLength);

                    data = targetUdpClient.Receive(ref targetIpEndPoint);
                    rec = data.Length;

                    if(rec > socks5MessageDataMaxSize)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] Receive error: {0}",
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
                        Console.WriteLine("[+] [client <- server] SendMessage message_id:{0}",
                                          sendMessageId);
#endif
                        sen = SendMessage(buffer,
                                          rec,
                                          forwarderTvSec,
                                          forwarderTvUsec);

                        if(sen <= 0)
                        {
//                            break;
                        }

                        sendMessageId++;
                    }else
                    {
//                        break;
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
            this.targetUdpClient.Client.ReceiveTimeout = timeout;
            this.targetUdpClient.Client.SendTimeout = timeout;

            Thread thread = new Thread(new ThreadStart(ForwarderUdpRecvSendData));
            thread.Start();

            thread.Join();

            return 0;
        }

        public int DoSocks5Connection(Socks5Message socks5Message)
        {
            bool socks5ConnectFlag = true;
            bool socks5ConnectShellFlag = false;
            bool socks5ConnectAddNodeFlag = false;
            bool socks5ConnectShowNodeFlag = false;
            bool socks5ConnectUdpFlag = false;

            byte authenticationMethod = SOCKS5_AUTHENTICATION_METHOD;   // 0x0:No Authentication Required  0x2:Username/Password Authentication
            byte[] username = Encoding.UTF8.GetBytes(SOCKS5_USERNAME);
            byte[] password = Encoding.UTF8.GetBytes(SOCKS5_PASSWORD);

            int rec;
            int sen;
            int ret;
            int size;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            byte[] data;
            int bufferMaxLength = NODE_BUFFER_SIZE;

            recvMessageId = 0;
            nextRecvMessageId = 0;
            sendMessageId = GenerateRandomId();

            SelectionRequest selectionRequest;
            SelectionResponse selectionResponse;
            UsernamePasswordAuthenticationRequest usernamePasswordAuthenticationRequest;
            UsernamePasswordAuthenticationResponse usernamePasswordAuthenticationResponse;
            SocksRequestTmp socksRequestTmp;
            SocksRequestIpv4 socksRequestIpv4;
            SocksRequestDomainname socksRequestDomainname;
            SocksRequestIpv6 socksRequestIpv6;

            byte method;
            byte[] methods;
            byte ver;

            byte[] uname;
            byte[] passwd;
            byte status = 0xFF;

            byte atyp;
            byte cmd;

            IPAddress ipAddress = null;
            ushort port = 0;
            string domainname = "";

            string ipv6LinkLocalPrefix = "fe80:";



            // socks SELECTION_REQUEST [client -> server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] recv selection request");
#endif
            rec = socks5Message.DataSize;
            if(rec <= 0 || rec > NODE_BUFFER_SIZE){
#if DEBUGPRINT
                Console.WriteLine("[+] [client -> server] recv selection request error");
#endif
                return -1;
            }

            data = socks5Message.Data;
            for(int i = 0; i < rec; i++)
            {
                buffer[i] = data[i];
            }
            recvMessageId = socks5Message.MessageId;
            nextRecvMessageId = recvMessageId + 1;

            if(encryption != null &&
               encryption.Flag)
            {
                ret = encryption.Decrypt(ref buffer,
                                         rec,
                                         bufferMaxLength);
                if(ret <= 0)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] [client -> server] selection request decrypt error: {0}",
                                      ret);
#endif
                    return -1;
                }

                rec = ret;
            }

#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] recv selection request: {0} bytes",
                              rec);
#endif
            try
            {
                selectionRequest = new SelectionRequest(buffer);
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] SelectionRequest error: {0}",
                                  ex.Message);
#endif
                return -1;
            }
            method = 0xFF;
            methods = selectionRequest.Methods;
            for(int i = 0; i < selectionRequest.NMethods; i++)
            {
                if(methods[i] == authenticationMethod)  // NO AUTHENTICATION REQUIRED or USERNAME/PASSWORD
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] selection_request->methods[{0}]: {1:X2}",
                                      i,
                                      methods[i]);
#endif
                    method = methods[i];
                    break;
                }
            }
            if(method == 0xFF)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client -> server] selection request method error");
#endif
            }
            ver = selectionRequest.Ver;


            // socks SELECTION_RESPONSE [client <- server]
            Array.Clear(buffer,
                        0,
                        bufferMaxLength);
            try
            {
                selectionResponse = new SelectionResponse(0x5,
                                                          method);

                if(ver != 0x5 ||
                    authenticationMethod != method)
                {
                    selectionResponse.Method = 0xFF;
                }

                size = selectionResponse.CopyToBuffer(ref buffer);
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] selectionResponse error: {0}",
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
            Console.WriteLine("[+] [client <- server] send selection response: {0} bytes",
                              sen);
#endif

            if(authenticationMethod != method)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] authentication method error: server:0x{0:X1} client:0x{1:X1}",
                                  authenticationMethod,
                                  method);
#endif
                return -1;
            }


            // socks USERNAME_PASSWORD_AUTHENTICATION [client -> server]
            if(method == 0x2)
            {
                // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST [client -> server]
#if DEBUGPRINT
                Console.WriteLine("[+] [client -> server] recv username password authentication request");
#endif
                Array.Clear(buffer,
                            0,
                            bufferMaxLength);

                rec = RecvMessage(buffer,
                                  bufferMaxLength,
                                  tvSec,
                                  tvUsec);

                if(rec <= 0 ||
                   nextRecvMessageId != recvMessageId)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] [client -> server] recv username password authentication request error");
#endif
                    return -1;
                }

                nextRecvMessageId++;

#if DEBUGPRINT
                Console.WriteLine("[+] [client -> server] Receive username password authentication request: {0} bytes",
                                  rec);
#endif
                try
                {
                    usernamePasswordAuthenticationRequest = new UsernamePasswordAuthenticationRequest(buffer);
                }catch (Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] UsernamePasswordAuthenticationRequest error: {0}",
                                      ex.Message);
#endif
                    return -1;
                }

                uname = usernamePasswordAuthenticationRequest.UName;
                passwd = usernamePasswordAuthenticationRequest.Passwd;

#if DEBUGPRINT
                Console.WriteLine("[+] uname:{0}, ulen:{1}, passwd:{2}, plen:{3}",
                                  uname,
                                  usernamePasswordAuthenticationRequest.Ulen,
                                  passwd,
                                  usernamePasswordAuthenticationRequest.Plen);
#endif

                ver = usernamePasswordAuthenticationRequest.Ver;


                // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- server]
                Array.Clear(buffer,
                            0,
                            bufferMaxLength);

                try
                {
                    if(ver == 0x1 &&
                        uname.SequenceEqual(username) &&
                        passwd.SequenceEqual(password))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[+] succeed username password authentication");
#endif
                        status = 0x0;
                    }else
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] fail username password authentication");
#endif
                        status = 0xFF;

                    }

                    usernamePasswordAuthenticationResponse = new UsernamePasswordAuthenticationResponse(ver,
                                                                                                        status);
                    size = usernamePasswordAuthenticationResponse.CopyToBuffer(ref buffer);
                }catch (Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] selectionResponse error: {0}",
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
                Console.WriteLine("[+] [client <- server] send username password authentication response: {0} bytes",
                                  sen);
#endif

                if(status == 0xFF)
                {
                    return -1;
                }
            }


            // socks SOCKS_REQUEST [client -> server]
#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] recv socks request");
#endif
            Array.Clear(buffer,
                        0,
                        bufferMaxLength);

            rec = RecvMessage(buffer,
                              bufferMaxLength,
                              tvSec,
                              tvUsec);

            if(rec <= 0 ||
               nextRecvMessageId != recvMessageId)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client -> server] recv socks request error");
#endif
                return -1;
            }

            nextRecvMessageId++;

#if DEBUGPRINT
            Console.WriteLine("[+] [client -> server] recv socks request: {0} bytes",
                              rec);
#endif

            try
            {
                socksRequestTmp = new SocksRequestTmp(buffer);
            }catch (Exception ex)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] SocksRequest error: {0}",
                                  ex.Message);
#endif
                return -1;
            }

            atyp = socksRequestTmp.Atyp;
            if(atyp != 0x1 &&
               atyp != 0x3 &&
               atyp != 0x4 &&
               atyp != 0x0)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socks request atyp({0}) error",
                                  atyp);
                Console.WriteLine("[-] not implemented");
#endif

                // socks SOCKS_RESPONSE send error [client <- server]
                sen = SendSocksResponseIpv4(buffer,
                                            bufferMaxLength,
                                            0x5,
                                            0x8,
                                            0x0,
                                            0x1);
                return -1;
            }

            cmd = socksRequestTmp.Cmd;
            if(cmd != 0x1 &&    // CONNECT (0x1)
               cmd != 0x8 &&    // CONNECT UDP (0x8, UDP over TCP, original command)
               cmd != 0x9 &&    // SHELL (0x9, shell, original command)
               cmd != 0xa &&    // ADD NODE (0xa, add node, original command)
               cmd != 0xb)      // SHOW NODE (0xa, show node, original command)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socks request cmd({0}) error",
                                  cmd);
                Console.WriteLine("[-] not implemented");
#endif

                // socks SOCKS_RESPONSE send error [client <- server]
                if(atyp == 0x1 ||
                   atyp == 0x3)  // IPv4
                {
                    sen = SendSocksResponseIpv4(buffer,
                                                bufferMaxLength,
                                                0x5,
                                                0x7,
                                                0x0,
                                                0x1);
                }else   // Ipv6
                {
                    sen = SendSocksResponseIpv6(buffer,
                                                bufferMaxLength,
                                                0x5,
                                                0x7,
                                                0x0,
                                                0x4);
                }

                return -1;
            }

            if(atyp == 0x1)  // IPv4
            {
                try
                {
                    socksRequestIpv4 = new SocksRequestIpv4(buffer);
                }catch (Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] SocksRequestIpv4 error: {0}",
                                      ex.Message);
#endif
                    return -1;
                }

                ipAddress = new IPAddress(socksRequestIpv4.DstAddr);
                port = NetworkToHostOrderUShort(BitConverter.ToUInt16(socksRequestIpv4.DstPort, 0));

                targetIp = ipAddress.ToString();
                targetPort = port.ToString();
            }else if(atyp == 0x3)    // domain name
            {
                try
                {
                    socksRequestDomainname = new SocksRequestDomainname(buffer);
                }catch (Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] SocksRequestDomainname error: {0}",
                                      ex.Message);
#endif
                    return -1;
                }

                domainname = Encoding.ASCII.GetString(socksRequestDomainname.DstAddr);

#if DEBUGPRINT
                Console.WriteLine("[+] domainname:{0}, length:{1}",
                                  domainname,
                                  socksRequestDomainname.DstAddrLen);
#endif

                if(!domainname.Contains(":"))   // ipv4 address or domainname
                {
                    try
                    {
                        IPHostEntry hostEntry = Dns.GetHostEntry(domainname);
                        if(hostEntry.AddressList.Length > 0)
                        {
                            ipAddress = hostEntry.AddressList[0];

                            if(ipAddress.AddressFamily == AddressFamily.InterNetwork)
                            {
                                targetIp = ipAddress.ToString();
                                port = NetworkToHostOrderUShort(BitConverter.ToUInt16(socksRequestDomainname.DstPort, 0));
                                targetPort = port.ToString();
                            }
                            else if(ipAddress.AddressFamily == AddressFamily.InterNetworkV6)
                            {
                                targetIp = ipAddress.ToString();
                                if(targetIp.StartsWith(ipv6LinkLocalPrefix, StringComparison.OrdinalIgnoreCase) &&
                                   !string.IsNullOrEmpty(spiderIp.SpiderIpv6LinkLocalScopeId))
                                {
                                    targetIp = targetIp + "%" + spiderIp.SpiderIpv6LinkLocalScopeId;
                                }
                                port = NetworkToHostOrderUShort(BitConverter.ToUInt16(socksRequestDomainname.DstPort, 0));
                                targetPort = port.ToString();
                            }
                        }else
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] cannot resolv the domain name: {0}",
                                              domainname);
#endif
                            // socks SOCKS_RESPONSE send error [client <- server]
                            sen = SendSocksResponseIpv4(buffer,
                                                        bufferMaxLength,
                                                        0x5,
                                                        0x5,
                                                        0x0,
                                                        0x1);

                            return -1;
                        }
                    }catch(Exception ex)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] cannot resolv the domain name: {0} error: {1}",
                                          domainname,
                                          ex.Message);
#endif
                        // socks SOCKS_RESPONSE send error [client <- server]
                        sen = SendSocksResponseIpv4(buffer,
                                                    bufferMaxLength,
                                                    0x5,
                                                    0x5,
                                                    0x0,
                                                    0x1);

                        return -1;
                    }
                }else   // ipv6 address
                {
                    try
                    {
                        IPHostEntry hostEntry = Dns.GetHostEntry(domainname);
                        if(hostEntry.AddressList.Length > 0)
                        {
                            ipAddress = hostEntry.AddressList[0];
                            if(ipAddress.AddressFamily == AddressFamily.InterNetwork)
                            {
                                targetIp = ipAddress.ToString();
                                port = NetworkToHostOrderUShort(BitConverter.ToUInt16(socksRequestDomainname.DstPort, 0));
                                targetPort = port.ToString();
                            }else if(ipAddress.AddressFamily == AddressFamily.InterNetworkV6)
                            {
                                targetIp = ipAddress.ToString();
                                if(targetIp.StartsWith(ipv6LinkLocalPrefix, StringComparison.OrdinalIgnoreCase) &&
                                   !string.IsNullOrEmpty(spiderIp.SpiderIpv6LinkLocalScopeId))
                                {
                                    targetIp = targetIp + "%" + spiderIp.SpiderIpv6LinkLocalScopeId;
                                }
                                port = NetworkToHostOrderUShort(BitConverter.ToUInt16(socksRequestDomainname.DstPort, 0));
                                targetPort = port.ToString();
                            }
                        }else
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] cannot resolv the domain name: {0}",
                                              domainname);
#endif
                            // socks SOCKS_RESPONSE send error [client <- server]
                            sen = SendSocksResponseIpv6(buffer,
                                                        bufferMaxLength,
                                                        0x5,
                                                        0x5,
                                                        0x0,
                                                        0x4);

                            return -1;
                        }
                    }catch(Exception ex)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] cannot resolv the domain name: {0} error: {1}",
                                          domainname,
                                          ex.Message);
#endif
                        // socks SOCKS_RESPONSE send error [client <- server]
                        sen = SendSocksResponseIpv6(buffer,
                                                    bufferMaxLength,
                                                    0x5,
                                                    0x5,
                                                    0x0,
                                                    0x4);

                        return -1;
                    }
                }
            }else if(atyp == 0x4){  // Ipv6
                try
                {
                    socksRequestIpv6 = new SocksRequestIpv6(buffer);
                }catch (Exception ex)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] SocksRequestIpv6 error: {0}",
                                      ex.Message);
#endif
                    return -1;
                }

                ipAddress = new IPAddress(socksRequestIpv6.DstAddr);
                port = NetworkToHostOrderUShort(BitConverter.ToUInt16(socksRequestIpv6.DstPort, 0));

                targetIp = ipAddress.ToString();
                if(targetIp.StartsWith(ipv6LinkLocalPrefix, StringComparison.OrdinalIgnoreCase) &&
                   !string.IsNullOrEmpty(spiderIp.SpiderIpv6LinkLocalScopeId))
                {
                    targetIp = targetIp + "%" + spiderIp.SpiderIpv6LinkLocalScopeId;
                }
                targetPort = port.ToString();
            }else if((cmd != 0x9 &&
                      cmd != 0xa &&
                      cmd != 0xb) &&
                     atyp != 0x0)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] not implemented");
#endif

                // socks SOCKS_RESPONSE send error [client <- server]
                sen = SendSocksResponseIpv4(buffer,
                                            bufferMaxLength,
                                            0x5,
                                            0x1,
                                            0x0,
                                            0x1);

                return -1;
            }


            // socks SOCKS_RESPONSE [client <- server]
            if(atyp == 0x1) // IPv4
            {
                if(cmd == 0x1)  // CONNECT
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] [server -> target] connecting ip: {0} port: {1}",
                                      targetIp,
                                      targetPort);
#endif

#if DEBUGPRINT
                    Console.WriteLine("[+] socks5 response cmd: CONNECT");
#endif
                    try
                    {
                        targetTcpClient = new TcpClient(targetIp,
                                                        int.Parse(targetPort));
                        targetStream = targetTcpClient.GetStream();
                    }catch(Exception ex)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [server <- target] cannot connect error: {0}",
                                          ex.Message);
#endif
                        sen = SendSocksResponseIpv4(buffer,
                                                    bufferMaxLength,
                                                    0x5,
                                                    0x5,
                                                    0x0,
                                                    0x1);
#if DEBUGPRINT
                        Console.WriteLine("[+] [client <- server] socks request: {0} bytes, socks response: {1} bytes",
                                          rec,
                                          sen);
#endif

                        if(targetTcpClient != null)
                        {
                            targetTcpClient.Dispose();
                        }

                        return -1;
                    }

#if DEBUGPRINT
                    Console.WriteLine("[+] [server <- target] connected ip: {0} port: {1}",
                            targetIp,
                            targetPort);
#endif

                    sen = SendSocksResponseIpv4(buffer,
                                                bufferMaxLength,
                                                0x5,
                                                0x0,
                                                0x0,
                                                0x1);

#if DEBUGPRINT
                    Console.WriteLine("[+] [client <- server] socks request: {0} bytes, socks response: {1} bytes",
                                      rec,
                                      sen);
#endif
                }else if(cmd == 0x2)    // BIND
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] socks5 response cmd: BIND");
                    Console.WriteLine("[-] not implemented");
#endif

                    sen = SendSocksResponseIpv4(buffer,
                                                bufferMaxLength,
                                                0x5,
                                                0x7,
                                                0x0,
                                                0x1);

                    return -1;
                }else if(cmd == 0x3)    // UDP ASSOCIATE
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] socks5 response cmd: UDP ASSOCIATE");
                    Console.WriteLine("[-] not implemented");
#endif

                    sen = SendSocksResponseIpv4(buffer,
                                                bufferMaxLength,
                                                0x5,
                                                0x7,
                                                0x0,
                                                0x1);

                    return -1;
                }else if(cmd == 0x8)    // CONNECT UDP (0x8, UDP over TCP, original command)
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] socks5 response cmd: CONNECT UDP (0x8, UDP over TCP, original command)");
#endif

#if DEBUGPRINT
                    Console.WriteLine("[+] ip: %s port: %s",
                                      targetIp,
                                      targetPort);
#endif

                    try
                    {
                        targetUdpClient = new UdpClient();
                        targetIpEndPoint = new IPEndPoint(IPAddress.Parse(targetIp),
                                                          int.Parse(targetPort));
                    }catch(Exception ex)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [server <- target] UdpClient or IPEndPoint error: {0}",
                                          ex.Message);
#endif
                        sen = SendSocksResponseIpv4(buffer,
                                                    bufferMaxLength,
                                                    0x5,
                                                    0x5,
                                                    0x0,
                                                    0x1);
#if DEBUGPRINT
                        Console.WriteLine("[+] [client <- server] socks request: {0} bytes, socks response: {1} bytes",
                                          rec,
                                          sen);
#endif

                        if(targetUdpClient != null)
                        {
                            targetUdpClient.Dispose();
                        }

                        return -1;
                    }

                    sen = SendSocksResponseIpv4(buffer,
                                                bufferMaxLength,
                                                0x5,
                                                0x0,
                                                0x0,
                                                0x1);

#if DEBUGPRINT
                    Console.WriteLine("[+] [client <- server] socks request: {0} bytes, socks response: {1} bytes",
                                      rec,
                                      sen);
#endif

                    socks5ConnectFlag = false;
                    socks5ConnectUdpFlag = true;
                }else
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] not implemented");
#endif

                    sen = SendSocksResponseIpv4(buffer,
                                                bufferMaxLength,
                                                0x5,
                                                0x1,
                                                0x0,
                                                0x1);

                    return -1;
                }
            }else if(atyp == 0x3)   // domainname
            {
                if(ipAddress.AddressFamily == AddressFamily.InterNetwork)   // IPv4
                {
                    if(cmd == 0x1)  // CONNECT
                    {
#if DEBUGPRINT
                        Console.WriteLine("[+] [server -> target] connecting ip: {0} port: {1}",
                                          targetIp,
                                          targetPort);
#endif

#if DEBUGPRINT
                        Console.WriteLine("[+] socks5 response cmd: CONNECT");
#endif
                        try
                        {
                            targetTcpClient = new TcpClient(targetIp,
                                                            int.Parse(targetPort));
                            targetStream = targetTcpClient.GetStream();
                        }catch(Exception ex)
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] [server <- target] cannot connect error: {0}",
                                              ex.Message);
#endif
                            sen = SendSocksResponseIpv4(buffer,
                                                        bufferMaxLength,
                                                        0x5,
                                                        0x5,
                                                        0x0,
                                                        0x1);
#if DEBUGPRINT
                            Console.WriteLine("[+] [client <- server] socks request: {0} bytes, socks response: {1} bytes",
                                              rec,
                                              sen);
#endif

                            if(targetTcpClient != null)
                            {
                                targetTcpClient.Dispose();
                            }

                            return -1;
                        }

#if DEBUGPRINT
                        Console.WriteLine("[+] [server <- target] connected ip: {0} port: {1}",
                                          targetIp,
                                          targetPort);
#endif

                        sen = SendSocksResponseIpv4(buffer,
                                                    bufferMaxLength,
                                                    0x5,
                                                    0x0,
                                                    0x0,
                                                    0x1);

#if DEBUGPRINT
                        Console.WriteLine("[+] [client <- server] socks request: {0} bytes, socks response: {1} bytes",
                                          rec,
                                          sen);
#endif
                    }else if(cmd == 0x2)    // BIND
                    {
#if DEBUGPRINT
                        Console.WriteLine("[+] socks5 response cmd: BIND");
                        Console.WriteLine("[-] not implemented");
#endif

                        sen = SendSocksResponseIpv4(buffer,
                                                    bufferMaxLength,
                                                    0x5,
                                                    0x7,
                                                    0x0,
                                                    0x1);

                        return -1;
                    }else if(cmd == 0x3)    // UDP ASSOCIATE
                    {
#if DEBUGPRINT
                        Console.WriteLine("[+] socks5 response cmd: UDP ASSOCIATE");
                        Console.WriteLine("[-] not implemented");
#endif

                        sen = SendSocksResponseIpv4(buffer,
                                                    bufferMaxLength,
                                                    0x5,
                                                    0x7,
                                                    0x0,
                                                    0x1);

                        return -1;
                    }else if(cmd == 0x8)    // CONNECT UDP (0x8, UDP over TCP, original command)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[+] socks5 response cmd: CONNECT UDP (0x8, UDP over TCP, original command)");
#endif

#if DEBUGPRINT
                        Console.WriteLine("[+] ip: %s port: %s",
                                          targetIp,
                                          targetPort);
#endif

                        try
                        {
                            targetUdpClient = new UdpClient();
                            targetIpEndPoint = new IPEndPoint(IPAddress.Parse(targetIp),
                                                              int.Parse(targetPort));
                        }catch(Exception ex)
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] [server <- target] UdpClient or IPEndPoint error: {0}",
                                              ex.Message);
#endif
                            sen = SendSocksResponseIpv4(buffer,
                                                        bufferMaxLength,
                                                        0x5,
                                                        0x5,
                                                        0x0,
                                                        0x1);
#if DEBUGPRINT
                            Console.WriteLine("[+] [client <- server] socks request: {0} bytes, socks response: {1} bytes",
                                              rec,
                                              sen);
#endif

                            if(targetUdpClient != null)
                            {
                                targetUdpClient.Dispose();
                            }

                            return -1;
                        }

                        sen = SendSocksResponseIpv4(buffer,
                                                    bufferMaxLength,
                                                    0x5,
                                                    0x0,
                                                    0x0,
                                                    0x1);

#if DEBUGPRINT
                        Console.WriteLine("[+] [client <- server] socks request: {0} bytes, socks response: {1} bytes",
                                          rec,
                                          sen);
#endif

                        socks5ConnectFlag = false;
                        socks5ConnectUdpFlag = true;
                    }else
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] not implemented");
#endif

                        sen = SendSocksResponseIpv4(buffer,
                                                    bufferMaxLength,
                                                    0x5,
                                                    0x1,
                                                    0x0,
                                                    0x1);

                        return -1;
                    }
                }else if(ipAddress.AddressFamily == AddressFamily.InterNetworkV6)   // IPv6
                {
                    if(cmd == 0x1)  // CONNECT
                    {
#if DEBUGPRINT
                        Console.WriteLine("[+] [server -> target] connecting ip: {0} port: {1}",
                                          targetIp,
                                          targetPort);
#endif

#if DEBUGPRINT
                        Console.WriteLine("[+] socks5 response cmd: CONNECT");
#endif
                        try
                        {
                            targetTcpClient = new TcpClient(targetIp,
                                                            int.Parse(targetPort));
                            targetStream = targetTcpClient.GetStream();
                        }catch(Exception ex)
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] [server <- target] cannot connect error: {0}",
                                              ex.Message);
#endif
                            sen = SendSocksResponseIpv6(buffer,
                                                        bufferMaxLength,
                                                        0x5,
                                                        0x5,
                                                        0x0,
                                                        0x4);
#if DEBUGPRINT
                            Console.WriteLine("[+] [client <- server] socks request: {0} bytes, socks response: {1} bytes",
                                              rec,
                                              sen);
#endif

                            if(targetTcpClient != null)
                            {
                                targetTcpClient.Dispose();
                            }

                            return -1;
                        }

#if DEBUGPRINT
                        Console.WriteLine("[+] [server <- target] connected ip: {0} port: {1}",
                                          targetIp,
                                          targetPort);
#endif

                        sen = SendSocksResponseIpv6(buffer,
                                                    bufferMaxLength,
                                                    0x5,
                                                    0x0,
                                                    0x0,
                                                    0x4);

#if DEBUGPRINT
                        Console.WriteLine("[+] [client <- server] socks request: {0} bytes, socks response: {1} bytes",
                                          rec,
                                          sen);
#endif
                    }else if(cmd == 0x2)    // BIND
                    {
#if DEBUGPRINT
                        Console.WriteLine("[+] socks5 response cmd: BIND");
                        Console.WriteLine("[-] not implemented");
#endif

                        sen = SendSocksResponseIpv6(buffer,
                                                    bufferMaxLength,
                                                    0x5,
                                                    0x7,
                                                    0x0,
                                                    0x4);

                        return -1;
                    }else if(cmd == 0x3)    // UDP ASSOCIATE
                    {
#if DEBUGPRINT
                        Console.WriteLine("[+] socks5 response cmd: UDP ASSOCIATE");
                        Console.WriteLine("[-] not implemented");
#endif

                        sen = SendSocksResponseIpv6(buffer,
                                                    bufferMaxLength,
                                                    0x5,
                                                    0x7,
                                                    0x0,
                                                    0x4);

                        return -1;
                    }else if(cmd == 0x8)    // CONNECT UDP (0x8, UDP over TCP, original command)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[+] socks5 response cmd: CONNECT UDP (0x8, UDP over TCP, original command)");
#endif

#if DEBUGPRINT
                        Console.WriteLine("[+] ip: %s port: %s",
                                          targetIp,
                                          targetPort);
#endif

                        try
                        {
                            targetUdpClient = new UdpClient();
                            targetIpEndPoint = new IPEndPoint(IPAddress.Parse(targetIp),
                                                              int.Parse(targetPort));
                        }catch(Exception ex)
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] [server <- target] UdpClient or IPEndPoint error: {0}",
                                              ex.Message);
#endif
                            sen = SendSocksResponseIpv6(buffer,
                                                        bufferMaxLength,
                                                        0x5,
                                                        0x5,
                                                        0x0,
                                                        0x4);
#if DEBUGPRINT
                            Console.WriteLine("[+] [client <- server] socks request: {0} bytes, socks response: {1} bytes",
                                              rec,
                                              sen);
#endif

                            if(targetUdpClient != null)
                            {
                                targetUdpClient.Dispose();
                            }

                            return -1;
                        }

                        sen = SendSocksResponseIpv6(buffer,
                                                    bufferMaxLength,
                                                    0x5,
                                                    0x0,
                                                    0x0,
                                                    0x4);

#if DEBUGPRINT
                        Console.WriteLine("[+] [client <- server] socks request: {0} bytes, socks response: {1} bytes",
                                          rec,
                                          sen);
#endif

                        socks5ConnectFlag = false;
                        socks5ConnectUdpFlag = true;
                    }else
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] not implemented");
#endif

                        sen = SendSocksResponseIpv6(buffer,
                                                    bufferMaxLength,
                                                    0x5,
                                                    0x1,
                                                    0x0,
                                                    0x4);

                        return -1;
                    }
                }else
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] not implemented");
#endif

                    sen = SendSocksResponseIpv4(buffer,
                                                bufferMaxLength,
                                                0x5,
                                                0x1,
                                                0x0,
                                                0x1);

                    return -1;
                }
            }else if(atyp == 0x4)   // IPv6
            {
                if(cmd == 0x1)  // CONNECT
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] [server -> target] connecting ip: {0} port: {1}",
                                      targetIp,
                                      targetPort);
#endif

#if DEBUGPRINT
                    Console.WriteLine("[+] socks5 response cmd: CONNECT");
#endif
                    try
                    {
                        targetTcpClient = new TcpClient(targetIp,
                                                        int.Parse(targetPort));
                        targetStream = targetTcpClient.GetStream();
                    }catch(Exception ex)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [server <- target] cannot connect error: {0}",
                                          ex.Message);
#endif
                        sen = SendSocksResponseIpv6(buffer,
                                                    bufferMaxLength,
                                                    0x5,
                                                    0x5,
                                                    0x0,
                                                    0x4);
#if DEBUGPRINT
                        Console.WriteLine("[+] [client <- server] socks request: {0} bytes, socks response: {1} bytes",
                                          rec,
                                          sen);
#endif

                        if(targetTcpClient != null)
                        {
                            targetTcpClient.Dispose();
                        }

                        return -1;
                    }

#if DEBUGPRINT
                    Console.WriteLine("[+] [server <- target] connected ip: {0} port: {1}",
                                      targetIp,
                                      targetPort);
#endif

                    sen = SendSocksResponseIpv6(buffer,
                                                bufferMaxLength,
                                                0x5,
                                                0x0,
                                                0x0,
                                                0x4);

#if DEBUGPRINT
                    Console.WriteLine("[+] [client <- server] socks request: {0} bytes, socks response: {1} bytes",
                                      rec,
                                      sen);
#endif
                }else if(cmd == 0x2)    // BIND
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] socks5 response cmd: BIND");
                    Console.WriteLine("[-] not implemented");
#endif

                    sen = SendSocksResponseIpv6(buffer,
                                                bufferMaxLength,
                                                0x5,
                                                0x7,
                                                0x0,
                                                0x4);

                    return -1;
                }else if(cmd == 0x3)    // UDP ASSOCIATE
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] socks5 response cmd: UDP ASSOCIATE");
                    Console.WriteLine("[-] not implemented");
#endif

                    sen = SendSocksResponseIpv6(buffer,
                                                bufferMaxLength,
                                                0x5,
                                                0x7,
                                                0x0,
                                                0x4);

                    return -1;
                }else if(cmd == 0x8)    // CONNECT UDP (0x8, UDP over TCP, original command)
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] socks5 response cmd: CONNECT UDP (0x8, UDP over TCP, original command)");
#endif

#if DEBUGPRINT
                    Console.WriteLine("[+] ip: %s port: %s",
                                      targetIp,
                                      targetPort);
#endif

                    try
                    {
                        targetUdpClient = new UdpClient();
                        targetIpEndPoint = new IPEndPoint(IPAddress.Parse(targetIp),
                                                          int.Parse(targetPort));
                    }catch(Exception ex)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [server <- target] UdpClient or IPEndPoint error: {0}",
                                          ex.Message);
#endif
                        sen = SendSocksResponseIpv6(buffer,
                                                    bufferMaxLength,
                                                    0x5,
                                                    0x5,
                                                    0x0,
                                                    0x4);
#if DEBUGPRINT
                        Console.WriteLine("[+] [client <- server] socks request: {0} bytes, socks response: {1} bytes",
                                          rec,
                                          sen);
#endif

                        if(targetUdpClient != null)
                        {
                            targetUdpClient.Dispose();
                        }

                        return -1;
                    }

                    sen = SendSocksResponseIpv6(buffer,
                                                bufferMaxLength,
                                                0x5,
                                                0x0,
                                                0x0,
                                                0x4);

#if DEBUGPRINT
                    Console.WriteLine("[+] [client <- server] socks request: {0} bytes, socks response: {1} bytes",
                                      rec,
                                      sen);
#endif

                    socks5ConnectFlag = false;
                    socks5ConnectUdpFlag = true;
                }else
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] not implemented");
#endif

                    sen = SendSocksResponseIpv6(buffer,
                                                bufferMaxLength,
                                                0x5,
                                                0x1,
                                                0x0,
                                                0x4);

                    return -1;
                }
            }else if(cmd == 0x9 &&
                     atyp == 0x0)    // SHELL (0x9, shell, original command)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] socks5 response cmd: CONNECT SHELL (0x9, original command)");
#endif
                sen = SendSocksResponseIpv4(buffer,
                                            bufferMaxLength,
                                            0x5,
                                            0x0,
                                            0x0,
                                            0x0);

#if DEBUGPRINT
                Console.WriteLine("[+] [client <- server] socks request: {0} bytes, socks response: {1} bytes",
                                  rec,
                                  sen);
#endif

                socks5ConnectFlag = false;
                socks5ConnectShellFlag = true;
            }else if(cmd == 0xa &&
                     atyp == 0x0)    // ADD NODE (0xa, add node, original command)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] socks5 response cmd: ADD NODE (0xa, original command)");
#endif
                sen = SendSocksResponseIpv4(buffer,
                                            bufferMaxLength,
                                            0x5,
                                            0x0,
                                            0x0,
                                            0x0);

#if DEBUGPRINT
                Console.WriteLine("[+] [client <- server] socks request: {0} bytes, socks response: {1} bytes",
                                  rec,
                                  sen);
#endif

                socks5ConnectFlag = false;
                socks5ConnectAddNodeFlag = true;
            }else if(cmd == 0xb &&
                     atyp == 0x0)    // SHOW NODE (0xb, show node, original command)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] socks5 response cmd: SHOW NODE (0xb, original command)");
#endif
                sen = SendSocksResponseIpv4(buffer,
                                            bufferMaxLength,
                                            0x5,
                                            0x0,
                                            0x0,
                                            0x0);

#if DEBUGPRINT
                Console.WriteLine("[+] [client <- server] socks request: {0} bytes, socks response: {1} bytes",
                                  rec,
                                  sen);
#endif

                socks5ConnectFlag = false;
                socks5ConnectShowNodeFlag = true;
            }else
            {
#if DEBUGPRINT
                Console.WriteLine("[-] not implemented");
#endif

                sen = SendSocksResponseIpv4(buffer,
                                            bufferMaxLength,
                                            0x5,
                                            0x1,
                                            0x0,
                                            0x1);

                    return -1;
            }

            // forwarder [client <> client <> server <> target]
#if DEBUGPRINT
            Console.WriteLine("[+] [client <> client <> server <> target] forwarder");
#endif

            sendMessageId++;
            if(socks5ConnectFlag == true)
            {
                ret = Forwarder();
            }else if(socks5ConnectShellFlag == true)
            {
                ret = ForwarderShell();
            }else if(socks5ConnectAddNodeFlag == true)
            {
                ret = ForwarderAddNode();
            }else if(socks5ConnectShowNodeFlag == true)
            {
                ret = ForwarderShowNode();
            }else if(socks5ConnectUdpFlag == true)
            {
                ret = ForwarderUdp();
            }

#if DEBUGPRINT
            Console.WriteLine("[+] worker exit");
#endif

            if(targetTcpClient != null)
            {
                targetTcpClient.Dispose();
            }

            if(targetUdpClient != null)
            {
                targetUdpClient.Dispose();
            }

            return -1;
        }
    }
}

