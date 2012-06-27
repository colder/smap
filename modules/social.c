
#include "../irc.h"
#include "../client.h"
#include "social.h"
#include <time.h>

#define GRAPH_INIT_NODES 10
#define GRAPH_INIT_EDGES 100
#define GRAPH_INIT_EPN   10

#define PATH_NODES "data/nodes.txt"
#define PATH_EDGES "data/edges.txt"
#define PATH_GRAPH "data/social.dot"

#define NICK_SIZE 80

void set_init(set *s) {
	s->size = 0;
	s->top = NULL;
}


set_node *set_add_or_get(set *s, const char *id, int do_add) {
	set_node **n = &s->top;
	while(*n) {
		int cmp = strcmp(id, (*n)->id);
		if (cmp > 0) {
			n = &(*n)->right;
		} else if (cmp < 0) {
			n = &(*n)->left;
		} else {
			return *n;
		}
	}

	if (do_add) {
		/* we are in a leaf */
		*n = malloc(sizeof(set_node));
		(*n)->right = NULL;
		(*n)->left  = NULL;
		(*n)->data  = NULL;
		(*n)->id    = strdup(id);

		s->size++;
		return *n;
	}

	return NULL;
}

void inline graph_edges_resize(graph *g, int n) {
	g->edges = realloc(g->edges, sizeof(graph_edge *)*n);
	g->edges_max_size = n;
}

void inline graph_node_edges_resize(graph_node *node, int n) {
	node->edges_p = realloc(node->edges_p, sizeof(graph_edge *)*n);
	node->edges_max_size = n;
}

void inline graph_nodes_resize(graph *g, int n) {
	g->nodes = realloc(g->nodes, sizeof(graph_node *)*n);
	g->nodes_max_size = n;
}

void graph_init(graph *g) {
	g->edges_size     = 0;
	g->edges_max_size = 0;
	g->nodes_size     = 0;
	g->nodes_max_size = 0;
	g->edges = NULL;
	g->nodes = NULL;
	graph_edges_resize(g, GRAPH_INIT_EDGES);
	graph_nodes_resize(g, GRAPH_INIT_NODES);
}

void graph_free(graph *g) {
	//TODO
}

graph_node *graph_add_node(graph *g, void *data) {
	graph_node *node;
	if (g->nodes_size == g->nodes_max_size) {
		graph_nodes_resize(g, g->nodes_max_size*2);
	}

	node = g->nodes[g->nodes_size++] = malloc(sizeof(graph_node));
	node->edges_size = 0;
	node->edges_p = NULL;
	graph_node_edges_resize(node, GRAPH_INIT_EPN);
	node->marked = 0;
	node->data   = data;

	return node;
}

int graph_add_or_update_edge(graph *g, graph_node *from, graph_node *to, int w, int allow_update) {
	int i;
	graph_edge *edge;
	for (i = 0; i < from->edges_size; i++) {
		edge = from->edges_p[i];
		if (edge->to == to) {
			if (allow_update) {
				edge->weight += w;
				return SUCCESS;
			} else {
				return FAILURE;
			}
		}
	}
	/* we haven't found the node, let's add it */

	if (from->edges_size == from->edges_max_size) {
		graph_node_edges_resize(from, from->edges_max_size*2);
	}
	if (g->edges_size == g->edges_max_size) {
		graph_edges_resize(g, g->edges_max_size*2);
	}

	edge = g->edges[g->edges_size++] = malloc(sizeof(graph_edge));
	edge->from   = from;
	edge->to     = to;
	edge->weight = w;

	from->edges_p[from->edges_size++] = edge;

	return SUCCESS;
}
#define graph_add_edge(g, from,to, w) \
	graph_add_or_update_edge((g), (from), (to), (w), 0)



int social_is_nick(const char *src, int len) {
	int i;
	for (i = 0; i < len; i++) {
		int c = (int)src[i];
		if (!(c >= 0x30 && c <=0x39) /*digit*/
			&& !(c >= 0x41 && c <=0x7d) /* A-Z a-z special */
			&& c != '-') { 
			return 0;
		}
	}
	return 1;
}

social_map *smap;

void social_save_graph(social_map *smap, const char *path) {
	int i;
	FILE *dest = fopen(path, "w");
	fputs("digraph G {\noverlap = false;\nnode [width=.3, height=.3];\n", dest);
	for (i = 0; i < smap->graph->edges_size; i++) {
		graph_edge *e = smap->graph->edges[i];
		char *nick_from = ((set_node *)e->from->data)->id;
		char *nick_to = ((set_node *)e->to->data)->id;
		/*    "nick_to" -> "nick_to"\n */
		fprintf(dest, "    \"%s\" -> \"%s\" [label=\"%d\"];\n", nick_from, nick_to, e->weight);
	}

	fputs("}\n", dest);
	fclose(dest);
}

void social_map_init(social_map *smap) {

	smap->users = malloc(sizeof(set));
	set_init(smap->users);

	smap->graph = malloc(sizeof(graph));
	graph_init(smap->graph);
	
	smap->lastDump = (int)time(NULL);

}

void social_add_nick(social_map *smap, const char *from) {
	set_add_or_get(smap->users, from, 1);
}

void social_dump_set_node(FILE *dest_nodes, set_node *n) {
	fprintf(dest_nodes, "%s\n", n->id);
	if (n->left) {
		social_dump_set_node(dest_nodes, n->left);
	}
	if (n->right) {
		social_dump_set_node(dest_nodes, n->right);
	}
}

void social_dump_data(social_map *smap, const char *path_edges, const char *path_nodes) {
	FILE *dest_edges = fopen(path_edges, "w");
	FILE *dest_nodes = fopen(path_nodes, "w");
	int i;
	/* dump nodes */
	if (smap->users->top) {
		social_dump_set_node(dest_nodes, smap->users->top);
	}
	fclose(dest_nodes);
	/* dump edges */
	for (i = 0; i < smap->graph->edges_size; i++) {
		graph_edge *e = smap->graph->edges[i];
		char *nick_from = ((set_node *)e->from->data)->id;
		char *nick_to = ((set_node *)e->to->data)->id;
		/*    "nick_to" -> "nick_to"\n */
		fprintf(dest_edges, "%s %s %d\n", nick_from, nick_to, e->weight);
	}
	fclose(dest_edges);
}

void social_load_data(social_map *smap, const char *path_edges, const char *path_nodes) {
	FILE *src_edges = fopen(path_edges, "r");
	FILE *src_nodes = fopen(path_nodes, "r");
	
	if (src_edges && src_nodes) {
		char from[NICK_SIZE+1];
		char to[NICK_SIZE+1];
		bzero(from, NICK_SIZE+1);
		bzero(to,   NICK_SIZE+1);
		int w;
		int n = 0;
		while (feof(src_nodes) == 0 && fscanf(src_nodes, "%s", from) == 1) {
			if (from[0]) {
				set_add_or_get(smap->users, from, 1);
			}
		}

		while (feof(src_edges) == 0 && fscanf(src_edges, "%s %s %d", from, to, &w) == 3) {
			n++;
			set_node *nfrom = set_add_or_get(smap->users, from, 0);
			set_node *nto = set_add_or_get(smap->users, to, 0);

			if (!nto || !nfrom) {
				fprintf(stdout, "[!!] Error here, nicks not found!");
				fflush(stdout);
				continue;
			}

			if (!nfrom->data) {
				nfrom->data = graph_add_node(smap->graph, nfrom);
			}

			if (!nto->data) {
				nto->data = graph_add_node(smap->graph, nto);
			}

			if (nfrom->data != nto->data) {
				graph_add_or_update_edge(smap->graph, (graph_node *) nfrom->data, (graph_node *) nto->data, w, 1);
			}
		}

	}

}

void social_add_quote(social_map *smap, const char *from, const char *to) {
	set_node *nfrom = set_add_or_get(smap->users, from, 1);
	set_node *nto = set_add_or_get(smap->users, to, 0);

	if (!nfrom->data) {
		/* new entry in the set, we add a corresponding node to the graph */
		nfrom->data = graph_add_node(smap->graph, nfrom);
	}

	if (!nto) {
		/* nick targetted by the quote is not found, probably not a quote */
		return;
	}

	if (!nto->data) {
		/* new entry in the set, we add a corresponding node to the graph */
		nto->data = graph_add_node(smap->graph, nto);
	}

	if (nfrom->data != nto->data) {
		graph_add_or_update_edge(smap->graph, (graph_node *) nfrom->data, (graph_node *) nto->data, 1, 1);
	}
	
}


M_INIT(social) {
	smap = malloc(sizeof(social_map));
	social_map_init(smap);
	social_load_data(smap, PATH_EDGES, PATH_NODES);
	return SUCCESS;
}

M_HANDLE(social) {
	irc_cmd *cmd = ctx->active_cmd;
	if (strcmp(cmd->command, "PRIVMSG") == 0) {

		const char *quote_chars = ",:.;";
		char       *min_pos     = NULL;
		int         i;
		char       *nick_end = strchr(cmd->prefix, '!');
		char       *from;

		if (nick_end) {
			from = strndup(cmd->prefix, nick_end-cmd->prefix);

			if (strncmp(cmd->trailing, "!j", sizeof("!j")-1) == 0 
				&& strcmp(from, "ekneuss") == 0) {
				char *chan = strchr(cmd->trailing, '#');
				if (chan) {
					irc_write_msg(ctx, "JOIN %s", chan);
				}
				free(from);
				return IRCM_CMD_MASK;
			}
			if (strncmp(cmd->trailing, "!gstate", sizeof("!gstate")-1) == 0 
				&& strcmp(from, "ekneuss") == 0) {

				irc_write_msg(ctx, "PRIVMSG %s :Nicks: %d, Graph: [N: %d, E: %d]", from, smap->users->size, smap->graph->nodes_size, smap->graph->edges_size);
				free(from);
				return IRCM_CMD_MASK;
			}
			if (strncmp(cmd->trailing, "!gdump", sizeof("!gdump")-1) == 0 
				&& strcmp(from, "ekneuss") == 0) {

				int t = (int)time(NULL);
				social_save_graph(smap, PATH_GRAPH);
				social_dump_data(smap, PATH_EDGES, PATH_NODES);
				smap->lastDump = t;

				free(from);
				return IRCM_CMD_MASK;
			}

			if (CMD_PARAM(cmd, 0)[0] == '#') {
				/* try to identify a quote */
				for (i = 0; quote_chars[i]; i++) {
					char *pos = strchr(cmd->trailing, quote_chars[i]);
					if (pos && (!min_pos || pos < min_pos)) {
						min_pos = pos;
					}
				}

				/* we found a possible quote char, let's see if it's a tangible
				 * quote */
				if (min_pos && social_is_nick(cmd->trailing, min_pos-cmd->trailing)) {
						int t = (int)time(NULL);
						char *to = strndup(cmd->trailing, min_pos-cmd->trailing);
						social_add_quote(smap, from, to);

						if (t > smap->lastDump+60) {
							social_save_graph(smap, PATH_GRAPH);
							social_dump_data(smap, PATH_EDGES, PATH_NODES);
							smap->lastDump = t;
						}

						free(to);
				} else {
					social_add_nick(smap, from);
				}

			}
			free(from);
		}
	}
	return IRCM_CMD_THROUGH;
}

M_SHUTDOWN(social) {
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
