#include <iostream>
#include <abstractprotocolmodel.h>
#include "requesthandler.h"
using namespace std;

int main()
{
    cout << "Hello World!" << endl;
    bcf::RequestHandlerBuilder builder;
    auto abandonCallBack = [](const std::string & adandonData) {
        cout << "adandonData:" << adandonData << endl;
    };
    builder.WithTimeOut(100'000).withAbandonCallback(abandonCallBack).build();
    return 0;
}
