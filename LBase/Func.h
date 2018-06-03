#ifndef __FUNC_H_
#define __FUNC_H_

#include <stddef.h>

void daemonize(char *dir); 
void SetCoreFileUnlimit();
void SetSockFileSize(size_t size); 
void SetProcessStackSize(size_t size);
int WriteProcessPID(const char *dir, const char* filename);

#endif
