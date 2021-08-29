#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

#define LILYGO_WATCH_2020_V1
#define LILYGO_WATCH_LVGL
#include <LilyGoWatch.h>

TTGOClass *ttgo;
String message = "";

lv_obj_t *msg;
lv_obj_t *title;
lv_obj_t *list1;

// The AWS IoT topics to publish and subscribe
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

WiFiClientSecure tacklebacnet = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting TackleBac to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  tacklebacnet.setCACert(AWS_CERT_CA);
  tacklebacnet.setCertificate(AWS_CERT_CRT);
  tacklebacnet.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, tacklebacnet);

  // Create a message handler
  client.onMessage(messageHandler);
  Serial.print("Connecting TackleBac to AWS IoT Core");
  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("");
  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  Serial.println("TackleBac and AWS IoT Connected!");
}

void publishMessage()
{
  unsigned long time;
  StaticJsonDocument<200> doc;
  
  time = millis();
  Serial.print("outgoing: ");
  
  doc["time"] = time;
  //doc["sensor_a0"] = analogRead(0);
  doc["COMPLETED"] = message;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
  
  Serial.println(time);
}

void messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  StaticJsonDocument<200> doc;
  char json[] = "{\"message\":\"world\"}";
  Serial.println(String(json));
  //deserializeJson(doc, json);

  String escaped = payload;
  
  if (escaped.charAt(0) == '\"') {
    escaped.replace("\\", "");
    escaped = escaped.substring(1, escaped.length() - 1);
  }
  Serial.println("escaped: " + escaped);
  
  deserializeJson(doc, escaped);
  const char* val = doc["message"];
  message = String(val);

  /*Add buttons to the list*/
  lv_obj_t * list_btn;
  list_btn = lv_list_add_btn(list1, NULL, message.c_str());
  lv_obj_set_event_cb(list_btn, event_handler);
  
  Serial.println(message);
  
}

/*when a room notification is pressed*/
void event_handler(lv_obj_t *obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    Serial.println("Button clicked!");
    /*The button/notification text is stored in the message varible*/ 
    message = lv_list_get_btn_text(obj);
    /*The correct notification is then removed from the list on screen*/
    lv_list_remove(list1, lv_list_get_btn_index(list1, obj));
    /*A message is published which logs the room cleansed*/
    publishMessage();
  }
}

void setup() {
  /*Initiate the watch object and begin, followed by turning on the backlight and the lvgl library*/ 
  ttgo = TTGOClass::getWatch();
  ttgo->begin();
  ttgo->openBL();
  ttgo->lvgl_begin();

  /*Create title for TackleBac Cleaner*/
  title = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(title, "TackleBac Cleaner");
  lv_obj_align(title, NULL, LV_ALIGN_CENTER, 0, -100);

  /*Creating a list */
  list1 = lv_list_create(lv_scr_act(), NULL);
  lv_obj_set_size(list1, 200, 200);
  lv_obj_align(list1, NULL, LV_ALIGN_CENTER, 0, 20);
  Serial.begin(9600);
  connectAWS();
}

void loop() {

  lv_task_handler();

  // MQTT
  client.loop();

  delay(100);
}
