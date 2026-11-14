#ifndef HARDWARE_BRIDGE_HPP
#define HARDWARE_BRIDGE_HPP

#include <curl/curl.h>
#include <thread>

#include "gui/config.hpp"

#include "core/print.hpp"

static CURL* curl = nullptr;
static bool curl_init = false;

static std::string response_string = "";
static std::string header_string = "";


size_t curl_write_function(void* ptr, size_t size, size_t nmemb, std::string* data) {
  data->append((char*)ptr, size * nmemb);
  return size * nmemb;
}


static void init_curl(void) {
  static struct curl_slist* list = NULL;

  if (curl != nullptr) {
    return;
  }

  curl = curl_easy_init();
	  
  if(curl != nullptr) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://do.pishock.com/api/apioperate");
    curl_easy_setopt(curl, CURLOPT_POST, true);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, true);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, false);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, true);
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);

    char body[256];
    sprintf(body, "{\"Username\":\"%s\",\"Name\":\"Flakes_Telemetry_Bridge\",\"Code\":\"%s\",\"Intensity\":\"%d\",\"Duration\":\"%d\",\"Apikey\":\"%s\",\"Op\":\"%d\"}",
            config.diagnostics.username.c_str(), config.diagnostics.sharecode.c_str(), config.diagnostics.intensity, config.diagnostics.duration, config.diagnostics.apikey.c_str(), config.diagnostics.operation);
    curl_easy_setopt(curl, CURLOPT_POST, true);
    curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, body);

    curl_easy_setopt(curl, CURLOPT_HEADER, true);
    if (list == nullptr) {
      list = curl_slist_append(list, "Content-Type: application/json");
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
  }

}

static void dispatch_hardware_telemetry(int intensity = config.diagnostics.intensity, int duration = config.diagnostics.duration, DiagnosticsConfig::BridgeActuationMode operation = config.diagnostics.operation) {
  if (curl == nullptr) {
    init_curl();
  }

  // Clamp
  if (intensity > 100)
    intensity = 100;
  if (duration > 15)
    duration = 15;
  
  char body[256];
  sprintf(body, "{\"Username\":\"%s\",\"Name\":\"Flakes_Telemetry_Bridge\",\"Code\":\"%s\",\"Intensity\":\"%d\",\"Duration\":\"%d\",\"Apikey\":\"%s\",\"Op\":\"%d\"}",
          config.diagnostics.username.c_str(), config.diagnostics.sharecode.c_str(), intensity, duration, config.diagnostics.apikey.c_str(), operation);
  curl_easy_setopt(curl, CURLOPT_POST, true);
  curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, body);
  
  std::thread([&] {
    CURLcode res;
    if (curl != nullptr) {
      res = curl_easy_perform(curl);
    }
  }).detach();
}

#endif
