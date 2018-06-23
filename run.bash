#!/bin/bash

~/.local/bin/mpv different_720.mp4 --video-unscaled --script vabslider.lua '--vf=@crop:lavfi="movie=720.mp4[larger]; [larger][vid1]scale2ref[scaled][main]; [main]crop=out_w=400:x=0:y=0:keep_aspect=1[cropped]; [scaled][cropped]overlay[vo]"'
