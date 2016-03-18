#include <stdio.h>
#include "backdoor_mgr.h"

int main (int argc, const char * argv[]){
    char string[10+1];
        
    printf("Input max %ld character: ", sizeof(string)-1);
    BACKDOOR_MGR_RequestKeyIn(string, sizeof(string));
    
    printf("Your input is : ");
    BACKDOOR_MGR_Printf("%s", string);
    return 0;
}
