/* Yay group 23
**Jackson Kelley
**Eric Guzman
**Taylor Terry
**Super BASH
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void fixequal (char bashline[]) ;
void fixifthen (char bashline[]) ;
void fixloop (char bashline[]) ;

int main (int argc, char** argv) {
FILE* pbashfile ;
FILE* psuperbash ;
char bashline[60] ;
char *test1, *test2 ;

pbashfile = fopen(argv[1], "r") ;
if (pbashfile==NULL){
   printf ("Error reading file.\n\n") ;
   return 0 ;
}
psuperbash = fopen("newfile", "w") ;
if (psuperbash==NULL){
   printf ("Error creating file.\n\n") ;
   return 0 ;
}

//okay, this should theoretically just copy the entire file right now
//IT DOES, I TESTED :D
//we should be able to use sscanf to parse this string and make tests in the while loop...
//the loop case is the only challenge now :)
while (fgets(bashline, 60, pbashfile)) {
   test1 = strchr(bashline, '=') ;
   if (test1 != NULL) {
      fixequal(bashline) ;    
   } 
   test1 = strstr(bashline, "if") ;
   test2 = strstr(bashline, "then") ;
   if (test1!=NULL && test2!=NULL){
      fixifthen(bashline) ;
   }
   fprintf (psuperbash, "%s", bashline) ;
}

   fclose (pbashfile) ;
   fclose (psuperbash) ;
   return 0 ;
}

void fixequal (char bashline[]) {
  int i, j;
  char* temp1;
  
  temp1 = (char *)malloc(strlen(bashline));
  for(i=0, j=0; i < strlen(bashline); i++){
     if(bashline[i] != ' '){
        temp1[j] = bashline[i];
        j++;
     }
  }
  
  strcpy(bashline, temp1);
  free(temp1);
  
return ;
}
void fixifthen (char bashline[]) {
   char *temp;
   temp=(char*)malloc(strlen(bashline));
   strncpy(temp,bashline,strlen(bashline)-5);
   strcat(temp,"\n\ttemp\n");
   strcpy(bashline,temp);
   
return ;   
}
void fixloop (char bashline[]) {
   
return ;   
}
