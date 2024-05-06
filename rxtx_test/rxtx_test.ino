#define SerialLoRa Serial1

const char SENDCODE_LATLON = '1';

const int LORA_GENERATE_ID = 1234;
int val;


String BlockingCommandLoRa(String cmd){
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

void ConfigureLoRa(unsigned short address) {
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

void SendLoRa(char sendCode, String payload) {
  Serial.print("Send LoRa cmd: ");
  String cmd = "AT+SEND=0," + String(payload.length() + 1) + "," + String(sendCode) + String(payload) + "\r";
  Serial.println(cmd);
  SerialLoRa.println(cmd);
}


void SendLatLon(float lat, float lon) {
  char* p_lat = (char*)(&lat);
  char* p_lon = (char*)(&lon);


  // clean this up? ...
  String payload = String("LAT_LON_");  // lol ;)

  memcpy(&(payload[0]), p_lat, 4);
  memcpy(&(payload[4]), p_lon, 4);

  Serial.print("preparing lat lon payload: ");
  Serial.println(payload);

  SendLoRa(SENDCODE_LATLON, payload);
}




void ProcessLoRa(String cmd) {
  // check for +OK string
  String OK = cmd.substring(0, 3);
  if (OK == "+OK") {
    Serial.print("Command Successful: ");
    Serial.println(cmd);
    return;
  }

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

  Serial.print("*** RECEIVED *** ");
  Serial.print("Process LoRa message: ");
  Serial.println(cmd);

  String sendCode_str = cmd.substring(9, 10);

  Serial.print("sendCode_str: ");
  Serial.println(sendCode_str);

  String payloadLen_str = cmd.substring(10, 14);
  int payloadLen = payloadLen_str.toInt();

  Serial.print("payloadLen_str: ");
  Serial.println(payloadLen_str);

  String payload_str = cmd.substring(14, 15 + payloadLen);

  Serial.print("payload_str: ");
  Serial.println(payload_str);
}

void PollLoRa() {
  //Serial.println("Polling for LoRa message");
  String inString;

  if (SerialLoRa.available()) {
    while (SerialLoRa.available()) {
      // should this be a while loop? can it build up multiple messages?
      if (SerialLoRa.available()) {
        inString += String(char(SerialLoRa.read()));
      }
    }
    ProcessLoRa(inString);
  }
}


void setup() {
  delay(5000); // delay so i have time to connect serial port

  Serial.begin(115200);
  SerialLoRa.begin(115200);

  ConfigureLoRa(LORA_GENERATE_ID);
}

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 10000;

float cur_lat = 0;
float cur_lon = 0;

void loop() {
  //Serial.println("begin loop");
  unsigned long time = millis();


  if (time - lastSendTime > sendInterval) {
    // SendLatLon(cur_lat, cur_lon);
    lastSendTime = time;
  }

  cur_lat = cur_lat + 1;
  cur_lon = cur_lon + 2;

  // intentionally process after send to see if send and receive clobber eachother on serial?

  PollLoRa();
}