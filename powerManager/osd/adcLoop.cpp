#include "NVStore.h"
#include "powerManager.h"
#include "vars.h"
#include "wiring.h"

#include "mbed.h"
#include "PeripheralPins.h"

static ADC_ChannelConfTypeDef sConfig = {0};

static uint32_t _channelMapping[] = {
  ADC_CHANNEL_0, ADC_CHANNEL_1, ADC_CHANNEL_2, ADC_CHANNEL_3,
  ADC_CHANNEL_4, ADC_CHANNEL_5, ADC_CHANNEL_6, ADC_CHANNEL_7,
  ADC_CHANNEL_8, ADC_CHANNEL_9, ADC_CHANNEL_10, ADC_CHANNEL_11, 
  ADC_CHANNEL_12, ADC_CHANNEL_13, ADC_CHANNEL_14, ADC_CHANNEL_15
};

static ADC_HandleTypeDef hadc1;
static ADC_HandleTypeDef hadc2;

static void _configure_adc(ADC_HandleTypeDef &hadc, PinName pin) {
  // NB: doesn't work for internal channels (VBAT, etc...)
  hadc.Instance = (ADC_TypeDef *)pinmap_peripheral(pin, PinMap_ADC);
  MBED_ASSERT(hadc.Instance != (ADC_TypeDef *)NC);
  pinmap_pinout(pin, PinMap_ADC);

  hadc.State = HAL_ADC_STATE_RESET;
  //hadc.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc.Init.Resolution            = ADC_RESOLUTION_12B;
  hadc.Init.ScanConvMode          = DISABLE;
  //hadc.Init.ContinuousConvMode    = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.NbrOfDiscConversion   = 0;
  hadc.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
  hadc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hadc.Init.NbrOfConversion       = 1;
  hadc.Init.DMAContinuousRequests = DISABLE;
  //hadc.Init.EOCSelection          = DISABLE;

  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
  hadc.Init.ContinuousConvMode = ENABLE;
  hadc.Init.EOCSelection          = ADC_EOC_SEQ_CONV;

  if (HAL_ADC_Init(&hadc) != HAL_OK) {
    error("Cannot initialize ADC");
  }
}

static void _adc_init() {
  __HAL_RCC_ADC1_CLK_ENABLE(); // That is power on
  __HAL_RCC_ADC2_CLK_ENABLE();
  ADC->CCR &= ~(ADC_CCR_VBATE | ADC_CCR_TSVREFE);

  // Config ADC1 for current sampling
  _configure_adc(hadc1, SAMPLER_CURRENT_PIN);
  
  uint32_t function = pinmap_function(SAMPLER_CURRENT_PIN, PinMap_ADC);
  MBED_ASSERT(function != (uint32_t)NC);

  sConfig.Rank         = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  sConfig.Offset       = 0;
  sConfig.Channel = _channelMapping[STM_PIN_CHANNEL(function)]; // Should be channel 11

  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

  // Config Channel for voltage sampling
  _configure_adc(hadc2, PinName(SAMPLER_VOLTAGE_PIN | ALT0));

  uint32_t function2 = pinmap_function(PinName(SAMPLER_VOLTAGE_PIN | ALT0), PinMap_ADC);
  MBED_ASSERT(function2 != (uint32_t)NC);
  sConfig.Channel = _channelMapping[STM_PIN_CHANNEL(function2)]; // PC_0 should by channel 10
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);

  // Configure for interrupt handling

  HAL_NVIC_SetPriority(ADC_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(ADC_IRQn);

  // Multi mode part, we have to go register level, HAL_ADCEx has only provision for DMA

  // ADC_MultiModeTypeDef multimode;
  // multimode.Mode = ADC_DUALMODE_REGSIMULT;
  // multimode.DMAAccessMode = ADC_DMAACCESSMODE_DISABLED;
  // multimode.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_20CYCLES;
  // HAL_StatusTypeDef res = HAL_ADCEx_MultiModeConfigChannel(&hadc2, &multimode);

  ADC_Common_TypeDef *tmpADC_Common = ADC_COMMON_REGISTER(&hadc1);
  tmpADC_Common->CCR &= ~(ADC_CCR_MULTI);
  tmpADC_Common->CCR |= ADC_DUALMODE_REGSIMULT;

  tmpADC_Common->CCR &= ~(ADC_CCR_DELAY);
  tmpADC_Common->CCR |= ADC_TWOSAMPLINGDELAY_20CYCLES;

  // Done, now start your engine

  __HAL_ADC_ENABLE(&hadc2);
  ADC2->CR2 |= (uint32_t)ADC_CR2_SWSTART;
  __HAL_ADC_CLEAR_FLAG(&hadc2, ADC_FLAG_EOC | ADC_FLAG_OVR);
  HAL_ADC_Start_IT(&hadc1); // Start conversion
  //HAL_ADC_Start_IT(&hadc2); // Start conversion
}

static volatile unsigned long _adc_num = 0;
static volatile uint32_t _adc_i = 0;
static volatile uint32_t _adc_v = 0;
static volatile uint32_t _last_i;
static volatile uint32_t _last_v;

extern "C" void ADC_IRQHandler() {
  HAL_ADC_IRQHandler(&hadc1);
}

extern "C" void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
  //uint32_t i = HAL_ADC_GetValue(hadc);
  uint32_t i = ADC1->DR;
  uint32_t v = ADC2->DR;
  CriticalSectionLock::enable();
  ++_adc_num;
  // Wrap around every 45s or so (depending on ADC clocking), so be
  // quick or be dead. Read in loop every sec.
  _adc_i += i;
  _adc_v += v;
  CriticalSectionLock::disable();
  _last_i = i;
}

extern "C" void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
  _adc_num = 12345678;
}

static volatile uint16_t uADCms;

static void loop() {
  for(;;) {
    uint32_t i, v, nn;
    CriticalSectionLock::enable();
    i = _adc_i;
    v = _adc_v;
    nn = _adc_num;
    _adc_i = 0;
    _adc_v = 0;
    _adc_num = 0;
    CriticalSectionLock::disable();
    i /= nn;
    v /= nn;
    powerManager_loop_cb(v, i);
    wait_ms(uADCms);
  }
}

static Thread thread(osPriorityNormal, OS_STACK_SIZE, NULL, "t/adcLoop");

void powerManager_osd_init() {
  vars_register("uADCms", const_cast<uint16_t *>(&uADCms));
  uADCms = NV<uint16_t>::get("uADCms");
  _adc_init();
  thread.start(loop);
}
