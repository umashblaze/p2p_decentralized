#include "../include/index_server.h"


//INDEX_SERVER's METHOD-1: REGISTER THE PEER AND HASH THE PEER IDs
bool IndexServer::register_peer(int peer_id, vector<string>& files)
{   
  //Lock for unique access
  RCF::Lock lock(mapMutex);
  
  cout << "I_IndexServer service: Connected to " << peer_id  << endl;
  
  if(!files.empty()){
      
    for(vector<string>::const_iterator c_itr = files.begin(); c_itr != files.end(); ++c_itr){
      //Avoid duplicates 
      if(find(registry[*c_itr].begin(), registry[*c_itr].end(), peer_id) != registry[*c_itr].end()){
          continue;
      }
      else{
          registry[*c_itr].push_back(peer_id);
      }
    }
    cout << peer_id << "'s files mapped successfully on " << my_config.name << "!" << endl;
   
    /* 
    cout << "The mapped files are: " << endl;
    for(vector<string>::const_iterator c_itr = files.begin(); c_itr != files.end(); ++c_itr){
        cout << *c_itr << endl;
    }
    */
    
    return SUCCESS;
  }
  else{
      cout << "No files exist to register!" << endl;
      return FAIL;
  }
}

//----------------------------------------------------------------------------

//INDEX_SERVER's METHOD-2: SEARCH THE FILE AND RETURN THE PEER LIST
vector<int> IndexServer::lookup_file(const string& file_name)
{   
  //Lock for unique access
  RCF::Lock lock(mapMutex);
  
  return registry[file_name];
}

//----------------------------------------------------------------------------

//INDEX_SERVER's METHOD-3: INDEX SERVER's CONSTRUCTOR
IndexServer::IndexServer(float serv_lat, float serv_long)
{   
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
  float ret_lat = 0, ret_long = 0;
  int ret_endpoint = 0;

  my_config.endpoint = 0;

  //Lookup the pre-defined server config for the given server co-ordinates
  //Initialize the server if a match is found
  for(int i = 0; i < group_info[0].getLength(); ++i){

    ret_lat = group_info[0][i]["geo_lat"];
    ret_long = group_info[0][i]["geo_long"];

    if(serv_lat == ret_lat && serv_long == ret_long){  
      cfg.lookupValue(group_info[0][i]["endpoint"].getPath(), my_config.endpoint);
      cfg.lookupValue(group_info[0][i]["name"].getPath(), my_config.name);
      cfg.lookupValue(group_info[0][i]["location"].getPath(), my_config.location);
      cfg.lookupValue(group_info[0][i]["geo_lat"].getPath(), my_config.geo_lat);
      cfg.lookupValue(group_info[0][i]["geo_long"].getPath(), my_config.geo_long);

      break;    
    }
  }

  //Fail if the server co-ordinates don't match
  if(0 == my_config.endpoint){
    cerr << "A server can't be initialized at the given co-ordinates: lat(" << \
    serv_lat << "), long(" << serv_long << ')' << "!\nExiting..." << endl;
    exit(1);
  }

  //Update server status to the config
  group_info[0][my_config.name]["status"] = 1;  
  cfg.writeFile(cfg_file.c_str());
}

//----------------------------------------------------------------------

//INDEX_SERVER's METHOD-5: INDEX SERVER'S DESTRUCTOR
IndexServer::~IndexServer()
{
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
  }
  catch(const libconfig::ParseException &pex)
  {
    cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
              << " - " << pex.getError() << endl;
  }

  const libconfig::Setting &group_info = cfg.getRoot();
  group_info[0][my_config.name]["status"] = 0;

  //Update server status to the config
  cfg.writeFile(cfg_file.c_str());
}

//-------------------------------EOF------------------------------------------