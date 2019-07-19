#include <Task.h>
extern TaskManager taskManager;
class Communication : public Task
{
public:
  static Communication *s_instance;
  Communication() : Task(MsToTaskTime(1))
  {
    cmdStr = "";
    cmdNumber = 0;
    cmdSize = 0;
    size = 0;
  };

  static Communication *instance()
  {
    if (!s_instance)
      s_instance = new Communication;
    return s_instance;
  }

private:
  String cmdStr;
  char res[100];
  int size, cmdNumber, cmdSize;
  virtual bool OnStart()
  {
    return true;
  }
  virtual void OnUpdate(uint32_t delta_time)
  {
    //{timer,1,1,20-60,90-150,200-260}{setpoint,2,50.5,30,30,1}{manual,3,0}{manual,4,1}
    while (mpuCom.available())
    {
      if (mpuCom.read() == '{')
      {
        int size = 0;
        while (true)
        {
          if (mpuCom.available())
          {
            char ch = mpuCom.read();
            if (ch == '}')
            {
              res[size] = '\0';
              String strRes = res;
              // CommandProcess(String(res));
              cmdStr += "{" + strRes + "}";
              cmdNumber++;
              break;
            }
            else
            {
              res[size] = ch;
              size++;
            }
          }
        }
      }
    }

    if (cmdNumber > 0)
    {

      for (int i = 0; i < cmdNumber; i++)
      {
        String subcom = cmdStr.substring(1, cmdStr.indexOf("}"));
        CommandProcess(subcom);
        cmdStr = cmdStr.substring(cmdStr.indexOf("}") + 1);
      }

      cmdNumber = 0;
      cmdStr = "";
    }
  }
  void ExtractDataFloat(float *data, int size, String res)
  {
    int i = 0, si = 0, ei, j = 0;
    while (j < size)
    {
      int index = res.indexOf(",");
      String a = res.substring(0, index);
      data[j] = a.toFloat();

      si = index;
      res = res.substring(index + 1);
      j++;
    }
  }

  void ExtractDataInt(int *data, int size, String res)
  {
    int i = 0, si = 0, ei, j = 0;
    while (j < size)
    {
      int index = res.indexOf(",");
      String a = res.substring(0, index);
      data[j] = a.toInt();

      si = index;
      res = res.substring(index + 1);
      j++;
    }
  }

  void ExtractDataString(String *data, int &size, String res)
  {
    int i = 0, si = 0, ei, j = 0;
    while (j < 20)
    {
      int index = res.indexOf(",");
      String a = res.substring(0, index);
      data[j] = a;
      // mpuCom.println(data[j]);
      si = index;
      res = res.substring(index + 1);
      j++;
      if (index == -1)
      {
        size = j;
        return;
      }
    }
  }

  timer_s *TimerStringToTimer(String *timer, timer_s *vtimer, int size)
  {
    for (int i = 0; i < size; i++)
    {
      int index = timer[i].indexOf("-");
      String startStr = timer[i].substring(0, index);
      String endStr = timer[i].substring(index + 1);

      uint16_t start = startStr.toInt();
      uint16_t end = endStr.toInt();
      timer_s t = {start, end};
      vtimer[i] = t;
    }
  }
  //{timer,1,1,20-60,90-150,200-260}{setpoint,2,50.5,30,30,1}{manual,3,0}{manual,4,1}
  bool CommandProcess(String res)
  {
    // mpuCom.println(res);
    // {datetime,8,1,17,6,31}
    if (res.startsWith("Gsensors"))
    {
      // mpuCom.println(Sensor::instance()->GetSensorsString());
      String data = "{st-sensors," + String(Sensor::instance()->GetSensor(0)) +
                    "," + String(Sensor::instance()->GetSensor(1)) +
                    "," + String(Sensor::instance()->GetSensor(2)) +
                    "," + String(Sensor::instance()->GetSensor(3)) +
                    "," + String(Sensor::instance()->GetSensor(4)) +
                    "," + String(Sensor::instance()->GetSensor(5)) +
                    "," + String(Sensor::instance()->GetSensor(6)) +
                    "," + String(Sensor::instance()->GetSensor(7)) +
                    "," + String(Sensor::instance()->GetSensor(8)) +
                    "," + String(ParAcc::instance()->GetParAcc()) + "}";
      mpuCom.println(data);
    }
    else if (res.startsWith("version")) {
      mpuCom.println("{st-v_sgmcu," + String(version) + "," + String(version) + "}");
    }
    else if (res.startsWith("Gdatetime"))
    {
      mpuCom.println(RTC::instance()->GetDateTimeString());
    }
    else if (res.startsWith("checkstatus"))
    {
      mpuCom.println(MCU_STATE);
    }
    else if (res.startsWith("freememory"))
    {
      String data = "{\"type\": \"free-memory\",\"data\": " + String(MemoryCheck::instance()->GetFreeMemory()) + "}";
      mpuCom.println(data);
    }
    else if (res.startsWith("Ggpio"))
    {
      res.replace("Ggpio,", "");
      res.trim();
      mpuCom.println(ChannelHanler::instance()->GpioStatus());
    }
    else if (res.startsWith("Gparacc"))
    {
      mpuCom.println(ChannelHanler::instance()->JsonChannelPar(0, 4));
    }
    else if (res.startsWith("Gcontrol")) /*  */
    {
      // ChannelHanler::instance()->JsonSettingAll();
      // mpuCom.println(ChannelHanler::instance()->JsonControl());
      res.replace("Gcontrol,", "");
      res.trim();
      String data[3];
      int size = 0;
      ExtractDataString(data, size, res);
      String cmd = data[0];
      int start_ch = data[1].toInt() - 1;
      int number = data[2].toInt();

      if (cmd.startsWith("channelstatus"))
      {
        mpuCom.println(ChannelHanler::instance()->JsonChannelStatus(start_ch, number));
      }
      else if (cmd.startsWith("manual"))
      {
        mpuCom.println(ChannelHanler::instance()->JsonManual(start_ch, number));
      }
      else if (cmd.startsWith("timer"))
      {
        mpuCom.println(ChannelHanler::instance()->JsonTimer(start_ch, number));
      }
      else if (cmd.startsWith("irrigation"))
      {
        mpuCom.println(ChannelHanler::instance()->JsonIrrigation(start_ch, number));
      }

      else if(cmd.startsWith("setpoint"))
      {
        mpuCom.println(ChannelHanler::instance()->JsonAdvanceSetpoint(start_ch, number));
      }
      else if(cmd.startsWith("advsbt"))
      {
        mpuCom.println(ChannelHanler::instance()->JsonAdvanceSetboundTiming(start_ch, number));
      }
      else if(cmd.startsWith("advsb"))
      {
        mpuCom.println(ChannelHanler::instance()->JsonAdvanceSetbound(start_ch, number));
      }

    }
    //{datetime, }
    else if (res.startsWith("datetime"))
    {
      res.replace("datetime,", "");
      res.trim();
      int dt[5] = {0, 0, 0, 0, 0};
      ExtractDataInt(dt, 5, res);
      // (byte s, byte m, byte h, byte dow, byte dom, byte mo, byte y)
      RTC::instance()->setDateDs1307(0, dt[4], dt[3], 0, dt[0], dt[1], dt[2]);
      mpuCom.println("UPD-DATETIME");
    }

    else if (res.startsWith("mode"))
    {
      res.replace("mode,", "");
      res.trim();
      int mode[2] = {0, 0};
      ExtractDataInt(mode, 2, res);
      int ch = (int)mode[0];
      rom_channel[ch - 1].mode = mode[1];
      EEPROM_Manager::Update(ch);
      ChannelHanler::instance()->Update(ch);
      mpuCom.println("UPD-MODE-" + String(ch));
    }

    //{manual, channel, status}
    // {manual, 1,1}
    else if (res.startsWith("manual"))
    {
      res.replace("manual,", "");
      res.trim();
      int mode[2] = {0, 0};
      ExtractDataInt(mode, 2, res);
      int ch = (int)mode[0];
      rom_channel[ch - 1].mode = 0;
      rom_channel[ch - 1].manual.status = mode[1];
      EEPROM_Manager::Update(ch);
      ChannelHanler::instance()->Update(ch);
      mpuCom.println("UPD-MANUAL-" + String(ch));
    }

    //{timer,1,1,20-60,90-150,200-260}
    else if (res.startsWith("timer"))
    {
      res.replace("timer,", "");
      res.trim();
      int ch = res.substring(0, 1).toInt();
      int timer_mode = res.substring(2, 3).toInt();
      res = res.substring(4);
      String timer[15];
      int size = 0;
      ExtractDataString(timer, size, res);
      timer_s timerlist[8];
      TimerStringToTimer(timer, timerlist, size);
      memcpy(rom_channel[ch - 1].timer.timer_list, timerlist, 8 * sizeof(timer_s));

      rom_channel[ch - 1].mode = 1;
      rom_channel[ch - 1].timer.size = size;
      rom_channel[ch - 1].timer.mode = timer_mode;
      EEPROM_Manager::Update(ch);
      ChannelHanler::instance()->Update(ch);
      mpuCom.println("UPD-TIMER-" + String(ch));
    }
    else if (res.startsWith("irrigation"))
    {
      res.replace("irrigation,", "");
      res.trim();
      float mode[12];
      ExtractDataFloat(mode, 12, res);
      int ch = (int)mode[0];
      rom_channel[ch - 1].mode = 5;
      rom_channel[ch - 1].irrigation.mode = (int)mode[1];
      rom_channel[ch - 1].irrigation.soil_upper = mode[2];
      rom_channel[ch - 1].irrigation.soil_lower = mode[3];
      rom_channel[ch - 1].irrigation.soil_detecting = mode[4];
      rom_channel[ch - 1].irrigation.soil_working = mode[5];

      rom_channel[ch - 1].irrigation.par_soil_setpoint = mode[6];
      rom_channel[ch - 1].irrigation.par_working = mode[7];
      rom_channel[ch - 1].irrigation.par_detecting = mode[8];
      rom_channel[ch - 1].irrigation.par_acc = mode[9];
      rom_channel[ch - 1].irrigation.descent_rate = mode[10];
      rom_channel[ch - 1].irrigation.limit_time = mode[11];

      EEPROM_Manager::Update(ch);
      ChannelHanler::instance()->Update(ch);
      mpuCom.println("UPD-IRR-" + String(ch));
    }
    // {Gcontrol,dfirrigation,1,1}
    // {dfirrigation,1,30,40,1.0,15,0.4}
    //{dfirrigation,ch, upper, lower, paracc, working, descent}
    // else if (res.startsWith("dfirrigation"))
    // {
    //   res.replace("dfirrigation,", "");
    //   res.trim();
    //   float mode[10];
    //   ExtractDataFloat(mode, 6, res);
    //   int ch = (int)mode[0];
    //   rom_channel[ch - 1].mode = 9;
    //   rom_channel[ch - 1].dfirrigation.upper = mode[1];
    //   rom_channel[ch - 1].dfirrigation.lower = mode[2];
    //   rom_channel[ch - 1].dfirrigation.paracc = mode[3];
    //   rom_channel[ch - 1].dfirrigation.working = mode[4];
    //   rom_channel[ch - 1].dfirrigation.descent = mode[5];

    //   EEPROM_Manager::Update(ch);
    //   // ChannelHanler::instance()->Update(ch);
    //   mpuCom.println("UPD-DFIRR-" + String(ch));
    // }

    //{advancecond, ch, setpoint, working, detecting, sensor, direction , sensor_cond, sensor_direction, sensor_set,
    //              sensor_flag, timer_flag, 480-1080,1100-1120}
    //{advancecond, 1, 600, 15, 30, 5, 0, 3, 0, 30, 1, 1, 480-1080}
    else if (res.startsWith("setpoint"))
    {
      res.replace("setpoint,", "");
      res.trim();
      float mode[12];
      ExtractDataFloat(mode, 11, res);
      int ch = (int)mode[0];
      rom_channel[ch - 1].mode = 6;
      rom_channel[ch - 1].setpoint.setpoint = mode[1];
      rom_channel[ch - 1].setpoint.working = mode[2];
      rom_channel[ch - 1].setpoint.detecting = mode[3];
      rom_channel[ch - 1].setpoint.sensor = (int)mode[4];
      rom_channel[ch - 1].setpoint.direction = (int)mode[5];

      rom_channel[ch - 1].setpoint.sensor_condition = (int)mode[6];
      rom_channel[ch - 1].setpoint.sensor_direction = (int)mode[7];
      rom_channel[ch - 1].setpoint.sensor_setpoint = mode[8];
      rom_channel[ch - 1].setpoint.sensor_flag = (int)mode[9] == 1 ? true : false;
      rom_channel[ch - 1].setpoint.timer_flag = (int)mode[10] == 1 ? true : false;

      int i = 0;
      while (i < 11)
      {
        int index = res.indexOf(",");
        String a = res.substring(0, index);
        res = res.substring(index + 1);
        i++;
      }

      String timer[15];
      int size = 0;
      ExtractDataString(timer, size, res);
      timer_s timerlist[3];
      TimerStringToTimer(timer, timerlist, size);
      memcpy(rom_channel[ch - 1].setpoint.timer_list, timerlist, 3 * sizeof(timer_s));
      rom_channel[ch - 1].setpoint.timer_size = size;

      EEPROM_Manager::Update(ch);
      ChannelHanler::instance()->Update(ch);
      mpuCom.println("UPD-SETPOINT-" + String(ch));
    }

     //{advancesbt, ch,upper ,lower ,working , detecting, sensor, direction , sensor_cond, sensor_direction, sensor_set,
    //              sensor_flag, timer_flag, 480-1080,1100-1120}
    //{advancesbt, 1, 1000,900, 15, 30, 5, 0, 3, 0, 30, 1, 1, 480-1080}
    else if (res.startsWith("advancesbt"))
    {
      res.replace("advancesbt,", "");
      res.trim();
      float mode[13];
      ExtractDataFloat(mode, 12, res);
      int ch = (int)mode[0];
      rom_channel[ch - 1].mode = 8;
      rom_channel[ch - 1].advsbt.upper = mode[1];
      rom_channel[ch - 1].advsbt.lower = mode[2];
      rom_channel[ch - 1].advsbt.working = mode[3];
      rom_channel[ch - 1].advsbt.detecting = mode[4];
      rom_channel[ch - 1].advsbt.sensor = (int)mode[5];
      rom_channel[ch - 1].advsbt.direction = (int)mode[6];

      rom_channel[ch - 1].advsbt.sensor_condition = (int)mode[7];
      rom_channel[ch - 1].advsbt.sensor_direction = (int)mode[8];
      rom_channel[ch - 1].advsbt.sensor_setpoint = mode[9];
      rom_channel[ch - 1].advsbt.sensor_flag = (int)mode[10] == 1 ? true : false;
      rom_channel[ch - 1].advsbt.timer_flag = (int)mode[11] == 1 ? true : false;

      int i = 0;
      while (i < 12)
      {
        int index = res.indexOf(",");
        String a = res.substring(0, index);
        res = res.substring(index + 1);
        i++;
      }
      mpuCom.println(res);
      String timer[15];
      int size = 0;
      ExtractDataString(timer, size, res);
      timer_s timerlist[3];
      TimerStringToTimer(timer, timerlist, size);
      memcpy(rom_channel[ch - 1].advsbt.timer_list, timerlist, 3 * sizeof(timer_s));
      rom_channel[ch - 1].advsbt.timer_size = size;

      EEPROM_Manager::Update(ch);
      ChannelHanler::instance()->Update(ch);
      mpuCom.println("UPD-ADVSBT-" + String(ch));
    }

  //{advancesb, ch,upper ,lower , sensor, direction , sensor_cond, sensor_direction, sensor_set,
    //              sensor_flag, timer_flag, 480-1080,1100-1120}
    //{advancesb, 1, 1000,900, 5, 0, 3, 0, 30, 1, 1, 480-1080}
    else if (res.startsWith("advancesb"))
    {
      res.replace("advancesb,", "");
      res.trim();
      float mode[11];
      ExtractDataFloat(mode, 10, res);
      int ch = (int)mode[0];
      rom_channel[ch - 1].mode = 7;
      rom_channel[ch - 1].advsb.upper = mode[1];
      rom_channel[ch - 1].advsb.lower = mode[2];
      rom_channel[ch - 1].advsb.sensor = (int)mode[3];
      rom_channel[ch - 1].advsb.direction = (int)mode[4];

      rom_channel[ch - 1].advsb.sensor_condition = (int)mode[5];
      rom_channel[ch - 1].advsb.sensor_direction = (int)mode[6];
      rom_channel[ch - 1].advsb.sensor_setpoint = mode[7];
      rom_channel[ch - 1].advsb.sensor_flag = (int)mode[8] == 1 ? true : false;
      rom_channel[ch - 1].advsb.timer_flag = (int)mode[9] == 1 ? true : false;

      int i = 0;
      while (i < 10)
      {
        int index = res.indexOf(",");
        String a = res.substring(0, index);
        res = res.substring(index + 1);
        i++;
      }
      mpuCom.println(res);
      String timer[15];
      int size = 0;
      ExtractDataString(timer, size, res);
      timer_s timerlist[3];
      TimerStringToTimer(timer, timerlist, size);
      memcpy(rom_channel[ch - 1].advsb.timer_list, timerlist, 3 * sizeof(timer_s));
      rom_channel[ch - 1].advsb.timer_size = size;

      EEPROM_Manager::Update(ch);
      ChannelHanler::instance()->Update(ch);
      mpuCom.println("UPD-ADVSB-" + String(ch));
    }

    else if (res.startsWith("clear-memory"))
    {
      EEPROM.write(EEPROM_Manager::init_byte, 0);
      EEPROM_Manager::Commit();
      mpuCom.println("UPD-CLEARMEM");
    }
    else if (res.startsWith("done"))
    {
      mpuCom.println("DONE");
    }
  }
};
Communication *Communication::s_instance = 0;
