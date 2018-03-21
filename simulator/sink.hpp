// Copyright Steinwurf APS 2011-2013.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <string>
#include "node.hpp"

/// Base class for a sink
class sink : public node
{
public:

    sink(const std::string& id) :
        node(id)
    { }

    /// @return true when the sink has completed decoding
    virtual bool is_complete() = 0;
};

