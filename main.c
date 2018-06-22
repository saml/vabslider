#include <stdio.h>

#include "input_video.h"




// int init_filters(struct InputVideo *vid, char const* filters_descr)
// {
//     char args[512];
//     int ret;
//     const AVFilter *buffersrc  = avfilter_get_by_name("buffer");
//     const AVFilter *buffersink = avfilter_get_by_name("buffersink");
//     AVFilterInOut *outputs = avfilter_inout_alloc();
//     AVFilterInOut *inputs  = avfilter_inout_alloc();
//     AVRational time_base = vid->format_ctx->streams[vid->video_stream_idx]->time_base;
//     enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE };

//     filter_graph = avfilter_graph_alloc();
//     if (!outputs || !inputs || !filter_graph) {
//         ret = AVERROR(ENOMEM);
//         goto end;
//     }

//     /* buffer video source: the decoded frames from the decoder will be inserted here. */
//     snprintf(args, sizeof(args),
//             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
//             dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
//             time_base.num, time_base.den,
//             dec_ctx->sample_aspect_ratio.num, dec_ctx->sample_aspect_ratio.den);

//     ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
//                                        args, NULL, filter_graph);
//     if (ret < 0) {
//         av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
//         goto end;
//     }

//     /* buffer video sink: to terminate the filter chain. */
//     ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
//                                        NULL, NULL, filter_graph);
//     if (ret < 0) {
//         av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
//         goto end;
//     }

//     ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
//                               AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
//     if (ret < 0) {
//         av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
//         goto end;
//     }

//     /*
//      * Set the endpoints for the filter graph. The filter_graph will
//      * be linked to the graph described by filters_descr.
//      */

//     /*
//      * The buffer source output must be connected to the input pad of
//      * the first filter described by filters_descr; since the first
//      * filter input label is not specified, it is set to "in" by
//      * default.
//      */
//     outputs->name       = av_strdup("in");
//     outputs->filter_ctx = buffersrc_ctx;
//     outputs->pad_idx    = 0;
//     outputs->next       = NULL;

//     /*
//      * The buffer sink input must be connected to the output pad of
//      * the last filter described by filters_descr; since the last
//      * filter output label is not specified, it is set to "out" by
//      * default.
//      */
//     inputs->name       = av_strdup("out");
//     inputs->filter_ctx = buffersink_ctx;
//     inputs->pad_idx    = 0;
//     inputs->next       = NULL;

//     if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
//                                     &inputs, &outputs, NULL)) < 0)
//         goto end;

//     if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
//         goto end;

// end:
//     avfilter_inout_free(&inputs);
//     avfilter_inout_free(&outputs);

//     return ret;
// }


int main(int argc, char const *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s vid1 vid2\n", argv[0]);
        exit(1);
    }
    char const* vid1_path = argv[1];
    char const* vid2_path = argv[2];

    struct InputVideo vid1;
    struct InputVideo vid2;
    int ret;
    
    if ((ret = new_input_video(&vid1, vid1_path)) != 0) {
        fprintf(stderr, "Failed to create vid1: %s\n", vid1_path);
        goto CLEANUP;
    }

    if ((ret = new_input_video(&vid2, vid2_path)) != 0) {
        fprintf(stderr, "Failed to create vid2: %s\n", vid2_path);
        goto CLEANUP;
    }
    

CLEANUP:
    free_input_video(&vid1);
    free_input_video(&vid2);
    return ret;
}
