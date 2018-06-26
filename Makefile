
.PHONY = clean

FFMPEG_INCLUDE = /usr/include/ffmpeg
SDL2_INCLUDE = /usr/include/SDL2

CFLAGS = -I$(FFMPEG_INCLUDE) -I$(SDL2_INCLUDE) -D_REENTRANT
LDFLAGS = -lavcodec -lavformat -lswscale -lavutil -lavfilter -lSDL2 -Wl,-rpath=_ORIGIN

vabslider: main.c input_video.o filter_graph.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
	chrpath -r '$$ORIGIN' $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf vabslider input_video.o

a.out: simple-streamcb.c
	$(CC) -o $@ $^ $(CFLAGS) -I$(HOME)/.local/include -L$(HOME)/.local/lib -lmpv $(LDFLAGS)
