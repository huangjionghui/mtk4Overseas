#ifndef  __MRF_H__
#define  __MRF_H__
#ifdef __cplusplus
extern "C" {
#endif

typedef  struct tagMRFFILEHEADER{

	wchar_t      szFileSignature[4];
	unsigned int u4Version;
	unsigned int u4Reserved1;
	unsigned int u4Reserved2;
	unsigned int u4Reserved3;
	unsigned int uReserved4;
	unsigned int u4OffsetHashTable;
	unsigned int u4OffsetData;
}MRFHEADER;

#define  OBJ_TYPE_BITMAP    1
#define  OBJ_TYPE_BIN       2
#define  OBJ_TYPE_WAV       3



typedef  struct  tagRCOBJECT {
	unsigned  int u4Size;
	unsigned  int objType;
    wchar_t szImageFileName[8];
	unsigned int u4FileID;
	unsigned int u4OffsetData;
	unsigned int u4SizeImage;
	unsigned int u4OffsetNextImage;
}RCOBJECT;


typedef struct tagBITMAPOBJINFO{


	unsigned  int u4Size;
	unsigned  int objType;
	wchar_t szImageFileName[8];
	unsigned int u4FileID;
	unsigned int u4OffsetData;
	unsigned int u4SizeImage;
	unsigned int u4OffsetNextImage;

	unsigned int u4Width;
	unsigned int u4Height;
	unsigned int u4BitCount;
	
}BITMAPOBJINFO;


typedef struct tagBINFILEOBJINFO{


	unsigned  int u4Size;
	unsigned  int objType;
	wchar_t szImageFileName[8];
	unsigned int u4FileID;
	unsigned int u4OffsetData;
	unsigned int u4SizeImage;
	unsigned int u4OffsetNextImage;

}BINFILEOBJINFO;


typedef struct tagWAVOBJINFO{


	unsigned  int u4Size;
	unsigned  int objType;
	wchar_t szImageFileName[8];
	unsigned int u4FileID;
	unsigned int u4OffsetData;
	unsigned int u4SizeImage;
	unsigned int u4OffsetNextImage;

}WAVOBJINFO;


void*   GetRCObjectMemAddr(HANDLE hMrf,RCOBJECT *pObjInfo);
BOOL GetWavResnfo(HANDLE hMrf,DWORD dwID,WAVOBJINFO *pInfo);
BOOL GetBitmapInfo(HANDLE hMrf,DWORD dwID,BITMAPOBJINFO *pInfo);
HANDLE LoadMRF();



#ifdef __cplusplus
}
#endif


#endif
