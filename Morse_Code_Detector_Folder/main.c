#include "project.h"
#include <stdio.h>
#define patternSize                       (2)  //this will be the size of desired
#define percentageTolerance      (0.6) // tolerance
#define normalizationValue          (10000) // normalize to 10000
#define debounceValue                (50) // 20th of a second  (bigger the debounce, faster the     knocks
#define thresholdValue              (600) // mV


//void gap(int x[])
//{
//    for(int i = 0; i < patternSize; i++)
//    {
//        x[i] = x[i+1] - x[i];
//    }
//}

int summation(int x[])
{
    int sum = 0;
    for (int i=0; i < patternSize; i++)
    {
        sum=sum+x[i];
    }    
    return sum;
}

void Normalizediff(int x[], int sum) // normalization function
{
    for (int i=0; i < patternSize; i++) 
    {
        x[i] = normalizationValue * x[i] / sum; // 10000 normalizes all arrays
      
    }
}

_Bool Check_Norm_Arrays(int x[],int y[]) // check normalized array values with one anothe
{
    for(int i = 0; i < patternSize; i++)
    {
            if( !((x[i] - x[i]* percentageTolerance <= y[i])  &&  (x[i] + x[i]* percentageTolerance >= y[i]))  )  
            {   
                return 0;
            }
    }  
    return 1; // the arrays match if it never enters the statement
}

// loop to detect if arrays match or if input is a scalar multiple of the desired array, itll enter the if statement and cause it to unlock

int biggestValue(int arr1[])
{
    int size = sizeof(arr1);
    int Largest = arr1[0];
    for(int i = 1; i < size; i++)
    {
        if(arr1[i] > Largest)
        {
            Largest = arr1[i]; //assuming the first element in array is greatest until proven wrong
        }

        }
    return Largest;
}

int main(void)
{
    ADC_Start();
    ADC_StartConvert();
    Timer_Start();
    CyGlobalIntEnable; /* Enable global interrupts. */
    int desired[] = {2000, 2000};
    //int timeLimit = 10*biggestValue(desired); //maybe this could be a macro
    int input[patternSize]; //the input gets an element of time diff                      
    //whenever it detects a knock
    for(;;)
    {
        uint32_t timerMax = 0xFFFFFFFF;
     //   uint32_t timerPrevious;
        uint32_t timeDifference;
        int j = 0;  // j is the counter that will be moving around the position in array "input"
        int i = 0;
        
//        for(int i = 0; i < timeLimit; i++)
//        {
            Out_Pin_Write(0); //assume locked until correct sequence is given
            while (j <= patternSize)
            {
                ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
                if(ADC_GetResult16() >= thresholdValue) // range of mV to detect
                {
                    uint32 time =  Timer_ReadCounter(); //the timers actual clock which is counting down
                    timeDifference = timerMax - time; //real time where it occurred
                   
                    timerMax = time; // This will now holdthe value of when the prev knock was detected
                    if(j != 0)
                    {
                        input[i] = timeDifference; //giving time where it detects knock to 
                        i++;
                        ////moving to the next element in "input"
                    }
                    j++; //moving to the next element in "input"
                    
                    CyDelay(debounceValue); //to get only one value of when it passes Vt
                }
        
            }
        //}
    // Function called
    int sum1 = summation(desired);
    int sum2 = summation(input);
    
    // gap(input);
    Normalizediff(desired, sum1);
    Normalizediff(input, sum2);
    
    _Bool Checker = Check_Norm_Arrays(desired, input); // checking arrays 
     //after normal
    if(Checker ==0)
    {
        Out_Pin_Write(0); //it just stays locked (this line isnt even 
        //needed, its overkill)
    }
    if(Checker == 1)
    {
        Out_Pin_Write(1);  //the thing unlocks
        CyDelay(500);
    }           
          
    }
}
 
//#include "project.h"
//#include <stdio.h>
//int main(void)
//{
//    ADC_Start();
//    ADC_StartConvert();
//    CyGlobalIntEnable; /* Enable global interrupts. */
//    
//    for(;;) {
//        Out_Pin_Write(0); //nothing is changing ; always remain locks 
//      //  CyDelay(500);
//      //  Out_Pin_Write(0);
//        ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
//        if(ADC_GetResult16() >=1000 )
//        {
//           CyDelay(200);
//            Out_Pin_Write(1);
//            CyDelay(500);
//        }
//    }
// }
 

