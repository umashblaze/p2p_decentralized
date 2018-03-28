#ifndef SERVER_H
#define SERVER_H

#include <RCF/RCF.hpp>
#include <SF/vector.hpp>
#include <libconfig.h++>

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include <map>

#define SUCCESS 1
#define FAIL 0
#define MAX_SERVERS 50

using std::cin;
using std::cout;
using std::endl;
using std::cerr;
using std::iterator;
using std::string;
using std::vector;
using std::map;
using std::make_pair;
using std::find;


//-------------------------------------------------------------------
//RCF Interface for Indexing Server and Peer communications
RCF_BEGIN(I_IndexServer, "I_IndexServer")
    RCF_METHOD_R2(bool, register_peer, int, vector<string>&)
    RCF_METHOD_R1(vector<int>, lookup_file, const string&)
RCF_END(I_IndexServer)
//------------------------------------------------------------------
//RCF Interface for Indexing Server and Server communications
/*
RCF_BEGIN(I_ServerPool, "I_ServerPool")
    RCF_METHOD_R0(map<string, vector<int> >, share_tables)
RCF_END(I_ServerPool)
*/
//------------------------------------------------------------------


//STRUCTURE: SERVER CONFIGURATION
struct ServConfig
{   
    int endpoint;
    string name;
    string location;
    float geo_lat;
    float geo_long;
};


//CLASS: CENTRAL INDEXING SERVER
class IndexServer
{
public:
    
  IndexServer(float, float);
  IndexServer(){};
  ~IndexServer();

  //RCF-Interface Methods:
  bool register_peer(int, vector<string>&);
  vector<int> lookup_file(const string&);
  string get_serv_name(void){return my_config.name;}
  int get_serv_endpoint(void){return my_config.endpoint;}
       
private:
  ServConfig my_config;
  map<string, vector<int> > registry;
  RCF::Mutex mapMutex;
};

#endif