#include <iostream>
#include <functional>
#include <map>
#include <json11.hpp>
#include "genlserver.hpp"


std::map<std::string,std::function<double(double,double)>> actionMap{
    {"add",std::plus<double>()},
    {"sub",std::minus<double>()},
    {"mul",std::multiplies<double>()}
};


struct actionMsg
{
    bool is_valid = false;
    std::string action;
    double f1;
    double f2;
};


actionMsg parseReq(const std::string &req)
{
    actionMsg msg;
    std::string err;
    auto reqJson = json11::Json::parse(req, err);
    
    if(reqJson.is_null())
    {
        printf("Request parse failed: %s\n", err.c_str());
        return msg;
    }
    if(!reqJson.is_object())
    {
        printf("Invalid json type\n");
        return msg;
    }
    msg.action = reqJson["action"].string_value();
    msg.f1 = reqJson["argument_1"].number_value();
    msg.f2 = reqJson["argument_2"].number_value();
    
    
    msg.is_valid=true;
    return msg;
}

std::string packResp(double result)
{
    json11::Json valJson(result);
    json11::Json respJson(json11::Json::object({{"result",valJson}}));
    
    std::string respStr;
    respJson.dump(respStr);
    return respStr;
}

std::string packErrorResp(const std::string &errStr)
{
    json11::Json valJson(errStr);
    json11::Json respJson(json11::Json::object({{"error",valJson}}));
    
    std::string respStr;
    respJson.dump(respStr);
    return respStr;
}

double process(const actionMsg &act, std::string &errStr)
{
    auto actIter = actionMap.find(act.action);
    if(actIter == actionMap.end())
    {
        errStr = "Unknown action: "+act.action;
        return .0;
    }
    return actIter->second(act.f1,act.f2);
}



int main(int argc, char **argv)
{
    uint32_t pid{2};
    if(argc>=2)
        pid = atol(argv[1]);
    
    
    Mwl2::GenlServer srv;
    return srv.listen(pid,[](const std::string &req, uint32_t pid)->std::string
    {
        auto msg =parseReq(req);
        std::string errStr;
        if(!msg.is_valid)
            return packErrorResp("Invalid message");
        double result = process(msg,errStr);
        if(!errStr.empty())
        {
            return packErrorResp(errStr);
        }
        return packResp(result);
    });

}
