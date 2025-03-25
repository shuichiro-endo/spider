/*
 * Title:  spider Socks5MessageQueue.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;
using System.Collections.Generic;
using System.Threading;

namespace spider
{
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
            queue.Enqueue(message);
            guard.Release();
            token.Release();
        }

        public int PushTimeout(Socks5Message message,
                               int tv_sec,
                               int tv_usec)
        {
            int ret = 0;
            int t = tv_sec * 1000 + tv_usec / 1000; // milliseconds

            if(guard.Wait(t))
            {
                queue.Enqueue(message);
                guard.Release();
                token.Release();
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
}

