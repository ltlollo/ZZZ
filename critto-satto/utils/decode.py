#!/usr/bin/env python

import base64
import sys

if len(sys.argv) > 1:
    in_ = open(sys.argv[1])
else:
    in_ = sys.stdin

if len(sys.argv) > 2:
    out = open(sys.argv[2], 'w')
else:
    out = sys.stdout

base64.encode(in_, out)
