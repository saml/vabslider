#ifndef VABSLIDER_FILTER_GRAPH_H
#define VABSLIDER_FILTER_GRAPH_H

#include <libavfilter/avfilter.h>

struct FilterGraph {
    AVFilterContext *vid1_out;
    AVFilterContext *vid1_in;
    AVFilterContext *vid2_out;
    AVFilterContext *vid2_in;
    AVFilterGraph *graph;
};

int init_filter_graph(struct FilterGraph *filters, struct InputVideo *larger, struct InputVideo *smaller);
int free_filter_graph(struct FilterGraph *filters);
#endif