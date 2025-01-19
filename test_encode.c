#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>
/*
NAME : S VIGNESH
DATE : 02 OCT 2024
PROJECT NAME : STEGANOGRAPHY
PROJECT INPUT : MAGIC STRINGS , SECRET MESSAGE , .bmp IMAGE.
PROJECT OUTPUT : OUTPUT.bmp IMAGE ,OUTPUT.txt FILE.
*/
int main(int argc,char*argv[])
{
    EncodeInfo encInfo;          //Encode structure
    DecodeInfo decInfo;          //Decode structure


    int var1 = check_operation_type(argv);   //to check the operation are encode or not
    if(var1 == 0)
    {
	if(argc >= 4)                   
	{
	    read_and_validate_encode_args(argv,&encInfo);  //this function are used to the user given file are suitable file or not
	    int var2 = do_encoding(&encInfo);     //do encoding function calling 
	    if(var2 == e_success)                       //to check do_encoding function return e_success or not
	    {
		printf("Encoding success\n");          //the do encoding return e_success the encoding part perform good
	    }
	    if(var2 == e_failure)
	    {
		printf("Encoding failure\n");
		return 1;
	    }
	}
	else
	{
	    printf("Invalid number of arguments\n");          //less than four argument user will give  the error message will print
		return 1;
	}
    }
    else if(var1 == 1)                  
    {
	if(argc >= 3)
	{
	    read_and_validate_decode_args(argv,&decInfo);  //this function are used to the user given file are not suitable file or not
	    int var3 = do_decoding(&decInfo);
	    if(var3 == e_success)                     //the do_decoding funtion 
	    {
		printf("Decoding success\n");
	    }
	    if(var3 == e_failure)
	    {
		printf("Decoding failure\n");
		return 1;
	    }
	}

	else
	{
	    printf("Invalid number of arguments\n");     //less than three argument user will give the error message will print
		return 1;
	}
    }
    else
    {
	printf("Unsupported\n");                      //the operation are not decode and encode the error message will print
	printf("Usage: For Encoding: ./a.out -e source.bmp secret.txt [stego.bmp]\n");
    printf("       For Decoding: ./a.out -d stego.bmp [output.txt]\n");
            return 1;
    }

    return 0;
}
