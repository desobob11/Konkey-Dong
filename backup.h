/**
 * Definition for backup frame buffer
*/
#include 

#ifndef BACKUP_H_
#define BACKUP_H_

extern unsigned backbuffer [1280][720];

void drawImageBackup(int x, int y, int img_id);
void init_backup();
#endif