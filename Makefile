
.PHONY = clean

FFMPEG_INCLUDE = /usr/include/ffmpeg
SDL2_INCLUDE = /usr/include/SDL2

CFLAGS = -I$(FFMPEG_INCLUDE) -I$(SDL2_INCLUDE) -D_REENTRANT
LDFLAGS = -lavcodec -lavformat -lswscale -lSDL2 -Wl,-rpath=_ORIGIN

vabslider: main.c input_video.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
	chrpath -r '$$ORIGIN' $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf vabslider input_video.o
