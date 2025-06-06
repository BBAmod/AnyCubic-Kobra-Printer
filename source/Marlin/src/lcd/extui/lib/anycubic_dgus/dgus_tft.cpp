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
 * lcd/extui/lib/dgus_tft.cpp
 *
 * Extensible_UI implementation for Anycubic Chiron
 * Written By Nick Wells, 2020 [https://github.com/SwiftNick]
 *  (not affiliated with Anycubic, Ltd.)
 */

#include "../../../../inc/MarlinConfigPre.h"

#if ENABLED(ANYCUBIC_LCD_DGUS)

#include "dgus_tft.h"
#include "dgus_Tunes.h"
#include "dgus_FileNavigator.h"

#include "../../../../gcode/queue.h"
#include "../../../../sd/cardreader.h"
#include "../../../../libs/numtostr.h"
//#include "../../../../libs/build_date.h"
#include "../../../../MarlinCore.h"
#include "../../../../feature/powerloss.h"

namespace Anycubic {

  const char MESSAGE_charu[]={0xB4,0xE6,0xB4,0xA2,0xBF,0xA8,0xD2,0xD1,0xB2,0xE5,0xC8,0xEB,0x00};
  const char MESSAGE_bachu[]={0xB4,0xE6,0xB4,0xA2,0xBF,0xA8,0xD2,0xD1,0xB0,0xCE,0xB3,0xF6,0x00};
  const char MESSAGE_wuka[]={0xCE,0xDE ,0xB4,0xE6 ,0xB4,0xA2 ,0xBF,0xA8,0x00};
  const char MESSAGE_lianji[]={0xC1, 0xAA ,0xBB ,0xFA, 0xD6 ,0xD0,0x00};
  const char MESSAGE_tuoji[]={0xCD,0xD1 ,0xBB,0xFA ,0xB4,0xF2 ,0xD3,0xA1 ,0xD6,0xD0,0x00};
  const char MESSAGE_zanting[]={0xB4,0xF2,0xD3,0xA1 ,0xD4,0xDD ,0xCD,0xA3 ,0xD6,0xD0,0x00}   ;
  const char MESSAGE_tingzhi[]={0xCD,0xA3 ,0xD6,0xB9 ,0xB4,0xF2 ,0xD3,0xA1,0x00} ;
  const char MESSAGE_wancheng[]={0xCD,0xEA ,0xB3,0xC9 ,0xB4,0xF2 ,0xD3,0xA1,0x00}    ;
  const char MESSAGE_hotend_heating[]={0xB4,0xF2 ,0xD3,0xA1 ,0xCD,0xB7 ,0xD5,0xFD ,0xD4,0xDA ,0xBC,0xD3 ,0xC8,0xC8,0x00} ;
  const char MESSAGE_hotend_over[]={0xB4,0xF2 ,0xD3,0xA1 ,0xCD,0xB7 ,0xBC,0xD3 ,0xC8,0xC8  ,0xCD,0xEA ,0xB3,0xC9,0x00}   ;
  const char MESSAGE_bed_heating[]={0xC8,0xC8 ,0xB4,0xB2 ,0xD5,0xFD ,0xD4,0xDA ,0xBC,0xD3 ,0xC8,0xC8,0x00}   ;
  const char MESSAGE_bed_over[]={0xC8,0xC8 ,0xB4,0xB2 ,0xBC,0xD3 ,0xC8,0xC8  ,0xCD,0xEA ,0xB3,0xC9,0x00} ;
  const char MESSAGE_ready[]={0xD7,0xBC  ,0xB1,0xB8 ,0xBE,0xCD ,0xD0,0xF7,0x00}  ;
  const char MESSAGE_cold[]={0xB4,0xF2 ,0xD3,0xA1 ,0xCD,0xB7 ,0xCE,0xC2 ,0xB6,0xC8 ,0xB9,0xFD ,0xB5,0xCD,0x00}   ;

  const char *p_mesage[]={MESSAGE_charu ,MESSAGE_bachu ,MESSAGE_wuka,MESSAGE_lianji ,MESSAGE_tuoji ,MESSAGE_zanting, MESSAGE_tingzhi ,MESSAGE_wancheng ,MESSAGE_hotend_heating, MESSAGE_hotend_over, MESSAGE_bed_heating, MESSAGE_bed_over, MESSAGE_ready, MESSAGE_cold};

  DgusTFT::p_fun fun_array[] = {
                         DgusTFT::page1_handle,
                         DgusTFT::page2_handle,
                         DgusTFT::page3_handle,
                         DgusTFT::page4_handle,
                         DgusTFT::page5_handle,
                         DgusTFT::page6_handle,
                         DgusTFT::page7_handle,
                         DgusTFT::page8_handle,
                         DgusTFT::page9_handle,
                         DgusTFT::page10_handle,
                         DgusTFT::page11_handle,
                         DgusTFT::page12_handle,
                         DgusTFT::page13_handle,
                         DgusTFT::page14_handle,
                         DgusTFT::page15_handle,
                         DgusTFT::page16_handle,
                         DgusTFT::page17_handle,
                         DgusTFT::page18_handle,
                         DgusTFT::page19_handle,
                         DgusTFT::page20_handle,
                         DgusTFT::page21_handle,
                         DgusTFT::page22_handle,
                         DgusTFT::page23_handle,
                         DgusTFT::page24_handle,
                         DgusTFT::page25_handle,
                         DgusTFT::page26_handle,
                         DgusTFT::page27_handle,
                         DgusTFT::page28_handle,
                         DgusTFT::page29_handle,
                         DgusTFT::page30_handle,
                         DgusTFT::page31_handle,
                         DgusTFT::page32_handle,
                         DgusTFT::page33_handle,
                         DgusTFT::page34_handle
                       };
  

  printer_state_t  DgusTFT::printer_state;
  paused_state_t   DgusTFT::pause_state;
  heater_state_t   DgusTFT::hotend_state;
  heater_state_t   DgusTFT::hotbed_state;
  xy_uint8_t       DgusTFT::selectedmeshpoint;
  char             DgusTFT::selectedfile[MAX_PATH_LEN];
  char             DgusTFT::panel_command[MAX_CMND_LEN];
  uint8_t          DgusTFT::command_len;
  float            DgusTFT::live_Zoffset;
  file_menu_t      DgusTFT::file_menu;
  
  bool             DgusTFT::data_received;
  uint8_t          DgusTFT::data_buf[64];
  uint8_t          DgusTFT::data_index;
  uint32_t         DgusTFT::page_index_last;
  uint32_t         DgusTFT::page_index_last_2;
  uint32_t         DgusTFT::page_index_now;
  uint8_t          DgusTFT::message_index;
  uint8_t          DgusTFT::pop_up_index;
  uint32_t         DgusTFT::key_index;
  uint32_t         DgusTFT::key_value;
  uint16_t         DgusTFT::filenumber;
  uint16_t         DgusTFT::filepage;
  uint8_t          DgusTFT::lcd_txtbox_index;
  uint8_t          DgusTFT::lcd_txtbox_page;
  uint16_t         DgusTFT::change_color_index;
  uint8_t          DgusTFT::TFTpausingFlag;
  uint8_t          DgusTFT::TFTStatusFlag;
  uint8_t          DgusTFT::TFTresumingflag;
  uint8_t          DgusTFT::ready;
  lcd_info_t       DgusTFT::lcd_info;
  lcd_info_t       DgusTFT::lcd_info_back;  // back for changing on lcd, to save flash lifecycle


  DgusTFT Dgus;

  DgusTFT::DgusTFT() {
    data_buf[0]   = '\0';
    message_index = 100;
    pop_up_index  = 100;
    page_index_now = 1;
    page_index_last = 1;
    page_index_last_2 = 1;

    lcd_txtbox_index = 0;
  }

  void DgusTFT::Startup() {
    selectedfile[0]   = '\0';
    panel_command[0]  = '\0';
    command_len       = 0;
    printer_state     = AC_printer_idle;
    pause_state       = AC_paused_idle;
    hotend_state      = AC_heater_off;
    hotbed_state      = AC_heater_off;
    live_Zoffset      = 0.0;
    file_menu         = AC_menu_file;

    // Setup pins for powerloss detection
    // Two IO pins are connected on the Trigorilla Board
    // On a power interruption the OUTAGECON_PIN goes low.

//    #if ENABLED(POWER_LOSS_RECOVERY)
//      OUT_WRITE(OUTAGECON_PIN, HIGH);
//    #endif

    // Filament runout is handled by Marlin settings in Configuration.h
    // opt_set    FIL_RUNOUT_STATE HIGH  // Pin state indicating that filament is NOT present.
    // opt_enable FIL_RUNOUT_PULLUP

    TFTSer.begin(115200);

    // Signal Board has reset
//    SendtoTFTLN(AC_msg_main_board_has_reset);

    // Enable levelling and Disable end stops during print
    // as Z home places nozzle above the bed so we need to allow it past the end stops
    
    injectCommands_P(AC_cmnd_enable_levelling);

    // Startup tunes are defined in Tunes.h
    // PlayTune(BEEPER_PIN, Anycubic_PowerOn, 1);
    // PlayTune(BEEPER_PIN, GB_PowerOn, 1);
    #if ACDEBUGLEVEL
      SERIAL_ECHOLNPAIR("AC Debug Level ", ACDEBUGLEVEL);
    #endif
//    SendtoTFTLN(AC_msg_ready);
  }

  void DgusTFT::ParamInit() {

    if(lcd_info.language == ExtUI::CHS) {
      page_index_now = 1;
    } else if(lcd_info.language == ExtUI::ENG) {
      page_index_now = 121;
    }

    LcdAudioSet(lcd_info.audio);

#if ACDEBUG(AC_MARLIN)
    if(lcd_info.language == ExtUI::CHS) {
      SERIAL_ECHOLN("lcd language: CHS");
    } else if(lcd_info.language == ExtUI::ENG) {
      SERIAL_ECHOLN("lcd language: ENG");
    }

    if(lcd_info.audio == ExtUI::ON) {
      SERIAL_ECHOLN("lcd audio: ON");
    } else if(lcd_info.audio == ExtUI::OFF) {
      SERIAL_ECHOLN("lcd audio: OFF");
    }
#endif

  RequestValueFromTFT(0x14);  // get page ID
  }

  void DgusTFT::IdleLoop()  {
    if (ReadTFTCommand()) {
      ProcessPanelRequest();
      command_len = 0;
    }

#if ACDEBUG(AC_MARLIN)
    if(key_value) {
      SERIAL_ECHOLNPAIR("page: ", page_index_now);
      SERIAL_ECHOLNPAIR("key: ", key_value);
    }
#endif

    static uint32_t milli_last = 0;
    if(millis() - milli_last > 1500) {
        milli_last = millis();

        char str_buf[10];
        sprintf(str_buf,"%u/%u",(uint16_t)getActualTemp_celsius(E0), (uint16_t)getTargetTemp_celsius(E0));
        SendTxtToTFT(str_buf, TXT_MAIN_HOTEND);

        sprintf(str_buf,"%u/%u",(uint16_t)getActualTemp_celsius(BED), (uint16_t)getTargetTemp_celsius(BED));
        SendTxtToTFT(str_buf, TXT_MAIN_BED);
    }

    if(page_index_now == 115) {
      page115_handle();
    } else if(page_index_now == 117) {
      page117_handle();
    } else if(page_index_now == 170) {
      page170_handle();
    } else if(page_index_now == 171) {
      page171_handle();
    } else if(page_index_now == 173) {
      page173_handle();
    } else if(page_index_now == 175) {
      page175_handle();
    } else if(page_index_now == 176) {
      page176_handle();
    } else if(177 <= page_index_now && page_index_now <= 198) {
#if 0 // ACDEBUG(AC_MARLIN)
        SERIAL_ECHOLNPAIR("line: ", __LINE__);
        SERIAL_ECHOLNPAIR("func: ", page_index_now);
#endif
//      page177_to_198_handle();
    } else if(199 == page_index_now || page_index_now == 200) {
#if 0 // ACDEBUG(AC_MARLIN)
        SERIAL_ECHOLNPAIR("line: ", __LINE__);
        SERIAL_ECHOLNPAIR("func: ", page_index_now);
#endif
      page199_to_200_handle();
    } else if(page_index_now == 201 || page_index_now == 204) {

      page201_handle();

    } else if(page_index_now == 202 || page_index_now == 205) {

      page202_handle();

    } else if(page_index_now == 203 || page_index_now == 206) {

      page203_handle();

    } else {

      if(lcd_info.language == ExtUI::CHS) {
        if(0 < page_index_now && page_index_now < 36) {
          fun_array[page_index_now-1]();
        } else {
#if ACDEBUG(AC_MARLIN)
          SERIAL_ECHOLNPAIR("line: ", __LINE__);
          SERIAL_ECHOLNPAIR("fun not exists: ", page_index_now);
#endif
        }
      } else if(lcd_info.language == ExtUI::ENG) {
        if(120 < page_index_now && page_index_now < 156) {
          fun_array[page_index_now-1-120]();  // ENG page_index is 120 more than CHS
        } else {
#if ACDEBUG(AC_MARLIN)
          SERIAL_ECHOLN("lcd function not exists");
          SERIAL_ECHOLNPAIR("page_index_last: ", page_index_last);
          SERIAL_ECHOLNPAIR("page_index_last_2: ", page_index_last_2);
#endif
        }
      }

    }

    pop_up_manager();
    key_value = 0;

    CheckHeaters();
  }

  void DgusTFT::PrinterKilled(PGM_P error,PGM_P component)  {
//    SendtoTFTLN(AC_msg_kill_lcd);
    #if ACDEBUG(AC_MARLIN)
      SERIAL_ECHOLNPAIR("PrinterKilled()\nerror: ", error , "\ncomponent: ", component);
    #endif
    
    if(strncmp(error, "Heating Failed", strlen("Heating Failed")) == 0) {

        if(strncmp(component, "Bed", strlen("Bed")) == 0) {
            ChangePageOfTFT(PAGE_CHS_ABNORMAL_BED_HEATER);
            SERIAL_ECHOLNPAIR("Check Bed heater");
        } else if(strncmp(component, "E1", strlen("E1")) == 0) {
            ChangePageOfTFT(PAGE_CHS_ABNORMAL_HOTEND_HEATER);
            SERIAL_ECHOLNPAIR("Check E1 heater");
        }

    } else if(strncmp(error, "Err: MINTEMP", strlen("Err: MINTEMP")) == 0) {

        if(strncmp(component, "Bed", strlen("Bed")) == 0) {
            ChangePageOfTFT(PAGE_CHS_ABNORMAL_BED_NTC);
            SERIAL_ECHOLNPAIR("Check Bed thermistor");
        } else if(strncmp(component, "E1", strlen("E1")) == 0) {
            ChangePageOfTFT(PAGE_CHS_ABNORMAL_HOTEND_NTC);
            SERIAL_ECHOLNPAIR("Check E1 thermistor");
        }
    } else if(strncmp(error, "Err: MAXTEMP", strlen("Err: MAXTEMP")) == 0) {

        if(strncmp(component, "Bed", strlen("Bed")) == 0) {
            ChangePageOfTFT(PAGE_CHS_ABNORMAL_BED_NTC);
            SERIAL_ECHOLNPAIR("Check Bed thermistor");
        } else if(strncmp(component, "E1", strlen("E1")) == 0) {
            ChangePageOfTFT(PAGE_CHS_ABNORMAL_HOTEND_NTC);
            SERIAL_ECHOLNPAIR("Check E1 thermistor");
        }
    } else if(strncmp(error, "THERMAL RUNAWAY", strlen("THERMAL RUNAWAY")) == 0) {

        if(strncmp(component, "Bed", strlen("Bed")) == 0) {
            ChangePageOfTFT(PAGE_CHS_ABNORMAL_BED_HEATER);
            SERIAL_ECHOLNPAIR("Check Bed thermal runaway");
        } else if(strncmp(component, "E1", strlen("E1")) == 0) {
            ChangePageOfTFT(PAGE_CHS_ABNORMAL_HOTEND_HEATER);
            SERIAL_ECHOLNPAIR("Check E1 thermal runaway");
        }

    } else if(strncmp(error, "Homing Failed", strlen("Homing Failed")) == 0) {

        if(strncmp(component, "X", strlen("X")) == 0) {
            ChangePageOfTFT(PAGE_CHS_ABNORMAL_X_ENDSTOP);
            SERIAL_ECHOLNPAIR("Check X endstop");
        } else if(strncmp(component, "Y", strlen("Y")) == 0) {
            ChangePageOfTFT(PAGE_CHS_ABNORMAL_Y_ENDSTOP);
            SERIAL_ECHOLNPAIR("Check Y endstop");
        } else if(strncmp(component, "Z", strlen("Z")) == 0) {
            ChangePageOfTFT(PAGE_CHS_ABNORMAL_Z_ENDSTOP);
            SERIAL_ECHOLNPAIR("Check Z endstop");
        }
    }

  }

  void DgusTFT::MediaEvent(media_event_t event)  {
    #if ACDEBUG(AC_MARLIN)
      SERIAL_ECHOLNPAIR("ProcessMediaStatus() ", event);
    #endif
    switch(event) {
      case AC_media_inserted:

        filenavigator.reset();

        lcd_txtbox_page = 0;
        if(lcd_txtbox_index) {
          SendColorToTFT(COLOR_BLUE, TXT_DISCRIBE_0+0x30*(lcd_txtbox_index-1));
          lcd_txtbox_index = 0;
        }

        SendFileList(lcd_txtbox_index);

      break;

      case AC_media_removed:
//        SendtoTFTLN(AC_msg_sd_card_removed);

        filenavigator.reset();

        lcd_txtbox_page = 0;
        if(lcd_txtbox_index) {
          SendColorToTFT(COLOR_BLUE, TXT_DISCRIBE_0+0x30*(lcd_txtbox_index-1));
          lcd_txtbox_index = 0;
        }

        SendFileList(lcd_txtbox_index);

      break;

      case AC_media_error:
//        SendtoTFTLN(AC_msg_no_sd_card);
      break;
    }
  }

  void DgusTFT::TimerEvent(timer_event_t event)  {
    char str_buf[20];

    #if ACDEBUG(AC_MARLIN)
      SERIAL_ECHOLNPAIR("TimerEvent() ", event);
      SERIAL_ECHOLNPAIR("Printer State: ", printer_state);
    #endif

    switch (event) {
      case AC_timer_started: {
        live_Zoffset = 0.0; // reset print offset
        setSoftEndstopState(false);  // disable endstops to print
        printer_state = AC_printer_printing;
//        SendtoTFTLN(AC_msg_print_from_sd_card);
      } break;

      case AC_timer_paused: {
//        printer_state = AC_printer_paused;
//        pause_state   = AC_paused_idle;
//        SendtoTFTLN(AC_msg_paused);
      }
      break;

      case AC_timer_stopped: {
        if (printer_state != AC_printer_idle) {

          if(printer_state == AC_printer_stopping_from_media_remove) {

            ChangePageOfTFT(PAGE_NO_SD);

          } else {

            printer_state = AC_printer_stopping;

            // Get Printing Time
            uint32_t time = getProgress_seconds_elapsed() / 60;
            sprintf(str_buf, "%s H ", utostr3(time/60));
            sprintf(str_buf+strlen(str_buf), "%s M", utostr3(time%60));
            SendTxtToTFT(str_buf, TXT_FINISH_TIME);
            ChangePageOfTFT(PAGE_PRINT_FINISH);
          }          
        }
        setSoftEndstopState(true); // enable endstops
      } 
      break;
    }
  }

  void DgusTFT::FilamentRunout()  {
    #if ACDEBUG(AC_MARLIN)
      SERIAL_ECHOLNPAIR("FilamentRunout() printer_state ", printer_state);
    
    // 1 Signal filament out
//    SendtoTFTLN(isPrintingFromMedia() ? AC_msg_filament_out_alert : AC_msg_filament_out_block);
//    printer_state = AC_printer_filament_out;

      SERIAL_ECHOLNPAIR("getFilamentRunoutState: ", getFilamentRunoutState());
    #endif

    pop_up_index = 15;  // show filament lack.
    
    if(READ(FIL_RUNOUT_PIN) != getFilamentRunoutOriginState()) {
      PlayTune(BEEPER_PIN, FilamentOut, 1);

      if(isPrintingFromMedia()) {
        pausePrint();
        printer_state = AC_printer_pausing;
        pause_state = AC_paused_filament_lack;
      }
    }
  }

  void DgusTFT::ConfirmationRequest(const char * const msg)  {
    // M108 continue
    #if ACDEBUG(AC_MARLIN)
      SERIAL_ECHOLNPAIR("ConfirmationRequest() ", msg);
      SERIAL_ECHOLNPAIR("printer_state:", printer_state);
      SERIAL_ECHOLNPAIR("pause_state:", pause_state);
    #endif

    switch (printer_state) {
      case AC_printer_pausing: {
        if (strcmp_P(msg, MARLIN_msg_print_paused) == 0 || strcmp_P(msg, MARLIN_msg_nozzle_parked) == 0) {
          if(pause_state != AC_paused_filament_lack) {
            ChangePageOfTFT(PAGE_STATUS1);    // enable continue button
          }
          printer_state = AC_printer_paused;
        }
      } break;

      case AC_printer_resuming_from_power_outage:
      case AC_printer_printing:
      case AC_printer_paused: {
        // Heater timout, send acknowledgement
        if (strcmp_P(msg, MARLIN_msg_heater_timeout) == 0) {
          pause_state = AC_paused_heater_timed_out;
//          SendtoTFTLN(AC_msg_paused); // enable continue button
          PlayTune(BEEPER_PIN,Heater_Timedout,1);
        }
        // Reheat finished, send acknowledgement
        else if (strcmp_P(msg, MARLIN_msg_reheat_done) == 0) {

#if ACDEBUG(AC_MARLIN)
          SERIAL_ECHOLNPAIR("send M108 ", __LINE__);
#endif
          injectCommands_P(PSTR("M108"));

          if(pause_state != AC_paused_filament_lack) {
            pause_state = AC_paused_idle;
          }

//          SendtoTFTLN(AC_msg_paused); // enable continue button
        }
        // Filament Purging, send acknowledgement enter run mode
        else if (strcmp_P(msg, MARLIN_msg_filament_purging) == 0) {
          pause_state = AC_paused_purging_filament;
//          SendtoTFTLN(AC_msg_paused); // enable continue button
        } else if(strcmp_P(msg, MARLIN_msg_nozzle_parked) == 0) {
#if ACDEBUG(AC_MARLIN)
            SERIAL_ECHOLNPAIR("send M108 ", __LINE__);
#endif
            injectCommands_P(PSTR("M108"));

            if(pause_state != AC_paused_filament_lack) {
              pause_state = AC_paused_idle;
            }
        }
      } break;

      default:
      break;
    }
  }

  void DgusTFT::StatusChange(const char * const msg)  {
    #if ACDEBUG(AC_MARLIN)
      SERIAL_ECHOLNPAIR("StatusChange() ", msg);
      SERIAL_ECHOLNPAIR("printer_state:", printer_state);
      SERIAL_ECHOLNPAIR("pause_state:", pause_state);
    #endif
    bool msg_matched = false;
    static uint8_t probe_cnt = 0;
    // The only way to get printer status is to parse messages
    // Use the state to minimise the work we do here.
    switch (printer_state) {
      case AC_printer_probing: {

        if (strncmp(msg, MARLIN_msg_probing_point, strlen(MARLIN_msg_probing_point)) == 0) {
            probe_cnt++;
        }

        // If probing completes ok save the mesh and park
        // Ignore the custom machine name
        if (strcmp_P(msg + strlen(CUSTOM_MACHINE_NAME), MARLIN_msg_ready) == 0) {
          if(probe_cnt == GRID_MAX_POINTS_X*GRID_MAX_POINTS_Y) {
            probe_cnt = 0;
            injectCommands_P(PSTR("M500"));
            FakeChangePageOfTFT(PAGE_PreLEVEL); // this avoid a overquick UI fresh when probing done
            printer_state = AC_printer_idle;
            msg_matched = true;
          }
        }

        // If probing fails dont save the mesh raise the probe above the bad point
        if (strcmp_P(msg, MARLIN_msg_probing_failed) == 0) {
          PlayTune(BEEPER_PIN, BeepBeepBeeep, 1);
          injectCommands_P(PSTR("G1 Z50 F500"));
          ChangePageOfTFT(PAGE_CHS_ABNORMAL_LEVELING_SENSOR);
//          SendtoTFTLN(AC_msg_probing_complete);
          printer_state = AC_printer_idle;
          msg_matched = true;
        }

        if (strcmp_P(msg, MARLIN_msg_probe_preheat_start) == 0) {
          ChangePageOfTFT(PAGE_CHS_PROBE_PREHEATING);
        }

        if (strcmp_P(msg, MARLIN_msg_probe_preheat_stop) == 0) {
          ChangePageOfTFT(PAGE_LEVELING);
        }

      } break;

      case AC_printer_printing: {
        if (strcmp_P(msg, MARLIN_msg_reheating) == 0) {
//          SendtoTFTLN(AC_msg_paused); // enable continue button
          ChangePageOfTFT(PAGE_STATUS2);
          msg_matched = true;
        } else if(strcmp_P(msg, MARLIN_msg_media_removed) == 0) {
          msg_matched = true;
          printer_state = AC_printer_stopping_from_media_remove;
        } else {

        #if ACDEBUG(AC_MARLIN)
          SERIAL_ECHOLNPAIR("setFilamentRunoutState: ", __LINE__);
        #endif

          setFilamentRunoutState(false);
        }
      } break;

      case AC_printer_pausing: {
        if (strcmp_P(msg, MARLIN_msg_print_paused) == 0) {

          if(pause_state != AC_paused_filament_lack) {
            ChangePageOfTFT(PAGE_STATUS1);        // show resume
            pause_state = AC_paused_idle;
          }

          printer_state = AC_printer_paused;

          msg_matched = true;
         }
      } break;

      case AC_printer_paused: {
        if (strcmp_P(msg, MARLIN_msg_print_paused) == 0) {

          if(pause_state != AC_paused_filament_lack) {
            ChangePageOfTFT(PAGE_STATUS1);        // show resume
            pause_state = AC_paused_idle;
          }

          printer_state = AC_printer_paused;

          msg_matched = true;
         }
      } break;

      case AC_printer_stopping: {
        if (strcmp_P(msg, MARLIN_msg_print_aborted) == 0) {
          ChangePageOfTFT(PAGE_MAIN);
          printer_state = AC_printer_idle;
          msg_matched = true;
        }
      } break;
      default:
      break;
    }

    // If not matched earlier see if this was a heater message
    if (!msg_matched) {
      if (strcmp_P(msg, MARLIN_msg_extruder_heating) == 0) {
//        SendtoTFTLN(AC_msg_nozzle_heating);
        hotend_state = AC_heater_temp_set;
      }
      else if (strcmp_P(msg, MARLIN_msg_bed_heating) == 0) {
//        SendtoTFTLN(AC_msg_bed_heating);
        hotbed_state = AC_heater_temp_set;
      }
    }
  }

  void DgusTFT::PowerLoss()  {
  // On:  5A A5 05 82 00 82 00 00
  // Off: 5A A5 05 82 00 82 00 64

    uint8_t data_buf[8] = { 0x5A, 0xA5, 0x05, 0x82, 0x00, 0x82, 0x00, 0x00 };

    for(uint8_t i=0; i<8; i++) {
      TFTSer.write(data_buf[i]);
    }
  }

  void DgusTFT::PowerLossRecovery()  {
    printer_state = AC_printer_resuming_from_power_outage; // Play tune to notify user we can recover.
  }

  void DgusTFT::HomingStart() {
    if(!isPrintingFromMedia()) {
        ChangePageOfTFT(PAGE_CHS_HOMING);
    }
  }

  void DgusTFT::HomingComplete() {
    if(lcd_info.language == ExtUI::ENG) {
        if(page_index_last > 120) {
            page_index_last -= 120;
        }
    }

#if ACDEBUG(AC_MARLIN)
      SERIAL_ECHOLNPAIR("HomingComplete, line: ", __LINE__);
      SERIAL_ECHOLNPAIR("page_index_last: ", page_index_last);
#endif

    if(!isPrintingFromMedia()) {
      ChangePageOfTFT(page_index_last);
    }
  }

  void DgusTFT::SendtoTFT(PGM_P str) {  // A helper to print PROGMEN string to the panel
    #if ACDEBUG(AC_SOME)
      serialprintPGM(str);
    #endif
    while (const char c = pgm_read_byte(str++)) TFTSer.print(c);
  }

  void DgusTFT::SendValueToTFT(uint32_t value, uint32_t address) {

    uint8_t data_buf[32] = {0};
    uint8_t data_index = 0;

    uint8_t *p_u8 =  (uint8_t *)(&address)+1 ;

    data_buf[data_index++] = 0x5A;
    data_buf[data_index++] = 0xA5;
    data_buf[data_index++] = 0x05;
    data_buf[data_index++] = 0x82;
    data_buf[data_index++] = *p_u8;
    p_u8--;
    data_buf[data_index++] = *p_u8;
    p_u8 =  (uint8_t *)(&value)+1;
    data_buf[data_index++] = *p_u8;
    p_u8--;
    data_buf[data_index++] = *p_u8;

    for(uint8_t i=0; i<data_index; i++) {
      TFTSer.write(data_buf[i]);
    }
  }

  void DgusTFT::RequestValueFromTFT(uint32_t address) {

    uint8_t data_buf[20] = {0};
    uint8_t data_index = 0;

    uint8_t *p_u8 =  (uint8_t *)(&address)+1 ;

    data_buf[data_index++] = 0x5A;
    data_buf[data_index++] = 0xA5;
    data_buf[data_index++] = 0x04;
    data_buf[data_index++] = 0x83;
    data_buf[data_index++] = *p_u8;
    p_u8--;
    data_buf[data_index++] = *p_u8;
    data_buf[data_index++] = 0x01;

    for(uint8_t i=0; i<data_index; i++) {
      TFTSer.write(data_buf[i]);
    }
  }
  

  void DgusTFT::SendTxtToTFT(const char *pdata, uint32_t address) {

    char data_buf[128] = {0};
    uint8_t data_index = 0;
    uint8_t data_len = 0;

    uint8_t *p_u8 =  (uint8_t *)(&address)+1 ;
    data_len = strlen(pdata);

    data_buf[data_index++] = 0x5A;
    data_buf[data_index++] = 0xA5;
    data_buf[data_index++] = data_len + 5;
    data_buf[data_index++] = 0x82;
    data_buf[data_index++] = *p_u8;
    p_u8--;
    data_buf[data_index++] = *p_u8;

    strncpy(&data_buf[data_index], pdata, data_len);
    data_index += data_len;

    data_buf[data_index++] = 0xFF;
    data_buf[data_index++] = 0xFF;

    for(uint8_t i=0; i<data_index; i++) {
      TFTSer.write(data_buf[i]);
    }
  }

  void DgusTFT::SendColorToTFT(uint32_t color, uint32_t address) {

    uint8_t data_buf[32] = {0};
    uint8_t data_index = 0;

    uint8_t *p_u8 =  (uint8_t *)(&address)+1 ;
    address += 3;

    data_buf[data_index++] = 0x5A;
    data_buf[data_index++] = 0xA5;
    data_buf[data_index++] = 0x05;
    data_buf[data_index++] = 0x82;
    data_buf[data_index++] = *p_u8;
    p_u8--;
    data_buf[data_index++] = *p_u8;
    p_u8 =  (uint8_t *)(&color)+1;
    data_buf[data_index++] = *p_u8;
    p_u8--;
    data_buf[data_index++] = *p_u8;

    for(uint8_t i=0; i<data_index; i++) {
      TFTSer.write(data_buf[i]);
    }
  }

  void DgusTFT::SendReadNumOfTxtToTFT(uint8_t number, uint32_t address) {
    uint8_t data_buf[32] = {0};
    uint8_t data_index = 0;

    uint8_t *p_u8 =  (uint8_t *)(&address)+1 ;

    data_buf[data_index++] = 0x5A;
    data_buf[data_index++] = 0xA5;
    data_buf[data_index++] = 0x04;      //frame length
    data_buf[data_index++] = 0x83;
    data_buf[data_index++] = *p_u8;
    p_u8--;
    data_buf[data_index++] = *p_u8;
    data_buf[data_index++] = number;    //how much bytes to read

    for(uint8_t i=0; i<data_index; i++) {
      TFTSer.write(data_buf[i]);
    }
  }

  void DgusTFT::ChangePageOfTFT(uint32_t page_index) {

#if ACDEBUG(AC_MARLIN)
    SERIAL_ECHOLNPAIR("ChangePageOfTFT: ", page_index);
#endif

    uint8_t data_buf[20] = {0};
    uint8_t data_index = 0;
    uint32_t data_temp = 0;

    if(lcd_info.language == ExtUI::CHS) {
        data_temp = page_index;

    } else if(lcd_info.language == ExtUI::ENG) {
        if(PAGE_OUTAGE_RECOVERY == page_index) {
            data_temp = PAGE_ENG_OUTAGE_RECOVERY;
        } else if(PAGE_CHS_PROBE_PREHEATING == page_index) {
            data_temp = PAGE_ENG_PROBE_PREHEATING;
        } else if(PAGE_CHS_HOMING <= page_index && page_index <= PAGE_ENG_HOMING) {
            data_temp = page_index+12;
        } else if(PAGE_CHS_PROBE_PRECHECK <= page_index && page_index <= PAGE_CHS_PROBE_PRECHECK_FAILED) {
            data_temp = page_index+3;
        } else {
            data_temp = page_index+120;
        }
    }

    uint8_t *p_u8 = (uint8_t *)(&data_temp)+1 ;

    data_buf[data_index++] = 0x5A;
    data_buf[data_index++] = 0xA5;
    data_buf[data_index++] = 0x07;
    data_buf[data_index++] = 0x82;
    data_buf[data_index++] = 0x00;
    data_buf[data_index++] = 0x84;
    data_buf[data_index++] = 0x5A;
    data_buf[data_index++] = 0x01;
    data_buf[data_index++] = *p_u8;
    p_u8--;
    data_buf[data_index++] = *p_u8;

    for(uint8_t i=0; i<data_index; i++) {
      TFTSer.write(data_buf[i]);
    }

    page_index_last_2 = page_index_last;
    page_index_last = page_index_now;
	  page_index_now = data_temp;

#if ACDEBUG(AC_MARLIN)
    SERIAL_ECHOLNPAIR("page_index_last_2: ", page_index_last_2);
    SERIAL_ECHOLNPAIR("page_index_last: ", page_index_last);
    SERIAL_ECHOLNPAIR("page_index_now: ", page_index_now);
#endif
  }

  void DgusTFT::FakeChangePageOfTFT(uint32_t page_index) {

#if ACDEBUG(AC_MARLIN)
    SERIAL_ECHOLNPAIR("ChangePageOfTFT: ", page_index);
#endif

    uint8_t data_buf[20] = {0};
    uint8_t data_index = 0;
    uint32_t data_temp = 0;

    if(lcd_info.language == ExtUI::CHS) {
        data_temp = page_index;
    } else if(lcd_info.language == ExtUI::ENG) {
        if(PAGE_OUTAGE_RECOVERY == page_index) {
            data_temp = PAGE_ENG_OUTAGE_RECOVERY;
        } else if(PAGE_CHS_PROBE_PREHEATING == page_index) {
            data_temp = PAGE_ENG_PROBE_PREHEATING;
        } else if(PAGE_CHS_HOMING <= page_index && page_index <= PAGE_ENG_HOMING) {
            data_temp = page_index+12;
        } else if(PAGE_CHS_PROBE_PRECHECK <= page_index && page_index <= PAGE_CHS_PROBE_PRECHECK_FAILED) {
            data_temp = page_index+3;
        } else {
            data_temp = page_index+120;
        }
    }

    page_index_last_2 = page_index_last;
    page_index_last = page_index_now;
    page_index_now = data_temp;

#if ACDEBUG(AC_MARLIN)
    SERIAL_ECHOLNPAIR("page_index_last_2: ", page_index_last_2);
    SERIAL_ECHOLNPAIR("page_index_last: ", page_index_last);
    SERIAL_ECHOLNPAIR("page_index_now: ", page_index_now);
#endif

  }

  void DgusTFT::LcdAudioSet(ExtUI::audio_t audio) {
  // On:  5A A5 07 82 00 80 5A 00 00 1A
  // Off: 5A A5 07 82 00 80 5A 00 00 12
    uint8_t data_buf[20] = { 0x5A, 0xA5, 0x07, 0x82, 0x00, 0x80, 0x5A, 0x00, 0x00 };

    if(audio == ExtUI::ON) {
        data_buf[9] = 0x1A;
    } else if(audio == ExtUI::OFF) {
        data_buf[9] = 0x12;
    }

    for(uint8_t i=0; i<10; i++) {
      TFTSer.write(data_buf[i]);
    }
  }

  bool DgusTFT::ReadTFTCommand() {

    static uint32_t tft_last_check = 0;

    static uint8_t length = 0, cnt = 0, tft_receive_steps = 0;
	uint8_t data;

	if(0 == TFTSer.available() || data_received) {
	  return false;
	}

	data = TFTSer.read();

    if(tft_receive_steps==0) {
      if(data != 0x5a) {
        cnt = 0;
        length = 0;
        data_index = 0;
        data_received = 0;
        return false;
      }

      tft_last_check = millis();
      while(!TFTSer.available()) {
        TERN_(USE_WATCHDOG, HAL_watchdog_refresh());
        if(millis() - tft_last_check > 500) {
          data_index = 0;
          data_received = 0;
          printf("lcd ReadTFTCommand error\n");
          return false;
        }
      }

      data = TFTSer.read(); 
      //MYSERIAL.write(data );
      if(data == 0xa5) {
        tft_receive_steps=2;
      }
    } else if(tft_receive_steps==2) {  
      length = data;
      tft_receive_steps = 3;
      data_index = 0;
      cnt = 0 ;
    } else if(tft_receive_steps==3) {
      if(data_index >= 63) {
#if ACDEBUG(AC_MARLIN)
        SERIAL_ECHOLNPAIR("lcd uart buff overflow: ", data_index);
#endif
        data_index = 0;
        data_received = 0;
        return false;
      }
      data_buf[data_index++] = data;
      cnt++;
      if(cnt>=length) {   //Receive complete
        tft_receive_steps = 0;
        cnt = 0;
        data_index = 0;
        data_received = 1;
        return true;
      }
    }

    return false;
  }

#if 0
  {

//    SERIAL_ECHOLNPAIR("ReadTFTCommand: ", millis());
//    return -1;

    bool command_ready = false;
    uint8_t data = 0;

    while(TFTSer.available() > 0 && command_len < MAX_CMND_LEN) {
      data = TFTSer.read();
      if(0 == command_len) {
        if
      }
      
      panel_command[command_len] = 
      if (panel_command[command_len] == '\n') {
        command_ready = true;
        break;
      }
      command_len++;
    }

    if (command_ready) {
      panel_command[command_len] = 0x00;
//      #if ACDEBUG(AC_ALL)
        SERIAL_ECHOLNPAIR("< ", panel_command);
//      #endif
      #if ACDEBUG(AC_SOME)
        // Ignore status request commands
        uint8_t req = atoi(&panel_command[1]);
        if (req > 7 && req != 20) {
          SERIAL_ECHOLNPAIR("> ", panel_command);
          SERIAL_ECHOLNPAIR("printer_state:", printer_state);
        }
      #endif
    }
    return command_ready;
  }
#endif

  int8_t DgusTFT::Findcmndpos(const char * buff, char q) {
    int8_t pos = 0;
    do { if (buff[pos] == q) return pos; } while(++pos < MAX_CMND_LEN);
    return -1;
  }

  void DgusTFT::CheckHeaters() {
    static uint8_t faultE0Duration = 0, faultBedDuration = 0;
    float temp = 0;
    static uint32_t time_last = 0;

    if(millis() < time_last + 500) {
        return;
    }

    time_last = millis();

    // if the hotend temp is abnormal, confirm state before signalling panel
    temp = getActualTemp_celsius(E0);
    if(!WITHIN(temp, HEATER_0_MINTEMP, HEATER_0_MAXTEMP)) {
        faultE0Duration++;
        if (faultE0Duration >= AC_HEATER_FAULT_VALIDATION_TIME) {
//          SendtoTFTLN(AC_msg_nozzle_temp_abnormal);
#if ACDEBUG(AC_MARLIN)
          SERIAL_ECHOLNPAIR("Extruder temp abnormal! : ", temp);
#endif
          faultE0Duration = 0;
        }
    }

    temp = getActualTemp_celsius(BED);
    if(!WITHIN(temp, BED_MINTEMP, BED_MAXTEMP)) {
        faultBedDuration++;
        if (faultBedDuration >= AC_HEATER_FAULT_VALIDATION_TIME) {
//            SendtoTFTLN(AC_msg_bed_temp_abnormal);
#if ACDEBUG(AC_MARLIN)
            SERIAL_ECHOLNPAIR("Bed temp abnormal! : ", temp);
#endif
            faultBedDuration = 0;
        }
    }

#if 0
    // Update panel with hotend heater status
    if (hotend_state != AC_heater_temp_reached) {
      if (WITHIN(getActualTemp_celsius(E0) - getTargetTemp_celsius(E0), -1, 1)) {
        SendtoTFTLN(AC_msg_nozzle_heating_done);
        hotend_state = AC_heater_temp_reached;
      }
    }

    // Update panel with bed heater status
    if (hotbed_state != AC_heater_temp_reached) {
      if (WITHIN(getActualTemp_celsius(BED) - getTargetTemp_celsius(BED), -0.5, 0.5)) {
        SendtoTFTLN(AC_msg_bed_heating_done);
        hotbed_state = AC_heater_temp_reached;
      }
    }
#endif
  }

  void DgusTFT::SendFileList(int8_t startindex) {
    // Respond to panel request for 4 files starting at index
    #if ACDEBUG(AC_INFO)
      SERIAL_ECHOLNPAIR("## SendFileList ## ", startindex);
    #endif
    filenavigator.getFiles(startindex);
  }

  void DgusTFT::SelectFile() {
    strncpy(selectedfile, panel_command + 4, command_len - 4);
    selectedfile[command_len - 5] = '\0';
    #if ACDEBUG(AC_FILE)
      SERIAL_ECHOLNPAIR_F(" Selected File: ",selectedfile);
    #endif
    switch (selectedfile[0]) {
      case '/':   // Valid file selected
//        SendtoTFTLN(AC_msg_sd_file_open_success);
        break;

      case '<':   // .. (go up folder level)
        filenavigator.upDIR();
//        SendtoTFTLN(AC_msg_sd_file_open_failed);
        SendFileList( 0 );
        break;
      default:   // enter sub folder
        filenavigator.changeDIR(selectedfile);
//        SendtoTFTLN(AC_msg_sd_file_open_failed);
        SendFileList( 0 );
        break;
    }
  }

  void DgusTFT::InjectCommandandWait(PGM_P cmd) {
    //injectCommands_P(cmnd); queue.enqueue_now_P(cmd);
    //SERIAL_ECHOLN(PSTR("Inject>"));
  }

  void DgusTFT::ProcessPanelRequest() {
    unsigned char * p_u8 ;
    unsigned char i,j;
    unsigned int control_index  = 0;
    unsigned int control_value;
    unsigned int temp;
    char str_buf[20];

    if(data_received) {
    
      data_received = 0;
  	
    if(0x83 == data_buf[0]) {
  	    p_u8 =  (unsigned char *)(&control_index) ;//get control address
  		*p_u8 = data_buf[2];
  		p_u8++;
  		*p_u8 = data_buf[1] ;
  
        if((control_index&0xF000) == KEY_ADDRESS)// is KEY
        {
          key_index = control_index;
           p_u8 =  (unsigned char *)(&key_value) ;//get key value
          *p_u8 = data_buf[5];
           p_u8++;
          *p_u8 = data_buf[4];
        }
  	    else if(control_index==TXT_HOTEND_TARGET||control_index==TXT_ADJUST_HOTEND)//hotend target temp
  	  	{
           p_u8 =  (unsigned char *)(&control_value) ;//get  value
          *p_u8 = data_buf[5];
           p_u8++;
          *p_u8 = data_buf[4];

          temp=constrain((uint16_t)control_value, 0, HEATER_0_MAXTEMP);
          setTargetTemp_celsius(temp, E0);
  	    }
  	    else if(control_index==TXT_BED_TARGET||control_index==TXT_ADJUST_BED)//bed target temp
  	  	{
            p_u8 =  (unsigned char *)(&control_value) ;//get value
   		 *p_u8 = data_buf[5];
   		  p_u8++;
   		 *p_u8 = data_buf[4];

  		  temp=constrain((uint16_t)control_value, 0, BED_MAXTEMP);
          setTargetTemp_celsius(temp, BED);
           //sprintf(str_buf,"%u/%u",(uint16_t)thermalManager.degBed(),(uint16_t)control_value);
  	     //SendTxtToTFT(str_buf,TXT_PRINT_BED );
  	    }
  	  	else if(control_index==TXT_FAN_SPEED_TARGET)//fan speed
  	  	{
  		   p_u8 =  (unsigned char *)(&control_value) ;//get value
  		  *p_u8 = data_buf[5];
  		   p_u8++;
  		  *p_u8 = data_buf[4];
  		  
  		    temp=constrain((uint16_t)control_value,0,100); 
  		    SendValueToTFT((uint16_t)temp,TXT_FAN_SPEED_NOW);
  			SendValueToTFT((uint16_t)temp,TXT_FAN_SPEED_TARGET);
            setTargetFan_percent(temp, FAN0);
  	    }
  		else if(control_index==TXT_PRINT_SPEED_TARGET||control_index==TXT_ADJUST_SPEED)//prinf speed
  	  	{
  		   p_u8 =  (unsigned char *)(&control_value) ;//get  value
  		  *p_u8 = data_buf[5];
  		   p_u8++;
  		  *p_u8 = data_buf[4];

  		   float feedrate = constrain((uint16_t)control_value,40,999);
//  		   feedrate_percentage=constrain(control_value,40,999);
  		   sprintf(str_buf,"%u",(uint16_t)feedrate);
  	       SendTxtToTFT(str_buf, TXT_PRINT_SPEED);
           SendValueToTFT((uint16_t)feedrate, TXT_PRINT_SPEED_NOW);
  		   SendValueToTFT((uint16_t)feedrate, TXT_PRINT_SPEED_TARGET);
  		   setFeedrate_percent(feedrate);
  	    }

  	    else if(control_index == TXT_PREHEAT_HOTEND_INPUT)
  	    {
            p_u8 =  (unsigned char *)(&control_value) ;//get  value
           *p_u8 = data_buf[5];
            p_u8++;
           *p_u8 = data_buf[4];

            temp=constrain((uint16_t)control_value, 0, HEATER_0_MAXTEMP);
            setTargetTemp_celsius(temp, E0);
  	    }

  	    else if(control_index == TXT_PREHEAT_BED_INPUT)
  	    {
            p_u8 =  (unsigned char *)(&control_value) ;//get  value
           *p_u8 = data_buf[5];
            p_u8++;
           *p_u8 = data_buf[4];

            temp=constrain((uint16_t)control_value, 0, BED_MAXTEMP);
            setTargetTemp_celsius(temp, BED);
  	    }

  	    else if(control_index == REG_LCD_READY)
  	    {
           p_u8 =  (unsigned char *)(&control_value) ;//get  value
           *p_u8 = data_buf[5];
           p_u8++;
           *p_u8 = data_buf[4];
           p_u8++;
           *p_u8 = data_buf[3];
           if((control_value&0x00FFFFFF) == 0x010072) { // startup last gif

             LcdAudioSet(lcd_info.audio);

             SendValueToTFT(2, ADDRESS_MOVE_DISTANCE);

#if ENABLED(CASE_LIGHT_ENABLE)
             SendValueToTFT(getCaseLightState(), ADDRESS_SYSTEM_LED_STATUS);
             SendValueToTFT(getCaseLightState(), ADDRESS_PRINT_SETTING_LED_STATUS);
#endif

             if(AC_printer_resuming_from_power_outage == printer_state) {

                char filename[64] = { '\0' };
                ChangePageOfTFT(PAGE_OUTAGE_RECOVERY);
                card.getLongPath(filename, recovery.info.sd_filename);
                SendTxtToTFT(filename, TXT_OUTAGE_RECOVERY_FILE);
                SendTxtToTFT(ui8tostr3rj(recovery.info.print_progress), TXT_OUTAGE_RECOVERY_PROGRESS);
                PlayTune(BEEPER_PIN, SOS, 1);
             } else {
                ChangePageOfTFT(PAGE_MAIN);
             }

           } else if((control_value&0x00FFFFFF) == 0x010000) {  // startup first gif
             PlayTune(BEEPER_PIN, Anycubic_PowerOn, 1);         // take 3500 ms
           }
  	    }

       /* else if((control_index&0xF000)==0x2000)// is TXT ADDRESS
        	{
        	  tft_txt_index = control_index;
        	  j=0;
         	  for(i=4; ;i++)
            {
              tft_txt_buf[j] = data_buf[i];
  			if(tft_txt_buf[j]==0xFF)
  			{
  			  tft_txt_buf[j]= 0 ;
  			  break;
  			}
  			j++;
         	  }
        	}
  		*/
  	 }
  	 else  if(data_buf[0]==0x82)
  	 {
  	 // send_cmd_to_pc(cmd ,start );
  	 }
    }
  
  
  
  }
#if 0
  {
    // Break these up into logical blocks // as its easier to navigate than one huge switch case!
    int8_t req = atoi(&panel_command[1]);

    // Information requests A0 - A8 and A33
    if (req <= 8 || req == 33) PanelInfo(req);

    // Simple Actions A9 - A28
    else if ( req <= 28) PanelAction(req);

    // Process Initiation
    else if (req <= 34) PanelProcess(req);

    else SendtoTFTLN();
  }
#endif

  void DgusTFT::page1_handle(void) {

	static millis_t flash_time = 0;
	char str_buf[20];

	switch (key_value) {
	  case 0:
      break;

	  case 1:   // main page, print
	  {

        lcd_txtbox_page = 0;

        if(lcd_txtbox_index) {
          SendColorToTFT(COLOR_BLUE, TXT_DISCRIBE_0+0x30*(lcd_txtbox_index-1));
          lcd_txtbox_index = 0;
        }

	    ChangePageOfTFT(PAGE_FILE);
	    SendFileList(0);
	  }
	  break;

	  case 2:   // tool
	  {
		ChangePageOfTFT(PAGE_TOOL);

#if ENABLED(CASE_LIGHT_ENABLE)
		SendValueToTFT(getCaseLightState(), ADDRESS_SYSTEM_LED_STATUS);
#endif

	  }
	  break;

	  case 3:   // prepare
       ChangePageOfTFT(PAGE_PREPARE);
	  break;

	  case 4:   // system
      {
        if(lcd_info.language == ExtUI::ENG) {
          if(lcd_info.audio == ExtUI::ON) {
            ChangePageOfTFT(11);    // PAGE_SYSTEM_ENG_AUDIO_ON - 120
          } else if(lcd_info.audio == ExtUI::OFF) {
            ChangePageOfTFT(50);   // PAGE_SYSTEM_ENG_AUDIO_OFF - 120
          }
        } else if(lcd_info.language == ExtUI::CHS) {
          if(lcd_info.audio == ExtUI::ON) {
            ChangePageOfTFT(PAGE_SYSTEM_CHS_AUDIO_ON);
          } else if(lcd_info.audio == ExtUI::OFF) {
            ChangePageOfTFT(PAGE_SYSTEM_CHS_AUDIO_OFF);
          }
        }
      }
	  break;
	}

#if 0
	if(message_index < 30) {
	 SendTxtToTFT(p_mesage[message_index], TXT_MAIN_MESSAGE);
	 message_index = 30;
	}
#endif

	if(millis() < (flash_time + 1500) ) {
	  return;
	}

	flash_time = millis();
  }


  void DgusTFT::page2_handle(void) {

    char file_index = 0;

	switch (key_value) {
	case 0:
      break;

	case 1: // return
	  {
	   ChangePageOfTFT(PAGE_MAIN);
	   SendColorToTFT(COLOR_BLUE, TXT_DISCRIBE_0+0x30*(lcd_txtbox_index-1));
	  }
	  break;

	case 2: // page up
	{
	  if(lcd_txtbox_page > 0) {
	    lcd_txtbox_page--;

        SendColorToTFT(COLOR_BLUE, TXT_DISCRIBE_0+0x30*(lcd_txtbox_index-1));
	    lcd_txtbox_index = 0;

	    SendFileList(lcd_txtbox_page*5);
	  }
	  break;
	}

	case 3: // page down
    {
      if((lcd_txtbox_page+1) * 5 < filenavigator.getFileNum()) {
        lcd_txtbox_page++;
     	
        SendColorToTFT(COLOR_BLUE, TXT_DISCRIBE_0+0x30*(lcd_txtbox_index-1));
        lcd_txtbox_index = 0;

     	SendFileList(lcd_txtbox_page*5);
      }
      break;
    }

	  case 4: // page refresh
	  {

	    if (!isMediaInserted()) {
	      safe_delay(500);
	    }

	    filenavigator.reset();

        lcd_txtbox_page = 0;
        if(lcd_txtbox_index) {
          SendColorToTFT(COLOR_BLUE, TXT_DISCRIBE_0+0x30*(lcd_txtbox_index-1));
          lcd_txtbox_index = 0;
        }

	    SendFileList(lcd_txtbox_index);
	  }
	  break;

	  case 5: // resume of outage(last power off)
#if ACDEBUG(AC_MARLIN)
	    SERIAL_ECHOLNPAIR("printer_state: ", printer_state);
#endif
        if(lcd_txtbox_index > 0 && lcd_txtbox_index  < 6) {   // 1~5

          if(filenavigator.filelist.seek(lcd_txtbox_page*5+(lcd_txtbox_index-1))) {
  
            SendColorToTFT(COLOR_BLUE, TXT_DISCRIBE_0+0x30*(lcd_txtbox_index-1));

#if ENABLED(CASE_LIGHT_ENABLE)
            setCaseLightState(1);
#endif

            char str_buf[20];
            strncpy_P(str_buf, filenavigator.filelist.longFilename(), 17);
            str_buf[17] = '\0';
            SendTxtToTFT(str_buf, TXT_PRINT_NAME);

            if (printer_state == AC_printer_resuming_from_power_outage) {
              // Need to home here to restore the Z position
//              injectCommands_P(AC_cmnd_power_loss_recovery);
//              SERIAL_ECHOLNPAIR("start resumeing from power outage: ", AC_cmnd_power_loss_recovery);
              ChangePageOfTFT(PAGE_STATUS2);    // show pause
              injectCommands_P(PSTR("M1000"));  // home and start recovery
            }
          }
        }
	    break;

	  case 6:   // start print
          if(lcd_txtbox_index > 0 && lcd_txtbox_index  < 6) {   // 1~5

            if(filenavigator.filelist.seek(lcd_txtbox_page*5+(lcd_txtbox_index-1))) {

#if 0
              SERIAL_ECHOLNPAIR("start print: ", lcd_txtbox_page*5+(lcd_txtbox_index-1));
              SERIAL_ECHOLNPAIR("start print: ", filenavigator.filelist.shortFilename());
              SERIAL_ECHOLNPAIR("start print: ", filenavigator.filelist.longFilename());
#endif

              SendColorToTFT(COLOR_BLUE, TXT_DISCRIBE_0+0x30*(lcd_txtbox_index-1));

              // Allows printer to restart the job if we dont want to recover
              if (printer_state == AC_printer_resuming_from_power_outage) {
                injectCommands_P(PSTR("M1000 C")); // Cancel recovery
                printer_state = AC_printer_idle;
              }

#if ENABLED(CASE_LIGHT_ENABLE)
              setCaseLightState(1);
#endif

              printFile(filenavigator.filelist.shortFilename());

              char str_buf[20];
              strncpy_P(str_buf, filenavigator.filelist.longFilename(), 17);
              str_buf[17] = '\0';
              SendTxtToTFT(str_buf, TXT_PRINT_NAME);

              sprintf(str_buf, "%d", (uint16_t)getFeedrate_percent());
              SendTxtToTFT(str_buf, TXT_PRINT_SPEED);

              sprintf(str_buf, "%d", (uint16_t)getProgress_percent());
              SendTxtToTFT(str_buf, TXT_PRINT_PROGRESS);

              uint32_t time = 0;
              sprintf(str_buf, "%s H ", utostr3(time/60));
              sprintf(str_buf+strlen(str_buf), "%s M", utostr3(time%60));
              SendTxtToTFT(str_buf, TXT_PRINT_TIME);

              ChangePageOfTFT(PAGE_STATUS2);
            }
          }
	  break;

	  case 7:   // txtbox 1 click
	  case 8:   // txtbox 2 click
	  case 9:   // txtbox 3 click
	  case 10:  // txtbox 4 click
	  case 11:  // txtbox 5 click
	  {
        static uint8_t lcd_txtbox_index_last = 0;

        if((lcd_txtbox_page*5 + key_value - 6) <= filenavigator.getFileNum()) {
            lcd_txtbox_index = key_value - 6;
        } else {
            break;
        }

#if ACDEBUG(AC_MARLIN)
        printf("getFileNum: %d\n", filenavigator.getFileNum());
        printf("file_index: %d\n", file_index);
        printf("lcd_txtbox_page: %d\n", lcd_txtbox_page);
        printf("lcd_txtbox_index: %d\n", lcd_txtbox_index);
        printf("lcd_txtbox_index_last: %d\n", lcd_txtbox_index_last);
#endif

        // lcd_txtbox_page 0~...
        // lcd_txtbox_index 1~5
	    file_index = lcd_txtbox_page*5 + (lcd_txtbox_index-1);
	    if(file_index < filenavigator.getFileNum()) {

          SendColorToTFT(COLOR_RED, TXT_DISCRIBE_0 + 0x30*(lcd_txtbox_index-1));

          if(lcd_txtbox_index_last && lcd_txtbox_index_last != lcd_txtbox_index) {   // 1~5
            SendColorToTFT(COLOR_BLUE, TXT_DISCRIBE_0 + 0x30*(lcd_txtbox_index_last-1));
          }
          lcd_txtbox_index_last = lcd_txtbox_index;
	    }
	    break;
	  }
    }
  }

    
    void DgusTFT::page3_handle(void)
    {
        
        static millis_t flash_time = 0;
        char str_buf[20];
        static int16_t speed_last = -1;
        static uint8_t progress_last = 0;
		static uint16_t feedrate_last = 0;

        switch (key_value)
        {
          case 0:

          break;

          case 1: // return
          {
            if(isPrintingFromMedia() == false) {  //only is idle status can return 
              ChangePageOfTFT(PAGE_FILE);
            }
          }
          break;

          case 2: // resume print
#if ACDEBUG(AC_MARLIN)
            SERIAL_ECHOLNPAIR("printer_state: ", printer_state);
            SERIAL_ECHOLNPAIR("pause_state: ", pause_state);
#endif
            if(pause_state == AC_paused_idle || pause_state == AC_paused_filament_lack ||
               printer_state == AC_printer_resuming_from_power_outage) {

              printer_state = AC_printer_idle;
              pause_state = AC_paused_idle;
              resumePrint();
              ChangePageOfTFT(PAGE_STATUS2);    // show pasue print
              flash_time = millis();

            } else {
              setUserConfirmed();
            }
            break;

          case 3: // print stop
            if(isPrintingFromMedia()) {    
              ChangePageOfTFT(PAGE_STOP_CONF);
            }
             break;

          case 4: // print change param
            ChangePageOfTFT(PAGE_ADJUST);
#if ENABLED(CASE_LIGHT_ENABLE)
            SendValueToTFT(getCaseLightState(), ADDRESS_PRINT_SETTING_LED_STATUS);
#endif
            SendValueToTFT((uint16_t)getTargetTemp_celsius(E0), TXT_ADJUST_HOTEND);
            SendValueToTFT((uint16_t)getTargetTemp_celsius(BED), TXT_ADJUST_BED);
            feedrate_last = (uint16_t)getFeedrate_percent();
            SendValueToTFT(feedrate_last, TXT_ADJUST_SPEED);
            flash_time = millis();
            break;
        }

        if(millis() < (flash_time +1500) ) {
          return;
        }
        flash_time=millis();

        if(feedrate_last != (uint16_t)getFeedrate_percent()) {
          feedrate_last = (uint16_t)getFeedrate_percent();
          sprintf(str_buf, "%d", feedrate_last);
          SendTxtToTFT(str_buf, TXT_PRINT_SPEED);
        }

        if(progress_last!=getProgress_percent()) {
          progress_last=getProgress_percent();
          sprintf(str_buf, "%u", progress_last);
          SendTxtToTFT(str_buf, TXT_PRINT_PROGRESS);
        }

        // Get Printing Time
        uint32_t time = getProgress_seconds_elapsed() / 60;
        sprintf(str_buf, "%s H ", utostr3(time/60));
        sprintf(str_buf+strlen(str_buf), "%s M", utostr3(time%60));
        SendTxtToTFT(str_buf, TXT_PRINT_TIME);
    }

    void DgusTFT::page4_handle(void)
    {
        
        static millis_t flash_time = 0;
        char str_buf[20];
        static uint8_t progress_last = 0;
		static uint16_t feedrate_last = 0;
    
        switch (key_value)
        {
        case 0:
          break;

        case 1: // return
        {
          if(isPrintingFromMedia() == false) {
  //            if(card.sdprinting==false)//only is idle status can return
              ChangePageOfTFT(PAGE_FILE);
          }
        }
        break;

        case 2:  // print pause
          if(isPrintingFromMedia()) {
            pausePrint();
            printer_state = AC_printer_pausing;
            pause_state = AC_paused_idle;
            ChangePageOfTFT(PAGE_WAIT_PAUSE);
//            injectCommands_P(PSTR("M108"));     // stop waiting temperature M109
          }
          break;

        case 3: // print stop
          if(isPrintingFromMedia()) {    
              ChangePageOfTFT(PAGE_STOP_CONF);
          }
          break;

        case 4: // print settings
          ChangePageOfTFT(PAGE_ADJUST);
#if ENABLED(CASE_LIGHT_ENABLE)
          SendValueToTFT(getCaseLightState(), ADDRESS_PRINT_SETTING_LED_STATUS);
#endif
          SendValueToTFT((uint16_t)getTargetTemp_celsius(E0), TXT_ADJUST_HOTEND);
          SendValueToTFT((uint16_t)getTargetTemp_celsius(BED), TXT_ADJUST_BED);
          feedrate_last = (uint16_t)getFeedrate_percent();
          SendValueToTFT(feedrate_last, TXT_ADJUST_SPEED);   
          SendValueToTFT((uint16_t)getActualFan_percent(FAN0), TXT_FAN_SPEED_TARGET);
          SendTxtToTFT(ftostr(getZOffset_mm()), TXT_LEVEL_OFFSET);

          break;
        }

        if(millis() < (flash_time +1500)) {
          return;
        }
        flash_time=millis();

        if(feedrate_last != (uint16_t)getFeedrate_percent()) {
          feedrate_last = (uint16_t)getFeedrate_percent();
          sprintf(str_buf, "%d", feedrate_last);
          SendTxtToTFT(str_buf, TXT_PRINT_SPEED);
        }


        if(progress_last!=getProgress_percent())
        {
          sprintf(str_buf, "%d", getProgress_percent());
          SendTxtToTFT(str_buf, TXT_PRINT_PROGRESS);
          progress_last=getProgress_percent();
        }

        uint32_t time = getProgress_seconds_elapsed() / 60;
        sprintf(str_buf, "%s H ", utostr3(time/60));
        sprintf(str_buf+strlen(str_buf), "%s M", utostr3(time%60));
        SendTxtToTFT(str_buf, TXT_PRINT_TIME);
    }

    void DgusTFT::page5_handle(void)          // print settings
    {
      char str_buf[10];
      float z_off;
      int16_t steps;
      static bool z_change = false;

      switch (key_value) {
        case 0:
      break;

      case 1: // return
        if(AC_printer_printing == printer_state) {
          ChangePageOfTFT(PAGE_STATUS2);  // show pause
        } else if(AC_printer_paused == printer_state) {
          ChangePageOfTFT(PAGE_STATUS1);  // show print
        }
      break;

      case 2: // -
      {
        z_off = getZOffset_mm();

        if(z_off <= -5) {
            return ;
        }

        steps = mmToWholeSteps(-BABYSTEP_MULTIPLICATOR_Z, Z);
        babystepAxis_steps(steps, Z);

        z_off -= BABYSTEP_MULTIPLICATOR_Z;
        setZOffset_mm(z_off);

        sprintf(str_buf, "%.2f", getZOffset_mm());
        SendTxtToTFT(str_buf, TXT_LEVEL_OFFSET);

        z_change = true;
      }
      break;

      case 3: // +
      {
        z_off = getZOffset_mm();

        if(z_off >= 5) {
            return ;
        }

        steps = mmToWholeSteps(BABYSTEP_MULTIPLICATOR_Z, Z);
        babystepAxis_steps(steps, Z);

        z_off += BABYSTEP_MULTIPLICATOR_Z;
        setZOffset_mm(z_off);

        sprintf(str_buf, "%.2f", getZOffset_mm());
        SendTxtToTFT(str_buf, TXT_LEVEL_OFFSET);

        z_change = true;
      }
      break;

        case 4: // light control
#if ENABLED(CASE_LIGHT_ENABLE)
          if(getCaseLightState()) {
            SendValueToTFT(0, ADDRESS_PRINT_SETTING_LED_STATUS);
            setCaseLightState(0);
          } else {
            SendValueToTFT(1, ADDRESS_PRINT_SETTING_LED_STATUS);
            setCaseLightState(1);
          }
#endif
          break;

        case 5:
          ChangePageOfTFT(PAGE_DONE);
          break;

        case 6:
  
          break;

        case 7:
          RequestValueFromTFT(TXT_ADJUST_BED);
          RequestValueFromTFT(TXT_ADJUST_SPEED);
          RequestValueFromTFT(TXT_ADJUST_HOTEND);
          RequestValueFromTFT(TXT_FAN_SPEED_TARGET);

          if(z_change) {
            z_change = false;
            injectCommands_P(PSTR("M500"));
          }

          if(AC_printer_printing == printer_state) {
            ChangePageOfTFT(PAGE_STATUS2);  // show pause
          } else if(AC_printer_paused == printer_state) {
            ChangePageOfTFT(PAGE_STATUS1);  // show print
          }

          break; 
      }
    }

    void DgusTFT::page6_handle(void)
    {
        switch (key_value)
        {
          case 0:
          break;

          case 1:
          break;
        }
    }

    void DgusTFT::page7_handle(void) // tools
    {
     unsigned char str_buf[20];
     unsigned int temp;
        switch (key_value)
        {
        
          case 0:
          break;

          case 1:   // return
          {
           ChangePageOfTFT(PAGE_MAIN);
          }
          break;

          case 2:
          {
             ChangePageOfTFT(PAGE_MOVE);
          }
          break;
          
          case 3:   // set temperature
          ChangePageOfTFT(PAGE_TEMP);
          SendValueToTFT((uint16_t)getActualTemp_celsius(E0), TXT_HOTNED_NOW);
          SendValueToTFT((uint16_t)getTargetTemp_celsius(E0), TXT_HOTEND_TARGET);
          SendValueToTFT((uint16_t)getActualTemp_celsius(BED), TXT_BED_NOW);
          SendValueToTFT((uint16_t)getTargetTemp_celsius(BED), TXT_BED_TARGET);

          break;
          
          case 4:
           ChangePageOfTFT(PAGE_SPEED);
           SendValueToTFT((uint16_t)getActualFan_percent(FAN0), TXT_FAN_SPEED_NOW);
           SendValueToTFT((uint16_t)getTargetFan_percent(FAN0), TXT_FAN_SPEED_TARGET);
           SendValueToTFT((uint16_t)getFeedrate_percent(), TXT_PRINT_SPEED_NOW);
           SendValueToTFT((uint16_t)getFeedrate_percent(), TXT_PRINT_SPEED_TARGET);
          break;

          case 5:   // turn off the xyz motor
            if(!isMoving()) {
                disable_all_steppers();
                set_all_unhomed();
            }
          break;

          case 6:   // light control
#if ENABLED(CASE_LIGHT_ENABLE)
            if(getCaseLightState()) {
              setCaseLightState(0);
              SendValueToTFT(0, ADDRESS_SYSTEM_LED_STATUS);
            } else {
              setCaseLightState(1);
              SendValueToTFT(1, ADDRESS_SYSTEM_LED_STATUS);
            }
#endif
          break;
        }
    }

    void DgusTFT::page8_handle(void)
    {
     static uint16_t movespeed = 50;
     static float move_dis = 1.0f;

     if(key_value == 2 || key_value == 4 ||
        key_value == 6 || key_value == 8 ||
        key_value == 10 || key_value == 12 && !isMoving()) {

        if(getAxisPosition_mm(Z) < 0) {
            setAxisPosition_mm(0, Z, 8);
        }
     }

     //  if(!planner.movesplanned())return;
        switch (key_value) {
          case 0:
            break;
          
          case 1://return
            ChangePageOfTFT(PAGE_TOOL);
            break;

          case 5:
            if(!isMoving()) {
              injectCommands_P(PSTR("G28 X"));
            }
            break;

          case 9:
            if(!isMoving()) {
              injectCommands_P(PSTR("G28 Y"));
            }
            break;

          case 13:
            if(!isMoving()) {
              if(axis_is_trusted(X_AXIS) && axis_is_trusted(Y_AXIS)) {
                injectCommands_P(PSTR("G28 Z"));
              } else {
                injectCommands_P(PSTR("G28"));
              }
            }
            break;

          case 17:
            if(!isMoving()) {
              injectCommands_P(PSTR("G28"));
            }
            break;

          case 2:   // X-
            if(!isMoving()) {
              setAxisPosition_mm(getAxisPosition_mm(X) - move_dis, X, 50);
            }
          break;

          case 4:   // X+
            if(!isMoving()) {
              setAxisPosition_mm(getAxisPosition_mm(X) + move_dis, X, 50);
            }
            break;

          case 6:   // Y+
            if(!isMoving()) {
              setAxisPosition_mm(getAxisPosition_mm(Y) + move_dis, Y, 50);
            }
            break;

          case 8:   // Y-
            if(!isMoving()) {
              setAxisPosition_mm(getAxisPosition_mm(Y) - move_dis, Y, 50);
            }
            break;

          case 10:  // Z-
            if(!isMoving()) {
              setAxisPosition_mm(getAxisPosition_mm(Z) - move_dis, Z, 8);
            }
            break;

          case 12:  // Z+
            if(!isMoving()) {
              setAxisPosition_mm(getAxisPosition_mm(Z) + move_dis, Z, 8);
            }
            break;

          case 3:
            move_dis = 0.1f ;
            SendValueToTFT(1, ADDRESS_MOVE_DISTANCE);
            break;

          case 7:
            move_dis = 1.0f ;
            SendValueToTFT(2, ADDRESS_MOVE_DISTANCE);
            break;

          case 11:
            move_dis = 10.0f ;
            SendValueToTFT(3, ADDRESS_MOVE_DISTANCE);
            break;

          case 14:
            movespeed = 3000;//SERIAL_PROTOCOLLN("3000");
            break;

          case 15:
            movespeed = 2000;//SERIAL_PROTOCOLLN("2000");
            break;

          case 16:
            movespeed = 1000;//SERIAL_PROTOCOLLN("1000");
            break;
         }
    }

    void DgusTFT::page9_handle(void)
    {
       static millis_t flash_time = 0;
     
        switch (key_value)
        {
         case 0:
         break;
         
          case 1://return
          {
            ChangePageOfTFT(PAGE_TOOL);
          }
          break;
          case 2:
          {
            
          }
          break;
          case 3:
    
          break;
          case 4:
          break;
          
          case 5:
          break;

          case 6: // cooling
            setTargetTemp_celsius(0, E0); 
            setTargetTemp_celsius(0, BED);
            ChangePageOfTFT(PAGE_TOOL);
          break;

          case 7: // send target temp
            RequestValueFromTFT(TXT_HOTEND_TARGET);
            RequestValueFromTFT(TXT_BED_TARGET);
            ChangePageOfTFT(PAGE_TOOL);
          break;
    }

        if(millis() < (flash_time +1500) ) {
          return;
        }
        flash_time=millis();
    
        SendValueToTFT( (uint16_t)getActualTemp_celsius(E0), TXT_HOTNED_NOW);
        SendValueToTFT( (uint16_t)getActualTemp_celsius(BED), TXT_BED_NOW);
    }
    
    void DgusTFT::page10_handle(void)
    {
        static millis_t flash_time = 0;
        switch (key_value)
        {
                case 0:
         break;
          case 1:   // return
          {
              ChangePageOfTFT(PAGE_TOOL);
          }
          break;
          
          case 2:
          {

          }
          break;
          
          case 3:
          break;
          
          case 4:
          break;
          
          case 5:
          break;

          case 6:   // ok
            RequestValueFromTFT(TXT_FAN_SPEED_TARGET);
            RequestValueFromTFT(TXT_PRINT_SPEED_TARGET);
            ChangePageOfTFT(PAGE_TOOL);
          break;
    }
    
      if(millis() < (flash_time +1500)) {
        return;
      }
      flash_time=millis();
  
       SendValueToTFT((uint16_t)getActualFan_percent(FAN0), TXT_FAN_SPEED_NOW);
       SendValueToTFT((uint16_t)getFeedrate_percent(), TXT_PRINT_SPEED_NOW);
    }

    void DgusTFT::page11_handle(void)
    {
        switch (key_value)
        {
          case 0:
          break;

          case 1:   // return 
          {
            ChangePageOfTFT(PAGE_MAIN);
            if(lcd_info_back.language != lcd_info.language ||
               lcd_info_back.audio    != lcd_info.audio) {

               lcd_info_back.language = lcd_info.language;
               lcd_info_back.audio    = lcd_info.audio;

               injectCommands_P(PSTR("M500"));
            }
          }
          break;

          case 2:   // language
          {
            if(lcd_info.language == ExtUI::CHS) {
                lcd_info.language = ExtUI::ENG;
                if(lcd_info.audio == ExtUI::ON) {
                    ChangePageOfTFT(11);  // PAGE_SYSTEM_ENG_AUDIO_ON-120
                } else if(lcd_info.audio == ExtUI::OFF) {
                    ChangePageOfTFT(50);  // PAGE_SYSTEM_ENG_AUDIO_OFF-120
                }
            } else if(lcd_info.language == ExtUI::ENG) {
                lcd_info.language = ExtUI::CHS;
                if(lcd_info.audio == ExtUI::ON) {
                    ChangePageOfTFT(PAGE_SYSTEM_CHS_AUDIO_ON);
                } else if(lcd_info.audio == ExtUI::OFF) {
                    ChangePageOfTFT(PAGE_SYSTEM_CHS_AUDIO_OFF);
                }
            }
          }
          break;

          case 3:
          break;

          case 4:   // audio
          {
            if(lcd_info.audio == ExtUI::ON) {
                lcd_info.audio = ExtUI::OFF;
                if(lcd_info.language == ExtUI::CHS) {
                  ChangePageOfTFT(PAGE_SYSTEM_CHS_AUDIO_OFF);
                } else if(lcd_info.language == ExtUI::ENG) {
                  ChangePageOfTFT(50);    // PAGE_SYSTEM_ENG_AUDIO_OFF - 120
                }
            } else if(lcd_info.audio == ExtUI::OFF) {
                lcd_info.audio = ExtUI::ON;
                if(lcd_info.language == ExtUI::CHS) {
                  ChangePageOfTFT(PAGE_SYSTEM_CHS_AUDIO_ON);
                } else if(lcd_info.language == ExtUI::ENG) {
                  ChangePageOfTFT(11);    // PAGE_SYSTEM_ENG_AUDIO_ON - 120
                }
            }

            LcdAudioSet(lcd_info.audio);
          }
          break;

          case 5:   // about

            SendTxtToTFT(DEVICE_NAME, TXT_ABOUT_DEVICE_NAME);
            SendTxtToTFT(FIRMWARE_VER, TXT_ABOUT_FW_VERSION);
            SendTxtToTFT(BUILD_VOLUME, TXT_ABOUT_PRINT_VOLUMN);
            SendTxtToTFT(TECH_SUPPORT, TXT_ABOUT_TECH_SUPPORT);

            ChangePageOfTFT(PAGE_ABOUT);
            break;

          case 6:
            ChangePageOfTFT(PAGE_RECORD);
            break;
        }
    }
    
    void DgusTFT::page12_handle(void)
    {
        switch (key_value)
            {
                    case 0:
             break;
              case 1://return
              {
               ChangePageOfTFT(PAGE_SYSTEM_CHS_AUDIO_ON);
              }
              break;
        
            }
    
    }

    void DgusTFT::page13_handle(void)
    {
        switch (key_value)
        {
          case 0:
          break;

          case 1://return
          {
              if(lcd_info.language == ExtUI::ENG) {
                if(lcd_info.audio == ExtUI::ON) {
                  ChangePageOfTFT(11);    // PAGE_SYSTEM_ENG_AUDIO_ON - 120
                } else if(lcd_info.audio == ExtUI::OFF) {
                  ChangePageOfTFT(50);   // PAGE_SYSTEM_ENG_AUDIO_OFF - 120
                }
              } else if(lcd_info.language == ExtUI::CHS) {
                if(lcd_info.audio == ExtUI::ON) {
                  ChangePageOfTFT(PAGE_SYSTEM_CHS_AUDIO_ON);
                } else if(lcd_info.audio == ExtUI::OFF) {
                  ChangePageOfTFT(PAGE_SYSTEM_CHS_AUDIO_OFF);
                }
              }
          }
          break;

          case 2:
          {
            
          }
          break;
        }
    }

    void DgusTFT::page14_handle(void)
    {
        switch (key_value)
            {
                    case 0:
             break;
              case 1://return
              {

              }
              break;
              case 2:
              {
                
              }
              break;

              case 3:
        
              break;

              case 4:
        
              break;
            }
    
    
    }
    
        
        void DgusTFT::page15_handle(void) 
        {
            char str_buf[16];

            switch (key_value)
            {
              case 0:
              break;

              case 1://return
              {
                ChangePageOfTFT(PAGE_MAIN);

              }
              break;

              case 2:
              {
                ChangePageOfTFT(PAGE_PreLEVEL);
              }
              break;

              case 3:
                ChangePageOfTFT(PAGE_PREHEAT);
                sprintf(str_buf,"%u/%u",(uint16_t)getActualTemp_celsius(E0), (uint16_t)getTargetTemp_celsius(E0));
                SendTxtToTFT(str_buf, TXT_PREHEAT_HOTEND);
                sprintf(str_buf,"%u/%u",(uint16_t)getActualTemp_celsius(BED), (uint16_t)getTargetTemp_celsius(BED));
                SendTxtToTFT(str_buf, TXT_PREHEAT_BED);
              break;

              case 4:
              {
                str_buf[0] = 0;
                sprintf(str_buf,"%u/%u", (uint16_t)getActualTemp_celsius(E0), (uint16_t)getTargetTemp_celsius(E0));
                SendTxtToTFT(str_buf, TXT_FILAMENT_TEMP);
                ChangePageOfTFT(PAGE_FILAMENT);
              }
              break;
            }
        }

        void DgusTFT::page16_handle(void)//AUTO LEVELING
        {
            switch (key_value)
                {
                        case 0:
             break;
                  case 1://return
                  {
                   ChangePageOfTFT(PAGE_PREPARE);
                  }
                  break;

                  case 2:
                    if(!isPrinting())
                    {
//                      setAxisPosition_mm(10.0, Z, 5);
#ifdef NOZZLE_AS_PROBE
                      ChangePageOfTFT(PAGE_CHS_PROBE_PRECHECK);
#else   // FIX_MOUNTED_PROBE
                      ChangePageOfTFT(PAGE_LEVEL_ENSURE);
#endif
                    }
                  break;

                  case 3:
                  char str_buf[10];
                  str_buf[0]=0;
                  strcat(str_buf, ftostr(getZOffset_mm()));
                  SendTxtToTFT(str_buf, TXT_LEVEL_OFFSET);
                  ChangePageOfTFT(PAGE_LEVEL_ADVANCE);
                  break;
    
                  case 4:
                   ChangePageOfTFT(PAGE_AUTO_OFFSET);
                  break;
                  
                }
        }

        void DgusTFT::page17_handle(void)
        {
            char str_buf[10];
            float z_off;
            int16_t steps;
            static bool z_change = false;

            switch (key_value)
            {
              case 0:
              break;

              case 1://return
              {
                ChangePageOfTFT(PAGE_PreLEVEL);
              }
              break;

              case 2:  // -
              {
                z_off = getZOffset_mm();

                if(z_off <= -5) {
                    return ;
                }

                steps = mmToWholeSteps(-BABYSTEP_MULTIPLICATOR_Z, Z);
                babystepAxis_steps(steps, Z);

                z_off -= BABYSTEP_MULTIPLICATOR_Z;
                setZOffset_mm(z_off);

                sprintf(str_buf, "%.2f", getZOffset_mm());
                SendTxtToTFT(str_buf, TXT_LEVEL_OFFSET);

                z_change = true;
              }
              break;

              case 3: // +
              {
                z_off = getZOffset_mm();

                if(z_off >= 5) {
                    return ;
                }

                steps = mmToWholeSteps(BABYSTEP_MULTIPLICATOR_Z, Z);
                babystepAxis_steps(steps, Z);

                z_off += BABYSTEP_MULTIPLICATOR_Z;
                setZOffset_mm(z_off);

                sprintf(str_buf, "%.2f", getZOffset_mm());
                SendTxtToTFT(str_buf, TXT_LEVEL_OFFSET);

                z_change = true;
              }
              break;

              case 4:

#if ACDEBUG(AC_MARLIN)
                SERIAL_ECHOLNPAIR("z off: ", ftostr(getZOffset_mm()));
#endif

                if(z_change) {
                  z_change = false;
                  injectCommands_P(PSTR("M500"));
                }

                ChangePageOfTFT(PAGE_PREPARE);
              break;
            }
        }

        void DgusTFT::page18_handle(void) //preheat
        {
            static millis_t flash_time = 0;
            char str_buf[16];

            switch (key_value)
            {
            case 0:
                break;

              case 1: // return
              {
                ChangePageOfTFT(PAGE_PREPARE);

              }
              break;

              case 2: // PLA
              { 
                setTargetTemp_celsius(190, E0);
                setTargetTemp_celsius(60, BED);
                ChangePageOfTFT(PAGE_PREHEAT);
              }
              break;

              case 3: // ABS
              { 
                setTargetTemp_celsius(240, E0);
                setTargetTemp_celsius(100, BED);
                ChangePageOfTFT(PAGE_PREHEAT);
              }

            }

            if(millis() < (flash_time +1500) ) {
              return;
            }
            flash_time=millis();

            sprintf(str_buf,"%u/%u",(uint16_t)getActualTemp_celsius(E0), (uint16_t)getTargetTemp_celsius(E0));
            SendTxtToTFT(str_buf, TXT_PREHEAT_HOTEND);
            sprintf(str_buf,"%u/%u",(uint16_t)getActualTemp_celsius(BED), (uint16_t)getTargetTemp_celsius(BED));
            SendTxtToTFT(str_buf, TXT_PREHEAT_BED);
        }

        void DgusTFT::page19_handle(void)   // Filament
        {
            typedef enum {
                FILA_NO_ACT = 0x00,
                FILA_IN     = 0x01,
                FILA_OUT    = 0x02,
            } filament_cmd_t;

            char str_buf[20];
            static bool fil_need_in = 0;
            static filament_cmd_t filament_cmd = FILA_NO_ACT;
            static millis_t flash_time = 0;
            switch (key_value)
            {
              case 0:
              break;

              case 1:   // return
              {
                filament_cmd = FILA_NO_ACT;
                ChangePageOfTFT(PAGE_PREPARE);
              }
              break;

              case 2:   // Filament in
              if(getActualTemp_celsius(E0)<220) {
                filament_cmd = FILA_NO_ACT;
                ChangePageOfTFT(PAGE_FILAMENT_HEAT);
              } else {
                if(getTargetTemp_celsius(E0) < 230) {
                  setTargetTemp_celsius(230, E0);
                }
                filament_cmd = FILA_IN;
              }
              break;

              case 3:   // filament out
              if(getActualTemp_celsius(E0)<220) {
                filament_cmd = FILA_NO_ACT;
                ChangePageOfTFT(PAGE_FILAMENT_HEAT);
              } else {
                if(getTargetTemp_celsius(E0) < 230) {
                  setTargetTemp_celsius(230, E0);
                }
                if(filament_cmd == FILA_NO_ACT) {
                  injectCommands_P(AC_cmnd_manual_unload_filament_first_in);
                }
                filament_cmd = FILA_OUT;
              }
              break;

              case 4:   // stop
                filament_cmd = FILA_NO_ACT;
              break;
              
            }

            if(millis() < (flash_time +1000) ) {
              return;
            }
            flash_time=millis();

            sprintf(str_buf,"%u/%u",(uint16_t)getActualTemp_celsius(E0), (uint16_t)getTargetTemp_celsius(E0));
            SendTxtToTFT(str_buf, TXT_FILAMENT_TEMP);

            if(!isPrinting()) {

              if(filament_cmd==FILA_IN) {
                if(canMove(E0) && !commandsInQueue()) {
                  injectCommands_P(AC_cmnd_manual_load_filament);
                }
              } else if(filament_cmd==FILA_OUT) {
                if(canMove(E0) && !commandsInQueue()) {
                  injectCommands_P(AC_cmnd_manual_unload_filament);
                }
              }
            }
        }

        void DgusTFT::page20_handle(void)   // confirm
        {
            static millis_t flash_time = 0;
            switch (key_value)
            {
              case 0:
              break;
              case 1://return
              {
                ChangePageOfTFT(page_index_last);
              }
              break;

            }
            if(millis() < (flash_time +1000) )return;
            flash_time=millis();
        
        }

        void DgusTFT::page21_handle(void)
        {
            static millis_t flash_time = 0;
            switch (key_value)
            {
              case 0:
              break;

              case 1://return
              {
                ChangePageOfTFT(page_index_last);
              }
              break;

              case 2:
              {
              }
              break;
            }

            if(millis() < (flash_time +1000) )return;
             flash_time=millis();
        }
    
        void DgusTFT::page22_handle(void)   // print finish
        {
            static millis_t flash_time = 0;
            switch (key_value)
            {
              case 0:
              break;

              case 1:  // OK to finish
              {
#if ENABLED(CASE_LIGHT_ENABLE)
                setCaseLightState(0);
#endif
                ChangePageOfTFT(PAGE_MAIN);
                setFeedrate_percent(100);           // resume print speed to 100
                clearProgress_seconds_elapsed();    // clear print time elapsed
              }
              break;

              case 2:
              {
              }
              break;
            }
                if(millis() < (flash_time +1000) )return;
                flash_time=millis();
            }

            void DgusTFT::page23_handle(void)
            {
                static millis_t flash_time = 0;
                switch (key_value)
                {
                  case 0:
                  break;

                  case 1:
                  {
                   ChangePageOfTFT(page_index_last);
                  }
                  break;

                  case 2:
                  {
                    ChangePageOfTFT(page_index_last);
                  }
                  break;
                }

             if(millis() < (flash_time +1000) )return;
               flash_time=millis();
            
            }
    
            
            void DgusTFT::page24_handle(void)
            {static millis_t flash_time = 0;
                switch (key_value)
                    {
            
                      case 0:
                      break;
                      case 1://return
                      {
                       ChangePageOfTFT(page_index_last);
                      }
                      break;
                      case 2:
                      {
                         ChangePageOfTFT(page_index_last);
                      }
                      break;
            
                    }
                    if(millis() < (flash_time +1000) )return;
                  flash_time=millis();
            }

            void DgusTFT::page25_handle(void)   // lack filament
            {
               static millis_t flash_time = 0;
                switch (key_value)
                {
                  case 0:
                  break;

                  case 1: // return
                  {
#if ACDEBUG(AC_MARLIN)
                    SERIAL_ECHOLNPAIR("printer_state: ", printer_state);
                    SERIAL_ECHOLNPAIR("pause_state: ", pause_state);
#endif
                    if(AC_printer_printing == printer_state) {
                      ChangePageOfTFT(PAGE_STATUS2);  // show pause
                    } else if(AC_printer_paused == printer_state) {
//                      injectCommands_P(PSTR("M108"));
                      ChangePageOfTFT(PAGE_STATUS1);  // show resume
                    }
                  }
                  break;
                }
                if(millis() < (flash_time +1000) )return;
                flash_time=millis();
            }

            void DgusTFT::page26_handle(void)
            {
            static millis_t flash_time = 0;
                switch (key_value)
                    {
            
                      case 0:
                      break;
                      case 1://return
                      {
                         ChangePageOfTFT(page_index_last);
            
                      }
                      break;
                      case 2:
                      {
                        
                      }
                      break;
            
                    }
    
    
              if(millis() < (flash_time +1000) )return;
              flash_time=millis();
            
            }
    
    
        void DgusTFT::page27_handle(void)
        {
        static millis_t flash_time = 0;
            switch (key_value)
            {
    
              case 0:
              break;

              case 1:   // print stop confirmed
              {
                if (isPrintingFromMedia()) {
                  printer_state = AC_printer_stopping;
                  stopPrint();
                  message_index = 6;
                  ChangePageOfTFT(PAGE_MAIN);
                } else {
                  if (printer_state == AC_printer_resuming_from_power_outage)
                    injectCommands_P(PSTR("M1000 C")); // Cancel recovery
                  printer_state = AC_printer_idle;
                }

                setFeedrate_percent(100);           // resume print speed to 100
                clearProgress_seconds_elapsed();    // clear print time elapsed

              }
              break;

              case 2:   // return
              {
                if(AC_printer_printing == printer_state) {
                  ChangePageOfTFT(PAGE_STATUS2);  // show pause
                } else if(AC_printer_paused == printer_state) {
                  ChangePageOfTFT(PAGE_STATUS1);  // show print
                }
              }
            }

          if(millis() < (flash_time +1000) )return;
          flash_time=millis();
        }
    
        void DgusTFT::page28_handle(void)
        {
        static millis_t flash_time = 0;
            switch (key_value)
                {
        
                  case 0:
                  break;
                  case 1://return
                  {
                     ChangePageOfTFT(page_index_last);
        
                  }
                  break;
                  case 2:
                  {
                    
                  }
                  break;
        
                }
    
    
          if(millis() < (flash_time +1000) )return;
          flash_time=millis();
        
        }
    
        void DgusTFT::page29_handle(void)
        {
        static millis_t flash_time = 0;
            switch (key_value)
            {
              case 0:
              break;

              case 1://return
              {
#if ENABLED(CASE_LIGHT_ENABLE)
                setCaseLightState(0);
#endif
                ChangePageOfTFT(PAGE_MAIN);
              }
              break;

              case 2:
              {
                
              }
              break;
    
            }

          if(millis() < (flash_time +1000) )return;
          flash_time=millis();
        
        }

        void DgusTFT::page30_handle(void)   // Auto heat filament
        {
            static millis_t flash_time = 0;
            switch (key_value)
            {
              case 0:
              break;

              case 1:   // return
              {
                setTargetTemp_celsius(230, E0);
                ChangePageOfTFT(PAGE_FILAMENT);
              }
              break;
            }

          if(millis() < (flash_time +1000) )return;
          flash_time=millis();
        }

        void DgusTFT::page31_handle(void)
        {
            static millis_t flash_time = 0;
            switch (key_value)
            {
              case 0:
              break;

              case 1://return
              {
              }
              break;

              case 2:
              {
              }
              break;
            }

          if(millis() < (flash_time +1000) )return;
          flash_time=millis();
        }

        void DgusTFT::page32_handle(void)
        {
          static millis_t flash_time = 0;
    
             if(millis() < (flash_time +1000) )return;
             flash_time=millis();
        
        }

        void DgusTFT::page33_handle(void)
        {
          static millis_t flash_time = 0;
          switch (key_value)
          {
            case 0:
            break;

            case 1:   // auto leveling start
            {
              #if PROBING_NOZZLE_TEMP || LEVELING_NOZZLE_TEMP
                setTargetTemp_celsius(LEVELING_NOZZLE_TEMP, E0);
              #endif

              #if PROBING_BED_TEMP || LEVELING_BED_TEMP
                setTargetTemp_celsius(LEVELING_BED_TEMP, BED);
              #endif

              injectCommands_P(PSTR("M851 Z0\nG28\nG29"));
              printer_state = AC_printer_probing;
              ChangePageOfTFT(PAGE_LEVELING);
            }
            break;

            case 2:
            {
              ChangePageOfTFT(PAGE_PreLEVEL);
            }
            break;
          }

          if(millis() < (flash_time +1000)){
            return;
          }
          flash_time = millis();
        }

        void DgusTFT::page34_handle(void)
        {
          char str_buf[20];
          static millis_t flash_time = 0;

          if(millis() < (flash_time + 1500) )return;
          flash_time=millis();

          if(pop_up_index==25)
          {
            pop_up_index=100;
            ChangePageOfTFT(PAGE_PreLEVEL);
          }
        
        }
    
    void DgusTFT::page115_handle(void)
    {
        static millis_t flash_time = 0;
            switch (key_value)
            {
    
              case 0:
              break;
              case 1:
              {            
               ChangePageOfTFT(PAGE_PreLEVEL);
              }
              break;
              
              case 2:
              {
                injectCommands_P(PSTR("M1024 S3"));      // -1
              //  char value[20]
                //sprintf_P(value,PSTR("G1 Z%iF%i")); enqueue_and_echo_command_now(value); }                                     
                    
              }
              break;
              
              case 3:
              {
                injectCommands_P(PSTR("M1024 S4"));      // 1
              }
              
              break;
              
              case 4:
              {            
                injectCommands_P(PSTR("M1024 S1"));      // -0.1
              }
              break;
              case 5:
              {
                injectCommands_P(PSTR("M1024 S2"));      // 0.1
              }
              break;
    
              case 6:
              {
                injectCommands_P(PSTR("M1024 S0"));      // prepare, move x y to center
              }
              break;
              case 7:
              {            
                injectCommands_P(PSTR("M1024 S5"));      // 0.1
              }
              break;
            }
    
    
        if(millis() < (flash_time +1000) )return;
        flash_time=millis();
    
        
    }

    void DgusTFT::page117_handle(void)  // Page CHS Mute handler
    {
        switch (key_value)
        {
          case 0:
          break;

          case 1:
          {
            ChangePageOfTFT(PAGE_MAIN);
            if(lcd_info_back.language != lcd_info.language ||
               lcd_info_back.audio    != lcd_info.audio) {

               lcd_info_back.language = lcd_info.language;
               lcd_info_back.audio    = lcd_info.audio;

               injectCommands_P(PSTR("M500"));
            }
          }
          break;

          case 2:   // language
          {
            if(lcd_info.language == ExtUI::CHS) {
                lcd_info.language = ExtUI::ENG;
                if(lcd_info.audio == ExtUI::ON) {
                    ChangePageOfTFT(11);  // PAGE_SYSTEM_ENG_AUDIO_ON-120
                } else if(lcd_info.audio == ExtUI::OFF) {
                    ChangePageOfTFT(50);  // PAGE_SYSTEM_ENG_AUDIO_OFF-120
                }
            } else if(lcd_info.language == ExtUI::ENG) {
                lcd_info.language = ExtUI::CHS;
                if(lcd_info.audio == ExtUI::ON) {
                    ChangePageOfTFT(PAGE_SYSTEM_CHS_AUDIO_ON);
                } else if(lcd_info.audio == ExtUI::OFF) {
                    ChangePageOfTFT(PAGE_SYSTEM_CHS_AUDIO_OFF);
                }
            }
          }
          break;

          case 3:
          break;

          case 4:   // audio
          {
            if(lcd_info.audio == ExtUI::ON) {
                lcd_info.audio = ExtUI::OFF;
                ChangePageOfTFT(PAGE_SYSTEM_CHS_AUDIO_OFF);
            } else if(lcd_info.audio == ExtUI::OFF) {
                lcd_info.audio = ExtUI::ON;
                ChangePageOfTFT(PAGE_SYSTEM_CHS_AUDIO_ON);
            }

            LcdAudioSet(lcd_info.audio);
          }
          break;

          case 5:   // about

            SendTxtToTFT(DEVICE_NAME, TXT_ABOUT_DEVICE_NAME);
            SendTxtToTFT(FIRMWARE_VER, TXT_ABOUT_FW_VERSION);
            SendTxtToTFT(BUILD_VOLUME, TXT_ABOUT_PRINT_VOLUMN);
            SendTxtToTFT(TECH_SUPPORT, TXT_ABOUT_TECH_SUPPORT);

            ChangePageOfTFT(PAGE_ABOUT);
            break;

          case 6:
            ChangePageOfTFT(PAGE_RECORD);
            break;
        }
    }

    void DgusTFT::page170_handle(void)  // ENG Mute handler
    {
        switch (key_value)
        {
          case 0:
          break;

          case 1:
          {
            ChangePageOfTFT(PAGE_MAIN);
            if(lcd_info_back.language != lcd_info.language ||
               lcd_info_back.audio    != lcd_info.audio) {

               lcd_info_back.language = lcd_info.language;
               lcd_info_back.audio    = lcd_info.audio;

               injectCommands_P(PSTR("M500"));
            }
          }
          break;

          case 2:   // language
          {
            if(lcd_info.language == ExtUI::CHS) {
                lcd_info.language = ExtUI::ENG;
                if(lcd_info.audio == ExtUI::ON) {
                    ChangePageOfTFT(11);  // PAGE_SYSTEM_ENG_AUDIO_ON-120
                } else if(lcd_info.audio == ExtUI::OFF) {
                    ChangePageOfTFT(50);  // PAGE_SYSTEM_ENG_AUDIO_OFF-120
                }
            } else if(lcd_info.language == ExtUI::ENG) {
                lcd_info.language = ExtUI::CHS;
                if(lcd_info.audio == ExtUI::ON) {
                    ChangePageOfTFT(PAGE_SYSTEM_CHS_AUDIO_ON);
                } else if(lcd_info.audio == ExtUI::OFF) {
                    ChangePageOfTFT(PAGE_SYSTEM_CHS_AUDIO_OFF);
                }
            }
          }
          break;

          case 3:
          break;

          case 4:   // audio
          {
            if(lcd_info.audio == ExtUI::ON) {
                lcd_info.audio = ExtUI::OFF;
                ChangePageOfTFT(PAGE_SYSTEM_CHS_AUDIO_OFF);
            } else if(lcd_info.audio == ExtUI::OFF) {
                lcd_info.audio = ExtUI::ON;
                ChangePageOfTFT(PAGE_SYSTEM_CHS_AUDIO_ON);
            }

            LcdAudioSet(lcd_info.audio);
          }
          break;

          case 5:   // about


            SendTxtToTFT(DEVICE_NAME, TXT_ABOUT_DEVICE_NAME);
            SendTxtToTFT(FIRMWARE_VER, TXT_ABOUT_FW_VERSION);
            SendTxtToTFT(BUILD_VOLUME, TXT_ABOUT_PRINT_VOLUMN);
            SendTxtToTFT(TECH_SUPPORT, TXT_ABOUT_TECH_SUPPORT);

            ChangePageOfTFT(PAGE_ABOUT);
            break;

          case 6:
            ChangePageOfTFT(PAGE_RECORD);
            break;
        }
    }

    void DgusTFT::page171_handle(void)  // CHS power outage resume handler
    {
        char str_buf[20] = { '\0' };

        switch (key_value)
        {
          case 0:
          break;

          case 1:   // resume
          {
            char filename[64] = { '\0' };
            ChangePageOfTFT(PAGE_OUTAGE_RECOVERY);
            card.getLongPath(filename, recovery.info.sd_filename);
            filename[17] = '\0';
            SendTxtToTFT(filename, TXT_OUTAGE_RECOVERY_FILE);

            sprintf(str_buf, "%u", (uint16_t)getFeedrate_percent());
            SendTxtToTFT(str_buf, TXT_PRINT_SPEED);

            sprintf(str_buf, "%u", (uint16_t)getProgress_percent());
            SendTxtToTFT(str_buf, TXT_PRINT_PROGRESS);

            ChangePageOfTFT(PAGE_STATUS2);               // show pause
#ifdef CASE_LIGHT_ENABLE
            injectCommands_P(PSTR("M355 S1\nM1000"));    // case light on, home and start recovery
#else
            injectCommands_P(PSTR("M1000"));
#endif
          }
          break;

          case 2:   // cancel
          {
            printer_state = AC_printer_idle;
            ChangePageOfTFT(PAGE_MAIN);

#ifdef CASE_LIGHT_ENABLE
            injectCommands_P(PSTR("M355 S0\nM1000 C"));  // cancel recovery
#else
            injectCommands_P(PSTR("M1000 C"));
#endif
          }
          break;
        }
    }

    void DgusTFT::page173_handle(void)  // ENG power outage resume handler
    {
        char str_buf[20] = { '\0' };

        switch (key_value)
        {
          case 0:
          break;

          case 1:   // resume
          {
            char filename[64] = { '\0' };
            ChangePageOfTFT(PAGE_OUTAGE_RECOVERY);
            card.getLongPath(filename, recovery.info.sd_filename);
            filename[17] = '\0';
            SendTxtToTFT(filename, TXT_OUTAGE_RECOVERY_FILE);
  
            sprintf(str_buf, "%u", (uint16_t)getFeedrate_percent());
            SendTxtToTFT(str_buf, TXT_PRINT_SPEED);
  
            sprintf(str_buf, "%u", (uint16_t)getProgress_percent());
            SendTxtToTFT(str_buf, TXT_PRINT_PROGRESS);

            ChangePageOfTFT(PAGE_STATUS2);      // show pause

#ifdef CASE_LIGHT_ENABLE
            injectCommands_P(PSTR("M355 S1\nM1000"));    // case light on, home and start recovery
#else
            injectCommands_P(PSTR("M1000"));
#endif
          }
          break;

          case 2:   // cancel
          {
            printer_state = AC_printer_idle;
            ChangePageOfTFT(PAGE_MAIN);

#ifdef CASE_LIGHT_ENABLE
            injectCommands_P(PSTR("M355 S0\nM1000 C"));  // cancel recovery
#else
            injectCommands_P(PSTR("M1000 C"));
#endif
          }
          break;
        }
    }

    void DgusTFT::page175_handle(void)     // CHS probe preheating handler
    {
        static millis_t flash_time = 0;
        char str_buf[16];

        if(millis() < (flash_time +1500) ) {
          return;
        }
        flash_time=millis();
    }

    void DgusTFT::page176_handle(void)     // ENG probe preheating handler
    {
        static millis_t flash_time = 0;
        char str_buf[16];

        if(millis() < (flash_time +1500) ) {
          return;
        }
        flash_time=millis();
    }

    void DgusTFT::page177_to_198_handle(void)
    {
        switch (key_value)
        {
          case 1:   // return

#if ACDEBUG(AC_MARLIN)
            SERIAL_ECHOLNPAIR("page_index_now: ", page_index_now);
            SERIAL_ECHOLNPAIR("page_index_last: ", page_index_last);
            SERIAL_ECHOLNPAIR("page_index_last_2: ", page_index_last_2);
#endif

            if((PAGE_CHS_ABNORMAL_X_ENDSTOP <= page_index_now && 
               page_index_now <= PAGE_CHS_ABNORMAL_Z_ENDSTOP) ||
               (PAGE_ENG_ABNORMAL_X_ENDSTOP <= page_index_now &&
               page_index_now <= PAGE_ENG_ABNORMAL_Z_ENDSTOP)) {
  
              if(lcd_info.language == ExtUI::ENG) {
                if(page_index_last_2 > 120) {
                    page_index_last_2 -= 120;
                }

                if(page_index_last > 120) {
                    page_index_last -= 120;
                }
              }

              if(PAGE_STATUS1 == page_index_last_2 || PAGE_STATUS2 == page_index_last_2 || 
                 PAGE_PRINT_FINISH == page_index_last) {
                ChangePageOfTFT(PAGE_MAIN);
              } else {
                ChangePageOfTFT(page_index_last_2);
              }

            } else {

              if(lcd_info.language == ExtUI::ENG) {
                if(page_index_last > 120) {
                    page_index_last -= 120;
                }
              }
              ChangePageOfTFT(page_index_last);
            }

            disable_all_steppers();

          break;

          default:
            break;
        }
    }

#if 0
    void DgusTFT::page178_to_181_190_to_193_handle(void)    // temperature abnormal
    {
        switch (key_value)
        {
          case 1:   // return

            SERIAL_ECHOLNPAIR("page_index_now: ", page_index_now);
            SERIAL_ECHOLNPAIR("page_index_last: ", page_index_last);
            SERIAL_ECHOLNPAIR("page_index_last_2: ", page_index_last_2);

            if(isPrinting() || isPrintingPaused() || isPrintingFromMedia()) {
              printer_state = AC_printer_stopping;
              stopPrint();
              ChangePageOfTFT(PAGE_MAIN);
            } else {
              ChangePageOfTFT(page_index_last);
            }

            onSurviveInKilled();

          break;

          default:
            break;
        }
    }
#endif

    void DgusTFT::page199_to_200_handle(void)
    {
        switch (key_value)
        {
          case 1:   // return

#if ACDEBUG(AC_MARLIN)
            SERIAL_ECHOLNPAIR("page_index_now: ", page_index_now);
            SERIAL_ECHOLNPAIR("page_index_last: ", page_index_last);
            SERIAL_ECHOLNPAIR("page_index_last_2: ", page_index_last_2);
#endif
            ChangePageOfTFT(PAGE_PreLEVEL);

          break;

          default:
            break;
        }
    }
    
    void DgusTFT::page201_handle(void)  // probe precheck
    {
        static millis_t probe_check_time = 0;
        static millis_t temperature_time = 0;
        static uint8_t probe_check_counter = 0;
        static uint8_t probe_state_last = 0;
        static bool probe_tare_flag = 0;
        char str_buf[16];

        if(!probe_tare_flag) {
            ProbeTare();

            delay(100);

            if(getProbeState()) {   // triggered too early
                probe_check_counter = 0;
                probe_tare_flag = 0;
                ChangePageOfTFT(PAGE_CHS_PROBE_PRECHECK_FAILED);
            }
            probe_tare_flag = 1;
        }

        switch (key_value) {
        case 1:   // cancel

            probe_check_counter = 0;
            probe_tare_flag = 0;
            ChangePageOfTFT(PAGE_PreLEVEL);
            break;

        default:
            break;
        }

        if(millis() >= (probe_check_time + 300) ) {

            probe_check_time=millis();

            if(!probe_state_last && getProbeState()) {

                probe_check_counter = 0;
                probe_tare_flag = 0;
                ChangePageOfTFT(PAGE_CHS_PROBE_PRECHECK_OK);
            }

            probe_state_last = getProbeState();

            if(probe_check_counter++ >=  200) {    // waiting for 1 min
                probe_check_counter = 0;
                probe_tare_flag = 0;
                ChangePageOfTFT(PAGE_CHS_PROBE_PRECHECK_FAILED);
            }
        }

        if(millis() >= (temperature_time + 1500) ) {
            temperature_time=millis();
        }
    }

    void DgusTFT::page202_handle(void)  // probe precheck ok
    {
        static millis_t flash_time = 0;
        static millis_t probe_check_counter = 0;
        static uint8_t probe_state_last = 0;
        char str_buf[16];

        delay(3000);

        injectCommands_P(PSTR("M851 Z0\nG28\nG29"));
        printer_state = AC_printer_probing;
        ChangePageOfTFT(PAGE_LEVELING);
    }

    void DgusTFT::page203_handle(void)    // probe precheck failed
    {
        static millis_t flash_time = 0;
        static millis_t probe_check_counter = 0;
        static uint8_t probe_state_last = 0;
        char str_buf[16];

        if(millis() < (flash_time + 1500) ) {
            return;
        }
        flash_time=millis();
    }

    void DgusTFT::pop_up_manager(void)
    {
     char str_buf[20];

       switch (pop_up_index)
       {
        case 10:    //T0 error
        if(page_index_now!=PAGE_ABNORMAL)
        ChangePageOfTFT(PAGE_ABNORMAL);
         pop_up_index = 100;
        break;

        case 15:    // filament lack
        if(page_index_now!=PAGE_FILAMENT_LACK)
        ChangePageOfTFT(PAGE_FILAMENT_LACK);
         pop_up_index = 100;
        break;
        
        case 16:    // stop wait
        //ChangePageOfTFT(PAGE_WAIT_STOP);
         pop_up_index = 100;
        break;
        
        case 18:    //
         ChangePageOfTFT(PAGE_STATUS1);
         pop_up_index = 100;
        break;

        case 23:    //
        if(page_index_now!=PAGE_FILAMENT_LACK) {
          ChangePageOfTFT(PAGE_FILAMENT_LACK);
        }
        pop_up_index = 100;
        break;

        case 24://
        {
           uint32_t time = getProgress_seconds_elapsed() / 60;
           sprintf(str_buf, "%s H ", utostr3(time/60));
           sprintf(str_buf+strlen(str_buf), "%s M", utostr3(time%60));
           SendTxtToTFT(str_buf, TXT_FINISH_TIME);
           ChangePageOfTFT(PAGE_PRINT_FINISH);
           pop_up_index = 100;
        }
        break;

        case 25://LEVEL DONE
         ChangePageOfTFT(PAGE_PreLEVEL);
         pop_up_index = 100;
        break;
       }
    }


  
} // namespace

#endif // ANYCUBIC_LCD_DGUS
