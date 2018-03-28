#ifndef PEER_H
#define PEER_H

#include <RCF/RCF.hpp>
#include <SF/vector.hpp>
#include <libconfig.h++>

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include <unistd.h>
#include <sys/wait.h>
#include <cmath>

#define SUCCESS 1
#define FAIL 0
#define SEMANTICS_ON

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::getline;


//------------------------------------------------------------------
//RCF Interface for Central Indexing Server and Peer communications
RCF_BEGIN(I_IndexServer, "I_IndexServer")
    RCF_METHOD_R2(bool, register_peer, int, vector<string>&)
    RCF_METHOD_R1(vector<int>, lookup_file, const string&)
RCF_END(I_IndexServer)
//------------------------------------------------------------------
//RCF Interface for Peer to Peer File transfer
RCF_BEGIN(I_PeerServer, "PeerServer")
    RCF_METHOD_V2(void, obtain_file, const string&, RCF::FileDownload)
RCF_END(I_PeerServer)
//------------------------------------------------------------------


//STRUCTURE: CO-ORDINATES
struct Coord
{
    float lat;
    float lon;
};


//CLASS: PEER
class Peer
{
  public:
    Peer()
    {
        registerOp_count = 0;
        total_register_time = 0;
    }
    ~Peer(){}
    
    Peer(int value, string path): my_id(value), my_dir(path), my_files(1), file_count(0){};

    bool find_my_server(float, float);
    vector<int> find_other_servers(void);
    int get_my_id(void){return my_id;}
    string get_my_dir_path(void){return my_dir;}
    bool enlist_my_files(void);
    vector<string> get_my_files(void){return my_files;}
    int get_file_count(void){return file_count;}
    int get_server_endpoint(void){return my_server;}
    bool check_if_downloaded(int prev_count){return file_count > prev_count;}
    bool check_if_present(const string&);
    void display_my_files() const;
    void register_my_files(const RCF::TcpEndpoint&);

    //Time Measure Methods
    double get_total_register_time(void){return total_register_time;}
    int get_total_register_count(void){return registerOp_count;}

  
  protected:
    int my_id; //Peer's port number
    Coord my_loc; //Peer's geographical position
    string my_dir; //Peer's directory
    vector<string> my_files; //Peer's files
    int file_count;
    int my_server; //Peer's nearby server

    //Time Measure Data members
    long int registerOp_count;
    double total_register_time;

    //For sync
    RCF::Mutex mapMutex;
};


//CLASS: PEER's SERVER
class Server : public Peer
{
  public:
    Server(){}
    ~Server(){}
    
    Server(int value, string path): Peer(value, path){};
    
    //An RCF-Interface method:
    void obtain_file(const string&, RCF::FileDownload);
};


//CLASS: PEER's CLIENT
class Client : public Peer
{
  public:
    Client(){
        num_files_downloaded = 0;
        searchOp_count = 0;
        obtainOp_count = 0;
        total_search_time = 0;
        total_obtain_time = 0;
    }

    ~Client(){}
    
    Client(int value, string path): Peer(value, path){}
    
    bool search_file(const string&, const RCF::TcpEndpoint&);
    bool download_file(const string& file_name);

    //Time-Measure Methods
    double get_total_search_time(void){return total_search_time;}
    double get_total_obtain_time(void){return total_obtain_time;}
    int get_total_search_count(void){return searchOp_count;}
    int get_total_obtain_count(void){return obtainOp_count;}
    int get_num_files_downloaded(void){return num_files_downloaded;}
    
  private:
    vector<int> seeds; //To store the server-ids that has the requested file
    int num_files_downloaded;

    //Time Measure Data members
    long int searchOp_count;
    long int obtainOp_count;
    double total_search_time;
    double total_obtain_time;  
};

#endif 