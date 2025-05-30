/*
 * Title:  spider RoutingMessageQueue.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

using System;
using System.Collections.Generic;
using System.Threading;

namespace spider
{
    public class RoutingMessageQueue
    {
        private const int ROUTING_MESSAGE_QUEUE_CAPACITY = 100;
        private Queue<RoutingMessage> queue = new Queue<RoutingMessage>();
        private SemaphoreSlim token = new SemaphoreSlim(0, ROUTING_MESSAGE_QUEUE_CAPACITY);
        private SemaphoreSlim guard = new SemaphoreSlim(1, 1);
        private int count = 0;

        public RoutingMessageQueue()
        {

        }

        ~RoutingMessageQueue()
        {

        }

        public int GetCount()
        {
            return count;
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
                count++;
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
                    count++;
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
            count--;
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
                count--;
                guard.Release();
            }else
            {
#if DEBUGPRINT
                Console.WriteLine("[-] pop timeout");
#endif
            }

            return message;
        }

        public RoutingMessage PopLatestMessage()
        {
            RoutingMessage message = null;

            for(int i=count; i>0; i--)
            {
                message = this.Pop();
            }

            return message;
        }
    }
}

