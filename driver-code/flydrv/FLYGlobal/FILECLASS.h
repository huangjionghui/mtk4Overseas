class NDFlash
{
public:
	NDFlash();

	BOOL OpenFlashFile(void);
	void CloseFlashFile(void);

	unsigned int GetDatalength(unsigned int type);
	BOOL InqureDevice(unsigned int type);

	BOOL ReadData(unsigned int type,unsigned char *pbuf,unsigned int size);
	BOOL WriteData(unsigned int type,unsigned char *pbuf,unsigned int size);


private:
	HANDLE H_FLASH;//= INVALID_HANDLE_VALUE;
};

#define AUTOTYPE				0		//默认类型
#define MAXSKINNUMBER			20		//默认类型

#define SKIN_CODE				1		//皮肤
#define S_W_CODE				2		//方向盘数据
#define PANELSTEER_CODE			3		//面板与默认方向盘数据
#define NK_CONFIG_CODE			4		//NK_CONFIG LCD与触摸屏数据
#define UPDATESYMBOL			5		//升级版本存储

//操作类型
#define	INQURE				(1<<0)		//查询
#define	READBUF				(1<<1)		//读BUF
#define	WRITEBUF			(1<<2)		//写BUF
#define	GETLEN				(1<<3)		//得出长度


#define NDF_CODE( DataType,SubType,Function) (     \
	((DataType) << 24) | ((SubType) << 4) | (Function ) \
	)
//皮肤
#define GET_MINSKIN_LEN		NDF_CODE(SKIN_CODE,AUTOTYPE,GETLEN)
#define INQURE_MINSKIN			NDF_CODE(SKIN_CODE,AUTOTYPE,INQURE)
#define READ_MINSKIN_BUF		NDF_CODE(SKIN_CODE,AUTOTYPE,READBUF)

#define GET_MAXSKIN_LEN		NDF_CODE(SKIN_CODE,MAXSKINNUMBER,GETLEN)
#define INQURE_MAXKIN			NDF_CODE(SKIN_CODE,MAXSKINNUMBER,INQURE)
#define READ_MAXSKIN_BUF		NDF_CODE(SKIN_CODE,MAXSKINNUMBER,READBUF)

//方向盘与版本号
#define GET_S_W_LEN			NDF_CODE(S_W_CODE,AUTOTYPE,GETLEN)
#define INQURE_S_W			NDF_CODE(S_W_CODE,AUTOTYPE,INQURE)
#define READ_S_W_BUF			NDF_CODE(S_W_CODE,AUTOTYPE,READBUF)

//面板与默认方向盘
#define GET_PANELSTEER_CODE_LEN		NDF_CODE(PANELSTEER_CODE,AUTOTYPE,GETLEN)
#define INQURE_PANELSTEER_CODE		NDF_CODE(PANELSTEER_CODE,AUTOTYPE,INQURE)
#define READ_PANELSTEER_CODE_BUF	NDF_CODE(PANELSTEER_CODE,AUTOTYPE,READBUF)

//皮肤
#define GET_MINSKIN_LEN			NDF_CODE(SKIN_CODE,AUTOTYPE,GETLEN)
#define INQURE_MINSKIN			NDF_CODE(SKIN_CODE,AUTOTYPE,INQURE)
#define READ_MINSKIN_BUF		NDF_CODE(SKIN_CODE,AUTOTYPE,READBUF)

#define GET_MAXSKIN_LEN			NDF_CODE(SKIN_CODE,MAXSKINNUMBER,GETLEN)
#define INQURE_MAXKIN			NDF_CODE(SKIN_CODE,MAXSKINNUMBER,INQURE)
#define READ_MAXSKIN_BUF		NDF_CODE(SKIN_CODE,MAXSKINNUMBER,READBUF)

//进入UPDATE模式
#define SET_UPDATEMODE			NDF_CODE(UPDATESYMBOL,AUTOTYPE,INQURE)
