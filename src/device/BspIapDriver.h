#ifndef __BSP_IAP_DRIVER_H_
#define __BSP_IAP_DRIVER_H_

#define INT8U     unsigned   char
#define INT32U    unsigned   int
#define IAP_ENTER_ADR   0x1FFF1FF1                      //IAP入口地址定义

#define IAP_ADDRESS_CALIB       0x00070000              //标校信息存储扇区
#define IAP_START_SECTOR_CALIB    28
#define IAP_END_SECTOR_CALIB      28

#define IAP_ADDRESS_CONFIG        0x00078000
#define IAP_START_SECTOR_CONFIG   29					//配置信息存储扇区
#define IAP_END_SECTOR_CONFIG     29



INT32U  sectorPrepare (INT8U ucSec1, INT8U ucSec2);
INT32U  ramCopy (INT32U ulDst, INT32U ulSrc, INT32U ulNo);
INT32U  sectorErase (INT8U ucSec1, INT8U ucSec2);
INT32U  blankChk (INT8U ucSec1, INT8U ucSec2);
INT32U  dataCompare (INT32U ulDst, INT32U ulSrc, INT32U ulNo);
INT32U  UIdRead (void);
 extern void (*IAP_Entry) (INT32U ulParam_tab[], INT32U ulPesult_tab[]);
void flashRead(INT32U *ulDst,INT32U ulSrc,INT32U ulNo);
unsigned int ReadUID(void);

/*
 *  定义IAP返回状态字
 */
#define CMD_SUCCESS                                0
#define INVALID_COMMAND                            1
#define SRC_ADDR_ERROR                             2 
#define DST_ADDR_ERROR                             3
#define SRC_ADDR_NOT_MAPPED                        4
#define DST_ADDR_NOT_MAPPED                        5
#define COUNT_ERROR                                6
#define INVALID_SECTOR                             7
#define SECTOR_NOT_BLANK                           8
#define SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION    9
#define COMPARE_ERROR                              10
#define BUSY   									   11
#endif

//////////////////////////////////////////////////////////////////
