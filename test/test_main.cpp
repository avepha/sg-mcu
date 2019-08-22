#include "mutation/m_date_save.h"
#include "mutation/m_sensor_order_save.h"
#include "mutation/m_precondition_save.h"
#include "mutation/m_criteria.h"
#include "mutation/m_timer_save.h"

#include "query/q_date.h"
#include "query/q_sensor.h"
#include "query/q_sensor_order.h"
#include "query/q_precondition.h"
#include "query/q_criteria.h"
#include "query/q_timer.h"
#include "query/q_nsensor.h"
#include "query/q_nsensors.h"

#include "test_util/date_time.h"
#include "test_util/updateNSensor.h"

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  delay(1000);

  UNITY_BEGIN();

  /* Query Test */
  q_date_RUN_TEST();
  q_sensor_RUN_TEST();
  q_sensor_order_RUN_TEST();
  q_precondition_RUN_TEST();
  q_criteria_RUN_TEST();
  q_timer_RUN_TEST();
  q_nsensor_RUN_TEST();
  q_nsensors_RUN_TEST();

  /* Mutation Test */
  m_date_save_RUN_TEST();
  m_sensor_order_save_RUN_TEST();
  m_precondition_save_RUN_TEST();
  m_criteria_save_RUN_TEST();
  m_timer_save_RUN_TEST();

  /* Util Test */
  util_date_time_RUN_TEST();
  util_updateNSensor_RUN_TEST();
  UNITY_END();
}

void loop() {

}