#ifndef lora_h

# define lora_h 

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif



union LatLonPayload {
  float f[2];
  byte s[8];
};

class LoRA{
public:

    LoRA(arduino::Stream & lora_serial, arduino::Stream & logging_serial);

    void init();
    void update();

    void set_send_lat_lon(float lat, float lon);
    void get_rcv_lat_lon(float& lat_out, float& lon_out);
    void get_rcv_time(float& last_time);

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

    float send_lat = -1;
    float send_lon = -1;

    float rcv_time = -1;
    float rcv_lat = -1;
    float rcv_lon = -1;

    
    unsigned long nextDiscoveryTime = -1;   

    unsigned long nextConversationTime = -1;




    String BlockingCommandLoRa(String cmd);
    void ConfigureLoRa(unsigned short address);
    char extract_char_after_second_comma(const char *str);
    void SendLoRa(String sendCode, String payload);
    void SendLoRa(String sendCode, byte* payload, unsigned int num_bytes);
    void LoRaBand();
    void SendLatLon();
    void SendIncremented();
    void SendDiscovery();
    void ProcessDiscovery();
    void ProcessConversation(byte* payload_buffer, int payload_len);
    void ProcessLoRa(byte* msg_buffer, int msg_len);
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