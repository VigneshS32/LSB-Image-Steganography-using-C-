#ifndef DECODE_H
#define DECODE_H
#include "types.h"   //cotains user defined types

typedef struct _DecodeInfo
{
    /* decoded file Info  */
    char *decode_fname;
    FILE *fptr_decoded;
    int secret_file_extn_size;
    char *secret_file_extn;
    int secret_file_size;
    char *magic_data;

    /* stego image Info  */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} DecodeInfo;

/*  Decodeing function prototype  */

/* Get File pointers for i/p and o/p files */
Status decode_open_files(DecodeInfo *decInfo);

/* raed and validate decode args grom argv  */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode function, which does the real decoding */
Status decode_image_to_magic_data(int size, FILE *fptr_stego_image,DecodeInfo *decInfo);

/* Decode a byte into LSB of image data array */
Status decode_byte_to_lsb(char *data, char *image_buffer);

/*  Decode secret file extn size  */
Status decode_secret_file_extn_size(FILE *fptr_stego_image,DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(char *secret_file_extn, DecodeInfo *decInfo);

/* Decode function, which does the real decoding */
Status decode_image_to_extn_data(int size, FILE *fptr_stego_image,DecodeInfo *decInfo);

/* Decode secret file size*/
Status decode_secret_file_size(FILE *fptr_stego_image,DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif
