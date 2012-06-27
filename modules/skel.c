
#include "../irc.h"
#include "../client.h"

M_INIT(skel) {
	return SUCCESS;
}

M_HANDLE(skel) {
	irc_cmd *cmd = ctx->active_cmd;
	return IRCM_CMD_THROUGH;
}

M_SHUTDOWN(skel) {
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
