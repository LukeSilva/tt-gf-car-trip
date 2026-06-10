# SPDX-License-Identifier: Apache-2.0
# Copyright (C) 2024, Tiny Tapeout LTD
# Authors: Uri Shaked, James Ross

TOP_MODULE:=$(shell awk -F'"' '/top_module:/ {print $$2}' ../info.yaml)
VERILOG_SOURCES = ../src/*.v

VFLAGS = -Wall -Wpedantic --default-language 1364-2005 --x-assign fast --x-initial fast --noassert --top-module $(TOP_MODULE)
CFLAGS = -flto -O3 -march=native -DTOP_MODULE=V$(TOP_MODULE) -Iobj_dir -I/usr/share/verilator/include -include V$(TOP_MODULE).h
LDFLAGS = -flto -lSDL2

all: obj_dir/V$(TOP_MODULE).h
	make -C obj_dir -f V$(TOP_MODULE).mk

obj_dir/V$(TOP_MODULE).h: $(VERILOG_SOURCES) main.cpp
	verilator $(VFLAGS) --cc $(VERILOG_SOURCES) --exe main.cpp -CFLAGS "$(CFLAGS)" -LDFLAGS "$(LDFLAGS)"

lint: $(VERILOG_SOURCES)
	verilator --lint-only $(VFLAGS) $(VERILOG_SOURCES)

sim: all
	obj_dir/V$(TOP_MODULE)

gif: all
	obj_dir/V$(TOP_MODULE) --gif 1024

clean:
	rm -rf obj_dir
	rm -f output.gif

distclean: clean

.PHONY: all lint sim gif clean distclean
