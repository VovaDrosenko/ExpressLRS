#include "FHSS.h"
#include "logging.h"
#include "options.h"
#include <string.h>

#if defined(RADIO_SX127X) || defined(RADIO_LR1121)

#if defined(RADIO_LR1121)
#include "LR1121Driver.h"
#else
#include "SX127xDriver.h"
#endif

const fhss_config_t domains[] = {
    {"FCC915", FREQ_HZ_TO_REG_VAL(903500000), FREQ_HZ_TO_REG_VAL(926900000), 40},

    {"UA210",  FREQ_HZ_TO_REG_VAL(198300000), FREQ_HZ_TO_REG_VAL(221700000), 20},
    {"UA230",  FREQ_HZ_TO_REG_VAL(218300000), FREQ_HZ_TO_REG_VAL(241700000), 20},
    {"UA250",  FREQ_HZ_TO_REG_VAL(238300000), FREQ_HZ_TO_REG_VAL(261700000), 20},
    {"UA270",  FREQ_HZ_TO_REG_VAL(258300000), FREQ_HZ_TO_REG_VAL(281700000), 20},
    {"UA290",  FREQ_HZ_TO_REG_VAL(278300000), FREQ_HZ_TO_REG_VAL(301700000), 20},

    {"UA310",  FREQ_HZ_TO_REG_VAL(298300000), FREQ_HZ_TO_REG_VAL(321700000), 20},
    {"UA330",  FREQ_HZ_TO_REG_VAL(318300000), FREQ_HZ_TO_REG_VAL(341700000), 20},
    {"UA350",  FREQ_HZ_TO_REG_VAL(338300000), FREQ_HZ_TO_REG_VAL(361700000), 20},
    {"UA370",  FREQ_HZ_TO_REG_VAL(358300000), FREQ_HZ_TO_REG_VAL(381700000), 20},
    {"UA390",  FREQ_HZ_TO_REG_VAL(378300000), FREQ_HZ_TO_REG_VAL(401700000), 20},

    {"UA410",  FREQ_HZ_TO_REG_VAL(398300000), FREQ_HZ_TO_REG_VAL(421700000), 20},
    {"UA430",  FREQ_HZ_TO_REG_VAL(418300000), FREQ_HZ_TO_REG_VAL(451700000), 20},
    {"UA450",  FREQ_HZ_TO_REG_VAL(438300000), FREQ_HZ_TO_REG_VAL(461700000), 20},
    {"UA470",  FREQ_HZ_TO_REG_VAL(458300000), FREQ_HZ_TO_REG_VAL(481700000), 20},
    {"UA490",  FREQ_HZ_TO_REG_VAL(478300000), FREQ_HZ_TO_REG_VAL(501700000), 20},

    {"UA510",  FREQ_HZ_TO_REG_VAL(498300000), FREQ_HZ_TO_REG_VAL(521700000), 20},
    {"UA530",  FREQ_HZ_TO_REG_VAL(520000000), FREQ_HZ_TO_REG_VAL(540000000), 20},
    {"UA550",  FREQ_HZ_TO_REG_VAL(538300000), FREQ_HZ_TO_REG_VAL(561700000), 20},
    {"UA570",  FREQ_HZ_TO_REG_VAL(558300000), FREQ_HZ_TO_REG_VAL(581700000), 20},
    {"UA590",  FREQ_HZ_TO_REG_VAL(578300000), FREQ_HZ_TO_REG_VAL(601700000), 20},

    {"UA610",  FREQ_HZ_TO_REG_VAL(598300000), FREQ_HZ_TO_REG_VAL(621700000), 20},
    {"UA630",  FREQ_HZ_TO_REG_VAL(618300000), FREQ_HZ_TO_REG_VAL(641700000), 20},
    {"UA650",  FREQ_HZ_TO_REG_VAL(638300000), FREQ_HZ_TO_REG_VAL(661700000), 20},
    {"UA670",  FREQ_HZ_TO_REG_VAL(658300000), FREQ_HZ_TO_REG_VAL(681700000), 20},
    {"UA690",  FREQ_HZ_TO_REG_VAL(678300000), FREQ_HZ_TO_REG_VAL(701700000), 20},

    {"UA710",  FREQ_HZ_TO_REG_VAL(698300000), FREQ_HZ_TO_REG_VAL(721700000), 20},                                   
    {"UA730",  FREQ_HZ_TO_REG_VAL(718300000), FREQ_HZ_TO_REG_VAL(741700000), 20},
    {"UA750",  FREQ_HZ_TO_REG_VAL(738300000), FREQ_HZ_TO_REG_VAL(761700000), 20},
    {"UA770",  FREQ_HZ_TO_REG_VAL(758300000), FREQ_HZ_TO_REG_VAL(781700000), 20},
    {"UA790",  FREQ_HZ_TO_REG_VAL(778300000), FREQ_HZ_TO_REG_VAL(801700000), 40},

    {"UA810",  FREQ_HZ_TO_REG_VAL(798300000), FREQ_HZ_TO_REG_VAL(821700000), 40},
    {"UA830",  FREQ_HZ_TO_REG_VAL(818300000), FREQ_HZ_TO_REG_VAL(841700000), 40},
    {"UA850",  FREQ_HZ_TO_REG_VAL(838300000), FREQ_HZ_TO_REG_VAL(861700000), 40},
    {"UA870",  FREQ_HZ_TO_REG_VAL(858300000), FREQ_HZ_TO_REG_VAL(881700000), 40},
    {"UA890",  FREQ_HZ_TO_REG_VAL(878300000), FREQ_HZ_TO_REG_VAL(901700000), 40},

    {"UA910",  FREQ_HZ_TO_REG_VAL(898300000), FREQ_HZ_TO_REG_VAL(921700000), 40},
    {"UA930",  FREQ_HZ_TO_REG_VAL(918300000), FREQ_HZ_TO_REG_VAL(941700000), 40},
    {"UA950",  FREQ_HZ_TO_REG_VAL(938300000), FREQ_HZ_TO_REG_VAL(961700000), 40},
    {"UA970",  FREQ_HZ_TO_REG_VAL(958300000), FREQ_HZ_TO_REG_VAL(981700000), 40},
    {"UA990",  FREQ_HZ_TO_REG_VAL(978300000), FREQ_HZ_TO_REG_VAL(1001700000), 40},

    {"UA1010",  FREQ_HZ_TO_REG_VAL(998300000), FREQ_HZ_TO_REG_VAL(1021700000), 40},
    {"UA1030",  FREQ_HZ_TO_REG_VAL(1018300000), FREQ_HZ_TO_REG_VAL(1041700000), 40},
    {"UA1050",  FREQ_HZ_TO_REG_VAL(1038300000), FREQ_HZ_TO_REG_VAL(1061700000), 40},
    {"UA1070",  FREQ_HZ_TO_REG_VAL(1058300000), FREQ_HZ_TO_REG_VAL(1081700000), 40},
    {"UA1090",  FREQ_HZ_TO_REG_VAL(1078300000), FREQ_HZ_TO_REG_VAL(1101700000), 40},
    
    {"UA1110",  FREQ_HZ_TO_REG_VAL(1098300000), FREQ_HZ_TO_REG_VAL(1121700000), 40},
    {"UA1130",  FREQ_HZ_TO_REG_VAL(1118300000), FREQ_HZ_TO_REG_VAL(1141700000), 40},
    {"UA1150",  FREQ_HZ_TO_REG_VAL(1138300000), FREQ_HZ_TO_REG_VAL(1161700000), 40},
    {"UA1170",  FREQ_HZ_TO_REG_VAL(1158300000), FREQ_HZ_TO_REG_VAL(1181700000), 40},
    {"UA1190",  FREQ_HZ_TO_REG_VAL(1178300000), FREQ_HZ_TO_REG_VAL(1201700000), 40},
};

#if defined(RADIO_LR1121)
const fhss_config_t domainsDualBand[] = {
    {"UA2100", FREQ_HZ_TO_REG_VAL(2100400000), FREQ_HZ_TO_REG_VAL(2179400000), 80, 2140000000},
    {"UA2400", FREQ_HZ_TO_REG_VAL(2400400000), FREQ_HZ_TO_REG_VAL(2479400000), 80, 2440000000},
    {"UA2500", FREQ_HZ_TO_REG_VAL(2500400000), FREQ_HZ_TO_REG_VAL(2579400000), 80, 2540000000}
};
#endif

#elif defined(RADIO_SX128X)
#include "SX1280Driver.h"

const fhss_config_t domains[] = {
    {
    #if defined(Regulatory_Domain_EU_CE_2400)
        "CE_LBT",
    #elif defined(Regulatory_Domain_ISM_2400)
        "ISM2G4",
    #endif
    FREQ_HZ_TO_REG_VAL(2400400000), FREQ_HZ_TO_REG_VAL(2479400000), 80, 2440000000}
};
#endif

// Our table of FHSS frequencies. Define a regulatory domain to select the correct set for your location and radio
const fhss_config_t *FHSSconfig;
const fhss_config_t *FHSSconfigDualBand;

// Actual sequence of hops as indexes into the frequency list
uint8_t FHSSsequence[FHSS_SEQUENCE_LEN];
uint8_t FHSSsequence_DualBand[FHSS_SEQUENCE_LEN];

// Which entry in the sequence we currently are on
uint8_t volatile FHSSptr;

// Channel for sync packets and initial connection establishment
uint_fast8_t sync_channel;
uint_fast8_t sync_channel_DualBand;

// Offset from the predefined frequency determined by AFC on Team900 (register units)
int32_t FreqCorrection;
int32_t FreqCorrection_2;

// Frequency hop separation
uint32_t freq_spread;
uint32_t freq_spread_DualBand;

// Variable for Dual Band radios
bool FHSSusePrimaryFreqBand = true;
bool FHSSuseDualBand = false;

uint16_t primaryBandCount;
uint16_t secondaryBandCount;

void FHSSrandomiseFHSSsequence(const uint32_t seed)
{
    FHSSconfig = &domains[firmwareOptions.domain];
    sync_channel = (FHSSconfig->freq_count / 2) + 1;
    freq_spread = (FHSSconfig->freq_stop - FHSSconfig->freq_start) * FREQ_SPREAD_SCALE / (FHSSconfig->freq_count - 1);
    primaryBandCount = (FHSS_SEQUENCE_LEN / FHSSconfig->freq_count) * FHSSconfig->freq_count;

    DBGLN("Setting %s Mode", FHSSconfig->domain);
    DBGLN("Number of FHSS frequencies = %u", FHSSconfig->freq_count);
    DBGLN("Sync channel = %u", sync_channel);

    FHSSrandomiseFHSSsequenceBuild(seed, FHSSconfig->freq_count, sync_channel, FHSSsequence);

#if defined(RADIO_LR1121)
    FHSSconfigDualBand = &domainsDualBand[firmwareOptions.domain2];
    sync_channel_DualBand = (FHSSconfigDualBand->freq_count / 2) + 1;
    freq_spread_DualBand = (FHSSconfigDualBand->freq_stop - FHSSconfigDualBand->freq_start) * FREQ_SPREAD_SCALE / (FHSSconfigDualBand->freq_count - 1);
    secondaryBandCount = (FHSS_SEQUENCE_LEN / FHSSconfigDualBand->freq_count) * FHSSconfigDualBand->freq_count;

    DBGLN("Setting Dual Band %s Mode", FHSSconfigDualBand->domain);
    DBGLN("Number of FHSS frequencies = %u", FHSSconfigDualBand->freq_count);
    DBGLN("Sync channel Dual Band = %u", sync_channel_DualBand);

    FHSSusePrimaryFreqBand = false;
    FHSSrandomiseFHSSsequenceBuild(seed, FHSSconfigDualBand->freq_count, sync_channel_DualBand, FHSSsequence_DualBand);
    FHSSusePrimaryFreqBand = true;
#endif
}

/**
Requirements:
1. 0 every n hops
2. No two repeated channels
3. Equal occurance of each (or as even as possible) of each channel
4. Pseudorandom

Approach:
  Fill the sequence array with the sync channel every FHSS_FREQ_CNT
  Iterate through the array, and for each block, swap each entry in it with
  another random entry, excluding the sync channel.

*/
void FHSSrandomiseFHSSsequenceBuild(const uint32_t seed, uint32_t freqCount, uint_fast8_t syncChannel, uint8_t *inSequence)
{
    // reset the pointer (otherwise the tests fail)
    FHSSptr = 0;
    rngSeed(seed);

    // initialize the sequence array
    for (uint16_t i = 0; i < FHSSgetSequenceCount(); i++)
    {
        if (i % freqCount == 0) {
            inSequence[i] = syncChannel;
        } else if (i % freqCount == syncChannel) {
            inSequence[i] = 0;
        } else {
            inSequence[i] = i % freqCount;
        }
    }

    for (uint16_t i = 0; i < FHSSgetSequenceCount(); i++)
    {
        // if it's not the sync channel
        if (i % freqCount != 0)
        {
            uint8_t offset = (i / freqCount) * freqCount;   // offset to start of current block
            uint8_t rand = rngN(freqCount - 1) + 1;         // random number between 1 and FHSS_FREQ_CNT

            // switch this entry and another random entry in the same block
            uint8_t temp = inSequence[i];
            inSequence[i] = inSequence[offset+rand];
            inSequence[offset+rand] = temp;
        }
    }

    // output FHSS sequence
    for (uint16_t i=0; i < FHSSgetSequenceCount(); i++)
    {
        DBG("%u ",inSequence[i]);
        if (i % 10 == 9)
            DBGCR;
    }
    DBGCR;
}

bool isDomain868()
{
    return strcmp(FHSSconfig->domain, "EU868") == 0;
}
