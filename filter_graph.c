#include <stdio.h>

#include <libavfilter/avfilter.h>
#include <libavutil/opt.h>

#include "input_video.h"
#include "filter_graph.h"

int init_filter_graph(struct FilterGraph *filters, struct InputVideo *larger, struct InputVideo *smaller) {
    char graph_txt[1024];
    int ret;

    AVRational larger_time_base = time_base(larger);
    AVRational smaller_time_base = time_base(smaller);
    
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE };

    filters->graph = avfilter_graph_alloc();
    if (!filters->graph) {
        fprintf(stderr, "Failed to allocate filter graph\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }

    snprintf(graph_txt, sizeof(graph_txt),
            "buffer@in1=video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d[larger];"
            "buffer@in2=video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d[smaller];"
            "[larger][smaller]scale2ref[out1][out2];"
            "[out1]buffersink@out1;"
            "[out2]buffersink@out2",
            larger->codec_ctx->width, larger->codec_ctx->height,
            larger->codec_ctx->pix_fmt,
            larger_time_base.num, larger_time_base.den,
            larger->codec_ctx->sample_aspect_ratio.num, larger->codec_ctx->sample_aspect_ratio.den,
            smaller->codec_ctx->width, smaller->codec_ctx->height,
            smaller->codec_ctx->pix_fmt,
            smaller_time_base.num, smaller_time_base.den,
            smaller->codec_ctx->sample_aspect_ratio.num, smaller->codec_ctx->sample_aspect_ratio.den);
    printf("filter graph: %s\n", graph_txt);

    AVFilterInOut *inputs;
    AVFilterInOut *outputs;
    ret = avfilter_graph_parse2(filters->graph, graph_txt, &inputs, &outputs);
    if (ret < 0) {
        fprintf(stderr, "Failed to parse filter graph: %s\n", graph_txt);
        goto end;
    }

    ret = avfilter_graph_config(filters->graph, NULL);
    if (ret < 0) {
        fprintf(stderr, "Failed to configure graph: %s\n", graph_txt);
        goto end;
    }

    filters->vid1_in = avfilter_graph_get_filter(filters->graph, "buffer@in1");
    if (!filters->vid1_in) {
        fprintf(stderr, "Cannot find filter: buffer@in1\n");
        ret = 1;
        goto end;
    }
    filters->vid1_out = avfilter_graph_get_filter(filters->graph, "buffersink@out1");
    if (!filters->vid1_out) {
        fprintf(stderr, "Cannot find filter: buffersink@out1\n");
        ret = 1;
        goto end;
    }

    filters->vid2_in = avfilter_graph_get_filter(filters->graph, "buffer@in2");
    if (!filters->vid2_in) {
        fprintf(stderr, "Cannot find filter: buffer@in2\n");
        ret = 1;
        goto end;
    }
    filters->vid2_out = avfilter_graph_get_filter(filters->graph, "buffersink@out2");
    if (!filters->vid2_out) {
        fprintf(stderr, "Cannot find filter: buffersink@out2\n");
        ret = 1;
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