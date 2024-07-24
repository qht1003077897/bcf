#pragma once

#include <memory>
#include <functional>
#include<list>
#include <abstractprotocolmodel.h>

namespace bcf
{
// 过滤器接口
class Filter
{
public:
    virtual bool filter(std::shared_ptr<bcf::AbstractProtocolModel> model) const = 0;
};



//class ProtocolFilter : public Filter
//{
//public:
//    bool filter(std::shared_ptr<bcf::AbstractProtocolModel> model) const override {
//        return person.age >= 18; // 只筛选年龄大于等于18岁的人

//    }
//};

class FilterChain
{
public:
    FilterChain() {};
    ~FilterChain();
    void addFilter(std::shared_ptr<bcf::Filter> filter)
    {
        this->filterList.emplace_back(filter);
    };

    void filter(std::shared_ptr<bcf::AbstractProtocolModel> model);

private:
    std::list<std::shared_ptr<bcf::Filter>> filterList;
};
}
