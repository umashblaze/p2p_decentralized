#include "../include/index_server.h"

int main()
{
    RCF::RcfInitDeinit rcfInit;

    //Create an index server with it's GPS co-ordinates
    IndexServer i_server(-33.868, 151.209);
    
    //Create RCF server and initialize its endpoint
    RCF::RcfServer server(RCF::TcpEndpoint(i_server.get_serv_endpoint()));

    //Bind the RCF interface to the server object
    server.bind<I_IndexServer>(i_server);
    
    //Create server threads - dynamic
    RCF::ThreadPoolPtr tpPtr( new RCF::ThreadPool(1, 50) );
    server.setThreadPool(tpPtr);

    //Set max message length for the server (30 MB)
    server.getServerTransport().setMaxIncomingMessageLength(30*1024*1024);
    
    server.start();
    cout << i_server.get_serv_name() << " started..." << std::endl;
    
    cout << "Press Enter to exit..." << endl;
    cin.get();
    
    cout << i_server.get_serv_name() << " exited..." << std::endl;
    
    return 0;
}