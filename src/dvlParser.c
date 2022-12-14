#include <stdio.h>
#include <stdlib.h>
/****************************************************************************/
/* Data ID Words */
/****************************************************************************/
#define FLdrSelected 0x0000
#define VLdrSelected 0x0080
#define VelSelected 0x0100
#define CorSelected 0x0200
#define AmpSelected 0x0300
#define PctSelected 0x0400
#define SttSelected 0x0500
#define BotSelected 0x0600
#define Prm0 0x0700
#define VelGood 0x0701
#define VelSum 0x0702
#define VelSumSqr 0x0703
#define Bm5VelSelected 0x0A00
#define Bm5CorSelected 0x0B00
#define Bm5AmpSelected 0x0C00
#define AmbientData 0x0C02
#define Bm5PctSelected 0x0D00
#define Bm5SttSelected 0x0E00
#define Prm0_5 0x1300
#define VelGood_5 0x1301
#define VelSum_5 0x1302
#define VelSumSqr_5 0x1303
/****************************************************************************/
/* structures */
/****************************************************************************/
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;
 
typedef struct {
    uchar Minute,
    Second,
    Sec100;
} TimeType;

typedef struct {
    uchar Year,
    Month,
    Day,
    Hour,
    Minute,
    Second,
    Sec100;
} DateTimeType;
 
typedef struct {
    uchar Version,
    Revision;
} VersionType;
 
 typedef struct {
    uchar ID,
    DataSource;
    ushort ChecksumOffset;
    uchar Spare,
    NDataTypes;
    ushort Offset [256];
 } HeaderType;

typedef struct {
    ushort ID;
    VersionType CPUFirmware;
    ushort Configuration;
    uchar DummyDataFlag,
    Lag,
    NBeams,
    NBins;
    ushort PingsPerEnsemble,
    BinLength,
    BlankAfterTransmit;
    uchar ProfilingMode,
    PctCorrelationLow,
    NCodeRepetitions,
    PctGoodMin;
    ushort ErrVelocityMax;
    TimeType TimeBetweenPings;
    uchar CoordSystemParms;
    short HeadingAlignment,
    HeadingBias;
    uchar SensorSource,
    AvailableSensors;
    ushort DistanceToBin1Middle,
    TransmitLength;
} FixLeaderType;
 
typedef struct {
    ushort ID,
    EnsembleNumber;
    DateTimeType RecordingTime;
    uchar Spare1;
    ushort BITResult,
    SpeedOfSound,
    Depth,
    Heading;
    short Pitch,
    Roll;
    ushort Salinity;
    short Temperature;
    TimeType MaxTimeBetweenPings;
    uchar HeadingStddev,
    PitchStddev,
    RollStddev;
    uchar VMeas [8];
} VarLeaderType;

typedef struct {
    ushort ID,
    PingsPerEnsemble,
    EnsembleDelay;
    uchar CorrelationMin,
    AmplitudeMin,
    PctGoodMin,
    BTMode;
    ushort ErrVelocityMax,
    NSearchPings,
    NTrackPings;
    ushort Range [4];
    short Velocity [4];
    uchar Correlation [4],
    Amplitude [4],
    PctGood [4];
    ushort WaterLayerMin,
    WaterLayerNear,
    WaterLayerFar;
    short WVelocity [4];
    uchar WCorrelation [4],
    WAmplitude [4],
    WPctGood [4];
    ushort MaxTrackingDepth;
    uchar Amp [4];
    uchar Gain;
    uchar RangeMSB [4];
} BottomTrackType;
 
typedef struct
{
    ushort ID;
    short Data [256];
} OneBeamShortType;
 
typedef struct
{
    ushort ID;
    uchar Data [256];
} OneBeamUcharType;

typedef struct {
    ushort ID;
    short Data [1024];
} IntStructType;

typedef struct {
    ushort ID;
    uchar Data [1024];
} ByteStructType;

typedef struct
{
    ushort ID;
    uchar Data [4];
} AmbientType;

typedef struct
{
    ushort ID;
    ushort UaH;
    ushort UaL;
    ushort AmbBitsPerBin;
    ushort AmbTrys;
    ushort AmbNBins;
    short AmbBinNum [ 5 ];
    short Est [ 5 ];
    ushort WAutoCor [ 5 ] [ 32 ];
    uchar SysFreq;
    uchar SampRate;
} T01Type;

typedef struct {
    ushort ID;
    uchar DAC [36];
} T02Type;
 
typedef struct
{
    ushort ID;
    ushort RSSIBinLen;
    ushort RSSIBins;
    uchar RSSI [512] [4];
    ushort AutoCor [32] [4];
    short Est [4];
    ushort Amb [4];
    uchar SysFreq;
    uchar SampRate;
    uchar MLen;
    ushort XmtSamples;
    ushort FirstBin[4];
    ushort LastBin[4];
    ulong BM6Depth[4];
    ushort BM6Ta[4];
} T03Type;
/****************************************************************************/
/* Global Pointers */
/****************************************************************************/
HeaderType *HdrPtr;
FixLeaderType *FLdrPtr;
VarLeaderType *VLdrPtr;
BottomTrackType *BotPtr;
BottomTrackType *WBotPtr;
IntStructType *VelPtr;
ByteStructType *CorPtr;
ByteStructType *AmpPtr;
ByteStructType *PctPtr;
ByteStructType *SttPtr;
AmbientType *AmbientPtr;
T01Type *T01Ptr;
T02Type *T02Ptr;
T03Type *T03Ptr;
OneBeamShortType *Bm5VelPtr;
OneBeamUcharType *Bm5CorPtr;
OneBeamUcharType *Bm5AmpPtr;
OneBeamUcharType *Bm5PctPtr;
OneBeamUcharType *Bm5SttPtr;
/*--------------------------------------------------------------------------*/
unsigned char RcvBuff[8192];
void DecodeBBensemble( void )
{
    unsigned short i, *IDptr, ID;

    FLdrPtr = (FixLeaderType *)&RcvBuff [ HdrPtr->Offset[0] ];

    if (FLdrPtr->NBins > 128) FLdrPtr->NBins = 32;

    for (i=1; i<HdrPtr->NDataTypes; i++)
    {
        IDptr = (unsigned short *)&RcvBuff [ HdrPtr->Offset [i] ];
        ID = IDptr[0];

        switch (ID)
        {
            case VLdrSelected:
            {
                VLdrPtr = (VarLeaderType *)&RcvBuff [ HdrPtr->Offset [i] ];
                break;
            }
            case VelSelected:
            {
                VelPtr = (IntStructType *)&RcvBuff [ HdrPtr->Offset [i] ];
                break;
            }
            case CorSelected :
            {
                CorPtr = (ByteStructType *)&RcvBuff [ HdrPtr->Offset [i] ];
                break;
            }
            case AmpSelected :
            {
                AmpPtr = (ByteStructType *)&RcvBuff [ HdrPtr->Offset [i] ];
                break;
            }
            case PctSelected :
            {
                PctPtr = (ByteStructType *)&RcvBuff [ HdrPtr->Offset [i] ];
                break;
            }
            case SttSelected :
            {
                SttPtr = (ByteStructType *)&RcvBuff [ HdrPtr->Offset [i] ];
                break;
            }
            case BotSelected :
            {
                BotPtr = (BottomTrackType*)&RcvBuff [ HdrPtr->Offset [i] ];
                break;
            }
            case AmbientData :
            {
                AmbientPtr = (AmbientType *)&RcvBuff [ HdrPtr->Offset [i] ];
                break;
            }
        }
    }
}

int main ()
{
    DecodeBBensemble();
    return 0;
} 