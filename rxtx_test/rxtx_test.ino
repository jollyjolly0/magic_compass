#define SerialLoRA Serial1

const char SENDCODE_LATLON = '1';
int val;




void SendLoRA(char sendCode, String payload){
  Serial.print("Send LoRA cmd: ");
  String cmd = "AT+SEND=0,"+String(payload.length() + 1) +","+ String(sendCode) + String(payload)+"\r";
  Serial.println(cmd);
  SerialLoRA.println(cmd);
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

  SendLoRA(SENDCODE_LATLON, payload);
}




void ProcessLoRA(String cmd){
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


  Serial.print("Process LoRA message: ");
  Serial.println(cmd);

  String sendCode_str = cmd.substring(9,10);

  Serial.print("sendCode_str: ");
  Serial.println(sendCode_str);

  String payloadLen_str = cmd.substring(10,14);
  int payloadLen = payloadLen_str.toInt();

  Serial.print("payloadLen_str: ");
  Serial.println(payloadLen_str);

  String payload_str = cmd.substring(14,15+payloadLen);

  Serial.print("payload_str: ");
  Serial.println(payload_str);

}

void PollLoRA(){
  //Serial.println("Polling for LoRA message");
  String inString;
  
  if(SerialLoRA.available()){
    while (SerialLoRA.available()){
    // should this be a while loop? can it build up multiple messages? 
      if(SerialLoRA.available()){
        inString += String(char(SerialLoRA.read()));
      }
    }
    ProcessLoRA(inString);
  }
}


void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  SerialLoRA.begin(115200);
  
}
 
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 1000;

float cur_lat = 0;
float cur_lon = 0;

void loop()
{
  //Serial.println("begin loop");
  unsigned long time = millis();


  if( time - lastSendTime > sendInterval){
    // SendLatLon(cur_lat, cur_lon);
    lastSendTime = time;
  }

  cur_lat = cur_lat + 1;
  cur_lon = cur_lon + 2;

  // intentionally process after send to see if send and receive clobber eachother on serial?

  PollLoRA();

}