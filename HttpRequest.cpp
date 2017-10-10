#include <Stepper.h>
#include <ArduinoJson.h>
#include "HttpRequest.h"
#include <string.h>
#include "config.h"

HttpRequest::HttpRequest() {}

HttpRequest::HttpRequest(char* inputMethod, char* inputPath, char* inputProtocol) {
  int strSize = (strlen("http://") + strlen(serverIP) + strlen(inputPath)) * sizeof(char);
  char* buildPath = malloc(strSize);
  strcpy (buildPath, "http://");
  strcpy (buildPath, serverIP);
  strcpy (buildPath, inputPath);
   
  httpRequestLine = {inputMethod, buildPath, inputProtocol};
  ammountOfHeaders = 0;
  bodySize = 0;
}

void  HttpRequest::sendRequest(EthernetClient *client) {
  client->println();
  client->print(httpRequestLine.method);
  client->print(httpRequestLine.path);
  client->print(httpRequestLine.protocol);
  client->print("\n");

  for (int i = 0; i < ammountOfHeaders; i++) {
    sendRequestHeader(httpRequestHeader[i], client);
  }
  client->println(); // always one empty line between headers and content

  if (bodySize > 0) {
    client->println(requestBody);
  }
}

void HttpRequest::sendRequestHeader(HttpRequestHeader header, EthernetClient *client) {
  client->print(header.key);
  client->print(header.value);
  client->print("\n");
}

void HttpRequest::addMetingToBody(Meting inputMeting) {

  char* metingJson = parseMetingToJsonBody(inputMeting);
  bodySize = strlen(metingJson);

  requestBody = metingJson;

  char* sizeString = new char[4];
  itoa(bodySize, sizeString, 10);

  addRequestHeader("Content-Length: ", sizeString);
  addRequestHeader("Content-Type: ","application/json");
}

char* HttpRequest::parseMetingToJsonBody(Meting inputMeting) {
  char* json = new char[100];

  StaticJsonBuffer<100> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["weatherstation"] = inputMeting.Weatherstation;
  //root["Timestamp"] = inputMeting.Timestamp;
  root["temperature"] = inputMeting.Temperature;
  root["illuminance"] = inputMeting.Illuminance;
  root.printTo(json, 100);
  return json;
}

void HttpRequest::addRequestHeader(char *key, char *value) {
  httpRequestHeader[ammountOfHeaders] = HttpRequestHeader{key, value};
  if (ammountOfHeaders < HEADERSIZE) {
    ammountOfHeaders ++;
  }
}

void HttpRequest::freeRequest() {
  //Alleen de onderste werkt op dit moment. Hier wordt aan gewerkt
    //free(httpRequestLine.method);
  free(httpRequestLine.path);
//    free(httpRequestLine.protocol);
//    for(int i = 0; i < ammountOfHeaders; i++){
//      free(httpRequestHeader[i].key);
//      free(httpRequestHeader[i].value);
//    }
  free(requestBody);
}

