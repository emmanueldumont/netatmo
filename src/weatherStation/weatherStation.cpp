/**
  * \file weatherStation.cpp
  * \brief This file contains functions used to manages the netatmo weatherStation in oroclient
  * \author DUMONT Emmanuel
  * \date 05/2016
  */




#include <signal.h>

#include "ros/ros.h"
#include "std_msgs/String.h"

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>

using namespace std;

enum enCommand
{
    CMD_ADD_INST =1,
    CMD_ADD_PROP,
    CMD_FIND,
    CMD_REMOVE,
    CMD_CLEAR,
    LAST_CMD
};

#define PHP_COMMAND "php /home/dumont/catkin_ws/src/netatmo/src/weatherStation/php/script/weatherStationGetData.php"
#define PERIOD 10 // Time between each measurement

// Chatter able to communicate with oroclient
ros::Publisher gOroChatter_pub;

// Name of the current node in the ontology
char * gName;
bool gToBeFree; // Boolean which indicates if gName has to be freed

void sayMyName();
std::string exec();
void get_data(std::string msg);
void oroChatterSender(std_msgs::String msg);
void updateData(std::string data, std::string value);

// Create a function which manages a clean "CTRL+C" command -> sigint command
void sigint_handler(int dummy)
{
    ROS_INFO("Netatmo weatherStation is shutting down...");
    
    // Free gName
    if(gToBeFree == true)
    {
      gToBeFree = false;
      free(gName);
    }
        
    ROS_INFO("\n\n... Bye bye !\n   -Manu");
    exit(EXIT_SUCCESS); // Shut down the program
}




int main(int argc, char **argv)
{
	// Initialize global variables
	gToBeFree = false;
	
  ros::init(argc, argv, "weatherStation");

  ros::NodeHandle nOroCl;  // Communicate wit oroclient
  
  // Override the signal interrupt from ROS
  signal(SIGINT, sigint_handler);
  
  // If a name is passed as an argument
  if(argc > 1)
  {
    gName = argv[argc-1];
    gToBeFree = false;
  }
  else
  {  	
    gName = (char *) malloc ( 7 * sizeof(char));
    memset(gName, 0, 7);
  	snprintf(gName, 7 ,"WS");
  	 
  	gToBeFree = true;
	}
 
  gOroChatter_pub = nOroCl.advertise<std_msgs::String>("oroChatter", 10);
  
  usleep(500000); // necessary to be able to send data
  
  // say my name
  sayMyName();
  
  while(1)
  {
    //Get netatmo data
    string msg;
    msg = exec();
    
    cout << "[msg]\n"<< msg << "[\\msg]\n"<< endl;
    
    get_data(msg);
    sleep(PERIOD);
  }
  

  return 0;
}

// Explain who am i
void sayMyName()
{
  ros::Rate loop_rate(10); // Communicate slow rate

  std::stringstream ss;
  char enumCmd = (char)CMD_ADD_INST;
  
  ss << "BigBrother#"<< enumCmd <<"#"<<gName<<"#WeatherStation";
  
  std_msgs::String msg;
  msg.data = ss.str();
  oroChatterSender(msg);
}

// Execute PHP from C
std::string exec()
{
  char buffer[128];
  std::string result = "";
  FILE* pipe = popen(PHP_COMMAND, "r");
  if (!pipe) throw std::runtime_error("popen() failed!");
  try {
      while (!feof(pipe)) {
          if (fgets(buffer, 128, pipe) != NULL)
              result += buffer;
      }
  } catch (...) {
      pclose(pipe);
      throw;
  }
  pclose(pipe);
  return result;
}


void get_data(std::string msg)
{
  std::size_t firstFound;
  std::size_t lastFound;
  std::size_t diff;
  
  char buffer[10];
  
  std::string data;
  std::string value;
  
  printf("get_data\n");
  
  // From the received string, get Temp, CO2, noise
  for(int cpt = 0; cpt < 3; cpt ++)
  {
    memset(buffer,'\0', 10);
    
    switch(cpt)
    {
      // Get Noise
      case 0: {
        std::string str("Noise: ");
        std::string str2(" db");
        // Find first use of "Noise: "
        firstFound = msg.find(str);
        
        // If found : 
        if (firstFound!=std::string::npos)
        {
          // Add "Noise: "
          firstFound += 7;
          // Find first use of " db"
          lastFound = msg.find(str2);
          if (firstFound!=std::string::npos)
          {
            diff = lastFound-firstFound;
            // Extract the number and display it
            if((diff > 0) && (diff <10))
            {
              printf("diff OK\n");
              msg.copy(buffer,diff,firstFound);
              cout << "Noise is "<< buffer<< ", diff is " << diff << "\n" <<endl;
              printf("\n");
              data = string("Noise");
              value = string(buffer);
            }
            else
            {
              cout << "Error diff is not positive\n" << endl;
            }
          }
          else
          {
            cout << "db not found\n" << endl;
          }
        }
        else
        {
          cout << "Noise not found\n" << endl;
        }
      }
      break;
      
      // Get Temperature
      case 1:{
        std::string str("Temperature: ");
        std::string str2(" °C");
        // Find first use of "Noise: "
        firstFound = msg.find(str);
        
        // If found : 
        if (firstFound!=std::string::npos)
        {
          // Add "Temperature: "
          firstFound += 13;
          // Find first use of " db"
          lastFound = msg.find(str2);
          if (firstFound!=std::string::npos)
          {
            diff = lastFound-firstFound;
            // Extract the number and display it
            if((diff > 0) && (diff <10))
            {
              msg.copy(buffer,diff,firstFound);
              cout << "Temperature is "<< buffer<< ", diff is " << diff << "\n" <<endl;
              
              
              data = string("Temperature");
              value = string("28.5");
            }
            else
            {
              cout << "Error diff is not positive\n" << endl;
            }
          }
          else
          {
            cout << "°C not found\n" << endl;
          }
        }
        else
        {
          cout << "Temperature not found\n" << endl;
        }
      }
      break;
      
      // Get CO2
      case 2:{
        std::string str("CO2: ");
        std::string str2(" ppm");
        // Find first use of "Noise: "
        firstFound = msg.find(str);
        
        // If found : 
        if (firstFound!=std::string::npos)
        {
          // Add "CO2: "
          firstFound += 5;
          // Find first use of " db"
          lastFound = msg.find(str2);
          if (firstFound!=std::string::npos)
          {
            diff = lastFound-firstFound;
            // Extract the number and display it
            if((diff > 0) && (diff <10))
            {
              msg.copy(buffer,diff,firstFound);
              cout << "CO2 is "<< buffer<< ", diff is " << diff << "\n" <<endl;
              
              
              data = string("CO2");
              value = string(buffer);
            }
            else
            {
              cout << "Error diff is not positive\n" << endl;
            }
          }
          else
          {
            cout << "ppm not found\n" << endl;
          }
        }
        else
        {
          cout << "CO2 not found\n" << endl;
        }
      }
      break;
      
      default:
      break;
    } 
    
    // Send data to oroclient
    updateData( data, value);
  }
}

// Send data to oroclient
void oroChatterSender(std_msgs::String msg)
{ 
  gOroChatter_pub.publish(msg);
  ros::spinOnce();
}

// Update a specific data of a source
void updateData(std::string data, std::string value)
{
  std::stringstream ss;
  char enumCmd = 0;
  
  // Clear previous data otherwise update won't be done correctly
  ss.str("");
  enumCmd = (char)CMD_CLEAR;
  ss << "BigBrother#"<<enumCmd<<"#"<< gName <<"#"<< data <<"#?y";
  std_msgs::String msg;
  msg.data = ss.str();
  oroChatterSender(msg);
  
  // "Update" in oro server seems to not work correctly, re-add instead
  ss.str("");
  enumCmd = (char)CMD_ADD_PROP;
  ss << "BigBrother#" << enumCmd <<"#"<< gName << "#"<< data <<"#"<< value;
  
  msg.data = ss.str();
  oroChatterSender(msg);
}
