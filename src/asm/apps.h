/**
 * --------------------------------------
 * 
 * CEaShell Source Code - apps.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2023
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef APPS_H
#define APPS_H

#ifdef __cplusplus
extern "C" {
#endif

bool detectApp(char *name, unsigned int *pointer);

unsigned int getAppSize(char *name);

char *getAppMinOSVersion(char *name);

char *getAppCopyrightInfo(char *name);

void executeApp(char *name);

void deleteApp(char *name);

void exitDefrag(void);

#ifdef __cplusplus
}
#endif

#endif
