/*!
 * @file AdafruitIO_Feed.cpp
 *
 *
 * Adafruit invests time and resources providing this open source code.
 * Please support Adafruit and open source hardware by purchasing
 * products from Adafruit!
 *
 * Copyright (c) 2015-2016 Adafruit Industries
 * Authors: Tony DiCola, Todd Treece
 * Licensed under the MIT license.
 *
 * All text above must be included in any redistribution.
 *
 */
#include "AdafruitIO_Feed.h"
#include "AdafruitIO.h"

/**************************************************************************/
/*!
    @brief    Creates a new instance of an Adafruit IO Feed.
    @param    *io
              Reference to AdafruitIO.
    @param    *n
              Valid feed name.
*/
/**************************************************************************/
AdafruitIO_Feed::AdafruitIO_Feed(AdafruitIO *io, const char *n)
    : AdafruitIO_MQTT() {
  _io = io;
  name = n;
  owner = _io->_username;

  _init();
}

/**************************************************************************/
/*!
    @brief    Creates a new instance of an Adafruit IO Feed.
    @param    *io
              Reference to AdafruitIO.
    @param    *n
              Valid feed name.
    @param    *un
              Feed owner's Adafruit IO username.
*/
/**************************************************************************/
AdafruitIO_Feed::AdafruitIO_Feed(AdafruitIO *io, const char *n, const char *un)
    : AdafruitIO_MQTT() {
  _io = io;
  name = n;
  owner = un;

  _init();
}

/**************************************************************************/
/*!
    @brief    Adafruit IO Feed destructor.
*/
/**************************************************************************/
AdafruitIO_Feed::~AdafruitIO_Feed() {
  if (_sub)
    delete _sub;

  if (_pub)
    delete _pub;

  if (_get_pub)
    delete _get_pub;

  if (data)
    delete data;

  if (_topic)
    free(_topic);

  if (_get_topic)
    free(_get_topic);

  if (_feed_url)
    free(_feed_url);

  if (_create_url)
    free(_create_url);
}

/**************************************************************************/
/*!
    @brief    Creates a new onMessage callback.
    @param    cb
              Adafruit IO callback.
*/
/**************************************************************************/
void AdafruitIO_Feed::onMessage(AdafruitIODataCallbackType cb) {
  _dataCallback = cb;
}





	//---------------------------------------------------------------------------
	// ATTN: jr test
  bool AdafruitIO_Feed::jrsaveRaw(char *value) {
  	//data->setValue(value, lat, lon, ele);
  	return _pub->publish(value);
  }
  //---------------------------------------------------------------------------





/**************************************************************************/
/*!
    @brief    Updates Adafruit IO Feed.
    @param    value
              Value to publish to feed.
    @param    lat
              Latitude metadata.
    @param    lon
              Longitudinal metadata.
    @param    ele
              Elevation metadata.
    @return   True if data successfully published to feed, False otherwise.
*/
/**************************************************************************/
bool AdafruitIO_Feed::save(char *value, double lat, double lon, double ele) {
  data->setValue(value, lat, lon, ele);
  return _pub->publish(data->toCSV());
}

/**************************************************************************/
/*!
    @brief    Updates Adafruit IO Feed.
    @param    value
              Value to publish to feed.
    @param    lat
              Latitude metadata.
    @param    lon
              Longitudinal metadata.
    @param    ele
              Elevation metadata.
    @return   True if data successfully published to feed, False otherwise.
*/
/**************************************************************************/
bool AdafruitIO_Feed::save(bool value, double lat, double lon, double ele) {
  data->setValue(value, lat, lon, ele);
  return _pub->publish(data->toCSV());
}

/**************************************************************************/
/*!
    @brief    Updates Adafruit IO Feed.
    @param    value
              Value to publish to feed.
    @param    lat
              Latitude metadata.
    @param    lon
              Longitudinal metadata.
    @param    ele
              Elevation metadata.
    @return   True if data successfully published to feed, False otherwise.
*/
/**************************************************************************/
bool AdafruitIO_Feed::save(String value, double lat, double lon, double ele) {
  data->setValue(value, lat, lon, ele);
  return _pub->publish(data->toCSV());
}

/**************************************************************************/
/*!
    @brief    Updates Adafruit IO Feed.
    @param    value
              Value to publish to feed.
    @param    lat
              Latitude metadata.
    @param    lon
              Longitudinal metadata.
    @param    ele
              Elevation metadata.
    @return   True if data successfully published to feed, False otherwise.
*/
/**************************************************************************/
bool AdafruitIO_Feed::save(int value, double lat, double lon, double ele) {
  data->setValue(value, lat, lon, ele);
  return _pub->publish(data->toCSV());
}

/**************************************************************************/
/*!
    @brief    Updates Adafruit IO Feed.
    @param    value
              Value to publish to feed.
    @param    lat
              Latitude metadata.
    @param    lon
              Longitudinal metadata.
    @param    ele
              Elevation metadata.
    @return   True if data successfully published to feed, False otherwise.
*/
/**************************************************************************/
bool AdafruitIO_Feed::save(unsigned int value, double lat, double lon,
                           double ele) {
  data->setValue(value, lat, lon, ele);
  return _pub->publish(data->toCSV());
}

/**************************************************************************/
/*!
    @brief    Updates Adafruit IO Feed.
    @param    value
              Value to publish to feed.
    @param    lat
              Latitude metadata.
    @param    lon
              Longitudinal metadata.
    @param    ele
              Elevation metadata.
    @return   True if data successfully published to feed, False otherwise.
*/
/**************************************************************************/
bool AdafruitIO_Feed::save(long value, double lat, double lon, double ele) {
  data->setValue(value, lat, lon, ele);
  return _pub->publish(data->toCSV());
}

/**************************************************************************/
/*!
    @brief    Updates Adafruit IO Feed.
    @param    value
              Value to publish to feed.
    @param    lat
              Latitude metadata.
    @param    lon
              Longitudinal metadata.
    @param    ele
              Elevation metadata.
    @return   True if data successfully published to feed, False otherwise.
*/
/**************************************************************************/
bool AdafruitIO_Feed::save(unsigned long value, double lat, double lon,
                           double ele) {
  data->setValue(value, lat, lon, ele);
  return _pub->publish(data->toCSV());
}

/**************************************************************************/
/*!
    @brief    Updates Adafruit IO Feed.
    @param    value
              Value to publish to feed.
    @param    lat
              Latitude metadata.
    @param    lon
              Longitudinal metadata.
    @param    ele
              Elevation metadata.
    @param    precision
              Desired amount of decimal precision.
    @return   True if data successfully published to feed, False otherwise.
*/
/**************************************************************************/
bool AdafruitIO_Feed::save(float value, double lat, double lon, double ele,
                           int precision) {
  data->setValue(value, lat, lon, ele, precision);
  return _pub->publish(data->toCSV());
}

/**************************************************************************/
/*!
    @brief    Updates Adafruit IO Feed.
    @param    value
              Value to publish to feed.
    @param    lat
              Latitude metadata.
    @param    lon
              Longitudinal metadata.
    @param    ele
              Elevation metadata.
    @param    precision
              Desired amount of decimal precision.
    @return   True if data successfully published to feed, False otherwise.
*/
/**************************************************************************/
bool AdafruitIO_Feed::save(double value, double lat, double lon, double ele,
                           int precision) {
  data->setValue(value, lat, lon, ele, precision);
  return _pub->publish(data->toCSV());
}

/****************************************************************************/
/*!
    @brief    Publishes a null character to an Adafruit IO /get topic.
              https://io.adafruit.com/api/docs/mqtt.html#using-the-get-topic
    @return   True if successful, otherwise False.
*/
/****************************************************************************/
bool AdafruitIO_Feed::get() { return _get_pub->publish("\0"); }

/****************************************************************************/
/*!
    @brief    Checks if Adafruit IO Feed exists and belongs to username.
              https://io.adafruit.com/api/docs/#get-feed
    @return   True if successful, otherwise False.
*/
/****************************************************************************/
bool AdafruitIO_Feed::exists() {
  _io->_http->beginRequest();
  _io->_http->get(_feed_url);
  _io->_http->sendHeader("X-AIO-Key", _io->_key);
  _io->_http->endRequest();

  int status = _io->_http->responseStatusCode();
  _io->_http->responseBody(); // needs to be read even if not used

  return status == 200;
}

/**************************************************************/
/*!
    @brief    Creates a new Adafruit IO Feed
              https://io.adafruit.com/api/docs/#create-feed
    @return   True if successful, otherwise False.
*/
/*************************************************************/
bool AdafruitIO_Feed::create() {
  String body = "name=";
  body += name;

  _io->_http->beginRequest();
  _io->_http->post(_create_url);

  _io->_http->sendHeader("Content-Type", "application/x-www-form-urlencoded");
  _io->_http->sendHeader("Content-Length", body.length());
  _io->_http->sendHeader("X-AIO-Key", _io->_key);

  // the following call to endRequest
  // should be replaced by beginBody once the
  // Arduino HTTP Client Library is updated
  // _io->_http->beginBody();
  _io->_http->endRequest();

  _io->_http->print(body);
  _io->_http->endRequest();

  int status = _io->_http->responseStatusCode();
  _io->_http->responseBody(); // needs to be read even if not used

  return status == 201;
}

/*****************************************************************************/
/*!
    @brief    Retrieves the most recent value published to a feed.
              https://io.adafruit.com/api/docs/mqtt.html#using-the-get-topic
    @return   NULL
*/
/*****************************************************************************/
AdafruitIO_Data *AdafruitIO_Feed::lastValue() {
  // 15 extra for api path, 12 for /data/retain, 1 for null
  String url = "/api/v2/";
  url += owner;
  url += "/feeds/";
  url += name;
  url += "/data/retain";

  AIO_DEBUG_PRINT("lastValue get ");
  AIO_DEBUG_PRINTLN(url);

  _io->_http->beginRequest();
  _io->_http->get(url.c_str());
  _io->_http->sendHeader("X-AIO-Key", _io->_key);
  _io->_http->endRequest();

  int status = _io->_http->responseStatusCode();
  String body = _io->_http->responseBody();

  if (status >= 200 && status <= 299) {

    if (body.length() > 0) {
      return new AdafruitIO_Data(this, body.c_str());
    }

    return NULL;

  } else {

    AIO_ERROR_PRINT("error retrieving lastValue, status: ");
    AIO_ERROR_PRINTLN(status);
    AIO_ERROR_PRINT("response body: ");
    AIO_ERROR_PRINTLN(_io->_http->responseBody());

    return NULL;
  }
}

/**************************************************************************/
/*!
    @brief    Sets Adafruit IO feed location metadata.
    @param    lat
              Latitude metadata.
    @param    lon
              Longitudinal metadata.
    @param    ele
              Elevation metadata.
*/
/**************************************************************************/
void AdafruitIO_Feed::setLocation(double lat, double lon, double ele) {
  data->setLocation(lat, lon, ele);
}

/**************************************************************************/
/*!
    @brief    Calls _datacallback if new data is avaliable on feed.
    @param    val
              Value to publish to Adafruit IO feed.
    @param    len
              Feed length.
*/
/**************************************************************************/
void AdafruitIO_Feed::subCallback(char *val, uint16_t len) {
  data->setCSV(val);

  // call callback with data
  if (_dataCallback)
    _dataCallback(data);
}

/**************************************************************************/
/*!
    @brief    Initialize MQTT topics and REST URLs for Adafruit IO feeds.
*/
/**************************************************************************/
void AdafruitIO_Feed::_init() {
  _sub = 0;
  _pub = 0;
  _get_pub = 0;
  _dataCallback = 0;

  // dynamically allocate memory for mqtt topic and REST URLs
  _topic = (char *)malloc(
      sizeof(char) * (strlen(owner) + strlen(name) +
                      8)); // 8 extra chars for /f/, /csv & null termination
  _get_topic = (char *)malloc(
      sizeof(char) *
      (strlen(owner) + strlen(name) +
       12)); // 12 extra chars for /f/, /csv/get & null termination
  _feed_url =
      (char *)malloc(sizeof(char) * (strlen(owner) + strlen(name) +
                                     16)); // 16 extra for api path & null term
  _create_url = (char *)malloc(
      sizeof(char) * (strlen(owner) + 15)); // 15 extra for api path & null term

  // init feed data
  data = new AdafruitIO_Data(this);

  if (_topic && _create_url && _feed_url) {

    // build topic string
    strcpy(_topic, owner);
    strcat(_topic, "/f/");
    strcat(_topic, name);
    strcat(_topic, "/csv");

    // build feed url string
    strcpy(_feed_url, "/api/v2/");
    strcat(_feed_url, owner);
    strcat(_feed_url, "/feeds/");
    strcat(_feed_url, name);

    // build create url string
    strcpy(_create_url, "/api/v2/");
    strcat(_create_url, owner);
    strcat(_create_url, "/feeds");

    // build /get topic string
    strcpy(_get_topic, owner);
    strcat(_get_topic, "/f/");
    strcat(_get_topic, name);
    strcat(_get_topic, "/csv/get");

    // setup subscription
    _sub = new Adafruit_MQTT_Subscribe(_io->_mqtt, _topic);
    _pub = new Adafruit_MQTT_Publish(_io->_mqtt, _topic);
    _get_pub = new Adafruit_MQTT_Publish(_io->_mqtt, _get_topic);
    _io->_mqtt->subscribe(_sub);

    _sub->setCallback(this, &AdafruitIO_MQTT::subCallback);

  } else {

    // malloc failed
    _topic = 0;
    _get_topic = 0;
    _create_url = 0;
    _feed_url = 0;
    _sub = 0;
    _pub = 0;
    _get_pub = 0;
    data = 0;
  }
}
