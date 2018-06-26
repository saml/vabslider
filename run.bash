#!/bin/bash

#~/.local/bin/mpv different_720.mp4 --video-unscaled --script vabslider.lua '--vf=@crop:lavfi="movie=720.mp4[larger]; [larger][vid1]scale2ref[scaled][main]; [main]crop=out_w=400:x=0:y=0:keep_aspect=1[cropped]; [scaled][cropped]overlay[vo]"'


if (( $# < 2 ))
then
	echo "Usage: $0 vid1 vid2"
	exit 1
fi

#~/.local/bin/mpv "$1" --external-file "$2" --video-unscaled --script vabslider.lua
~/.local/bin/mpv "$1" --external-file "$2" --no-osc --script vabslider.lua
