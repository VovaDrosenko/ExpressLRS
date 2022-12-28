#if defined(Regulatory_Domain_EU_CE_2400)
#include "common.h"
#include "logging.h"
#include "LBT.h"

extern SX1280Driver Radio;

LQCALC<100> LBTSuccessCalc;
static uint32_t rxStartTime;

#if !defined(LBT_RSSI_THRESHOLD_OFFSET_DB)
  #define LBT_RSSI_THRESHOLD_OFFSET_DB 0
#endif

bool LBTEnabled = false;
static bool LBTStarted = false;

static uint32_t ICACHE_RAM_ATTR SpreadingFactorToRSSIvalidDelayUs(
  SX1280_RadioLoRaSpreadingFactors_t SF,
  uint8_t radio_type
)
{
  // The necessary wait time from RX start to valid instant RSSI reading
  // changes with the spreading factor setting.
  // The worst case necessary wait time is TX->RX switch time + Lora symbol time
  // This assumes the radio switches from either TX, RX or FS (Freq Synth mode)
  // TX to RX switch time is 60us for sx1280
  // Lora symbol time for the relevant spreading factors is:
  // SF5: 39.4us
  // SF6: 78.8us
  // SF7: 157.6us
  // SF9: 630.5us
  // However, by measuring when the RSSI reading is stable and valid, it was found that
  // actual necessary wait times are:
  // SF5 ~100us (60us + SF5 symbol time)
  // SF6 ~141us (60us + SF6 symbol time)
  // SF7 ~218us (60us + SF7 symbol time)
  // SF9 ~218us (Odd one out, measured to same as SF7 wait time)

  if (radio_type == RADIO_TYPE_LORA)
  {
    switch(SF)
      {
        case SX1280_LORA_SF5: return 100;
        case SX1280_LORA_SF6: return 141;
        case SX1280_LORA_SF7: return 218;
        case SX1280_LORA_SF8: return 218;
        default: return 218;
      }
  }
  else if (radio_type == RADIO_TYPE_FLRC)
  {
    return 60 + 20; // switching time (60us) + 20us settling time (seems fine when testing)
  }
  else
  {
    ERRLN("LBT not support on this radio type");
    return 0;
  }
}

static int8_t ICACHE_RAM_ATTR PowerEnumToLBTLimit(PowerLevels_e txPower, uint8_t radio_type)
{
  // Calculated from EN 300 328, adjusted for 800kHz BW for sx1280
  // TL = -70 dBm/MHz + 10*log10(0.8MHz) + 10 × log10 (100 mW / Pout) (Pout in mW e.i.r.p.)
  // This threshold should be offset with a #define config for each HW that corrects for antenna gain,
  // different RF frontends.
  // TODO: Maybe individual adjustment offset for differences in
  // rssi reading between bandwidth setting is also necessary when other BW than 0.8MHz are used.

  if (radio_type == RADIO_TYPE_LORA)
  {
    switch(txPower)
    {
      case PWR_10mW: return -61 + LBT_RSSI_THRESHOLD_OFFSET_DB;
      case PWR_25mW: return -65 + LBT_RSSI_THRESHOLD_OFFSET_DB;
      case PWR_50mW: return -68 + LBT_RSSI_THRESHOLD_OFFSET_DB;
      case PWR_100mW: return -71 + LBT_RSSI_THRESHOLD_OFFSET_DB;
      // Values above 100mW are not relevant, default to 100mW threshold
      default: return -71 + LBT_RSSI_THRESHOLD_OFFSET_DB;
    }
  }
  else if (radio_type == RADIO_TYPE_FLRC)
  {
    switch(txPower)
    {
      case PWR_10mW: return -63 + LBT_RSSI_THRESHOLD_OFFSET_DB;
      case PWR_25mW: return -67 + LBT_RSSI_THRESHOLD_OFFSET_DB;
      case PWR_50mW: return -70 + LBT_RSSI_THRESHOLD_OFFSET_DB;
      case PWR_100mW: return -73 + LBT_RSSI_THRESHOLD_OFFSET_DB;
      // Values above 100mW are not relevant, default to 100mW threshold
      default: return -73 + LBT_RSSI_THRESHOLD_OFFSET_DB;
    }
  }
  else
  {
    ERRLN("LBT not support on this radio type");
    return 0;
  }
}

void ICACHE_RAM_ATTR SetClearChannelAssessmentTime(void)
{
  rxStartTime = micros();
}

void ICACHE_RAM_ATTR BeginClearChannelAssessment()
{
  if(!LBTStarted)
  {
    Radio.RXnb(SX1280_MODE_RX_CONT);
    if (LBTEnabled)
    {
      rxStartTime = micros();
      LBTStarted = true;
    }
  }
}

SX12XX_Radio_Number_t ICACHE_RAM_ATTR ChannelIsClear(SX12XX_Radio_Number_t radioNumber)
{
  LBTSuccessCalc.inc(); // Increment count for every channel check
  LBTStarted = false;

  if (!LBTEnabled)
  {
    LBTSuccessCalc.add();
    return SX12XX_Radio_All;
  }

  // Read rssi after waiting the minimum RSSI valid delay.
  // If this function is called long enough after RX enable,
  // this will always be ok on first try as is the case for TX.

  // TODO: Better way than busypolling this for RX?
  // this loop should only run for RX, where listen before talk RX is started right after FHSS hop
  // so there is no dead-time to run RX before telemetry TX is supposed to happen.
  // if flipping the logic, so telemetry TX happens right before FHSS hop, then TX-side ends up with polling here instead?
  // Maybe it could be skipped if there was only TX of telemetry happening when FHSShop does not happen.
  // Then RX for LBT could stay enabled from last received packet, and RSSI would be valid instantly.
  // But for now, FHSShops and telemetry rates does not divide evenly, so telemetry will some times happen
  // right after FHSS and we need wait here.

  uint32_t validRSSIdelayUs = SpreadingFactorToRSSIvalidDelayUs((SX1280_RadioLoRaSpreadingFactors_t)ExpressLRS_currAirRate_Modparams->sf,
      ExpressLRS_currAirRate_Modparams->radio_type);
  uint32_t elapsed = micros() - rxStartTime;
  if(elapsed < validRSSIdelayUs)
  {
    delayMicroseconds(validRSSIdelayUs - elapsed);
  }

  int8_t rssiInst = 0;
  SX12XX_Radio_Number_t clearChannelsMask = 0x00;

  if (radioNumber & SX12XX_Radio_1 || (Radio.GetLastSuccessfulPacketRadio() == SX12XX_Radio_1 && radioNumber == SX12XX_Radio_Default))
  {
    rssiInst = Radio.GetRssiInst(SX12XX_Radio_1);
    if(rssiInst < PowerEnumToLBTLimit((PowerLevels_e)POWERMGNT::currPower(), ExpressLRS_currAirRate_Modparams->radio_type))
    {
      clearChannelsMask |= SX12XX_Radio_1;
    }
  }

  if (radioNumber & SX12XX_Radio_2 || (Radio.GetLastSuccessfulPacketRadio() == SX12XX_Radio_2 && radioNumber == SX12XX_Radio_Default))
  {
    rssiInst = Radio.GetRssiInst(SX12XX_Radio_2);
    if(rssiInst < PowerEnumToLBTLimit((PowerLevels_e)POWERMGNT::currPower(), ExpressLRS_currAirRate_Modparams->radio_type))
    {
      clearChannelsMask |= SX12XX_Radio_2;
    }
  }
  
  // Useful to debug if and how long the rssi wait is, and rssi threshold level
  // DBGLN("wait: %d, rssi: %d, %s", validRSSIdelayUs - elapsed, rssiInst, clearChannelsMask ? "clear" : "in use");

  if(clearChannelsMask)
  {
    LBTSuccessCalc.add(); // Add success only when actually preparing for TX
  }

  return clearChannelsMask;
}
#endif
