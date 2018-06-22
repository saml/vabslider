#ifndef VABSLIDER_FILTER_GRAPH_H
#define VABSLIDER_FILTER_GRAPH_H

#include <libavfilter/avfilter.h>

struct FilterGraph {
    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *graph;
};

int init_filter_graph(struct FilterGraph *filters, struct InputVideo *vid, char const* graph_desc);
int free_filter_graph(struct FilterGraph *filters);
#endif