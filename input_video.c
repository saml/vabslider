#include "input_video.h"


int new_input_video(struct InputVideo *vid, char const *path) {
    vid->format_ctx = avformat_alloc_context();
    if (!vid->format_ctx) {
        return AVERROR(ENOMEM);
    }

    int ret;
    if ((ret = avformat_open_input(&vid->format_ctx, path, NULL, NULL)) < 0) {
        fprintf(stderr, "File open failed: %s\n", path);
        return ret;
    }

    if ((ret = avformat_find_stream_info(vid->format_ctx, NULL)) < 0) {
        fprintf(stderr, "Cannot find stream: %s\n", path);
        return ret;
    }

    if ((ret = av_find_best_stream(vid->format_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &(vid->codec), 0)) < 0) {
        fprintf(stderr, "Cannot find video stream: %s\n", path);
        return ret;
    }
    vid->video_stream_idx = ret;

    vid->codec_ctx = avcodec_alloc_context3(vid->codec);
    if (!vid->codec_ctx) {
        ret = AVERROR(ENOMEM);
        fprintf(stderr, "Cannot allocate codec context: %s\n", path);
        return ret;
    }

    if ((ret = avcodec_parameters_to_context(
            vid->codec_ctx,
            vid->format_ctx->streams[vid->video_stream_idx]->codecpar)) < 0) {
        fprintf(stderr, "Failed avcodec_parameters_to_context: %s\n", path);
        return ret;
    }

    if ((ret = avcodec_open2(vid->codec_ctx, vid->codec, NULL)) < 0) {
        fprintf(stderr, "Cannot open video decoder: %s\n", path);
        return ret;
    }

    return 0;
}

int free_input_video(struct InputVideo *vid) {
    avcodec_free_context(&vid->codec_ctx);
    avformat_close_input(&vid->format_ctx);
}
