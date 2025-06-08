CC       = g++
CFLAGS   =
LDFLAGS  = -pthread -lcares -lssl -lcrypto

SRCS    += spiderip.cpp
SRCS    += encryption.cpp
SRCS    += xor.cpp
SRCS    += aes.cpp
SRCS    += caresmanager.cpp
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
SRCS    += spidercommand.cpp
SRCS    += spider.cpp

OBJS     = $(SRCS:.cpp=.o)
TARGET   = spider

INCDIR   = -I./include -I./include/openssl -I.
LIBDIR   = -L./lib
LIBS     =
OPTS     = -std=c++20
STATIC   = -static


all:       clean $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OPTS) $(STATIC) -o $@ $^ $(LIBDIR) $(LIBS) $(LDFLAGS)

$(OBJS):   $(SRCS)
	$(CC) $(OPTS) $(CFLAGS) $(INCDIR) -c $(SRCS)
clean:
	rm -f $(OBJS) $(TARGET)

