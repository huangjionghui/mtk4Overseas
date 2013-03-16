#include <windows.h>
#include <commctrl.h>
#include <assert.h>
#include "flyshmlib.h"
#include <basetsd.h>

#include <errno.h>  
#include <stdio.h>
#include <assert.h>

#define BUF_LEN 100
#define BUF_LIMIT 256
#define FILE_LIMIT_SIZE 1024*10000*2 //20M
struct dbg_msg_info{   
	int g_fd;
	BYTE temp_buff[BUF_LEN];
	BYTE val_buff[BUF_LIMIT];
};


struct dbg_msg_info msg_info;

#define DBG_MSG_PATH L"\\APP_DISK\\fly_dbg_msg.txt"

static int is_already_exist(LPCWSTR wsPath)
{
	if (GetFileAttributes(wsPath) == -1)
		return -1;
	else
		return 0;
}

static unsigned long get_file_size(LPCWSTR wsPath)
{
	WIN32_FIND_DATA fileInfo; 
	HANDLE hFind; 
	DWORD fileSize = 0; 
	hFind = FindFirstFile(wsPath ,&fileInfo); 
	if(hFind != INVALID_HANDLE_VALUE) 
		fileSize = fileInfo.nFileSizeLow; 
	FindClose(hFind); 

	return fileSize;
}

static int write_msg_file(BYTE *buf, unsigned long len)
{
	FILE *file = fopen("\\APP_DISK\\fly_dbg_msg.txt","wb");
	if (file != NULL){
		fseek(file,0,SEEK_END);
		fwrite(buf,1,len,file);
		fclose(file);
	}

	return -1;
}


//
//void dbg_write_save_file(const char *path, BYTE *buf, UINT32 size)
//{
//	int fd = -1;
//	int flag = 0;
//
//	if (!dbg_is_already_exist(path)){
//		if (dbg_get_save_file_size(path) > FILE_LIMIT_SIZE){
//			dbg_close_save_file(dbg_info.g_fd);
//			dbg_info.g_fd = -1;
//			flag = O_TRUNC|O_CREAT|O_WRONLY;
//		}
//		else
//			flag = O_WRONLY;
//	}
//	else{
//		dbg_info.g_fd = -1;
//		flag = O_TRUNC|O_CREAT|O_WRONLY;
//	}
//
//	if (dbg_info.g_fd < 0)
//		dbg_info.g_fd = dbg_open_save_file(path,flag);
//
//
//	if (dbg_info.g_fd > 0){
//		lseek(dbg_info.g_fd, 0, SEEK_END);
//		dbg_write_to_save_file(dbg_info.g_fd,(BYTE*)buf, size);
//	}
//}