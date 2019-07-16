#include <Task.h>
extern TaskManager taskManager;
class ChannelHanler : public Task
{
  public:
    static ChannelHanler *s_instance;

    ChannelHanler() : Task(MsToTaskTime(100))
    {
        for (int i = 0; i < 4; i++)
        {
            pinMode(ChannelGpio[i], OUTPUT);
        }
        Init();
    };
    static ChannelHanler *instance()
    {
        if (!s_instance)
            s_instance = new ChannelHanler;
        return s_instance;
    }

    void Update(int ch)
    {
        DigitalWrite(ch - 1, CH_OFF);
        taskManager.StopTask(channel[ch - 1]);
        int mode = rom_channel[ch - 1].mode;

        switch (mode)
        {
        case 0:
            channel[ch - 1] = ControlFactory::instance("manual", ch);
            taskManager.StartTask(channel[ch - 1]);
            break;
        case 1:
            channel[ch - 1] = ControlFactory::instance("timer", ch);
            taskManager.StartTask(channel[ch - 1]);
            break;
        case 2:
            channel[ch - 1] = ControlFactory::instance("p_setpoint", ch);
            taskManager.StartTask(channel[ch - 1]);
            break;
        case 3:
            channel[ch - 1] = ControlFactory::instance("setbound", ch);
            taskManager.StartTask(channel[ch - 1]);
            break;
        case 4:
            channel[ch - 1] = ControlFactory::instance("sbtiming", ch);
            taskManager.StartTask(channel[ch - 1]);
            break;
        case 5:
            channel[ch - 1] = ControlFactory::instance("irrigation", ch);
            taskManager.StartTask(channel[ch - 1]);
            break;
        case 6:
            channel[ch - 1] = ControlFactory::instance("setpoint", ch);
            taskManager.StartTask(channel[ch - 1]);
            break;
        case 7:
            channel[ch - 1] = ControlFactory::instance("advsb", ch);
            taskManager.StartTask(channel[ch - 1]);
            break;
        case 8:
            channel[ch - 1] = ControlFactory::instance("advsbt", ch);
            taskManager.StartTask(channel[ch - 1]);
            break;
        }
    }

    static String GpioStatus()
    {
        String data = "{st-gpio,";
        for (int i = 0; i < CHANNEL_NUMBER; i++)
        {
            data += String(ChannelStatus[i]);
            if (i < CHANNEL_NUMBER - 1)
                data += ",";
        }
        data += "}";
        return data;
    }

    static String JsonChannelStatus(int start_ch, int number)
    {
        int end_ch = start_ch + number;
        String data = "";
        for (int i = start_ch; i < end_ch; i++)
        {
            String chstr = "{ct-chst," + String(i + 1) +
                           "," + String(rom_channel[i].mode) +
                           "," + String(rom_channel[i].sensor) +
                           "," + String(rom_channel[i].manual.status) + "}";
            data += chstr;
        }
        return data;
    }

    String JsonManual(int start_ch, int number)
    {
        int end_ch = start_ch + number;
        String data = "";
        for (int i = start_ch; i < end_ch; i++)
        {
            //{ct-manual, ch, status}
            String chstr = "{ct-manual," + String(i + 1) +
                           "," + String(rom_channel[i].manual.status) + "}";
            data += chstr;
        }
        return data;
    }

    String JsonAdvanceSetpoint(int start_ch, int number)
    {

        int end_ch = start_ch + number;
        String data = "";
        for (int i = start_ch; i < end_ch; i++)
        {
            //{advancecond, ch, setpoint, working, detecting, sensor, direction , sensor_cond, sensor_direction, sensor_set,
            //              sensor_flag, timer_flag, 480-1080,1100-1120}
            String chstr = "{ct-setpoint," + String(i + 1) +
                           "," + String(rom_channel[i].setpoint.setpoint) +
                           "," + String(rom_channel[i].setpoint.working) +
                           "," + String(rom_channel[i].setpoint.detecting) +
                           "," + String(rom_channel[i].setpoint.sensor) +
                           "," + String(rom_channel[i].setpoint.direction) +
                           "," + String(rom_channel[i].setpoint.sensor_condition) +
                           "," + String(rom_channel[i].setpoint.sensor_direction) +
                           "," + String(rom_channel[i].setpoint.sensor_setpoint) +
                           "," + String(rom_channel[i].setpoint.sensor_flag) +
                           "," + String(rom_channel[i].setpoint.timer_flag);
            String timer_list = "[";
            for (int j = 0; j < rom_channel[i].setpoint.timer_size; j++)
            {
                String timer = "[" + String(rom_channel[i].setpoint.timer_list[j].st) + "-" + String(rom_channel[i].setpoint.timer_list[j].en) + "]";
                timer_list += timer;
                if (j != (rom_channel[i].setpoint.timer_size - 1))
                {
                    timer_list += "-";
                }
            }
            timer_list += "]";
            chstr += +"," + timer_list + "}";
            data += chstr;
        }
        return data;
    }

    String JsonAdvanceSetbound(int start_ch, int number)
    {

        int end_ch = start_ch + number;
        String data = "";
        for (int i = start_ch; i < end_ch; i++)
        {
            //{advancecond, ch, setpoint, working, detecting, sensor, direction , sensor_cond, sensor_direction, sensor_set,
            //              sensor_flag, timer_flag, 480-1080,1100-1120}
            String chstr = "{ct-advsb," + String(i + 1) +
                           "," + String(rom_channel[i].advsb.upper) +
                           "," + String(rom_channel[i].advsb.lower) +
                           "," + String(rom_channel[i].advsb.sensor) +
                           "," + String(rom_channel[i].advsb.direction) +
                           "," + String(rom_channel[i].advsb.sensor_condition) +
                           "," + String(rom_channel[i].advsb.sensor_direction) +
                           "," + String(rom_channel[i].advsb.sensor_setpoint) +
                           "," + String(rom_channel[i].advsb.sensor_flag) +
                           "," + String(rom_channel[i].advsb.timer_flag);
            String timer_list = "[";
            for (int j = 0; j < rom_channel[i].advsb.timer_size; j++)
            {
                String timer = "[" + String(rom_channel[i].advsb.timer_list[j].st) + "-" + String(rom_channel[i].advsb.timer_list[j].en) + "]";
                timer_list += timer;
                if (j != (rom_channel[i].advsb.timer_size - 1))
                {
                    timer_list += "-";
                }
            }
            timer_list += "]";
            chstr += +"," + timer_list + "}";
            data += chstr;
        }
        return data;
    }

    String JsonAdvanceSetboundTiming(int start_ch, int number)
    {

        int end_ch = start_ch + number;
        String data = "";
        for (int i = start_ch; i < end_ch; i++)
        {
            //{advancecond, ch, setpoint, working, detecting, sensor, direction , sensor_cond, sensor_direction, sensor_set,
            //              sensor_flag, timer_flag, 480-1080,1100-1120}
            String chstr = "{ct-advsbt," + String(i + 1) +
                           "," + String(rom_channel[i].advsbt.upper) +
                           "," + String(rom_channel[i].advsbt.lower) +
                           "," + String(rom_channel[i].advsbt.working) +
                           "," + String(rom_channel[i].advsbt.detecting) +
                           "," + String(rom_channel[i].advsbt.sensor) +
                           "," + String(rom_channel[i].advsbt.direction) +
                           "," + String(rom_channel[i].advsbt.sensor_condition) +
                           "," + String(rom_channel[i].advsbt.sensor_direction) +
                           "," + String(rom_channel[i].advsbt.sensor_setpoint) +
                           "," + String(rom_channel[i].advsbt.sensor_flag) +
                           "," + String(rom_channel[i].advsbt.timer_flag);
            String timer_list = "[";
            for (int j = 0; j < rom_channel[i].advsbt.timer_size; j++)
            {
                String timer = "[" + String(rom_channel[i].advsbt.timer_list[j].st) + "-" + String(rom_channel[i].advsbt.timer_list[j].en) + "]";
                timer_list += timer;
                if (j != (rom_channel[i].advsbt.timer_size - 1))
                {
                    timer_list += "-";
                }
            }
            timer_list += "]";
            chstr += +"," + timer_list + "}";
            data += chstr;
        }
        return data;
    }

     String JsonIrrigation(int start_ch, int number)
    {

        int end_ch = start_ch + number;
        String data = "";
        for (int i = start_ch; i < end_ch; i++)
        {
            //{ct-manual, ch, status}
            String chstr = "{ct-irrigation," + String(i + 1) +
                           "," + String(rom_channel[i].irrigation.soil_upper) +
                           "," + String(rom_channel[i].irrigation.soil_lower) +
                           "," + String(rom_channel[i].irrigation.soil_detecting) +
                           "," + String(rom_channel[i].irrigation.soil_working) +
                           "," + String(rom_channel[i].irrigation.par_soil_setpoint) +
                           "," + String(rom_channel[i].irrigation.par_detecting) +
                           "," + String(rom_channel[i].irrigation.par_working) +
                           "," + String(rom_channel[i].irrigation.par_acc) +
                           "," + String(rom_channel[i].irrigation.limit_time) +
                           "," + String(rom_channel[i].irrigation.descent_rate) +
                           "," + String(rom_channel[i].irrigation.mode) + "}";
            data += chstr;
        }
        return data;
    }

    String JsonChannelPar(int start_ch, int number)
    {
        String data = "";
        //{ct-manual, ch, status}
        String chstr = "{st-paracc," + String(ParAcc::parChAcc[0]) +
                       "," + String(ParAcc::parChAcc[1]) +
                       "," + String(ParAcc::parChAcc[2]) +
                       "," + String(ParAcc::parChAcc[3]) + "}";

        return chstr;
    }

    String JsonTimer(int start_ch, int number)
    {
        /*
        String ch[CHANNEL_NUMBER];
        for (int i = 0; i < CHANNEL_NUMBER; i++)
        {
            String timer_list = "[";
            for (int j = 0; j < rom_channel[i].timer.size; j++)
            {
                String timer = "[" + String(rom_channel[i].timer.timer_list[j].st) + "," + String(rom_channel[i].timer.timer_list[j].en) + "]";
                timer_list += timer;
                if (j != (rom_channel[i].timer.size - 1))
                {
                    timer_list += ",";
                }
            }
            timer_list += "]";
            // data = "{\"mode\": 1,\"timer_mode\":" + String(rom_channel[i].timer.mode) +
            //         ",\"timer_list\":" + timer_list + "}";
            String data = "\"timer\": { \"mode\":" + String(rom_channel[i].timer.mode) + ",\"list\":" + timer_list + "}";
            ch[i] = "{ \"ch\":" + String(i + 1) + "," + data + "}";
        }
        return "{\"type\": \"control-timer\",\"data\": [" + ch[0] + "," + ch[1] + "," + ch[2] + "," + ch[3] + "," + ch[4] + "]}";
        */
        int end_ch = start_ch + number;
        String data = "";
        for (int i = start_ch; i < end_ch; i++)
        {
            String timer_list = "[";
            for (int j = 0; j < rom_channel[i].timer.size; j++)
            {
                String timer = "[" + String(rom_channel[i].timer.timer_list[j].st) + "-" + String(rom_channel[i].timer.timer_list[j].en) + "]";
                timer_list += timer;
                if (j != (rom_channel[i].timer.size - 1))
                {
                    timer_list += "-";
                }
            }
            timer_list += "]";
            data += "{ct-timer," + String(i + 1) + "," + String(rom_channel[i].timer.mode) +
                    "," + timer_list + "}";
        }
        return data;
    }

  private:
    void Init()
    {
        int i = 0;
        for (i = 0; i < CHANNEL_NUMBER; i++)
        {
            int mode = rom_channel[i].mode;
            switch (mode)
            {
            case 0:
                channel[i] = ControlFactory::instance("manual", (i + 1));
                taskManager.StartTask(channel[i]);
                break;
            case 1:
                channel[i] = ControlFactory::instance("timer", (i + 1));
                taskManager.StartTask(channel[i]);
                break;
            case 2:
                channel[i] = ControlFactory::instance("p_setpoint", (i + 1));
                taskManager.StartTask(channel[i]);
                break;
            case 3:
                channel[i] = ControlFactory::instance("setbound", (i + 1));
                taskManager.StartTask(channel[i]);
                break;
            case 4:
                channel[i] = ControlFactory::instance("sbtiming", (i + 1));
                taskManager.StartTask(channel[i]);
                break;
            case 5:
                channel[i] = ControlFactory::instance("irrigation", (i + 1));
                taskManager.StartTask(channel[i]);
                break;
            case 6:
                channel[i] = ControlFactory::instance("setpoint", (i + 1));
                taskManager.StartTask(channel[i]);
                break;
            case 7:
                channel[i] = ControlFactory::instance("advsb", (i + 1));
                taskManager.StartTask(channel[i]);
                break;
            case 8:
                channel[i] = ControlFactory::instance("advsbt", (i + 1));
                taskManager.StartTask(channel[i]);
                break;
            }
        }
    }
    virtual bool OnStart()
    {

        return true;
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
    }
};
ChannelHanler *ChannelHanler::s_instance = 0;
