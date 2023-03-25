#!/usr/bin/env sh
pandoc --defaults="config/release.yml" --defaults="config/$1.yml"
