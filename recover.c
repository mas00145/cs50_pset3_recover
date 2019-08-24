#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// number of block to read at a time
#define BLOCK_SIZE 512

bool isJPG(uint8_t buffer[]);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s image\n", argv[0]);
        return 1;
    }

    // remember the input file
    char *infile = argv[1];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    uint8_t buffer[512]; // create a buffer of 512 bytes to read in sectors
    int fileNumber = 0;
    char fileName[8]; // array to hold chars for the file name XXX.jpg
    FILE *outptr = NULL; // create out file pointer and set to NULL
    int ret = 0; // to hold fread() return value
    while ((fread(&buffer, BLOCK_SIZE, 1, inptr)) == 1)
    {
        if (isJPG(buffer))
        {
            sprintf(fileName, "%03i.jpg", fileNumber); // create the file name to write to

            outptr = fopen(fileName, "w"); // open pointer to output file

            // if unable to open print error and exit program
            if (outptr == NULL)
            {
                fprintf(stderr, "Could not open %s for writing.\n", fileName);
                return 2;
            }

            do // need to do this at least one time
            {
                fwrite(&buffer, BLOCK_SIZE, 1, outptr); // write the buffer to the out file
                ret = fread(&buffer, BLOCK_SIZE, 1, inptr); // read the next block of 512 bytes
            }
            while (!isJPG(buffer) && ret != 0); // while we're not at the next jpg file

            fseek(inptr, -(BLOCK_SIZE), SEEK_CUR);  // go back one sector
            fileNumber++; // update file number
            fclose(outptr); // close the output file
        }
    }

    fclose(inptr); // at this point done with input file, so close it
}

// check if we are at the start of a jpg file or not
bool isJPG(uint8_t buffer[])
{
    if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

