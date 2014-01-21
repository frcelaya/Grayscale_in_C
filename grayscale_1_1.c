/*VERSION: Bonus points incluídos*/
/*Grayscale
 * This program takes a ppm file and outputs its equivalent in a black
 * and white gradient, with either of the average and luminance algorithms.
 * First it checks the command line arguments, then the magic value in
 * the input file, then dimensions, which are also saved as ints, and
 * finally the max color value. Then it converts all the image data and
 * writes it in the output file.
 * 
 * Fernando Ramón Celaya Loaiza k1156507*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Image data work is done here*/
int conversion(FILE *input_file, FILE *ouput_file, int total_bytes, char method);

/*To check magic number*/
int check_magic (FILE *input_file, FILE *output_file);

/*To check dimensions*/
int check_dimensions (int dimensions_array[], int size, FILE *input_file, FILE *output_file);

/*To check luminance*/
int check_luminance (FILE *input_file, FILE *ouput_file);

/*This is used to save our dimensions as ints*/
void trimmer (char *number, int dimensions_array[], int size, int position);													

/**********************************************************************/

int main(int argc, char *argv[]) 
{ 
	int i;
	int code = 0;
	char method;
	FILE *input_file = NULL;
	FILE *output_file = NULL;
	
	/*Check if no files or argument were given*/
	if (argc<=2 || argc>4)
	{
		fprintf(stderr, "Not enough arguments in command line. Error code 1.\n");									
		return 1;
	}
	
	/*If we only have 3 arguments, go to average algorithm*/
	if (argc==3)
	{			
		method='a';
		/*Opening input file and looking for errors*/
		input_file = fopen(argv[1], "r");									
		if (input_file == NULL)
		{
			fprintf(stderr, "Failed to open file '%s' for reading. Error code 2.\n", argv[1]);
			return 2;
		}
		/*Opening output file and looking for errors*/
		output_file = fopen(argv[2], "w");
		if (output_file == NULL)
		{
			fprintf(stderr, "Failed to open file '%s' for writing. Error code 3.\n", argv[2]);
			return 3;
		}
	}
	
	
	/*Determine algorithm if we have 4 arguments*/
	if (argc==4)								
	{		
		if (strcmp(argv[1], "-a") == 0 || strcmp(argv[1], "-l") == 0)
		{
			char *p;
			p = "-a";
			i = strcmp(argv[1], p);
			if (i==0)
				method='a';		
			p = "-l";
			i = strcmp(argv[1], p);
			if (i==0)
				method='l';
				
			/*Opening input file and looking for errors*/
			input_file = fopen(argv[2], "r");									
			if (input_file == NULL)
			{
				fprintf(stderr, "Failed to open file '%s' for reading. Error code 2.\n", argv[2]);
				return 2;
			}
			/*Opening output file and looking for errors*/
			output_file = fopen(argv[3], "w");
			if (output_file == NULL)
			{
				fprintf(stderr, "Failed to open file '%s' for writing. Error code 3.\n", argv[3]);
				return 3;
			}
		}
		else 
		
		if (strcmp(argv[2], "-a") == 0 || strcmp(argv[2], "-l") == 0)
		{		
			char *p;
			p = "-a";
			i = strcmp(argv[2], p);
			if (i==0)
				method='a';		
			p = "-l";
			i = strcmp(argv[2], p);
			if (i==0)
				method='l';
				
			/*Opening input file and looking for errors*/
			input_file = fopen(argv[1], "r");									
			if (input_file == NULL)
			{
				fprintf(stderr, "Failed to open file '%s' for reading. Error code 2.\n", argv[1]);
				return 2;
			}
			/*Opening output file and looking for errors*/
			output_file = fopen(argv[3], "w");
			if (output_file == NULL)
			{
				fprintf(stderr, "Failed to open file '%s' for writing. Error code 3.\n", argv[3]);
				return 3;
			}
		}
		
		if (strcmp(argv[3], "-a") == 0 || strcmp(argv[3], "-l") == 0)
		{		
			char *p;
			p = "-a";
			i = strcmp(argv[3], p);
			if (i==0)
				method='a';		
			p = "-l";
			i = strcmp(argv[3], p);
			if (i==0)
				method='l';
				
			/*Opening input file and looking for errors*/
			input_file = fopen(argv[1], "r");									
			if (input_file == NULL)
			{
				fprintf(stderr, "Failed to open file '%s' for reading. Error code 2.\n", argv[1]);
				return 2;
			}
			/*Opening output file and looking for errors*/
			output_file = fopen(argv[2], "w");
			if (output_file == NULL)
			{
				fprintf(stderr, "Failed to open file '%s' for writing. Error code 3.\n", argv[2]);
				return 3;
			}
		}	
	}
	    
    /*Time to check magic number. Also writes to output file*/
    code = check_magic(input_file, output_file);
    if (code!=0)
	{
		fcloseall();
		fprintf(stderr, "Header problem, please check magic number line in '%s'. Error code 4.\n", argv[1]);
		return code;
	}
		
	/*Time to check dimensions. Also writes to output file*/
	int dimensions_array[] = {0, 0};
	code = check_dimensions(dimensions_array, 2, input_file, output_file);
	if (code!=0)
	{
		fcloseall();
		fprintf(stderr, "Header problem, please check dimensions line in '%s'. Error code 4.\n", argv[1]);
		return code;
	}
		
	/*Time to check luminance. Also writes to output file*/
    code = check_luminance(input_file, output_file);
    if (code!=0)
	{
		fcloseall();
		fprintf(stderr, "Header problem, please check luminance line in '%s'. Error code 4.\n", argv[1]);
		return code;
	}
		
	/*Image data work is done here*/
	i = dimensions_array[0]*dimensions_array[1]*3;
	code = conversion(input_file, output_file, i, method);
	if (code!=0)
	{
		fcloseall();
		fprintf(stderr, "Error reading image data from '%s'. Error code 2.\n", argv[1]);
		return code;
	}
	
	fcloseall();
	return code;	
}

/**********************************************************************/

int conversion(FILE *input_file, FILE *output_file, int total_bytes, char method)
{
	int i, count, average;
	unsigned char *dynamic_array = NULL;
	unsigned char *dynamic_proof = NULL;
	unsigned char char_array[3];
	
	dynamic_array = (unsigned char *)malloc(total_bytes * sizeof(unsigned char));
	dynamic_proof = (unsigned char *)malloc(1 * sizeof(unsigned char));
	
	/*Reading all the bytes of image data at once. Possibly not the best
	 * implementation, in the future this would be the first thing I
	 * would rethink*/
	count = fread(dynamic_array, sizeof(char), total_bytes, input_file);
	if (count<=0)														
		return 2;
	
	/*This is to see if image data isn't shorter than what the dimensions specified*/
	if (count!=total_bytes)												
		return 2;
			
	count = fread(dynamic_proof, sizeof(char), 1, input_file);
	/*Checking if there is additional data*/
	if ((int)dynamic_proof[0]!=0)										
		return 2;
	
	/*Here we modify the values, depending on the indicated method*/
	for (i=0; i<total_bytes; i++)
	{
		if(method=='l')
			average = ((int)dynamic_array[i]*21+(int)dynamic_array[i+1]*71+(int)dynamic_array[i+2]*8)/100;
		else
			average = ((int)dynamic_array[i]+(int)dynamic_array[i+1]+(int)dynamic_array[i+2])/3;
		char_array[0]=(unsigned char)average;
		char_array[1]=(unsigned char)average;
		char_array[2]=(unsigned char)average;
		fwrite(char_array, sizeof(unsigned char), 3, output_file);
		i+=2;
	}
	
	free(dynamic_array);
	free(dynamic_proof);
	
	return 0;
}

/**********************************************************************/

int check_magic (FILE *input_file, FILE *output_file)
{
	int i, count;
	char *dynamic_array = NULL;
	
	dynamic_array = (char *)malloc(1 * sizeof(char));
	
	for (i = 0; i<=2; i++)
	{
		count = fread(dynamic_array, sizeof(char), 1, input_file);		/*Read from input_file*/
		if (count<=0)													/*Check if we read correctly or at least something*/
			return 4;
				
		if (i==0)														/*Check the P*/
		{
			if(dynamic_array[0] != 'P')
			{
				free(dynamic_array);
				return 4;
			}
			fwrite(dynamic_array, sizeof(char), 1, output_file);		/*Write to output_file*/
		}
		
		if (i==1)														/*Check the 6*/
		{
			if(dynamic_array[0] != '6')
			{
				free(dynamic_array);
				return 4;
			}
			fwrite(dynamic_array, sizeof(char), 1, output_file);		/*Write to output_file*/
		}
		
		if (i==2)														/*Check the new line*/
		{
			if((int)dynamic_array[0]!=10)
			{
				free(dynamic_array);
				return 4;
			}
			fwrite(dynamic_array, sizeof(char), 1, output_file);		/*Write to output_file*/
		}
	} 
			
	free(dynamic_array);
	return 0;
}

/**********************************************************************/

int check_dimensions (int dimensions_array[], int size, FILE *input_file, FILE *output_file)
{
	char *dynamic_array = NULL;
	char *width = NULL; 
	char *height = NULL;
	int count;
	
	dynamic_array = (char *)malloc(1 * sizeof(char));
	width = (char *)malloc(4 * sizeof(char));
	height = (char *)malloc(4 * sizeof(char));
	
	/*Construct our width string and write it to the ouput file*/
	do
	{
		count = fread(dynamic_array, sizeof(char), 1, input_file);
		if (count<=0)
			return 4;
		
		if ((dynamic_array[0] >= '0' && dynamic_array[0] <= '9') || dynamic_array[0] == ' ')
		{
			strcat(width, dynamic_array);
			fwrite(dynamic_array, sizeof(char), 1, output_file);
		}
		else
		{
			free(dynamic_array);
			free(width);
			free(height);
			return 4;
		}
		
	}
	while ((dynamic_array[0] >= '0' && dynamic_array[0] <= '9'));
	
	/*Construct our height string and write it to the ouput file*/
	do
	{
		count = fread(dynamic_array, sizeof(char), 1, input_file);
		if (count<=0)
			return 4;
		
		if ((dynamic_array[0] >= '0' && dynamic_array[0] <= '9') || (strcmp(dynamic_array, "\n") == 0))
		{
			strcat(height, dynamic_array);
			fwrite(dynamic_array, sizeof(char), 1, output_file);
		}
		else
		{
			free(dynamic_array);
			free(width);
			free(height);
			return 4;
		}
		
	}
	while ((dynamic_array[0] >= '0' && dynamic_array[0] <= '9'));
	
	/*Now send the strings to the trimmer function to obtain the value in ints*/
	trimmer(width, dimensions_array, size, 0);
	trimmer(height, dimensions_array, size, 1);
		
	free(dynamic_array);
	free(width);
	free(height);
	
	return 0;
}

void trimmer (char *number, int dimensions_array[], int size, int position)
{
	int i, count=0, mul=1;
	
	/*Get rid of the padding zeros by moving our pointer*/
	do
		if(number[0]=='0')
			number++;
	while(number[0]=='0');
	
	/*Determine magnitude of trimmed integer*/
	for (i=2; i<(int)strlen(number); i++)
			mul*=10;
	
	/*Cast, multiply, and accumulate the dimension value in the array*/
	for (i=0; i<(int)strlen(number); i++)
		if (number[i]>= '0' && number[i] <= '9')
		{
			count += (mul*(number[i]-48));
			mul /= 10;
		}
	/*Store the resulting value in the array*/
	dimensions_array[position]=count;
}

/**********************************************************************/

int check_luminance (FILE *input_file, FILE *output_file)
{
	int i, count;
	char *dynamic_array = NULL;
	
	dynamic_array = (char *)malloc(1 * sizeof(char));
	
	for (i = 0; i<=3; i++)
	{
		count = fread(dynamic_array, sizeof(char), 1, input_file);
		if (count<=0)
			return 4;
		
		/*Check the 2*/
		if (i==0)														
		{
			if(dynamic_array[0] != '2')
			{
				free(dynamic_array);
				return 4;
			}
			fwrite(dynamic_array, sizeof(char), 1, output_file);
		}
		
		/*Check the 5's*/
		if (i==1 || i==2)												
		{
			if(dynamic_array[0] != '5')
			{
				free(dynamic_array);
				return 4;
			}
			fwrite(dynamic_array, sizeof(char), 1, output_file);
		}
		
		/*Check the new line*/
		if (i==3)														
		{
			if((int)dynamic_array[0]!=10)
			{
				free(dynamic_array);
				return 4;
			}
			fwrite(dynamic_array, sizeof(char), 1, output_file);
		}
	} 
	
	free(dynamic_array);
	return 0;
}
