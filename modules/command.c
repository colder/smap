
#include "../irc.h"
#include "../client.h"

int command_handle_private(irc_ctx *ctx) {
	//irc_cmd *cmd = ctx->active_cmd;
	return IRCM_CMD_MASK;
}

int command_handle_public(irc_ctx *ctx) {
	//irc_cmd *cmd = ctx->active_cmd;
	return IRCM_CMD_MASK;
}

M_INIT(command) {
	return SUCCESS;
}

M_HANDLE(command) {
	irc_cmd *cmd = ctx->active_cmd;
	if (strcmp(cmd->command, "PRIVMSG") == 0) {
		if (strcmp(ctx->client.nick, CMD_PARAM(cmd, 0)) == 0) {
			return command_handle_private(ctx);
		} else if (CMD_PARAM(cmd, 0)[0] == '#') {
			return command_handle_public(ctx);
		}
	}
	return IRCM_CMD_THROUGH;
}

M_SHUTDOWN(command) {
	return SUCCESS;
}
/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
