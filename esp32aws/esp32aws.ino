/* ESP32 AWS IoT
 *  
 * Simplest possible example (that I could come up with) of using an ESP32 with AWS IoT.
 *  
 * Author: Anthony Elder 
 * License: Apache License v2
 */
#include <WiFiClientSecure.h>
#include <PubSubClient.h> // install with Library Manager, I used v2.6.0

const char* ssid = "SmallBitsBigBytes";
const char* password = "2Widdlepea";

const char* awsEndpoint = "aff0pj8o9veij-ats.iot.eu-west-1.amazonaws.com";

// xxxxxxxxxx-certificate.pem.crt
const char* certificate_pem_crt = \

"-----BEGIN CERTIFICATE-----\n" \
"MIIDWjCCAkKgAwIBAgIVALrp6qv8Y5xrUZVe2vQCJQ5J5mJ0MA0GCSqGSIb3DQEB\n" \
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n" \
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMDEyMDcyMTIw\n" \
"MjdaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n" \
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCxTrG9Ma43WNOVfLC2\n" \
"YyI8KmWx4mzHN/rf0ELCd+U2sRSFb7S/5TQ7m0wwu8ZbLNhoTSxXYwzp/9hp67+l\n" \
"IgMhgAQYPG3FZ39JcnvhNCIGo+8J872RWFln35O7JfERSo/wthWxA2/TMVtY/lWm\n" \
"kJtOkELRRoeH7nTpfcuHPhMiPtNREAm4x7v8dNHR5y3eWucZe3uBKnMRvrLaePYx\n" \
"HcEP+E1o0ywP3cPAIPwU96/8TSZuSjK5EPBR4ejgQdEPqVVGOxKRd5NghHuZGeqS\n" \
"nCHsYlhkZnY0l6quwYRIRVoTqtzxGUdsZE+fCkmLKrza2FXABCwMjogs0T0AsM8w\n" \
"2mA5AgMBAAGjYDBeMB8GA1UdIwQYMBaAFI/C4fpH66RcFDRjIZ4zcqp5tZNdMB0G\n" \
"A1UdDgQWBBTW3vxgBQSeFoMNldObO8Rb/vxA5DAMBgNVHRMBAf8EAjAAMA4GA1Ud\n" \
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAYlhNZhonc00nitPpDFvW3c3L\n" \
"If5saN79yjvtYP4L0aIt13sBFE3yyqcz1Gk+d5mEYU0n04PA7+1zGsVy61zS8Ls3\n" \
"UWiUR7t0X+03jpL2GopMxtuGasIr9rxWVgC0PgbPRUfhz9CqoEe+mdKyK1swVge5\n" \
"WTqxTaNW6KsOldt66ss1+zq5zyQpyCqfgRqZQ4C1tsmkWAj8EcMTzLY64l1feGxA\n" \
"QTUTIHtTsuaAy4HSJgfUOICsezSFTbFHjiwpISnwfzEG4oPQx9dQDo/FAxPRGubR\n" \
"qXZM4B8TDxoAIQZp4KcNn308f0ZpHNl96Sa4esgtm0am2QR5xZXiuAspgZTi2Q==\n" \
"-----END CERTIFICATE-----\n";

// xxxxxxxxxx-private.pem.key
const char* private_pem_key = \

"-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEowIBAAKCAQEAsU6xvTGuN1jTlXywtmMiPCplseJsxzf639BCwnflNrEUhW+0\n" \
"v+U0O5tMMLvGWyzYaE0sV2MM6f/Yaeu/pSIDIYAEGDxtxWd/SXJ74TQiBqPvCfO9\n" \
"kVhZZ9+TuyXxEUqP8LYVsQNv0zFbWP5VppCbTpBC0UaHh+506X3Lhz4TIj7TURAJ\n" \
"uMe7/HTR0ect3lrnGXt7gSpzEb6y2nj2MR3BD/hNaNMsD93DwCD8FPev/E0mbkoy\n" \
"uRDwUeHo4EHRD6lVRjsSkXeTYIR7mRnqkpwh7GJYZGZ2NJeqrsGESEVaE6rc8RlH\n" \
"bGRPnwpJiyq82thVwAQsDI6ILNE9ALDPMNpgOQIDAQABAoIBAB8l3PGInbtZXmof\n" \
"aofP6H67T/ws88EXo3nYAzJYj6rI9vruAcnIQSHLiVnyUslHmojQLd74Jpeg+2q4\n" \
"IIWo8PXcoFuFmh8YrHLBquOIHgYenGqaKA41gBdQXop9AtlvyNOXmT3shaOLQyni\n" \
"HDfTpYTqsOJEyqBlkqJGGBAdJo1YQzEWFXSLJ/I3pmTbUU7PQz1ANjUAmF5sEUOK\n" \
"5piwh+PT1AO49cuR4bpsThJtTFC7nQjxJPavd3kle16vx6g3sBGJ16Tq0mD/Lhv0\n" \
"rvhnIqRnYmks/yj3+h9mIqDorkIG9XkHVwEXsUgNIC6/G8gmFsdiQhEnux8+Raay\n" \
"Vl97GtUCgYEA28Ccp8PmycGGTHyVwaMyuu5axNYJ/Jcr1xy8fPkJv71kYBGduJ2x\n" \
"ict8t0Q4hzGWSasQJ00fh+RaV8FOYGvlLS9NEKPrEnARso1JOeELWnMf1pdzGm+K\n" \
"+l26UKoq67lmMeSsk5wENtnvaoX59GSV6alhoMnt11J2vtPnKAyQ5wcCgYEAzo3G\n" \
"u3xkU4aFHB/vvM+ARPjdcLXIOSFJmT4k2E7joSrTnR/GknP7JT9UrRWxBezvyn6l\n" \
"0sTPRcVcmJDtj+c4xtNQFdWRgAgYQuUbfiwPUl5JiaMpHfSFEirpDKMqhqmqztks\n" \
"6EfazrVaXwjDmAbcfsc1RH85fRa2+UAcXkd/br8CgYBe5klAsfsiue5TbaIGo4Gu\n" \
"9GxCgOSDLo2CMii8F5g8M3/7dOAV8BPRnAzZ4B5BVArA+ZzaPBYXy2ewagx821t3\n" \
"Lewr+X300mzyHM24rxUGFJ11QKY2jmEISh3B5Brw/XnCpixY+pFfVrJr4DvXbPDI\n" \
"nZtJoqqRyEz7j+tNgk+5VQKBgQCZwADxKBvJ7LUChWc/pTl2ncS/lCCRHg0Zy59D\n" \
"gB2Cp01ThQabl0eXpzeRizb1ETRBB9rJNASRi+JsEl0zI8sofLz9M7cVlXDxwnaf\n" \
"iY5yIoscr934VT4EkvpclMwf5wWk6RW7ZVkjSu0VFoH5lQnmOMSW0xjo2tKiYC19\n" \
"pDzgIwKBgFmYHx0e72hESl7jzKH1ONQ9QSQjAfaVaHmQNXAs74gXxksRowvyO1z0\n" \
"2sZYaS/V+2QmuOzqIEjGBpdFC8I+M9TejRspZIYewoDHfwAK9XSqWTwXrFU29jrD\n" \
"S67KHdGPdJyD4gc5vA31ri0dlM+CagirD6ShUHzt+mTWwPRRMmIh\n" \
"-----END RSA PRIVATE KEY-----\n";

const char* rootCA = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
"rqXRfboQnoZsG4q5WTP468SQvvG5\n" \
"-----END CERTIFICATE-----\n";

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

void setup() {
  Serial.begin(115200); delay(50); Serial.println();
  Serial.println("ESP32 AWS IoT Example");
  Serial.printf("SDK version: %s\n", ESP.getSdkVersion());

  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); Serial.println(WiFi.localIP());

  wiFiClient.setCACert(rootCA);
  wiFiClient.setCertificate(certificate_pem_crt);
  wiFiClient.setPrivateKey(private_pem_key);
}

unsigned long lastPublish;
int msgCount;

void loop() {

  pubSubCheckConnect();

  if (millis() - lastPublish > 10000) {
    String msg = String("Hello from TackleBac: ") + ++msgCount;
    boolean rc = pubSubClient.publish("outTopic", msg.c_str());
    Serial.print("Published, rc="); Serial.print( (rc ? "OK: " : "FAILED: ") );
    Serial.println(msg);
    lastPublish = millis();
  }
}

void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("TackleBacHW");
      delay(1000);
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}
