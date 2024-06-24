
# include "lora.h"


String LoRA::BlockingCommandLoRa(String cmd){
  Serial.print("Blocking Command: ");
  Serial.println(cmd);
  SerialLoRa.println(cmd);
  String response;
  // wait for response
  while (!SerialLoRa.available()) {
    delay(5);
  }
  while (SerialLoRa.available()) {
    if (SerialLoRa.available()) {
      response += String(char(SerialLoRa.read()));
    }
  }
  // Serial.print("Response: ");
  // Serial.println(cmd_response);
  return response;

}

void LoRA::ConfigureLoRa(unsigned short address) {
  // blocks until setup
  // only devices on same network can communicate
  const String NETWORK_ID = "4";
  const String FREQ_BAND = "915000000";

  const String SPREAD_FACTOR = "12"; // this determines how "wide" the chirps are. larger spread takes longer to transmit, but more resilient to noise
  const String BANDWIDTH = "7"; // how much data you will try to push through per second? 7 = 125 KHz
  const String CODING_RATE = "1"; // error correction bits. 4/5 means 4 info bits , 1 error bit. setting 1 sets as 1/5? 
  const String PREAMBLE = "4";

  String cmd_response;

  unsigned short set_address = address;

  if (address == LORA_GENERATE_ID){
    Serial.println("generating address from chip UID...");
    String at_uuid_cmd = "AT+UID?";
    cmd_response = BlockingCommandLoRa(at_uuid_cmd);
    Serial.print("uid response: ");
    Serial.println(cmd_response);

    String uid = cmd_response.substring(5,256); // 5 to the end
    
    String hex_word;
    const int word_size = 6;
    for (int i = 0; i < uid.length() / word_size; i++){
      hex_word = "0x" + uid.substring( i*word_size, (i+1) * word_size );
      set_address+=(int)strtol(hex_word.c_str(), NULL, 0);
    }
    hex_word = "0x" + uid.substring( (uid.length() / word_size)*word_size, uid.length());
    set_address +=(int)strtol(hex_word.c_str(), NULL, 0);

    if (set_address == 0){
      set_address = 1;
    }

    Serial.print("generated address: ");
    Serial.println(set_address);
  }


  String at_address_cmd = "AT+ADDRESS=" + String(set_address);
  cmd_response = BlockingCommandLoRa(at_address_cmd);
  Serial.print("address_response: ");
  Serial.println(cmd_response);


  String at_networkid_cmd = "AT+NETWORKID=" + NETWORK_ID;
  cmd_response = BlockingCommandLoRa(at_networkid_cmd);
  Serial.print("networkID response: ");
  Serial.println(cmd_response);


  String at_band_cmd = "AT+BAND=" + FREQ_BAND;
  cmd_response = BlockingCommandLoRa(at_band_cmd);
  Serial.print("band response: ");
  Serial.println(cmd_response);


  String at_param_cmd = "AT+PARAMETER=" + SPREAD_FACTOR + "," + BANDWIDTH + "," + CODING_RATE + "," + PREAMBLE;
  cmd_response = BlockingCommandLoRa(at_param_cmd);
  Serial.print("param response: ");
  Serial.println(cmd_response);
}



char LoRA::extract_char_after_second_comma(const char *str) {
    int comma_count = 0;
    const char *ptr = str;

    // Iterate through the string
    while (*ptr != '\0') {
        // Check for comma
        if (*ptr == ',') {
            comma_count++;
            // Check if it's the second comma
            if (comma_count == 2) {
                // Move pointer to the character after the second comma
                ptr++;
                // Return the character
                return *ptr;
            }
        }
        // Move to the next character
        ptr++;
    }

    // If the second comma is not found or there are no characters after it, return '\0'
    return '\0';
}



void LoRA::SendLoRa(String sendCode, String payload){
  Serial.print("Send LoRa cmd: ");
  String cmd = "AT+SEND=0,"+String(payload.length() + 1) +","+ String(sendCode) + String(payload)+"\r";
  Serial.println(cmd);
  SerialLoRa.println(cmd);
}

void LoRA::LoRaBand(){
  Serial.print("Send LoRa cmd: ");
  String cmd = "AT+BAND=470000000";
  Serial.println(cmd);
  SerialLoRa.println(cmd);
}

void LoRA::SendLatLon(float lat, float lon) {
  char* p_lat = (char*)(&lat);
  char* p_lon = (char*)(&lon);


  // clean this up? ...
  String payload = String("LAT_LON_");  // lol ;)

  memcpy(&(payload[0]), p_lat, 4);
  memcpy(&(payload[4]), p_lon, 4);

  Serial.print("preparing lat lon payload: ");
  Serial.println(payload);


  Serial.print(  (String("Sending Lat Lon : ") +  String(millis()) ) + "\n" );

  SendLoRa(SENDCODE_CONV, payload);
}

void LoRA::SendIncremented(){
    Serial.print("Time since last send: ");
    Serial.println((millis()-lastSendTime));


  char* inc = (char*)(&num_messages);


  // clean this up? ...
  String payload = String("EXTRA_DATA") + String("data=") + String(num_messages); // lol ;)

  // Serial.print("preparing inc payload: ");
  // Serial.println(payload);

  SendLoRa(SENDCODE_CONV, payload);

  num_messages++;
}

void LoRA::SendDiscovery(){
  String payload = String("DISCOVERY"); // lol ;)
  Serial.print("preparing discovery payload: ");
  Serial.println(payload);

  SendLoRa(SENDCODE_DISCOVERY, payload);
}

void LoRA::ProcessDiscovery(){
  lastRcvTime = millis();
  // EnterConversationState();
}

void LoRA::ProcessConversation(String payload){

  lastRcvTime = millis();

  Serial.print("Conversation Processed: ");
  Serial.println(payload);
}

void LoRA::ProcessLoRa(String cmd){
  // check for REC string

  // too short to be a usable payload
  if (cmd.length() < 14) {
    Serial.print("Received packet too small for transmission: ");
    Serial.println(cmd);
    return;
  }

  // check for REC string
  String recv = cmd.substring(0, 4);
  if (recv != "+RCV") {
    Serial.print("Not a receive packet. discarding : ");
    Serial.println(cmd);
    return;
  }


  Serial.print("Process LoRa message: ");
  Serial.println(cmd);

  // TODO properly parse string 
  
  // String sendCode_str = cmd.substring(14,15);
  String sendCode_str = String(extract_char_after_second_comma(cmd.c_str()));

  Serial.print("sendCode_str: ");
  Serial.println(sendCode_str);

  String payloadLen_str = cmd.substring(10, 14);
  int payloadLen = payloadLen_str.toInt();

  Serial.print("payloadLen_str: ");
  Serial.println(payloadLen_str);

  String payload_str = cmd.substring(14, 15 + payloadLen);

  Serial.print("payload_str: ");
  Serial.println(payload_str);

  if (sendCode_str == SENDCODE_DISCOVERY){
    ProcessDiscovery();
  }else if (sendCode_str == SENDCODE_CONV){
    ProcessConversation(payload_str);
  }

}

void LoRA::PollLoRa(){
  //Serial.println("Polling for LoRa message");
  String inString;
  
  if(SerialLoRa.available()){
    while (SerialLoRa.available()){
    // should this be a while loop? can it build up multiple messages? 
      if(SerialLoRa.available()){
        inString += String(char(SerialLoRa.read()));
      }
    }
    ProcessLoRa(inString);
  }
}



unsigned long LoRA::GetNextDiscovery(){
  unsigned long time = millis() + random(1,3) * WINDOW_TIME;
  Serial.print(  (String("Next Discovery Time: ") +  String(time) ) + "\n" );
  return time;
}

void LoRA::EnterDiscoveryState(){
  Serial.println("enter discovery");

  currentState = STATE_DISCOVERY;

  lastRcvTime = 0;
  stateEnterTime = millis();

  // calculate next discovery
  nextDiscoveryTime = GetNextDiscovery();
}

void LoRA::DiscoveryState(){

  // get a random number (0,1)
  // wait either 0 or 1 seconds 

  if ( millis() > nextDiscoveryTime ){
    // send, and set new discovery time
    
    if (lastRcvTime != 0){
      EnterConversationState();
    }else{
      SendDiscovery();
      nextDiscoveryTime = GetNextDiscovery();
    }
  }
  
  PollLoRa();

}




unsigned long LoRA::GetNextConversation(){
  return millis() + 2 * WINDOW_TIME;
}


void LoRA::EnterConversationState(){
  if (currentState == STATE_CONVERSATION){
    return;
  }

  Serial.println("enter conversation");
  currentState = STATE_CONVERSATION;
  stateEnterTime = millis();

  nextConversationTime = GetNextConversation();
}

void LoRA::ConversationState(){
  // get a random number (0,1)
  // wait either 0 or 1 seconds 

  if ( millis() > nextConversationTime ){
    SendLatLon(0.1, 1.2);
    nextConversationTime = GetNextConversation();
  }
  
  PollLoRa();
}

void LoRA::ListenState(){
  PollLoRa();
}

 
void LoRA::ProcessComms(){
  if (currentState == STATE_DISCOVERY){
    DiscoveryState();
  }
  else if (currentState == STATE_CONVERSATION){
    ConversationState();
  }
}



void LoRA::init()
{

  // put your setup code here, to run once:
  Serial.begin(9600);
  SerialLoRa.begin(115200);
  delay(5000);
  
  // initialize random seed with analog noise

  int analog0 = analogRead(0);
  Serial.print("seed: ");
  Serial.println(analog0);


  randomSeed(analog0);

  Serial.println("booted");
  ConfigureLoRa(LORA_GENERATE_ID);

  EnterDiscoveryState();
}



void LoRA::update()
{
  currentTime = millis();
  ProcessComms();
}