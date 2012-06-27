#ifndef SOCIAL_H
#define SOCIAL_H

#include "../irc.h"


typedef struct _set_node set_node;

struct _set_node {
	set_node *left;
	set_node *right;
	char *id;
	void *data;
};

typedef struct _set {
	set_node *top;
	int size;
} set;

typedef struct _graph_edge graph_edge;

typedef struct _graph_node {
	graph_edge **edges_p;
	int edges_size;
	int edges_max_size;
	void *data;
	int marked;
} graph_node;

struct _graph_edge {
	graph_node *from;
	graph_node *to;
	int weight;
};

typedef struct _graph {
	graph_node **nodes;
	int nodes_size;
	int nodes_max_size;
	graph_edge **edges;
	int edges_size;
	int edges_max_size;
} graph;

typedef struct _social_map {
	graph *graph;
	set   *users;
	int   lastDump;
} social_map;



M_INIT(social);
M_HANDLE(social);
M_SHUTDOWN(social);

#endif /* SOCIAL_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
