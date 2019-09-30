#pragma once

#include "token_receiver.hpp"

#include <stream/input_stream.hpp>

namespace bia {
namespace tokenizer {

class lexer
{
public:
    virtual ~lexer();
    virtual void lex(stream::input_stream& input, token_receiver& receiver) = 0;
};

}
}