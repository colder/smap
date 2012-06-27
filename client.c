#include "client.h"
#include "irc.h"


int main(int argc, char *argv[]){
		irc_ctx *ctx;

		if(argc < 3){
				fprintf(stderr, "Usage: %s <hostname> <port>\n", argv[0]);
				exit(1); 
		}

		ctx = irc_init_context(argv[1], atoi(argv[2]));
		irc_startup(ctx);
		irc_idle(ctx);
		irc_shutdown(ctx);

		return 0;
}

char *trim(const char *in) {
	int i;
	int len = strlen(in);
	for (i = len-1; i >= 0; i--) {
		if (in[i] != '\r' && in[i] != '\n' && in[i] != ' ') {
			break;
		}
	}

	return strndup(in, i+1);
}
void msg_out(const char *msg) {
	char *msgt = trim(msg);
	fprintf(stdout, "[>] %s\n", msgt);
	free(msgt);
	fflush(stdout);
}

void msg_in(const char *msg) {
	char *msgt = trim(msg);
	fprintf(stdout, "[<] %s\n", msgt);
	free(msgt);
	fflush(stdout);
}

void msg_err(const char *msg) {
	char *msgt = trim(msg);
	fprintf(stdout, "[!] %s\n", msgt);
	free(msgt);
	fflush(stderr);
}

