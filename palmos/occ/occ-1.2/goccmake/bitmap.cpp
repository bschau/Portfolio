//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include "bitmap.h"

//---------------------------------------------------------------------------
#define BMPBITSSTARTOFFSET 10
#define BMPWIDTHOFFSET 18
#define BMPHEIGHTOFFSET 22
#define PALMBITMAPLEN 16

#pragma package(smart_init)
//---------------------------------------------------------------------------
static void
putShort(unsigned char *dst,
         unsigned short v)
{
        *dst++=(v>>8)&0xff;
        *dst=v&0xff;
}

//---------------------------------------------------------------------------
static unsigned long
getLong(unsigned char *lwB)
{
        unsigned long l;

        l=(*(lwB+3)<<24);
        l|=(*(lwB+2)<<16);
        l|=(*(lwB+1)<<8);
        l|=*lwB;

        return l;
}

//---------------------------------------------------------------------------
unsigned char *
bmpCreate(char *fileName,
          unsigned int *bitmapLen)
{
        FILE *fh;
        long fileLen, loadedLen;
        unsigned char *dst;
        unsigned short bitmapWidth, bitmapHeight, sH;
        unsigned int bitmapBytes, len, h;
        unsigned char *bitmapBits, *bitmapDst, *tryl, *src;

        if ((fh=fopen(fileName, "rb"))==NULL)
                return NULL;

        if (fseek(fh, 0, SEEK_END)) {
                fclose(fh);
                return NULL;
        }

        fileLen=ftell(fh);
        rewind(fh);

        dst=new char[fileLen];
        loadedLen=fread(dst, 1, fileLen, fh);
        fclose(fh);
        if (loadedLen!=fileLen) {
                delete(dst);
                return NULL;
        }

        // parse out useful information
        if (*dst=='B' && *(dst+1)=='M') {
                bitmapWidth=(unsigned short)getLong(dst+BMPWIDTHOFFSET);
                bitmapHeight=(unsigned short)getLong(dst+BMPHEIGHTOFFSET);
                bitmapBytes=((bitmapWidth+31)/32)*4;
                bitmapBits=dst+getLong(dst+BMPBITSSTARTOFFSET);
        } else {
                delete(dst);
                return NULL;
        }

        // create stripped down PalmOS bitmap
        len=bitmapWidth*bitmapBytes;
        bitmapDst=new char[len+PALMBITMAPLEN];

        putShort(bitmapDst, bitmapWidth);
        putShort(bitmapDst+2, bitmapHeight);
        putShort(bitmapDst+4, bitmapBytes);
        *(bitmapDst+8)=1;               // pixelLine
        *(bitmapDst+9)=1;               // version

        tryl=bitmapDst+PALMBITMAPLEN;
        src=bitmapBits+(bitmapHeight*bitmapBytes)-bitmapBytes;
        for (sH=0; sH<bitmapHeight; sH++) {
                memmove(tryl, src, bitmapBytes);
                tryl+=bitmapBytes;
                src-=bitmapBytes;
        }

        // invert
        tryl=bitmapDst+PALMBITMAPLEN;
        for (h=0; h<len; h++) {
                *tryl=~*tryl;
                tryl++;
        }

        *bitmapLen=len+PALMBITMAPLEN;
        delete(dst);

        return bitmapDst;
}