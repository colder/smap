#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "../irc.h"

typedef struct protocol_state {
	int registered;
} protocol_state;

M_INIT(protocol);
M_HANDLE(protocol);
M_SHUTDOWN(protocol);

#endif /* PROTOCOL_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
