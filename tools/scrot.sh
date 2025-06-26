#!/bin/bash
scrot -s -f - | xclip -selection clipboard -t image/png
