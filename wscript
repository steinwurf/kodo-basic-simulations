#! /usr/bin/env python
# encoding: utf-8

APPNAME = 'kodo-basic-simulations'
VERSION = '1.0.0'


def build(bld):

    if bld.is_toplevel():

        bld.recurse('relay_simulations')
        bld.recurse('butterfly')
