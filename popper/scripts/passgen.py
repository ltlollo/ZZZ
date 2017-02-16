#!/usr/bin/env python

import uuid


pre, post = ["#ifndef", "#define"], ["#endif"]
spassw = '"'.join(["char pass[]{", uuid.uuid4().hex, "};"])
code = ["namespace {",
        spassw,
        "}",
        "constexpr auto passw() {",
        "return pass;",
        "}"
        ]


def main(fname):
    fsig = fname.replace('.', '_').upper()
    with open(fname, 'w') as fpass:
        for s in pre:
            fpass.write(' '.join([s, fsig]) + '\n')
        for s in code:
            fpass.write(s+"\n")
        for s in post:
            fpass.write(" // ".join([s, fsig]) + '\n')


if __name__ == "__main__":
    main("pass.h")
