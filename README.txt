########### Peer to Peer System - Steps to follow ###############

1) Uncompress the provided p2p package submitted as part of the assignment. 

2) The p2p system code is written in c++ and it requires RCF and Boost libraries. 
	a) The RCF is an open source library that is already included in the zipped p2p package submitted. You can download separately by running the following commands:
		wget -i http://www.deltavsoft.com/downloads/RCF-2.2.0.0.tar.gz
		tar -xzvf -C lib
	b) Boost library is usually installed in ubuntu machines. If not in yours, it can be installed by using the following command:
		sudo apt-get install libboost-all-dev 
	c) Libconfig library to read and write to the config file:
		sudo apt-get install libconfig++-dev

3) Note that for RCF to handle file transfer operations, RCF_FEATURE_FILETRANSFER has to be enaled explicitly in the configuration. This was already done in the provided p2p package, so only for a fresh download of RCF library the following step is required.
	Comment out the lines (134 to 141) of /lib/RCF-2.2.0.0/include/RCF/Config.hpp in the following way:
		// File transfer feature
		//#ifndef RCF_FEATURE_FILETRANSFER
		//#ifdef RCF_USE_BOOST_FILESYSTEM
		#define RCF_FEATURE_FILETRANSFER    1
		//#else
		//#define RCF_FEATURE_FILETRANSFER    0
		//#endif
		//#endif
	[Note: The line "#define RCF_FEATURE_FILETRANSFER  1" must be uncommented]

4) The server and peer binaries are provided as part of the p2p package. You can play around with them by following the steps given in the points 7, 8 and 9 below.

5) Go to the source (src) folder and run the makefile.  
	[Note: Compilation in general takes time, as it links with 2 big libraries. This again depends on the system performance. In general, it might take upto 4 mins for compiling all the files. Also, we ain't using -std=c++11 constructs so to include it.]


6) Run the binaries(iServer1, iServer2,..., iServer8 and peer1, peer2,.., peer8, etc) in their respective windows, by executing as: ./<binary>
   [Note: Begin by executing iServer binaries first. Rest can be executed in any order].

7) Once you start the iServers, it will wait for the peers to connect and register files with it. Execute the Peers too.

8) Executing any peer binary will register its files and wait for you to enter the file to lookup and download.
	a) Enter any filename as you want, but as long as you are entering the right filename as in the directories the requested file will be downloaded.
	b) Ensure the iServer binaries are always running. Just don't press any key as long as you want the operations to happen.
	c) Follow the instructions as given by executing the peer binaries on the terminals and play with it.



Thanks!

Author: Umashankar
Email: urajaram@hawk.iit.edu





	 

