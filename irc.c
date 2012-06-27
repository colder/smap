#include <string.h>
#include "client.h"
#include "socket.h"
#include "irc.h"

/* Modules includes */
#include "modules/protocol.h"
#include "modules/invite.h"
#include "modules/command.h"
#include "modules/social.h"

int irc_write_msg(irc_ctx *ctx, const char *format, ...) {
	int size, res;
	va_list args;
	char *msg = malloc(sizeof(char)*MSG_MAXLEN);
	bzero(msg, MSG_MAXLEN);

	va_start (args, format);
	size = vsnprintf(msg, MSG_MAXLEN-2, format, args);
	va_end (args);

	msg[size++] = '\r';
	msg[size++] = '\n';

	res = socket_nwrite(ctx->socket, msg, size);
	free(msg);
	return res;
}

irc_ctx *irc_init_context(const char *host, int port) {
	irc_ctx *ctx         = malloc(sizeof(irc_ctx));
	ctx->client.nick     = strdup("smap");
	ctx->client.realname = strdup("Social Mapper");
	ctx->server.host     = strdup(host);
	ctx->server.port     = port;

	ctx->modules.count     = 0;
	ctx->modules.max       = 10;
	ctx->modules.handlers  = malloc(sizeof(irc_module)*10);

	return ctx;
}

void irc_register_module(irc_ctx *ctx, irc_module_callback init, irc_module_callback handle, irc_module_callback shutdown) {
	if (ctx->modules.count < ctx->modules.max) {
		irc_module *module = malloc(sizeof(irc_module));
		module->init      = init;
		module->handle    = handle;
		module->shutdown  = shutdown;
		ctx->modules.handlers[ctx->modules.count++] = module;
	}
}

void irc_free_context(irc_ctx *ctx) {

	if (ctx->client.nick) {
		free(ctx->client.nick);
	}

	if (ctx->client.realname) {
		free(ctx->client.realname);
	}

	if (ctx->server.host) {
		free(ctx->server.host);
	}

	free(ctx);
}

int irc_nsplit_string(char ***dest, const char *src, int len, const char delim, int limit) {
	int count = 0, size = 1;
	const char *pos = src;

	*dest = malloc(sizeof(char *)*size);

	while((limit < 0 || count < limit) && pos && pos < &src[len]) {
		const char *next;

		if (count >= size) {
			size *= 3;
			*dest = realloc(*dest, sizeof(char *)*size);
		}

		if (count == limit-1) {
			next = &src[len];
		} else {
			next = strchr(pos, delim) ?: &src[len];
		}
		(*dest)[count] = strndup(pos, next-pos);
		pos = &next[1];

		count++;
	}

	if (size > count) {
		*dest = realloc(*dest, sizeof(char *)*count);
	}

	return count;
}

irc_cmd *irc_parse_line(const char *line) {
	irc_cmd *cmd = malloc(sizeof(irc_cmd));
	const char *cmd_start = line;
	const char *trailing_start, *params_start, *params_end, *line_end;

	if (line[0] == ':') {
		/* prefix used */
		cmd_start = strchr(line, ' ');
		cmd->prefix = strndup(&line[1], cmd_start-line-1);
		cmd_start++;
	} else {
		cmd->prefix = NULL;
	}

	trailing_start = strchr(cmd_start, ':');

	line_end     = strchr(cmd_start, '\r');
	params_start = &strchr(cmd_start, ' ')[1];
	params_end   = trailing_start ? &trailing_start[-1] : line_end;

	cmd->command = strndup(cmd_start, params_start-cmd_start-1);

	cmd->params_count = irc_nsplit_string(&cmd->params, params_start, params_end-params_start, ' ', -1);

	if (trailing_start) {
		cmd->trailing = strndup(&trailing_start[1], line_end-trailing_start-1);
	} else {
		cmd->trailing = NULL;
	}
	
	return cmd;
}

void irc_free_cmd(irc_cmd *cmd) {

	while (cmd->params_count > 0) {
		free(cmd->params[--cmd->params_count]);
	}

	free(cmd->params);

	if (cmd->prefix) {
		free(cmd->prefix);
	}

	if (cmd->command) {
		free(cmd->command);
	}

	if (cmd->trailing) {
		free(cmd->trailing);
	}


	free(cmd);
	cmd = NULL;
}

int irc_dispatch_to_modules(irc_ctx *ctx) {
	int i;
	/* the list of callbaks is ordered in layers, each layer is able to mask
	 * the current command to the sub-layers by returning IRCM_CMD_MASK */
	for (i = 0; i < ctx->modules.count; i++) {
		int res = ctx->modules.handlers[i]->handle(ctx);
		if (res == IRCM_CMD_MASK) {
			break;
		}
	}
	return SUCCESS;
}

void irc_startup(irc_ctx *ctx) {
	int i;

	/* Modules registration */
	M_REGISTER(protocol);
	M_REGISTER(invite);
	M_REGISTER(social);

	for (i = 0; i < ctx->modules.count; i++) {
		ctx->modules.handlers[i]->init(ctx);
	}

	ctx->socket = socket_connect(ctx->server.host, ctx->server.port);
}

int irc_idle(irc_ctx *ctx) {
	char *line = malloc(sizeof(char)*(IRC_LINE_MAXLEN+1));
	line[IRC_LINE_MAXLEN] = '\0';

	while (socket_read_line(ctx->socket, line, IRC_LINE_MAXLEN) > 0) {
		ctx->active_cmd = irc_parse_line(line);
		if (!ctx->active_cmd) {
			debug_err("Can't parse line");
			continue;
		}
		irc_dispatch_to_modules(ctx);
		irc_free_cmd(ctx->active_cmd);
	}

	free(line);
	return SUCCESS;
}

void irc_shutdown(irc_ctx *ctx) {
	int i;
	for (i = 0; i < ctx->modules.count; i++) {
		ctx->modules.handlers[i]->shutdown(ctx);
		free(ctx->modules.handlers[i]);
	}
	free(ctx->modules.handlers);

	socket_close(ctx->socket);
	irc_free_context(ctx);
}


