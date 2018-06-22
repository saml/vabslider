#include <stdio.h>

#include <libavfilter/avfilter.h>
#include <libavutil/opt.h>

#include "input_video.h"
#include "filter_graph.h"

int init_filter_graph(struct FilterGraph *filters, struct InputVideo *vid, char const* graph_desc) {
    char args[512];
    int ret;
    const AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    if (!outputs) {
        fprintf(stderr, "Failed to allocate filter outputs\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    if (!inputs) {
        fprintf(stderr, "Failed to allocate filter inputs\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }
    AVRational time_base = vid->format_ctx->streams[vid->video_stream_idx]->time_base;
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE };

    filters->graph = avfilter_graph_alloc();
    if (!filters->graph) {
        fprintf(stderr, "Failed to allocate filter graph\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }

    // buffer video source: the decoded frames from the decoder will be inserted here.
    snprintf(args, sizeof(args),
            "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
            vid->codec_ctx->width, vid->codec_ctx->height,
            vid->codec_ctx->pix_fmt,
            time_base.num, time_base.den,
            vid->codec_ctx->sample_aspect_ratio.num, vid->codec_ctx->sample_aspect_ratio.den);
    printf("filter graph: %s\n", args);
    
    ret = avfilter_graph_create_filter(&filters->buffersrc_ctx,
        buffersrc, "in",
        args,
        NULL,
        filters->graph);
    if (ret < 0) {
        fprintf(stderr, "Failed to create filter graph\n");
        goto end;
    }
    

    /* buffer video sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&filters->buffersink_ctx,
        buffersink, "out", NULL, NULL, filters->graph);
    if (ret < 0) {
        fprintf(stderr, "Failed to create buffer sink\n");
        goto end;
    }

    ret = av_opt_set_int_list(filters->buffersink_ctx,
        "pix_fmts", pix_fmts, AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        fprintf(stderr, "Failed to set pix fmt\n");
        goto end;
    }

    /*
     * Set the endpoints for the filter graph. The filter_graph will
     * be linked to the graph described by filters_descr.
     */

    /*
     * The buffer source output must be connected to the input pad of
     * the first filter described by filters_descr; since the first
     * filter input label is not specified, it is set to "in" by
     * default.
     */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = filters->buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    /*
     * The buffer sink input must be connected to the output pad of
     * the last filter described by filters_descr; since the last
     * filter output label is not specified, it is set to "out" by
     * default.
     */
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = filters->buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    if ((ret = avfilter_graph_parse_ptr(
        filters->graph, graph_desc, &inputs, &outputs, NULL)) < 0) {
        fprintf(stderr, "Failed to parse filter graph: %s\n", graph_desc);
        goto end;
    }

    if ((ret = avfilter_graph_config(filters->graph, NULL)) < 0) {
        fprintf(stderr, "Failed to configure filter graph: %s\n", graph_desc);
        goto end;
    }

end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;
}

int free_filter_graph(struct FilterGraph *filters) {
    avfilter_graph_free(&filters->graph);
}