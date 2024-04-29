#define SerialLoRa Serial1

const String SENDCODE_CONV = "1";
const String SENDCODE_DISCOVERY = "2";

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


char extract_char_after_second_comma(const char *str) {
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



void SendLoRa(String sendCode, String payload){
  Serial.print("Send LoRa cmd: ");
  String cmd = "AT+SEND=0,"+String(payload.length() + 1) +","+ String(sendCode) + String(payload)+"\r";
  Serial.println(cmd);
  SerialLoRa.println(cmd);
}

void LoRaBand(){
  Serial.print("Send LoRa cmd: ");
  String cmd = "AT+BAND=470000000";
  Serial.println(cmd);
  SerialLoRa.println(cmd);
}

void SendLatLon(float lat, float lon){
  char* p_lat = (char*)(&lat);
  char* p_lon = (char*)(&lon);


  // clean this up? ...
  String payload = String("LAT_LON_"); // lol ;)

  memcpy(&(payload[0]) , p_lat, 4 );
  memcpy(&(payload[4]) , p_lon, 4 );
  
  Serial.print("preparing lat lon payload: ");
  Serial.println(payload);


  Serial.print(  (String("Sending Lat Lon : ") +  String(millis()) ) + "\n" );

  SendLoRa(SENDCODE_CONV, payload);
}

void SendIncremented(){
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

void SendDiscovery(){
  String payload = String("DISCOVERY"); // lol ;)
  Serial.print("preparing discovery payload: ");
  Serial.println(payload);

  SendLoRa(SENDCODE_DISCOVERY, payload);
}

void ProcessDiscovery(){
  lastRcvTime = millis();
  // EnterConversationState();
}

void ProcessConversation(String payload){

  lastRcvTime = millis();

  Serial.print("Conversation Processed: ");
  Serial.println(payload);
}

void ProcessLoRa(String cmd){
  // check for REC string

  // too short to be a usable payload
  if (cmd.length() < 14){
      Serial.print("Received packet too small for transmission: ");
      Serial.println(cmd);
      return;
  }

  String recv = cmd.substring(0,4);

  if (recv != "+RCV"){
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

  String payloadLen_str = cmd.substring(10,14);
  int payloadLen = payloadLen_str.toInt();

  Serial.print("payloadLen_str: ");
  Serial.println(payloadLen_str);

  String payload_str = cmd.substring(14,15+payloadLen);

  Serial.print("payload_str: ");
  Serial.println(payload_str);

  if (sendCode_str == SENDCODE_DISCOVERY){
    ProcessDiscovery();
  }else if (sendCode_str == SENDCODE_CONV){
    ProcessConversation(payload_str);
  }

}

void PollLoRa(){
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



unsigned long nextDiscoveryTime = -1;

unsigned long GetNextDiscovery(){
  unsigned long time = millis() + random(1,3) * WINDOW_TIME;
  Serial.print(  (String("Next Discovery Time: ") +  String(time) ) + "\n" );
  return time;
}

void EnterDiscoveryState(){
  Serial.println("enter discovery");

  currentState = STATE_DISCOVERY;

  lastRcvTime = 0;
  stateEnterTime = millis();

  // calculate next discovery
  nextDiscoveryTime = GetNextDiscovery();
}

void DiscoveryState(){

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


unsigned long nextConversationTime = -1;


unsigned long GetNextConversation(){
  return millis() + 2 * WINDOW_TIME;
}


void EnterConversationState(){
  if (currentState == STATE_CONVERSATION){
    return;
  }

  Serial.println("enter conversation");
  currentState = STATE_CONVERSATION;
  stateEnterTime = millis();

  nextConversationTime = GetNextConversation();
}

void ConversationState(){
  // get a random number (0,1)
  // wait either 0 or 1 seconds 

  if ( millis() > nextConversationTime ){
    SendLatLon(0.1, 1.2);
    nextConversationTime = GetNextConversation();
  }
  
  PollLoRa();
}

void ListenState(){
  PollLoRa();
}

 
void ProcessComms(){
  if (currentState == STATE_DISCOVERY){
    DiscoveryState();
  }
  else if (currentState == STATE_CONVERSATION){
    ConversationState();
  }
}



void setup()
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

  EnterDiscoveryState();
}



void loop()
{
  currentTime = millis();
  ProcessComms();
}

