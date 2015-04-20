/**
 * guuid.c
 *
 * Thomas Gouverneur - 2013
 */
/* gcc -o guuid guuid.c -luuid */
#include <uuid/uuid.h>
#include <stdio.h>

int main(void){
  uuid_t u; 
  char uu[UUID_PRINTABLE_STRING_LENGTH+1]; 
  uuid_generate_random(u); 
  uuid_unparse(u, uu); 
  printf("%s\n", uu); 
  return 0; 
}
