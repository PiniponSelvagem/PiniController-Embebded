#pragma once

// Select on of the LOG levels from 'log.hpp', THIS MUST BE DEFINED BEFORE INCLUDE OF LOG.HPP
#ifdef DEBUG_ALL
    //#define LOG_LEVEL LOG_LEVEL_DEBUG
    #define LOG_LEVEL LOG_LEVEL_TRACE
#else
    #define LOG_LEVEL LOG_LEVEL_INFO
#endif
#define LOG_COLORED
#include "utils/log.hpp"

#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <esp_attr.h>
#include <Wire.h>
#include <TimeLib.h>
#include <time.h>

#include "architecture.hpp"
#include "utils/utils.hpp"

/* Localization used in WiFi AP page */
#define LANGUAGE_SELECTION__PT_PT
//#define LANGUAGE_SELECTION__EN_US

#define RB_SECTORS  9990    // Only used in the defines below and must be unique
#define RB_PIVOT    9991    // Only used in the defines below and must be unique

/* Device specific configuration */
#ifdef SECTORS
    /**
     * Firmware information:
     * - 110: Major rewrite
     */
    #define FIRMWARE_VERSION 110
    #define DEVICE_TYPE RB_SECTORS
    #define TYPE_PREFIX "r_e_"
    #define AP_FIRMWARE_INFORMATION "REVOBOT_SECTORS"
    #define WDTG_INTERNAL_TIMER_IN_SECONDS   90  // GSM connection start can lock the main thread for up to 75 seconds
    /***************/
#elif defined(PIVOT)
    /**
     * Firmware information:
     * - 140: Major rewrite
     */
    #define FIRMWARE_VERSION 140
    #define DEVICE_TYPE RB_PIVOT
    #define TYPE_PREFIX "r_p_"
    #define AP_FIRMWARE_INFORMATION "REVOBOT_PIVOT"
    #define WDTG_INTERNAL_TIMER_IN_SECONDS   90  // GSM connection start can lock the main thread for up to 75 seconds
#endif


/**
 * 
 * 
__/\\\______________/\\\_____/\\\\\\\\\_______/\\\\\\\\\______/\\\\\_____/\\\__/\\\\\\\\\\\__/\\\\\_____/\\\_____/\\\\\\\\\\\\_        
 _\/\\\_____________\/\\\___/\\\\\\\\\\\\\___/\\\///////\\\___\/\\\\\\___\/\\\_\/////\\\///__\/\\\\\\___\/\\\___/\\\//////////__       
  _\/\\\_____________\/\\\__/\\\/////////\\\_\/\\\_____\/\\\___\/\\\/\\\__\/\\\_____\/\\\_____\/\\\/\\\__\/\\\__/\\\_____________      
   _\//\\\____/\\\____/\\\__\/\\\_______\/\\\_\/\\\\\\\\\\\/____\/\\\//\\\_\/\\\_____\/\\\_____\/\\\//\\\_\/\\\_\/\\\____/\\\\\\\_     
    __\//\\\__/\\\\\__/\\\___\/\\\\\\\\\\\\\\\_\/\\\//////\\\____\/\\\\//\\\\/\\\_____\/\\\_____\/\\\\//\\\\/\\\_\/\\\___\/////\\\_    
     ___\//\\\/\\\/\\\/\\\____\/\\\/////////\\\_\/\\\____\//\\\___\/\\\_\//\\\/\\\_____\/\\\_____\/\\\_\//\\\/\\\_\/\\\_______\/\\\_   
      ____\//\\\\\\//\\\\\_____\/\\\_______\/\\\_\/\\\_____\//\\\__\/\\\__\//\\\\\\_____\/\\\_____\/\\\__\//\\\\\\_\/\\\_______\/\\\_  
       _____\//\\\__\//\\\______\/\\\_______\/\\\_\/\\\______\//\\\_\/\\\___\//\\\\\__/\\\\\\\\\\\_\/\\\___\//\\\\\_\//\\\\\\\\\\\\/__ 
        ______\///____\///_______\///________\///__\///________\///__\///_____\/////__\///////////__\///_____\/////___\////////////____
 *
 *
 * WARNING: End of safe variables area. Changing defines after this warning can have unintended consequences if you
 *          are NOT sure of WTF you doing!
 *          Proceed with caution.
 */

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define WDG_BEACON_TIMING_MS            10*1000 // send beacon each 10s

#define TIMEOUT_INSTRUCTION_RECEIVED    30*1000 // 30s window of receiving instructions start that will send programs_received


#define MAX_TIME_NO_NETWORK                 (5 * 60 * 1000)   // 5 min

#define MQTT_RECCONECT_TIMER_IN_MILLIS      (5 * 1000)        // 5 seconds
#define MQTT_RECONNECT_TRIES_BEFORE_REBOOT  10


/**
 * If STAGING defined, controller will use stagging MQTT.
 * This define has been moved to an environment variable of PlatformIO, use that instead.
 */
//#define STAGING

#ifdef SECTORS
    #include "controller/sectors/mqttsectors.hpp"

    #define TOTAL_RELAYS_MODULES        8
    #define TOTAL_RELAYS_PER_MODULE     16      // Currently this is 'hardcoded' with the type used to represent the relays, which is 'uint16_t'

    /**
     * Current or next selected Instructions
     */
    struct Program {
        time_t startTime;                       // timestamp when to start this program
        time_t stopTime;                        // timestamp when to stop this program and check for more instructions and create the next program
        uint16_t relays[TOTAL_RELAYS_MODULES];  // relays state during this program
    };

    #define ALERT_TYPE_SIZE     2   // EAlert number of digits
    #define ALERT_EXTRA_SIZE    64  // Extra information space + NULL termination
    #define ALERT_BUFFER_SIZE   (ALERT_TYPE_SIZE + 1 + ALERT_EXTRA_SIZE)  // EAlert number of digits + ',' + space for extra (including termination)
    enum EAlert {
        ALERT_UNKNOWN,
        INSTRUCTION_LIST_FULL,              // in use
        PRESSURE_LOW, PRESSURE_HIGH,        // in use
        FLOW_LOW, FLOW_HIGH,                // TODO: use it
        FILTER_START, FILTER_END,           // in use
        EXCESSIVE_PRESSURE_SWITCH_INPUTS,   // in use
        EXTERNAL_POWER_CUT, EXTERNAL_POWER_RESTABLISHED,    // in use
        PROGRAM_START, PROGRAM_END          // in use
    };
#elif defined(PIVOT)
    // TODO
#endif
