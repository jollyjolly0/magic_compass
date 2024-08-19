
# include "lora.h"


String LoRA::BlockingCommandLoRa(String cmd){
  logging_serial.print("Blocking Command: ");
  logging_serial.println(cmd);
  lora_serial.println(cmd);
  String response;
  // wait for response
  while (!lora_serial.available()) {
    delay(5);
  }
  while (lora_serial.available()) {
    if (lora_serial.available()) {
      response += String(char(lora_serial.read()));
    }
  }
  // logging_serial.print("Response: ");
  // logging_serial.println(cmd_response);
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
    logging_serial.println("generating address from chip UID...");
    String at_uuid_cmd = "AT+UID?";
    cmd_response = BlockingCommandLoRa(at_uuid_cmd);
    logging_serial.print("uid response: ");
    logging_serial.println(cmd_response);

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

    logging_serial.print("generated address: ");
    logging_serial.println(set_address);
  }


  String at_address_cmd = "AT+ADDRESS=" + String(set_address);
  cmd_response = BlockingCommandLoRa(at_address_cmd);
  logging_serial.print("address_response: ");
  logging_serial.println(cmd_response);


  String at_networkid_cmd = "AT+NETWORKID=" + NETWORK_ID;
  cmd_response = BlockingCommandLoRa(at_networkid_cmd);
  logging_serial.print("networkID response: ");
  logging_serial.println(cmd_response);


  String at_band_cmd = "AT+BAND=" + FREQ_BAND;
  cmd_response = BlockingCommandLoRa(at_band_cmd);
  logging_serial.print("band response: ");
  logging_serial.println(cmd_response);


  String at_param_cmd = "AT+PARAMETER=" + SPREAD_FACTOR + "," + BANDWIDTH + "," + CODING_RATE + "," + PREAMBLE;
  cmd_response = BlockingCommandLoRa(at_param_cmd);
  logging_serial.print("param response: ");
  logging_serial.println(cmd_response);
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

void LoRA::SendLoRa(String sendCode, byte* payload, unsigned int num_bytes){

  logging_serial.print("Send LoRa bytearray cmd: ");
  String cmd = "AT+SEND=0,"+String(num_bytes + 1) + "," + String(sendCode);
  
  byte* cmd_bytes = new byte[cmd.length() + num_bytes + 1];

  memcpy(cmd_bytes, cmd.c_str(), cmd.length());
  memcpy(cmd_bytes + cmd.length(), payload, num_bytes );
  memcpy(cmd_bytes + cmd.length() + num_bytes, "\r", 1 );

  logging_serial.println(( char*)cmd_bytes);
  lora_serial.println(( char*)cmd_bytes);

  delete[] cmd_bytes;
}

void LoRA::SendLoRa(String sendCode, String payload){
  logging_serial.print("Send LoRa String cmd: ");
  String cmd = "AT+SEND=0,"+String(payload.length() + 1) +","+ String(sendCode) + String(payload)+"\r";
  logging_serial.println(cmd);
  lora_serial.println(cmd);
}

void LoRA::LoRaBand(){
  logging_serial.print("Send LoRa cmd: ");
  String cmd = "AT+BAND=470000000";
  logging_serial.println(cmd);
  lora_serial.println(cmd);
}


void LoRA::SendLatLon() {
  float lat = send_lat;
  float lon = send_lon;

  LatLonPayload latlon_payload;
  latlon_payload.f[0] = lat;
  latlon_payload.f[1] = lon;

  logging_serial.print("preparing lat lon payload: ");
  // logging_serial.println(String(latlon_payload.s));


  // debuging lat lon
  char buffer[100];  // Adjust size as needed
  sprintf(buffer, "\t lat: %.3f lon: %.3f, bytes: [%d %d %d %d] , [%d %d %d %d]", lat, lon,
    (byte)latlon_payload.s[0], 
    (byte)latlon_payload.s[1],
    (byte)latlon_payload.s[2],
    (byte)latlon_payload.s[3],
    (byte)latlon_payload.s[4], 
    (byte)latlon_payload.s[5],
    (byte)latlon_payload.s[6],
    (byte)latlon_payload.s[7]);

  String log_string = String(buffer);

  logging_serial.println("Debug_lat_lon");
  logging_serial.println(log_string);

  LatLonPayload converted_latlon;
  memcpy(converted_latlon.s, latlon_payload.s, 8);

  logging_serial.println(converted_latlon.f[0]);
  logging_serial.println(converted_latlon.f[1]);


  logging_serial.print(  (String("Sending Lat Lon : ") +  String(millis()) ) + "\n" );

  SendLoRa(SENDCODE_CONV, latlon_payload.s , 8); // sizeof(latlonPayload)
}



void LoRA::SendIncremented(){
    logging_serial.print("Time since last send: ");
    logging_serial.println((millis()-lastSendTime));


  char* inc = (char*)(&num_messages);


  // clean this up? ...
  String payload = String("EXTRA_DATA") + String("data=") + String(num_messages); // lol ;)

  // logging_serial.print("preparing inc payload: ");
  // logging_serial.println(payload);

  SendLoRa(SENDCODE_CONV, payload);

  num_messages++;
}

void LoRA::SendDiscovery(){
  String payload = String("DISCOVERY"); // lol ;)
  logging_serial.print("preparing discovery payload: ");
  logging_serial.println(payload);

  SendLoRa(SENDCODE_DISCOVERY, payload);
}

void LoRA::ProcessDiscovery(){
  lastRcvTime = millis();
  // EnterConversationState();
}

void LoRA::ProcessConversation(byte* payload_ptr, int payload_len){

  lastRcvTime = millis();

  logging_serial.print("Conversation Processed: ");
  logging_serial.write(payload_ptr, (size_t)payload_len);
  logging_serial.println("");

  LatLonPayload recv_latlon;
  memcpy(recv_latlon.s, payload_ptr, payload_len);
  
  rcv_lat = recv_latlon.f[0];
  rcv_lon = recv_latlon.f[1];

  // debuging lat lon
  char buffer[100];  // Adjust size as needed
  sprintf(buffer, "\t lat: %.3f lon: %.3f, bytes: [%d %d %d %d] , [%d %d %d %d]", rcv_lat, rcv_lon,
    (byte)recv_latlon.s[0], 
    (byte)recv_latlon.s[1],
    (byte)recv_latlon.s[2],
    (byte)recv_latlon.s[3],
    (byte)recv_latlon.s[4], 
    (byte)recv_latlon.s[5],
    (byte)recv_latlon.s[6],
    (byte)recv_latlon.s[7]);

  String log_string = String(buffer);
  logging_serial.println(log_string);
}

void LoRA::ProcessLoRa(byte* msg_buffer, int msg_len){
  // check for REC string

  // too short to be a usable payload
  if (msg_len < 14) {
    logging_serial.print("Received packet too small for transmission: ");
    logging_serial.write(msg_buffer, (size_t)msg_len);
    return;
  }

  // check for REC string
  // String recv = cmd.substring(0, 4);
  String recv(reinterpret_cast<char*>(msg_buffer + 0), 4);
  if (recv != "+RCV") {
    logging_serial.print("Not a receive packet. discarding : ");
    logging_serial.write(msg_buffer, (size_t)msg_len);
    return;
  }


  logging_serial.print("Process LoRa message: ");
  logging_serial.write(msg_buffer, (size_t)msg_len);

  // TODO properly parse string 
  
  // String sendCode_str = cmd.substring(14,15);
  String sendCode_str = String(extract_char_after_second_comma((char*)msg_buffer));

  logging_serial.print("sendCode_str: ");
  logging_serial.println(sendCode_str);

  // String payloadLen_str = cmd.substring(10, 14);
  int payloadLen = 8;//payloadLen_str.toInt();

  // logging_serial.print("payloadLen_str: ");
  // logging_serial.println(payloadLen_str);

  byte* payload_ptr = (msg_buffer + 14);

  logging_serial.print("payload_str: ");
  logging_serial.write(payload_ptr, (size_t)payloadLen);
  logging_serial.println("");

  if (sendCode_str == SENDCODE_DISCOVERY){
    ProcessDiscovery();
  }else if (sendCode_str == SENDCODE_CONV){
    ProcessConversation(payload_ptr, payloadLen);
  }

}

void LoRA::PollLoRa(){
  //logging_serial.println("Polling for LoRa message");
  String inString;

  const int MAX_MSG_LEN = 100;

  byte msg_buffer[MAX_MSG_LEN] = {};
  int msg_index = 0;
  
  if(lora_serial.available()){
    while (lora_serial.available()){
    // should this be a while loop? can it build up multiple messages? 
      if(lora_serial.available()){
        msg_buffer[msg_index] = byte(lora_serial.read());
        msg_index++;

        // TODO check if we overflow
      }
    }
    ProcessLoRa(msg_buffer, msg_index);
  }
}



unsigned long LoRA::GetNextDiscovery(){
  unsigned long time = millis() + random(1,3) * WINDOW_TIME;
  logging_serial.print(  (String("Next Discovery Time: ") +  String(time) ) + "\n" );
  return time;
}

void LoRA::EnterDiscoveryState(){
  logging_serial.println("enter discovery");

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

  logging_serial.println("enter conversation");
  currentState = STATE_CONVERSATION;
  stateEnterTime = millis();

  nextConversationTime = GetNextConversation();
}

void LoRA::ConversationState(){
  // get a random number (0,1)
  // wait either 0 or 1 seconds 

  if ( millis() > nextConversationTime ){
    SendLatLon();
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
  // logging_serial.begin(9600);
  // lora_serial.begin(115200);
  delay(5000);
  
  // initialize random seed with analog noise

  int analog0 = analogRead(0);
  logging_serial.print("seed: ");
  logging_serial.println(analog0);


  randomSeed(analog0);

  logging_serial.println("booted");
  ConfigureLoRa(LORA_GENERATE_ID);

  EnterDiscoveryState();
}



void LoRA::update()
{
  currentTime = millis();
  ProcessComms();
}

LoRA::LoRA(arduino::Stream & lora_uart, arduino::Stream & logging_uart)
 : lora_serial(lora_uart), logging_serial(logging_uart)
{
}

void LoRA::set_send_lat_lon(float lat, float lon)
{
  send_lat = lat;
  send_lon = lon;
}

void LoRA::get_rcv_lat_lon(float &lat_out, float &lon_out)
{
  lat_out = rcv_lat;
  lon_out = rcv_lon;
}
