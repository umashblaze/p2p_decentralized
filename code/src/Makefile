CC = g++
CPPFLAGS = -I 
INCLUDE = ../lib/RCF-2.2.0.0/include/
SRCLIB = ../lib/RCF-2.2.0.0/src/RCF/RCF.cpp
DEPS_CI = ../include/index_server.h
DEPS_PR = ../include/peer.h
LIB_OPTIONS = -lboost_system -lboost_filesystem -lpthread -ldl /usr/local/lib/libconfig++.a

all: iServer1 iServer2 iServer3 iServer4 iServer5 iServer6 iServer7 iServer8 peer1 peer2 peer3 peer4 peer5 peer6 peer7 peer8

iServer1: index_server1_main.cpp index_server.cpp $(DEPS_CI) 
	$(CC) index_server1_main.cpp index_server.cpp $(SRCLIB) $(CPPFLAGS) $(INCLUDE) $(LIB_OPTIONS) -o iServer1

iServer2: index_server2_main.cpp index_server.cpp $(DEPS_CI) 
	$(CC) index_server2_main.cpp index_server.cpp $(SRCLIB) $(CPPFLAGS) $(INCLUDE) $(LIB_OPTIONS) -o iServer2

iServer3: index_server3_main.cpp index_server.cpp $(DEPS_CI) 
	$(CC) index_server3_main.cpp index_server.cpp $(SRCLIB) $(CPPFLAGS) $(INCLUDE) $(LIB_OPTIONS) -o iServer3

iServer4: index_server4_main.cpp index_server.cpp $(DEPS_CI) 
	$(CC) index_server4_main.cpp index_server.cpp $(SRCLIB) $(CPPFLAGS) $(INCLUDE) $(LIB_OPTIONS) -o iServer4

iServer5: index_server5_main.cpp index_server.cpp $(DEPS_CI) 
	$(CC) index_server5_main.cpp index_server.cpp $(SRCLIB) $(CPPFLAGS) $(INCLUDE) $(LIB_OPTIONS) -o iServer5

iServer6: index_server6_main.cpp index_server.cpp $(DEPS_CI) 
	$(CC) index_server6_main.cpp index_server.cpp $(SRCLIB) $(CPPFLAGS) $(INCLUDE) $(LIB_OPTIONS) -o iServer6

iServer7: index_server7_main.cpp index_server.cpp $(DEPS_CI) 
	$(CC) index_server7_main.cpp index_server.cpp $(SRCLIB) $(CPPFLAGS) $(INCLUDE) $(LIB_OPTIONS) -o iServer7

iServer8: index_server8_main.cpp index_server.cpp $(DEPS_CI) 
	$(CC) index_server8_main.cpp index_server.cpp $(SRCLIB) $(CPPFLAGS) $(INCLUDE) $(LIB_OPTIONS) -o iServer8


peer1: peer1_main.cpp peer.cpp $(DEPS_PR)    
	$(CC) peer1_main.cpp peer.cpp $(SRCLIB) $(CPPFLAGS) $(INCLUDE) $(LIB_OPTIONS) -o peer1

peer2: peer2_main.cpp peer.cpp $(DEPS_PR)
	$(CC) peer2_main.cpp peer.cpp $(SRCLIB) $(CPPFLAGS) $(INCLUDE) $(LIB_OPTIONS) -o peer2 

peer3: peer3_main.cpp peer.cpp $(DEPS_PR)
	$(CC) peer3_main.cpp peer.cpp $(SRCLIB) $(CPPFLAGS) $(INCLUDE) $(LIB_OPTIONS) -o peer3

peer4: peer4_main.cpp peer.cpp $(DEPS_PR)    
	$(CC) peer4_main.cpp peer.cpp $(SRCLIB) $(CPPFLAGS) $(INCLUDE) $(LIB_OPTIONS) -o peer4

peer5: peer5_main.cpp peer.cpp $(DEPS_PR)
	$(CC) peer5_main.cpp peer.cpp $(SRCLIB) $(CPPFLAGS) $(INCLUDE) $(LIB_OPTIONS) -o peer5 

peer6: peer6_main.cpp peer.cpp $(DEPS_PR)
	$(CC) peer6_main.cpp peer.cpp $(SRCLIB) $(CPPFLAGS) $(INCLUDE) $(LIB_OPTIONS) -o peer6

peer7: peer7_main.cpp peer.cpp $(DEPS_PR)    
	$(CC) peer7_main.cpp peer.cpp $(SRCLIB) $(CPPFLAGS) $(INCLUDE) $(LIB_OPTIONS) -o peer7

peer8: peer8_main.cpp peer.cpp $(DEPS_PR)
	$(CC) peer8_main.cpp peer.cpp $(SRCLIB) $(CPPFLAGS) $(INCLUDE) $(LIB_OPTIONS) -o peer8 


clean:
	rm -rf *.o