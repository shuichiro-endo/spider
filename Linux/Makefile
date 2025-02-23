CC       = g++
CFLAGS   =
LDFLAGS  = -pthread

SRCS    += spiderip.cpp
SRCS    += encryption.cpp
SRCS    += xor.cpp
SRCS    += aes.cpp
SRCS    += route.cpp
SRCS    += message.cpp
SRCS    += routingmessagequeue.cpp
SRCS    += socks5messagequeue.cpp
SRCS    += socks5message.cpp
SRCS    += routingmessage.cpp
SRCS    += routingmanager.cpp
SRCS    += node.cpp
SRCS    += pipe.cpp
SRCS    += pipemanager.cpp
SRCS    += client.cpp
SRCS    += clientmanager.cpp
SRCS    += server.cpp
SRCS    += servermanager.cpp
SRCS    += messagemanager.cpp
SRCS    += spider.cpp

OBJS     = $(SRCS:.cpp=.o)
TARGET   = spider

INCDIR   = -I.
LIBDIR   =
LIBS     =
OPTS     = -std=c++20


all:       clean $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OPTS) -o $@ $^ $(LIBDIR) $(LIBS) $(LDFLAGS)

$(OBJS):   $(SRCS)
	$(CC) $(OPTS) $(CFLAGS) $(INCDIR) -c $(SRCS)
clean:
	rm -f $(OBJS) $(TARGET)

