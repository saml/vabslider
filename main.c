#include <stdio.h>

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
    
    struct FilterGraph filters;
    //"[0][1]scale2ref[v0][v1]; [v0]crop@crop=w=200[cropped]; [cropped][v1]overlay"
    init_filter_graph(&filters, &vid1, "crop=w=200");

CLEANUP:
    free_input_video(&vid1);
    free_input_video(&vid2);
    free_filter_graph(&filters);
    return ret;
}
