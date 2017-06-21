#pragma once
#include "ViType.h"
// zhan's
typedef struct
{
	IMG_INT  *ptr;
	IMG_SIZE   size;
	IMG_UWORD  linestep; /* offset from one row of image buffer to
						 another on the same column in term of pixel */
} IMG_INTBUF;
class VisBuf
{
public:
	VisBuf();
	~VisBuf();

	int set_IMG_INTBUF(IMG_INTBUF &srcBuf, IMG_INT *ptr, IMG_SIZE size, IMG_UWORD  linestep);
	int set_IMG_UBBUF(IMG_UBBUF &srcBuf, IMG_UBYTE *ptr, IMG_SIZE size, IMG_UWORD  linestep);
	int set_IMG_WBUF(IMG_WBUF &srcBuf, IMG_WORD  *ptr, IMG_SIZE  size, IMG_UWORD linestep);
	int set_IMG_RBUF(IMG_RBUF &srcBuf, IMG_REAL  *ptr, IMG_SIZE  size, IMG_UWORD linestep);
	int set_IMG_UWBUF(IMG_UWBUF & srcBuf, IMG_UWORD * ptr, IMG_SIZE size, IMG_UWORD linestep);
private:

};