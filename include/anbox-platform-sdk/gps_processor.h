/*
 * This file is part of Anbox Platform SDK
 *
 * Copyright 2021 Canonical Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANBOX_SDK_GPS_PROCESSOR_H_
#define ANBOX_SDK_GPS_PROCESSOR_H_

#include <stdint.h>
#include <stddef.h>
#include <cstdint>

#define GNSS_MAX_MEASUREMENT 64

/** Milliseconds since January 1, 1970 */
typedef int64_t GpsUtcTime;

/**
 * Constellation type of GnssSvInfo
 */
typedef enum : uint8_t {
  UNKNOWN = 0,
  GPS     = 1,
  SBAS    = 2,
  GLONASS = 3,
  QZSS    = 4,
  BEIDOU  = 5,
  GALILEO = 6,
} GnssConstellationType;

/**
 * Flags indicating the GNSS measurement state.
 *
 * The expected behavior here is for GPS HAL to set all the flags that applies.
 * For example, if the state for a satellite is only C/A code locked and bit
 * synchronized, and there is still millisecond ambiguity, the state should be
 * set as:
 *
 * GNSS_MEASUREMENT_STATE_CODE_LOCK | GNSS_MEASUREMENT_STATE_BIT_SYNC |
 *         GNSS_MEASUREMENT_STATE_MSEC_AMBIGUOUS
 *
 * If GNSS is still searching for a satellite, the corresponding state should be
 * set to GNSS_MEASUREMENT_STATE_UNKNOWN(0).
 */
typedef enum : uint32_t {
  STATE_UNKNOWN                = 0,
  STATE_CODE_LOCK              = 1 << 0,
  STATE_BIT_SYNC               = 1 << 1,
  STATE_SUBFRAME_SYNC          = 1 << 2,
  STATE_TOW_DECODED            = 1 << 3,
  STATE_MSEC_AMBIGUOUS         = 1 << 4,
  STATE_SYMBOL_SYNC            = 1 << 5,
  STATE_GLO_STRING_SYNC        = 1 << 6,
  STATE_GLO_TOD_DECODED        = 1 << 7,
  STATE_BDS_D2_BIT_SYNC        = 1 << 8,
  STATE_BDS_D2_SUBFRAME_SYNC   = 1 << 9,
  STATE_GAL_E1BC_CODE_LOCK     = 1 << 10,
  STATE_GAL_E1C_2ND_CODE_LOCK  = 1 << 11,
  STATE_GAL_E1B_PAGE_SYNC      = 1 << 12,
  STATE_SBAS_SYNC              = 1 << 13,
  STATE_TOW_KNOWN              = 1 << 14,
  STATE_GLO_TOD_KNOWN          = 1 << 15,
} GnssMeasurementState;

/**
 * Flags to indicate what fields in GnssClock are valid.
 */
typedef enum : std::uint16_t {
  /** A valid 'leap second' is stored in the data structure. */
  HAS_LEAP_SECOND        = 1 << 0,
  /** A valid 'time uncertainty' is stored in the data structure. */
  HAS_TIME_UNCERTAINTY   = 1 << 1,
  /** A valid 'full bias' is stored in the data structure. */
  HAS_FULL_BIAS          = 1 << 2,
  /** A valid 'bias' is stored in the data structure. */
  HAS_BIAS               = 1 << 3,
  /** A valid 'bias uncertainty' is stored in the data structure. */
  HAS_BIAS_UNCERTAINTY   = 1 << 4,
  /** A valid 'drift' is stored in the data structure. */
  HAS_DRIFT              = 1 << 5,
  /** A valid 'drift uncertainty' is stored in the data structure. */
  HAS_DRIFT_UNCERTAINTY  = 1 << 6
} GnssClockFlags;

/**
 * Represents a GNSS Measurement, it contains raw and computed information.
 *
 * Independence - All signal measurement information (e.g. sv_time,
 * pseudorange_rate, multipath_indicator) reported in this struct should be
 * based on GNSS signal measurements only. You may not synthesize measurements
 * by calculating or reporting expected measurements based on known or estimated
 * position, velocity, or time.
 */
struct GnssMeasurement {
  /**
   * Satellite vehicle ID number, as defined in GnssSvInfo::svid
   * This is a mandatory value.
   */
  int16_t svid;

  /**
   * Defines the constellation of the given SV. Value should be one of those
   * GNSS_CONSTELLATION_* constants
   */
  GnssConstellationType constellation;

  /**
   * Per satellite sync state. It represents the current sync state for the associated satellite.
   * Based on the sync state, the 'received GPS tow' field should be interpreted accordingly.
   *
   * This is a mandatory value.
   */
  GnssMeasurementState state;

  /**
   * The received GNSS Time-of-Week at the measurement time, in nanoseconds.
   * Ensure that this field is independent (see comment at top of
   * GnssMeasurement struct.)
   *
   * For GPS & QZSS, this is:
   *   Received GPS Time-of-Week at the measurement time, in nanoseconds.
   *   The value is relative to the beginning of the current GPS week.
   *
   *   Given the highest sync state that can be achieved, per each satellite, valid range
   *   for this field can be:
   *     Searching       : [ 0       ]   : GNSS_MEASUREMENT_STATE_UNKNOWN
   *     C/A code lock   : [ 0   1ms ]   : GNSS_MEASUREMENT_STATE_CODE_LOCK is set
   *     Bit sync        : [ 0  20ms ]   : GNSS_MEASUREMENT_STATE_BIT_SYNC is set
   *     Subframe sync   : [ 0    6s ]   : GNSS_MEASUREMENT_STATE_SUBFRAME_SYNC is set
   *     TOW decoded     : [ 0 1week ]   : GNSS_MEASUREMENT_STATE_TOW_DECODED is set
   *
   *   Note well: if there is any ambiguity in integer millisecond,
   *   GNSS_MEASUREMENT_STATE_MSEC_AMBIGUOUS should be set accordingly, in the 'state' field.
   *
   *   This value must be populated if 'state' != GNSS_MEASUREMENT_STATE_UNKNOWN.
   *
   * For Glonass, this is:
   *   Received Glonass time of day, at the measurement time in nanoseconds.
   *
   *   Given the highest sync state that can be achieved, per each satellite, valid range for
   *   this field can be:
   *     Searching       : [ 0       ]   : GNSS_MEASUREMENT_STATE_UNKNOWN
   *     C/A code lock   : [ 0   1ms ]   : GNSS_MEASUREMENT_STATE_CODE_LOCK is set
   *     Symbol sync     : [ 0  10ms ]   : GNSS_MEASUREMENT_STATE_SYMBOL_SYNC is set
   *     Bit sync        : [ 0  20ms ]   : GNSS_MEASUREMENT_STATE_BIT_SYNC is set
   *     String sync     : [ 0    2s ]   : GNSS_MEASUREMENT_STATE_GLO_STRING_SYNC is set
   *     Time of day     : [ 0  1day ]   : GNSS_MEASUREMENT_STATE_GLO_TOD_DECODED is set
   *
   * For Beidou, this is:
   *   Received Beidou time of week, at the measurement time in nanoseconds.
   *
   *   Given the highest sync state that can be achieved, per each satellite, valid range for
   *   this field can be:
   *     Searching    : [ 0       ] : GNSS_MEASUREMENT_STATE_UNKNOWN
   *     C/A code lock: [ 0   1ms ] : GNSS_MEASUREMENT_STATE_CODE_LOCK is set
   *     Bit sync (D2): [ 0   2ms ] : GNSS_MEASUREMENT_STATE_BDS_D2_BIT_SYNC is set
   *     Bit sync (D1): [ 0  20ms ] : GNSS_MEASUREMENT_STATE_BIT_SYNC is set
   *     Subframe (D2): [ 0  0.6s ] : GNSS_MEASUREMENT_STATE_BDS_D2_SUBFRAME_SYNC is set
   *     Subframe (D1): [ 0    6s ] : GNSS_MEASUREMENT_STATE_SUBFRAME_SYNC is set
   *     Time of week : [ 0 1week ] : GNSS_MEASUREMENT_STATE_TOW_DECODED is set
   *
   * For Galileo, this is:
   *   Received Galileo time of week, at the measurement time in nanoseconds.
   *
   *     E1BC code lock   : [ 0   4ms ]   : GNSS_MEASUREMENT_STATE_GAL_E1BC_CODE_LOCK is set
   *     E1C 2nd code lock: [ 0 100ms ]   :
   *     GNSS_MEASUREMENT_STATE_GAL_E1C_2ND_CODE_LOCK is set
   *
   *     E1B page    : [ 0    2s ] : GNSS_MEASUREMENT_STATE_GAL_E1B_PAGE_SYNC is set
   *     Time of week: [ 0 1week ] : GNSS_MEASUREMENT_STATE_TOW_DECODED is set
   *
   * For SBAS, this is:
   *   Received SBAS time, at the measurement time in nanoseconds.
   *
   *   Given the highest sync state that can be achieved, per each satellite,
   *   valid range for this field can be:
   *     Searching    : [ 0     ] : GNSS_MEASUREMENT_STATE_UNKNOWN
   *     C/A code lock: [ 0 1ms ] : GNSS_MEASUREMENT_STATE_CODE_LOCK is set
   *     Symbol sync  : [ 0 2ms ] : GNSS_MEASUREMENT_STATE_SYMBOL_SYNC is set
   *     Message      : [ 0  1s ] : GNSS_MEASUREMENT_STATE_SBAS_SYNC is set
   */
  int64_t received_sv_time_in_ns;

  /**
   * 1-Sigma uncertainty of the Received GPS Time-of-Week in nanoseconds.
   *
   * This value must be populated if 'state' != GPS_MEASUREMENT_STATE_UNKNOWN.
   */
  int64_t received_sv_time_uncertainty_in_ns;

  /**
   * Carrier-to-noise density in dB-Hz, typically in the range [0, 63].
   * It contains the measured C/N0 value for the signal at the antenna port.
   *
   * This is a mandatory value.
   */
  double c_n0_dbhz;

  /**
   * Pseudorange rate at the timestamp in m/s. The correction of a given
   * Pseudorange Rate value includes corrections for receiver and satellite
   * clock frequency errors. Ensure that this field is independent (see
   * comment at top of GnssMeasurement struct.)
   *
   * It is mandatory to provide the 'uncorrected' 'pseudorange rate', and provide GpsClock's
   * 'drift' field as well (When providing the uncorrected pseudorange rate, do not apply the
   * corrections described above.)
   *
   * The value includes the 'pseudorange rate uncertainty' in it.
   * A positive 'uncorrected' value indicates that the SV is moving away from the receiver.
   *
   * The sign of the 'uncorrected' 'pseudorange rate' and its relation to the sign of 'doppler
   * shift' is given by the equation:
   *      pseudorange rate = -k * doppler shift   (where k is a constant)
   *
   * This should be the most accurate pseudorange rate available, based on
   * fresh signal measurements from this channel.
   *
   * It is mandatory that this value be provided at typical carrier phase PRR
   * quality (few cm/sec per second of uncertainty, or better) - when signals
   * are sufficiently strong & stable, e.g. signals from a GPS simulator at >=
   * 35 dB-Hz.
   */
  double pseudorange_rate_mps;

  /**
   * 1-Sigma uncertainty of the pseudorange_rate_mps.
   * The uncertainty is represented as an absolute (single sided) value.
   *
   * This is a mandatory value.
   */
  double pseudorange_rate_uncertainty_mps;

  /**
   * Carrier frequency at which codes and messages are modulated, it can be L1 or L2.
   * If the field is not set, the carrier frequency is assumed to be L1.
   *
   * If the data is available, 'flags' must contain
   * GNSS_MEASUREMENT_HAS_CARRIER_FREQUENCY.
   */
  float carrier_frequency_hz;
};

/**
 * Represents an estimate of the GPS clock time.
 */
struct GnssClock {
  /**
   * The GNSS receiver internal clock value. This is the local hardware clock
   * value.
   *
   * For local hardware clock, this value is expected to be monotonically
   * increasing while the hardware clock remains power on. (For the case of a
   * HW clock that is not continuously on, see the
   * hw_clock_discontinuity_count field). The receiver's estimate of GPS time
   * can be derived by substracting the sum of full_bias_ns and bias_ns (when
   * available) from this value.
   *
   * This GPS time is expected to be the best estimate of current GPS time
   * that GNSS receiver can achieve.
   *
   * Sub-nanosecond accuracy can be provided by means of the 'bias_ns' field.
   * The value contains the 'time uncertainty' in it.
   *
   * This field is mandatory.
   */
  int64_t time_ns;

  /**
   * The difference between hardware clock ('time' field) inside GPS receiver
   * and the true GPS time since 0000Z, January 6, 1980, in nanoseconds.
   *
   * The sign of the value is defined by the following equation:
   *      local estimate of GPS time = time_ns - (full_bias_ns + bias_ns)
   *
   * This value is mandatory if the receiver has estimated GPS time. If the
   * computed time is for a non-GPS constellation, the time offset of that
   * constellation to GPS has to be applied to fill this value. The error
   * estimate for the sum of this and the bias_ns is the bias_uncertainty_ns,
   * and the caller is responsible for using this uncertainty (it can be very
   * large before the GPS time has been solved for.) If the data is available
   * 'flags' must contain GNSS_CLOCK_HAS_FULL_BIAS.
   */
  int64_t full_bias_ns;

  /**
   * Sub-nanosecond bias.
   * The error estimate for the sum of this and the full_bias_ns is the
   * bias_uncertainty_ns
   *
   * If the data is available 'flags' must contain GNSS_CLOCK_HAS_BIAS. If GPS
   * has computed a position fix. This value is mandatory if the receiver has
   * estimated GPS time.
   */
  double bias_ns;

  /**
   * 1-Sigma uncertainty associated with the local estimate of GPS time (clock
   * bias) in nanoseconds. The uncertainty is represented as an absolute
   * (single sided) value.
   *
   * If the data is available 'flags' must contain
   * GNSS_CLOCK_HAS_BIAS_UNCERTAINTY. This value is mandatory if the receiver
   * has estimated GPS time.
   */
  double bias_uncertainty_ns;

  /**
   * The clock's drift in nanoseconds (per second).
   *
   * A positive value means that the frequency is higher than the nominal
   * frequency, and that the (full_bias_ns + bias_ns) is growing more positive
   * over time.
   *
   * The value contains the 'drift uncertainty' in it.
   * If the data is available 'flags' must contain GNSS_CLOCK_HAS_DRIFT.
   *
   * This value is mandatory if the receiver has estimated GNSS time
   */
  double drift_nsps;

  /**
   * 1-Sigma uncertainty associated with the clock's drift in nanoseconds (per second).
   * The uncertainty is represented as an absolute (single sided) value.
   *
   * If the data is available 'flags' must contain
   * GNSS_CLOCK_HAS_DRIFT_UNCERTAINTY. If GPS has computed a position fix this
   * field is mandatory and must be populated.
   */
  double drift_uncertainty_nsps;

  /**
   * When there are any discontinuities in the HW clock, this field is
   * mandatory.
   *
   * A "discontinuity" is meant to cover the case of a switch from one source
   * of clock to another.  A single free-running crystal oscillator (XO)
   * should generally not have any discontinuities, and this can be set and
   * left at 0.
   *
   * If, however, the time_ns value (HW clock) is derived from a composite of
   * sources, that is not as smooth as a typical XO, or is otherwise stopped &
   * restarted, then this value shall be incremented each time a discontinuity
   * occurs.  (E.g. this value may start at zero at device boot-up and
   * increment each time there is a change in clock continuity. In the
   * unlikely event that this value reaches full scale, rollover (not
   * clamping) is required, such that this value continues to change, during
   * subsequent discontinuity events.)
   *
   * While this number stays the same, between GnssClock reports, it can be
   * safely assumed that the time_ns value has been running continuously, e.g.
   * derived from a single, high quality clock (XO like, or better, that's
   * typically used during continuous GNSS signal sampling.)
   *
   * It is expected, esp. during periods where there are few GNSS signals
   * available, that the HW clock be discontinuity-free as long as possible,
   * as this avoids the need to use (waste) a GNSS measurement to fully
   * re-solve for the GPS clock bias and drift, when using the accompanying
   * measurements, from consecutive GnssData reports.
   */
  uint32_t hw_clock_discontinuity_count;

  /**
   * Flags to indicate what fields in GnssClock are valid.
   */
  GnssClockFlags flags;
};
/**
 * AnboxGGAData represents GGA essential fix data which provide 3D location and accuracy data
 */
struct AnboxGGAData {
  /** Represents time for the location fix. */
  GpsUtcTime      time;
  /** Represents latitude in degrees. */
  double          latitude;
  /** Represents latitude in hemisphere. */
  char            latitudeHemi;
  /** Represents longitude in degrees. */
  double          longitude;
  /** Represents longitude in degrees. */
  char            longitudeHemi;
  /** Represents altitude. */
  double          altitude;
  /** Represents altitude unit. */
  char            altitudeUnit;
  /** Represents horizontal accuracy. */
  float           horizontalAccuracy;
  /** Represents vertical accuracy. */
  float           verticalAccuracy;
};

/**
 * AnboxRMCData represents essential gps pvt (position, velocity, time) data.
 */
struct AnboxRMCData {
  /** Represents time for the location fix. */
  GpsUtcTime      time;
  /** Represents gps status */
  char            status;
  /** Represents latitude in degrees. */
  double          latitude;
  /** Represents latitude in hemisphere. */
  char            latitudeHemi;
  /** Represents longitude in degrees. */
  double          longitude;
  /** Represents longitude in degrees. */
  char            longitudeHemi;
  /** Represents speed in meters per second.*/
  float           speed;
  /** Represents heading in degrees. */
  float           bearing;
  /** Represents date for the location fix. */
  GpsUtcTime      date;
  /** Represents horizontal accuracy. */
  float           horizontalAccuracy;
  /** Represents vertical accuracy. */
  float           verticalAccuracy;
};

/**
 * AnboxGnssData represents a reading of GNSS measurements, which includes
 * the GPS constellation measurements received from the GNSS receiver.
 *
 * - Reporting of GPS constellation measurements is mandatory.
 * - Reporting of all tracked constellations are encouraged.
 */
struct AnboxGnssData {
  /** The GPS clock time reading. */
  GnssClock clock;

  /** Number of measurements. */
  size_t measurement_count;

  /** The array of measurements. */
  GnssMeasurement measurements[GNSS_MAX_MEASUREMENT];
};


/**
 * @brief AnboxGpsDataType describes all gps data types supported by Anbox
 */
typedef enum : std::uint32_t {
  /** Unknown GPS data type */
  Unknown = 1 << 0,
  /** GGA represents essential fix data which provide 3D location and accuracy data */
  GGA     = 1 << 1,
  /** RMC represents essential gps pvt (position, velocity, time) data */
  RMC     = 1 << 2,
  /** GNSSv1 represents the version one of a constellation of satellites providing signals
   * information from space that transmit positioning and timing data */
  GNSSv1  = 1 << 3,
} AnboxGpsDataType;

/**
 * AnboxGpsData represents a Gps data that is used to not only hold information taken from
 * GPS sensors and will represent longitude, latitude, altitude and so on, but also contains
 * GNSS measurements data emitted from GNSS satellites to obtains the position information.
 */
struct AnboxGpsData {
  /** Type of the gps data */
  AnboxGpsDataType data_type;
  union {
    AnboxGGAData gga_data;
    AnboxRMCData rmc_data;
    AnboxGnssData  gnss_data;
  };
};

namespace anbox {
/**
 * @brief GpsProcessor allows forwarding the gps data from platform plugin to
 * Android container and process gps data which is in one of the following forms
 *   . GGA NEMA sentence
 *   . RMC NEMA sentence
 *   . GNSS NEMA sentence
 * Please check http://www.gpsinformation.org/dale/nmea.htm out for more details
 * about the above NEMA sentence.
 */
class GpsProcessor {
  public:
    GpsProcessor() = default;
    virtual ~GpsProcessor() = default;
    GpsProcessor(const GpsProcessor &) = delete;
    GpsProcessor& operator=(const GpsProcessor &) = delete;

    /**
     * @brief Read available gps data.
     *
     * Anbox will call read_data() to query the plugin for the next available
     * gps data which is then forwarded to the Android container. If no gps
     * data is available and \a timeout is set to -1, the function must block
     * until gps data is available.
     *
     * @param gps data provided by the processor on a successful read
     * @param timeout maximum number of milliseconds to wait for the next available gps data.
     * The following possible values for \a timeout must be handled:
     * | \a timeout  | Behavior |
     * | ------------- | ------------- |
     * | 0   | Non-blocking mode; return -EIO immediately if no gps data to process. |
     * | < 0 | Block indefinitely until a gps data is available. |
     * | > 0 | Wait up to a maximum of \a timeout milliseconds for a gps data. |
     * @return 0 on success otherwise returns EINVAL on error occurs.
     */
    virtual int read_data(AnboxGpsData* data, int timeout) = 0;

    /**
     * @brief Inject gps data into AnboxPlatform.
     *
     * This function allows injecting gps data into the platform and letting
     * the GpsProcessor handle the gps data.
     *
     * @param data a chunk of gps data to be pushed into the internal queue.
     * @return 0 on success, otherwise returns EINVAL on error occurs.
     * @note This function is only used in our test suite to facilitate our automation
     *       tests and it is subject to change at any time.
     **/
    virtual int inject_data(AnboxGpsData data) = 0;
};
} // namespace anbox

#endif
