/*
 * fat.h
 *
 *  Created on: Dec 19, 2013
 *      Author: Jed Frey
 */

#include "ff.h"

#ifndef FAT_H_
#define FAT_H_

/**
 * @brief FS object.
 */

extern FATFS SDC_FS;
void log_init(BaseSequentialStream *chp, int argc, char *argv[]);
void log_exit(BaseSequentialStream *chp, int argc, char *argv[]);
void log_data(BaseSequentialStream *chp, int argc, char *argv[]);

void _log_data(BaseSequentialStream *chp, int len, char *data);
#endif /* FAT_H_ */
