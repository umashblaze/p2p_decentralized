#include "../include/peer.h"

//Using POSIX APIs to get the file contents
#include <sys/types.h>
#include <dirent.h>

//Using C libraries for error messages
#include <cstring> 
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <ctime>

using std::strerror;

//----------------------------------------------------------------------------

//PEER's METHOD-1: ATTACH THE PEER WITH THE NEARBY SERVER
bool Peer::find_my_server(float peer_lat, float peer_long)
{
  using namespace libconfig;
  Config cfg;

  string cfg_file = "../include/serv_commons.conf";

  //Read the file. If there is an error, report it and exit.
  try
  {
    cfg.readFile(cfg_file.c_str());
  }
  catch(const FileIOException &fioex)
  {
    cerr << "I/O error while reading file." << endl;
    return FAIL;
  }
  catch(const ParseException &pex)
  {
    cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
              << " - " << pex.getError() << endl;
    return FAIL;
  }

  const Setting &group_info = cfg.getRoot();
  float serv_lat = 0, serv_long = 0;
  float dist = 0, min_dist = 360;
  int serv_status = 0;
  my_server = 0;

  //Lookup the pre-defined server config and pick the closest server
  for(int i = 0; i < group_info[0].getLength(); ++i){

    cfg.lookupValue(group_info[0][i]["geo_lat"].getPath(), serv_lat);
    cfg.lookupValue(group_info[0][i]["geo_long"].getPath(), serv_long);

    //Calculate the distance between the peer and server locations
    dist = sqrt(pow((serv_lat - peer_lat), 2) + pow((serv_long - peer_long), 2));

    //Find the nearby server
    if(dist < min_dist){
      min_dist = dist;
      cfg.lookupValue(group_info[0][i]["endpoint"].getPath(), my_server);
      cfg.lookupValue(group_info[0][i]["status"].getPath(), serv_status);
    }
  }

  //Fail if the nearby server isn't active
  if(0 == serv_status){
    cerr << "Nearby server isn't active!" << endl;
    return FAIL;
  }

  //Fail if the nearby server can't be found
  if(0 == my_server){
    cerr << "No nearby server for the client with lat(" << \
    peer_lat << "), long(" << peer_long << ')' << endl;
    return FAIL;
  }

  cout << "Peer " << my_id << " has been connected to server " << my_server << endl;

  return SUCCESS;
}

//----------------------------------------------------------------------------

//PEER's METHOD-2: SEARCH THE FILE AND RETURN THE PEER LIST
vector<int> Peer::find_other_servers(void)
{   
  //Lock for unique access
  RCF::Lock lock(mapMutex);
  
  libconfig::Config cfg;

  string cfg_file = "../include/serv_commons.conf";

  // Read the file. If there is an error, report it and exit.
  try
  {
    cfg.readFile(cfg_file.c_str());
  }
  catch(const libconfig::FileIOException &fioex)
  {
    cerr << "I/O error while reading file." << endl;
    exit(1);
  }
  catch(const libconfig::ParseException &pex)
  {
    cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
              << " - " << pex.getError() << endl;
    exit(1);
  }

  const libconfig::Setting &group_info = cfg.getRoot();
  vector<int> active_servers;

  int ret_status = 0;
  int ret_endpoint = 0;

  //Return active servers for lookup by the client
  for(int i = 0; i < group_info[0].getLength(); ++i){

    ret_status = group_info[0][i]["status"];
    ret_endpoint = group_info[0][i]["endpoint"];

    if(1 == ret_status){  
      active_servers.push_back(ret_endpoint);
    }
  }
 
  return active_servers;
}

//----------------------------------------------------------------------------

//PEER's METHOD-3: IDENTIFY AND ENLIST THE FILES
bool Peer::enlist_my_files(void)
{
  string dir = get_my_dir_path();
  DIR *dp;
  struct dirent *dirp;

  if((dp  = opendir(dir.c_str())) == NULL) {
      cout << "Error in opening the directory!\n" << strerror(errno) << endl;
      return FAIL;
  }

  while ((dirp = readdir(dp)) != NULL) {
      if(dirp->d_name[0] != '.' && dirp->d_name[strlen(dirp->d_name)-1] != '~'){
        my_files.push_back(string(dirp->d_name));
      }
  }
  closedir(dp);
  
  //Update the file count 
  file_count = my_files.size();
  
  return SUCCESS;
}

//----------------------------------------------------------------------------

//PEER's METHOD-4: SHOW THE FILES
void Peer::display_my_files() const
{
  cout << my_id << "'s Files are:" << endl;
  for(vector<string>::const_iterator c_itr = my_files.begin(); c_itr != my_files.end(); ++c_itr){
      cout << *c_itr << endl;
  }
}

//----------------------------------------------------------------------------

//PEER's METHOD-5: REGISTER THE FILES
void Peer::register_my_files(const RCF::TcpEndpoint& i_server_endpoint)
{
  RcfClient<I_IndexServer> tcp_client(i_server_endpoint);
  bool result = FAIL;

  //Set max message length for the client (10 MB)
  tcp_client.getClientStub().getTransport().setMaxIncomingMessageLength(10*1024*1024);
  
  //#### TIME MEASURE BEGINS ####
  clock_t tic = clock();

  //Accessing RCF interface - for registering the peer files
  try{
    result = tcp_client.register_peer(my_id, my_files);
  }
  catch(const RCF::Exception & e){
     std::cout << "Error: " << e.getErrorString() << std::endl;
  }

  //#### TIME MEASURE ENDS ####
  clock_t toc = clock();
  
  //Measure the time diff and aggregate
  total_register_time += ((double)(toc - tic) / CLOCKS_PER_SEC);

  //Increment Register operation counter
  registerOp_count++;
  
  if(result){
      //cout << "Peer " << get_my_id() << "'s files are registered." << endl;
  }
  else{
      cerr << "Cannot register Peer "<< get_my_id() << "'s files..\nPeer Exiting!" << endl;
      exit(EXIT_FAILURE);
  }
}

//----------------------------------------------------------------------------

//PEER's METHOD-6: CHECK IF THE FILE IS ALREADY PRESENT
bool Peer::check_if_present(const string& file_name)
{
  return find(my_files.begin(), my_files.end(), file_name) != my_files.end();
}

//----------------------------------------------------------------------------

//PEER-SERVER's METHOD: TRANSFER THE FILE TO THE REQUESTED CLIENT
void Server::obtain_file(const string& file_name, RCF::FileDownload filedownload)
{
  //Lock for unique access
  RCF::Lock lock(mapMutex);
  
  filedownload = RCF::FileDownload(string(get_my_dir_path() + file_name));
}

//----------------------------------------------------------------------------

//PEER-CLIENT's METHOD-1: SEARCH A FILE AND FIND THE SEED SERVERS
bool Client::search_file(const string& file_name, const RCF::TcpEndpoint& i_server_endpoint)
{
  
  RcfClient<I_IndexServer> tcp_client(i_server_endpoint);

  //Set max message length for the client (10 MB)
  tcp_client.getClientStub().getTransport().setMaxIncomingMessageLength(10*1024*1024);

  //#### TIME MEASURE BEGINS ####
  clock_t tic = clock();
  
  //Accessing RCF interface - for retrieving the seed servers
  try{
     seeds = tcp_client.lookup_file(file_name);
  }
  catch(const RCF::Exception & e){
     cerr << "Error: " << e.getErrorString() << std::endl;
  }
  
  //#### TIME MEASURE ENDS ####
  clock_t toc = clock();
  
  //Measure the time diff and aggregate
  total_search_time += ((double)(toc - tic) / CLOCKS_PER_SEC);

  //Increment Search operation counter
  searchOp_count++;
 
  if(seeds.empty()){
      return FAIL;
  }/*
  else{
      cout << "The seed server(s) containing " << file_name << " is/are:" << endl;
      for(vector<int>::const_iterator c_itr = seeds.begin(); c_itr != seeds.end(); ++c_itr){
          cout << *c_itr << endl;
      }
  }*/
  return SUCCESS;
}

//----------------------------------------------------------------------------

//PEER-CLIENT's METHOD-2: CLIENT DOWNLOADS THE FILE AFTER FINDING THE PEER
bool Client::download_file(const string& file_name)
{
  RCF::FileDownload fileDownload;
  fileDownload.setDownloadToPath(Peer::get_my_dir_path());
  
  //Loop through all the seeds (source servers) and download the file from atleast one
  if(!seeds.empty()){
    for(vector<int>::const_iterator c_itr = seeds.begin(); c_itr != seeds.end(); ++c_itr)
    {
      //Pick one of the endpoints
      RCF::TcpEndpoint serv_endpoint = *c_itr;
      RcfClient<I_PeerServer> tcp_client(serv_endpoint);

      //Set max message length for the client (10 MB)
      tcp_client.getClientStub().getTransport().setMaxIncomingMessageLength(10*1024*1024);
      
      //#### TIME MEASURE BEGINS ####
      clock_t tic = clock();
      
      int result = FAIL;
      //Accessing RCF interface - for downloading the desired file from the chosen server
      try{
        tcp_client.obtain_file(file_name, fileDownload);
        result = SUCCESS;
      }
      catch(const RCF::Exception & e){
        cerr << "Error: " << e.getErrorString() << std::endl;
        return FAIL;
      }

      //#### TIME MEASURE ENDS ####
      clock_t toc = clock();
      
      //Measure the time diff and aggregate
      total_obtain_time += ((double)(toc - tic) / CLOCKS_PER_SEC);

      //Increment Obtain operation counter
      obtainOp_count++;
      
      if(result){
        //cout << "$$ Client " << get_my_id() << \
                  " has got " << file_name << " from Peer " << *c_itr << " $$" << endl;
        break;
      }
      else{//The requested server didn't respond!
          //cout << "Server: " << *c_itr << \
                  "didn't serve the request! Let's try the next one..." << endl;
          continue;
      }
    }
  }
  else{//No seeds for the requested file
    //cerr << "There is no server having the file!" << endl;
    return FAIL;
  }

  //Track the number of files downloaded by the client
  num_files_downloaded++;

  return SUCCESS;
}

//-----------------------------EOF--------------------------------