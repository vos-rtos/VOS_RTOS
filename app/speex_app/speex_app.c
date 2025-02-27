#include "speex_stm32_config.h"


/* Modified from speexlib/testenc.c for Code Composer simulator */

#include <speex/speex.h>
#include <stdio.h>
#include <stdlib.h>
#include <speex/speex_callbacks.h>
#include <..\libspeex\arch.h>
#include "ff.h"
#include "vos.h"
#if 0
#undef DECODE_ONLY
#define CHECK_RESULT  /* Compares original file with encoded/decoder file */
#define TESTENC_BYTES_PER_FRAME 20  /* 8kbps */
#define TESTENC_QUALITY 4 /* 8kbps */
//#define TESTENC_BYTES_PER_FRAME 28  /* 11kbps */
//#define TESTENC_QUALITY 5	      /* 11 kbps */

/* For narrowband, QUALITY maps to these bit rates (see modes.c, manual.pdf)
 *   {1, 8, 2, 3, 3, 4, 4, 5, 5, 6, 7}
 * 0 -> 2150
 * 1 -> 3950
 * 2 -> 5950
 * 3 -> 8000
 * 4 -> 8000
 * 5 -> 11000
 * 6 -> 11000
 * 7 -> 15000
 * 8 -> 15000
 * 9 -> 18200
 *10 -> 26400  */

#ifdef FIXED_DEBUG
extern long long spx_mips;
#endif
#include <math.h>

#ifdef MANUAL_ALLOC
/* Take all Speex space from this private heap */
/* This is useful for multichannel applications */
#pragma DATA_SECTION(spxHeap, ".myheap");
static char spxHeap[SPEEX_PERSIST_STACK_SIZE];

#pragma DATA_SECTION(spxScratch, ".myheap");
static char spxScratch[SPEEX_SCRATCH_STACK_SIZE];

char *spxGlobalHeapPtr, *spxGlobalHeapEnd;
char *spxGlobalScratchPtr, *spxGlobalScratchEnd;
#endif

void byte2word(short* pInBuf, short* pOutBuf, int nWords)
{
   short *pIn, *pOut, sNext;
   int i;

   pIn = pInBuf;
   pOut = pOutBuf;
   for(i=0;i<nWords;i++)
   {
      sNext = *pIn++;			// Get low byte
      *pOut++ = (sNext & 0x00ff) | (*pIn++ << 8);	// insert high byte
   }
}

void word2byte(short* pInBuf, short* pOutBuf, int nWords)
{
   short *pIn, *pOut;
   int i;

   pIn = pInBuf;
   pOut = pOutBuf;
   for(i=0;i<nWords;i++)
   {
      *pOut++ = *pIn & 0x00ff;	// Get low byte
      *pOut++ = (short) ((unsigned short) *pIn++ >> 8);
   }
}

void speex_app_test()
{
   char *outFile, *bitsFile;
   //FILE *fout, *fbits=NULL;
   FIL fout, fbits;
   u32 num;
   FRESULT res;
#if !defined(DECODE_ONLY) || defined(CHECK_RESULT)
   char *inFile;
//   FILE *fin;
   FIL fin;
   short in_short[FRAME_SIZE];
#endif
   short out_short[FRAME_SIZE];
   short inout_byte[2*FRAME_SIZE];
#ifndef DECODE_ONLY
   int nbChars;
#endif
#ifdef CHECK_RESULT
   float sigpow,errpow,snr, seg_snr=0;
   int snr_frames = 0;
   int i;
#endif
   char cbits[TESTENC_BYTES_PER_FRAME/2 + 2];  /* temp store for encoded data */
   void *st;
   void *dec;
   SpeexBits bits;
   spx_int32_t tmp;
   unsigned long bitCount=0;
   spx_int32_t skip_group_delay;
   SpeexCallback callback;

   /* C54xx defaults to max wait states, even for parts like C5416 with
      larger internal memory.  Need to force the wait state register to zero */

#ifdef CONFIG_TI_C54X
   asm("	STM	#0,SWWSR");
#endif

#ifdef CHECK_RESULT
   sigpow = 0;
   errpow = 0;
#endif

#ifdef MANUAL_ALLOC
	spxGlobalHeapPtr = spxHeap;
	spxGlobalHeapEnd = spxHeap + sizeof(spxHeap);

	spxGlobalScratchPtr = spxScratch;
	spxGlobalScratchEnd = spxScratch + sizeof(spxScratch);
#endif
   st = speex_encoder_init(&speex_nb_mode);
#ifdef MANUAL_ALLOC
	spxGlobalScratchPtr = spxScratch;		/* Reuse scratch for decoder */
#endif
   dec = speex_decoder_init(&speex_nb_mode);

   callback.callback_id = SPEEX_INBAND_CHAR;
   callback.func = speex_std_char_handler;
   callback.data = stderr;
   speex_decoder_ctl(dec, SPEEX_SET_HANDLER, &callback);

   callback.callback_id = SPEEX_INBAND_MODE_REQUEST;
   callback.func = speex_std_mode_request_handler;
   callback.data = st;
   speex_decoder_ctl(dec, SPEEX_SET_HANDLER, &callback);

   tmp=0;
   speex_decoder_ctl(dec, SPEEX_SET_ENH, &tmp);
   tmp=0;
   speex_encoder_ctl(st, SPEEX_SET_VBR, &tmp);
   tmp=TESTENC_QUALITY;
   speex_encoder_ctl(st, SPEEX_SET_QUALITY, &tmp);
   tmp=1;  /* Lowest */
   speex_encoder_ctl(st, SPEEX_SET_COMPLEXITY, &tmp);

#ifdef DISABLE_HIGHPASS
   /* Turn this off if you want to measure SNR (on by default) */
   tmp=0;
   speex_encoder_ctl(st, SPEEX_SET_HIGHPASS, &tmp);
   speex_decoder_ctl(dec, SPEEX_SET_HIGHPASS, &tmp);
#endif

   speex_encoder_ctl(st, SPEEX_GET_LOOKAHEAD, &skip_group_delay);
   speex_decoder_ctl(dec, SPEEX_GET_LOOKAHEAD, &tmp);
   skip_group_delay += tmp;
   kprintf ("decoder lookahead = %ld\n", skip_group_delay);

#ifdef DECODE_ONLY
   bitsFile = "c:\\speextrunktest\\samples\\malebitsin.dat";
   //fbits = fopen(bitsFile, "rb");
   res = f_open(&fbits, bitsFile, FA_READ);
#else
   //bitsFile = "c:\\speextrunktest\\samples\\malebits5x.dat";
   bitsFile = "0:/female_speex_8.dat";
   //fbits = fopen(bitsFile, "wb");
   res = f_open(&fbits, bitsFile, FA_WRITE | FA_CREATE_ALWAYS);
#endif
#if !defined(DECODE_ONLY) || defined(CHECK_RESULT)
//   inFile = "c:\\speextrunktest\\samples\\male.snd";
   inFile = "0:/female_speex_8.snd";
   //fin = fopen(inFile, "rb");
   res = f_open(&fin, inFile, FA_READ);
#endif
//   outFile = "c:\\speextrunktest\\samples\\maleout5x.snd";
   outFile = "0:/female_speex_8_vos.snd";
   //fout = fopen(outFile, "wb+");
   res = f_open(&fout, outFile, FA_WRITE | FA_CREATE_ALWAYS);
   speex_bits_init(&bits);
#ifndef DECODE_ONLY
   //while (!feof(fin))
   while(!f_eof(&fin))
   {
      //fread(inout_byte, 2, FRAME_SIZE, fin);
	res = f_read (&fin, inout_byte, FRAME_SIZE*2, &num);
      byte2word(inout_byte, in_short, FRAME_SIZE);  /* C5x has 16-bit char */

      if (f_eof(&fin))
         break;
      speex_bits_reset(&bits);

      speex_encode_int(st, in_short, &bits);
      nbChars = speex_bits_write(&bits, cbits,
                         sizeof(cbits)*BYTES_PER_CHAR) /BYTES_PER_CHAR;
      bitCount+=bits.nbBits;

      word2byte((short *) cbits, inout_byte, nbChars);
      //fwrite(inout_byte, 2, nbChars, fbits);
	  res = f_write (&fbits, inout_byte, nbChars * 2, &num);
      speex_bits_rewind(&bits);

#else /* DECODE_ONLY */
   while (!f_eof(&fbits))
   {
      //fread(inout_byte, 1, TESTENC_BYTES_PER_FRAME, fbits);
	   res = f_read (&fbits, inout_byte, TESTENC_BYTES_PER_FRAME, &num);
      if (f_eof(&fbits))
         break;

      byte2word(inout_byte, (short *)cbits, TESTENC_BYTES_PER_FRAME/2);
      speex_bits_read_from(&bits, cbits, TESTENC_BYTES_PER_FRAME);
      bitCount+=160;
#endif
      speex_decode_int(dec, &bits, out_short);
      speex_bits_reset(&bits);

      word2byte(&out_short[skip_group_delay], inout_byte, FRAME_SIZE-skip_group_delay);
      //fwrite(inout_byte, 2, FRAME_SIZE-skip_group_delay, fout);
      res = f_write (&fout, inout_byte, (FRAME_SIZE-skip_group_delay) * 2, &num);
      skip_group_delay = 0;
#if 1
   kprintf ("Bits so far: %lu \r\n", bitCount);
#endif
   }
   kprintf ("Total encoded size: %lu bits\r\n", bitCount);
   speex_encoder_destroy(st);
   speex_decoder_destroy(dec);

#ifdef CHECK_RESULT
   f_rewind(&fin);
   f_rewind(&fout);

   while (1 /* FRAME_SIZE == fread(inout_byte, 2, FRAME_SIZE, fin)*/)
   {
	   res = f_read (&fin, inout_byte, 2 * FRAME_SIZE, &num);
	   if (res == 0 && num/2 != FRAME_SIZE) {
		   break;
	   }

	float s=0, e=0;

    byte2word(inout_byte, in_short, FRAME_SIZE);
    //fread(inout_byte, 2, FRAME_SIZE, fout);
    res = f_read (&fout, inout_byte, 2 * FRAME_SIZE, &num);
    byte2word(inout_byte, out_short, FRAME_SIZE);

        for (i=0;i<FRAME_SIZE;++i) {
            s += (float)in_short[i] * in_short[i];
            e += ((float)in_short[i]-out_short[i]) * ((float)in_short[i]-out_short[i]);
        }
	seg_snr += 10*log10((s+160)/(e+160));
	sigpow += s;
	errpow += e;
	snr_frames++;
   }

   snr = 10 * log10( sigpow / errpow );
   seg_snr /= snr_frames;
   kprintf("SNR = %f\r\nsegmental SNR = %f\r\n",snr, seg_snr);

#ifdef FIXED_DEBUG
   kprintf ("Total: %f MIPS\r\n", (float)(1e-6*50*spx_mips/snr_frames));
#endif
#endif /* CHECK_RESULT */
#if !defined(DECODE_ONLY) || defined(CHECK_RESULT)
   f_close(&fin);
#endif
   f_close(&fout);
   f_close(&fbits);
}
#endif
