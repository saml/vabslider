#include <stdio.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

struct InputVideo {
    char const* path;
    AVFormatContext *format_ctx;
    AVCodec *codec;
    AVCodecContext *codec_ctx;
};

int new_video(struct InputVideo *vid, char const *path) {
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
    int stream_idx = ret;

    vid->codec_ctx = avcodec_alloc_context3(vid->codec);
    if (!vid->codec_ctx) {
        ret = AVERROR(ENOMEM);
        fprintf(stderr, "Cannot allocate codec context: %s\n", path);
        return ret;
    }

    if ((ret = avcodec_parameters_to_context(vid->codec_ctx, vid->format_ctx->streams[stream_idx]->codecpar)) < 0) {
        fprintf(stderr, "Failed avcodec_parameters_to_context: %s\n", path);
        return ret;
    }

    if ((ret = avcodec_open2(vid->codec_ctx, vid->codec, NULL)) < 0) {
        fprintf(stderr, "Cannot open video decoder: %s\n", path);
        return ret;
    }

    return 0;
}

int free_video(struct InputVideo *vid) {
    avcodec_free_context(&vid->codec_ctx);
    avformat_close_input(&vid->format_ctx);
}

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
    
    if ((ret = new_video(&vid1, vid1_path)) != 0) {
        fprintf(stderr, "Failed to create vid1: %s\n", vid1_path);
        goto CLEANUP;
    }

    if ((ret = new_video(&vid2, vid2_path)) != 0) {
        fprintf(stderr, "Failed to create vid2: %s\n", vid2_path);
        goto CLEANUP;
    }
    

CLEANUP:
    free_video(&vid1);
    free_video(&vid2);
    return ret;
}
