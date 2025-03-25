/*
 * Title:  spider SpiderIp.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;
using System.Text;

namespace spider
{
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
}

