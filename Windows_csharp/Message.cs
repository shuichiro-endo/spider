/*
 * Title:  spider Message.cs (Windows csharp)
 * Author: Shuichiro Endo
 */

namespace spider
{
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
}

