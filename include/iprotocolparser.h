#pragma once

#include <functional>
#include <memory>
#include <abstractprotocolmodel.h>
namespace bcf
{
class IProtocolParser
{
public:

    enum ParserState {
        OK = 0,            //代表整包数据解析成功，解析器返回数据后内部清空数据buffer
        WaitingStick = 1,  //代表有粘包,解析器在等待新的数据输送进来,解析器内部有上一包数据等着被“粘”到一起
        Error = 2,         //代表解析出错，解析器内部清空数据buffer
        Abandon = 3        //当前解析器无法解析的数据,当作垃圾一样丢出去，但解析器内部“可能”还会残留数据buffer（取决于具体的数据包），以等待后面的包能否粘成正常包
    };
    virtual ~IProtocolParser() = default;
    virtual bcf::ProtocolType getType() const = 0;
    //使用者自己实现parse函数，通过回调返回parser好的数据和parser状态
    virtual void parse(const std::string& data,
                       std::function<void(ParserState, std::shared_ptr<bcf::AbstractProtocolModel> model)>) = 0;
};
}
