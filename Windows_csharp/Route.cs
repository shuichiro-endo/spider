/*
 * Title:  spider Route.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;

namespace spider
{
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
}

