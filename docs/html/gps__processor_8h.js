var gps__processor_8h =
[
    [ "GnssMeasurement", "structGnssMeasurement.html", "structGnssMeasurement" ],
    [ "GnssClock", "structGnssClock.html", "structGnssClock" ],
    [ "AnboxGGAData", "structAnboxGGAData.html", "structAnboxGGAData" ],
    [ "AnboxRMCData", "structAnboxRMCData.html", "structAnboxRMCData" ],
    [ "AnboxGnssData", "structAnboxGnssData.html", "structAnboxGnssData" ],
    [ "AnboxGpsData", "structAnboxGpsData.html", "structAnboxGpsData" ],
    [ "GpsProcessor", "classanbox_1_1GpsProcessor.html", "classanbox_1_1GpsProcessor" ],
    [ "GNSS_MAX_MEASUREMENT", "gps__processor_8h.html#ab0b354a57479f2d93eaaeb36b37adb34", null ],
    [ "GpsUtcTime", "gps__processor_8h.html#af2b0ea531a44c010f81a4abd27504c15", null ],
    [ "AnboxGpsDataType", "gps__processor_8h.html#a79eae436a01c972a1364599301b367aa", [
      [ "Unknown", "gps__processor_8h.html#a79eae436a01c972a1364599301b367aaa4e81c184ac3ad48a389cd4454c4a05bb", null ],
      [ "GGA", "gps__processor_8h.html#a79eae436a01c972a1364599301b367aaad09474c94f2854023aa8da0c20848b7a", null ],
      [ "RMC", "gps__processor_8h.html#a79eae436a01c972a1364599301b367aaa37404ce986f9a62c55b87b12fe39b532", null ],
      [ "GNSSv1", "gps__processor_8h.html#a79eae436a01c972a1364599301b367aaa87bb72bb4f17e077a2ad3d57f1bc10fe", null ]
    ] ],
    [ "GnssClockFlags", "gps__processor_8h.html#a023d389886e7c26aa3fd3d1add1669f8", [
      [ "HAS_LEAP_SECOND", "gps__processor_8h.html#a023d389886e7c26aa3fd3d1add1669f8a0836ae08906baf63eb489220ea3f3566", null ],
      [ "HAS_TIME_UNCERTAINTY", "gps__processor_8h.html#a023d389886e7c26aa3fd3d1add1669f8af1a2e192c7526b9a4c96094944007264", null ],
      [ "HAS_FULL_BIAS", "gps__processor_8h.html#a023d389886e7c26aa3fd3d1add1669f8a6c4190a8c3d7d9bceebe65597dc3e45f", null ],
      [ "HAS_BIAS", "gps__processor_8h.html#a023d389886e7c26aa3fd3d1add1669f8a274db7ea6020340f7fa760dde09c227e", null ],
      [ "HAS_BIAS_UNCERTAINTY", "gps__processor_8h.html#a023d389886e7c26aa3fd3d1add1669f8ac376cdccbe0d18412f671a30aa742b0d", null ],
      [ "HAS_DRIFT", "gps__processor_8h.html#a023d389886e7c26aa3fd3d1add1669f8a3dc024c93a1114fd4047cb60c9591890", null ],
      [ "HAS_DRIFT_UNCERTAINTY", "gps__processor_8h.html#a023d389886e7c26aa3fd3d1add1669f8a7d3b3a14b63313e9baa83dd98fb37794", null ]
    ] ],
    [ "GnssConstellationType", "gps__processor_8h.html#a4a78e8148254ece4b4c92a4d3816f680", [
      [ "UNKNOWN", "gps__processor_8h.html#a4a78e8148254ece4b4c92a4d3816f680a6ce26a62afab55d7606ad4e92428b30c", null ],
      [ "GPS", "gps__processor_8h.html#a4a78e8148254ece4b4c92a4d3816f680a171ede81b49deebf3e342cdf41c62182", null ],
      [ "SBAS", "gps__processor_8h.html#a4a78e8148254ece4b4c92a4d3816f680a47cc80f30e4ee0408b2b5ab8d51e579b", null ],
      [ "GLONASS", "gps__processor_8h.html#a4a78e8148254ece4b4c92a4d3816f680a7d23de99080060f3d7d481c5ee0dddb7", null ],
      [ "QZSS", "gps__processor_8h.html#a4a78e8148254ece4b4c92a4d3816f680a263bcf05bc26af8494982eb5d86fc7f1", null ],
      [ "BEIDOU", "gps__processor_8h.html#a4a78e8148254ece4b4c92a4d3816f680acca02fbf265438c90c5ab4a9bbd35096", null ],
      [ "GALILEO", "gps__processor_8h.html#a4a78e8148254ece4b4c92a4d3816f680af6d041d14f6a7e8a76ae3f0eae6b9814", null ]
    ] ],
    [ "GnssMeasurementState", "gps__processor_8h.html#aa479d42a9b58eeeae0cf06b90bc7ba08", [
      [ "STATE_UNKNOWN", "gps__processor_8h.html#aa479d42a9b58eeeae0cf06b90bc7ba08a786a8e9401b8091f65b2afbd342c90f5", null ],
      [ "STATE_CODE_LOCK", "gps__processor_8h.html#aa479d42a9b58eeeae0cf06b90bc7ba08a0451f3f2992fcffddcf53174b0f5692e", null ],
      [ "STATE_BIT_SYNC", "gps__processor_8h.html#aa479d42a9b58eeeae0cf06b90bc7ba08a9b204121a072093aaadc3e628923632b", null ],
      [ "STATE_SUBFRAME_SYNC", "gps__processor_8h.html#aa479d42a9b58eeeae0cf06b90bc7ba08a92fc94e26b846dd80d7e0cfc937790ec", null ],
      [ "STATE_TOW_DECODED", "gps__processor_8h.html#aa479d42a9b58eeeae0cf06b90bc7ba08ad5791d68db83cab03ec5975be001ccc2", null ],
      [ "STATE_MSEC_AMBIGUOUS", "gps__processor_8h.html#aa479d42a9b58eeeae0cf06b90bc7ba08a272dfc292541c4422f6d4105a70a1679", null ],
      [ "STATE_SYMBOL_SYNC", "gps__processor_8h.html#aa479d42a9b58eeeae0cf06b90bc7ba08a3b9cdbe2b394309dedf2d7d13ba72485", null ],
      [ "STATE_GLO_STRING_SYNC", "gps__processor_8h.html#aa479d42a9b58eeeae0cf06b90bc7ba08a6723f2dd08082ff551b0c12bd7c94390", null ],
      [ "STATE_GLO_TOD_DECODED", "gps__processor_8h.html#aa479d42a9b58eeeae0cf06b90bc7ba08a5a85e1f7e3941110fa4142140beb409d", null ],
      [ "STATE_BDS_D2_BIT_SYNC", "gps__processor_8h.html#aa479d42a9b58eeeae0cf06b90bc7ba08ae61cce57eb4a42c393cc2db35a63257d", null ],
      [ "STATE_BDS_D2_SUBFRAME_SYNC", "gps__processor_8h.html#aa479d42a9b58eeeae0cf06b90bc7ba08ac397b170b4df2d232c0eb693e5129834", null ],
      [ "STATE_GAL_E1BC_CODE_LOCK", "gps__processor_8h.html#aa479d42a9b58eeeae0cf06b90bc7ba08a5b21b4e649c6e5856dca88f475d4ec71", null ],
      [ "STATE_GAL_E1C_2ND_CODE_LOCK", "gps__processor_8h.html#aa479d42a9b58eeeae0cf06b90bc7ba08acb6a6e5a7931781cde3a0cd677db2858", null ],
      [ "STATE_GAL_E1B_PAGE_SYNC", "gps__processor_8h.html#aa479d42a9b58eeeae0cf06b90bc7ba08aadd04811108c3f0fd742a11a2b133bad", null ],
      [ "STATE_SBAS_SYNC", "gps__processor_8h.html#aa479d42a9b58eeeae0cf06b90bc7ba08a733b62517782da1214077979f611d150", null ],
      [ "STATE_TOW_KNOWN", "gps__processor_8h.html#aa479d42a9b58eeeae0cf06b90bc7ba08a2e12fdb7fc981fe77630fbea32eeccd7", null ],
      [ "STATE_GLO_TOD_KNOWN", "gps__processor_8h.html#aa479d42a9b58eeeae0cf06b90bc7ba08a363bd6e65ff817b80413c402bf2d2c8e", null ]
    ] ]
];