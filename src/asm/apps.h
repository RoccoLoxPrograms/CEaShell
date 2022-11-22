/**
 * --------------------------------------
 * 
 * CEaShell Source Code - apps.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef APPS_H
#define APPS_H

#ifdef __cplusplus
extern "C" {
#endif

bool detectApp(char *, unsigned int *);

unsigned int getAppSize(char *);

char *getAppMinOSVersion(char *);

char *getAppCopyrightInfo(char *);

void executeApp(char *);

void deleteApp(char *);

void exitDefrag(void);

#ifdef __cplusplus
}
#endif

#endif
