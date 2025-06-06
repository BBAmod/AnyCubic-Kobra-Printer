/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

/**
 * feature/powerloss.cpp - Resume an SD print after power-loss
 */

/**
 * 2023.06.25 George Corrigan
 * 
 * Updated this file to fix the resume print homing bug left
 * behind by AnyCubic. After resume print from power-loss process
 * should only home X and Y, and NOT Z axes.
 */


#include "../inc/MarlinConfigPre.h"

#if ENABLED(POWER_LOSS_RECOVERY)

#include "powerloss.h"
#include "power_monitor.h"
#include "../core/macros.h"
#include "../module/stepper/indirection.h"
#include "../HAL/shared/eeprom_api.h"

#if ENABLED(BABYSTEPPING)
#include "babystep.h"
#endif

bool PrintJobRecovery::enabled; // Initialized by settings.load()

SdFile PrintJobRecovery::file;
job_recovery_info_t PrintJobRecovery::info;
const char PrintJobRecovery::filename[5] = "/PLR";
uint8_t PrintJobRecovery::queue_index_r;
uint32_t PrintJobRecovery::cmd_sdpos, // = 0
         PrintJobRecovery::sdpos[BUFSIZE];

#if ENABLED(DWIN_CREALITY_LCD)
  bool PrintJobRecovery::dwin_flag; // = false
#endif

#include "../sd/cardreader.h"
#include "../lcd/marlinui.h"
#include "../gcode/queue.h"
#include "../gcode/gcode.h"
#include "../module/motion.h"
#include "../module/planner.h"
#include "../module/printcounter.h"
#include "../module/temperature.h"
#include "../core/serial.h"

#if ENABLED(FWRETRACT)
  #include "fwretract.h"
#endif

#if ENABLED(EXTENSIBLE_UI)
  #include "../lcd/extui/ui_api.h"
#endif
#define DEBUG_OUT ENABLED(DEBUG_POWER_LOSS_RECOVERY)
#include "../core/debug_out.h"

PrintJobRecovery recovery;

#ifndef POWER_LOSS_PURGE_LEN
  #define POWER_LOSS_PURGE_LEN 0
#endif
#ifndef POWER_LOSS_ZRAISE
  #define POWER_LOSS_ZRAISE 0     // Move on loss with backup power, or on resume without it
#endif

#if DISABLED(BACKUP_POWER_SUPPLY)
  #undef POWER_LOSS_RETRACT_LEN   // No retract at outage without backup power
#endif
#ifndef POWER_LOSS_RETRACT_LEN
  #define POWER_LOSS_RETRACT_LEN 0
#endif

/**
 * Clear the recovery info
 */
void PrintJobRecovery::init() { memset(&info, 0, sizeof(info)); }

/**
 * Enable or disable then call changed()
 */
void PrintJobRecovery::enable(const bool onoff) {
  enabled = onoff;
  changed();
}

/**
 * The enabled state was changed:
 *  - Enabled: Purge the job recovery file
 *  - Disabled: Write the job recovery file
 */
void PrintJobRecovery::changed() {
  if (!enabled)
    purge();
  else if (IS_SD_PRINTING())
    save(true);
}

/**
 * Check for Print Job Recovery during setup()
 *
 * If a saved state exists send 'M1000 S' to initiate job recovery.
 */
void PrintJobRecovery::check() {
  //if (!card.isMounted()) card.mount();
  if(card.isMounted()) {
    load();
    if (!valid()) return cancel();
    queue.inject_P(PSTR("M1000S"));
  }
}

/**
 * Delete the recovery file and clear the recovery data
 */
void PrintJobRecovery::purge() {
	
#if 0
	  card.removeJobRecoveryFile();
#endif
	
	  if(info.valid_head != 0xFF || info.valid_foot != 0xFF) {
		if(persistentStore.FLASH_If_Erase(FLASH_OUTAGE_DATA_ADDR, FLASH_OUTAGE_DATA_ADDR+0x400) != FLASHIF_OK) {
		}
	  }
	
	  memset(&info, 0, sizeof(info));	// init();

}

/**
 * Load the recovery data, if it exists
 */
void PrintJobRecovery::load() {
     memcpy(&info, (uint8_t *)(FLASH_OUTAGE_DATA_ADDR), sizeof(info));
}

/**
 * Set info fields that won't change
 */
void PrintJobRecovery::prepare() {
  card.getAbsFilename(info.sd_filename);  // SD filename
  cmd_sdpos = 0;
}

/**
 * Save the current machine state to the power-loss recovery file
 */
void PrintJobRecovery::save(const bool force/*=false*/, const float zraise/*=0*/) {

  #if SAVE_INFO_INTERVAL_MS > 0
    static millis_t next_save_ms; // = 0
    millis_t ms = millis();
  #endif

  #ifndef POWER_LOSS_MIN_Z_CHANGE
    #define POWER_LOSS_MIN_Z_CHANGE 0.05  // Vase-mode-friendly out of the box
  #endif

  // Did Z change since the last call?
  if (force
    #if DISABLED(SAVE_EACH_CMD_MODE)      // Always save state when enabled
      #if SAVE_INFO_INTERVAL_MS > 0       // Save if interval is elapsed
        || ELAPSED(ms, next_save_ms)
      #endif
      // Save if Z is above the last-saved position by some minimum height
      || current_position.z > info.current_position.z + POWER_LOSS_MIN_Z_CHANGE
    #endif
  ) {

    #if SAVE_INFO_INTERVAL_MS > 0
      next_save_ms = ms + SAVE_INFO_INTERVAL_MS;
    #endif

    // Set Head and Foot to matching non-zero values
    if (!++info.valid_head) ++info.valid_head; // non-zero in sequence
    //if (!IS_SD_PRINTING()) info.valid_head = 0;
    info.valid_foot = info.valid_head;
//	SERIAL_ECHOLNPAIR("head=", info.valid_head,"foot=", info.valid_foot);

    // Machine state
    info.current_position = current_position;
    info.feedrate = uint16_t(MMS_TO_MMM(feedrate_mm_s));
    info.zraise = zraise;

    TERN_(GCODE_REPEAT_MARKERS, info.stored_repeat = repeat);
    TERN_(HAS_HOME_OFFSET, info.home_offset = home_offset);
    TERN_(HAS_POSITION_SHIFT, info.position_shift = position_shift);

    #if HAS_MULTI_EXTRUDER
      info.active_extruder = active_extruder;
    #endif

    #if DISABLED(NO_VOLUMETRICS)
      info.volumetric_enabled = parser.volumetric_enabled;
      #if HAS_MULTI_EXTRUDER
        for (int8_t e = 0; e < EXTRUDERS; e++) info.filament_size[e] = planner.filament_size[e];
      #else
        if (parser.volumetric_enabled) info.filament_size[0] = planner.filament_size[active_extruder];
      #endif
    #endif

    #if EXTRUDERS
      HOTEND_LOOP() info.target_temperature[e] = thermalManager.temp_hotend[e].target;
    #endif

    TERN_(HAS_HEATED_BED, info.target_temperature_bed = thermalManager.temp_bed.target);

    #if HAS_FAN
      COPY(info.fan_speed, thermalManager.fan_speed);
    #endif

    #if HAS_LEVELING
      info.flag.leveling = planner.leveling_active;
      info.fade = TERN0(ENABLE_LEVELING_FADE_HEIGHT, planner.z_fade_height);
    #endif

    TERN_(GRADIENT_MIX, memcpy(&info.gradient, &mixer.gradient, sizeof(info.gradient)));

    #if ENABLED(FWRETRACT)
      COPY(info.retract, fwretract.current_retract);
      info.retract_hop = fwretract.current_hop;
    #endif

    // Elapsed print job time
    info.print_job_elapsed = print_job_timer.duration();

    info.print_progress = card.percentDone();

    // Relative axis modes
    info.axis_relative = gcode.axis_relative;

    // Misc. Marlin flags
    info.flag.dryrun = !!(marlin_debug_flags & MARLIN_DEBUG_DRYRUN);
    info.flag.allow_cold_extrusion = TERN0(PREVENT_COLD_EXTRUSION, thermalManager.allow_cold_extrude);

    write();
  }
}

#if PIN_EXISTS(POWER_LOSS)
    void PrintJobRecovery::outage() {

      static uint8_t cnt = 0;
      static uint32_t adc_raw_last = 0;

      if(!enabled) {
        return ;
      }

        if(power_monitor.getVoltsADC() < 2200) {

//        SERIAL_ECHOLNPAIR("v:", AD_DMA[2]);

        if(cnt >= 4) {
            _outage();
        }

        if(power_monitor.getVoltsADC() < adc_raw_last) {
            cnt++;
        }

      } else {

        if(cnt != 0) {
            cnt = 0;
        }
      }

      adc_raw_last = power_monitor.getVoltsADC();
    }

    void PrintJobRecovery::adc_raw() {
    }

  #if ENABLED(BACKUP_POWER_SUPPLY)

    void PrintJobRecovery::retract_and_lift(const float &zraise) {
      #if POWER_LOSS_RETRACT_LEN || POWER_LOSS_ZRAISE

        gcode.set_relative_mode(true);  // Use relative coordinates

        #if POWER_LOSS_RETRACT_LEN
          // Retract filament now
          gcode.process_subcommands_now_P(PSTR("G1 F3000 E-" STRINGIFY(POWER_LOSS_RETRACT_LEN)));
        #endif

        #if POWER_LOSS_ZRAISE
          // Raise the Z axis now
          if (zraise) {
            char cmd[20], str_1[16];
            sprintf_P(cmd, PSTR("G0 Z%s"), dtostrf(zraise, 1, 3, str_1));
            gcode.process_subcommands_now(cmd);
          }
        #else
          UNUSED(zraise);
        #endif

        //gcode.axis_relative = info.axis_relative;
        planner.synchronize();
      #endif
    }

  #endif

  /**
   * An outage was detected by a sensor pin.
   *  - If not SD printing, let the machine turn off on its own with no "KILL" screen
   *  - Disable all heaters first to save energy
   *  - Save the recovery data for the current instant
   *  - If backup power is available Retract E and Raise Z
   *  - Go to the KILL screen
   */
  void PrintJobRecovery::_outage() {
    #if ENABLED(BACKUP_POWER_SUPPLY)
      static bool lock = false;
      if (lock) return; // No re-entrance from idle() during retract_and_lift()
      lock = true;
    #endif

  //    TERN_(USE_WATCHDOG, HAL_watchdog_refresh());
  //    __disable_irq();
  
    #if POWER_LOSS_ZRAISE
      // Get the limited Z-raise to do now or on resume
      const float zraise = _MAX(0, _MIN(current_position.z + POWER_LOSS_ZRAISE, Z_MAX_POS - 1) - current_position.z);
    #else
      constexpr float zraise = 0;
    #endif
  
    WRITE(HEATER_0_PIN, 0);
    WRITE(HEATER_BED_PIN, 0);
    ExtUI::onPowerLoss();
    WRITE(X_ENABLE_PIN, 1);
    WRITE(Y_ENABLE_PIN, 1);
    WRITE(Z_ENABLE_PIN, 1);
  
    // Save, including the limited Z raise
    if (IS_SD_PRINTING()) save(true, zraise);
  
    // Disable all heaters to reduce power loss
    thermalManager.disable_all_heaters();
  
    #if ENABLED(BACKUP_POWER_SUPPLY)
      // Do a hard-stop of the steppers (with possibly a loud thud)
      quickstop_stepper();
      // With backup power a retract and raise can be done now
      retract_and_lift(zraise);
    #endif
  
    kill(GET_TEXT(MSG_OUTAGE_RECOVERY));
  }

#endif

/**
 * Save the recovery info the recovery file
 */
void PrintJobRecovery::write() {


  #if 0
  open(false);
  file.seekSet(0);
  const int16_t ret = file.write(&info, sizeof(info));
  if (ret == -1) DEBUG_ECHOLNPGM("Power-loss file write failed.");
  if (!file.close()) DEBUG_ECHOLNPGM("Power-loss file close failed.");
#endif
/*
  if(persistentStore.FLASH_If_Erase(FLASH_OUTAGE_DATA_ADDR, FLASH_OUTAGE_DATA_ADDR+0x400) != FLASHIF_OK) {
    return;
  }
*/
  if(persistentStore.FLASH_If_Write(FLASH_OUTAGE_DATA_ADDR, &info, sizeof(info)) != FLASHIF_OK) {
  	SERIAL_ECHOLNPGM("write error");
  }
}

/**
 * Resume the saved print job
 */
void PrintJobRecovery::resume() {


#define DEBUG_POWERLOSS_RESUME false


#if DEBUG_POWERLOSS_RESUME
  printf("info.x: %f\n", info.current_position.x);
  printf("info.y: %f\n", info.current_position.y);
  printf("info.z: %f\n", info.current_position.z);
#endif

  char cmd[MAX_CMD_SIZE+16], str_1[16], str_2[16];

  const uint32_t resume_sdpos = info.sdpos; // Get here before the stepper ISR overwrites it

  // Apply the dry-run flag if enabled
  if (info.flag.dryrun) marlin_debug_flags |= MARLIN_DEBUG_DRYRUN;

  // Restore cold extrusion permission
  TERN_(PREVENT_COLD_EXTRUSION, thermalManager.allow_cold_extrude = info.flag.allow_cold_extrusion);

#if DEBUG_POWERLOSS_RESUME
  printf("Before M420 S0 Z0: %d\n", __LINE__);
  gcode.process_subcommands_now("M114 D");
#endif

  #if HAS_LEVELING
    // Make sure leveling is off before any G92 and G28
    gcode.process_subcommands_now_P(PSTR("M420 S0 Z0"));
  #endif

#if DEBUG_POWERLOSS_RESUME
  printf("After M420 S0 Z0: %d\n", __LINE__);
  gcode.process_subcommands_now("M114 D");
#endif

  // Restore the bed temperature,no waiting
  #if HAS_HEATED_BED
    int16_t bt = info.target_temperature_bed;
    if (bt) {
      sprintf_P(cmd, PSTR("M140 S%i"), bt);
      gcode.process_subcommands_now(cmd);
    }
  #endif

  // Restore all hotend temperatures,no waiting
  #if HAS_HOTEND
    int16_t et = 0;
    HOTEND_LOOP() {
      et = info.target_temperature[e];
      if (et) {
    #if HAS_MULTI_HOTEND
          sprintf_P(cmd, PSTR("T%i S"), e);
          gcode.process_subcommands_now(cmd);
    #endif
        sprintf_P(cmd, PSTR("M104 S%i"), et);
        gcode.process_subcommands_now(cmd);
      }
    }
  #endif

  #if HAS_HEATED_BED
    bt = info.target_temperature_bed;
    if (bt) {
      // Restore the bed temperature
      sprintf_P(cmd, PSTR("M190 S%i"), bt);
      gcode.process_subcommands_now(cmd);
    }
  #endif

  // Restore all hotend temperatures
  #if HAS_HOTEND
    HOTEND_LOOP() {
      et = info.target_temperature[e];
      if (et) {
        #if HAS_MULTI_HOTEND
          sprintf_P(cmd, PSTR("T%i S"), e);
          gcode.process_subcommands_now(cmd);
        #endif
        sprintf_P(cmd, PSTR("M109 S%i"), et);
        gcode.process_subcommands_now(cmd);
      }
    }
  #endif

#if DEBUG_POWERLOSS_RESUME
  printf("Before XY homed: %d\n", __LINE__);
  gcode.process_subcommands_now("M114 D");
#endif

  // Reset E, raise Z, home XY...
  #if Z_HOME_DIR > 0

    // If Z homing goes to max, just reset E and home all
    gcode.process_subcommands_now_P(PSTR(
      "G92.9 E0\n"
      "G28R0"
    ));

  #else // "G92.9 E0 ..."

    // If a Z raise occurred at outage restore Z, otherwise raise Z now
    sprintf_P(cmd, PSTR("G92.9 E0 " TERN(BACKUP_POWER_SUPPLY, "Z%s", "Z0\nG1Z%s")), dtostrf(info.zraise, 1, 3, str_1));
    gcode.process_subcommands_now(cmd);

    // Home safely with no Z raise
    gcode.process_subcommands_now_P(PSTR(
//      "G28R0"                               // No raise during G28
//      "G28"                                 // raise
      "G28R2"                               // Raise 2mm during G28 to avoid hitting print
      #if IS_CARTESIAN && DISABLED(POWER_LOSS_RECOVER_ZHOME)
        "XY"                                // Don't home Z on Cartesian unless overridden
      #endif
    ));

  #endif

  // Pretend that all axes are homed
  set_all_homed();

#if DEBUG_POWERLOSS_RESUME
  printf("After set all homed: %d\n", __LINE__);
  gcode.process_subcommands_now("M114 D");
#endif

  #if ENABLED(POWER_LOSS_RECOVER_ZHOME)
    // Z has been homed so restore Z to ZsavedPos + POWER_LOSS_ZRAISE
    sprintf_P(cmd, PSTR("G1 F500 Z%s"), dtostrf(info.current_position.z + POWER_LOSS_ZRAISE, 1, 3, str_1));
    gcode.process_subcommands_now(cmd);
  #endif

  // Recover volumetric extrusion state
  #if DISABLED(NO_VOLUMETRICS)
    #if HAS_MULTI_EXTRUDER
      for (int8_t e = 0; e < EXTRUDERS; e++) {
        sprintf_P(cmd, PSTR("M200 T%i D%s"), e, dtostrf(info.filament_size[e], 1, 3, str_1));
        gcode.process_subcommands_now(cmd);
      }
      if (!info.volumetric_enabled) {
        sprintf_P(cmd, PSTR("M200 T%i D0"), info.active_extruder);
        gcode.process_subcommands_now(cmd);
      }
    #else
      if (info.volumetric_enabled) {
        sprintf_P(cmd, PSTR("M200 D%s"), dtostrf(info.filament_size[0], 1, 3, str_1));
        gcode.process_subcommands_now(cmd);
      }
    #endif
  #endif

  // Select the previously active tool (with no_move)
  #if HAS_MULTI_EXTRUDER
    sprintf_P(cmd, PSTR("T%i S"), info.active_extruder);
    gcode.process_subcommands_now(cmd);
  #endif

  // Restore print cooling fan speeds
  #if HAS_FAN
    FANS_LOOP(i) {
      const int f = info.fan_speed[i];
      if (f) {
        sprintf_P(cmd, PSTR("M106 P%i S%i"), i, f);
        gcode.process_subcommands_now(cmd);
      }
    }
  #endif

  // Restore retract and hop state
  #if ENABLED(FWRETRACT)
    LOOP_L_N(e, EXTRUDERS) {
      if (info.retract[e] != 0.0) {
        fwretract.current_retract[e] = info.retract[e];
        fwretract.retracted[e] = true;
      }
    }
    fwretract.current_hop = info.retract_hop;
  #endif

// George: Removed for testing
//sprintf_P(cmd, PSTR("G1 F500 Z%s"), dtostrf(info.current_position.z + 5, 1, 3, str_1));
//gcode.process_subcommands_now(cmd);

#if DEBUG_POWERLOSS_RESUME
  printf("line: %d\n", __LINE__);
  printf("info.flag.leveling: %d\n", info.flag.leveling);
  printf("info.flag.fade: %d\n", info.fade);
  gcode.process_subcommands_now("M114 D");
#endif

  #if HAS_LEVELING
    // Restore leveling state before 'G92 Z' to ensure
    // the Z stepper count corresponds to the native Z.
    if (info.fade || info.flag.leveling) {
      sprintf_P(cmd, PSTR("M420 S%i Z%s"), int(info.flag.leveling), dtostrf(info.fade, 1, 1, str_1));
      gcode.process_subcommands_now(cmd);
    }
  #endif

#if DEBUG_POWERLOSS_RESUME
  printf("After M420: %d\n", __LINE__);
  gcode.process_subcommands_now("M114 D");
#endif

  #if ENABLED(GRADIENT_MIX)
    memcpy(&mixer.gradient, &info.gradient, sizeof(info.gradient));
  #endif

  // Un-retract if there was a retract at outage
  #if POWER_LOSS_RETRACT_LEN
    gcode.process_subcommands_now_P(PSTR("G1 E" STRINGIFY(POWER_LOSS_RETRACT_LEN) " F3000"));
  #endif

  // Additional purge if configured
  #if POWER_LOSS_PURGE_LEN
    sprintf_P(cmd, PSTR("G1 E%d F200"), (POWER_LOSS_PURGE_LEN) + (POWER_LOSS_RETRACT_LEN));
    gcode.process_subcommands_now(cmd);
  #endif

  #if ENABLED(NOZZLE_CLEAN_FEATURE)
    gcode.process_subcommands_now_P(PSTR("G12"));
  #endif

#if ENABLED(BABYSTEPPING)
  #if DEBUG_POWERLOSS_RESUME
    printf("Before baby: %d\n", __LINE__);
    gcode.process_subcommands_now("M114 D");
  #endif

    dtostrf(info.current_position.z + 2.0f, 1, 3, str_1); // We raised 2mm before homing XY to avoid hitting print
    sprintf_P(cmd, PSTR("G92.9 Z%s"), str_1);
    gcode.process_subcommands_now(cmd);

    xyze_pos_t pos_lev   = info.current_position;
    planner.apply_leveling(pos_lev);

    float z_diff = info.current_position.z - pos_lev.z;

  #if DEBUG_POWERLOSS_RESUME
    printf("pos_lev.z:  %f\n", pos_lev.z);
    printf("z_diff   :  %f\n", z_diff);
  #endif

    xyze_pos_t hm_pos_lev = { X_HOME_POS, Y_HOME_POS, Z_HOME_POS };
    planner.apply_leveling(hm_pos_lev);
    float hm_z_diff = 0 - hm_pos_lev.z;

    float all_diff = z_diff - hm_z_diff;

  #if DEBUG_POWERLOSS_RESUME
    printf("hm_pos_lev.z:  %f\n", hm_pos_lev.z);
    printf("hm_z_diff   :  %f\n", hm_z_diff);
  #endif

    float all_steps = all_diff / planner.steps_to_mm[Z_AXIS];
    int16_t all_baby_steps = all_steps > 0 ? CEIL(all_steps) : FLOOR(all_steps);

  #if DEBUG_POWERLOSS_RESUME
    printf("all_steps     :  %f\n", all_steps);
    printf("all_baby_steps:  %f\n", all_baby_steps);
  #endif

    babystep.add_steps(Z_AXIS, all_baby_steps);
  
    gcode.process_subcommands_now("M400\nG4 P1000");

  #if DEBUG_POWERLOSS_RESUME
    printf("After baby: %d\n", __LINE__);
    gcode.process_subcommands_now("M114 D");
  #endif

#endif

#if DEBUG_POWERLOSS_RESUME
  printf("Before moving to XY: %d\n", __LINE__);
  gcode.process_subcommands_now("M114 D");
#endif

  // Move back to the saved XY
  sprintf_P(cmd, PSTR("G1 X%s Y%s F3000"),
    dtostrf(info.current_position.x, 1, 3, str_1),
    dtostrf(info.current_position.y, 1, 3, str_2)
  );
  gcode.process_subcommands_now(cmd);

  gcode.process_subcommands_now("M400");

#if DEBUG_POWERLOSS_RESUME
  printf("Before Z down: %d\n", __LINE__);
  gcode.process_subcommands_now("M114 D");
#endif

// Move back to the saved Z
  dtostrf(info.current_position.z, 1, 3, str_1);
  sprintf_P(cmd, PSTR("G1 Z%s F200"), str_1);
  gcode.process_subcommands_now(cmd);

#if DEBUG_POWERLOSS_RESUME
  printf("After Z down: %d\n", __LINE__);
  gcode.process_subcommands_now("M114 D");
#endif

  // Restore the feedrate
  sprintf_P(cmd, PSTR("G1 F%d"), info.feedrate);
  gcode.process_subcommands_now(cmd);

  // Restore E position with G92.9
  sprintf_P(cmd, PSTR("G92.9 E%s"), dtostrf(info.current_position.e, 1, 3, str_1));
  gcode.process_subcommands_now(cmd);

  TERN_(GCODE_REPEAT_MARKERS, repeat = info.stored_repeat);
  TERN_(HAS_HOME_OFFSET, home_offset = info.home_offset);
  TERN_(HAS_POSITION_SHIFT, position_shift = info.position_shift);
  #if HAS_HOME_OFFSET || HAS_POSITION_SHIFT
    LOOP_XYZ(i) update_workspace_offset((AxisEnum)i);
  #endif

  // Relative axis modes
  gcode.axis_relative = info.axis_relative;

  #if ENABLED(DEBUG_POWER_LOSS_RECOVERY)
    const uint8_t old_flags = marlin_debug_flags;
    marlin_debug_flags |= MARLIN_DEBUG_ECHO;
  #endif

  // Continue to apply PLR when a file is resumed!
  enable(true);

  // Resume the SD file from the last position
  char *fn = info.sd_filename;
  extern const char M23_STR[];
  sprintf_P(cmd, M23_STR, fn);
  gcode.process_subcommands_now(cmd);
  sprintf_P(cmd, PSTR("M24 S%ld T%ld"), resume_sdpos, info.print_job_elapsed);
  gcode.process_subcommands_now(cmd);

  TERN_(DEBUG_POWER_LOSS_RECOVERY, marlin_debug_flags = old_flags);
}

#if ENABLED(DEBUG_POWER_LOSS_RECOVERY)

  void PrintJobRecovery::debug(PGM_P const prefix) {
    DEBUG_PRINT_P(prefix);
    DEBUG_ECHOLNPAIR(" Job Recovery Info...\nvalid_head:", int(info.valid_head), " valid_foot:", int(info.valid_foot));
    if (info.valid_head) {
      if (info.valid_head == info.valid_foot) {
        DEBUG_ECHOPGM("current_position: ");
        LOOP_XYZE(i) {
          if (i) DEBUG_CHAR(',');
          DEBUG_DECIMAL(info.current_position[i]);
        }
        DEBUG_EOL();

        DEBUG_ECHOLNPAIR("zraise: ", info.zraise);

        #if HAS_HOME_OFFSET
          DEBUG_ECHOPGM("home_offset: ");
          LOOP_XYZ(i) {
            if (i) DEBUG_CHAR(',');
            DEBUG_DECIMAL(info.home_offset[i]);
          }
          DEBUG_EOL();
        #endif

        #if HAS_POSITION_SHIFT
          DEBUG_ECHOPGM("position_shift: ");
          LOOP_XYZ(i) {
            if (i) DEBUG_CHAR(',');
            DEBUG_DECIMAL(info.position_shift[i]);
          }
          DEBUG_EOL();
        #endif

        DEBUG_ECHOLNPAIR("feedrate: ", info.feedrate);

        #if HAS_MULTI_EXTRUDER
          DEBUG_ECHOLNPAIR("active_extruder: ", int(info.active_extruder));
        #endif

        #if HAS_HOTEND
          DEBUG_ECHOPGM("target_temperature: ");
          HOTEND_LOOP() {
            DEBUG_ECHO(info.target_temperature[e]);
            if (e < HOTENDS - 1) DEBUG_CHAR(',');
          }
          DEBUG_EOL();
        #endif

        #if HAS_HEATED_BED
          DEBUG_ECHOLNPAIR("target_temperature_bed: ", info.target_temperature_bed);
        #endif

        #if HAS_FAN
          DEBUG_ECHOPGM("fan_speed: ");
          FANS_LOOP(i) {
            DEBUG_ECHO(int(info.fan_speed[i]));
            if (i < FAN_COUNT - 1) DEBUG_CHAR(',');
          }
          DEBUG_EOL();
        #endif

        #if HAS_LEVELING
          DEBUG_ECHOLNPAIR("leveling: ", int(info.flag.leveling), " fade: ", info.fade);
        #endif
        #if ENABLED(FWRETRACT)
          DEBUG_ECHOPGM("retract: ");
          for (int8_t e = 0; e < EXTRUDERS; e++) {
            DEBUG_ECHO(info.retract[e]);
            if (e < EXTRUDERS - 1) DEBUG_CHAR(',');
          }
          DEBUG_EOL();
          DEBUG_ECHOLNPAIR("retract_hop: ", info.retract_hop);
        #endif
        DEBUG_ECHOLNPAIR("sd_filename: ", info.sd_filename);
        DEBUG_ECHOLNPAIR("sdpos: ", info.sdpos);
        DEBUG_ECHOLNPAIR("print_job_elapsed: ", info.print_job_elapsed);
        DEBUG_ECHOLNPAIR("dryrun: ", int(info.flag.dryrun));
        DEBUG_ECHOLNPAIR("allow_cold_extrusion: ", int(info.flag.allow_cold_extrusion));
      }
      else
        DEBUG_ECHOLNPGM("INVALID DATA");
    }
    DEBUG_ECHOLNPGM("---");
  }

#endif // DEBUG_POWER_LOSS_RECOVERY

#endif // POWER_LOSS_RECOVERY
