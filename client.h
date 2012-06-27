#ifndef CLIENT_H
#define CLIENT_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define SUCCESS 0
#define FAILURE 1

#define NICK_MAXLEN 30
#define RNAME_MAXLEN 60

#define MSG_MAXLEN 1024

char *trim(const char *in);

void msg_in(const char *msg);
void msg_out(const char *msg);
void msg_err(const char *msg);

#define G(name) _global_##name

#ifdef DEBUG
#define debug_in(msg) msg_in(msg)
#define debug_out(msg) msg_out(msg)
#define debug_err(msg) msg_err(msg)
#else
#define debug_in(msg)
#define debug_out(msg)
#define debug_err(msg)
#endif

#endif /* CLIENT_H */

