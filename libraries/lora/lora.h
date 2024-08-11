#ifndef lora_h

# define lora_h 

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


// TODO figure out how to pass in a serial port

#define SerialLoRa Serial2


// NEEDS Serial output to be init'd ?

class LoRA{
public:

    LoRA(arduino::Stream & lora_serial, arduino::Stream & logging_serial);

    void init();
    void update();

private:

    const int LORA_GENERATE_ID = 1234;

    const String SENDCODE_CONV = "1";
    const String SENDCODE_DISCOVERY = "2";
    const String SENDCODE_LATLON = "3";

    arduino::Stream & lora_serial;
    arduino::Stream & logging_serial;

    int val;
    int num_messages = 0;
    unsigned long lastSendTime = 0;
    unsigned long lastRcvTime = 0;
    unsigned long stateEnterTime = 0;
    const unsigned long sendInterval = 8000;
    float cur_lat = 0;
    float cur_lon = 0;
    int current_delay = 0;

    const unsigned long WINDOW_TIME = 4000;
    unsigned long currentTime = 0;

    const int STATE_BOOTING = -1;
    const int STATE_LISTEN = 0;
    const int STATE_DISCOVERY = 1;
    const int STATE_CONVERSATION = 2;

    int currentState = STATE_BOOTING;


    float last_lat = -1;
    float last_lon = -1;

    
    unsigned long nextDiscoveryTime = -1;   

    unsigned long nextConversationTime = -1;




    String BlockingCommandLoRa(String cmd);
    void ConfigureLoRa(unsigned short address);
    char extract_char_after_second_comma(const char *str);
    void SendLoRa(String sendCode, String payload);
    void LoRaBand();
    void SendLatLon(float lat, float lon);
    void SendIncremented();
    void SendDiscovery();
    void ProcessDiscovery();
    void ProcessConversation(String payload);
    void ProcessLoRa(String cmd);
    void PollLoRa();
    unsigned long GetNextDiscovery();
    void EnterDiscoveryState();
    void DiscoveryState();
    unsigned long GetNextConversation();
    void EnterConversationState();
    void ConversationState();
    void ListenState();
    void ProcessComms();



};


#endif