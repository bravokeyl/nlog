/*
 * fat.c
 *
 *  Created on: Dec 19, 2013
 *      Author: Jed Frey
 */

/*===========================================================================*/
/* FatFs related.                                                            */
/*===========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "ch.h"
#include "hal.h"
#include "chrtclib.h"

#include "chprintf.h"
#include "shell.h"

//#include "usb_cdc.h"
#include "log.h"
#include "senddata.h"
#include "ff.h"


#define MAXLOGFILESIZE 5*1024*1024 // 5KB

/* Generic large buffer.*/
FATFS SDC_FS;

static char buffer[256];
static FIL logfile; /* File object */
static uint32_t byteswritten;
static uint32_t _filecount = 0; //the number of the file that receives log data

void newLogFile(BaseSequentialStream *chp);
void getfilename(const struct tm *timeptr, const char *buffer, uint32_t filecount);

/**
 *
 * init the log system: mount file system, and chk the existing of the log file, used as shell command
 *
 *
 */
void log_init(BaseSequentialStream *chp, int argc, char *argv[]) {
	FRESULT err;
	(void)argc;
	(void)argv;
	/*
	 * Attempt to mount the drive.
	 */
	err = f_mount(0, &SDC_FS);
	if (err != FR_OK) {
		chprintf(chp, "FS: f_mount() failed. Is the SD card inserted?\r\n");
		return;
	}
	chprintf(chp, "FS: f_mount() succeeded\r\n");
	palSetPad(GPIOD, GPIOD_LED6);
	sdcConnect(&SDCD1);

	memset(&logfile, 0, sizeof(FIL));

//	time_t unix_time = 1488030989;
//	rtcSetTimeUnixSec(&RTCD1, unix_time);

	newLogFile(chp);
}
/**
 * close the log file, umount the file system, used as shell command
 */
void log_exit(BaseSequentialStream *chp, int argc, char *argv[]) {
	FRESULT err;
	(void)argc;
	(void)argv;
	//close log file first
	if(logfile.fs != NULL){
		f_close(&logfile);
	}

	palClearPad(GPIOD, GPIOD_LED6);
	sdcDisconnect(&SDCD1);
	err = f_mount(0, NULL);
	if (err != FR_OK) {
		chprintf(chp, "FS: f_mount() unmount failed\r\n");
		return;
	}
	return;
}

/**
 * log template data to the file, used as shell command
 *
 */
void log_data(BaseSequentialStream *chp, int argc, char *argv[]){
	(void)argc;
	(void)argv;
	_log_data(chp, 0, NULL);
}

/**
 * save data to the log file, used as API
 */
void _log_data(BaseSequentialStream *chp, int len, char *data) {
	FRESULT err;
	//move to end of file
	uint32_t fsize = f_size(&logfile);

	if(fsize+len > MAXLOGFILESIZE){
		newLogFile(chp);
	}

	err = f_lseek(&logfile,f_size(&logfile));
	if(err!= FR_OK){
		chprintf(chp, "FS: failed to write data to the log file\r\n");
		return;
	}
	palTogglePad(GPIOD, GPIOD_LED6);
	if(data == NULL){
		data = buffer;
		len = strlen(data);
	}

	err = f_write(&logfile, data, len, (void *) &byteswritten);
	if(err!= FR_OK) chprintf(chp, "FS: failed to write data to the log file\r\n");
	else f_sync(&logfile);

	fsize = f_size(&logfile);
	if(chp) chprintf(chp, "FS: write len=%d, total len = %d\r\n", byteswritten, fsize);
}

/**
 * close the previous log file, and open a new log file
 */
void newLogFile(BaseSequentialStream *chp){
	//check the log file
	FRESULT err;
	FILINFO fno;

	if(logfile.fs != NULL){
		f_close(&logfile);
	}

	//check the file system is OK
	do{
		//generate file name in the buffer
		struct tm timp;
		rtcGetTimeTm(&RTCD1, &timp);
		getfilename(&timp, buffer, _filecount);
		chprintf(chp, "file name %s\r\n",buffer);
		err = f_stat(buffer, &fno);
		if (err == FR_OK) { //file exists
			_filecount++;
			continue;
		}else if(err == FR_NO_FILE){
			break;
		}
		chprintf(chp, "FS: f_stat() failed. Is the SD card inserted?\r\n");
		chThdSleep(MS2ST(500)); //sleep 500ms
	}while(1);


	if (f_open(&logfile, buffer, FA_OPEN_ALWAYS | FA_WRITE | FA_READ)
				!= FR_OK) {
		/* 'log_2000_1_1_*.csv' file Open for write Error */
		chprintf(chp, "FS: f_open() failed. Is the SD card inserted?\r\n");
		return;
	}

	sprintf(buffer,
			"Time,channel,V RMS,C RMS,EN_ACT, POW_ACT,V MOM,C MOM,V FUND,C FUND,V PERIOD,"
					"C PHASE SH,V SAG TIME,V SWELL TIME,C SWELL TIME,EN_REACT,"
					"EN_APP,POW_REACT,POW_APP,AHACC\n");

	err = f_write(&logfile, buffer, strlen(buffer), (void *) &byteswritten);
	if(err!= FR_OK){
		chprintf(chp, "FS: failed to write header to the log file\r\n");
	}else{
		chprintf(chp, "FS: write header (size = %d) to the log file\r\n", byteswritten);
		f_sync(&logfile);
	}

	_filecount++;
}

/**
 * get the log file name, put it into buffer
 *
 */
void getfilename(const struct tm *timeptr, const char *buffer, uint32_t filecount){
    static char mon_name[12][3] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    sprintf(buffer, "log_%d_%.3s_%d_%u.csv",
    	1900 + timeptr->tm_year,
        mon_name[timeptr->tm_mon],
        timeptr->tm_mday,
		filecount
        );
}
