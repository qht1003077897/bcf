﻿#include "private/protocolparsermanager.h"

using namespace bcf;

void ProtocolParserManager::addParser(const std::shared_ptr<IProtocolParser>& parser)
{
    if (parser) {
        m_parsers.emplace(std::make_pair(parser->getType(), std::move(parser)));
    }
}

std::shared_ptr<IProtocolParser> ProtocolParserManager::findParse(PackMode id)
{
    const auto itr = m_parsers.find(id);
    if (itr == m_parsers.end()) {
        return nullptr;
    }

    return itr->second;
}

void ProtocolParserManager::parseByID(bcf::PackMode id,
                                      const std::shared_ptr<bb::ByteBuffer>& byteBufferPtr,
                                      std::function<void(bcf::ParserState, const std::shared_ptr<AbstractProtocolModel>& model)>
                                      _callback)
{
    const auto itr = m_parsers.find(id);
    if (itr == m_parsers.end()) {
        return;
    }

    const auto& parser = itr->second;
    if (!parser->sniff(byteBufferPtr)) {
        return;
    }

    parser->parse(_callback);
}

void ProtocolParserManager::parseByAll(const std::shared_ptr<bb::ByteBuffer>& byteBufferPtr,
                                       std::function<void (bcf::ParserState, const std::shared_ptr<AbstractProtocolModel>&)>
                                       _callback)
{
    for (auto& p : m_parsers) {
        auto& parser = p.second;
        if (!parser->sniff(byteBufferPtr)) {
            continue;
        }

        parser->parse(_callback);
    }
}


