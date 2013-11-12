/* ***************************************************************** */


/*  5bit */

/*  author  John Cade Griffin */

/*
  5bit: take one file name as its input on the command line and produce on standard 
        output a translated file. Turns 8 bit sequences (bytes) into 5 bit sequences for encoding
        and reverses this for decoding

  modified to allow translation to hex, octal, base 4 or binary.

  Written around 2013.

*/

/* ***************************************************************** */


/* missing type from C language */
typedef short Boolean;
#define TRUE 1
#define FALSE 0




/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

#include <stdio.h>
#include <stdlib.h>

FILE * input;
Boolean DECODE = FALSE;
int newCharSize = 5; /* 5 for encoding, 8 for decoding */

void process(void);
void encodeFile(); /* called if DECODE = False, encodes the file into newCharSize sequences */
void printChar(int temp); /* called from encodeFile, prints a char representation of a shifted int */
void decodeFile(); /* called if DECODE = True, decodes the file into newCharSize sequences */
int decodeChar(int byte); /* called from decodeFile, returns an shifted int in the form previous of printChar */
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void scanargs(char * s)
{
    /* check each character of the option list for
       its meaning. */

    while (*++s != '\0')
        switch (*s)
            {

            case 'd': /* decode option */
                DECODE = TRUE;
                newCharSize = 8;
                break;

            default:
                fprintf (stderr,"PROGR: Bad option %c\n", *s);
                fprintf (stderr,"usage: PROGR [-D] file\n");
                exit(1);
            }
}

/* ***************************************************************** */

int main(int argc, char  **argv)
{
    Boolean namenotgiven = TRUE;

    /* main driver program.  Define the input file from either standard input or
       a name on the command line.  Process all arguments. */

    while (argc > 1)
        {
            argc--, argv++;
            if (**argv == '-')
                scanargs(*argv);
            else
                {
                    namenotgiven = FALSE;
                    input = fopen(*argv, "r");
                    if (input == NULL)
                        {
                            fprintf(stderr, "Can't open %s\n", *argv);
                            exit(1);
                        }
                    process();
                    fclose(input);
                }
        }

    /* input from stdin if not file named on the input line */
    if (namenotgiven)
        {
            input = stdin;
            process();
        }

    exit(0);
}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void process(void)
{
    if(!DECODE)
    {
        encodeFile();
    }
    else{
        decodeFile();
    }
  

}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
void encodeFile()
{
    int byte;
    int temp = 0;
    int charCount = 0; /*counts the characters before a new line */
    int bSize = 0; /*size of string of bits, 5 if encoding, 8 if decoding, initialize to 0 */
    while ((byte = getc(input)) != EOF) /* while there are still characters, grab the next */
    {   

            int bitCount = 8; /* re-initialize bitCount */
        
            while(bitCount > 0) /* while there are still bits left in this byte, grab the next */
            {   
                  /* shift byte to isolate desired bit, then reshift that bit to the position in the newChar which
                 is the equivalent of multiplying by powers of two, we want to total up temp */
              temp += (((byte >> (bitCount-1)) & 1) << (newCharSize - (bSize + 1)));

                 /*we are now in the next position of our new char */
              bSize++;
            
              /*we now need one less bit from byte */
              bitCount--;

              /*if encoding newCharSize = 5, else newCharSize = 8. if bSize = newCharSize temp is ready to become
              a newCharSize character. Print new character */
              if(bSize == newCharSize)
              {
                 if (charCount == 72)
                 {
                    printf("\n");
                    charCount = 0;
                 }
                    printChar(temp);
                    bSize=0;
                    temp=0;
                    charCount++;   
              }
            }
        }
         /* check for remaining bits */
            if (bSize > 0);
            {
                printChar(temp);
                printf("\n");
            }

}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
void decodeFile()
{


    int tempNum = 0;
    int byte;
    int bitCount = 0; /*counts the # of bits processed so far, stop ever 5  */
    while((byte = getc(input))!=EOF)
    {


        /*check for newlines */
        if(byte != '\n')
        {

            /* decode the character to its previous value prior to printChar */
            char decodedChar = decodeChar(byte);            
            /* count bits processed. Size will equal 5 */
            int bSize = 0;

            while(bSize < 5)
            {       
                    /*we have processed 1 bit */ 
                    bSize++;
                    /* remove 1 bit */
                    char bit = decodedChar >> (5 - bSize) & 1; 

                    bitCount++;
                    tempNum+= bit << (newCharSize - bitCount);

                    /*if we have  processed all bits */ 
                   if(bitCount == newCharSize)
                    {
                        printf("%c", tempNum);
                        tempNum = 0;
                        bitCount = 0;
                    }

            }
        }
    
    }

}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
void printChar(int temp) 
{
    

        if(temp < 26)
        {   
            temp += 65;
            printf("%c",temp );
        }
        else
        {
            temp += 22;
            printf("%c",temp );
        }
}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
int decodeChar(int temp)
{
    if(temp >= 65)
    {
        temp = temp - 65;
        return temp;
    }
    else
    {   
        temp = temp - 22;
    }
        return temp;
}
 