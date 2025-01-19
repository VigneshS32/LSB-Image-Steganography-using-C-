#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <string.h>
/* unction Definitions */
/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);
    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);
    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);
    // Return image capacity
    return width * height * 3;
}
/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }
    // No failure return e_success
    return e_success;
}

OperationType check_operation_type(char *argv[])   
{
    if(argv[1] != NULL)
    {
	if(strcmp(argv[1],"-e")==0)       //to check the second argument are encode or decode
	{ 
	    return e_encode;              //if it is encode return e_encode
	}
	else if(strcmp(argv[1],"-d")==0)  //to check the second argument are encode or decode
	{
	    return e_decode;             //if it is decode return e_decode
	}
	else
	{
	    return e_unsupported;       //if it is not decode and encode it will return e_unsupported
	}
    }
    else
    {
	printf("Error	");
	return e_unsupported;     //the second argument not precent it will give e_unsupported
    }
}

Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_failure)        //function call fro open file
    {
	printf("Encode Open file failure\n");
	return e_failure;
    }
    printf("Encode Open file Success\n");
    if(check_capacity(encInfo) == e_failure)       //function call for check capacity
    {
	printf("Check capacity failure\n");
	return e_failure;
    }
    printf("Check capacity success\n");
    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)      //function call for copy bmp header
    {
	printf("copy bmp header failure\n");
	return e_failure;
    }
    printf("copy bmp header success\n");
    if(encode_magic_string(MAGIC_STRING,encInfo) == e_failure)           //function call for magic string
    {
	printf("encode magic string failure\n");
	return e_failure;
    }
    printf("encode magic string sucess\n");
    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo->fptr_src_image,encInfo->fptr_stego_image))   //function call for secret file extn size
    {
	printf("encoding secret file extn size failure\n"); 
	return e_failure;
    }
    printf("encoding secret file extn size success\n");
    if( encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_failure )       //function call for secret file extn
    {
	printf("encode secret file extn failure\n");
	return e_failure;
    }
    printf("encode secret file extn success\n");
    if( encode_secret_file_size(encInfo->size_secret_file,encInfo) != e_success )           //function call for secret file size
    {
	printf("encode secret file size failure\n");
	return e_failure;
    }
    printf("encode secret file size success\n");
    if( encode_secret_file_data(encInfo) != e_success )                           //function call for secret file data
    {
	printf("encode secret file data failure\n");
	return e_failure;
    }
    printf("encode secret file data success\n");
    if( copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) != e_success )       //function call for copy remaing image data
    {
	printf("copy remaining image data failure\n");
	return e_failure;
    }
    printf("copy remaining image data success\n");
    return e_success;
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(strcmp(strstr(argv[2],".bmp"),".bmp")==0)
    {
      encInfo->src_image_fname = argv[2];     
    }
    else
    {
     // printf("please provide .bmp formate of image");
      return e_failure;
    }

    if(strcmp(strstr(argv[3],".txt"),".txt")==0)
    {
      encInfo->secret_fname = argv[3];
    }
    else
    {
     // printf("please provide .txt formate of secret data");
      return e_failure;
    }

     //since stegno.bmp is optional
    if(argv[4]!= NULL)
    {
    if(strcmp(strstr(argv[4],".bmp"),".bmp")==0)
    {
        encInfo->stego_image_fname = argv[4];
    }
    }
      else{
       // printf("please provide .bmp formate of stegno image");
         encInfo->stego_image_fname = "stego.bmp";
       }  

  return e_success;
}
uint get_file_size(FILE*fptr)           //the function are used to get the file size
{
    fseek(fptr,0,SEEK_END);  
    return ftell(fptr);              // the ftell to returns how many char are present

}
Status check_capacity(EncodeInfo *encInfo)
{
    int size= get_file_size(encInfo->fptr_secret);   
    encInfo->size_secret_file =size;
    if( get_image_size_for_bmp(encInfo->fptr_src_image) >= 54+( strlen (MAGIC_STRING) + 4 + strlen(encInfo->extn_secret_file) + 4 + size )*8 )  //to check image size are grater than equal or not
    {
	return e_success;  
    }
    else
    {
	return e_failure;
    }
}
Status copy_bmp_header(FILE*fptr_src_image,FILE*fptr_dest_image) //to copy the bmp header
{
    char buff[54];                                  //to create the 54 byte buffer
    rewind(fptr_src_image);                         //to set the curser at first
    fread(buff,1,54,fptr_src_image);                     //to read the first 54 byte hexa value from the input image
    int ret = fwrite(buff,1,54,fptr_dest_image);          //to write the 54 byte the output image 
    if( ret == 54)
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}
Status encode_magic_string(const char*magic_string,EncodeInfo *encInfo)        //teh magic string function
{
    if(encode_data_to_image(MAGIC_STRING,strlen(MAGIC_STRING),encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)  //teh function call for encode data to image
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)        //the encode data to image function 
{
    char buff[8];       //to create the 8 byte buffer
    int i;
    for(i=0;i<size;i++)
    {
	fread(buff,1,8,fptr_src_image);       //to read the 8 byte data from source image
	encode_byte_to_lsb(data[i],buff);    //function call for encode byte to lsb
	fwrite(buff,1,8,fptr_stego_image);    //to write the lsb value to destination image
    }
    return e_success;
}
Status encode_byte_to_lsb(char data, char *image_buffer)     //the encode byte to lsb  function
{
    int i;
    char get;
    for(i=0;i<8;i++)
    {
	image_buffer[i] = image_buffer[i] & 0xFE;            //to clear the lsb bit from source image
	get=(data & (1<<i)) >> i ;                           //to get the lsb of magic string
	image_buffer[i]=image_buffer[i]|get;                 //to store the lsb value to the destination image
    }
    return e_success;
}

Status encode_secret_file_extn_size(int size,FILE*fptr_src_image,FILE*fptr_stego_image)     //encode secret file extn size
{
    char buff[32];                                        //to create the 32 byte buffer 
    int i,get;
    fread(buff,1,32,fptr_src_image);                        //to read the 32 byte from the source image
    for(i=0;i<32;i++)              
    {
	buff[i]=buff[i]&0xFE;                          //to clrear the lsb bit from the sourec image
	get=((size&(1<<i))>>i);                         //to get the lsb of extn size 
	buff[i]=buff[i]|get;                          //to store the sb of the destination image
    }
    fwrite(buff,1,32,fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)            //the encode secret file extn function
{
    if( encode_data_to_image(file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image ) == e_success ) //function call for encode data to image
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)            //the encode secret file size
{
    if( encode_secret_file_extn_size( file_size,encInfo->fptr_src_image,encInfo->fptr_stego_image ) == e_success )     //function call for encode secret file extn size
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

Status encode_secret_file_data(EncodeInfo *encInfo)             //the encode secret file data function
{
    int ret;
    fseek(encInfo->fptr_secret,0l,SEEK_END);         //to set the curser from the first character teh secret file
    ret=ftell(encInfo->fptr_secret);                 //ftell returns the how many character precent the secret file
    rewind(encInfo->fptr_secret);                    //to set the curser to first
    char buff[ret];                                  //to creat the file size of buffer
    fread(buff,1,ret,encInfo->fptr_secret);          //to read the file size of data one by one
    encode_data_to_image(buff,ret,encInfo->fptr_src_image,encInfo->fptr_stego_image); //the function call for encode data to image 
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)     //the function for copy remaing data
{
    char var;
    while( fread(&var,1,1,fptr_src)>0 )               //to read the data from source image to one by one
    {
	fwrite(&var,1,1,fptr_dest);                       //to write the data to destination image to one by one
    }
    return e_success;
}
