#include"../include/peer.h"

int main()
{
    RCF::RcfInitDeinit rcfInit;
    
    //PEER-3 Details:
    int peer_port = 50023;
    string peer_dir_path = "../data/50023/";

    //Create a peer object
    Peer p1(peer_port, peer_dir_path);

    //Find the nearby server
    if(!p1.find_my_server(40.500, 22.600)){
        cerr << "Peer3 can't find any nearby server to register with! Exiting..." << endl;
        exit(1);
    }
   
    //Explore peer's directory and record the files
    p1.enlist_my_files();
    
    //Display peer's directory contents
    //p1.display_my_files();
    
    //Register
    int i_server_endpoint = p1.get_server_endpoint();
    p1.register_my_files(i_server_endpoint);
    
    //-----------SERVER-------------
    //Initialize Peer's Server
    Server ps(peer_port, peer_dir_path);
        
    RCF::TcpEndpoint server_endpoint = peer_port;
    RCF::RcfServer tcp_server(server_endpoint);
    
    //Bind the RCF-Interface to the server
    tcp_server.bind<I_PeerServer>(ps); 
        
    //Create server threads - dynamic
    RCF::ThreadPoolPtr tpPtr(new RCF::ThreadPool(1, 50) );
    tcp_server.setThreadPool(tpPtr);
    
    //Start the server
    tcp_server.start();
    cout << "Peer " << peer_port << "'s Server started and threads are set." << endl;
    
    //-----------CLIENT---------------
    //Initialize Peer's Client
    //Client pc = new Client(peer_port, peer_dir_path);
    Client pc(peer_port, peer_dir_path);
    cout << "Peer " << peer_port << "'s Client operation begins..." << endl;
    
    string in_fileName;
    int prev_file_count = 0;
    int loop_over = 1;
    int search_result = 0;
    vector<int> active_servers;
    
    for(;;){
         cout << "Enter the name of the file you'd like to download:" << endl;
         
         getline(std::cin, in_fileName);
         if(cin.fail())
         {
            cerr << "Error reading from STDIN! Exiting..." << std::endl;
            exit(EXIT_FAILURE);
         }
         
         //Checking peer's directory if the file exists
         if (p1.check_if_present(in_fileName)){
            cout << in_fileName << " already exists for client " << \
                    peer_port << "! Try another file.." << endl;
                    
         }
         else{
             //Search the file in the nearby server
             if(!pc.search_file(in_fileName, i_server_endpoint)){

                //If the file isn't present in the nearby server, try other active ones
                active_servers = p1.find_other_servers();

                if(active_servers.empty()){
                    break;
                }
                //Search in other active servers
                for(vector<int>::iterator itr = active_servers.begin(); itr != active_servers.end(); ++itr){
                    if(pc.search_file(in_fileName, *itr)){
                        search_result = 1;
                        break;
                    }
                }
                
             }
             //If the file's source is found, download the file
             if(search_result){
                //Download the file
                if(pc.download_file(in_fileName)){
                    prev_file_count = p1.get_file_count();
                    
                    //Record the file's presence
                    if(p1.enlist_my_files()){
                        if(p1.check_if_downloaded(prev_file_count)){
                            cout << "Download Success!" << endl;
                        }
                        else{
                            cerr << "It seems there's a problem with the download!" << endl;
                        }
                    }
                    else{
                        cerr << "Error: In opening" << peer_port << "'s directory!" << endl;
                        break;
                    }
                    
                    //Informing the file's presence
                    p1.register_my_files(i_server_endpoint);
                }
                else{//Download didn't happen!
                    cout << "The requested file isn't downloaded! Try again.." << endl;
                }
             }
             else{
                 cout << "No seed sources for the given file! Try another file.." << endl;
             }
         }
         
         cout << "#---------------------------------------------" << endl;
         cout << "Wanna try a new file? \
                 \n* Enter 1 to continue, Or else\
                 \n* Enter 0 to exit." << endl;
            
         cin >> loop_over;
         cin.ignore();
         
         if(!loop_over){
             break;
         }
    }
   
    //Exit
    cout << "Peer " << peer_port << " exits..!" << endl;
    
    return 0;
}   