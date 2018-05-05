#include "dataConverter.h"

void floatToString(float num, char *str, int mode)
{
    int tmp = num * 100;

    if (mode == VOLTAGE)
    {
        // format to xxxx
        sprintf(str, "%4d", tmp);
        // place '.' -> xx.xx
        str[5] = '\0';
        str[4] = str[3];
        str[3] = str[2];
        str[2] = '.';

        //fill
        if (str[1] == ' ')
            str[1] = '0';
    }
    else if (mode == CURRENT)
    {
        // format to xxxxxx
        sprintf(str, "%6d", tmp);
        // place '.' -> xxxx.xx
        str[7] = '\0';
        str[6] = str[5];
        str[5] = str[4];
        str[4] = '.';

        //fill
        if (str[3] == ' ')
            str[3] = '0';
        if (str[5] == ' ')
            str[5] = '0';
        if (str[6] == ' ')
            str[6] = '0';
    }
    else if(mode == POWER)
    {
        // format to xxxxxxxx
        sprintf(str, "%8d", tmp);
        // place '.' -> xxxxxx.xx
        str[9] = '\0';
        str[8] = str[7];
        str[7] = str[6];
        str[6] = '.';

        //fill
        if (str[5] == ' ')
            str[5] = '0';
        if (str[7] == ' ')
            str[7] = '0';
        if (str[8] == ' ')
            str[8] = '0';
    }
    else
    {
        str[0] = '\0';
    }
}


void intToString(int input,char* str)
{
    sprintf(str, "%8d", input);
}