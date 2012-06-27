SOURCES= client.c irc.c socket.c modules/invite.c modules/command.c modules/social.c modules/protocol.c
CC= gcc
INCLUDES=
CFLAGS= -O0 -g -Wall -DDEBUG -D_GNU_SOURCE
SLIBS= 
PROGRAM= client

OBJECTS=$(SOURCES:.c=.o)

.KEEP_STATE:

all debug: $(PROGRAM)

$(PROGRAM): $(INCLUDES) $(OBJECTS)
		${CC} ${CFLAGS} ${INCLUDES} -o $@ $(OBJECTS) $(SLIBS)

clean:
		rm -f $(PROGRAM) $(OBJECTS)
