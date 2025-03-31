#
#  Title:  spider spider.ps1 (Windows csharp powershell)
#  Author: Shuichiro Endo
#  Ver:    1.21
#

$csharpcode = @"
//#define DEBUGPRINT

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.NetworkInformation;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.Threading;

namespace spider
{
    public class SelectionRequest
    {
        private byte ver;
        private byte nmethods;
        private byte[] methods;

        public SelectionRequest(byte ver,
                                byte nmethods,
                                byte[] methods)
        {
            try
            {
                this.ver = ver;
                this.nmethods = nmethods;
                this.methods = new byte[nmethods];
                for(int i=0; i < nmethods; i++)
                {
                    this.methods[i] = methods[i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public SelectionRequest(byte[] array)
        {
            try
            {
                ver = array[0];
                nmethods = array[1];
                this.methods = new byte[nmethods];
                for(int i = 0; i < nmethods; i++)
                {
                    methods[i] = array[i + 2];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte Ver
        {
            get { return ver; }
            set { ver = value; }
        }

        public byte NMethods
        {
          get { return nmethods; }
          set { nmethods = value; }
        }

        public byte[] Methods
        {
          get { return methods; }
          set { methods = value; }
        }

        public byte[] GetByteArray()
        {
            byte[] array = new byte[2 + methods.Length];

            array[0] = ver;
            array[1] = nmethods;
            for(int i = 0; i < methods.Length; i++)
            {
                array[i + 2] = methods[i];
            }

            return array;
        }

        public int CopyToBuffer(ref byte[] buffer)
        {
            int size = 2 + methods.Length;

            try
            {
                buffer[0] = ver;
                buffer[1] = nmethods;
                for(int i = 0; i < methods.Length; i++)
                {
                    buffer[i + 2] = methods[i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }

            return size;
        }
    }

    public class SelectionResponse
    {
        public static int SELECTION_RESPONSE_SIZE = 2;

        private byte ver;
        private byte method;

        public SelectionResponse(byte ver,
                                 byte method)
        {
            this.ver = ver;
            this.method = method;
        }

        public SelectionResponse(byte[] array)
        {
            try
            {
                ver = array[0];
                method = array[0];
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }

        }

        public byte Ver
        {
            get { return ver; }
            set { ver = value; }
        }

        public byte Method
        {
            get { return method; }
            set { method = value; }
        }

        public byte[] GetByteArray()
        {
            byte[] array = new byte[2];

            array[0] = ver;
            array[1] = method;

            return array;
        }

        public int CopyToBuffer(ref byte[] buffer)
        {
            int size = 2;

            try
            {
                buffer[0] = ver;
                buffer[1] = method;
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }

            return size;
        }
    }

    public class UsernamePasswordAuthenticationRequest
    {
        private byte ver;
        private byte ulen;
        private byte[] uname;
        private byte plen;
        private byte[] passwd;

        public UsernamePasswordAuthenticationRequest(byte ver,
                                                     byte ulen,
                                                     byte[] uname,
                                                     byte plen,
                                                     byte[] passwd)
        {
            try
            {
                this.ver = ver;
                this.ulen = ulen;
                this.uname = new byte[ulen];
                for(int i = 0; i < ulen; i++)
                {
                    this.uname[i] = uname[i];
                }
                this.plen = plen;
                this.passwd = new byte[plen];
                for(int i = 0; i < ulen; i++)
                {
                    this.passwd[i] = passwd[i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public UsernamePasswordAuthenticationRequest(byte[] array)
        {
            try
            {
                ver = array[0];
                ulen = array[1];
                uname = new byte[ulen];
                for(int i = 0; i < ulen; i++)
                {
                    uname[i] = array[i + 2];
                }
                plen = array[2 + ulen];
                passwd = new byte[plen];
                for(int i = 0; i < plen; i++)
                {
                    passwd[i] = array[i + 3 + ulen];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte Ver
        {
            get { return ver; }
            set { ver = value; }
        }

        public byte Ulen
        {
            get { return ulen; }
            set { ulen = value; }
        }

        public byte[] UName
        {
            get { return uname; }
            set { uname = value; }
        }

        public byte Plen
        {
            get { return plen; }
            set { plen = value; }
        }

        public byte[] Passwd
        {
            get { return passwd; }
            set { passwd = value; }
        }

        public byte[] GetByteArray()
        {
            byte[] array = new byte[3 + ulen + plen];

            array[0] = ver;
            array[1] = ulen;
            for(int i = 0; i < ulen; i++)
            {
                array[i + 2] = uname[i];
            }
            array [2 + ulen] = plen;
            for(int i = 0; i < plen; i++)
            {
                array[3 + ulen + i] = passwd[i];
            }

            return array;
        }

        public int CopyToBuffer(ref byte[] buffer)
        {
            int size = 3 + ulen + plen;

            try
            {
                buffer[0] = ver;
                buffer[1] = ulen;
                for(int i = 0; i < ulen; i++)
                {
                    buffer[i + 2] = uname[i];
                }
                buffer [2 + ulen] = plen;
                for(int i = 0; i < plen; i++)
                {
                    buffer[3 + ulen + i] = passwd[i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }

            return size;
        }
    }

    public class UsernamePasswordAuthenticationResponse
    {
        public static int USERNAME_PASSWORD_AUTHENTICATION_RESPONSE = 2;

        private byte ver;
        private byte status;

        public UsernamePasswordAuthenticationResponse(byte ver,
                                                      byte status)
        {
            this.ver = ver;
            this.status = status;
        }

        public UsernamePasswordAuthenticationResponse(byte[] array)
        {
            try
            {
                ver = array[0];
                status = array[1];
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte Ver
        {
            get { return ver; }
            set { ver = value; }
        }

        public byte Status
        {
            get { return status; }
            set { status = value; }
        }

        public byte[] GetByteArray()
        {
            byte[] array = new byte[2];

            array[0] = ver;
            array[1] = status;

            return array;
        }

        public int CopyToBuffer(ref byte[] buffer)
        {
            int size = 2;

            try
            {
                buffer[0] = ver;
                buffer[1] = status;
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }

            return size;
        }
    }

    public class SocksRequestTmp
    {
        public static int SOCKS_REQUEST_TMP_SIZE = 4;

        private byte ver;
        private byte cmd;
        private byte rsv;
        private byte atyp;

        public SocksRequestTmp(byte ver,
                               byte cmd,
                               byte rsv,
                               byte atyp)
        {
            this.ver = ver;
            this.cmd = cmd;
            this.rsv = rsv;
            this.atyp = atyp;
        }

        public SocksRequestTmp(byte[] array)
        {
            try
            {
                ver = array[0];
                cmd = array[1];
                rsv = array[2];
                atyp = array[3];
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte Ver
        {
            get { return ver; }
            set { ver = value; }
        }

        public byte Cmd
        {
            get { return cmd; }
            set { cmd = value; }
        }

        public byte Rsv
        {
            get { return rsv; }
            set { rsv = value; }
        }

        public byte Atyp
        {
            get { return atyp; }
            set { atyp = value; }
        }

        public byte[] GetByteArray()
        {
            byte[] array = new byte[4];

            array[0] = ver;
            array[1] = cmd;
            array[2] = rsv;
            array[3] = atyp;

            return array;
        }

        public int CopyToBuffer(ref byte[] buffer)
        {
            int size = 4;

            try
            {
                buffer[0] = ver;
                buffer[1] = cmd;
                buffer[2] = rsv;
                buffer[3] = atyp;
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }

            return size;
        }
    }

    public class SocksRequestIpv4
    {
        public static int SOCKS_REQUEST_IPV4_SIZE = 10;

        private byte ver;
        private byte cmd;
        private byte rsv;
        private byte atyp;
        private byte[] dstAddr;
        private byte[] dstPort;

        public SocksRequestIpv4(byte ver,
                                byte cmd,
                                byte rsv,
                                byte atyp,
                                byte[] dstAddr,
                                byte[] dstPort)
        {
            try
            {
                this.ver = ver;
                this.cmd = cmd;
                this.rsv = rsv;
                this.atyp = atyp;
                this.dstAddr = new byte[4];
                for(int i = 0; i < 4; i++)
                {
                    this.dstAddr[i] = dstAddr[i];
                }
                this.dstPort = new byte[2];
                for(int i = 0; i < 2; i++)
                {
                    this.dstPort[i] = dstPort[i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public SocksRequestIpv4(byte[] array)
        {
            try
            {
                ver = array[0];
                cmd = array[1];
                rsv = array[2];
                atyp = array[3];
                dstAddr = new byte[4];
                for(int i = 0; i < 4; i++)
                {
                    dstAddr[i] = array[4 + i];
                }
                dstPort = new byte[2];
                for(int i = 0; i < 2; i++)
                {
                    dstPort[i] = array[8 + i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte Ver
        {
            get { return ver; }
            set { ver = value; }
        }

        public byte Cmd
        {
            get { return cmd; }
            set { cmd = value; }
        }

        public byte Rsv
        {
            get { return rsv; }
            set { rsv = value; }
        }

        public byte Atyp
        {
            get { return atyp; }
            set { atyp = value; }
        }

        public byte[] DstAddr
        {
            get { return dstAddr; }
            set { dstAddr = value; }
        }

        public byte[] DstPort
        {
            get { return dstPort; }
            set { dstPort = value; }
        }

        public byte[] GetByteArray()
        {
            byte[] array = new byte[10];

            array[0] = ver;
            array[1] = cmd;
            array[2] = rsv;
            array[3] = atyp;
            for(int i = 0; i < 4; i++)
            {
                array[4 + i] = dstAddr[i];
            }
            for(int i = 0; i < 2; i++)
            {
                array[8 + i] = dstPort[i];
            }

            return array;
        }

        public int CopyToBuffer(ref byte[] buffer)
        {
            int size = 10;

            try
            {
                buffer[0] = ver;
                buffer[1] = cmd;
                buffer[2] = rsv;
                buffer[3] = atyp;
                for(int i = 0; i < 4; i++)
                {
                    buffer[4 + i] = dstAddr[i];
                }
                for(int i = 0; i < 2; i++)
                {
                    buffer[8 + i] = dstPort[i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }

            return size;
        }
    }

    public class SocksRequestDomainname
    {
        private byte ver;
        private byte cmd;
        private byte rsv;
        private byte atyp;
        private byte dstAddrLen;
        private byte[] dstAddr;
        private byte[] dstPort; // network byte order

        public SocksRequestDomainname(byte ver,
                                      byte cmd,
                                      byte rsv,
                                      byte atyp,
                                      byte dstAddrLen,
                                      byte[] dstAddr,
                                      byte[] dstPort)
        {
            try
            {
                this.ver = ver;
                this.cmd = cmd;
                this.rsv = rsv;
                this.atyp = atyp;
                this.dstAddrLen = dstAddrLen;
                this.dstAddr = new byte[dstAddrLen];
                for(int i = 0; i < dstAddrLen; i++)
                {
                    this.dstAddr[i] = dstAddr[i];
                }

                this.dstPort = new byte[2];
                for(int i = 0; i < 2; i++)
                {
                    this.dstPort[i] = dstPort[i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public SocksRequestDomainname(byte ver,
                                      byte cmd,
                                      byte rsv,
                                      byte atyp,
                                      string ip,
                                      string port)
        {
            try
            {
                this.ver = ver;
                this.cmd = cmd;
                this.rsv = rsv;
                this.atyp = atyp;
                this.dstAddr = Encoding.UTF8.GetBytes(ip);
                this.dstAddrLen = (byte)this.dstAddr.Length;
                this.dstPort = BitConverter.GetBytes(HostToNetworkOrderUShort(ushort.Parse(port)));
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public SocksRequestDomainname(byte[] array)
        {
            try
            {
                ver = array[0];
                cmd = array[1];
                rsv = array[2];
                atyp = array[3];
                dstAddrLen = array[4];
                dstAddr = new byte[dstAddrLen];
                for(int i = 0; i < dstAddrLen; i++)
                {
                    dstAddr[i] = array[5 + i];
                }
                dstPort = new byte[2];
                for(int i = 0; i < 2; i++)
                {
                    dstPort[i] = array[5 + dstAddrLen + i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte Ver
        {
            get { return ver; }
            set { ver = value; }
        }

        public byte Cmd
        {
            get { return cmd; }
            set { cmd = value; }
        }

        public byte Rsv
        {
            get { return rsv; }
            set { rsv = value; }
        }

        public byte Atyp
        {
            get { return atyp; }
            set { atyp = value; }
        }

        public byte DstAddrLen
        {
            get { return dstAddrLen; }
            set { dstAddrLen = value; }
        }

        public byte[] DstAddr
        {
            get { return dstAddr; }
            set { dstAddr = value; }
        }

        public byte[] DstPort
        {
            get { return dstPort; }
            set { dstPort = value; }
        }

        public ushort NetworkToHostOrderUShort(ushort value)
        {
            if(BitConverter.IsLittleEndian)
            {
                byte[] bytes = BitConverter.GetBytes(value);
                Array.Reverse(bytes);
                return BitConverter.ToUInt16(bytes, 0);
            }

            return value;
        }

        public ushort HostToNetworkOrderUShort(ushort value)
        {
            if(BitConverter.IsLittleEndian)
            {
                byte[] bytes = BitConverter.GetBytes(value);
                Array.Reverse(bytes);
                return BitConverter.ToUInt16(bytes, 0);
            }

            return value;
        }

        public byte[] GetByteArray()
        {
            byte[] array = new byte[7 + dstAddrLen];

            array[0] = ver;
            array[1] = cmd;
            array[2] = rsv;
            array[3] = atyp;
            array[4] = dstAddrLen;
            for(int i = 0; i < dstAddrLen; i++)
            {
                array[5 + i] = dstAddr[i];
            }
            for(int i = 0; i < 2; i++)
            {
                array[5 + dstAddrLen + i] = dstPort[i];
            }

            return array;
        }

        public int CopyToBuffer(ref byte[] buffer)
        {
            int size = 7 + dstAddrLen;

            try
            {
                buffer[0] = ver;
                buffer[1] = cmd;
                buffer[2] = rsv;
                buffer[3] = atyp;
                buffer[4] = dstAddrLen;
                for(int i = 0; i < dstAddrLen; i++)
                {
                    buffer[5 + i] = dstAddr[i];
                }
                for(int i = 0; i < 2; i++)
                {
                    buffer[5 + dstAddrLen + i] = dstPort[i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }

            return size;
        }
    }

    public class SocksRequestIpv6
    {
        public static int SOCKS_REQUEST_IPV6_SIZE = 12;

        private byte ver;
        private byte cmd;
        private byte rsv;
        private byte atyp;
        private byte[] dstAddr;
        private byte[] dstPort;

        public SocksRequestIpv6(byte ver,
                                byte cmd,
                                byte rsv,
                                byte atyp,
                                byte[] dstAddr,
                                byte[] dstPort)
        {
            try
            {
                this.ver = ver;
                this.cmd = cmd;
                this.rsv = rsv;
                this.atyp = atyp;
                this.dstAddr = new byte[16];
                for(int i = 0; i < 16; i++)
                {
                    this.dstAddr[i] = dstAddr[i];
                }

                this.dstPort = new byte[2];
                for(int i = 0; i < 2; i++)
                {
                    this.dstPort[i] = dstPort[i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public SocksRequestIpv6(byte[] array)
        {
            try
            {
                ver = array[0];
                cmd = array[1];
                rsv = array[2];
                atyp = array[3];
                dstAddr = new byte[16];
                for(int i = 0; i < 16; i++)
                {
                    dstAddr[i] = array[4 + i];
                }
                dstPort = new byte[2];
                for(int i = 0; i < 2; i++)
                {
                    dstPort[i] = array[20 + i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte Ver
        {
            get { return ver; }
            set { ver = value; }
        }

        public byte Cmd
        {
            get { return cmd; }
            set { cmd = value; }
        }

        public byte Rsv
        {
            get { return rsv; }
            set { rsv = value; }
        }

        public byte Atyp
        {
            get { return atyp; }
            set { atyp = value; }
        }

        public byte[] DstAddr
        {
            get { return dstAddr; }
            set { dstAddr = value; }
        }

        public byte[] DstPort
        {
            get { return dstPort; }
            set { dstPort = value; }
        }

        public byte[] GetByteArray()
        {
            byte[] array = new byte[22];

            array[0] = ver;
            array[1] = cmd;
            array[2] = rsv;
            array[3] = atyp;
            for(int i = 0; i < 16; i++)
            {
                array[4 + i] = dstAddr[i];
            }
            for(int i = 0; i < 2; i++)
            {
                array[20 + i] = dstPort[i];
            }

            return array;
        }

        public int CopyToBuffer(ref byte[] buffer)
        {
            int size = 22;

            try
            {
                buffer[0] = ver;
                buffer[1] = cmd;
                buffer[2] = rsv;
                buffer[3] = atyp;
                for(int i = 0; i < 16; i++)
                {
                    buffer[4 + i] = dstAddr[i];
                }
                for(int i = 0; i < 2; i++)
                {
                    buffer[20 + i] = dstPort[i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }

            return size;
        }
    }

    public class SocksResponseTmp
    {
        public static int SOCKS_RESPONSE_TMP_SIZE = 4;

        private byte ver;
        private byte rep;
        private byte rsv;
        private byte atyp;

        public SocksResponseTmp(byte ver,
                                byte rep,
                                byte rsv,
                                byte atyp)
        {
            this.ver = ver;
            this.rep = rep;
            this.rsv = rsv;
            this.atyp = atyp;
        }

        public SocksResponseTmp(byte[] array)
        {
            try
            {
                ver = array[0];
                rep = array[1];
                rsv = array[2];
                atyp = array[3];
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte Ver
        {
            get { return ver; }
            set { ver = value; }
        }

        public byte Rep
        {
            get { return rep; }
            set { rep = value; }
        }

        public byte Rsv
        {
            get { return rsv; }
            set { rsv = value; }
        }

        public byte Atyp
        {
            get { return atyp; }
            set { atyp = value; }
        }

        public byte[] GetByteArray()
        {
            byte[] array = new byte[4];

            array[0] = ver;
            array[1] = rep;
            array[2] = rsv;
            array[3] = atyp;

            return array;
        }

        public int CopyToBuffer(ref byte[] buffer)
        {
            int size = 4;

            try
            {
                buffer[0] = ver;
                buffer[1] = rep;
                buffer[2] = rsv;
                buffer[3] = atyp;
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }

            return size;
        }
    }

    public class SocksResponseIpv4
    {
        public static int SOCKS_RESPONSE_IPV4_SIZE = 10;

        private byte ver;
        private byte rep;
        private byte rsv;
        private byte atyp;
        private byte[] bndAddr;
        private byte[] bndPort;

        public SocksResponseIpv4(byte ver,
                                 byte rep,
                                 byte rsv,
                                 byte atyp,
                                 byte[] bndAddr,
                                 byte[] bndPort)
        {
            try
            {
                this.ver = ver;
                this.rep = rep;
                this.rsv = rsv;
                this.atyp = atyp;
                this.bndAddr = new byte[4];
                for(int i = 0; i < 4; i++)
                {
                    this.bndAddr[i] = bndAddr[i];
                }
                this.bndPort = new byte[2];
                for(int i = 0; i < 2; i++)
                {
                    this.bndPort[i] = bndPort[i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public SocksResponseIpv4(byte[] array)
        {
            try
            {
                ver = array[0];
                rep = array[1];
                rsv = array[2];
                atyp = array[3];
                bndAddr = new byte[4];
                for(int i = 0; i < 4; i++)
                {
                    bndAddr[i] = array[4 + i];
                }
                bndPort = new byte[2];
                for(int i = 0; i < 2; i++)
                {
                    bndPort[i] = array[8 + i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte Ver
        {
            get { return ver; }
            set { ver = value; }
        }

        public byte Rep
        {
            get { return rep; }
            set { rep = value; }
        }

        public byte Rsv
        {
            get { return rsv; }
            set { rsv = value; }
        }

        public byte Atyp
        {
            get { return atyp; }
            set { atyp = value; }
        }

        public byte[] BndAddr
        {
            get { return bndAddr; }
            set { bndAddr = value; }
        }

        public byte[] BndPort
        {
            get { return bndPort; }
            set { bndPort = value; }
        }

        public byte[] GetByteArray()
        {
            byte[] array = new byte[10];

            array[0] = ver;
            array[1] = rep;
            array[2] = rsv;
            array[3] = atyp;
            for(int i = 0; i < 4; i++)
            {
                array[4 + i] = bndAddr[i];
            }
            for(int i = 0; i < 2; i++)
            {
                array[8 + i] = bndPort[i];
            }

            return array;
        }

        public int CopyToBuffer(ref byte[] buffer)
        {
            int size = 10;

            try
            {
                buffer[0] = ver;
                buffer[1] = rep;
                buffer[2] = rsv;
                buffer[3] = atyp;
                for(int i = 0; i < 4; i++)
                {
                    buffer[4 + i] = bndAddr[i];
                }
                for(int i = 0; i < 2; i++)
                {
                    buffer[8 + i] = bndPort[i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }

            return size;
        }
    }

    public class SocksResposeDomainname
    {
        private byte ver;
        private byte rep;
        private byte rsv;
        private byte atyp;
        private byte bndAddrLen;
        private byte[] bndAddr;
        private byte[] bndPort;

        public SocksResposeDomainname(byte ver,
                                      byte rep,
                                      byte rsv,
                                      byte atyp,
                                      byte bndAddrLen,
                                      byte[] bndAddr,
                                      byte[] bndPort)
        {
            try
            {
                this.ver = ver;
                this.rep = rep;
                this.rsv = rsv;
                this.atyp = atyp;
                this.bndAddrLen = bndAddrLen;
                this.bndAddr = new byte[bndAddrLen];
                for(int i = 0; i < bndAddrLen; i++)
                {
                    this.bndAddr[i] = bndAddr[i];
                }

                this.bndPort = new byte[2];
                for(int i = 0; i < 2; i++)
                {
                    this.bndPort[i] = bndPort[i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public SocksResposeDomainname(byte[] array)
        {
            try
            {
                ver = array[0];
                rep = array[1];
                rsv = array[2];
                atyp = array[3];
                bndAddrLen = array[4];
                bndAddr = new byte[bndAddrLen];
                for(int i = 0; i < bndAddrLen; i++)
                {
                    bndAddr[i] = array[5 + i];
                }
                bndPort = new byte[2];
                for(int i = 0; i < 2; i++)
                {
                    bndPort[i] = array[5 + bndAddrLen + i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte Ver
        {
            get { return ver; }
            set { ver = value; }
        }

        public byte Rep
        {
            get { return rep; }
            set { rep = value; }
        }

        public byte Rsv
        {
            get { return rsv; }
            set { rsv = value; }
        }

        public byte Atyp
        {
            get { return atyp; }
            set { atyp = value; }
        }

        public byte BndAddrLen
        {
            get { return bndAddrLen; }
            set { bndAddrLen = value; }
        }

        public byte[] BndAddr
        {
            get { return bndAddr; }
            set { bndAddr = value; }
        }

        public byte[] BndPort
        {
            get { return bndPort; }
            set { bndPort = value; }
        }

        public byte[] GetByteArray()
        {
            byte[] array = new byte[7 + bndAddrLen];

            array[0] = ver;
            array[1] = rep;
            array[2] = rsv;
            array[3] = atyp;
            array[4] = bndAddrLen;
            for(int i = 0; i < bndAddrLen; i++)
            {
                array[5 + i] = bndAddr[i];
            }
            for(int i = 0; i < 2; i++)
            {
                array[5 + bndAddrLen + i] = bndPort[i];
            }

            return array;
        }

        public int CopyToBuffer(ref byte[] buffer)
        {
            int size = 7 + bndAddrLen;

            try
            {
                buffer[0] = ver;
                buffer[1] = rep;
                buffer[2] = rsv;
                buffer[3] = atyp;
                buffer[4] = bndAddrLen;
                for(int i = 0; i < bndAddrLen; i++)
                {
                    buffer[5 + i] = bndAddr[i];
                }
                for(int i = 0; i < 2; i++)
                {
                    buffer[5 + bndAddrLen + i] = bndPort[i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }

            return size;
        }
    }

    public class SocksResponseIpv6
    {
        public static int SOCKS_RESPONSE_IPV6_SIZE = 22;

        private byte ver;
        private byte rep;
        private byte rsv;
        private byte atyp;
        private byte[] bndAddr;
        private byte[] bndPort;

        public SocksResponseIpv6(byte ver,
                                 byte rep,
                                 byte rsv,
                                 byte atyp,
                                 byte[] bndAddr,
                                 byte[] bndPort)
        {
            try
            {
                this.ver = ver;
                this.rep = rep;
                this.rsv = rsv;
                this.atyp = atyp;
                this.bndAddr = new byte[16];
                for(int i = 0; i < 16; i++)
                {
                    this.bndAddr[i] = bndAddr[i];
                }
                this.bndPort = new byte[2];
                for(int i = 0; i < 2; i++)
                {
                    this.bndPort[i] = bndPort[i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public SocksResponseIpv6(byte[] array)
        {
            try
            {
                ver = array[0];
                rep = array[1];
                rsv = array[2];
                atyp = array[3];
                bndAddr = new byte[16];
                for(int i = 0; i < 16; i++)
                {
                    bndAddr[i] = array[4 + i];
                }
                bndPort = new byte[2];
                for(int i = 0; i < 2; i++)
                {
                    bndPort[i] = array[20 + i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte Ver
        {
            get { return ver; }
            set { ver = value; }
        }

        public byte Rep
        {
            get { return rep; }
            set { rep = value; }
        }

        public byte Rsv
        {
            get { return rsv; }
            set { rsv = value; }
        }

        public byte Atyp
        {
            get { return atyp; }
            set { atyp = value; }
        }

        public byte[] BndAddr
        {
            get { return bndAddr; }
            set { bndAddr = value; }
        }

        public byte[] BndPort
        {
            get { return bndPort; }
            set { bndPort = value; }
        }

        public byte[] GetByteArray()
        {
            byte[] array = new byte[22];

            array[0] = ver;
            array[1] = rep;
            array[2] = rsv;
            array[3] = atyp;
            for(int i = 0; i < 16; i++)
            {
                array[4 + i] = bndAddr[i];
            }
            for(int i = 0; i < 2; i++)
            {
                array[20 + i] = bndPort[i];
            }

            return array;
        }

        public int CopyToBuffer(ref byte[] buffer)
        {
            int size = 22;

            try
            {
                buffer[0] = ver;
                buffer[1] = rep;
                buffer[2] = rsv;
                buffer[3] = atyp;
                for(int i = 0; i < 16; i++)
                {
                    buffer[4 + i] = bndAddr[i];
                }
                for(int i = 0; i < 2; i++)
                {
                    buffer[20 + i] = bndPort[i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }

            return size;
        }
    }

    public class RouteData
    {
        public static int ROUTE_DATA_SIZE = 48;
        const int INET6_ADDR_STRING_LENGTH = 46;

        byte[] ip = new byte[INET6_ADDR_STRING_LENGTH + 1];
        byte metric;


        public RouteData(byte[] ip,
                         byte metric)
        {
            try
            {
                for(int i = 0; i < INET6_ADDR_STRING_LENGTH + 1; i++)
                {
                    this.ip[i] = ip[i];
                }
                this.metric = metric;
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public RouteData(byte[] array)
        {
            try
            {
                for(int i = 0; i < INET6_ADDR_STRING_LENGTH + 1; i++)
                {
                    ip[i] = array[i];
                }
                metric = array[INET6_ADDR_STRING_LENGTH + 2];
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte[] Ip
        {
            get { return ip; }
            set { ip = value; }
        }

        public byte Metric
        {
            get { return metric; }
            set { metric = value;}
        }
    }

    public class RoutingMessageDataHeader
    {
        public static int ROUTING_MESSAGE_DATA_HEADER_SIZE = 4;

        byte messageType;
        byte reserved;
        ushort dataSize;    // network byte order

        public RoutingMessageDataHeader(byte[] array)
        {
            try
            {
                messageType = array[0];
                reserved = array[1];
                dataSize = BitConverter.ToUInt16(array, 2);     // network byte order
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte MessageType
        {
            get { return messageType; }
            set { messageType = value; }
        }

        public byte Reserved
        {
            get { return reserved; }
            set { reserved = value; }
        }

        public ushort DataSize
        {
            get { return dataSize; }
            set { dataSize = value; }
        }
    }

    public class RoutingMessageData
    {
        const int ROUTING_MESSAGE_DATA_SIZE = 60000;

        byte messageType;
        byte reserved;
        ushort dataSize;    // host byte order
        byte[] data;

        public RoutingMessageData(byte messageType,
                                  byte reserved,
                                  ushort dataSize,
                                  byte[] data)
        {
            this.messageType = messageType;
            this.reserved = reserved;
            this.dataSize = dataSize;
            if(dataSize <= ROUTING_MESSAGE_DATA_SIZE)
            {
                this.data = new byte[dataSize];
                for(int i = 0; i < dataSize; i++)
                {
                    this.data[i] = data[i];
                }
            }else
            {
                this.data = new byte[ROUTING_MESSAGE_DATA_SIZE];
                for(int i = 0; i < ROUTING_MESSAGE_DATA_SIZE; i++)
                {
                    this.data[i] = data[i];
                }
            }
        }

        public RoutingMessageData(byte[] array)
        {
            try
            {
                messageType = array[0];
                reserved = array[1];
                dataSize = NetworkToHostOrderUShort(BitConverter.ToUInt16(array, 2));   // host byte order
                if(dataSize <= ROUTING_MESSAGE_DATA_SIZE)
                {
                    this.data = new byte[dataSize];
                    for(int i = 0; i < dataSize; i++)
                    {
                        this.data[i] = array[4 + i];
                    }
                }else
                {
                    this.data = new byte[ROUTING_MESSAGE_DATA_SIZE];
                    for(int i = 0; i < ROUTING_MESSAGE_DATA_SIZE; i++)
                    {
                        this.data[i] = array[4 + i];
                    }
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte MessageType
        {
            get { return messageType; }
            set { messageType = value; }
        }

        public byte Reserved
        {
            get { return reserved; }
            set { reserved = value; }
        }

        public ushort DataSize
        {
            get { return dataSize; }
            set { dataSize = value; }
        }

        public byte[] Data
        {
            get { return data; }
            set { data = value; }
        }

        public ushort NetworkToHostOrderUShort(ushort value)
        {
            if(BitConverter.IsLittleEndian)
            {
                byte[] bytes = BitConverter.GetBytes(value);
                Array.Reverse(bytes);
                return BitConverter.ToUInt16(bytes, 0);
            }

            return value;
        }
    }

    public class Socks5MessageDataHeader
    {
        public static int SOCKS5_MESSAGE_DATA_HEADER_SIZE = 144;
        const int INET6_ADDR_STRING_LENGTH = 46;

        byte messageType;
//      byte reserved1;
//      byte reserved2;
//      byte reserved3;
        uint messageId;         // network byte order
        uint connectionId;      // network byte order
        uint clientId;          // network byte order
        uint serverId;          // network byte order
        byte sourceNodeType;
//      byte reserved4;
        byte[] sourceIp = new byte[INET6_ADDR_STRING_LENGTH + 1];
//      byte reserved5;
        byte destinationNodeType;
//      byte reserved6;
        byte[] destinationIp = new byte[INET6_ADDR_STRING_LENGTH + 1];
//      byte reserved7;
        int tvSec;              // network byte order
        int tvUsec;             // network byte order
        int forwarderTvSec;     // network byte order
        int forwarderTvUsec;    // network byte order
        ushort dataSize;        // network byte order
//      byte reserved8;
//      byte reserved9;
//      byte reserved10;
//      byte reserved11;
//      byte reserved12;
//      byte reserved13;

        public Socks5MessageDataHeader(byte[] array)
        {
            try
            {
                messageType = array[0];
                messageId = BitConverter.ToUInt32(array, 4);
                connectionId = BitConverter.ToUInt32(array, 8);
                clientId = BitConverter.ToUInt32(array, 12);
                serverId = BitConverter.ToUInt32(array, 16);
                sourceNodeType = array[20];
                for(int i = 0; i < INET6_ADDR_STRING_LENGTH + 1; i++)
                {
                    sourceIp[i] = array[22 + i];
                }
                destinationNodeType = array[70];
                for(int i = 0; i < INET6_ADDR_STRING_LENGTH + 1; i++)
                {
                    destinationIp[i] = array[72 + i];
                }
                tvSec = BitConverter.ToInt32(array, 120);
                tvUsec = BitConverter.ToInt32(array, 124);
                forwarderTvSec = BitConverter.ToInt32(array, 128);
                forwarderTvUsec = BitConverter.ToInt32(array, 132);
                dataSize = BitConverter.ToUInt16(array, 136);

            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte MessageType
        {
            get { return messageType; }
            set { messageType = value; }
        }

        public uint MessageId
        {
            get { return messageId; }
            set { messageId = value; }
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

        public byte SourceNodeType
        {
            get { return sourceNodeType; }
            set { sourceNodeType = value; }
        }

        public byte[] SourceIp
        {
            get { return sourceIp; }
            set { sourceIp = value; }
        }

        public byte DestinationNodeType
        {
            get { return destinationNodeType; }
            set { destinationNodeType = value; }
        }

        public byte[] DestinationIp
        {
            get { return destinationIp; }
            set { destinationIp = value; }
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

        public ushort DataSize
        {
            get { return dataSize; }
            set { dataSize = value; }
        }
    }

    public class Socks5MessageData
    {
        const int SOCKS5_MESSAGE_DATA_SIZE = 60000;
        const int INET6_ADDR_STRING_LENGTH = 46;

        byte messageType;
//      byte reserved1;
//      byte reserved2;
//      byte reserved3;
        uint messageId;         // host byte order
        uint connectionId;      // host byte order
        uint clientId;          // host byte order
        uint serverId;          // host byte order
        byte sourceNodeType;
//      byte reserved4;
        byte[] sourceIp = new byte[INET6_ADDR_STRING_LENGTH + 1];
//      byte reserved5;
        byte destinationNodeType;
//      byte reserved6;
        byte[] destinationIp = new byte[INET6_ADDR_STRING_LENGTH + 1];
//      byte reserved7;
        int tvSec;              // host byte order
        int tvUsec;             // host byte order
        int forwarderTvSec;     // host byte order
        int forwarderTvUsec;    // host byte order
        ushort dataSize;        // host byte order
//      byte reserved8;
//      byte reserved9;
//      byte reserved10;
//      byte reserved11;
//      byte reserved12;
//      byte reserved13;
        byte[] data;

        public Socks5MessageData(byte[] array)
        {
            try
            {
                messageType = array[0];
                messageId = (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(array, 4));
                connectionId = (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(array, 8));
                clientId = (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(array, 12));
                serverId = (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(array, 16));
                sourceNodeType = array[20];
                for(int i = 0; i < INET6_ADDR_STRING_LENGTH + 1; i++)
                {
                    sourceIp[i] = array[22 + i];
                }
                destinationNodeType = array[70];
                for(int i = 0; i < INET6_ADDR_STRING_LENGTH + 1; i++)
                {
                    destinationIp[i] = array[72 + i];
                }
                tvSec = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(array, 120));
                tvUsec = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(array, 124));
                forwarderTvSec = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(array, 128));
                forwarderTvUsec = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(array, 132));
                dataSize = NetworkToHostOrderUShort(BitConverter.ToUInt16(array, 136));
                if(dataSize <= SOCKS5_MESSAGE_DATA_SIZE)
                {
                    this.data = new byte[dataSize];
                    for(int i = 0; i < dataSize; i++)
                    {
                        this.data[i] = array[144 + i];
                    }
                }else
                {
                    this.data = new byte[SOCKS5_MESSAGE_DATA_SIZE];
                    for(int i = 0; i < SOCKS5_MESSAGE_DATA_SIZE; i++)
                    {
                        this.data[i] = array[144 + i];
                    }
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte MessageType
        {
            get { return messageType; }
            set { messageType = value; }
        }

        public uint MessageId
        {
            get { return messageId; }
            set { messageId = value; }
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

        public byte SourceNodeType
        {
            get { return sourceNodeType; }
            set { sourceNodeType = value; }
        }

        public byte[] SourceIp
        {
            get { return sourceIp; }
            set { sourceIp = value; }
        }

        public byte DestinationNodeType
        {
            get { return destinationNodeType; }
            set { destinationNodeType = value; }
        }

        public byte[] DestinationIp
        {
            get { return destinationIp; }
            set { destinationIp = value; }
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

        public ushort DataSize
        {
            get { return dataSize; }
            set { dataSize = value; }
        }

        public byte[] Data
        {
            get { return data; }
            set { data = value; }
        }

        public ushort NetworkToHostOrderUShort(ushort value)
        {
            if(BitConverter.IsLittleEndian)
            {
                byte[] bytes = BitConverter.GetBytes(value);
                Array.Reverse(bytes);
                return BitConverter.ToUInt16(bytes, 0);
            }

            return value;
        }
    }

    public class UploadDownloadDataHeader
    {
        public static int UPLOAD_DOWNLOAD_DATA_HEADER_SIZE = 544;

        byte[] command = new byte[16];
        byte[] fileName = new byte[256];
        byte[] filePath = new byte[256];
        ulong fileSize;
        ulong dataSize;

        public UploadDownloadDataHeader(byte[] command,
                                        byte[] fileName,
                                        byte[] filePath,
                                        ulong fileSize,
                                        ulong dataSize)
        {
            try
            {
                for(int i = 0; i < 16; i++)
                {
                    this.command[i] = command[i];
                }
                for(int i = 0; i < 256; i++)
                {
                    this.fileName[i] = fileName[i];
                }
                for(int i = 0; i < 256; i++)
                {
                    this.filePath[i] = filePath[i];
                }
                this.fileSize = fileSize;
                this.dataSize = dataSize;
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public UploadDownloadDataHeader(byte[] array)
        {
            int index = 0;

            try
            {
                for(int i = 0; i < 16; i++)
                {
                    this.command[i] = array[i];
                }
                index += 16;

                for(int i = 0; i < 256; i++)
                {
                    this.fileName[i] = array[index + i];
                }
                index += 256;

                for(int i = 0; i < 256; i++)
                {
                    this.filePath[i] = array[index + i];
                }
                index += 256;

                this.fileSize = (ulong)IPAddress.NetworkToHostOrder((long)BitConverter.ToUInt64(array, index));
                index += 8;

                this.dataSize = (ulong)IPAddress.NetworkToHostOrder((long)BitConverter.ToUInt64(array, index));
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte[] Command
        {
            get { return command; }
            set { command = value; }
        }

        public byte[] FileName
        {
            get { return fileName; }
            set { fileName = value; }
        }

        public byte[] FilePath
        {
            get { return filePath; }
            set { filePath = value; }
        }

        public ulong FileSize
        {
            get { return fileSize; }
            set { fileSize = value; }
        }

        public ulong DataSize
        {
            get {return dataSize; }
            set { dataSize = value; }
        }

        public int CopyToBuffer(ref byte[] buffer)
        {
            int index = 0;
            byte[] tmp;

            try
            {
                for(int i = 0; i < 16; i++)
                {
                    buffer[i] = command[i];
                }
                index += 16;

                for(int i = 0; i < 256; i++)
                {
                    buffer[index + i] = fileName[i];
                }
                index += 256;

                for(int i = 0; i < 256; i++)
                {
                    buffer[index + i] = filePath[i];
                }
                index += 256;

                tmp = BitConverter.GetBytes((ulong)IPAddress.HostToNetworkOrder((long)fileSize));
                for(int i = 0; i < 8; i++)
                {
                    buffer[index + i] = tmp[i];
                }
                index += 8;

                tmp = BitConverter.GetBytes((ulong)IPAddress.HostToNetworkOrder((long)dataSize));
                for(int i = 0; i < 8; i++)
                {
                    buffer[index + i] = tmp[i];
                }
                index += 8;
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }

            return index;
        }
    }

    public class UploadDownloadData
    {
        const int SHELL_UPLOAD_DOWNLOAD_DATA_SIZE = 50000;

        byte[] command = new byte[16];
        byte[] fileName = new byte[256];
        byte[] filePath = new byte[256];
        ulong fileSize;
        ulong dataSize;
        byte[] data = new byte[SHELL_UPLOAD_DOWNLOAD_DATA_SIZE];

        public UploadDownloadData(string command,
                                  string fileName,
                                  string filePath,
                                  ulong fileSize,
                                  ulong dataSize,
                                  byte[] data)
        {
            byte[] tmp;

            try
            {
                tmp = Encoding.UTF8.GetBytes(command);
                if(tmp.Length <= 16)
                {
                    for(int i = 0; i < tmp.Length; i++)
                    {
                        this.command[i] = tmp[i];
                    }
                }else
                {
                    for(int i = 0; i < 16; i++)
                    {
                        this.command[i] = tmp[i];
                    }
                }

                tmp = Encoding.UTF8.GetBytes(fileName);
                if(tmp.Length <= 256)
                {
                    for(int i = 0; i < tmp.Length; i++)
                    {
                        this.fileName[i] = tmp[i];
                    }
                }else
                {
                    for(int i = 0; i < 256; i++)
                    {
                        this.fileName[i] = tmp[i];
                    }
                }

                tmp = Encoding.UTF8.GetBytes(filePath);
                if(tmp.Length <= 256)
                {
                    for(int i = 0; i < tmp.Length; i++)
                    {
                        this.filePath[i] = tmp[i];
                    }
                }else
                {
                    for(int i = 0; i < 256; i++)
                    {
                        this.filePath[i] = tmp[i];
                    }
                }

                this.fileSize = fileSize;
                this.dataSize = dataSize;
                for(ulong i = 0; i < dataSize; i++)
                {
                    this.data[i] = data[i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public UploadDownloadData(byte[] array)
        {
            try
            {
                int index = 0;

                for(int i = 0; i < 16; i++)
                {
                    this.command[i] = array[i];
                }
                index += 16;

                for(int i = 0; i < 256; i++)
                {
                    this.fileName[i] = array[index + i];
                }
                index += 256;

                for(int i = 0; i < 256; i++)
                {
                    this.filePath[i] = array[index + i];
                }
                index += 256;

                this.fileSize = (ulong)IPAddress.NetworkToHostOrder((long)BitConverter.ToUInt64(array, index));
                index += 8;

                this.dataSize = (ulong)IPAddress.NetworkToHostOrder((long)BitConverter.ToUInt64(array, index));
                index += 8;

                for(ulong i = 0; i < this.dataSize; i++)
                {
                    this.data[i] = array[(ulong)index + i];
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }
        }

        public byte[] Command
        {
            get { return command; }
            set { command = value; }
        }

        public byte[] FileName
        {
            get { return fileName; }
            set { fileName = value; }
        }

        public byte[] FilePath
        {
            get { return filePath; }
            set { filePath = value; }
        }

        public ulong FileSize
        {
            get { return fileSize; }
            set { fileSize = value; }
        }

        public ulong DataSize
        {
            get {return dataSize; }
            set { dataSize = value; }
        }

        public byte[] Data
        {
            get { return data; }
            set { data = value; }
        }

        public int CopyToBuffer(ref byte[] buffer)
        {
            int index = 0;
            byte[] tmp;

            try
            {
                for(int i = 0; i < 16; i++)
                {
                    buffer[i] = command[i];
                }
                index += 16;

                for(int i = 0; i < 256; i++)
                {
                    buffer[index + i] = fileName[i];
                }
                index += 256;

                for(int i = 0; i < 256; i++)
                {
                    buffer[index + i] = filePath[i];
                }
                index += 256;

                tmp = BitConverter.GetBytes((ulong)IPAddress.HostToNetworkOrder((long)fileSize));
                for(int i = 0; i < 8; i++)
                {
                    buffer[index + i] = tmp[i];
                }
                index += 8;

                tmp = BitConverter.GetBytes((ulong)IPAddress.HostToNetworkOrder((long)dataSize));
                for(int i = 0; i < 8; i++)
                {
                    buffer[index + i] = tmp[i];
                }
                index += 8;

                for(ulong i = 0; i < dataSize; i++)
                {
                    buffer[(ulong)index + i] = data[i];
                }
                index += (int)dataSize;
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }

            return index;
        }
    }

    public class Encryption
    {
        protected bool flag;

        public Encryption(bool flag)
        {
            this.flag = flag;
        }

        ~Encryption()
        {

        }

        public bool Flag
        {
            get { return flag; }
            set { flag = value; }
        }

        protected byte HexByteToInt(byte c)
        {
            if((c >= 0x30) && (c <= 0x39))
            {
                c = (byte)(c - 0x30);
            }else if((c >= 0x61) && (c <= 0x66))
            {
                c = (byte)(c + (byte)0xa - (byte)0x61);
            }else if((c >= 0x41) && (c <= 0x46))
            {
                c = (byte)(c + (byte)0xa - (byte)0x41);
            }else
            {
                c = 0;
            }

            return c;
        }

        protected void HexStringToArray(byte[] hexString,
                                        int hexStringLength,
                                        ref byte[] buffer,
                                        int bufferSize)
        {
            byte tmp1 = 0;
            byte tmp2 = 0;
            int length = 0;

            for(int i = 0; i < hexStringLength && length < bufferSize; i += 2)
            {
                tmp1 = HexByteToInt(hexString[i]);
                tmp2 = HexByteToInt(hexString[i + 1]);

                tmp1 = (byte)(tmp1 << 4);
                buffer[length] = (byte)(tmp1 + tmp2);
                length++;
            }
        }

        public void PrintBytes(byte[] buffer,
                               int bufferLength)
        {
            for(int i = 0; i < bufferLength; i++)
            {
                if(i != 0 && i % 16 == 0){
                    Console.WriteLine("");
                }else if(i % 16 == 8)
                {
                    Console.Write(" ");
                }
                Console.Write("{0:X2} ", buffer[i]);
            }
            Console.WriteLine("");
        }

        public virtual int Encrypt(ref byte[] data,
                                   int dataSize,
                                   int bufferSize)
        {
            return 0;
        }

        public virtual int Decrypt(ref byte[] data,
                                   int dataSize,
                                   int bufferSize)
        {
            return 0;
        }
    }

    public class AesEncryption : Encryption
    {
        private const int AES_BLOCK_LEN = 16;
        private const int AES_KEY_LEN = 32;

        private string aesKeyHexString;
        private int aesKeyHexStringSize;
        private string aesIvHexString;
        private int aesIvHexStringSize;
        private byte[] key;
        private byte[] iv;
        ICryptoTransform encryptor = null;
        ICryptoTransform decryptor = null;

        public AesEncryption(bool flag,
                             string aesKeyHexString,
                             string aesIvHexString)
        : base(flag)
        {
            this.aesKeyHexString = aesKeyHexString;
            this.aesKeyHexStringSize = aesKeyHexString.Length;
            this.aesIvHexString = aesIvHexString;
            this.aesIvHexStringSize = aesIvHexString.Length;

            this.key = new byte[AES_KEY_LEN];
            this.iv = new byte[AES_BLOCK_LEN];

            if(flag == true &&
               this.aesKeyHexStringSize == AES_KEY_LEN * 2 &&
               this.aesIvHexStringSize == AES_BLOCK_LEN * 2)
            {
                HexStringToArray(Encoding.UTF8.GetBytes(this.aesKeyHexString),
                                 this.aesKeyHexStringSize,
                                 ref this.key,
                                 AES_KEY_LEN);

                HexStringToArray(Encoding.UTF8.GetBytes(this.aesIvHexString),
                                 this.aesIvHexStringSize,
                                 ref this.iv,
                                 AES_BLOCK_LEN);
            }else
            {
                Console.WriteLine("[-] aes key or iv size error: key({0}):{1} iv({2}):{3}",
                                  AES_KEY_LEN * 2,
                                  this.aesKeyHexStringSize,
                                  AES_BLOCK_LEN * 2,
                                  this.aesIvHexStringSize);

                this.flag = false;
            }
        }

        ~AesEncryption()
        {

        }

        public string AesKeyHexString
        {
            get { return aesKeyHexString; }
            set { aesKeyHexString = value; }
        }

        public int AesKeyHexStringSize
        {
            get { return aesKeyHexStringSize; }
            set { aesKeyHexStringSize = value; }
        }

        public string AesIvHexString
        {
            get { return aesIvHexString; }
            set { aesIvHexString = value; }
        }

        public int AesIvHexStringSize
        {
            get { return aesIvHexStringSize; }
            set { aesIvHexStringSize = value; }
        }

        public override int Encrypt(ref byte[] data,
                                    int dataSize,
                                    int bufferSize)
        {
            byte[] decryptedData;
            Aes aes = null;


            if(dataSize + AES_BLOCK_LEN > bufferSize)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] aes encrypt buffer size error: {0}",
                                  dataSize);
#endif
                return -1;
            }

#if DEBUGPRINT
//            PrintBytes(data, dataSize);
#endif

            try
            {
                decryptedData = new byte[dataSize];
                for(int i = 0; i < dataSize; i++)
                {
                    decryptedData[i] = data[i];
                }

                aes = Aes.Create();
                aes.Key = key;
                aes.IV = iv;
                aes.Mode = CipherMode.CBC;
                aes.Padding = PaddingMode.PKCS7;

                encryptor = aes.CreateEncryptor(aes.Key, aes.IV);

                using(var ms = new MemoryStream())
                {
                    using(var cs = new CryptoStream(ms, encryptor, CryptoStreamMode.Write))
                    {
                        cs.Write(decryptedData, 0, decryptedData.Length);
                        cs.FlushFinalBlock();
                    }

                    byte[] encryptedData = ms.ToArray();

                    Array.Clear(data,
                                0,
                                bufferSize);

                    for(int i = 0; i < encryptedData.Length; i++)
                    {
                        data[i] = encryptedData[i];
                    }

                    return encryptedData.Length;
                }
            }catch(CryptographicException)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] aes encrypt error)";
#endif
                return -1;
            }catch(Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] aes encrypt error");
#endif
                return -1;
            }
        }

        public override int Decrypt(ref byte[] data,
                                    int dataSize,
                                    int bufferSize)
        {
            byte[] encryptedData;
            Aes aes = null;


            if(dataSize % AES_BLOCK_LEN != 0)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] aes decrypt data size error: {0}",
                                  dataSize);
#endif
                return -1;
            }

            try
            {
                encryptedData = new byte[dataSize];
                for(int i = 0; i < dataSize; i++)
                {
                    encryptedData[i] = data[i];
                }

                aes = Aes.Create();
                aes.Key = key;
                aes.IV = iv;
                aes.Mode = CipherMode.CBC;
                aes.Padding = PaddingMode.PKCS7;

                decryptor = aes.CreateDecryptor(aes.Key, aes.IV);

                using(var ms = new MemoryStream(encryptedData))
                using(var cs = new CryptoStream(ms, decryptor, CryptoStreamMode.Read))
                using(var resultStream = new MemoryStream())
                {
                    cs.CopyTo(resultStream);
                    byte[] decryptedData = resultStream.ToArray();

                    Array.Clear(data,
                                0,
                                bufferSize);

                    for(int i = 0; i < decryptedData.Length; i++)
                    {
                        data[i] = decryptedData[i];
                    }

#if DEBUGPRINT
//                    PrintBytes(data, decryptedData.Length);
#endif

                    return decryptedData.Length;
                }
            }catch(CryptographicException)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] aes decrypt error");
#endif
                return -1;
            }catch(Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] aes decrypt error");
#endif
                return -1;
            }
        }
    }

    public class XorEncryption : Encryption
    {
        private string xorKeyHexString;
        int xorKeyHexStringSize;
        byte[] key;
        int keyLength;

        public XorEncryption(bool flag,
                             string xorKeyHexString)
        : base(flag)
        {
            this.xorKeyHexString = xorKeyHexString;
            this.xorKeyHexStringSize = xorKeyHexString.Length;

            if(this.xorKeyHexStringSize > 0){
                if(this.xorKeyHexStringSize % 2 == 0){
                    this.keyLength = this.xorKeyHexStringSize / 2;
                }else{
                    this.keyLength = this.xorKeyHexStringSize / 2 + 1;
                }

                this.key = new byte[this.xorKeyHexStringSize];

                HexStringToArray(Encoding.UTF8.GetBytes(this.xorKeyHexString),
                                 this.xorKeyHexStringSize,
                                 ref this.key,
                                 this.keyLength);
            }else{
                this.keyLength = 0;
                this.key = null;
            }
        }

        ~XorEncryption()
        {

        }

        public string XorKeyHexString
        {
            get { return xorKeyHexString; }
            set { xorKeyHexString = value; }
        }

        public int XorKeyHexStringSize
        {
            get { return xorKeyHexStringSize; }
            set { xorKeyHexStringSize = value; }
        }

        public override int Encrypt(ref byte[] data,
                                    int dataSize,
                                    int bufferSize)
        {
            if(keyLength == 0)
            {
                return 0;
            }else if(keyLength > 0)
            {
                for(int i = 0; i < dataSize; i++)
                {
                    data[i] = (byte)(data[i] ^ key[i % keyLength]);
                }
            }else
            {
                return -1;
            }

            return dataSize;
        }

        public override int Decrypt(ref byte[] data,
                                    int dataSize,
                                    int bufferSize)
        {
            if(keyLength == 0)
            {
                return 0;
            }else if(keyLength > 0)
            {
                for(int i = 0; i < dataSize; i++)
                {
                    data[i] = (byte)(data[i] ^ key[i % keyLength]);
                }
            }else
            {
                return -1;
            }

            return dataSize;
        }
    }

    public class Route
    {
        private char mode;  // auto:a self:s
        private string ip;
        private byte metric;
        private uint pipeId;
        private DateTime time;

        public Route(char mode,
                     string ip,
                     byte metric,
                     uint pipeId)
        {
            this.mode = mode;
            this.ip = ip;
            this.metric = metric;
            this.pipeId = pipeId;
            this.time = DateTime.Now;
        }

        ~Route()
        {

        }

        public char Mode
        {
            get { return mode; }
            set { mode = value; }
        }

        public string Ip
        {
            get { return ip; }
            set { ip = value; }
        }

        public byte Metric
        {
            get { return metric; }
            set { metric = value; }
        }

        public uint PipeId
        {
            get { return pipeId; }
            set { pipeId = value; }
        }

        public DateTime Time
        {
            get { return time; }
            set { time = value; }
        }
    }

    public class Message
    {
        protected char messageType;

        public Message()
        {

        }

        ~Message()
        {

        }

        public char MessageType
        {
            get { return messageType; }
            set { messageType = value; }
        }

        public virtual int CopyToBuffer(ref byte[] buffer)
        {
            return 0;
        }
    }

    public class RoutingMessage : Message
    {
        private const int ROUTING_MESSAGE_DATA_SIZE = 60000;

        private uint pipeId;
        private ushort dataSize;
        private byte[] data;


        public RoutingMessage()
        {
            this.pipeId = 0;
            this.messageType = 'r';
            this.dataSize = 0;
            this.data = new byte[ROUTING_MESSAGE_DATA_SIZE];
        }

        public RoutingMessage(uint pipeId,
                              byte[] array)
        {
            this.pipeId = pipeId;
            try
            {
                this.messageType = (char)array[0];
                this.dataSize = NetworkToHostOrderUShort(BitConverter.ToUInt16(array, 2));
                if(dataSize <= ROUTING_MESSAGE_DATA_SIZE)
                {
                    this.data = new byte[dataSize];
                    for(int i = 0; i < (int)dataSize; i++)
                    {
                        this.data[i] = array[4 + i];
                    }
                }else
                {
                    this.data = new byte[ROUTING_MESSAGE_DATA_SIZE];
                    for(int i = 0; i < ROUTING_MESSAGE_DATA_SIZE; i++)
                    {
                        this.data[i] = array[4 + i];
                    }
                }
            }catch(Exception ex)
            {
                throw new Exception("", ex);
            }

        }

        ~RoutingMessage()
        {

        }

        public uint PipeId
        {
            get { return pipeId; }
            set { pipeId = value; }
        }

        public ushort DataSize
        {
            get { return dataSize; }
            set { dataSize = value; }
        }

        public byte[] Data
        {
            get { return data; }
            set { data = value; }
        }

        public void PrintBytes()
        {
            for(int i = 0; i < dataSize; i++)
            {
                if(i != 0 && i % 16 == 0){
                    Console.WriteLine("");
                }else if(i % 16 == 8)
                {
                    Console.Write(" ");
                }
                Console.Write("{0:X2} ", data[i]);
            }
            Console.WriteLine("");
        }

        public ushort NetworkToHostOrderUShort(ushort value)
        {
            if(BitConverter.IsLittleEndian)
            {
                byte[] bytes = BitConverter.GetBytes(value);
                Array.Reverse(bytes);
                return BitConverter.ToUInt16(bytes, 0);
            }

            return value;
        }

        public ushort HostToNetworkOrderUShort(ushort value)
        {
            if(BitConverter.IsLittleEndian)
            {
                byte[] bytes = BitConverter.GetBytes(value);
                Array.Reverse(bytes);
                return BitConverter.ToUInt16(bytes, 0);
            }

            return value;
        }

        public override int CopyToBuffer(ref byte[] buffer)
        {
            int length = 0;

            buffer[0] = (byte)this.messageType;
            BitConverter.GetBytes(HostToNetworkOrderUShort(this.dataSize)).CopyTo(buffer, 2);
            for(int i = 0; i < (int)this.dataSize; i++)
            {
                buffer[4 + i] = this.data[i];
            }

            length = RoutingMessageDataHeader.ROUTING_MESSAGE_DATA_HEADER_SIZE
                   + this.dataSize;

            return length;
        }
    }

    public class Socks5Message : Message
    {
        private const int SOCKS5_MESSAGE_DATA_SIZE = 60000;
        private const int INET6_ADDR_STRING_LENGTH = 46;

        private uint messageId;
        private uint connectionId;
        private uint clientId;
        private uint serverId;
        private char sourceNodeType;
        private string sourceIp;
        private char destinationNodeType;
        private string destinationIp;
        private int tvSec;
        private int tvUsec;
        private int forwarderTvSec;
        private int forwarderTvUsec;
        private ushort dataSize;
        private byte[] data;

        public Socks5Message(char messageType,
                             uint messageId,
                             uint connectionId,
                             uint clientId,
                             uint serverId,
                             char sourceNodeType,
                             string sourceIp,
                             char destinationNodeType,
                             string destinationIp,
                             int tvSec,
                             int tvUsec,
                             int forwarderTvSec,
                             int forwarderTvUsec,
                             ushort dataSize,
                             byte[] data)
        {
            this.messageType = messageType;
            this.messageId = messageId;
            this.connectionId = connectionId;
            this.clientId = clientId;
            this.serverId = serverId;
            this.sourceNodeType = sourceNodeType;
            this.sourceIp = sourceIp;
            this.destinationNodeType = destinationNodeType;
            this.destinationIp = destinationIp;
            this.tvSec = tvSec;
            this.tvUsec = tvUsec;
            this.forwarderTvSec = forwarderTvSec;
            this.forwarderTvUsec = forwarderTvUsec;
            this.dataSize = dataSize;
            this.data = new byte[SOCKS5_MESSAGE_DATA_SIZE];
            if(this.dataSize <= SOCKS5_MESSAGE_DATA_SIZE)
            {
                for(int i = 0; i < this.dataSize; i++)
                {
                    this.data[i] = data[i];
                }
            }else
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socks5 message data size error: {0}",
                                  this.dataSize);
#endif
                for(int i = 0; i < SOCKS5_MESSAGE_DATA_SIZE; i++)
                {
                    this.data[i] = data[i];
                }
            }
        }

        public Socks5Message(byte[] array)
        {
            byte[] tmp;

            messageType = (char)array[0];
            messageId = (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(array, 4));
            connectionId = (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(array, 8));
            clientId = (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(array, 12));
            serverId = (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(array, 16));
            sourceNodeType = (char)array[20];

            tmp = new byte[INET6_ADDR_STRING_LENGTH + 1];
            for(int i = 0; i < INET6_ADDR_STRING_LENGTH + 1; i++)
            {
                tmp[i] = array[22 + i];
            }
            tmp = tmp.Where(b => b != 0x00).ToArray();
            sourceIp = Encoding.UTF8.GetString(tmp);
            sourceIp = sourceIp.Trim();

            destinationNodeType = (char)array[70];

            tmp = new byte[INET6_ADDR_STRING_LENGTH + 1];
            for(int i = 0; i < INET6_ADDR_STRING_LENGTH + 1; i++)
            {
                tmp[i] = array[72 + i];
            }
            tmp = tmp.Where(b => b != 0x00).ToArray();
            destinationIp = Encoding.UTF8.GetString(tmp);
            destinationIp = destinationIp.Trim();

            tvSec = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(array, 120));
            tvUsec = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(array, 124));
            forwarderTvSec = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(array, 128));
            forwarderTvUsec = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(array, 132));
            dataSize = NetworkToHostOrderUShort(BitConverter.ToUInt16(array, 136));
            if(dataSize <= SOCKS5_MESSAGE_DATA_SIZE)
            {
                this.data = new byte[dataSize];
                for(int i = 0; i < dataSize; i++)
                {
                    this.data[i] = array[144 + i];
                }
            }else
            {
                this.data = new byte[SOCKS5_MESSAGE_DATA_SIZE];
                for(int i = 0; i < SOCKS5_MESSAGE_DATA_SIZE; i++)
                {
                    this.data[i] = array[144 + i];
                }
            }
        }

        public uint MessageId
        {
            get { return messageId; }
            set { messageId = value; }
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

        public char SourceNodeType
        {
            get { return sourceNodeType; }
            set { sourceNodeType = value; }
        }

        public string SourceIp
        {
            get { return sourceIp; }
            set { sourceIp = value; }
        }

        public char DestinationNodeType
        {
            get { return destinationNodeType; }
            set { destinationNodeType = value; }
        }

        public string DestinationIp
        {
            get { return destinationIp; }
            set { destinationIp = value; }
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

        public ushort DataSize
        {
            get { return dataSize; }
            set { dataSize = value; }
        }

        public byte[] Data
        {
            get { return data; }
            set { data = value; }
        }

        public void PrintBytes()
        {
            for(int i = 0; i < dataSize; i++)
            {
                if(i != 0 && i % 16 == 0){
                    Console.WriteLine("");
                }else if(i % 16 == 8)
                {
                    Console.Write(" ");
                }
                Console.Write("{0:X2} ", data[i]);
            }
            Console.WriteLine("");
        }

        public ushort NetworkToHostOrderUShort(ushort value)
        {
            if(BitConverter.IsLittleEndian)
            {
                byte[] bytes = BitConverter.GetBytes(value);
                Array.Reverse(bytes);
                return BitConverter.ToUInt16(bytes, 0);
            }

            return value;
        }

        public ushort HostToNetworkOrderUShort(ushort value)
        {
            if(BitConverter.IsLittleEndian)
            {
                byte[] bytes = BitConverter.GetBytes(value);
                Array.Reverse(bytes);
                return BitConverter.ToUInt16(bytes, 0);
            }

            return value;
        }

        public override int CopyToBuffer(ref byte[] buffer)
        {
            int length = 0;
            byte[] tmp;

            buffer[0] = (byte)messageType;
            BitConverter.GetBytes((uint)IPAddress.HostToNetworkOrder((int)this.messageId)).CopyTo(buffer, 4);
            BitConverter.GetBytes((uint)IPAddress.HostToNetworkOrder((int)this.connectionId)).CopyTo(buffer, 8);
            BitConverter.GetBytes((uint)IPAddress.HostToNetworkOrder((int)this.clientId)).CopyTo(buffer, 12);
            BitConverter.GetBytes((uint)IPAddress.HostToNetworkOrder((int)this.serverId)).CopyTo(buffer, 16);
            buffer[20] = (byte)sourceNodeType;
            tmp = Encoding.UTF8.GetBytes(sourceIp);
            for(int i = 0; i < tmp.Length; i++)
            {
                buffer[22 + i] = tmp[i];
            }
            buffer[70] = (byte)destinationNodeType;
            tmp = Encoding.UTF8.GetBytes(destinationIp);
            for(int i = 0; i < tmp.Length; i++)
            {
                buffer[72 + i] = tmp[i];
            }
            BitConverter.GetBytes(IPAddress.HostToNetworkOrder(this.tvSec)).CopyTo(buffer, 120);
            BitConverter.GetBytes(IPAddress.HostToNetworkOrder(this.tvUsec)).CopyTo(buffer, 124);
            BitConverter.GetBytes(IPAddress.HostToNetworkOrder(this.forwarderTvSec)).CopyTo(buffer, 128);
            BitConverter.GetBytes(IPAddress.HostToNetworkOrder(this.forwarderTvUsec)).CopyTo(buffer, 132);
            BitConverter.GetBytes(HostToNetworkOrderUShort(this.dataSize)).CopyTo(buffer, 136);
            for(int i = 0; i < this.dataSize; i++)
            {
                buffer[144 + i] = this.data[i];
            }

            length = Socks5MessageDataHeader.SOCKS5_MESSAGE_DATA_HEADER_SIZE
                   + this.dataSize;

            return length;
        }
    }

    public class RoutingMessageQueue
    {
        private const int ROUTING_MESSAGE_QUEUE_CAPACITY = 100;
        private Queue<RoutingMessage> queue = new Queue<RoutingMessage>();
        private SemaphoreSlim token = new SemaphoreSlim(0, ROUTING_MESSAGE_QUEUE_CAPACITY);
        private SemaphoreSlim guard = new SemaphoreSlim(1, 1);

        public RoutingMessageQueue()
        {

        }

        ~RoutingMessageQueue()
        {

        }

        public void Push(RoutingMessage message)
        {
            guard.Wait();
            try
            {
                while(token.CurrentCount >= ROUTING_MESSAGE_QUEUE_CAPACITY)
                {
                    guard.Release();
                    Thread.Sleep(10);
                    guard.Wait();
                }
                queue.Enqueue(message);
                token.Release();
            }finally
            {
                guard.Release();
            }
        }

        public int PushTimeout(RoutingMessage message,
                               int tv_sec,
                               int tv_usec)
        {
            int ret = 0;
            int t = tv_sec * 1000 + tv_usec / 1000; // milliseconds

            if(guard.Wait(t))
            {
                try
                {
                    while(token.CurrentCount >= ROUTING_MESSAGE_QUEUE_CAPACITY)
                    {
                        guard.Release();
                        Thread.Sleep(10);
                        guard.Wait();
                    }
                    queue.Enqueue(message);
                    token.Release();
                }finally
                {
                    guard.Release();
                }
            }else
            {
#if DEBUGPRINT
                Console.WriteLine("[-] push timeout");
#endif
                ret = -1;
            }

            return ret;
        }

        public RoutingMessage Pop()
        {
            RoutingMessage message;

            token.Wait();
            guard.Wait();
            message = queue.Dequeue();
            guard.Release();

            return message;
        }

        public RoutingMessage PopTimeout(int tv_sec,
                                         int tv_usec)
        {
            RoutingMessage message = null;
            int t = tv_sec * 1000 + tv_usec / 1000; // milliseconds

            if(token.Wait(t))
            {
                guard.Wait();
                message = queue.Dequeue();
                guard.Release();
            }else
            {
#if DEBUGPRINT
                Console.WriteLine("[-] pop timeout");
#endif
            }

            return message;
        }
    }

    public class Socks5MessageQueue
    {
        private const int SOCKS5_MESSAGE_QUEUE_CAPACITY = 100;
        private Queue<Socks5Message> queue = new Queue<Socks5Message>();
        private SemaphoreSlim token = new SemaphoreSlim(0, SOCKS5_MESSAGE_QUEUE_CAPACITY);
        private SemaphoreSlim guard = new SemaphoreSlim(1, 1);

        public Socks5MessageQueue()
        {

        }

        ~Socks5MessageQueue()
        {

        }

        public void Push(Socks5Message message)
        {
            guard.Wait();
            try
            {
                while(token.CurrentCount >= SOCKS5_MESSAGE_QUEUE_CAPACITY)
                {
                    guard.Release();
                    Thread.Sleep(10);
                    guard.Wait();
                }
                queue.Enqueue(message);
                token.Release();
            }finally
            {
                guard.Release();
            }
        }

        public int PushTimeout(Socks5Message message,
                               int tv_sec,
                               int tv_usec)
        {
            int ret = 0;
            int t = tv_sec * 1000 + tv_usec / 1000; // milliseconds

            if(guard.Wait(t))
            {
                try
                {
                    while(token.CurrentCount >= SOCKS5_MESSAGE_QUEUE_CAPACITY)
                    {
                        guard.Release();
                        Thread.Sleep(10);
                        guard.Wait();
                    }
                    queue.Enqueue(message);
                    token.Release();
                }finally
                {
                    guard.Release();
                }
            }else
            {
#if DEBUGPRINT
                Console.WriteLine("[-] push timeout");
#endif
                ret = -1;
            }

            return ret;
        }

        public Socks5Message Pop()
        {
            Socks5Message message;

            token.Wait();
            guard.Wait();
            message = queue.Dequeue();
            guard.Release();

            return message;
        }

        public Socks5Message PopTimeout(int tv_sec,
                                        int tv_usec)
        {
            Socks5Message message = null;
            int t = tv_sec * 1000 + tv_usec / 1000; // milliseconds

            if(token.Wait(t))
            {
                guard.Wait();
                message = queue.Dequeue();
                guard.Release();
            }else
            {
#if DEBUGPRINT
                Console.WriteLine("[-] pop timeout");
#endif
            }

            return message;
        }
    }

    public class SpiderIp
    {
        private bool initFlag = false;
        private string spiderIpv4 = "";
        private string spiderIpv6Global = "";
        private string spiderIpv6UniqueLocal = "";
        private string spiderIpv6LinkLocal = "";
        private string spiderIpv6LinkLocalScopeId = "";

        public SpiderIp(string spiderIpv4,
                        string spiderIpv6Global,
                        string spiderIpv6UniqueLocal,
                        string spiderIpv6LinkLocal)
        {
            byte[] tmp;

            tmp = Encoding.UTF8.GetBytes(spiderIpv4.Trim());
            this.spiderIpv4 = Encoding.UTF8.GetString(tmp);

            tmp = Encoding.UTF8.GetBytes(spiderIpv6Global.Trim());
            this.spiderIpv6Global = Encoding.UTF8.GetString(tmp);

            tmp = Encoding.UTF8.GetBytes(spiderIpv6UniqueLocal.Trim());
            this.spiderIpv6UniqueLocal = Encoding.UTF8.GetString(tmp);

            tmp = Encoding.UTF8.GetBytes(spiderIpv6LinkLocal.Trim());
            this.spiderIpv6LinkLocal = Encoding.UTF8.GetString(tmp);

            if(!string.IsNullOrEmpty(spiderIpv6LinkLocal))
            {
                int percentIndex = spiderIpv6LinkLocal.IndexOf('%');
                if (percentIndex != -1)
                {
                    this.spiderIpv6LinkLocalScopeId = spiderIpv6LinkLocal.Substring(percentIndex + 1);
                    this.spiderIpv6LinkLocal = spiderIpv6LinkLocal.Substring(0, percentIndex);

                    initFlag = true;
                }else
                {
                    Console.WriteLine("[-] ipv6 link local address does not include a scope id");

                    initFlag = false;
                }

            }else
            {
                initFlag = true;
            }
        }

        ~SpiderIp()
        {

        }

        public bool InitFlag
        {
            get { return initFlag; }
            set { initFlag = value; }
        }

        public string SpiderIpv4
        {
            get { return spiderIpv4; }
            set { spiderIpv4 = value; }
        }

        public string SpiderIpv6Global
        {
            get { return spiderIpv6Global; }
            set { spiderIpv6Global = value; }
        }

        public string SpiderIpv6UniqueLocal
        {
            get { return spiderIpv6UniqueLocal; }
            set { spiderIpv6UniqueLocal = value; }
        }

        public string SpiderIpv6LinkLocal
        {
            get { return spiderIpv6LinkLocal; }
            set { spiderIpv6LinkLocal = value; }
        }

        public string SpiderIpv6LinkLocalScopeId
        {
            get { return spiderIpv6LinkLocalScopeId; }
            set { spiderIpv6LinkLocalScopeId = value; }
        }
    }

    public class Node
    {
        protected const int NODE_BUFFER_SIZE = 65535;

        protected TcpListener tcpListener = null;
        protected TcpClient tcpClient = null;
        protected UdpClient udpClient = null;
        protected IntPtr sock = IntPtr.Zero;
        protected NetworkStream stream = null;
        protected MessageManager messageManager;
        protected Socks5MessageQueue socks5MessagesQueue;

        public Node(TcpListener tcpListener,
                    TcpClient tcpClient,
                    MessageManager messageManager)
        {
            this.tcpListener = tcpListener;
            if(this.tcpListener != null)
            {
                this.sock = this.tcpListener.Server.Handle;
            }
            this.tcpClient = tcpClient;
            if(this.tcpClient != null)
            {
                this.sock = this.tcpClient.Client.Handle;
                this.stream = this.tcpClient.GetStream();
            }
            this.MessageManager = messageManager;
            this.socks5MessagesQueue = new Socks5MessageQueue();
        }

        public Node(UdpClient udpClient,
                    MessageManager messageManager)
        {
            this.udpClient = udpClient;
            if(this.udpClient != null)
            {
                this.sock = this.udpClient.Client.Handle;
            }
            this.MessageManager = messageManager;
            this.socks5MessagesQueue = new Socks5MessageQueue();
        }

        ~Node()
        {

        }

        public TcpListener TcpListener
        {
            get { return tcpListener; }
            set { tcpListener = value; }
        }

        public TcpClient TcpClient
        {
            get { return tcpClient; }
            set { tcpClient = value; }
        }

        public UdpClient UdpClient
        {
            get { return udpClient; }
            set { udpClient = value; }
        }

        public IntPtr Sock
        {
            get { return sock; }
            set { sock = value; }
        }

        public NetworkStream Stream
        {
            get { return stream; }
            set { stream = value; }
        }

        public MessageManager MessageManager
        {
            get { return messageManager; }
            set { messageManager = value; }
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

        protected Socks5Message PopSocks5Message()
        {
            Socks5Message socks5Message = socks5MessagesQueue.Pop();

            return socks5Message;
        }

        protected Socks5Message PopTimeoutSocks5Message(int tvSec,
                                                        int tvUsec)
        {
            Socks5Message socks5Message = socks5MessagesQueue.PopTimeout(tvSec,
                                                                         tvUsec);

            return socks5Message;
        }

        public int RecvData(byte[] buffer,
                            int bufferSize,
                            int tvSec,
                            int tvUsec)
        {
            int rec = 0;
            tcpClient.ReceiveTimeout = tvSec * 1000 + tvUsec / 1000;

            if(bufferSize > NODE_BUFFER_SIZE)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] buffer size error");
#endif
                return -1;
            }

            try
            {
                rec = stream.Read(buffer,
                                  0,
                                  NODE_BUFFER_SIZE);
            }catch(IOException)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] RecvData timeout");
#endif
                return -1;
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] RecvData error");
#endif
                return -1;
            }

            return rec;
        }

        public int SendData(byte[] buffer,
                            int dataSize,
                            int tvSec,
                            int tvUsec)
        {
            int sen = 0;
            this.tcpClient.SendTimeout = tvSec * 1000 + tvUsec / 1000;

            if(dataSize > NODE_BUFFER_SIZE)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] data size error");
#endif
                return -1;
            }

            try
            {
                stream.Write(buffer,
                             0,
                             dataSize);
            }catch(IOException)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] SendData timeout");
#endif
                return -1;
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] SendData error");
#endif
                return -1;
            }

            sen = dataSize;

            return sen;
        }

        public uint GenerateRandomId()
        {
            Random random = new Random();
            ulong randomValue = 0;

            do
            {
                randomValue = (ulong)random.Next(1, int.MaxValue) + (ulong)random.Next(0, int.MaxValue);
            }while(randomValue >= (ulong)uint.MaxValue);

            return (uint)randomValue;
        }

        public ushort NetworkToHostOrderUShort(ushort value)
        {
            if(BitConverter.IsLittleEndian)
            {
                byte[] bytes = BitConverter.GetBytes(value);
                Array.Reverse(bytes);
                return BitConverter.ToUInt16(bytes, 0);
            }

            return value;
        }

        public ushort HostToNetworkOrderUShort(ushort value)
        {
            if(BitConverter.IsLittleEndian)
            {
                byte[] bytes = BitConverter.GetBytes(value);
                Array.Reverse(bytes);
                return BitConverter.ToUInt16(bytes, 0);
            }

            return value;
        }

        public void PrintBytes(byte[] buffer,
                               int bufferLength)
        {
            for(int i = 0; i < bufferLength; i++)
            {
                if(i != 0 && i % 16 == 0){
                    Console.WriteLine("");
                }else if(i % 16 == 8)
                {
                    Console.Write(" ");
                }
                Console.Write("{0:X2} ", buffer[i]);
            }
            Console.WriteLine("");
        }
    }

    public class Client : Node
    {
        private const int SOCKS5_MESSAGE_DATA_SIZE = 60000;
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
                }catch(IOException)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderRecvData timeout");
#endif
                    break;
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderRecvData error");
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
            Dictionary<uint, ValueTuple<int, byte[]>> msgsMap = new Dictionary<uint, ValueTuple<int, byte[]>>();
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
                            msg = new ValueTuple<int, byte[]>(rec,
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
                }catch(IOException)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderSendData timeout");
#endif
                    break;
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderSendData error");
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
            }catch(Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] SelectionResponse error");
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
            int rec = 0;
            int sen = 0;
            int length = 0;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            byte[] data = new byte[NODE_BUFFER_SIZE];
            byte[] tmp;
            int bufferMaxLength = NODE_BUFFER_SIZE;
            int socks5MessageDataMaxSize = SOCKS5_MESSAGE_DATA_SIZE;

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
                Console.WriteLine("[+] [client <- client] Write");
#endif
                stream.Write(buffer,
                             0,
                             length);
                sen = length;
            }catch(IOException)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] ForwarderShellRecvData timeout");
#endif
                return;
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] ForwarderShellRecvData error");
#endif
                return;
            }

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
                                    Console.WriteLine("[+] [client -> server] SendMessage message_id{0}",
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
                                    uploadFileRemainingSize -= readBytes;

                                    Thread.Sleep(1);    // 1ms
                                }
                            }catch(Exception)
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] upload file error");
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
                                Console.WriteLine("[+] [client <- client] Write");
#endif
                                stream.Write(buffer,
                                             0,
                                             length);
                                sen = length;

                                continue;
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
                            Console.WriteLine("[+] [client -> server] SendMessage message_id:{0}",
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
                    }else
                    {
                        break;
                    }
                }catch(IOException)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderShellRecvData timeout");
#endif
                    break;
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderShellRecvData error");
#endif
                    break;
                }
            }

            return;
        }

        private void ForwarderShellSendData()
        {
            int rec = 0;
            int sen = 0;
            int length = 0;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            byte[] data;
            byte[] tmp;
            int bufferMaxLength = NODE_BUFFER_SIZE;
            Dictionary<uint, ValueTuple<int, byte[]>> msgsMap = new Dictionary<uint, ValueTuple<int, byte[]>>();
            ValueTuple<int, byte[]> msg;
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
                            nextRecvMessageId++;
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
                            }catch(Exception)
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
                                            continue;
                                        }
                                    }
                                }catch(Exception)
                                {
#if DEBUGPRINT
                                    Console.WriteLine("[-] download file error");
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
                                    Console.WriteLine("[+] [client <- client] Write");
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

                                    if(fileStream != null)
                                    {
                                        fileStream.Close();
                                        fileStream = null;
                                    }
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
                                Console.WriteLine("[+] [client <- client] Write");
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

                                if(sen <= 0)
                                {
                                    break;
                                }
                            }else
                            {
                                length = rec;
#if DEBUGPRINT
                                Console.WriteLine("[+] [client <- client] Write message_id:{0}",
                                                  nextRecvMessageId - 1);
#endif
                                stream.Write(buffer,
                                             0,
                                             length);
                                sen = length;
                            }
                        }else
                        {
                            msg = new ValueTuple<int, byte[]>(rec,
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
                                }catch(Exception)
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
                                                continue;
                                            }
                                        }
                                    }catch(Exception)
                                    {
#if DEBUGPRINT
                                        Console.WriteLine("[-] download file error");
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
                                        Console.WriteLine("[+] [client <- client] Write");
#endif
                                        stream.Write(buffer,
                                                     0,
                                                     length);
                                        sen = length;

                                        continue;
                                    }finally
                                    {
                                        if(fileStream != null)
                                        {
                                            fileStream.Close();
                                            fileStream = null;
                                        }
                                    }

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
                                    Console.WriteLine("[+] [client <- client] Write");
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

                                    if(sen <= 0)
                                    {
                                        break;
                                    }
                                }else
                                {
                                    length = rec;
#if DEBUGPRINT
                                    Console.WriteLine("[+] [client <- client] Write message_id:{0}",
                                                      nextRecvMessageId - 1);
#endif
                                    stream.Write(buffer,
                                                 0,
                                                 length);
                                    sen = length;
                                }

                            }

                            buffer = new byte[NODE_BUFFER_SIZE];
                        }
                    }else
                    {
                        break;
                    }
                }catch(IOException)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderShellSendData timeout");
#endif
                    break;
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderShellSendData error");
#endif
                    break;
                }
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
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] selectionRequest error");
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
            }catch(Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] SelectionResponse error");
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
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] usernamePasswordAuthenticationRequest error");
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
                }catch(Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] usernamePasswordAuthenticationResponse error");
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
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socksRequest error");
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
            }catch(Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] socksResponse error");
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
            Console.WriteLine("[+] [client -> server] SendMessage message_id:{0}",
                              sendMessageId);
#endif
            sen = SendMessage(buffer,
                              configSize,
                              forwarderTvSec,
                              forwarderTvUsec);
            if(sen > 0){
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
                    Console.WriteLine("[+] [client <- server] RecvMessage message_id:{0}",
                                      recvMessageId);
                    Console.WriteLine("[+] RecvMessage: {0}",
                                      result);
#endif
                    }else
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client <- server] RecvMessage error message_id:{0}",
                                          recvMessageId);
#endif
                    }
                }else
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] [client <- server] RecvMessage error");
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
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] selectionRequest error");
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
            }catch(Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] SelectionResponse error");
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
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] usernamePasswordAuthenticationRequest error");
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
                }catch(Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] usernamePasswordAuthenticationResponse error");
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
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socksRequest error");
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
            }catch(Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] socksResponse error");
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
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] selectionRequest error");
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
            }catch(Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] SelectionResponse error");
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
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] usernamePasswordAuthenticationRequest error");
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
                }catch(Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] usernamePasswordAuthenticationResponse error");
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
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socksRequest error");
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
            }catch(Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] socksResponse error");
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
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] selectionRequest error");
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
            }catch(Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] SelectionResponse error");
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
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] usernamePasswordAuthenticationRequest error");
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
                }catch(Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] usernamePasswordAuthenticationResponse error");
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
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socksRequest error");
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
            }catch(Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] socksResponse error");
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
            int rec = 0;
            int sen = 0;
            int length = 0;

            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            byte[] data;
            int bufferMaxLength = NODE_BUFFER_SIZE;
            int socks5MessageDataMaxSize = SOCKS5_MESSAGE_DATA_SIZE;
            Dictionary<uint, ValueTuple<IPEndPoint, int, byte[]>> msgsMap = new Dictionary<uint, ValueTuple<IPEndPoint, int, byte[]>>();
            ValueTuple<IPEndPoint, int, byte[]> msg;
            recvMessageId = 0;


            while(true)
            {
                try
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] [client -> client] Receive");
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
                        Console.WriteLine("[+] [client -> server] SendMessage message_id:{0}",
                                          sendMessageId);
#endif
                        sen = SendMessage(buffer,
                                          rec,
                                          forwarderTvSec,
                                          forwarderTvUsec);
                        if(sen <= 0){
//                            break;
                        }

                        sendMessageId++;
                    }else
                    {
//                        break;
                    }


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
                            Console.WriteLine("[+] [client <- client] Send message_id:{0}",
                                              nextRecvMessageId);
#endif
                            udpClient.Send(buffer,
                                           length,
                                           ipEndPoint);

                            sen = length;
                            nextRecvMessageId++;
                        }else
                        {
                            msg = new ValueTuple<IPEndPoint, int, byte[]>(ipEndPoint,
                                                                          rec,
                                                                          buffer);
                            msgsMap.Add(recvMessageId,
                                        msg);

                            while(msgsMap.ContainsKey(nextRecvMessageId))
                            {
                                msg = msgsMap[nextRecvMessageId];
                                msgsMap.Remove(nextRecvMessageId);

                                ipEndPoint = msg.Item1;
                                length = msg.Item2;
                                buffer = msg.Item3;

#if DEBUGPRINT
                                Console.WriteLine("[+] [client <- client] Send message_id:{0}",
                                                  nextRecvMessageId);
#endif
                                udpClient.Send(buffer,
                                               length,
                                               ipEndPoint);

                                sen = length;
                                nextRecvMessageId++;
                            }

                            buffer = new byte[NODE_BUFFER_SIZE];
                        }
                    }else
                    {
                        break;
                    }
                }catch(IOException)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderUdpRecvSendData error");
#endif
                    break;
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderUdpRecvSendData error");
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
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] selectionRequest error");
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
            }catch(Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] SelectionResponse error");
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
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] usernamePasswordAuthenticationRequest error");
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
                }catch(Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] usernamePasswordAuthenticationResponse error");
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
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socksRequest error");
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
            }catch(Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] socksResponse error");
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
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] selectionRequest error");
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
            }catch(Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] SelectionResponse error");
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
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] usernamePasswordAuthenticationRequest error");
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
                }catch(Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[+] usernamePasswordAuthenticationResponse error");
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
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socksRequest error");
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
            }catch(Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] socksResponse error");
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
            }catch(Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] udpClient error");
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

    public class Pipe : Node
    {
        private SpiderIp spiderIp;
        private uint pipeId;
        private char mode;
        private string pipeIp;
        private string pipeIpScopeId;
        private string pipeListenPort;
        private string pipeDestinationIp;
        private string pipeDestinationIpScopeId;
        private string pipeDestinationPort;
        private RoutingManager routingManager;
        private RoutingMessageQueue routingMessagesQueue;

        public Pipe(SpiderIp spiderIp,
                    uint pipeId,
                    char mode,
                    string pipeIp,
                    string pipeIpScopeId,
                    string pipeDestinationIp,
                    string pipeDestinationIpScopeId,
                    string pipeDestinationPort,
                    TcpClient tcpClient,
                    RoutingManager routingManager,
                    MessageManager messageManager)
        : base(null,
               tcpClient,
               messageManager)
        {
            this.spiderIp = spiderIp;
            this.pipeId = pipeId;
            this.mode = mode;
            this.pipeIp = pipeIp;
            this.pipeIpScopeId = pipeIpScopeId;
            this.pipeDestinationIp = pipeDestinationIp;
            this.pipeDestinationIpScopeId = pipeDestinationIpScopeId;
            this.pipeDestinationPort = pipeDestinationPort;
            this.routingManager = routingManager;
            this.routingMessagesQueue = new RoutingMessageQueue();

            this.tcpClient.ReceiveTimeout = 3600 * 1000;
            this.tcpClient.SendTimeout = 3600 * 1000;
        }

        public Pipe(SpiderIp spiderIp,
                    uint pipeId,
                    char mode,
                    string pipeIp,
                    string pipeIpScopeId,
                    string pipeListenPort,
                    TcpListener tcpListener,
                    RoutingManager routingManager,
                    MessageManager messageManager)
        : base(tcpListener,
               null,
               messageManager)
        {
            this.spiderIp = spiderIp;
            this.pipeId = pipeId;
            this.mode = mode;
            this.pipeIp = pipeIp;
            this.pipeIpScopeId = pipeIpScopeId;
            this.pipeListenPort = pipeListenPort;
            this.routingManager = routingManager;
            this.routingMessagesQueue = new RoutingMessageQueue();
        }

        ~Pipe()
        {

        }

        public SpiderIp SpiderIp
        {
            get { return spiderIp; }
            set { spiderIp = value; }
        }

        public uint PipeId
        {
            get { return pipeId; }
            set { pipeId = value; }
        }

        public char Mode
        {
            get { return mode; }
            set { mode = value; }
        }

        public string PipeIp
        {
            get { return pipeIp; }
            set { pipeIp = value; }
        }

        public string PipeIpScopeId
        {
            get { return pipeIpScopeId; }
            set { pipeIpScopeId = value; }
        }

        public string PipeListenPort
        {
            get { return pipeListenPort; }
            set { pipeListenPort = value; }
        }

        public string PipeDestinationIp
        {
            get { return pipeDestinationIp; }
            set { pipeDestinationIp = value; }
        }

        public string PipeDestinationIpScopeId
        {
            get { return pipeDestinationIpScopeId; }
            set { pipeDestinationIpScopeId = value; }
        }

        public string PipeDestinationPort
        {
            get { return pipeDestinationPort; }
            set { pipeDestinationPort = value; }
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

        public int RecvMessage()
        {
            int rec = 0;
            int tmprec = 0;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            byte[] tmp = new byte[NODE_BUFFER_SIZE];

            RoutingMessage routingMessage;
            RoutingMessageDataHeader routingMessageDataHeader = new RoutingMessageDataHeader(buffer);
            ushort routingMessageHeaderSize = (ushort)RoutingMessageDataHeader.ROUTING_MESSAGE_DATA_HEADER_SIZE;

            Socks5Message socks5Message;
            Socks5MessageDataHeader socks5MessageDataHeader = new Socks5MessageDataHeader(buffer);
            ushort socks5MessageHeaderSize = (ushort)Socks5MessageDataHeader.SOCKS5_MESSAGE_DATA_HEADER_SIZE;

            bool recvMessageTypeFlag = false;
            bool recvHeaderFlag = false;
            int recvMessageTypeSize = 1;    // byte
            int recvDataSize = 0;
            int remainingSize = 0;

            Pipe pipe;


            while(true)
            {
                try
                {
                    if(recvMessageTypeFlag == false)
                    {

                        tmprec = stream.Read(buffer,
                                             rec,
                                             recvMessageTypeSize);
                    }else
                    {
                        tmprec = stream.Read(buffer,
                                             rec,
                                             remainingSize);
                    }

                    if(tmprec > 0)
                    {
                        rec += tmprec;
                        tmprec = 0;

#if DEBUGPRINT
//                        Console.WriteLine("rec: {0}", rec);
//                        PrintBytes(buffer, rec);
#endif

                        if(recvMessageTypeFlag == false)
                        {
                            recvMessageTypeFlag = true;

                            if(buffer[0] == 0x72)   // 'r'
                            {
                                remainingSize = routingMessageHeaderSize - rec;
                            }else if(buffer[0] == 0x73 )    // 's'
                            {
                                remainingSize = socks5MessageHeaderSize - rec;
                            }else
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] RecvMessage message type error: {0}",
                                                  (char)buffer[0]);
#endif
                                return 0;
                            }

                            continue;
                        }else if(recvHeaderFlag == false)
                        {
                            if(buffer[0] == 0x72)   // 'r'
                            {
                                if(rec < routingMessageHeaderSize)
                                {
                                    remainingSize = routingMessageHeaderSize - rec;
                                }else
                                {
                                    recvHeaderFlag = true;

                                    routingMessageDataHeader = new RoutingMessageDataHeader(buffer);

                                    recvDataSize = NetworkToHostOrderUShort(routingMessageDataHeader.DataSize);

                                    remainingSize = recvDataSize;
                                }

                                continue;
                            }else if(buffer[0] == 0x73) // 's'
                            {
                                if(rec < socks5MessageHeaderSize)
                                {
                                    remainingSize = socks5MessageHeaderSize - rec;
                                }else
                                {
                                    recvHeaderFlag = true;

                                    socks5MessageDataHeader = new Socks5MessageDataHeader(buffer);

                                    recvDataSize = NetworkToHostOrderUShort(socks5MessageDataHeader.DataSize);

                                    remainingSize = recvDataSize;
                                }

                                continue;
                            }else
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] RecvMessage message type error: {0}",
                                                  (char)buffer[0]);
#endif
                                return 0;
                            }
                        }else
                        {
                            if(buffer[0] == 0x72)   // 'r'
                            {
                                if(rec < routingMessageHeaderSize + recvDataSize)
                                {
                                    remainingSize = routingMessageHeaderSize + recvDataSize - rec;
                                    continue;
                                }else
                                {
                                    routingMessageDataHeader = new RoutingMessageDataHeader(buffer);
                                    if(rec < routingMessageHeaderSize + NetworkToHostOrderUShort(routingMessageDataHeader.DataSize))
                                    {
                                        continue;
                                    }else if(rec == routingMessageHeaderSize + NetworkToHostOrderUShort(routingMessageDataHeader.DataSize))
                                    {
                                        routingMessage = new RoutingMessage(this.PipeId,
                                                                            buffer);

                                        Thread threadMessageManager = new Thread(new ParameterizedThreadStart(messageManager.PushRoutingMessage));
                                        threadMessageManager.Start(routingMessage);

                                        recvMessageTypeFlag = false;
                                        recvHeaderFlag = false;
                                        Array.Clear(buffer,
                                                    0,
                                                    NODE_BUFFER_SIZE);
                                    }
                                }
                            }else if(buffer[0] == 0x73) // 's'
                            {
                                if(rec < socks5MessageHeaderSize + recvDataSize)
                                {
                                    remainingSize = socks5MessageHeaderSize + recvDataSize - rec;
                                    continue;
                                }else
                                {
                                    socks5MessageDataHeader = new Socks5MessageDataHeader(buffer);
                                    if(rec < socks5MessageHeaderSize + NetworkToHostOrderUShort(socks5MessageDataHeader.DataSize))
                                    {
                                        continue;
                                    }else if(rec == socks5MessageHeaderSize + NetworkToHostOrderUShort(socks5MessageDataHeader.DataSize))
                                    {
                                        socks5Message = new Socks5Message(buffer);

                                        if((String.Compare(spiderIp.SpiderIpv4, socks5Message.DestinationIp) == 0) ||
                                           (String.Compare(spiderIp.SpiderIpv6Global, socks5Message.DestinationIp) == 0) ||
                                           (String.Compare(spiderIp.SpiderIpv6UniqueLocal, socks5Message.DestinationIp) == 0) ||
                                           (String.Compare(spiderIp.SpiderIpv6LinkLocal, socks5Message.DestinationIp) == 0))
                                        {
                                            Thread threadMessageManager = new Thread(new ParameterizedThreadStart(messageManager.PushSocks5Message));
                                            threadMessageManager.Start(socks5Message);

                                            recvMessageTypeFlag = false;
                                            recvHeaderFlag = false;
                                            Array.Clear(buffer,
                                                        0,
                                                        NODE_BUFFER_SIZE);
                                        }else
                                        {
                                            pipe = routingManager.GetDestinationPipe(socks5Message.DestinationIp);
                                            if(pipe != null)
                                            {
                                                Thread threadPipe = new Thread(new ParameterizedThreadStart(pipe.PushSocks5Message));
                                                threadPipe.Start(socks5Message);

                                                recvMessageTypeFlag = false;
                                                recvHeaderFlag = false;
                                                Array.Clear(buffer,
                                                            0,
                                                            NODE_BUFFER_SIZE);
                                            }else
                                            {
#if DEBUGPRINT
                                                Console.WriteLine("[-] cannot transfer pipe message");
#endif

                                                return 0;
                                            }
                                        }
                                    }
                                }
                            }else
                            {
#if DEBUGPRINT
                                Console.WriteLine("[-] recv_message message type error: {0}",
                                                  (char)buffer[0]);
#endif
                                return 0;
                            }
                        }

                        recvMessageTypeFlag = false;
                        recvHeaderFlag = false;
                        Array.Clear(buffer,
                                    0,
                                    NODE_BUFFER_SIZE);

                        break;
                    }
                }catch(IOException)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] RecvMessage error");
#endif
                    return -1;
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] RecvMessage error");
#endif
                    return -1;
                }
            }

            return rec;
        }

        public int SendRoutingMessage()
        {
            int sen = 0;
            int length = 0;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];

            RoutingMessage routingMessage;
            routingMessage = PopRoutingMessage();
            if(routingMessage == null)
            {
                return 0;
            }

            length = routingMessage.CopyToBuffer(ref buffer);

#if DEBUGPRINT
//            Console.WriteLine("length: {0}", length);
//            PrintBytes(buffer, length);
#endif

            try
            {
                stream.Write(buffer,
                             0,
                             length);
            }catch(IOException)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] SendRoutingMessage error");
#endif
                return -1;
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] SendRoutingMessage error");
#endif
                return -1;
            }

            sen = length;

            return sen;
        }

        public int SendSocks5Message()
        {
            int sen = 0;
            int length = 0;
            byte[] buffer = new byte[NODE_BUFFER_SIZE];

            Socks5Message socks5Message;
            socks5Message = PopSocks5Message();
            length = socks5Message.CopyToBuffer(ref buffer);

#if DEBUGPRINT
//            Console.WriteLine("length: {0}", length);
//            PrintBytes(buffer, length);
#endif

            try
            {
                stream.Write(buffer,
                             0,
                             length);
            }catch(IOException)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] SengSocks5Message error");
#endif
                return -1;
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] SengSocks5Message error");
#endif
                return -1;
            }

            sen = length;

            return sen;
        }
    }

    public class Server : Node
    {
        private const int SOCKS5_MESSAGE_DATA_SIZE = 60000;
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
        private RoutingManager routingManager;
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
                      RoutingManager routingManager,
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
            this.routingManager = routingManager;
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
            Dictionary<uint, ValueTuple<int, byte[]>> msgsMap = new Dictionary<uint, ValueTuple<int, byte[]>>();
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
                            msg = new ValueTuple<int, byte[]>(rec,
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
                }catch(IOException)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderRecvData error");
#endif
                    break;
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderRecvData error");
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
                }catch(IOException)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderSendData error");
#endif
                    break;
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderSendData error");
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
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] socksResponseIvp4 error");
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
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] SocksResponseIpv6 error");
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
            ProcessStartInfo processStartInfo = new ProcessStartInfo
            {
                FileName = "cmd.exe",
                Arguments = "/c " + command,
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
            Dictionary<uint, ValueTuple<int, byte[]>> msgsMap = new Dictionary<uint, ValueTuple<int, byte[]>>();
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
                            }catch(Exception)
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
                                }catch(Exception)
                                {
#if DEBUGPRINT
                                    Console.WriteLine("[-] upload file error");
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
                                        }catch(Exception)
                                        {
#if DEBUGPRINT
                                            Console.WriteLine("[-] change directory error");
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

                                            Thread.Sleep(1);    // 1ms
                                        }
                                    }catch(Exception)
                                    {
#if DEBUGPRINT
                                        Console.WriteLine("[-] download file error");
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
                            msg = new ValueTuple<int, byte[]>(rec,
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
                                }catch(Exception)
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
                                    }catch(Exception)
                                    {
#if DEBUGPRINT
                                        Console.WriteLine("[-] upload file error");
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
                                            }catch(Exception)
                                            {
#if DEBUGPRINT
                                                Console.WriteLine("[-] change directory error");
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

                                                Thread.Sleep(1);    // 1ms
                                            }
                                        }catch(Exception)
                                        {
#if DEBUGPRINT
                                            Console.WriteLine("[-] download file error");
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
                }catch(IOException)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderShellSendData timeout");
#endif
                    break;
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderShellSendData error");
#endif
                    break;
                }
            }

            return 0;
        }

        private int ForwarderAddNode()
        {
            int ret = 0;
            int rec = 0;
            int sen = 0;

            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            byte[] data;
            int bufferMaxLength = NODE_BUFFER_SIZE;
            recvMessageId = 0;

            string config = "";
            string result = "";
            int resultSize = 0;


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
                    data = buffer.Where(b => b != 0x00).ToArray();
                    config = Encoding.UTF8.GetString(data);

                    ret = spiderCommand.ReadConfig(config);
                    if(ret != 0)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] ReadConfig error");
#endif
                        result = "ReadConfig ng";
                    }else
                    {
#if DEBUGPRINT
                        Console.WriteLine("[+] ReadConfig");
#endif
                        result = "ReadConfig ok";
                    }

                    data = Encoding.UTF8.GetBytes(result);
                    resultSize = data.Length;

                    for(int i = 0; i < resultSize; i++)
                    {
                        buffer[i] = data[i];
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
                }
            }else
            {
#if DEBUGPRINT
                Console.WriteLine("[-] [client -> server] RecvMessage error");
#endif
                return -1;
            }

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

        private int ForwarderShowRoute()
        {
            int sen = 0;

            byte[] buffer = new byte[NODE_BUFFER_SIZE];
            byte[] data;
            int socks5MessageDataMaxSize = SOCKS5_MESSAGE_DATA_SIZE;
            recvMessageId = 0;

            string result = "";
            int resultSize = 0;


            result += routingManager.ShowRoutingTableString();

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
            Dictionary<uint, ValueTuple<int, byte[]>> msgsMap = new Dictionary<uint, ValueTuple<int, byte[]>>();
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
                            msg = new ValueTuple<int, byte[]>(rec,
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
                }catch(IOException)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderUdpRecvSendData error");
#endif
                    break;
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] ForwarderUdpRecvSendData error");
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
            bool socks5ConnectShowRouteFlag = false;
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
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] SelectionRequest error");
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
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] selectionResponse error");
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
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] UsernamePasswordAuthenticationRequest error");
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
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] selectionResponse error");
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
            }catch (Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] SocksRequest error");
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
               cmd != 0xb &&    // SHOW NODE (0xb, show node information, original command)
               cmd != 0xc)      // SHOW ROUTE (0xc, show routing table, original command)
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
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] SocksRequestIpv4 error");
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
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] SocksRequestDomainname error");
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
                    }catch(Exception)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] cannot resolv the domain name");
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
                    }catch(Exception)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] cannot resolv the domain name");
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
                }catch (Exception)
                {
#if DEBUGPRINT
                    Console.WriteLine("[-] SocksRequestIpv6 error");
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
                      cmd != 0xb &&
                      cmd != 0xc) &&
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
                    }catch(Exception)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [server <- target] cannot connect error");
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
                    }catch(Exception)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [server <- target] UdpClient or IPEndPoint error");
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
                        }catch(Exception)
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] [server <- target] cannot connect error");
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
                        }catch(Exception)
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] [server <- target] UdpClient or IPEndPoint error");
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
                        }catch(Exception)
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] [server <- target] cannot connect error");
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
                        }catch(Exception)
                        {
#if DEBUGPRINT
                            Console.WriteLine("[-] [server <- target] UdpClient or IPEndPoint error");
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
                    }catch(Exception)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [server <- target] cannot connect error");
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
                    }catch(Exception)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [server <- target] UdpClient or IPEndPoint error");
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
                     atyp == 0x0)    // SHOW NODE (0xb, show node information, original command)
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
            }else if(cmd == 0xc &&
                     atyp == 0x0)    // SHOW ROUTE (0xc, show routing table, original command)
            {
#if DEBUGPRINT
                Console.WriteLine("[+] socks5 response cmd: SHOW ROUTE (0xc, original command)");
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
                socks5ConnectShowRouteFlag = true;
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
            }else if(socks5ConnectShowRouteFlag == true)
            {
                ret = ForwarderShowRoute();
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

    public class ClientManager
    {
        [DllImport("iphlpapi.dll", SetLastError = true)]
        public static extern uint if_nametoindex(string ifname);

        private Dictionary<ValueTuple<uint, uint>, Client> clientsMap = new Dictionary<ValueTuple<uint, uint>, Client>();
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
            ValueTuple<uint, uint> clientKey = new ValueTuple<uint, uint>(connectionId,
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
            ValueTuple<uint, uint> clientKey = new ValueTuple<uint, uint>(connectionId,
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
            ValueTuple<uint, uint> clientKey = new ValueTuple<uint, uint>(connectionId,
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
            Console.WriteLine("---------------------------------------------------------------------------------------------------- pipe ------------------------------------------------------------------------------------------------------");
            Console.WriteLine("|pipe id   |mode|pipe ip                                       |pipe ip scope id|pipe listen port|pipe destination ip                           |pipe destination ip scope id|pipe destination port|pipe socket|");
            Console.WriteLine("----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");

            lock(pipesMapLock)
            {
                foreach(var kvp in pipesMap)
                {
                    Console.WriteLine("|{0,10}|{1}   |{2,-46}|{3,-10}      |           {4,5}|{5,-46}|{6,-10}                  |                {7,5}|      {8,5}|",
                                      kvp.Value.PipeId,
                                      kvp.Value.Mode,
                                      kvp.Value.PipeIp,
                                      kvp.Value.PipeIpScopeId,
                                      kvp.Value.PipeListenPort,
                                      kvp.Value.PipeDestinationIp,
                                      kvp.Value.PipeDestinationIpScopeId,
                                      kvp.Value.PipeDestinationPort,
                                      kvp.Value.Sock);
                }
            }

            Console.WriteLine("----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
            Console.WriteLine("");

            return;
        }

        public string ShowPipesMapString()
        {
            string result = "";

            result += "---------------------------------------------------------------------------------------------------- pipe ------------------------------------------------------------------------------------------------------\n";

            result += "|pipe id   |mode|pipe ip                                       |pipe ip scope id|pipe listen port|pipe destination ip                           |pipe destination ip scope id|pipe destination port|pipe socket|\n";

            result += "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";

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

                    result += string.Format("|{0,10}|{1}   |{2,-46}|{3,-10} ({4,3})|           {5,5}|{6,-46}|{7,-10}             ({8,3})|                {9,5}|      {10,5}|\n",
                                      kvp.Value.PipeId,
                                      kvp.Value.Mode,
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

            result += "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\n";

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

    public class ServerManager
    {
        [DllImport("iphlpapi.dll", SetLastError = true)]
        public static extern uint if_nametoindex(string ifname);

        private Dictionary<ValueTuple<uint, uint>, Server> serversMap = new Dictionary<ValueTuple<uint, uint>, Server>();
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
            ValueTuple<uint, uint> serverKey = new ValueTuple<uint, uint>(connectionId,
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
            ValueTuple<uint, uint> serverKey = new ValueTuple<uint, uint>(connectionId,
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
            ValueTuple<uint, uint> serverKey = new ValueTuple<uint, uint>(connectionId,
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

                        if(clientRemoteEndPoint != null)
                        {
                            clientClientIpAddress = clientRemoteEndPoint.Address;
                            port = clientRemoteEndPoint.Port;
                            scopeId = clientClientIpAddress.ScopeId;

                            clientIp = clientClientIpAddress.ToString();
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
            string clientListenIp = "";
            string clientListenIpScopeId = "";
            string clientListenPort = "";
            string destinationSpiderIp = "";
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

                Console.Write("destination spider ip                          > ");
                input = Console.ReadLine();
                input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                tmp = Encoding.UTF8.GetBytes(input.Trim());
                destinationSpiderIp = Encoding.UTF8.GetString(tmp);

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
                Console.WriteLine("client listen ip          : {0}", clientListenIp);
                if(!string.IsNullOrEmpty(clientListenIpScopeId))
                {
                    Console.WriteLine("client listen ip scope id : {0}", clientListenIpScopeId);
                }
                Console.WriteLine("client listen port        : {0}", clientListenPort);
                Console.WriteLine("destination spider ip     : {0}", destinationSpiderIp);
                Console.WriteLine("recv/send tv_sec          : {0,7} sec", tvSec);
                Console.WriteLine("recv/send tv_usec         : {0,7} microsec", tvUsec);
                Console.WriteLine("forwarder_tv_sec          : {0,7} sec", forwarderTvSec);
                Console.WriteLine("forwarder_tv_usec         : {0,7} microsec", forwarderTvUsec);
                Console.WriteLine("");

                Console.Write("ok? (yes:y no:n quit:q)                        > ");
                input = Console.ReadLine();
                input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                check = input[0];
                if(check == 'y')
                {
                    break;
                }else if(check == 'n')
                {
                    continue;
                }else if(check == 'q'){
                    return;
                }else{
                    return;
                }

            }

            parameters = new object[] {clientListenIp,
                                       clientListenIpScopeId,
                                       clientListenPort,
                                       destinationSpiderIp,
                                       tvSec,
                                       tvUsec,
                                       forwarderTvSec,
                                       forwarderTvUsec};

            Thread thread = new Thread(new ParameterizedThreadStart(ListenClient));
            thread.Start(parameters);

            Thread.Sleep(2000); // 2s

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

                        if(pipeRemoteEndPoint != null)
                        {
                            pipeClientIpAddress = pipeRemoteEndPoint.Address;
                            port = pipeRemoteEndPoint.Port;
                            scopeId = pipeClientIpAddress.ScopeId;

                            pipeDestinationIp = pipeClientIpAddress.ToString();
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
            char mode;  // client:c server:s
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
                Console.Write("mode (client:c server:s)                       > ");
                input = Console.ReadLine();
                input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                mode = input[0];
                if(mode == 'c')
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
                    Console.WriteLine("mode                      : {0}", mode);
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
                        parameters = new object[] {mode,
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
                    }else if(check == 'q'){
                        return;
                    }else{
                        return;
                    }
                }else if(mode == 's')
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
                    Console.WriteLine("mode                      : {0}", mode);
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
                        parameters = new object[] {mode,
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

                        if(clientRemoteEndPoint != null)
                        {
                            clientClientIpAddress = clientRemoteEndPoint.Address;
                            port = clientRemoteEndPoint.Port;
                            scopeId = clientClientIpAddress.ScopeId;

                            clientIp = clientClientIpAddress.ToString();
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
            string clientListenIp = "";
            string clientListenIpScopeId = "";
            string clientListenPort = "";
            string destinationSpiderIp = "";
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

                Console.Write("destination spider ip                          > ");
                input = Console.ReadLine();
                input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                tmp = Encoding.UTF8.GetBytes(input.Trim());
                destinationSpiderIp = Encoding.UTF8.GetString(tmp);

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
                Console.WriteLine("client listen ip          : {0}", clientListenIp);
                if(!string.IsNullOrEmpty(clientListenIpScopeId))
                {
                    Console.WriteLine("client listen ip scope id : {0}", clientListenIpScopeId);
                }
                Console.WriteLine("client listen port        : {0}", clientListenPort);
                Console.WriteLine("destination spider ip     : {0}", destinationSpiderIp);
                Console.WriteLine("target ip                 : {0}", targetIp);
                Console.WriteLine("target port               : {0}", targetPort);
                Console.WriteLine("recv/send tv_sec          : {0,7} sec", tvSec);
                Console.WriteLine("recv/send tv_usec         : {0,7} microsec", tvUsec);
                Console.WriteLine("forwarder_tv_sec          : {0,7} sec", forwarderTvSec);
                Console.WriteLine("forwarder_tv_usec         : {0,7} microsec", forwarderTvUsec);
                Console.WriteLine("");

                Console.Write("ok? (yes:y no:n quit:q)                        > ");
                input = Console.ReadLine();
                input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                check = input[0];
                if(check == 'y')
                {
                    break;
                }else if(check == 'n')
                {
                    continue;
                }else if(check == 'q'){
                    return;
                }else{
                    return;
                }
            }

            parameters = new object[] {clientListenIp,
                                       clientListenIpScopeId,
                                       clientListenPort,
                                       destinationSpiderIp,
                                       targetIp,
                                       targetPort,
                                       tvSec,
                                       tvUsec,
                                       forwarderTvSec,
                                       forwarderTvUsec};

            Thread thread = new Thread(new ParameterizedThreadStart(ListenClientTcp));
            thread.Start(parameters);

            Thread.Sleep(2000); // 2s

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
            string clientListenIp = "";
            string clientListenIpScopeId = "";
            string clientListenPort = "";
            string destinationSpiderIp = "";
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

                Console.Write("destination spider ip                          > ");
                input = Console.ReadLine();
                input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                tmp = Encoding.UTF8.GetBytes(input.Trim());
                destinationSpiderIp = Encoding.UTF8.GetString(tmp);

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
                Console.WriteLine("client listen ip          : {0}", clientListenIp);
                if(!string.IsNullOrEmpty(clientListenIpScopeId))
                {
                    Console.WriteLine("client listen ip scope id : {0}", clientListenIpScopeId);
                }
                Console.WriteLine("client listen port        : {0}", clientListenPort);
                Console.WriteLine("destination spider ip     : {0}", destinationSpiderIp);
                Console.WriteLine("target ip                 : {0}", targetIp);
                Console.WriteLine("target port               : {0}", targetPort);
                Console.WriteLine("recv/send tv_sec          : {0,7} sec", tvSec);
                Console.WriteLine("recv/send tv_usec         : {0,7} microsec", tvUsec);
                Console.WriteLine("forwarder_tv_sec          : {0,7} sec", forwarderTvSec);
                Console.WriteLine("forwarder_tv_usec         : {0,7} microsec", forwarderTvUsec);
                Console.WriteLine("FORWARDER_UDP_TIMEOUT     : {0,7} sec", FORWARDER_UDP_TIMEOUT);
                Console.WriteLine("");

                Console.Write("ok? (yes:y no:n quit:q)                        > ");
                input = Console.ReadLine();
                input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                check = input[0];
                if(check == 'y')
                {
                    break;
                }else if(check == 'n')
                {
                    continue;
                }else if(check == 'q'){
                    return;
                }else{
                    return;
                }
            }

            parameters = new object[] {clientListenIp,
                                       clientListenIpScopeId,
                                       clientListenPort,
                                       destinationSpiderIp,
                                       targetIp,
                                       targetPort,
                                       tvSec,
                                       tvUsec,
                                       forwarderTvSec,
                                       forwarderTvUsec};

            Thread thread = new Thread(new ParameterizedThreadStart(ClientUdpWorker));
            thread.Start(parameters);

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

                        if(clientRemoteEndPoint != null)
                        {
                            clientClientIpAddress = clientRemoteEndPoint.Address;
                            port = clientRemoteEndPoint.Port;
                            scopeId = clientClientIpAddress.ScopeId;

                            clientIp = clientClientIpAddress.ToString();
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
            string clientListenIp = "";
            string clientListenIpScopeId = "";
            string clientListenPort = "";
            string destinationSpiderIp = "";
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

                Console.Write("destination spider ip                          > ");
                input = Console.ReadLine();
                input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                tmp = Encoding.UTF8.GetBytes(input.Trim());
                destinationSpiderIp = Encoding.UTF8.GetString(tmp);

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
                Console.WriteLine("client listen ip          : {0}", clientListenIp);
                if(!string.IsNullOrEmpty(clientListenIpScopeId))
                {
                    Console.WriteLine("client listen ip scope id : {0}", clientListenIpScopeId);
                }
                Console.WriteLine("client listen port        : {0}", clientListenPort);
                Console.WriteLine("destination spider ip     : {0}", destinationSpiderIp);
                Console.WriteLine("recv/send tv_sec          : {0,7} sec", tvSec);
                Console.WriteLine("recv/send tv_usec         : {0,7} microsec", tvUsec);
                Console.WriteLine("forwarder_tv_sec          : {0,7} sec", forwarderTvSec);
                Console.WriteLine("forwarder_tv_usec         : {0,7} microsec", forwarderTvUsec);
                Console.WriteLine("");

                Console.Write("ok? (yes:y no:n quit:q)                        > ");
                input = Console.ReadLine();
                input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                check = input[0];
                if(check == 'y')
                {
                    break;
                }else if(check == 'n')
                {
                    continue;
                }else if(check == 'q'){
                    return;
                }else{
                    return;
                }
            }

            parameters = new object[] {clientListenIp,
                                       clientListenIpScopeId,
                                       clientListenPort,
                                       destinationSpiderIp,
                                       tvSec,
                                       tvUsec,
                                       forwarderTvSec,
                                       forwarderTvUsec};

            Thread thread = new Thread(new ParameterizedThreadStart(ListenClientShell));
            thread.Start(parameters);

            Thread.Sleep(2000); // 2s

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

        public void AddNodeSpiderClientToDestinationSpider()
        {
            string config = "";
            string sourceSpiderIp = "";
            string sourceSpiderIpScopeId = "";
            string destinationSpiderIp = "";
            string clientListenIp = "";
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

                    config += "destination_spider_ip:";
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

                    break;
                }else if(check == 'n')
                {
                    continue;
                }else if(check == 'q'){
                    return;
                }else{
                    return;
                }
            }

            parameters = new object[] {config,
                                       sourceSpiderIp,
                                       sourceSpiderIpScopeId,
                                       destinationSpiderIp};

            Thread thread = new Thread(new ParameterizedThreadStart(AddNodeToDestinationSpiderWorker));
            thread.Start(parameters);

            return;
        }

        public void AddNodeSpiderPipeToDestinationSpider()
        {
            string config = "";
            char mode;  // client:c server:s
            string sourceSpiderIp = "";
            string sourceSpiderIpScopeId = "";
            string destinationSpiderIp = "";
            string pipeIp = "";
            string pipeDestinationIp = "";
            string pipeDestinationPort = "";
            string pipeListenPort = "";
            string input = "";
            byte[] tmp;
            char check = 'n';
            object[] parameters;


            while(true)
            {
                Console.Write("mode (client:c server:s)                       > ");
                input = Console.ReadLine();
                input = new string(input.Where(c => !char.IsWhiteSpace(c)).ToArray());
                mode = input[0];
                if(mode == 'c')
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
                    Console.WriteLine("mode                      : {0}", mode);
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

                        break;
                    }else if(check == 'n')
                    {
                        continue;
                    }else if(check == 'q'){
                        return;
                    }else{
                        return;
                    }
                }else if(mode == 's')
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
                    Console.WriteLine("mode                      : {0}", mode);
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

            parameters = new object[] {config,
                                       sourceSpiderIp,
                                       sourceSpiderIpScopeId,
                                       destinationSpiderIp};

            Thread thread = new Thread(new ParameterizedThreadStart(AddNodeToDestinationSpiderWorker));
            thread.Start(parameters);

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
                    string destinationSpiderIp = "";
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


                    // destination_spider_ip
                    line = lines[index];
                    index++;
                    if(line == null)
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client] error");
#endif
                        return -1;
                    }

                    if(line.StartsWith("destination_spider_ip:", StringComparison.Ordinal))
                    {
                        destinationSpiderIp = line.Substring(line.IndexOf(":") + 1);
                    }

                    if(string.IsNullOrEmpty(destinationSpiderIp))
                    {
#if DEBUGPRINT
                        Console.WriteLine("[-] [client] [destination_spider_ip] error");
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
                                               destinationSpiderIp,
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

                }
            }catch(Exception)
            {
#if DEBUGPRINT
                Console.WriteLine("[-] read config error");
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
                        string destinationSpiderIp = "";
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


                        // destination_spider_ip
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client] error");
                            break;
                        }

                        if(line.StartsWith("destination_spider_ip:", StringComparison.Ordinal))
                        {
                            destinationSpiderIp = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(destinationSpiderIp))
                        {
                            Console.WriteLine("[-] [client] [destination_spider_ip] error");
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
                                                   destinationSpiderIp,
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

                    }else if(String.Compare(line, "[client_udp]") == 0)
                    {
                        string clientListenIp = "";
                        string clientListenIpScopeId = "";
                        string clientListenPort = "";
                        string destinationSpiderIp = "";
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


                        // destination_spider_ip
                        line = getLine(lineEnumerator);
                        if(line == null)
                        {
                            Console.WriteLine("[-] [client_udp] error");
                            break;
                        }

                        if(line.StartsWith("destination_spider_ip:", StringComparison.Ordinal))
                        {
                            destinationSpiderIp = line.Substring(line.IndexOf(":") + 1);
                        }

                        if(string.IsNullOrEmpty(destinationSpiderIp))
                        {
                            Console.WriteLine("[-] [client_udp] [destination_spider_ip] error");
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
                                                   destinationSpiderIp,
                                                   targetIp,
                                                   targetPort,
                                                   tvSec,
                                                   tvUsec,
                                                   forwarderTvSec,
                                                   forwarderTvUsec};

                        Thread thread = new Thread(new ParameterizedThreadStart(ClientUdpWorker));
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
    }

    public class Spider
    {
        [DllImport("kernel32.dll")]
        private static extern IntPtr GetConsoleWindow();

        [DllImport("user32.dll")]
        private static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

        private const string SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT                       = "1";
        private const string SPIDER_COMMAND_ADD_NODE_SPIDER_PIPE                         = "2";
        private const string SPIDER_COMMAND_SHOW_NODE_INFORMATION                        = "3";
        private const string SPIDER_COMMAND_SHOW_ROUTING_TABLE                           = "4";
        private const string SPIDER_COMMAND_EDIT_ROUTING_TABLE                           = "5";
        private const string SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_TCP                   = "6";
        private const string SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_UDP                   = "7";
        private const string SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_SHELL                 = "8";
        private const string SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_TO_DESTINATION_SPIDER = "9";
        private const string SPIDER_COMMAND_ADD_NODE_SPIDER_PIPE_TO_DESTINATION_SPIDER   = "10";
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
            Console.WriteLine("        =-    -: Windows c# powershell Ver: 1.21  Author: Shuichiro Endo  ");
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
            Console.WriteLine("");
        }

        public void Main(string[] args)
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

            Thread messageManagerThread = new Thread(new ThreadStart(spiderCommand.MessageManagerWorker));
            messageManagerThread.Start();

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
                Console.WriteLine("----------------------------- spider command -----------------------------");
                Console.WriteLine(" {0,2}: add node (spider client)", SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT);
                Console.WriteLine(" {0,2}: add node (spider pipe)", SPIDER_COMMAND_ADD_NODE_SPIDER_PIPE);
                Console.WriteLine(" {0,2}: show node information", SPIDER_COMMAND_SHOW_NODE_INFORMATION);
                Console.WriteLine(" {0,2}: show routing table", SPIDER_COMMAND_SHOW_ROUTING_TABLE);
                Console.WriteLine(" {0,2}: edit routing table", SPIDER_COMMAND_EDIT_ROUTING_TABLE);
                Console.WriteLine(" {0,2}: add node (spider client tcp)", SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_TCP);
                Console.WriteLine(" {0,2}: add node (spider client udp)", SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_UDP);
                Console.WriteLine(" {0,2}: add node (spider client shell)", SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_SHELL);
                Console.WriteLine(" {0,2}: add node (spider client) to destination spider", SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_TO_DESTINATION_SPIDER);
                Console.WriteLine(" {0,2}: add node (spider pipe) to destination spider", SPIDER_COMMAND_ADD_NODE_SPIDER_PIPE_TO_DESTINATION_SPIDER);
                Console.WriteLine(" {0,2}: exit", SPIDER_COMMAND_EXIT);
                Console.WriteLine("--------------------------------------------------------------------------");
                Console.WriteLine("");
                Console.Write("command > ");

                spiderCommandInput = Console.ReadLine();

                switch(spiderCommandInput)
                {
                    case SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT:
                        Console.WriteLine("[+] add node (spider client)");
                        spiderCommand.AddNodeSpiderClient();
                        break;

                    case SPIDER_COMMAND_ADD_NODE_SPIDER_PIPE:
                        Console.WriteLine("[+] add node (spider pipe)");
                        spiderCommand.AddNodeSpiderPipe();
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

                    case SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_TO_DESTINATION_SPIDER:
                        Console.WriteLine("[+] add node (spider client) to destination spider");
                        spiderCommand.AddNodeSpiderClientToDestinationSpider();
                        break;

                    case SPIDER_COMMAND_ADD_NODE_SPIDER_PIPE_TO_DESTINATION_SPIDER:
                        Console.WriteLine("[+] add node (spider pipe) to destination spider");
                        spiderCommand.AddNodeSpiderPipeToDestinationSpider();
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
"@

Add-Type -TypeDefinition $csharpcode -Language CSharp

$spider = New-Object spider.Spider

$args = @()
#$args = @("-h")
#$args = @("-4", "192.168.0.31")
#$args = @("-d", "-i", "192.168.0.25", "-p", "1025")
#$args = @("-4", "192.168.0.31", "-e", "x", "-k", "deadbeef")
#$args = @("-4", "192.168.0.31", "-e", "a", "-k", "47a2baa1e39fa16752a2ea8e8e3e24256b3c360f382b9782e2e57d4affb19f8c", "-v", "c87114c8b36088074c7ec1398f5c168a")

$spider.Main($args)

