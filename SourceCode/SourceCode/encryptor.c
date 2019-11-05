/*
Filename: encryptor.c
Function of the program: En- and decrypts strings.
Autor: Jan Schüssler

Version 1.0 - 2019-11-05

Version History:
0.1 - 2019-09-19 (Projectstart)


Important findings:
  - use "unsigned" char for a 0-255 range instead -128-127. easier for algorithms
  - character value "0" is seen as the end of a string/file, so avoid getting the value zero for a character
  - in this program 226 mod 226 (226%226) should stay as 226, but mathematically it's 0. solved the problem with a if function to turn the 0 back to 226
  - avoid all characters until 32 (take 32 again, it's space), except 9,10,11 (these are tabulators). use array to "map" all the possible numbers to a upcounting sequence, so modulo and shifts will be easier to handle

Questions:
  - for-loop won't stop counting down if it should stop with argument "i>=0" or "i=0"
  - if I put the do-loop (in main, question if "e" or "d") before opening the file, the program reads three additional character in at the beginning, and the en- / decryption won't work
*/

// Include Headerfiles.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAXCHAR 100000

// un-comment for windows:
const char cls[]="CLS";
const char pause[]="PAUSE";
const int windows=1;

//un-comment for linux:
/*const char cls[]="clear";
const char pause[]="read -n 1 -s -r -p \"Press any key to continue...\"";
const int windows=0;*/

void encryption(unsigned char *txt, unsigned char *pass, int *map2, int *map1, unsigned int *cross_sum, int *cs_pw){
  int size_txt=strlen(txt); // variable with the size (amount of characters) of the text
  int size_pass=strlen(pass); // variable with the size (amount of characters) of the password
  signed int p=0; // countervariable for choosing the passwordcharacter
  int pw_count=0; // how many times will the password be repeated to encrypt the string?
  int cs= (*cross_sum); // see main
  int cs_pass=(*cs_pw); // see main

  for (size_t i = 0; i < size_txt; i++) { // check if the string contains any problenatic characters
    if (txt[i]<9 || (txt[i]>11 && txt[i]<32) || txt[i]==127 || txt[i]>255) {
      printf("\n\nAttention! Plain text contains invalid characters:\n%d\n\n",txt[i]);
      if (windows) {
        system(pause);
      }
      return;
    }
  }

  for (size_t i = 0; i < size_txt; i++) { // mix cross_sum in
    txt[i]=map1[txt[i]]; // map all the characters (9,10,11,32-126,128-255) in the range 0-255 to the possible range (0-226)
    txt[i]=map2[(txt[i]+cs+(8*i)+(size_pass*i)+(cs_pass*size_pass))%226]; // add cross_sum from string plus other numbers. map it back to normal range
    if (txt[i]==0) {
      txt[i]=map2[226]; // 0 should be 255 (map2[226] points to 255)
    }
  }

  txt[size_txt]=map2[cs]; // save the string-cross_sum to the end of the string, because this information will be lost
  txt[size_txt+1]='\0'; // add the new end of the string
  size_txt++; // increase size of string

  for (size_t i = 0; i < size_txt; i++) { // ENCRYPTION 1
    txt[i]=map1[txt[i]];
    txt[i]=map2[(txt[i]+pass[p]+(10*i)+(8*pw_count)+size_pass+cs_pass)%226]; // add value of the passwordcharacter at position "p" and "i" to character from text at position "i"
    if (txt[i]==0) { // in case of 226%226, mathematically it's 0, but in our case we want 226. zero we want to avoid at all cause of complications (end of string)
      txt[i]=map2[226];
    }
    p++;
    if (p>=(size_pass)) { // if variable p reached value of password size, reset to zero
      p=0;
      pw_count++;
      if (pw_count>16000000) {
        printf("String is too long to encrypt!\n");
        if (windows) {
          system(pause);
        }
        return;
      }
    }
  }

  p=0;
  for (size_t i = 0; i < size_txt; i++) { // ENCRYPTION 2: (same as 1 but password starts at the end)
    txt[i]=map1[txt[i]];
    txt[i]=map2[(txt[i]+pass[size_pass-p-1]+i)%226]; // same function as above, just start with the last passwordcharacter to mix it up a bit more
    if (txt[i]==0) {
      txt[i]=map2[226];
    }
    p++;
    if (p=(size_pass)) {
      p=0;
    }
  }

  for (size_t i = 0; i < size_txt; i++) { // changing position of characters
    int character=txt[i]; // save character at position i temporary
    int change=(pass[p]*i+pass[p])%(size_txt-1); // formula to point to the character to switch the position with the character at position i in relation with the password within the string size

    if (change==0) {
      change=size_txt-1;
    }

    txt[i]=txt[change]; // do the switch
    txt[change]=character; // do the switch

    p++;
    if (p==(size_pass)) { // for changing wich password character is used...
      p=0;
    }
  }
}

void decryption(unsigned char *txt, unsigned char *pass, int *map2, int *map1, int *cs_pw){ // same function as "encryption", just undo everything again
  int size_txt=strlen(txt);
  int size_pass=strlen(pass);
  signed int p=0;
  int pw_count=0;
  int cs=0;
  int cs_pass=(*cs_pw);

  p=(size_txt%size_pass); // position where the password was at the end of the string
  if (p==0) { // the modulo thing mentioned in "findings"
    p=size_pass;
  }
  p--;
  for (size_t i = size_txt; i>0; i--) { // change position back
    int change=(pass[p]*(i-1)+pass[p])%(size_txt-1);
    if (change==0) {
      change=size_txt-1;
    }
    int character=txt[change];

    txt[change]=txt[i-1];
    txt[i-1]=character;

    p--;
    if (p<=(-1)) {
      p=size_pass-1;
    }
  }

  p=0;
  for (size_t i = 0; i < size_txt; i++) { // decrypt ENCRYPTION 2
    txt[i]=map1[txt[i]];
    txt[i]=map2[(txt[i]+(226-((pass[size_pass-p-1]+i)%226)))%226];
    if (txt[i]==0) {
      txt[i]=map2[226];
    }
    p++;
    if (p=(size_pass)) {
      p=0;
    }
  }

  p=0;
  for (size_t i = 0; i < size_txt; i++) { // decrypt ENCRYPTION 1
    txt[i]=map1[txt[i]];
    txt[i]=map2[(txt[i]+(226-((pass[p]+(10*i)+(8*pw_count)+size_pass+cs_pass)%226)))%226]; // reversing the calculation from "encryption"
    if (txt[i]==0) {
      txt[i]=map2[226];
    }
    p++;
    if (p>=(size_pass)) {
      p=0;
      pw_count++;
    }
  }

  cs=map1[txt[size_txt-1]]; // read out the saved cross sum wich is now at the end again
  txt[size_txt-1]='\0'; // delete cross sum character with the end of string
  size_txt--; // decrease string length
  for (size_t i = 0; i < size_txt; i++) { // demix cross_sum
    txt[i]=map1[txt[i]];
    txt[i]=map2[(txt[i]+(226-((cs+(8*i)+(size_pass*i)+(cs_pass*size_pass))%226)))%226];
    if (txt[i]==0) {
      txt[i]=map2[226];
    }
  }
}

void counter_map1(int *range){ // create array where all possible numbers (0-255) are mapped to possible numbers (not ANSI anymore, but easy to shift). nonpossible numbers will change to zero
  int p=1;
  for(int i=0; i<32; i++){
      range[i]=0;
  }
  for (size_t i = 9; i < 12; i++) {
    range[i]=p;
    p++;
  }
  for(int i=32; i<127; i++){
      range[i]=p;
      p++;
  }
  range[127]=0;
  for(int i=128; i<256; i++){
      range[i]=p;
      p++;
  }
}

void counter_map2(int *range){ // create array that maps all possible, non-ANSI numbers to its origin place (0-255, ANSI)
  range[0]=0;
  int p=1;
  for(int i=9; i<12; i++){
      range[p]=i;
      p++;
  }
  for(int i=32; i<127; i++){
      range[p]=i;
      p++;
  }
  for(int i=128; i<256; i++){
      range[p]=i;
      p++;
  }
}

void cross_sum_mod(unsigned char *txt, int *cross_sum){ // creates a cross-sum-like sum of the string
  int size_txt=strlen(txt); // length of the string
  for (size_t i = 0; i < size_txt; i++) {
    *cross_sum+=txt[i]*(i+1)+i; // add some numbers
    *cross_sum=(*cross_sum)%226; // cross sum gets lost in encryption, so later we will put it as a ANSI character to the end (after encryption it's probably not at the end anymore)
  }
  if (*cross_sum==0) { // a zero should be a 226
    *cross_sum=226;
  }
}

void cs_pw(unsigned char *pass, int *cs_pass){ // creates a cross-sum-like sum of the password
  int size_pass=strlen(pass); // take the length from the password
  for (size_t i = 0; i < size_pass; i++) {
    *cs_pass+=pass[i]*(i+1)+i*size_pass; // add some numbers
    *cs_pass=(*cs_pass)%100000; // avoid getting the number to be bigger than 100000
  }
}

int main(void){
  system("color 04");
  system(cls);
  printf("TEXTFILE ENCRYPTOR  |  Version 1.0\n\n");
  FILE *ptr_file_rw; // pointer to input file
  unsigned char string_line[MAXCHAR]; // Actual stringline from file
  unsigned char *ptr_line=string_line;
  unsigned char string_total[MAXCHAR]; // String with all strings attached
  unsigned char *ptr_total=string_total;
  unsigned char *filename="secret_message.txt"; //filename

  char en_or_de; // encrypt or decrypt
  unsigned char password[50]; // String where the password is stored in
  unsigned char password_in[50]; // String where the password is stored in, read from a file
  unsigned int cross_sum=0; // cross-sum-like sum of the inputfile-string
  int cs_pass=0; // cross-sum-like sum of the password
  int map1[256]; // see function "countermap"
  int map2[227]; // see function countermap

  counter_map1(map1); // create array where all possible numbers (0-255) are mapped to possible numbers (not ANSI anymore, but easy to shift). nonpossible numbers will change to zero
  counter_map2(map2); // create array that maps all possible, non-ANSI numbers to its origin place (0-255, ANSI)


  ptr_file_rw=fopen(filename,"r"); // open the txtfile in read mode to ptr_file_rw

  if(ptr_file_rw==NULL) // If something went wrong with opening the file...
  {
    printf("Could not open/find %s. Please name your inputfile \"%s\".\n\n",filename,filename);
    if (windows) {
      system(pause);
    }
    return 1;
  }
  else
  {
    int pw_read=1;
    while(fgets(ptr_line,MAXCHAR,ptr_file_rw) != NULL){ // read line per line into the string ptr_line
      strcat(ptr_total,ptr_line); // add every line to ptr_line
      if (pw_read==1 && strlen(ptr_line)<50) { // will take the first line as the password for the fast-encryption
        strcpy(password_in,ptr_line);
        password_in[strlen(password_in)-1]='\0';
        printf("The password for fast-encryption [f] is: %s\n",password_in);
        pw_read=0;
      }
    }
  }
  fclose(ptr_file_rw); // close the file

  do {
    printf("Do you want to encrypt or decrypt your textfile? [e]/[d]/[f]  |  Advanced options: [a]\n"); // chose mode
    fflush(stdin);
    en_or_de=getchar();
  } while(!((en_or_de=='e') || (en_or_de=='d') || (en_or_de=='a') || (en_or_de=='f'))); // force correct input
  system(cls);

  if (en_or_de=='e') {
    cross_sum_mod(ptr_total,&cross_sum); // make the cross-sum-like number with the string
    //printf("To encrypt:\n%s\n\n",ptr_total);
    printf("Type in a safe password (max. 50 signs):\n");
    fflush(stdin);
    scanf("%s",password);
    cs_pw(password,&cs_pass); // make the cross-sum-like number with the password
    printf("\n-----------------------------\nEncrypting...\n-----------------------------\n\n");
    encryption(ptr_total,password,map2,map1,&cross_sum,&cs_pass); // encrypt the string
    printf("Encrypted:\n%s\n",ptr_total);
  }
  if (en_or_de=='d') {
    printf("Type in the correct password to decrypt:\n");
    fflush(stdin);
    scanf("%s",password);
    cs_pw(password,&cs_pass);// make the cross-sum-like number with the password
    printf("\n-----------------------------\nDecrypting...\n-----------------------------\n\n");
    decryption(ptr_total,password,map2,map1,&cs_pass); // undo the encryption
    printf("Decrypted:\n%s\n",ptr_total);
  }
  if (en_or_de=='f') {
    cross_sum_mod(ptr_total,&cross_sum); // make the cross-sum-like number with the string
    strcpy(password, password_in);
    cs_pw(password,&cs_pass); // make the cross-sum-like number with the password
    printf("\n-----------------------------\nEncrypting...\n-----------------------------\n\n");
    encryption(ptr_total,password,map2,map1,&cross_sum,&cs_pass); // encrypt the string
    printf("Encrypted with password: %s\n",password);
  }

  int pass_amount; // variable for the amount how many passwords (and encryptions) should be used
  int encrypt_amount; // variable for the amount how many times the encryption should go over the string
  int check_scan=0; // checkvariable for scanf funtion
  if (en_or_de=='a') {
    do {
      system(cls);
      printf("-----------------------------\nADVANCED OPTIONS\n-----------------------------\n\n");
      printf("Do you want to encrypt or decrypt your textfile? [e]/[d]\n");
      fflush(stdin);
      en_or_de=getchar();
    } while(!((en_or_de=='e') || (en_or_de=='d')));

    printf("\nHow many passwords?\n");
    do {
      fflush(stdin);
      check_scan=scanf("%d",&pass_amount);
    } while(check_scan==0);
    system(cls);
    printf("-----------------------------\nADVANCED OPTIONS\n-----------------------------\n\n");

    if (en_or_de=='e') {
      for (size_t i = 0; i < pass_amount; i++) {
        printf("\nHow many encryptions for password %d/%d?\n",i+1,pass_amount);
        do {
          fflush(stdin);
          check_scan=scanf("%d",&encrypt_amount);
        } while(check_scan==0);
        printf("Type in password nr. %d/%d\n",i+1,pass_amount);
        fflush(stdin);
        scanf("%s",password);
        system(cls);
        printf("-----------------------------\nADVANCED OPTIONS\n-----------------------------\n\n");
        cs_pass=0;
        cs_pw(password,&cs_pass);
        printf("\n-----------------------------\nEncrypting...\n-----------------------------\n\n");
        for (size_t i = 0; i < encrypt_amount; i++) {
          cross_sum_mod(ptr_total,&cross_sum);
          encryption(ptr_total,password,map2,map1,&cross_sum,&cs_pass);
        }
        printf("Encrypted with password %d/%d:\n\n",i+1,pass_amount);
      }
    }

    if (en_or_de=='d') {
      for (size_t i = 0; i < pass_amount; i++) {
        printf("\nHow many decryptions for password %d/%d?\n",pass_amount-i,pass_amount);
        do {
          fflush(stdin);
          check_scan=scanf("%d",&encrypt_amount);
        } while(check_scan==0);
        printf("Type in password nr. %d/%d\n",pass_amount-i,pass_amount);
        fflush(stdin);
        scanf("%s",password);
        system(cls);
        printf("-----------------------------\nADVANCED OPTIONS\n-----------------------------\n\n");
        cs_pass=0;
        cs_pw(password,&cs_pass);
        printf("\n-----------------------------\nDecrypting...\n-----------------------------\n\n");
        for (size_t i = 0; i < encrypt_amount; i++) {
          decryption(ptr_total,password,map2,map1,&cs_pass);
        }
        printf("Decrypted with password %d/%d:\n\n",pass_amount-i,pass_amount);
      }
    }
  }
  ptr_file_rw=fopen(filename,"w"); // open the textfile in write mode
  fprintf(ptr_file_rw,"%s",ptr_total); // write the en- or decrypted string into the textfile
  fclose(ptr_file_rw); // close the file

  printf("\n");
  if (en_or_de=='e') {
    printf("Encryption completed.\n");
  }

  if (en_or_de=='d') {
    printf("Decryption completed.\n");
  }
  printf("\n");

  if (windows) {
    system(pause);
  }
  return 0;

}
