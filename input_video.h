#ifndef VABSLIDER_INPUT_VIDEO
#define VABSLIDER_INPUT_VIDEO

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavfilter/avfilter.h>

struct InputVideo {
    char const* path;
    int video_stream_idx;
    AVFormatContext *format_ctx;
    AVCodec *codec;
    AVCodecContext *codec_ctx;
};

int new_input_video(struct InputVideo *vid, char const *path);
int free_input_video(struct InputVideo *vid);

#endif