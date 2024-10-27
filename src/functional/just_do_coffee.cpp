/* 09:32 15/03/2023 - change triggering comment */
#include "just_do_coffee.h"
#include "../lcd/lcd.h"

extern unsigned long steamTime;
// inline static float TEMP_DELTA(float d) { return (d*DELTA_RANGE); }
inline static float TEMP_DELTA(float d, const SensorState &currentState) {
  return (
    d * (currentState.pumpFlow < 1.f
      ? currentState.pumpFlow / 7.f
      : currentState.pumpFlow / 5.f
    )
  );
}

void justDoCoffee(const eepromValues_t &runningCfg, const SensorState &currentState, const bool brewActive) {
  lcdTargetState((int)HEATING::MODE_brew); // setting the target mode to "brew temp"
  float brewTempSetPoint = ACTIVE_PROFILE(runningCfg).setpoint + runningCfg.offsetTemp;
  float sensorTemperature = currentState.temperature + runningCfg.offsetTemp;

  if (brewActive) { //if brewState == true
    if(sensorTemperature <= brewTempSetPoint - 10.f) {
      setBoilerOn();
    } else if (sensorTemperature <= brewTempSetPoint) {
        pulseHeaters(75);
      } else if (sensorTemperature > brewTempSetPoint) {
        setBoilerOff();
    }
  } else { //if brewState == false
    if (sensorTemperature <= ((float)brewTempSetPoint - 10.f)) {
      setBoilerOn();
    } else {
      if (sensorTemperature <= ((float)brewTempSetPoint - 5.f)) {
        pulseHeaters(75);
      } else if (sensorTemperature < ((float)brewTempSetPoint)) {
        pulseHeaters(50);
      } else {
        setBoilerOff();
      }
    }
  }
  if (brewActive || !currentState.brewSwitchState) { // keep steam boiler supply valve open while steaming/descale only
    setSteamValveRelayOff();
  }
  setSteamBoilerRelayOff();
}

void pulseHeaters(const int heaterPercent) {
  static uint32_t heaterWave;
  static bool heaterState;
  if (!heaterState && (millis() > (heaterWave + (1000 - heaterPercent*10)))) {
    setBoilerOn();
    heaterState=!heaterState;
    heaterWave=millis();
  } else if (heaterState && (millis() > (heaterWave + heaterPercent*10))) {
    setBoilerOff();
    heaterState=!heaterState;
    heaterWave=millis();
  }
}

//#############################################################################################
//################################____STEAM_POWER_CONTROL____##################################
//#############################################################################################
void steamCtrl(const eepromValues_t &runningCfg, SensorState &currentState) {
  currentState.steamSwitchState ? lcdTargetState((int)HEATING::MODE_steam) : lcdTargetState((int)HEATING::MODE_brew); // setting the steam/hot water target temp
  // steam temp control, needs to be aggressive to keep steam pressure acceptable
  float steamTempSetPoint = runningCfg.steamSetPoint + runningCfg.offsetTemp;
  float sensorTemperature = currentState.temperature + runningCfg.offsetTemp;

  if (sensorTemperature > steamTempSetPoint + 10) {
    setBoilerOff();
    setPumpToRawValue(runningCfg.brewDivider);
  } else if (sensorTemperature > steamTempSetPoint) {
      pulseHeaters(runningCfg.mainDivider);
    } else if (sensorTemperature < steamTempSetPoint) {
      setBoilerOn();
    }
  /*In case steam is forgotten ON for more than 15 min*/
  if (currentState.smoothedPressure > passiveSteamPressure_) {
    currentState.isSteamForgottenON = millis() - steamTime >= STEAM_TIMEOUT;
  } else steamTime = millis();
}

/*Water mode and all that*/
void hotWaterMode(const SensorState &currentState) {
  setPumpToRawValue(50);
  setBoilerOn();
  if (currentState.temperature < MAX_WATER_TEMP) setBoilerOn();
  else setBoilerOff();
}
