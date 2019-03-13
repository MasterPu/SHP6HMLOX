#define JSONCONFIG_IP                    "ip"
#define JSONCONFIG_NETMASK               "netmask"
#define JSONCONFIG_GW                    "gw"
#define JSONCONFIG_CCUIP                 "ccuip"
#define JSONCONFIG_SHP6                  "shp6"
#define JSONCONFIG_LOXUDPPORT            "loxudpport"
#define JSONCONFIG_LOXUSERNAME           "loxusername"
#define JSONCONFIG_LOXPASSWORD           "loxpassword"
#define JSONCONFIG_HMPOWERVARIABLE         "powervariable"
#define JSONCONFIG_HMENERGYCOUNTERVARIABLE "ecvariable"
#define JSONCONFIG_MEASUREINTERVAL         "measureinterval"
#define JSONCONFIG_BACKENDTYPE             "backendtype"
#define JSONCONFIG_RESTOREOLDSTATE         "restoreOldState"
#define JSONCONFIG_LEDDISABLED             "ledDisabled"
#define JSONCONFIG_HLW_CURRENTMULTIPLIER   "hlw_currentmultiplier"
#define JSONCONFIG_HLW_VOLTAGEMULTIPLIER   "hlw_voltagemultiplier"
#define JSONCONFIG_HLW_POWERMULTIPLIER     "hlw_powermultiplier"
#define JSONCFONIG_GPIO5MODE             "gpio5mode"
#define JSONCFONIG_GPIO5ASSENDER         "gpio5assender"

bool loadSystemConfig() {
  DEBUG(F("loadSystemConfig mounting FS..."));
  if (SPIFFS.begin()) {
    DEBUG(F("loadSystemConfig mounted file system"));
    if (SPIFFS.exists("/" + configJsonFile)) {
      DEBUG(F("loadSystemConfig reading config file"));
      File configFile = SPIFFS.open("/" + configJsonFile, "r");
      if (configFile) {
        DEBUG(F("loadSystemConfig opened config file"));
        size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        DEBUG("Content of JSON Config-File: /" + configJsonFile);
#ifdef SERIALDEBUG
        json.printTo(Serial);
        Serial.println();
#endif
        DEBUG("\nJSON OK");
        ((json[JSONCONFIG_IP]).as<String>()).toCharArray(SHP6NetConfig.ip, IPSIZE);
        ((json[JSONCONFIG_NETMASK]).as<String>()).toCharArray(SHP6NetConfig.netmask, IPSIZE);
        ((json[JSONCONFIG_GW]).as<String>()).toCharArray(SHP6NetConfig.gw, IPSIZE);
        ((json[JSONCONFIG_CCUIP]).as<String>()).toCharArray(GlobalConfig.ccuIP, IPSIZE);
        ((json[JSONCONFIG_SHP6]).as<String>()).toCharArray(GlobalConfig.DeviceName, VARIABLESIZE);

        //((json[JSONCONFIG_LOXUSERNAME]).as<String>()).toCharArray(LoxoneConfig.Username, VARIABLESIZE);
        //((json[JSONCONFIG_LOXPASSWORD]).as<String>()).toCharArray(LoxoneConfig.Password, VARIABLESIZE);
        ((json[JSONCONFIG_LOXUDPPORT]).as<String>()).toCharArray(LoxoneConfig.UDPPort, 10);
        ((json[JSONCONFIG_HMPOWERVARIABLE]).as<String>()).toCharArray(HomeMaticConfig.PowerVariableName, VARIABLESIZE);
        ((json[JSONCONFIG_HMENERGYCOUNTERVARIABLE]).as<String>()).toCharArray(HomeMaticConfig.EnergyCounterVariableName, VARIABLESIZE);
          GlobalConfig.MeasureInterval = json[JSONCONFIG_MEASUREINTERVAL];
          if (GlobalConfig.MeasureInterval == 0)
            GlobalConfig.MeasureInterval = 60;

        GlobalConfig.BackendType = json[JSONCONFIG_BACKENDTYPE];
        GlobalConfig.GPIO5Mode = json[JSONCFONIG_GPIO5MODE];
        GlobalConfig.restoreOldRelayState = json[JSONCONFIG_RESTOREOLDSTATE];
        GlobalConfig.LEDDisabled = json[JSONCONFIG_LEDDISABLED];
        GlobalConfig.GPIO5asSender = json[JSONCFONIG_GPIO5ASSENDER];
        GlobalConfig.Hostname = "SHP6-" + String(GlobalConfig.DeviceName);

                    float fVal = (json[JSONCONFIG_HLW_CURRENTMULTIPLIER]).as<float>();
            if (fVal > 0.0) {
              if (HLW8012Calibration.CurrentMultiplier != fVal) {
                HLW8012Calibration.CurrentMultiplier = fVal;
              }
            }
            fVal = (json[JSONCONFIG_HLW_VOLTAGEMULTIPLIER]).as<float>();
            if (fVal > 0.0) {
              if (HLW8012Calibration.VoltageMultiplier != fVal) {
                HLW8012Calibration.VoltageMultiplier =  fVal;
              }
            }
            fVal = (json[JSONCONFIG_HLW_POWERMULTIPLIER]).as<float>();
            if (fVal > 0.0) {
              if (HLW8012Calibration.PowerMultiplier != fVal) {
                HLW8012Calibration.PowerMultiplier =  fVal;
              }
            }
      }
      return true;
    } else {
      DEBUG("/" + configJsonFile + " not found.");
      return false;
    }
    SPIFFS.end();
  } else {
    DEBUG(F("loadSystemConfig failed to mount FS"));
    return false;
  }
}

bool saveSystemConfig() {
  SPIFFS.begin();
  DEBUG(F("saving config"));
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json[JSONCONFIG_IP] = SHP6NetConfig.ip;
  json[JSONCONFIG_NETMASK] = SHP6NetConfig.netmask;
  json[JSONCONFIG_GW] = SHP6NetConfig.gw;
  json[JSONCONFIG_CCUIP] = GlobalConfig.ccuIP;
  json[JSONCONFIG_SHP6] = GlobalConfig.DeviceName;
  json[JSONCONFIG_RESTOREOLDSTATE] = GlobalConfig.restoreOldRelayState;
  json[JSONCONFIG_LEDDISABLED] = GlobalConfig.LEDDisabled;
  json[JSONCONFIG_BACKENDTYPE] = GlobalConfig.BackendType;
  //json[JSONCONFIG_LOXUSERNAME] = LoxoneConfig.Username;
  //json[JSONCONFIG_LOXPASSWORD] = LoxoneConfig.Password;
  json[JSONCONFIG_LOXUDPPORT] = LoxoneConfig.UDPPort;
  json[JSONCONFIG_HMPOWERVARIABLE] = HomeMaticConfig.PowerVariableName;
  json[JSONCONFIG_HMENERGYCOUNTERVARIABLE] = HomeMaticConfig.EnergyCounterVariableName;
  json[JSONCONFIG_MEASUREINTERVAL] = GlobalConfig.MeasureInterval;
  if (GlobalConfig.MeasureInterval == 0) GlobalConfig.MeasureInterval = 60;
  json[JSONCFONIG_GPIO5MODE] = GlobalConfig.GPIO5Mode;
  json[JSONCFONIG_GPIO5ASSENDER] = GlobalConfig.GPIO5asSender;

    json[JSONCONFIG_HLW_CURRENTMULTIPLIER] = HLW8012Calibration.CurrentMultiplier;
    json[JSONCONFIG_HLW_VOLTAGEMULTIPLIER] = HLW8012Calibration.VoltageMultiplier;
    json[JSONCONFIG_HLW_POWERMULTIPLIER] = HLW8012Calibration.PowerMultiplier;

  SPIFFS.remove("/" + configJsonFile);
  File configFile = SPIFFS.open("/" + configJsonFile, "w");
  if (!configFile) {
    DEBUG(F("failed to open config file for writing"));
    return false;
  }

#ifdef SERIALDEBUG
  json.printTo(Serial);
  Serial.println();
#endif
  json.printTo(configFile);
  configFile.close();
  SPIFFS.end();
  return true;
}

void setLastRelayState(bool state) {
  GlobalConfig.lastRelayState = state;
  if (GlobalConfig.restoreOldRelayState == RelayStateOnBoot_LAST) {
    if (SPIFFS.begin()) {
      DEBUG(F("setLastState mounted file system"));
      //SPIFFS.remove("/" + lastStateFilename);
      File setLastStateFile = SPIFFS.open("/" + lastRelayStateFilename, "w");
      setLastStateFile.print(state);
      setLastStateFile.close();
      SPIFFS.end();
      DEBUG("setLastState (" + String(state) + ") saved.");
    } else {
      DEBUG(F("setLastState SPIFFS mount fail!"));
    }
  }
}

bool getLastRelayState() {
  if (GlobalConfig.restoreOldRelayState == RelayStateOnBoot_LAST) {
    if (SPIFFS.begin()) {
      DEBUG(F("getLastState mounted file system"));
      if (SPIFFS.exists("/" + lastRelayStateFilename)) {
        DEBUG(lastRelayStateFilename + " existiert");
        File lastStateFile = SPIFFS.open("/" + lastRelayStateFilename, "r");
        bool bLastState = false;
        if (lastStateFile && lastStateFile.size()) {
          String content = String(char(lastStateFile.read()));
          DEBUG("getLastState FileContent = " + content);
          bLastState = (content == "1");
        }
        SPIFFS.end();
        return bLastState;
      } else {
        DEBUG(lastRelayStateFilename + " existiert nicht");
      }
    } else {
      DEBUG(F("getLastState SPIFFS mount fail!"));
      false;
    }
  } else {
    return false;
  }
}

void setBootConfigMode() {
  if (SPIFFS.begin()) {
    DEBUG(F("setBootConfigMode mounted file system"));
    if (!SPIFFS.exists("/" + bootConfigModeFilename)) {
      File bootConfigModeFile = SPIFFS.open("/" + bootConfigModeFilename, "w");
      bootConfigModeFile.print("0");
      bootConfigModeFile.close();
      SPIFFS.end();
      DEBUG(F("Boot to ConfigMode requested. Restarting..."));
      WebServer.send(200, "text/plain", F("<state>enableBootConfigMode - Rebooting</state>"));
      delay(500);
      ESP.restart();
    } else {
      WebServer.send(200, "text/plain", F("<state>enableBootConfigMode - FAILED!</state>"));
      SPIFFS.end();
    }
  }
}
