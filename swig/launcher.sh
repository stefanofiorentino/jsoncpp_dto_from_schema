#!/usr/bin/env bash
LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libjsoncpp.so \
    PYTHONPATH=$PWD:$PWD/python:$PYTHONPATH \
    python -c 'from ProductModule import product; p = product(); p.set_productId(42); p.set_productName("Product 42"); print p.toStyledString();'