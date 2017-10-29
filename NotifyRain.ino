#include <Nefry.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

int isPushedButton = 0;
int sensorValue;
int isRain = 0;

const int httpPort = 80;
const char* host = "api.openweathermap.org";
const char* url = "/data/2.5/weather";

void setup(){
  Nefry.setLed(102,255,153);
  Nefry.enableSW();
  Nefry.setStoreTitle("cityID", 0);
  Nefry.setStoreTitle("APIKey", 1);
}

void loop(){
  if(Nefry.readSW()){
    if(isPushedButton){
      isPushedButton = 0;
    }else{
      isRain = askRain();
      isPushedButton = 1;
    }
    
    // 高速でloopしているので以下の処理が必要（？）
    Nefry.ndelay(20);
    while(Nefry.readSW()){
    }
  }

  if(isPushedButton){
    turnOnLed(isRain);
  }else{
    Nefry.setLed(102,255,153);
  }
}

int askRain(){
  // 天気APIを叩いて、雨かどうかを調べる
  // 雨だったら1を返す

  const int BUFFER_SIZE = JSON_OBJECT_SIZE(50) + JSON_ARRAY_SIZE(1);
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  String id = Nefry.getStoreStr(0);
  String appid = Nefry.getStoreStr(1);

  String body = "";
  HTTPClient http;

  http.begin(String("http://") + host + url + String("?id=") + id + String("&APPID=") + appid);
  int httpCode = http.GET();
  Nefry.ndelay(100);

  if (httpCode < 0) {
    body = http.errorToString(httpCode);
  } else {
    body = http.getString();
  }
  Nefry.println(body);
  http.end();
  
  // jsonデコード
  int len = body.length()+1; 
  char* json =new char[len];
  body.toCharArray(json, len);
  JsonObject& root = jsonBuffer.parseObject(json);
  if (!root.success()) {
    Nefry.println("parseObject() failed");
  }

  String weather = root["weather"][0]["main"];
  int weatherIsRain = 0;

  if (!weather.compareTo("Rain")) {
    weatherIsRain = 1;
  }

  return weatherIsRain;
  
}

void turnOnLed(int isRain){
  // 雨かどうかの引数によって、LEDの色が変わる
  if(isRain){
      Nefry.setLed(0,0,255);
      Nefry.ndelay(100);
      Nefry.setLed(0,0,0);
      Nefry.ndelay(100);
    }else{
      Nefry.setLed(255,165,0);
      Nefry.ndelay(100);
    }
}
