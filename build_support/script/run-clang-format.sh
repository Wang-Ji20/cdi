#!/bin/bash
find . -name '*.hh' -o -name '*.cc' | xargs clang-format-17 -i
