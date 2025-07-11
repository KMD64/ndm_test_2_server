#ifndef GENLSERVER_H
#define GENLSERVER_H

#include <functional>
#include <linux/netlink.h>
#include <memory>
/**
 * @todo write docs
 */


namespace Mwl2
{

typedef std::function<std::string (const std::string &, uint32_t)> GenlHandler;

class GenlServer
{
private:
    int _sockFd;
    std::unique_ptr<char[]> _buffer;
    void _closeSocket();
public:
    
    GenlServer();
    ~GenlServer();

    int listen(uint32_t pid, const GenlHandler &handler);    
};
}
#endif // GENLSERVER_H
