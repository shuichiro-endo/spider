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

        public RoutingMessageQueue()
        {

        }

        ~RoutingMessageQueue()
        {

        }

        public void Push(RoutingMessage message)
        {
            guard.Wait();
            queue.Enqueue(message);
            guard.Release();
            token.Release();
        }

        public int PushTimeout(RoutingMessage message,
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
}

