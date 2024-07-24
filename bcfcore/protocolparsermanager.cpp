﻿#include "protocolparsermanager.h"
using namespace bcf;

void ProtocolParserManager::addParser(const std::shared_ptr<IProtocolParser>& parser)
{
    if (parser) {
        parsers.emplace(std::make_pair(parser->getType(), std::move(parser)));
    }
}

std::shared_ptr<IProtocolParser> ProtocolParserManager::findParse(ProtocolType id)
{
    const auto itr = parsers.find(id);
    if (itr == parsers.end()) {
        return nullptr;
    }

    return itr->second;
}


void ProtocolParserManager::parseByID(bcf::ProtocolType id, const std::string& data,
                                      std::function<void(IProtocolParser::ParserState, const std::shared_ptr<AbstractProtocolModel>& model)>
                                      _callback)
{
    const auto itr = parsers.find(id);
    if (itr != parsers.end()) {
        const auto& parser = itr->second;
        parser->parse(data, _callback);
    }
}

void ProtocolParserManager::parseByAll(const std::string& data,
                                       std::function<void (IProtocolParser::ParserState, const std::shared_ptr<AbstractProtocolModel>&)>
                                       _callback)
{
    IProtocolParser::ParserState state = IProtocolParser::ParserState::OK;
    std::shared_ptr<AbstractProtocolModel> model;
    for (auto& p : parsers) {
        auto& parser = p.second;
        parser->parse(data, [&](IProtocolParser::ParserState _state,
        std::shared_ptr<AbstractProtocolModel> _model) {
            state = _state;
            model = _model;
        });

        if (IProtocolParser::ParserState::Error != state) {
            break;
        }
    }

    _callback(state, model);
}


