
#include "../irc.h"
#include "../client.h"
#include "protocol.h"

protocol_state *state;

M_INIT(protocol) {
	state = malloc(sizeof(protocol_state));
	state->registered = 0;
	return SUCCESS;
}

M_HANDLE(protocol) {
	irc_cmd *cmd = ctx->active_cmd;
	if (strcmp(cmd->command, "NOTICE") == 0 && !state->registered) {
		irc_write_msg(ctx, "USER %s 0 * :%s", ctx->client.nick , ctx->client.realname);
		irc_write_msg(ctx, "NICK %s", ctx->client.nick);
		state->registered = 1;
		return IRCM_CMD_MASK;
	} else if (strcmp(cmd->command, "PING") == 0) {
		irc_write_msg(ctx, "PONG :%s", ctx->active_cmd->trailing);
		return IRCM_CMD_MASK;
	}
	return IRCM_CMD_THROUGH;
}

M_SHUTDOWN(protocol) {
	free(state);
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
