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

#define AUTOTYPE				0		//Ĭ������
#define MAXSKINNUMBER			20		//Ĭ������

#define SKIN_CODE				1		//Ƥ��
#define S_W_CODE				2		//����������
#define PANELSTEER_CODE			3		//�����Ĭ�Ϸ���������
#define NK_CONFIG_CODE			4		//NK_CONFIG LCD�봥��������
#define UPDATESYMBOL			5		//�����汾�洢

//��������
#define	INQURE				(1<<0)		//��ѯ
#define	READBUF				(1<<1)		//��BUF
#define	WRITEBUF			(1<<2)		//дBUF
#define	GETLEN				(1<<3)		//�ó�����


#define NDF_CODE( DataType,SubType,Function) (     \
	((DataType) << 24) | ((SubType) << 4) | (Function ) \
	)
//Ƥ��
#define GET_MINSKIN_LEN		NDF_CODE(SKIN_CODE,AUTOTYPE,GETLEN)
#define INQURE_MINSKIN			NDF_CODE(SKIN_CODE,AUTOTYPE,INQURE)
#define READ_MINSKIN_BUF		NDF_CODE(SKIN_CODE,AUTOTYPE,READBUF)

#define GET_MAXSKIN_LEN		NDF_CODE(SKIN_CODE,MAXSKINNUMBER,GETLEN)
#define INQURE_MAXKIN			NDF_CODE(SKIN_CODE,MAXSKINNUMBER,INQURE)
#define READ_MAXSKIN_BUF		NDF_CODE(SKIN_CODE,MAXSKINNUMBER,READBUF)

//��������汾��
#define GET_S_W_LEN			NDF_CODE(S_W_CODE,AUTOTYPE,GETLEN)
#define INQURE_S_W			NDF_CODE(S_W_CODE,AUTOTYPE,INQURE)
#define READ_S_W_BUF			NDF_CODE(S_W_CODE,AUTOTYPE,READBUF)

//�����Ĭ�Ϸ�����
#define GET_PANELSTEER_CODE_LEN		NDF_CODE(PANELSTEER_CODE,AUTOTYPE,GETLEN)
#define INQURE_PANELSTEER_CODE		NDF_CODE(PANELSTEER_CODE,AUTOTYPE,INQURE)
#define READ_PANELSTEER_CODE_BUF	NDF_CODE(PANELSTEER_CODE,AUTOTYPE,READBUF)

//Ƥ��
#define GET_MINSKIN_LEN			NDF_CODE(SKIN_CODE,AUTOTYPE,GETLEN)
#define INQURE_MINSKIN			NDF_CODE(SKIN_CODE,AUTOTYPE,INQURE)
#define READ_MINSKIN_BUF		NDF_CODE(SKIN_CODE,AUTOTYPE,READBUF)

#define GET_MAXSKIN_LEN			NDF_CODE(SKIN_CODE,MAXSKINNUMBER,GETLEN)
#define INQURE_MAXKIN			NDF_CODE(SKIN_CODE,MAXSKINNUMBER,INQURE)
#define READ_MAXSKIN_BUF		NDF_CODE(SKIN_CODE,MAXSKINNUMBER,READBUF)

//����UPDATEģʽ
#define SET_UPDATEMODE			NDF_CODE(UPDATESYMBOL,AUTOTYPE,INQURE)
