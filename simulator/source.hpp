// Copyright Steinwurf APS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <string>

#include "node.hpp"

class source : public node
{
public:

    source(const std::string& id) :
        node(id)
    {}

    virtual void systematic_off() = 0;
    virtual void systematic_on() = 0;

};

