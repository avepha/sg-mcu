#include "domain/rtc/context.h"
#include "domain/sensor/context.h"
#include "domain/precondition/context.h"

#include "domain/control/criteria/context.h"
#include "domain/control/timer/context.h"

#ifndef SG_MCU_COMBINE_CONTEXT_H
#define SG_MCU_COMBINE_CONTEXT_H

class CombineContext {
public:
  RtcContext *rtcContext;
  SensorContext *sensorContext;
  PreConditionContext *preConditionContext;
  CriteriaContext *criteriaContext;
  TimerContext *timerContext;
  CombineContext();
};

CombineContext::CombineContext() {
  rtcContext = new RtcContext;
  sensorContext = new SensorContext;
  preConditionContext = new PreConditionContext;
  criteriaContext = new CriteriaContext;
  timerContext = new TimerContext;
};


#endif //SG_MCU_COMBINECONTEXT_H
