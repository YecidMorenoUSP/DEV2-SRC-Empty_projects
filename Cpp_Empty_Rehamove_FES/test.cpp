#include <rehamovelib.c>
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <iostream>
#include <atomic>
#include <chrono> 

using namespace std;


int main(){ 
    cout<<"Test C++  ";
    RehamoveDevice * FES1 = open_port("COM14");
    battery_request(FES1);
    cout << "Bateria : " << get_battery(FES1)<<endl;
    change_mode(FES1,MODE_LOW_LEVEL);

    //print_device(FES1->device,"test1.txt");
    //return 0;

    int mA[] = {10,20,30,20,30};
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    for(int j = 0 ; j < 5 ; j++)
    for(int i = 0 ; i < 50 ; i++){  
            pulse(FES1,Smpt_Channel::Smpt_Channel_Blue,mA[j],100);   
    } 
    end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count()/1000.f << "[S]" << std::endl;
    change_mode(FES1,MODE_MID_LEVEL);
    set_pulse_data(FES1,30,80);
    run(FES1,Smpt_Channel_Blue,100,5000); 
 
       
    close_port(FES1);
    return 0; 
}  

