#ifndef IRC_H
#define IRC_H

#define IRC_LINE_MAXLEN 1024

#define M_REGISTER(name) \
	irc_register_module(ctx, &ircm_ ##name## _init, &ircm_ ##name## _handle, &ircm_ ##name## _shutdown)

#define M_INIT(name) \
	int ircm_ ##name## _init(irc_ctx *ctx)

#define M_HANDLE(name) \
	int ircm_ ##name## _handle(irc_ctx *ctx)

#define M_SHUTDOWN(name) \
	int ircm_ ##name## _shutdown(irc_ctx *ctx)

typedef struct _irc_ctx irc_ctx;

typedef int (*irc_module_callback)(struct _irc_ctx *ctx);

typedef struct _irc_cmd {
	char  *prefix;
	char  *command;
	char **params;
	int    params_count;
	char  *trailing;
} irc_cmd;

typedef struct _irc_module {
	irc_module_callback init;
	irc_module_callback handle;
	irc_module_callback shutdown;
} irc_module;

typedef struct _irc_modules {
	int count;
	int max;
	irc_module **handlers;
} irc_modules;

struct _irc_ctx {
	int socket;
	struct {
		char *host;
		int   port;
	} server;
	struct {
		char *nick;
		char *realname;
	} client;

	irc_cmd *active_cmd;
	irc_modules modules;
};

#define CMD_PARAM(cmd, i) ((i) >= 0 && (i) < (cmd)->params_count ? (cmd)->params[i] : "")

#define IRCM_CMD_THROUGH  0x000
#define IRCM_CMD_MASK     0x001

int irc_write_msg(irc_ctx *ctx, const char *format, ...);
irc_ctx *irc_init_context(const char *host, int port);
void irc_startup(irc_ctx *ctx);
int irc_idle(irc_ctx *ctx);
void irc_shutdown(irc_ctx *ctx);

#endif /* IRC_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
