#!/usr/bin/env bash
set -ex

find src \( -name *.cpp -o -name *.h \) | xargs clang-format -i