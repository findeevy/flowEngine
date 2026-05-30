#!/bin/bash
set -e
 
if [ -d "imgui/.git" ]; then
    git -C imgui pull --ff-only
else
    git clone --branch docking --depth 1 \
        https://github.com/ocornut/imgui.git \
        imgui
fi
 
sudo pacman -S glad
 
mkdir -p glad KHR ../src/glad
 
glad --api gl:core=3.3 --out-path .. c
 
mv ../src/gl.c ../src/glad/glad.c