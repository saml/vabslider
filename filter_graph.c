#include <stdio.h>

#include <libavfilter/avfilter.h>
#include <libavutil/opt.h>

#include "input_video.h"
#include "filter_graph.h"

int init_filter_graph(struct FilterGraph *filters, struct InputVideo *vid) {
    char graph_txt[1024];
    int ret;

    AVRational time_base = vid->format_ctx->streams[vid->video_stream_idx]->time_base;
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE };

    filters->graph = avfilter_graph_alloc();
    if (!filters->graph) {
        fprintf(stderr, "Failed to allocate filter graph\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }



    // crop@crop1=w=100:x=0:y=0
    snprintf(graph_txt, sizeof(graph_txt),
            "buffer@in1=video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d[in];"
            "[in]drawtext@text=text=hello:fontsize=120:fontcolor=white:x=(w-text_w)/2:y=(h-text_h)/2[cropped];"
            "[cropped]buffersink@out1",
            vid->codec_ctx->width, vid->codec_ctx->height,
            vid->codec_ctx->pix_fmt,
            time_base.num, time_base.den,
            vid->codec_ctx->sample_aspect_ratio.num, vid->codec_ctx->sample_aspect_ratio.den);
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

    filters->buffersrc_ctx = avfilter_graph_get_filter(filters->graph, "buffer@in1");
    if (!filters->buffersrc_ctx) {
        fprintf(stderr, "Cannot find filter: buffer@in1\n");
        ret = 1;
        goto end;
    }
    filters->buffersink_ctx = avfilter_graph_get_filter(filters->graph, "buffersink@out1");
    if (!filters->buffersink_ctx) {
        fprintf(stderr, "Cannot find filter: buffersink@out1\n");
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