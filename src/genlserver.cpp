#include "genlserver.hpp"

#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>
#include <string.h>

#define MSG_BUFFER_SIZE 1024

namespace Mwl2
{
    
GenlServer::GenlServer()
:_sockFd{-1},_buffer{0}
{
    
}

GenlServer::~GenlServer()
{
    _closeSocket();
}

void GenlServer::_closeSocket()
{
    if(_sockFd>=0)
        close(_sockFd);
}

int GenlServer::listen(uint32_t pid, const GenlHandler &handler)
{
    _closeSocket();
    _sockFd = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_GENERIC);
    
    if(_sockFd<0)
    {
        perror("socket");
        return -1;
    }
    
    sockaddr_nl ownAddress =
    {
        .nl_family = AF_NETLINK,
        .nl_pad = 0,
        .nl_pid = pid,
        .nl_groups = 0
    };
    
    sockaddr_nl remoteAddress = 
    {
        .nl_family = AF_NETLINK,
        .nl_pad = 0,
        .nl_pid = 1,
        .nl_groups = 0
    };
    
    int res = 0;
    
    if((res = bind(_sockFd, reinterpret_cast<sockaddr *>(&ownAddress),sizeof(ownAddress))) < 0)
    {
        perror("bind");
        _closeSocket();
        return res;
    }

    //map nlmsghdr struct to buffer space
    std::unique_ptr<char[]> buf(new char[NLMSG_SPACE(MSG_BUFFER_SIZE)]());
    _buffer.swap(buf);
    
    nlmsghdr* pheader = reinterpret_cast<nlmsghdr*>(_buffer.get());
    pheader->nlmsg_pid = 1000;
    pheader->nlmsg_flags=0;
    
    
    //msg data 
    iovec iov = 
    {
        .iov_base=pheader,
        .iov_len=pheader->nlmsg_len
    };
    
    iov.iov_len = pheader->nlmsg_len = NLMSG_SPACE(MSG_BUFFER_SIZE);
    
    msghdr msgHdr = 
    {
        .msg_name = &remoteAddress,
        .msg_namelen = sizeof(remoteAddress),
        .msg_iov=&iov,
        .msg_iovlen = 1
    };
    
    printf("Ready to receive msg (%d)\n",pid);
    
    while(1)
    {
        //restore size values to buffer size
        iov.iov_len = pheader->nlmsg_len = NLMSG_SPACE(MSG_BUFFER_SIZE);
        
        auto rc = recvmsg(_sockFd,&msgHdr,0);
        if(rc < 0)
        {
            perror("recvFrom");
            _closeSocket();
            break;
        }

        
        printf("Received message from %d (%ld):%s\n",remoteAddress.nl_pid, rc
        ,(char*)NLMSG_DATA(pheader));
        
        std::string reqMsg(reinterpret_cast<char*>(NLMSG_DATA(pheader)));
        std::string respMsg(std::move(handler(reqMsg,remoteAddress.nl_pid)));
        

        strcpy(reinterpret_cast<char*>(NLMSG_DATA(pheader)),respMsg.data());
        iov.iov_len = pheader->nlmsg_len = NLMSG_HDRLEN + respMsg.size()+1;
        
        rc = sendmsg(_sockFd,&msgHdr,0);
        if(rc<0)
        {
            perror("sendto");
            _closeSocket();
            break;
        }
    }
    
    return 0;
}
}
