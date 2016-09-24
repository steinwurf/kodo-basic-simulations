// Copyright Steinwurf APS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <string>
#include "node.hpp"

// Base class for a channel
class channel : public node
{
public:
    channel(const std::string& id) :
        node(id)
    { }
};



