
#include "SharpLCD.hpp"
#include "GrLib.hpp"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <math.h>

//added libaries by communication
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <string>
#include <map>
#include <time.h>
#include <chrono>
#include <vector>

using boost::asio::buffer;
using boost::asio::ip::address;
using boost::asio::ip::udp;
namespace ip = boost::asio::ip;

class UDPManager{
        boost::asio::io_service io_service;
        udp::endpoint broadcast_endpoint;
        std::string start_string = "EdisonPongs";
        std::string myip = "myip";
        int package_size;
        boost::array<char,100> buf;
        udp::socket socket;
        ip::udp::endpoint sender_endpoint;
        boost::system::error_code err;

    public:
	    UDPManager() : socket(io_service, udp::endpoint(udp::v4(), 7788)), broadcast_endpoint(ip::address_v4::broadcast(), 7788){
		    socket.set_option(boost::asio::socket_base::broadcast(true));    
	        // Broadcast data
            socket.non_blocking(true);
	    }
	    ~UDPManager(){
		    socket.close();
	    }
	    void sendAsMaster(std::array<char, 4> pad_position, std::array<char, 5> ball_position){
   		    socket.send_to(buffer(start_string), broadcast_endpoint);
       	    socket.send_to(buffer(pad_position), broadcast_endpoint);
       	    socket.send_to(buffer(ball_position), broadcast_endpoint);
	    }
	    void sendAsSlave(std::array<char, 4> pad_position){
   		    socket.send_to(buffer(start_string), broadcast_endpoint);
       	    socket.send_to(buffer(pad_position), broadcast_endpoint);
	    }
	    void sayHi(){
   		    socket.send_to(buffer(start_string), broadcast_endpoint);
    	    socket.send_to(buffer(myip), broadcast_endpoint);
	    }
        ip::address getSenderNameFromBroadcast(){
            return sender_endpoint.address();
        }
        int* getBallDataFromBroadcast(){
            int* ballData = new int[4];
            ballData[0] = (int)buf[1];
		    ballData[1] = (int)buf[2];
		    ballData[2] = (int)buf[3];
    	    ballData[3] = (int)buf[4];
            return ballData;
        }
        int* getPadDataFromBroadcast(){
            int* padData = new int[3];
            padData[0] = (int)buf[1];
            padData[1] = (int)buf[2];
            padData[2] = (int)buf[3];
            return padData;
        }
        
	    int didIReceive(){
			switch(buf[0]){
                case 1:{ //ball positions
                    return 3;
                }
				case 2:{ //pad positions
			        return 4;
			    }
			    case 'E':{
			        std::string read_E(buf.c_array(),package_size);
		            if (read_E == "EdisonPongs"){
                        return 1;
		            }
                    break;
                }
    		    case 'm':{
    			    std::string read_m(buf.c_array(),package_size);
		            if (read_m == "myip"){
    		            return 2;
            	    }
            	}
                default:
                    return 0;
           }
        }
        
	    bool receive(){
    	    int len = socket.receive_from(buffer(buf), sender_endpoint, 0, err);
            if (err == boost::asio::error::would_block) {
                return false;
            }else if(err) {
                std::cout << "error in receive_from" << err << std::endl;
                return false;
            }else{
                package_size=len;
                return true;
    	    }
	    }
};





///////////////////Example


pad_position = {(char)2,(char)position_x,(char)position_y,(char)selfId};

    udpManager->sendAsSlave(pad_position);