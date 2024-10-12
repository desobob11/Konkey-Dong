/**
 * Definition for backup frame buffer
*/


#ifndef BACKUP_H_
#define BACKUP_H_

#include "image.h"
#include "framebuffer.h"

extern uint32_t backbuffer[1280][720];

void drawImageBackup(int x, int y, int img_id);
void init_backup();
#endif