#! /usr/bin/env python
# encoding: utf-8

APPNAME = 'kodo_basic_simulations'
VERSION = '1.0.0'

import waflib.extras.wurf_options


def options(opt):

    opt.load('wurf_common_tools')


def resolve(ctx):

    import waflib.extras.wurf_dependency_resolve as resolve

    ctx.load('wurf_common_tools')

    ctx.add_dependency(resolve.ResolveVersion(
        name='waf-tools',
        git_repository='github.com/steinwurf/waf-tools.git',
        major=3))

    ctx.add_dependency(resolve.ResolveVersion(
        name='boost',
        git_repository='github.com/steinwurf/boost.git',
        major=2))

    ctx.add_dependency(resolve.ResolveVersion(
        name='kodo-rlnc',
        git_repository='github.com/steinwurf/kodo-rlnc.git',
        major=5))

    ctx.add_dependency(resolve.ResolveVersion(
        name='tables',
        git_repository='github.com/steinwurf/tables.git',
        major=6))

    # Internal dependencies
    if ctx.is_toplevel():

        ctx.add_dependency(resolve.ResolveVersion(
            name='gauge',
            git_repository='github.com/steinwurf/gauge.git',
            major=10))


def configure(conf):

    conf.load("wurf_common_tools")


def build(bld):

    bld.load("wurf_common_tools")

    if bld.is_toplevel():

        bld.recurse('relay_simulations')
        bld.recurse('butterfly')
