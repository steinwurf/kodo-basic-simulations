License
=======

A valid Kodo license is required if you wish to use and install this library.

Please request a license by **filling out the license request** form_.

Kodo is available under a research- and education-friendly license,
see the details in the LICENSE.rst file.

.. _form: http://steinwurf.com/license/

About
=====

This repository contains some basic Network Coding simulations based on the
kodo library.

.. image:: http://buildbot.steinwurf.dk/svgstatus?project=kodo-basic-simulations
    :target: http://buildbot.steinwurf.dk/stats?projects=kodo-basic-simulations

If you have any questions or suggestions about this library, please contact
us at our developer mailing list (hosted at Google Groups):

* http://groups.google.com/group/steinwurf-dev

Build
=====

To configure the examples run::

  python waf configure

This will configure the project and download all the dependencies needed.

After configure run the following command to build::

  python waf build

Run
===

After building you may run e.g. the ``relay_simulations`` binary::

  ./build/linux/relay_simulations/relay_simulations





