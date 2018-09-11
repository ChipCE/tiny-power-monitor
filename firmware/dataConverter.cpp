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
        if (str[3] == ' ')
            str[3] = '0';
        
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
        sprintf(str, "%11d", tmp);
        // place '.' -> xxxxxx.xx
        str[12] = '\0';
        str[11] = str[10];
        str[10] = str[9];
        str[9] = '.';

        //fill
        if (str[11] == ' ')
            str[11] = '0';
        if (str[10] == ' ')
            str[10] = '0';
        if (str[8] == ' ')
            str[8] = '0';
    }
    else if(mode==RLOAD)
    {
        tmp = (int)num;
        sprintf(str, "%10d", tmp);
        str[10] = '\0';
        //fill zero with space
        for(int i=0;(str[i]=='0')&&i<9;i++)
            if(str[i]=='0')
                str[i]=' ';
    }
    else
    {
        str[0] = '\0';
    }
}

