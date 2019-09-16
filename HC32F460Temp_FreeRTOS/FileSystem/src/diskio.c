#include "diskio.h"
#include "User_SDIO.h"

DSTATUS disk_initialize (BYTE pdrv)
{
    DSTATUS res;
    switch(pdrv)
    {
        case SD_Card:
            res = (DSTATUS)SD_CARD_Init();
            if(res== RES_OK)
            {
                return res;
            }
            return RES_ERROR;
 //       break;
    }
    return RES_PARERR;
}
DSTATUS disk_status (BYTE pdrv)
{
    switch(pdrv)
    {
        case SD_Card:
            return RES_OK;
//            break;
    }
    return RES_PARERR;
}
DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, BYTE count)
{
    DRESULT res;
    switch(pdrv)
    {
        case SD_Card:
            res = (DRESULT)FS_SD_ReadBlocks(buff,sector,count);
            if(res== RES_OK)
            {
                return res;
            }
            return RES_ERROR;
//            break;
    }
   return RES_PARERR;//(DRESULT)SDCARD_ReadBlocks()
}
DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, BYTE count)
{
    DRESULT res;
    switch(pdrv)
    {
        case SD_Card:
            res = (DRESULT)FS_SD_WriteBlocks(buff,sector,count);
            if(res== RES_OK)
            {
                return res;
            }
            return RES_ERROR;
 //           break;
    }
    return RES_PARERR;
}
DRESULT disk_ioctl (BYTE pdrv, BYTE ctrl, void* buff)
{
    switch(pdrv)
    {
        case SD_Card:
            return RES_OK;
 //           break;
    }
    return RES_PARERR;
}


