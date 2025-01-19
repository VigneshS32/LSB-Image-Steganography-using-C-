#include <stdio.h> 
#include "decode.h"
#include "types.h"
#include "common.h"
#include <string.h>
#include <stdlib.h>

Status decode_open_files(DecodeInfo *decInfo)
{
    // stego image file
    decInfo->fptr_stego_image=fopen(decInfo->stego_image_fname, "r");
    // Do error handliing
    if(decInfo->fptr_stego_image==NULL)
    {
	perror("fopen");
	fprintf(stderr,"Error : unable toopen file %s\n",decInfo->stego_image_fname);
	return e_failure;
    }
    // output file
    decInfo->fptr_decoded=fopen(decInfo->decode_fname, "w");
    // Do error handling
    if(decInfo->fptr_decoded==NULL)
    {
	perror("fopen");
	fprintf(stderr,"Error Unable to open file %s\n",decInfo->decode_fname);
	return e_failure;
    }

    return e_success;
}

Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo)
{
    if( argv[2] != NULL && strstr(argv[2],".bmp") != NULL )                     //to check the file are .bmp or not
    {
	decInfo->stego_image_fname = argv[2];
    }
    else
    {
	return e_failure;
    }
    if( argv[3] != NULL && strstr(argv[3],".txt") != NULL )               //the 3rd argunment are not mandatory if you not give it store output.txt
    {
	decInfo->decode_fname = argv[3];
	return e_success;
    }
    else
    {
	decInfo->decode_fname = "output.txt";
	return e_success;
    }
}

Status do_decoding(DecodeInfo *decInfo)                 //the do decoding function
{
    if(decode_open_files(decInfo) == e_failure)           //function call for open files1
    {
	printf("Decode Open file failure\n");
	return e_failure;
    }
    printf("Decode Open file success\n");
    if(decode_magic_string(decInfo) == e_failure)                         //function call for magic string
    {
	printf("Decode magic string failure\n");
	return e_failure;
    }
    printf("Decode magic string success\n");
    if(decode_secret_file_extn_size(decInfo->fptr_stego_image,decInfo) == e_failure)        //function call for decode secret file extn size
    {
	printf("decode secret file extn size failure\n");
	return e_failure;
    }
    printf("decode secret file extn size success\n");
    if(decode_secret_file_extn(decInfo->secret_file_extn,decInfo) == e_failure)           //function call for decode secret file extn
    {
	printf("decode secret file extn failure\n");
	return e_failure;
    }
    printf("decode secret file extn success\n");
    if(decode_secret_file_size(decInfo->fptr_stego_image,decInfo) == e_failure)           //function call fro secret file size
    {
	printf("decode secret file size failure\n");
	return e_failure;
    }
    printf("decode secret file size success\n");
    if(decode_secret_file_data(decInfo) == e_failure)                      //function call for secret file data
    {
	printf("Decode secret file data failure\n");
	return e_failure;
    }
    printf("Decode secret file data success\n");
    return e_success;
}

Status decode_image_to_magic_data(int size, FILE *fptr_stego_image,DecodeInfo *decInfo )        //the decode image to data function
{
    char buff[8];                                //to creat the 8 byte buffer
    int i;
    for(i=0;i<size;i++)
    {
	fread(buff,8,1,fptr_stego_image);                             //to read the 8 byte from the stego image 
	decode_byte_to_lsb(&decInfo->magic_data[i],buff);             //function call for decode byte to lsb
    }
    return e_success;
}

Status decode_byte_to_lsb(char *data, char *image_buffer)                  //the function for decode byte to lsb
{
    int i;
    char get,ch=0;
    for(i=7;i>=0;i--)
    {
	get = image_buffer[i] & 0x01;               //to get the lsb bit from the buffer
	ch = (get << i) | ch ;                     
    }
    *data=ch;                          //to get the magic string 
    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)              //the function for magic string
{
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);            //to set the curscer to 54th byte
    int var = (strlen(MAGIC_STRING));                        //find the magic string length
    decInfo->magic_data = malloc(var+1);                      //to craet the memory dynamically
    decode_image_to_magic_data(var,decInfo->fptr_stego_image,decInfo);      //function call for decode image to data
    decInfo->magic_data[var] = '\0';
    if( (strcmp(decInfo->magic_data,MAGIC_STRING)) == 0 )
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

Status decode_secret_file_extn_size(FILE *fptr_stego_image,DecodeInfo *decInfo)   //the decode secret file extn size
{
    int i,size=0;
    char get;
    char buff[32];                               //to creat the 32 byte buffer
    fread(buff,32,1,fptr_stego_image);              //to read the 32 byte from the stego image
    for(i=31;i>=0;i--)
    {
	get = buff[i] & 0x01;                      //get the lsb from the buffer
	size = (get << i) | size;
    }
    decInfo->secret_file_extn_size = size;                //the size value will store the secret file extn size
    return e_success;
}

Status decode_secret_file_extn(char *secret_file_extn, DecodeInfo *decInfo)     //the decode file exnt function
{
    int var = decInfo->secret_file_extn_size;                       
    decInfo->secret_file_extn = malloc(var+1);                        //to craet the memory dynamically
    decode_image_to_extn_data(var,decInfo->fptr_stego_image,decInfo);   //the function call for decode image to data1
    decInfo->secret_file_extn[var] = '\0';
    return e_success;
}
Status decode_image_to_extn_data(int size, FILE *fptr_stego_image,DecodeInfo *decInfo )         //the decode image to data1 function
{
    char buff[8];               //to creat the 8 byte buffer
    int i;
    for(i=0;i<size;i++)
    {
	fread(buff,8,1,fptr_stego_image);                                    //to raed the 8 byte data from the stego image
	decode_byte_to_lsb(&decInfo->secret_file_extn[i],buff);            //the function call for decode byte to lsb
    }
    return e_success;
}
Status decode_secret_file_size(FILE *fptr_stego_image,DecodeInfo *decInfo)               //the decode secret file size
{
    char buff[32];                            //to creat the 32 byte buffer
    int i,size=0;
    char get;
    fread(buff,32,1,fptr_stego_image);           //to read the 32 byte from the stego image
    for(i=31;i>=0;i--)
    {
	get = buff[i] & 0x01;                    //to get the lsb from the buffer
	size = (get << i) | size;
    }
    decInfo->secret_file_size = size;                 //the size value will store the secret file size
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)           //the decode secret file data function
{
    char var;                                    //to creat the character variable
    int i;
    for(i=0;i<decInfo->secret_file_size;i++)              //the loop will run the secret file size times
    {
	fread(decInfo->stego_image_fname,8,1,decInfo->fptr_stego_image);   //to raed the 8 byte data from the stego image
	decode_byte_to_lsb(&var,decInfo->stego_image_fname);   //the function call for decode data to lsb
	fputc(var,decInfo->fptr_decoded);                    //to write one by one character to the decoded file
    }
}
