#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include "SimpleModbusMaster.h"

#define baud 9600
#define timeout 1000
#define polling 200
#define retry_count 10
#define TxEnablePin 0
#define LED_BUILTIN 2

#define DEBUG 1
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

WiFiMulti WiFiMulti;
const uint32_t connectTimeoutMs = 10000;

const long utctimezone = 28800;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "id.pool.ntp.org", utctimezone);
String Weeks[7] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};
String Months[12] = {"Januari", "Februari", "Maret", "April", "Mei", "Juni", "Juli", "Agustus", "September", "Oktober", "November", "Desember"};

const char* hostWeb = "script.google.com:443";
String GOOGLE_SCRIPT_ID = "xxxxxxxxxxxxxxxxxxxxxxx"; // silahkan sesuaikan Gscript ID

enum
{
  PACKET1,
  PACKET2,
  PACKET3,
  PACKET4,
  PACKET5,
  PACKET6,
  PACKET7,
  TOTAL_NO_OF_PACKETS
};

Packet packets[TOTAL_NO_OF_PACKETS];
packetPointer packet1 = &packets[PACKET1];
packetPointer packet2 = &packets[PACKET2];
packetPointer packet3 = &packets[PACKET3];
packetPointer packet4 = &packets[PACKET4];
packetPointer packet5 = &packets[PACKET5];
packetPointer packet6 = &packets[PACKET6];
packetPointer packet7 = &packets[PACKET7];

unsigned int load[5];
unsigned int kvar[1];
unsigned int volt[5];
unsigned int current[1];
unsigned int freq[1];
unsigned int standexp[1];
unsigned int standimp[1];
int i = 0;

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  pinMode(LED_BUILTIN, OUTPUT);
  delay(500);
  WiFiMulti.addAP("xxxxxxxxxx", "xxxxxxxxx"); //silahkan sesuaikan SSID dan pass wifi
  while (WiFiMulti.run(connectTimeoutMs) != WL_CONNECTED)
  {
    debugln("Connection failed, Restarting");
    delay(500);
    ESP.restart();
  }
  digitalWrite(LED_BUILTIN, HIGH);
  debugln("Connected to WiFi");
  timeClient.begin();
  
  packet1->id = 1;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = 1139;
  packet1->no_of_registers = 5;
  packet1->register_array = load;

  packet2->id = 1;
  packet2->function = READ_HOLDING_REGISTERS;
  packet2->address = 1146;
  packet2->no_of_registers = 1;
  packet2->register_array = kvar;
  
  packet3->id = 1;
  packet3->function = READ_HOLDING_REGISTERS;
  packet3->address = 1119;
  packet3->no_of_registers = 5;
  packet3->register_array = volt;
  
  packet4->id = 1;
  packet4->function = READ_HOLDING_REGISTERS;
  packet4->address = 1104;
  packet4->no_of_registers = 1;
  packet4->register_array = current;

  packet5->id = 1;
  packet5->function = READ_HOLDING_REGISTERS;
  packet5->address = 1179;
  packet5->no_of_registers = 1;
  packet5->register_array = freq;

  packet6->id = 1;
  packet6->function = READ_HOLDING_REGISTERS;
  packet6->address = 1708;
  packet6->no_of_registers = 1;
  packet6->register_array = standexp;

  packet7->id = 1;
  packet7->function = READ_HOLDING_REGISTERS;
  packet7->address = 1700; 
  packet7->no_of_registers = 1;
  packet7->register_array = standimp;

  modbus_configure(baud, timeout, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS);
}

void loop()
{
    unsigned int connection_status = modbus_update(packets);
    if (connection_status != TOTAL_NO_OF_PACKETS)
    {
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
      ESP.restart();
    }
    else
    {
      digitalWrite(LED_BUILTIN, HIGH);
    }
    
    timeClient.update();
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    
    int currentSecond = timeClient.getSeconds();
    if (currentSecond == 0)
    {
      String time_format = timeClient.getFormattedTime();
      unsigned long epochTime = timeClient.getEpochTime();
      
      struct tm *ptm = gmtime((time_t *)&epochTime);
      String hari, bulan, tahun, tanggal_sekarang;
      int tanggal;
      hari = String(Weeks[timeClient.getDay()]);
      tanggal = ptm->tm_mday;
      bulan = String(Months[(ptm->tm_mon + 1) - 1]);
      tahun = String(ptm->tm_year + 1900);
      tanggal_sekarang = toStringAddZero(tanggal) + "-" + bulan + "-" + tahun;
  
      int jam, menit, detik;
      String waktu_kirim, waktu_print;
      jam = timeClient.getHours();
      menit = timeClient.getMinutes();
      detik = timeClient.getSeconds();
      waktu_kirim = String(jam) + ":" + toStringAddZero(menit);
      waktu_print = String(jam) + ":" + toStringAddZero(menit) + ":" + toStringAddZero(detik);
      
      debug("load0 = ");
      debugln(load[0]);
      debug("load1 = ");
      debugln(load[1]);
      debug("load2 = ");
      debugln(load[2]);
      debug("load3 = ");
      debugln(load[3]);
      debug("load4 = ");
      debugln(load[4]);
      debug("var = ");
      debugln(kvar[0]);
      debug("volt0 = ");
      debugln(volt[0]);
      debug("volt1 = ");
      debugln(volt[1]);
      debug("volt2 = ");
      debugln(volt[2]);
      debug("volt3 = ");
      debugln(volt[3]);
      debug("current = ");
      debugln(current[0]);
      debug("freq = ");
      debugln(float(freq[0]*0.01));
      debug("exp = ");
      debugln(standexp[0]);
      debug("imp = ");
      debugln(standimp[0]);
      debugln("");
      
      String urlFinal = "https://" + String(hostWeb) + "/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + "id=UNIT_1" 
      + "&date=" + (tanggal_sekarang) 
      + "&time=" + (waktu_kirim) 
      + "&load=" + (load[3]) 
      + "&kvar=" + (kvar[0]) 
      + "&volt=" + (volt[3]) 
      + "&current=" + (current[0]) 
      + "&freq=" + (float(freq[0]*0.01)) 
      + "&standexp=" + (standexp[0]) 
      + "&standimp=" + (standimp[0]);
          
      http.begin(client, urlFinal.c_str());
      int httpCode = http.GET();
      debug("HTTP Status Code: ");
      debugln(httpCode);
      String payload;
      if (httpCode > 0)
      {
        payload = http.getString();
        debugln("Payload: " + payload);
        debugln("");
      }
      if (httpCode < 0)
      {
        debugln("HTTP request failed. Restarting ESP...");
        digitalWrite(LED_BUILTIN, LOW);
        delay(1000);
        ESP.restart();
      }
      Serial.flush();
      http.end();
    }
}
