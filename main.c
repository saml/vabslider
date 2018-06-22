#include <stdio.h>

#include <SDL.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>

#include "input_video.h"
#include "filter_graph.h"

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
    
    if ((ret = init_input_video(&vid1, vid1_path)) != 0) {
        fprintf(stderr, "Failed to create vid1: %s\n", vid1_path);
        goto CLEANUP;
    }

    if ((ret = init_input_video(&vid2, vid2_path)) != 0) {
        fprintf(stderr, "Failed to create vid2: %s\n", vid2_path);
        goto CLEANUP;
    }

    fprintf(stdout, "vid1: %ix%i (%s)\n", vid1.codec_ctx->width, vid1.codec_ctx->height, vid1_path);
    fprintf(stdout, "vid2: %ix%i (%s)\n", vid2.codec_ctx->width, vid2.codec_ctx->height, vid2_path);
    struct FilterGraph filters;
    int width;
    int height;
    if (vid1.codec_ctx->width * vid1.codec_ctx->height < vid2.codec_ctx->width * vid2.codec_ctx->height) {
        width = vid1.codec_ctx->width;
        height = vid1.codec_ctx->height;
    } else {
        width = vid2.codec_ctx->width;
        height = vid2.codec_ctx->height;
    }
    if ((ret = init_filter_graph(&filters, &vid1, "[in]crop@xcrop=w=100:x=0:y=0[cropped]")) != 0) {
        fprintf(stderr, "Failed to initialize filter graph\n");
    }
    printf("filter graph dump:\n%s\n", avfilter_graph_dump(filters.graph, NULL));
    //"[0][1]scale2ref[v0][v1]; [v0]crop@crop=w=200[cropped]; [cropped][v1]overlay"

    if ((ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) != 0) {
        fprintf(stderr, "Failed to initialize SDL2: %s\n", SDL_GetError());
        goto CLEANUP;
    }

    SDL_Window *win = SDL_CreateWindow(argv[0],
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (win == NULL) {
        fprintf(stderr, "Failed to create SDL2 window: %s\n", SDL_GetError());
	    ret = 1;
        goto CLEANUP;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (win == NULL) {
        fprintf(stderr, "Failed to create SDL2 renderer: %s\n", SDL_GetError());
	    ret = 1;
        goto CLEANUP;
    }

    SDL_RendererInfo renderer_info;
    if ((ret = SDL_GetRendererInfo(ren, &renderer_info)) != 0) {
        fprintf(stderr, "Failed to get renderer info: %s\n", SDL_GetError());
        goto CLEANUP;
    } else {
        fprintf(stdout, "Initialized renderer: %s\n", renderer_info.name);
    }
    SDL_Texture *texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, width, height);


    AVPacket packet;
    AVFrame *frame = av_frame_alloc();
    AVFrame *filter_frame = av_frame_alloc();
    while (1) {
        if ((ret = av_read_frame(vid1.format_ctx, &packet)) < 0) {
            break;
        }
        if (packet.stream_index == vid1.video_stream_idx) {
            if ((ret = avcodec_send_packet(vid1.codec_ctx, &packet)) < 0) {
                fprintf(stderr, "Failed to send packet to decoder\n");
                goto CLEANUP;
            }

            while ((ret = avcodec_receive_frame(vid1.codec_ctx, frame)) >= 0) {
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                }

                frame->pts = frame->best_effort_timestamp;

                if ((ret = av_buffersrc_add_frame_flags(filters.buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF)) < 0) {
                    fprintf(stderr, "Failed to feed filter graph\n");
                    goto CLEANUP;
                }

                while ((ret = av_buffersink_get_frame(filters.buffersink_ctx, filter_frame)) >= 0) {
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                        break;
                    }
                    printf("frame dimension %ix%i\n", frame->width, frame->height);
                    SDL_UpdateYUVTexture(texture, NULL,
                        frame->data[0], frame->linesize[0],
                        frame->data[1], frame->linesize[1],
                        frame->data[2], frame->linesize[2]);
                    SDL_RenderClear(ren);
                    SDL_RenderCopy(ren, texture, NULL, NULL);
                    SDL_RenderPresent(ren);
                    av_frame_unref(filter_frame);
                }
                av_frame_unref(frame);
            }
        }
        av_packet_unref(&packet);
    }
    

CLEANUP:
    free_input_video(&vid1);
    free_input_video(&vid2);
    free_filter_graph(&filters);
    av_frame_free(&frame);
    av_frame_free(&filter_frame);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return ret;
}
