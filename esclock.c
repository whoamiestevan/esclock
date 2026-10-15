#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

void passgen(void){// passgen s.

char characters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890()!@#$%^&*";
int length = 100 ;

srand(time(NULL));

printf("> ");

for (int i = 0; i < length; i ++){
printf("%c",characters[rand() % (sizeof(characters) - 1)]);
}
printf("\n");
}// passgen e.

void passgen2(void){// passgen2 .s

char characters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890()!@#$%^&*";
int length = 200 ;

srand(time(NULL));

printf("> ");

for (int i = 0; i < length; i ++){
printf("%c",characters[rand() % (sizeof(characters) - 1)]);
}
printf("\n");
}// passgen2 .e



int main(){
int genpass;
printf("\n1. Generate 100 random password");
printf("\n2. Generate 200 random password");
printf("\n> ");
scanf("%d", &genpass);

if (genpass == 1){
passgen();
} else if (genpass == 2){
passgen2();
}
return 0;
}

