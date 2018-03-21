// Copyright Steinwurf APS 2011-2013.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <string>
#include "node.hpp"

/// Simple base-class for a relay
class relay : public node
{
public:

    relay(const std::string& id) :
        node(id)
    { }

    virtual void set_recode_on() = 0;
    virtual void set_recode_off() = 0;
    virtual bool is_recode_on() const = 0;
    virtual void set_transmit_on_receive(bool) = 0;
};

