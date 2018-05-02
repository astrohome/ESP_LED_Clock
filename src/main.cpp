#include "main.h"

String weatherKey;
String ipstring;
String Text;
char buf[256];

String y;     // год
String mon;   // месяц
String wd;    // день недели
String d;     // дени
String h;     // часоы
String m;     // минуты
String s;     // секунды

int disp=0;
int rnd;
int lp=0;

unsigned long eventTime=0;
int buttonstate =1;

String weatherMain = "";
String weatherDescription = "";
String weatherLocation = "";
String country;
int humidity;
int pressure;
float temp;
String tempz;

float lon;
float lat;

int clouds;
float windSpeed;
int windDeg;

String date;
String date1;
String currencyRates;
String weatherString;
String weatherString1;
String weatherStringz;
String weatherStringz1;
String weatherStringz2;

String cityID;
String chipID;

void setup()
{
  P.begin();
  P.setInvert(false);
  P.setFont(fontBG);

  bool CFG_saved = false;
  int WIFI_connected = false;
  Serial.begin(115200);


  pinMode(LED_PIN,OUTPUT);
  pinMode(buttonPin,INPUT);
  digitalWrite(buttonPin, HIGH);
  digitalWrite(LED_PIN, HIGH);
 
  //**** Network Config load 
  EEPROM.begin(512); // define an EEPROM space of 512Bytes to store data
  CFG_saved = ReadConfig();

  //  Connect to WiFi acess point or start as Acess point
  if (CFG_saved)  //if no configuration yet saved, load defaults
  {    
      // Connect the ESP8266 to local WIFI network in Station mode
      Serial.println("Booting");
      
      WiFi.mode(WIFI_STA);

  if (!config.dhcp)
  {
    WiFi.config(IPAddress(config.IP[0], config.IP[1], config.IP[2], config.IP[3] ),  IPAddress(config.Gateway[0], config.Gateway[1], config.Gateway[2], config.Gateway[3] ) , IPAddress(config.Netmask[0], config.Netmask[1], config.Netmask[2], config.Netmask[3] ) , IPAddress(config.DNS[0], config.DNS[1], config.DNS[2], config.DNS[3] ));
  }
      WiFi.begin(config.ssid.c_str(), config.password.c_str());
      printConfig();
      WIFI_connected = WiFi.waitForConnectResult();   
  
      if(WIFI_connected!= WL_CONNECTED ){
        Serial.println("Connection Failed! activating to AP mode...");
        Serial.print("Wifi ip:");Serial.println(WiFi.localIP());
        Serial.print("Email:");Serial.println(config.email.c_str());
        
      }
  }

  if ( (WIFI_connected!= WL_CONNECTED) or !CFG_saved) {
    // DEFAULT CONFIG
    scrollConnect();
    Serial.println("Setting AP mode default parameters");
    config.ssid = "UFA Iot";       // SSID of access point
    config.password = "" ;   // password of access point
    config.dhcp = true;
    config.IP[0] = 192; config.IP[1] = 168; config.IP[2] = 1; config.IP[3] = 100;
    config.Netmask[0] = 255; config.Netmask[1] = 255; config.Netmask[2] = 255; config.Netmask[3] = 0;
    config.Gateway[0] = 192; config.Gateway[1] = 168; config.Gateway[2] = 1; config.Gateway[3] = 1;
    config.DNS[0] = 192; config.DNS[1] = 168; config.DNS[2] = 1; config.DNS[3] = 1;
    config.ntpServerName = "0.ru.pool.ntp.org"; // to be adjusted to PT ntp.ist.utl.pt
    config.Update_Time_Via_NTP_Every =  10;
    config.timeZone = 3;
    config.isDayLightSaving = true;
    config.DeviceName = "API ключь";
    config.email = "cityID";
    WiFi.mode(WIFI_AP);  
    WiFi.softAP(config.ssid.c_str());
    Serial.print("Wifi ip:");Serial.println(WiFi.softAPIP());

   }
   

    // Start HTTP Server for configuration
    server.on ( "/", []() {
      Serial.println("admin.html");
      server.send_P ( 200, "text/html", PAGE_AdminMainPage);  // const char top of page
    }  );
  
    server.on ( "/favicon.ico",   []() {
      Serial.println("favicon.ico");
      server.send( 200, "text/html", "" );
    }  );
  
    // Network config
    server.on ( "/config.html", send_network_configuration_html );
    // Info Page
    server.on ( "/info.html", []() {
      Serial.println("info.html");
      server.send_P ( 200, "text/html", PAGE_Information );
    }  );
    server.on ( "/ntp.html", send_NTP_configuration_html  );
  
    //server.on ( "/appl.html", send_application_configuration_html  );
    server.on ( "/general.html", send_general_html  );
    //  server.on ( "/example.html", []() { server.send_P ( 200, "text/html", PAGE_EXAMPLE );  } );
    server.on ( "/style.css", []() {
      Serial.println("style.css");
      server.send_P ( 200, "text/plain", PAGE_Style_css );
    } );
    server.on ( "/microajax.js", []() {
      Serial.println("microajax.js");
      server.send_P ( 200, "text/plain", PAGE_microajax_js );
    } );
    server.on ( "/admin/values", send_network_configuration_values_html );
    server.on ( "/admin/connectionstate", send_connection_state_values_html );
    server.on ( "/admin/infovalues", send_information_values_html );
    server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
    //server.on ( "/admin/applvalues", send_application_configuration_values_html );
    server.on ( "/admin/generalvalues", send_general_configuration_values_html);
    server.on ( "/admin/devicename",     send_devicename_value_html);
 

    server.onNotFound ( []() {
      Serial.println("Page Not Found");
      server.send ( 400, "text/html", "Page not Found" );
    }  );
    server.begin();
    Serial.println( "HTTP server started" );

    // ***********  OTA SETUP
  
    ArduinoOTA.setHostname(config.DeviceName.c_str());
    ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
        ESP.restart();
      });
  
    ArduinoOTA.onError([](ota_error_t error) { 
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
        ESP.restart(); 
      });
  
  /* setup the OTA server */
  ArduinoOTA.begin();
  Serial.println("Ready");
  
  for(int i=0; i<3; i++){ // Bling the LED to show the program started
    digitalWrite(LED_PIN, LOW);
    delay(200);
    digitalWrite(LED_PIN, HIGH);
    delay(200);
  }
  
  ipstring = (
    String(WiFi.localIP()[0]) +"." +
    String(WiFi.localIP()[1]) + "." +
    String(WiFi.localIP()[2]) + "." +
    String(WiFi.localIP()[3])
  );

  for (uint8_t i=0; i<ARRAY_SIZE(catalog); i++)
  {
    catalog[i].speed *= P.getSpeed();
    catalog[i].pause *= 500;
  }

  ticker.attach(1, ISRsecondTick);

  if (WiFi.status() == WL_CONNECTED) {
    getTime();
    scrollIP();
  }  
  
  getTime();
  getWeatherData();
  getWeatherDataz();
  weatherKey = config.DeviceName.c_str();
  cityID = config.email.c_str();
}

// the loop function runs over and over again forever
void loop()
{
  // OTA request handling
  ArduinoOTA.handle();

  //  WebServer requests handling
  server.handleClient();

  //  feed de DOG :) 
  customWatchdog = millis();

  //**** Normal Skecth code here ... 

  if (lp >= 10) lp=0;

  if (disp == 0) {
    if (lp == 0) {
      getWeatherData();
      getWeatherDataz();
    }
    getTime();
    disp=1;
    lp++;
  }
   
  if (disp == 1) {
    rnd = random(0, ARRAY_SIZE(catalog));
    Text = h + ":" + m;
    displayInfo();
  }
   
  if (disp == 2) {
    Text = wd + " " + d + " " + mon + " " + y;
    scrollText();
  }

  if (disp == 3) {
    rnd = random(0, ARRAY_SIZE(catalog));
    Text = h + ":" + m;
    displayInfo1();
  }

  if (disp == 4) {
    Text = weatherString;
    scrollText1();
  }

  if (disp == 5) {
    rnd = random(0, ARRAY_SIZE(catalog));
    Text = h + ":" + m;
    displayInfo2();
  }

  if (disp == 6 ){
    Text = weatherStringz + " " + weatherStringz1;
    scrollText2();
  }
   
  //============длительное нажатие кнопки форматирует EEPROM
  int buttonstate = digitalRead(buttonPin);

  if (buttonstate == HIGH)
    eventTime=millis();
  if (millis() - eventTime > 5000) {      // при нажатии 15 секунд - 
    digitalWrite(16, LOW);
    resetAll();                 // форматируем EEPROM
    Serial.println("EEPROM formatted");
    ESP.restart();
  } else {
    digitalWrite(16, HIGH); 
  }
}

void resetAll()
{
  EEPROM.begin(512);
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++){
  EEPROM.write(i, 0);
  }
  EEPROM.end();
  ESP.reset();
}

//==========================================================
void getTime()
{
    getNTPtime();
    h = String (DateTime.hour/10) + String (DateTime.hour%10);
    m = String (DateTime.minute/10) + String (DateTime.minute%10);
    s = String (DateTime.second/10 + String (DateTime.second%10));

    d = String (DateTime.day);

    y = String (DateTime.year);
     
    if (DateTime.month == 1) mon = "Января";
    if (DateTime.month == 2) mon = "Февраля";
    if (DateTime.month == 3) mon = "Марта";
    if (DateTime.month == 4) mon = "Апреля";
    if (DateTime.month == 5) mon = "Мая";
    if (DateTime.month == 6) mon = "Июня";
    if (DateTime.month == 7) mon = "Июля";
    if (DateTime.month == 8) mon = "Августа";
    if (DateTime.month == 9) mon = "Сентября";
    if (DateTime.month == 10) mon = "Октября";
    if (DateTime.month == 11) mon = "Ноября";
    if (DateTime.month == 12) mon = "Декабря";

    if (DateTime.wday == 2) wd = "Понедельник";
    if (DateTime.wday == 3) wd = "Вторник";
    if (DateTime.wday == 4) wd = "Среда";
    if (DateTime.wday == 5) wd = "Четверг";
    if (DateTime.wday == 6) wd = "Пятница";
    if (DateTime.wday == 7) wd = "Суббота";
    if (DateTime.wday == 1) wd = "Воскресенье";
    
}
//==========================================================
void scrollIP()
{
  Text = "Ваш IP: "+ipstring;
  if (P.displayAnimate()) {
    utf8rus(Text).toCharArray(buf, 256);
    P.displayScroll(buf, PA_LEFT, PA_SCROLL_LEFT, 60);
  }
}
//==========================================================
void scrollConnect()
{
  Text = "Отсутствует подключение к WIFI. Подключитесь к точке доступа 'UFA Iot' и войдите в веб интерфейс 192.168.4.1" ;
  if  (P.displayAnimate()){
  utf8rus(Text).toCharArray(buf, 256);
  P.displayScroll(buf, PA_LEFT, PA_SCROLL_LEFT, 40);
  if (!P.displayAnimate()) disp = 3;
  }
}
//==========================================================
void displayInfo()
{
    if (P.displayAnimate()){
      utf8rus(Text).toCharArray(buf, 256);
      P.displayText(buf, PA_CENTER, catalog[rnd].speed, 5000, catalog[rnd].effect, catalog[rnd].effect);   
      if (!P.displayAnimate()) disp = 2;
    }
}
//==========================================================
void displayInfo1()
{
    if (P.displayAnimate()){
      utf8rus(Text).toCharArray(buf, 256);
      P.displayText(buf, PA_CENTER, catalog[rnd].speed, 5000, catalog[rnd].effect, catalog[rnd].effect);   
      if (!P.displayAnimate()) disp = 4;
    }
}
//==========================================================
void displayInfo2()
{
  if (P.displayAnimate()){
    utf8rus(Text).toCharArray(buf, 256);
    P.displayText(buf, PA_CENTER, catalog[rnd].speed, 5000, catalog[rnd].effect, catalog[rnd].effect);   
    if (!P.displayAnimate()) disp = 6;
  }
}
//==========================================================
void displayInfo3()
{
  if (P.displayAnimate()){
    utf8rus(Text).toCharArray(buf, 256);
    P.displayText(buf, PA_CENTER, catalog[rnd].speed, 5000, catalog[rnd].effect, catalog[rnd].effect);   
    if (!P.displayAnimate()) disp = 0;
  }
}
//==========================================================
void scrollText()
{
  if  (P.displayAnimate()) {
    utf8rus(Text).toCharArray(buf, 256);
    P.displayScroll(buf, PA_LEFT, PA_SCROLL_LEFT, 40);
    if (!P.displayAnimate()) disp = 3;
  }
}
//==========================================================
void scrollText1()
{
  if  (P.displayAnimate()) {
    utf8rus(Text).toCharArray(buf, 256);
    P.displayScroll(buf, PA_LEFT, PA_SCROLL_LEFT, 40);
    if (!P.displayAnimate()) disp = 5;
  }
}
//==========================================================
void scrollText2()
{
  if  (P.displayAnimate()) {
    utf8rus(Text).toCharArray(buf, 256);
    P.displayScroll(buf, PA_LEFT, PA_SCROLL_LEFT, 40);
    if (!P.displayAnimate()) disp = 0;
  }
}
//==========================================================
void scrollText3()
{
  if  (P.displayAnimate()) {
    utf8rus(Text).toCharArray(buf, 256);
    P.displayScroll(buf, PA_LEFT, PA_SCROLL_LEFT, 40);
    if (!P.displayAnimate()) disp = 0;
  }
}


// =======================================================================
// Берем погоду с сайта openweathermap.org
// =======================================================================



const char *weatherHost = "api.openweathermap.org";

void getWeatherData()
{
  Serial.print("connecting to "); Serial.println(weatherHost);
  if (client.connect(weatherHost, 80)) {
    client.println(String("GET /data/2.5/weather?id=") + cityID + "&units=metric&appid=" + weatherKey + "&lang=ru" + "\r\n" +
                "Host: " + weatherHost + "\r\nUser-Agent: ArduinoWiFi/1.1\r\n" +
                "Connection: close\r\n\r\n");
  } else {
    Serial.println("connection failed");
    return;
  }
  String line;
  int repeatCounter = 0;
  while (!client.available() && repeatCounter < 10) {
    delay(500);
    Serial.println("w.");
    repeatCounter++;
  }
  while (client.connected() && client.available()) {
    char c = client.read(); 
    if (c == '[' || c == ']') c = ' ';
    line += c;
  }

  client.stop();
  Serial.println(line + "\n");
  DynamicJsonBuffer jsonBuf;
  JsonObject &root = jsonBuf.parseObject(line);
  if (!root.success())
  {
    Serial.println("parseObject() failed");
    return;
  }
  //weatherMain = root["weather"]["main"].as<String>();
  weatherDescription = root["weather"]["description"].as<String>();
  weatherDescription.toLowerCase();
  //  weatherLocation = root["name"].as<String>();
  //  country = root["sys"]["country"].as<String>();
  temp = root["main"]["temp"];
  humidity = root["main"]["humidity"];
  pressure = root["main"]["pressure"];
  windSpeed = root["wind"]["speed"];
  windDeg = root["wind"]["deg"];
  clouds = root["clouds"]["all"];
  String deg = String(char('~'+25));
  weatherString = "Сейчас " + String(temp,0)+" ";
  weatherString += weatherDescription;
  weatherString += " Влажн " + String(humidity) + "% ";
  weatherString += "Давл " + String(pressure/1.3332239,0) + " мм ";
  //  weatherString += "Облачность: " + String(clouds) + "% ";

  String windDegString;

  if (windDeg>=345 || windDeg<=22) windDegString = "Северный";
  if (windDeg>=23 && windDeg<=68) windDegString = "Северо-восточный";
  if (windDeg>=69 && windDeg<=114) windDegString = "Восточный";
  if (windDeg>=115 && windDeg<=160) windDegString = "Юго-восточный";
  if (windDeg>=161 && windDeg<=206) windDegString = "Южный";
  if (windDeg>=207 && windDeg<=252) windDegString = "Юго-западный";
  if (windDeg>=253 && windDeg<=298) windDegString = "Западный";
  if (windDeg>=299 && windDeg<=344) windDegString = "Северо-западный";

  weatherString += "Ветер " + windDegString + " " + String(windSpeed,1) + " м/с";

  Serial.println("POGODA: " + String(temp,0) + "\n");
}

// =======================================================================
// Берем ПРОГНОЗ!!! погоды с сайта openweathermap.org
// =======================================================================

void getWeatherDataz()
{
  Serial.print("connecting to "); Serial.println(weatherHostz);
  if (client.connect(weatherHostz, 80)) {
    client.println(String("GET /data/2.5/forecast/daily?id=") + cityID + "&units=metric&appid=" + weatherKey + "&lang=ru" + "&cnt=2" + "\r\n" +
                "Host: " + weatherHostz + "\r\nUser-Agent: ArduinoWiFi/1.1\r\n" +
                "Connection: close\r\n\r\n");
  } else {
    Serial.println("connection failed");
    return;
  }
  String line;
  int repeatCounter = 0;
  while (!client.available() && repeatCounter < 10) {
    delay(500);
    Serial.println("w.");
    repeatCounter++;
  }
  while (client.connected() && client.available()) {
    char c = client.read(); 
    if (c == '[' || c == ']') c = ' ';
    line += c;
  }
  tvoday(line);
  Serial.println(tempz + "\n");

  client.stop();
  
  DynamicJsonBuffer jsonBuf;
  JsonObject &root = jsonBuf.parseObject(tempz);
  if (!root.success())
  {
    Serial.println("parseObject() failed");
    return;
  }
  lon = root ["coord"]["lon"];
  lat = root ["coord"]["lat"];
  
  float wSpeed = root ["speed"];
  int wDeg = root ["deg"];
  float tempMin = root ["temp"]["min"];
  float tempMax = root ["temp"]["max"];
  weatherDescription = root ["weather"]["description"].as<String>();
  
  weatherStringz = "Завтра " + String(tempMin,1) + " .. " + String(tempMax,1) + " " + weatherDescription;
  Serial.println("!!!!!PROGNOZ: " + weatherStringz + " Wind: "+wSpeed+ " WindDeg: "+(wDeg)+ "\n");
  
  String windDegString;

  if (wDeg>=345 || wDeg<=22) windDegString = "Северный";
  if (wDeg>=23 && wDeg<=68) windDegString = "Северо-восточный";
  if (wDeg>=69 && wDeg<=114) windDegString = "Восточный";
  if (wDeg>=115 && wDeg<=160) windDegString = "Юго-восточный";
  if (wDeg>=161 && wDeg<=206) windDegString = "Южный";
  if (wDeg>=207 && wDeg<=252) windDegString = "Юго-западный";
  if (wDeg>=253 && wDeg<=298) windDegString = "Западный";
  if (wDeg>=299 && wDeg<=344) windDegString = "Северо-западный";

  weatherStringz1 = "Ветер " + windDegString + " " + String(wSpeed,1) + " м/с";
}

void tvoday(String line)
{
    String s;
    int strt = line.indexOf('}');
    for (int i=1; i<=4; i++){
      strt = line.indexOf('}', strt + 1);
    }
    s = line.substring(2+strt, line.length());
    tempz=s;
}

String utf8rus(String source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;

  while (i < k) {
    n = source[i]; i++;

    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB8; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
  return target;
}
