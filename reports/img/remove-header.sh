#!/usr/bin/env sh
parallel convert -gravity South -crop +0-27 {} {.}.png ::: *.png