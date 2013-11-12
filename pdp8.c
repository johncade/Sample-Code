
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


FILE *input;

typedef short Boolean;
#define TRUE 1
#define FALSE 0

Boolean verbose = FALSE;

typedef char *STRING;


/* Global Variables */
short PC; /* Program Counter */
short AC;
short MBR;
short MAR;
short PAGESIZE = 128;
short LINKBIT= 0;
short bitsToRotate = 0;
long long int TIME;

short memory[4096]; 

void processFile(FILE* input);
void initMemory();

int getVal(int byte);
void addToMemory(int address, int instruction);
Boolean peak(void);

void processOperations(int value);

/* Check D/I and Z/C Bits */
Boolean checkDI(int val);
Boolean checkZC(int val);

/* Stops the Program */
Boolean HALT = FALSE;

/* Memory Reference Instructions */
char* TAD(int address);
char* ISZ(int address);
char* AND(int address);
char* DCA(int address);
char* JMS(int address);
char* JMP(int address);
char* IOT(int address);
char* operate(int address);
/* Operate Group One */
char* opGroupOne(int codeVal);
void CLA();
void CLL();
void CMA();
void CML();
void RAR();
void RAL(int codeVal);
void RTL(int codeVal);
void IAC();
int numBitsToRotate (int value);
/* Operate Group Two */
char* opGroupTwo(int codeVal);
void SMA(int value);
void SZA(int value);
void  SNL(int value);
Boolean RSS(int val);
void OSR();
void HLT();


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
void initMemory()
{
 int i = 0;
 for (i = 0; i < 4096; i++)
    {
    memory[i] = 0;
    }
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void scanargs(STRING s)
{
    /* check each character of the option list for
       its meaning. */

    while (*++s != '\0')
        switch (*s)
            {

            case 'v': /* verbose option */
                verbose = TRUE;
                break;

            default:
                fprintf (stderr,"PROGR: Bad option %c\n", *s);
                fprintf (stderr,"usage: PROGR [-D] file\n");
                exit(1);
            }
}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
void processOperations(int value)
{
char* OpString;
//OpString[0]= '\0';
int prevPC;
int mask = 0x007;
int opCode;
while (!(HALT))
{
    prevPC = PC;
    opCode = (value >> 9) & mask;
    //printf("PC = %d\n",PC);
    //printf("opCode = %d\n",opCode);
    //printf("VALUE = %d\n",value );
switch(opCode)
{
    case 0:
    /* AND */
        //char* operation = "AND";
        OpString = AND(value);
        break;
    case 1:
    /* TAD */
       // printf("TAD\n");
        OpString = TAD(value);
        break;
    case 2:
    /* ISZ */
       // printf("ISZ\n");    
        OpString = ISZ(value);
        break;
    case 3:
    /* DCA */
       // printf("DCA\n");
        OpString = DCA(value);
        break;
    case 4:
    /* JMS */
       // printf("JMS\n");
        OpString = JMS(value);
        break;
    case 5: 
    /* JMP */
       // printf("JMP\n");
        OpString = JMP(value);
        break;
    case 6:
    /* I/O */
        OpString = IOT(value);
        break;
    case 7:
    /* Op */ 
        OpString = operate(value);
        TIME++;
        //PC++;
        //printf("OP");
        break;
    default:
        fprintf(stderr, "Bad opCode %d found, exiting program\n", opCode);
        exit(1);
    }   
    if(verbose)
    fprintf(stderr,"Time %lld: PC=0x%03X instruction = 0x%03X (%s), rA = 0x%03X, rL = %d\n",TIME, prevPC, value, OpString, AC, LINKBIT);
    //PC++;
    value = memory[PC];
    }
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
int getPageAddress(int instruction)
{   
Boolean currentPage = instruction >> 7 & 1;
char initAddress = instruction & 0x7f;
short effectiveAddress = initAddress;
if(currentPage)
{
    short page = PC/PAGESIZE;
    effectiveAddress = initAddress + page*PAGESIZE;
    return effectiveAddress;
}
else
{
    return effectiveAddress;
}
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
char* AND(int address)
{
    short value =0;
    char* name;
    if(checkDI(address))
    {   //go to address, AND Value with AC
        address = getPageAddress(address);
        value = memory[address];
        name = "AND";
    }
    else
    {   //go to address, follow that address to the next, AND value in second addresss with AC
        address = getPageAddress(address);
        address = memory[address];
        value = memory[address];
        //int indirectVal = memory[indirectAddress];
        TIME += 1;
        name = "AND I";
    }
        AC = (AC & value);
        TIME +=  2;
        PC = (PC + 1) % 4096;

       
    return name;
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
char* TAD(int address)
{
    char * name;
    int value;
    if(checkDI(address))
      {   
        address = getPageAddress(address);
        value = memory[address];
        name = "TAD";
    }
    else
    {   //go to address, follow that address to the next, ADD value in second addresss with AC
        address = getPageAddress(address);
        address = memory[address];
       // printf("TAD INDIRECT mem Address = %d\n",memory[address]);
        //int indirectAddress = memory[address];
        value = memory[address];

        TIME += 1;
        name = "TAD I";  
    }
        AC = (AC + value);
        if(AC > 4095)
        {
            LINKBIT =  !LINKBIT;
            AC = AC & 0xFFF; //check for overflow
        }
        TIME +=  2;
        PC = ( PC + 1) % 4096;
        
        return name;
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
char* ISZ(int address)
{
    short value;
    char * name;
    if(checkDI(address))
    {   //increment and skip if zero
        address = getPageAddress(address);
        value = memory[address];
        name = "ISZ"; 
    }
    else
    {   //go to address, follow that address to the next, incriment and skip if zero
        address = getPageAddress(address);
        address = memory[address];
        value = memory[address];
        TIME++;
        name = "ISZ I"; 
    }
        value = (value + 1) & 0xFFF;
        memory[address] = value;
        if(value == 0)
        {
            PC = (PC + 2) % 4096;
        }
        else
        {
            PC = (PC + 1) % 4096;
        }
        TIME += 2;    
        return name; 
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
char* DCA(int address)
{
    char * name;
    if(checkDI(address))
    {   
        address = getPageAddress(address);
        name = "DCA"; 

    }
    else
    {   //go to address, follow that address to the next
        address = getPageAddress(address);
        address = memory[address];
        TIME += 1;
        name = "DCA I"; 
    }   
        memory[address] = AC;
        AC = 0;
        PC = (PC + 1) % 4096;
        TIME += 2;    
        return name;
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
char* JMS(int address)
{
    char * name = "JMS";
    if(checkDI(address))
    {   
        name = "JMS";
        address = getPageAddress(address);
        memory[address] = (PC + 1) % 4096;
        //PC = (address +1) % 4096;
        
    }
    else
    {   //go to address, follow that address to the next, 
        address = getPageAddress(address);
        address = memory[address];
        //memory[indirectAddress] = (PC + 1) % 4096;
        //PC = (indirectAddress + 1) % 4096;
        TIME += 1;
        name = "JMS I";
    }
        memory[address] = (PC + 1) % 4096;
        PC = (address + 1) % 4096;
        TIME += 2; 
        //char * name = "JMS";    
        return name;
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
char* JMP(int address)
{
    char * name; 
    if(checkDI(address))
    {   
        address = getPageAddress(address);
        PC = address;
        name = "JMP"; 
    }
    else
    {   //go to address, follow that address to the next, 
        address = getPageAddress(address);
        int indirectAddress = memory[address];
        PC = (indirectAddress) % 4096;
        TIME += 1;
        name = "JMP I"; 
    }   
        //PC++;
        TIME ++; 
           
        return name;
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
char* IOT(int address)
{   int device = ((address >> 3) & 0x03F);
    int byte;
    char * name; 
    if(device == 3)
    {
        byte = getchar();
        AC = byte & 0xFFF;
        name = "IOT 3"; 
    }
    else if(device == 4)
    {
        byte = AC & 0x0FF;
        putchar(byte);
        name = "IOT 4"; 
    }
    else
    {
        HALT = TRUE;
        name = "HLT";
    }
        PC = (PC + 1)%4096;
        TIME ++; 
            
        return name;
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
char * operate(int address)
{   
    int test = ((address >> 8) & 1);
    char* instructions; 
    if(test == 0) /* Group 1 */
    {
        instructions = opGroupOne(address);
    }
    else
    {
        instructions = opGroupTwo(address);
    }
    PC = (PC + 1) % 4096;
        return instructions;
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
char* opGroupOne(int value)
{
    static char instructions[50];
    instructions[0] = '\0';

    if(((value >> 7) & 1) == 1) /* CLA */
    {
        CLA();
        strcat(instructions, "CLA ");
    }
    if(((value >> 6) & 1) == 1) /* CLL */
    {
        CLL();
        strcat(instructions, "CLL ");
    }
    if(((value >> 5) & 1) == 1) /* CMA */
    {
        CMA();
        strcat(instructions, "CMA ");
    }
    if(((value >> 4) & 1) == 1) /* CML */
    {
        CML();
        strcat(instructions, "CML ");
    }
    if((value & 1) == 1) /* IAC */
    {
        IAC();
        strcat(instructions, "IAC ");
    }
    if(((value >> 3) & 1) == 1) /* RAR */
    {

        if(numBitsToRotate(value) == 1)
        {
            strcat(instructions, "RAR ");
        }
        else
        {
            strcat(instructions, "RTR ");
        }
        RAR(value);
    }
    if(((value >> 2) & 1) == 1) /* RAL */
    {   
        if(numBitsToRotate(value)==1)
        {
                    
        strcat(instructions, "RAL "); 
        }
        else
        {
        strcat(instructions, "RTL ");            
        }
        RAL(value);
    }
    int length = strlen(instructions) - 1;
    instructions[length] = '\0';
    return instructions;                                     
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
char* opGroupTwo(int value)
{   
    Boolean skipFlag = FALSE;
    Boolean checkRSS = RSS(value);
    //printf("RSS: %d\n",checkRSS );
    static char instructions[50];
    instructions[0] = '\0';

    if(((value >> 6) & 1) == 1) /* SMA */
    {
    
        Boolean negativeAC = AC>>11 & 1;
        if(((!negativeAC) && (checkRSS)) || (!checkRSS && negativeAC))
        {
            skipFlag = TRUE;
        }
        strcat(instructions, "SMA ");
    }
    if(((value >> 5) & 1) == 1) /* SZA */
    {
        if((checkRSS && AC!=0) || (!checkRSS && AC==0))
        {
        skipFlag=TRUE;
        }        
        strcat(instructions, "SZA ");
    }
    if(((value >> 4) & 1) == 1) /* SNL */
    {
        if((checkRSS && LINKBIT==0) || (!checkRSS && LINKBIT==1))
        {
        skipFlag=TRUE;
        }
        strcat(instructions, "SNL ");
    }
    if(((value >> 3) & 1) == 1) /* RSS */
    {
        //RSS(value);
        strcat(instructions, "RSS ");
    }
    if(((value >> 7) & 1) == 1) /* CLA */
    {
        CLA();
        strcat(instructions, "CLA ");
    }
    if(((value >> 2) & 1) == 1) /* OSR */
    {
        /* Do Nothing NOP */ 
        strcat(instructions, "");
    }
    if(((value >> 1) & 1) == 1) /* HLT */
    {
        HLT();
        strcat(instructions, "HLT ");
    }
    if(skipFlag)
    {
        PC = (PC + 1)%4096;
        //printf("double cointing skip ");
    }
    int length = strlen(instructions) - 1;
    instructions[length] = '\0';
    return instructions;                                     
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
void CLA()
{
    AC = 0;
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
void CLL()
{
    LINKBIT = 0;

}
/* **************)*************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
void CMA()
{   


    AC = (~AC) & (0xFFF);

}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
void CML()
{
    LINKBIT = !LINKBIT;

}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
void RAR(int codeVal)
{   
    int bitsToRotate = (numBitsToRotate(codeVal));
    int i;
    int temp;
    for(i = 0; i <bitsToRotate; i++)
    {   
        temp = LINKBIT;
        LINKBIT = AC & 1;
        AC = (AC >> 1 | temp << 11) & 0xFFF;
    }   
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
void RAL(int codeVal)
{
    int bitsToRotate = (numBitsToRotate(codeVal));
       //printf("in RAL bitstoRotate == %d\n",bitsToRotate );
       int i;
        int temp;
        for(i = 0; i < bitsToRotate; i++)
        {   
            //printf("AC = %d\n",AC );
            temp = LINKBIT;

            //printf("temp = %d\n",temp );
            LINKBIT = (AC >> 11)& 1;
            AC = ((AC << 1) | temp) & 0xFFF;
            //printf("AC = %d\n",AC );
        }
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
int numBitsToRotate (int value)
{   int rotateBit = ((value >> 1) & 1);
    if(rotateBit == 1)
        return 2; /* rotate two bits */
    else
        return 1; /* rotate one bit */
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
void IAC()
{
    AC += 1;

        if(((AC >> 12) & 0x001) == 1)
        LINKBIT = 1;

    AC = AC & 0xFFF;

}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
Boolean RSS(int val)
{
 int temp = (val >> 3) & 1;
 if(temp == 1)
    return TRUE;
else
    return FALSE;
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
void OSR()
{
 /* Treat as NOP, Do nothing */
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
void HLT()
{
 HALT = TRUE;
}
/* ***************************************************************** */
/*                                                                   */
/*                      Returns TRUE if DIRECT                       */
/* ***************************************************************** */
Boolean checkDI(int val)
{
    int result = ((val >> 8) & 1);

    return (result == 0);
}
/* ***************************************************************** */
/*                                                                   */
/*                    Returns TRUE if Zero Page                      */
/* ***************************************************************** */
Boolean checkZC(int val)
{
    int result = ((val >> 7) & 1);

    return (result == 0);
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

int main(int argc, STRING *argv)
{
    Boolean filenamenotgiven = TRUE;

    /* main driver program.  Define the input file
       from either standard input or a name on the
       command line.  Process all arguments. */
    initMemory();
    while (argc > 1)
        {
            argc--, argv++;
            if (**argv == '-')
                scanargs(*argv);
            else
                {
                    filenamenotgiven = FALSE;
                    input = fopen(*argv,"rb");
                    if (input == NULL)
                        {
                            fprintf (stderr, "Can't open %s\n",*argv);
                            exit(1);                        
                        }
                    else
                        {
                            processFile(input);
                            processOperations(memory[PC]);
                            fclose(input);
                        }
                }
        }

    if (filenamenotgiven)
        {
            fprintf (stderr, "File name not given");
            exit(0);
        }
    exit(0);
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
Boolean peak(void)
{
    Boolean result = FALSE;
    int testForP = getc(input);
    if (testForP == 'P')
    {
        result = TRUE;
    }
    else
    {
        result = FALSE;
    }
    ungetc(testForP,input);
    return result;
}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
int getVal(int byte)
{   
    int result = 0;
    int power;
   // printf("-------------------------------- Entering getVal Byte = %c ------------------------------\n", byte);
    for(power = 2; power >= 0; power--)
    {
           // printf("-------------------------------- Top of For loop getVal Byte = %c ------------------------------\n", byte);
        if ((byte > 64) && (byte < 91)) /*byte is a capital letter */
            {   
                //printf("byte before sub = %d\n",byte );
                byte -= 55; /* subtracting 65 since A = 65 in ASCII, adding 10 since A = 10 in Hex */  
                //printf("byte after sub = %d\n",byte );
               // hexFactor = pow(16,power);/*figure out the hex representation for current character including factor of 16 */
                //printf("hexFactor = %d\n",hexFactor);
                //byte = byte * hexFactor;  
                //printf("byte = %d\n", byte); 
                //result += byte;  /* add this character to temp */
                //printf("result = %d\n",result );
            }
        else if ((byte > 47) && (byte < 58))/* byte is a number */
            {
                byte -= 48;
                //hexFactor = pow(16,power);/*figure out the hex representation for current character including factor of 16 */
                //printf("hexFactor = %d\n",hexFactor);
               // byte = byte * hexFactor;  
                //printf("byte = %d\n", byte); 
               // result += byte;  /* add this character to temp */
                //printf("result = %d\n",result );
            }
        else /*byte is an invalid character */
            {
                fprintf(stderr, "%d is an invalid character\n", byte); 
                exit(1);
            }
         result += (byte << 4*(power));  
        byte = getc(input);
    }
    return result;

}
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
void processFile(FILE* input)
{   

    int byte = getc(input);
    int address = 0;
    int  instruction = 0;   
    int i;
    TIME = 0;
    while ((byte != EOF))
    {
        //printf("Byte in Process File = %d\n",byte);
            if(byte == 'E')
            {
                {
                int testForP = getc(input);
                if (testForP == 'P')
                {
                    ungetc(testForP,input);
                    for(i = 0; i < 4; i++)
                    {
                        byte = getc(input);
                        if (byte == EOF)
                        {
                            fprintf(stderr, "Incorrect Format: EOF before finished parsing\n");
                            exit(1);
                        }
                    }
                    address = getVal(byte);
                    PC = address;
                    //printf("Assigning PC = %d\n",address);

                        byte = getc(input);
                        if (byte == EOF)
                        {       
                         break;
                        }
                }
                else
                {
                ungetc(testForP,input);
                }

    
                }

            }
            
            //printf("printing Address\n");
            address = getVal(byte);
            for(i = 0; i < 2; i++)
            {
                byte = getc(input);
                if (byte == EOF)
                {
                    fprintf(stderr, "Incorrect Format: EOF before finished parsing\n");
                    exit(1);
                }
            }
            //printf("printing Instruction\n");
            instruction = getVal(byte);
            addToMemory(address,instruction);

                byte = getc(input);
                if (byte == EOF)
                {
                    break;
                }
    }

}

/* ***************************************************************** */
/*                addToMemory                                        */
/* ToDo:                                                             */
/* ***************************************************************** */
void addToMemory(int address, int instruction)
{
//fprintf(stderr, "Address : 0x%03x Instruction : 0x%03x\n",address,instruction);
    memory[address] = instruction;
}