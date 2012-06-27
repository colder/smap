
#include "../irc.h"
#include "../client.h"

M_INIT(invite) {
	return SUCCESS;
}

M_HANDLE(invite) {
	irc_cmd *cmd = ctx->active_cmd;
	if (strcmp(cmd->command, "INVITE") == 0) {
		if (strcmp(ctx->client.nick, CMD_PARAM(cmd, 0)) == 0) {
			if (CMD_PARAM(cmd, 1)[0] == '#') {
				irc_write_msg(ctx, "JOIN %s", CMD_PARAM(cmd, 1));
			} else {
				irc_write_msg(ctx, "JOIN %s", cmd->trailing);
			}
		}
		return IRCM_CMD_MASK;
	}
	return IRCM_CMD_THROUGH;
}

M_SHUTDOWN(invite) {
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
