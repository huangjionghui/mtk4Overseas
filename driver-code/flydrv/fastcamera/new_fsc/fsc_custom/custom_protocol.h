#ifndef  __EMUL_COMM_H__
#define __EMUL_COMM_H__

#define  CONNECT_SIGNATURE   (0x7ADEFE8D)


//data transfer type
#define DT_UNKNOWN 0
#define DT_POINT  1
#define DT_ANGLE 2
#define DT_DISTANCE 3
#define DT_BINARY   4
#define DT_BITMAP  5
//data transfer status report
#define TSF_SUCCESS  0
#define TSF_FAIL     1

typedef struct _tagTransferBlockData
{
   UINT32 u4DataType;
   UINT32 u4DataLength;
   VOID   *pData;
}TransferBlockData;

typedef struct _tagTransferStatus
{

  UINT32 u4Status;

}TransferStatus;

#define BC_STATE_IDLE  0
#define BC_STATE_RX_DATA 1
#define BC_STATE_DRAW    2
#define BC_STATE_FIN     3
#define BC_STATE_END     4

#endif