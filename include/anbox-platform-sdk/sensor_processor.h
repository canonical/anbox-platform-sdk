// Anbox - The Android in a Box runtime environment
// Copyright 2019 Canonical Ltd.  All rights reserved.

#ifndef ANBOX_SDK_SENSOR_PROCESSOR_H_
#define ANBOX_SDK_SENSOR_PROCESSOR_H_

#include <stdint.h>
#include <stddef.h>
#include <cstdint>

#define MAX_SENSOR_DATA_LENGTH 16
#define MAX_VECTOR_DATA_LENGTH 3

/**
 * @brief AnboxSensorType describes all sensor types supported by Anbox
 */
typedef enum : std::uint32_t {
  /** no sensor support */
  NONE = 0,
  /** 3-axis Accelerometer */
  ACCELERATION  = 1 << 0,
  /** 3-axis Gyroscope */
  GYROSCOPE     = 1 << 1,
  /** 3-axis Magnetic field sensor */
  MAGNETOMETER  = 1 << 2,
  /** Orientation sensor */
  ORIENTATION   = 1 << 3,
  /** Ambient Temperature sensor */
  TEMPERATURE   = 1 << 4,
  /** Proximity sensor */
  PROXIMITY     = 1 << 5,
  /** Light sensor */
  LIGHT         = 1 << 6,
  /** Pressure sensor */
  PRESSURE      = 1 << 7,
  /** Humidity sensor */
  HUMIDITY      = 1 << 8,
} AnboxSensorType;

/**
 * @brief AnboxSensorVector represents acceleration along each device axis
 * or current device rotation angles: azimuth, pitch, roll
 */
struct AnboxSensorVector {
  union {
    /** Underlying data value */
    float v[MAX_VECTOR_DATA_LENGTH];
    struct {
      /** X axis */
      float x;
      /** Y axis */
      float y;
      /** Z axis */
      float z;
    } axis;
    struct {
      /** Rotation angles: azimuth */
      float azimuth;
      /** Rotation angles: pitch */
      float pitch;
      /** Rotation angles: roll */
      float roll;
    } angle;
  };
};

/**
 * @brief AnboxSensorData represents the sensor data stucture,
 */
struct AnboxSensorData {
  /** Type of the sensor */
  AnboxSensorType sensor_type;
  union {
    float  values[MAX_SENSOR_DATA_LENGTH];
    /** Sensor data for 3-axis accelerometer */
    AnboxSensorVector acceleration;
    /** Sensor data for 3-axis gyroscope */
    AnboxSensorVector gyroscope;
    /** Sensor data for orientation */
    AnboxSensorVector orientation;
    /** Sensor data for magnetic field */
    AnboxSensorVector magnetic;
    /** Sensor data for ambient temperature */
    float           temperature;
    /** Sensor data for proximity */
    float           proximity;
    /** Sensor data for light */
    float           light;
    /** Sensor data for pressure */
    float           pressure;
    /** Sensor data for humidity */
    float           humidity;
  };
};

namespace anbox {
/**
 * @brief SensorProcessor allows processing sensor events from the Android container
 * and perform sensor processing for a variety of sensor devices.
 */
class SensorProcessor {
  public:
    SensorProcessor() = default;
    virtual ~SensorProcessor() = default;
    SensorProcessor(const SensorProcessor &) = delete;
    SensorProcessor& operator=(const SensorProcessor &) = delete;

    /**
     * @brief Sensors supported by this processor
     *
     * Internally Anbox will tell Android about the supported sensors and 
     * ensure they are being exposed through the right Android APIs.
     * Multiple sensor types can be specified by using OR bitwise operator.
     *
     * @return sensors that are supported by this processor
     */
    virtual AnboxSensorType supported_sensors() const = 0;

    /**
     * @brief Read available sensor data.
     *
     * Anbox will call read_data() to query the plugin for the next available
     * sensor data which is then forwarded to the Android container. If no sensor
     * data is available and \a timeout is set to -1, the function must block until
     * sensor data is available.
     *
     * @param sensor data provided by the processor on a successful read
     * @param timeout maximum number of milliseconds to wait for the next available sensor data.
     * The following possible values for \a timeout must be handled:
     * | \a timeout  | Behavior |
     * | ------------- | ------------- |
     * | 0   | Non-blocking mode; return -EIO immediately if no sensor data to process. |
     * | < 0 | Block indefinitely until a sensor data is available. |
     * | > 0 | Wait up to a maximum of \a timeout milliseconds for a sensor data. |
     * @return 0 on success otherwise returns EINVAL on error occurs.
     */
    virtual int read_data(AnboxSensorData* data, int timeout) = 0;

    /**
     * @brief Inject sensor data into AnboxPlatform.
     *
     * This function allows injecting sensor data into the platform and letting
     * the SensorProcessor handle the sensor data.
     *
     * @param data a chunk of sensor data to be pushed into the internal queue.
     * @return 0 on success, otherwise returns EINVAL on error occurs.
     * @note This function is only used in our test suite to facilitate our automation
     *       tests and it is subject to change at any time.
     **/
    virtual int inject_data(AnboxSensorData data) = 0;
};
} // namespace anbox

#endif
