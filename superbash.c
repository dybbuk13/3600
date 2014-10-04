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
char filename[30] ;

pbashfile = fopen(argv[1], "r") ;
if (pbashfile==NULL){
   printf ("Error reading file.\n\n") ;
   return 0 ;
}
psuperbash = fopen("bashfile", "w") ;
if (psuperbash==NULL){
   printf ("Error creating file.\n\n") ;
   return 0 ;
}

//reads file one line at a time and runs tests to see if things need to change
while (fgets(bashline, 60, pbashfile)) {
   if (strchr(bashline, '=') != NULL) {
      fixequal(bashline) ;    
   } 
   if (strstr(bashline, "if")!=NULL && strstr(bashline, "then")!=NULL){
      fixifthen(bashline) ;
   }
   if (strstr(bashline,"repeat")!=NULL && strstr(bashline, "times")!=NULL){
      fixloop(bashline) ;
      fprintf (psuperbash, "%s", bashline) ;
      do {
         fgets(bashline, 200, pbashfile) ;
      } while (strchr(bashline, '{')==NULL) ;
      while (strchr(bashline, '}')==NULL) {
         fgets(bashline, 200, pbashfile) ;
         if (strchr(bashline, '}')==NULL) {
            fprintf (psuperbash, "%s", bashline) ;
         }
         //I don't get why I needed this...
         
      }
      fprintf(psuperbash, "repeatIndex1=$[$repeatIndex1+1]\ndone\n") ; 
      continue ;

   }
   fprintf (psuperbash, "%s", bashline) ;
}

   fclose (pbashfile) ;
   fclose (psuperbash) ;
   return 0 ;
}

//couldn't think of any easier way to do this than to just remove all spaces from the bashline.
//but with a nested loop we can probably copy the lhs into temp, add an '=' with no space, then
//copy the rhs after removing spaces from it.
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
   char temp[200];
   //temp=(char*)malloc(strlen(bashline));
   strncpy(temp,bashline,strlen(bashline)-6);
   strcat(temp,"\n     then\n");
   strcpy(bashline,temp);
   //free(temp);

//I changed this because the strcat on the malloced array often caused faults :( -taylor
//Also the space was causing issues so the 5 became 6
return ;   
}
void fixloop (char bashline[]) {
   char temp[200] ;
   char buff[20] ;
   char var[20] ;
  
   sscanf(bashline, "%s %s", buff, var) ;
   strcpy(temp, "repeatIndex1=0\n\nwhile [ $repeatIndex1 -lt ") ;
   strcat(temp, var) ;
   strcat(temp, " ]\ndo\n") ;
   strcpy(bashline, temp) ;
//this funciton only does the first half of the work, I was a little afraid to let the fuction do file i\o stuff
return ;   
}
