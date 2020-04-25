#include "Weather.h" 
#include "Config.h"
#include <json-c/json.h>
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <iostream>

Weather::Weather()
    :mThread(nullptr)
    ,mUpdateDone(false)
{
    /* Schedule the very first update */
    SysTimer::getInstance().subscribe(*this, 1000, false);
}

void Weather::updateWeather()
{
    std::string json = getRawWeatherData();
    parseRawWeatherData(json);
    mUpdateDone = true;
}

std::vector<Weather::WeatherInfo> Weather::getWeatherForecast()
{
    std::lock_guard<std::mutex> lock(mWeatherMutex);
    return mWeatherForecast;
}

void Weather::onTimer()
{
    if (!mThread)
    {
        mUpdateDone = false;
        mThread = new std::thread(&Weather::updateWeather, this);
        /* Periodically check if thread is joinable */
        SysTimer::getInstance().subscribe(*this, 1000, true);
        return;
    }
    if (mUpdateDone)
    {
        mThread->join();
        delete mThread;
        mThread = nullptr;
        /* Schedule a next check */
        SysTimer::getInstance().subscribe(*this, Config::Weather::UPDATE_INTERVAL, true);
    }
}

int Weather::parseWeatherInfo(WeatherInfo &info, struct json_object *object)
{
    json_object *tmp_obj;
    json_object *weather_obj;
    memset(&info, 0, sizeof(WeatherInfo));

    if (!json_object_object_get_ex(object, "dt", &tmp_obj))
    {
        return 1;
    }
    info.timestamp = json_object_get_int64(tmp_obj);

    if (!json_object_object_get_ex(object, "temp", &tmp_obj))
    {
        return 1;
    }
    info.temperature = json_object_get_double(tmp_obj);

    if (!json_object_object_get_ex(object, "pressure", &tmp_obj))
    {
        return 1;
    }
    info.pressure = json_object_get_double(tmp_obj);

    if (!json_object_object_get_ex(object, "humidity", &tmp_obj))
    {
        return 1;
    }
    info.humidity = json_object_get_double(tmp_obj);

    if (!json_object_object_get_ex(object, "wind_speed", &tmp_obj))
    {
        return 1;
    }
    info.wind_speed = json_object_get_double(tmp_obj);

    if (json_object_object_get_ex(object, "rain", &tmp_obj))
    {
        info.rain = json_object_get_double(tmp_obj);
    }
    else
    {
        info.rain = 0;
    }

    if (json_object_object_get_ex(object, "snow", &tmp_obj))
    {
        info.snow = json_object_get_double(tmp_obj);
    }
    else
    {
        info.snow = 0;
    }

    if (!json_object_object_get_ex(object, "weather", &weather_obj))
    {
        return 1;
    }
    weather_obj = json_object_array_get_idx(weather_obj, 0);
    if (nullptr == weather_obj)
    {
        return 1;
    }
    if (!json_object_object_get_ex(weather_obj, "id", &tmp_obj))
    {
        return 1;
    }
    info.weatherId = json_object_get_int(tmp_obj);
    if (!json_object_object_get_ex(weather_obj, "icon", &tmp_obj))
    {
        return 1;
    }
    info.weatherIcon = json_object_get_string(tmp_obj);

    return 0;
}

static int writer(char *data, size_t size, size_t nmemb,
                  std::string *writerData)
{
  if(writerData == NULL)
    return 0;

  writerData->append(data, size*nmemb);

  return size * nmemb;
}

std::string Weather::getRawWeatherData()
{
    static char errorBuffer[CURL_ERROR_SIZE];
    std::string ret;
    CURLcode code;
    CURL *conn;

    conn = curl_easy_init();

    if(conn == NULL) {
        fprintf(stderr, "Failed to create CURL connection\n");
        return ret;
    }

    code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer);
    if(code != CURLE_OK) {
        fprintf(stderr, "Failed to set error buffer [%d]\n", code);
        goto CURL_CLEANUP;
    }

    code = curl_easy_setopt(conn, CURLOPT_URL, Config::Weather::UPDATE_URL);
    if(code != CURLE_OK) {
        fprintf(stderr, "Failed to set URL [%s]\n", errorBuffer);
        goto CURL_CLEANUP;
    }

    code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L);
    if(code != CURLE_OK) {
        fprintf(stderr, "Failed to set redirect option [%s]\n", errorBuffer);
        goto CURL_CLEANUP;
    }

    code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer);
    if(code != CURLE_OK) {
        fprintf(stderr, "Failed to set writer [%s]\n", errorBuffer);
        goto CURL_CLEANUP;
    }

    code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, &ret);
    if(code != CURLE_OK) {
        fprintf(stderr, "Failed to set write data [%s]\n", errorBuffer);
        goto CURL_CLEANUP;
    }

    /* Perform the request, res will get the return code */
    code = curl_easy_perform(conn);
    if(code != CURLE_OK){
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(code));
        goto CURL_CLEANUP;
    }


CURL_CLEANUP:
    curl_easy_cleanup(conn);
    return ret;
}

int Weather::parseRawWeatherData(std::string json)
{
    std::lock_guard<std::mutex> lock(mWeatherMutex);

    json_object *parsed_json;
    json_object *current;
    json_object *hourly;

    WeatherInfo info;
    /* 1. Parse original json */
    parsed_json = json_tokener_parse(json.c_str());
    if (nullptr == parsed_json)
    {
        return 1;
    }
    /* 2. Parse current weather json object */
    if (!json_object_object_get_ex(parsed_json, "current", &current))
    {
        return 1;
    }
    if (0 != parseWeatherInfo(info, current))
    {
        return 1;
    }
    mWeatherForecast.clear();
    mWeatherForecast.push_back(info);
    /* 3. Get hourly json array object */
    if (!json_object_object_get_ex(parsed_json, "hourly", &hourly))
    {
        return 1;
    }
    size_t n_hourly = json_object_array_length(hourly);
    for(size_t i = 0; i < n_hourly; i++)
    {
        WeatherInfo info;
        json_object *hourly_item = json_object_array_get_idx(hourly, i);
        if (nullptr == hourly_item)
        {
            return 1;
        }
        if (0 != parseWeatherInfo(info, hourly_item))
        {
            return 1;
        }
        mWeatherForecast.push_back(info);
    }

    return 0;
}
