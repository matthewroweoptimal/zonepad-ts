#pragma once
#include "esphome/components/json/json_util.h"
#include "esphome/core/log.h"
#include <esp_http_client.h>
#include <string>

namespace oa_http {

inline int fetch_source_value(int zone, int &out_value) {
  out_value = -1;

  esp_http_client_config_t config = {};
  char url[96];
  snprintf(url, sizeof(url), "http://optimalaudio.local/api/get?zone=%d&source", zone);
  config.url = url;
  config.timeout_ms = 150;  // keep calls short to avoid WDT
  config.disable_auto_redirect = true;
  config.keep_alive_enable = false;
  config.method = HTTP_METHOD_GET;

  esp_http_client_handle_t client = esp_http_client_init(&config);
  if (!client) {
    ESP_LOGW("zone_poll", "Failed to init HTTP client");
    return -1;
  }

  int status = -1;
  esp_err_t err = esp_http_client_open(client, 0);
  if (err == ESP_OK) {
    status = esp_http_client_fetch_headers(client);
    status = esp_http_client_get_status_code(client);
    ESP_LOGD("zone_poll", "HTTP ok, status=%d url=%s", status, config.url);
    if (status == 200) {
      std::string body;
      char buf[256];
      int total_read = 0;
      while (true) {
        int r = esp_http_client_read(client, buf, sizeof(buf));
        if (r <= 0) break;
        body.append(buf, r);
        total_read += r;
        if (total_read > 4096) {
          ESP_LOGW("zone_poll", "HTTP body too large (%d), truncating", total_read);
          break;
        }
      }
      ESP_LOGD("zone_poll", "HTTP read total=%d", total_read);
      if (!body.empty()) {
        ESP_LOGD("zone_poll", "HTTP body: %s", body.c_str());
        auto doc = esphome::json::parse_json(body);
        if (!doc.isNull() && doc.is<JsonObject>()) {
          JsonObject root = doc.as<JsonObject>();
          out_value = root["value"] | -1;
        }
      } else {
        ESP_LOGW("zone_poll", "Empty HTTP body");
      }
    }
  } else {
    ESP_LOGW("zone_poll", "HTTP open failed: %d url=%s", err, config.url);
  }

  esp_http_client_cleanup(client);
  return status;
}

inline int fetch_param_value(int zone, const char *param, int &out_value) {
  out_value = -1;

  char url[128];
  snprintf(url, sizeof(url), "http://optimalaudio.local/api/get?zone=%d&%s", zone, param);

  esp_http_client_config_t config = {};
  config.url = url;
  config.timeout_ms = 150;  // keep calls short to avoid WDT
  config.disable_auto_redirect = true;
  config.keep_alive_enable = false;
  config.method = HTTP_METHOD_GET;

  esp_http_client_handle_t client = esp_http_client_init(&config);
  if (!client) {
    ESP_LOGW("zone_poll", "Failed to init HTTP client");
    return -1;
  }

  int status = -1;
  esp_err_t err = esp_http_client_open(client, 0);
  if (err == ESP_OK) {
    status = esp_http_client_fetch_headers(client);
    status = esp_http_client_get_status_code(client);
    ESP_LOGD("zone_poll", "HTTP ok, status=%d url=%s", status, config.url);
    if (status == 200) {
      std::string body;
      char buf[256];
      int total_read = 0;
      while (true) {
        int r = esp_http_client_read(client, buf, sizeof(buf));
        if (r <= 0) break;
        body.append(buf, r);
        total_read += r;
        if (total_read > 4096) {
          ESP_LOGW("zone_poll", "HTTP body too large (%d), truncating", total_read);
          break;
        }
      }
      ESP_LOGD("zone_poll", "HTTP read total=%d", total_read);
      if (!body.empty()) {
        ESP_LOGD("zone_poll", "HTTP body: %s", body.c_str());
        auto doc = esphome::json::parse_json(body);
        if (!doc.isNull() && doc.is<JsonObject>()) {
          JsonObject root = doc.as<JsonObject>();
          out_value = root["value"] | -1;
        }
      } else {
        ESP_LOGW("zone_poll", "Empty HTTP body");
      }
    }
  } else {
    ESP_LOGW("zone_poll", "HTTP open failed: %d url=%s", err, config.url);
  }

  esp_http_client_cleanup(client);
  return status;
}

inline void send_level(int zone, const char *param, int value) {
  char url[128];
  snprintf(url, sizeof(url), "http://optimalaudio.local/api/set?zone=%d&%s=%d", zone, param, value);

  esp_http_client_config_t config = {};
  config.url = url;
  config.timeout_ms = 150;
  config.disable_auto_redirect = true;
  config.keep_alive_enable = false;
  config.method = HTTP_METHOD_GET;

  esp_http_client_handle_t client = esp_http_client_init(&config);
  if (!client) {
    ESP_LOGW("zone_level", "Failed to init HTTP client url=%s", url);
    return;
  }

  esp_err_t err = esp_http_client_open(client, 0);
  if (err == ESP_OK) {
    int status = esp_http_client_fetch_headers(client);
    status = esp_http_client_get_status_code(client);
    ESP_LOGD("zone_level", "Sent %s -> status=%d", url, status);
  } else {
    ESP_LOGW("zone_level", "HTTP open failed: %d url=%s", err, url);
  }
  esp_http_client_cleanup(client);
}

}  // namespace oa_http
