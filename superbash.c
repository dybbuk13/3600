/* Yay group 23
**Jackson Kelley
**Eric Guzman
**Taylor Terry
**Super BASH
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>

int main () {
FILE* pbashfile ;
FILE* psuperbash ;

pbashfile = fopen("file", "r") ;
if (pbashfile==NULL){
   printf ("Error reading file.\n\n") ;
   return 0 ;
}
psuperbash = fopen("newfile", "w") ;
if (psuperbash==NULL){
   printf ("Error creating file.\n\n") ;
   return 0 ;
}

//okay the files are "open" now.
//to read from the file use fscanf(pbashfile, *normalscanfstuff)
//write is the same fprintf(psuperbash, *normalprintfstuff)

   return 0 ;
}
