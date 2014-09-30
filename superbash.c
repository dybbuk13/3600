/* Yay group 23
**Jackson Kelley
**Eric Guzman
**Taylor Terry
**Super BASH
*/

#include <stdio.h>
#include <string.h>

int main (int argc, char** argv) {
FILE* pbashfile ;
FILE* psuperbash ;
char bashline[60] ;

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
//we should be able to use sscanf to parse this string and make tests in the while loop...
while (fgets(bashline, 60, pbashfile)) {
   fprintf (psuperbash, "%s\n", bashline) ;
}

   fclose (pbashfile) ;
   fclose (psuperbash) ;
   return 0 ;
}
