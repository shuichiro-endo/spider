/*
 * Title:  spider Socks5.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

/*
 * Reference:
 * https://www.ietf.org/rfc/rfc1928.txt
 * https://www.ietf.org/rfc/rfc1929.txt
 */

using System;
using System.Runtime.InteropServices;

namespace spider
{
/*
    The client connects to the server, and sends a version identifier/method selection message:

    +-----+-------------+-------------+
    | VER |   NMETHODS  |   METHODS   |
    +-----+-------------+-------------+
    |  1  |      1      |   1 to 255  |
    +-----+-------------+-------------+
*/
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

/*
    The server selects from one of the methods given in METHODS, and sends a METHOD selection message:

    +-----+----------+
    | VER |  METHOD  |
    +-----+----------+
    | 1   |   1      |
    +-----+----------+

    If the selected METHOD is X'FF', none of the methods listed by the
    client are acceptable, and the client MUST close the connection.

    The values currently defined for METHOD are:

           o  X'00' NO AUTHENTICATION REQUIRED
           o  X'01' GSSAPI
           o  X'02' USERNAME/PASSWORD
           o  X'03' to X'7F' IANA ASSIGNED
           o  X'80' to X'FE' RESERVED FOR PRIVATE METHODS
           o  X'FF' NO ACCEPTABLE METHODS

*/
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

/*
    Once the SOCKS V5 server has started, and the client has selected the
    Username/Password Authentication protocol, the Username/Password
    subnegotiation begins.  This begins with the client producing a
    Username/Password request:

    +-----+--------+-------------+--------+-------------+
    | VER |  ULEN  |    UNAME    |  PLEN  |    PASSWD   |
    +-----+--------+-------------+--------+-------------+
    |  1  |   1    |   1 to 255  |   1    |   1 to 255  |
    +-----+--------+-------------+--------+-------------+


    The VER field contains the current version of the subnegotiation,
    which is X'01'. The ULEN field contains the length of the UNAME field
    that follows. The UNAME field contains the username as known to the
    source operating system. The PLEN field contains the length of the
    PASSWD field that follows. The PASSWD field contains the password
    association with the given UNAME.
*/
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

/*
    The server verifies the supplied UNAME and PASSWD, and sends the
    following response:

    +-----+----------+
    | VER |  STATUS  |
    +-----+----------+
    | 1   |   1      |
    +-----+----------+

    A STATUS field of X'00' indicates success. If the server returns a
    `failure' (STATUS value other than X'00') status, it MUST close the
    connection.
*/
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

/*
    The SOCKS request is formed as follows:

    +-----+-------+---------+--------+-------------+-------------+
    | VER |  CMD  |   RSV   |  ATYP  |   DST.ADDR  |   DST.PORT  |
    +-----+-------+---------+--------+-------------+-------------+
    |  1  |   1   |  X'00'  |   1    |   Variable  |      2      |
    +-----+-------+---------+--------+-------------+-------------+

      Where:

           o  VER    protocol version: X'05'
           o  CMD
              o  CONNECT X'01'
              o  BIND X'02'
              o  UDP ASSOCIATE X'03'
           o  RSV    RESERVED
           o  ATYP   address type of following address
              o  IP V4 address: X'01'
              o  DOMAINNAME: X'03'
              o  IP V6 address: X'04'
           o  DST.ADDR       desired destination address
           o  DST.PORT desired destination port in network octet
              order
*/
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
        private byte[] dstPort;

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

/*
    The SOCKS request information is sent by the client as soon as it has
    established a connection to the SOCKS server, and completed the
    authentication negotiations.  The server evaluates the request, and
    returns a reply formed as follows:

    +-----+-------+---------+--------+-------------+-------------+
    | VER |  REP  |   RSV   |  ATYP  |   BND.ADDR  |   BND.PORT  |
    +-----+-------+---------+--------+-------------+-------------+
    |  1  |   1   |  X'00'  |   1    |   Variable  |      2      |
    +-----+-------+---------+--------+-------------+-------------+

      Where:

           o  VER    protocol version: X'05'
           o  REP    Reply field:
              o  X'00' succeeded
              o  X'01' general SOCKS server failure
              o  X'02' connection not allowed by ruleset
              o  X'03' Network unreachable
              o  X'04' Host unreachable
              o  X'05' Connection refused
              o  X'06' TTL expired
              o  X'07' Command not supported
              o  X'08' Address type not supported
              o  X'09' to X'FF' unassigned
           o  RSV    RESERVED
           o  ATYP   address type of following address
              o  IP V4 address: X'01'
              o  DOMAINNAME: X'03'
              o  IP V6 address: X'04'
           o  BND.ADDR       server bound address
           o  BND.PORT       server bound port in network octet order
*/
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
}
