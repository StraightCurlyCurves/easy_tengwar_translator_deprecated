/*
EASY TENGWAR TRANSLATOR

Dieses Programm übersetzt ein normal geschriebener Text in das richtige Format,
um von der Tengwar-Schriftart richtig dargestellt zu werden.

Im Header kann zwischen deutsch und englisch umgeschaltet werden, womit das
Programm entweder das "und" oder das "and" mit dem und-Symbol ersetzt.

Die anderen zwei Konstanten sollten so belassen werden.

Autor: Jan Schüssler
Version: 1.0
Datum: 02.12.2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define MAXCHAR 1000000 // Maximale Zeichen die man einlesen kann

const bool german_and=0; // ersetzt alle "und" in & bzw. #
const bool english_and=1; // ersetzt alle "and" in & bzw. #

// 0-0 Konfiguration ist am einfachsten zu lesen (mehr db als wh), andere Konfiguration könnte auch zu Designfehlern führen.
const bool wh_first=0; // 1: tott=t[wh]o, 0: tott=tot[db] | wh=wiederholung, db=doppelbuchstaben
const bool wh_over_db=0; // 1: mmm=m[wh]m, 0: mmm=mm[db] | wh=wiederholung, db=doppelbuchstaben

void cap_to_small(char *w) //Gross- in Kleinbuchstaben umwandeln
{
  int size=strlen(w); // Grösse des eingelesenen Strings ermitteln
  char compare[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // Die zu überprüfenden Buchstaben
  char replace[]="abcdefghijklmnopqrstuvwxyz"; // Die entsprechende Ersetzung
  int size_compare=strlen(compare); // Anzahl der zu überprüfenden Buchstaben

  for (size_t i = 0; i < size; i++) { // Stringwert, der überprüft wird
    for (size_t j = 0; j < size_compare; j++) { // Vergleichsstring, der überprüft wird
      if(w[i]==compare[j])
      {
        w[i]=replace[j]; // Ersetzung in Kleinbuchstabe
        break; // Springt direkt zur Überprüfung des nächsten Buchstabens
      }
    }
  }
}
void specialsigns(char *w) // Gewisse spezialzeichen ersetzen
{
  int size=strlen(w);

  for (size_t i = 0; i < size; i++) {
    if(w[i]==(-33)) // Scharfes S (nicht ASCII)
    {
      w[i]='$';
    }
  }
  for (size_t i = 0; i < size; i++) {
    if(w[i]==(-124)) // Anführungszeichen unten (nicht ASCII)
    {
      w[i]='@';
    }
  }
  for (size_t i = 0; i < size; i++) {
    if(w[i]==(-109)) // Anführungszeichen oben (nicht ASCII)
    {
      w[i]='"';
    }
  }
  for (size_t i = 0; i < size; i++) {
    if(w[i]==(-106)) // Bindestrich (nicht ASCII)
    {
      w[i]='-';
    }
  }
  for (size_t i = 0; i < size; i++) {
    if(w[i]==(-110)) // Apostroph (nicht ASCII)
    {
      w[i]=39;
    }
  }
}
void uml_r(char *w) // Umlautzeichen vor Umlaute setzen
{
  int size=strlen(w);
  int check[]={-28,-10,-4,-60,-42,-36}; //äöüÄÖÜ Werte, wenn sie per fgets kommen.
  char replace[]={-28,-10,-4,-28,-10,-4}; //äöüäöü
  char new[2*size]; //temporärer String wird unbestimmt grösser (max 2mal)
  // int size_check=strlen(replace); // ACHTUNG es ist nicht die Anzahl an Werten, diese Codezeile wird nicht benutzt und kann eigentlich gelöscht werden

  for (size_t i = 0; i < size; i++) { // Stringwert, der überprüft wird
    for (size_t j = 0; j < 6; j++) { // Vergleichsstring, der überprüft wird
      if(w[i]==check[j])
      {
        for (size_t k = 0; k < i; k++) { // neuer string bis zum gefundenen Umlaut gleich setzen
          new[k]=w[k];
        }
        new[i]='M'; // Umlaut ersetzen durch Umlautzeichen
        new[i+1]=replace[j]; // Entsprechenden Vokal als nächsten Buchstaben setzen. Zufälligerweise sind es gleich die Umlaute selbst, welche die korrekte Position für das Umlautzeichen haben
        for (size_t z = i+2; z <= size; z++) { // vorverschieben: temporärer String nimmt an Stelle z den Wert des alten Strings an Stelle z-1 an, da sich der temporäre String mit dem eingeschobenen M um eins verlängert hat
          new[z]=w[z-1];
        }
        i++; //damit der neu gesetzte Umlaut (der gewollte) nicht mehr überprüft wird
        new[size+1]='\0'; // Ende des Strings festlegen
        strcpy(w,new); // in Originalstring kopieren
        size++; // String wurde um 1 grösser. damit die for-Schlaufe weiterhin funktioniert, muss dies berücksichtigt werden
        break; // Springt direkt zur Überprüfung des nächsten Buchstabens
      }
    }
  }
}
void und_and_to_sign(char *w) // und oder and ersetzen
{
  int size=strlen(w);
  char new[size];
  char und[]="&"; // und-Symbol
  // char und_slim[]="#"; Das Design wird erst später definiert, Zeile könnte gelöscht werden

  if(german_and) // falls ganz oben german_and auf 1 ist...
  {
    for (size_t i = 0; i < size-2; i++) { // size-2, da danach kein und mehr kommen kann
      if(w[i]=='u' && w[i+1]=='n' && w[i+2]=='d') // überprüfung, ob ab Stelle i ein u, dann ein n und d folgt
      {
        for (size_t j = 0; j < i; j++) { // neuer string bis zum gefundenen "und" gleich setzen (bis und ohne u)
          new[j]=w[j];
        }

        new[i]=und[0]; // u mit & ersetzen

        for (size_t k = i+1; k < size-2; k++) { // size-2, da der temporäre String k+2 vom Originalen bekommt
          new[k]=w[k+2]; // temporärer String neu Aufbauen, aber um zwei zurückverschoben
        }
        new[size-2]='\0'; // Ende des Strings festlegen
        if(size>3) // Da bei der for-Schlaufe sonst eine Minuszahl überprüft wird.
        {
          size=size-2; // der neue String ist um zwei Zeichen kürzer. wichtig, damit die for Schlaufe funktioniert
        }
        strcpy(w,new); // in Originalstring kopieren
      }
    }
  }

  else if(english_and) // falls ganz oben english_and auf 1 ist...
  {
    for (size_t i = 0; i < size-2; i++) { // size-2, da danach kein and mehr kommen kann
      if(w[i]=='a' && w[i+1]=='n' && w[i+2]=='d') // überprüfung, ob ab Stelle i ein a, dann ein n und d folgt
      {
        for (size_t j = 0; j < i; j++) { // neuer string bis zum gefundenen "and" gleich setzen (bis und ohne a)
          new[j]=w[j];
        }

        new[i]=und[0]; // a mit & ersetzen

        for (size_t k = i+1; k < size-2; k++) { // size-2, da der temporäre String k+2 vom Originalen bekommt
          new[k]=w[k+2]; // temporärer String neu Aufbauen, aber um zwei zurückverschoben
        }
        new[size-2]='\0'; // Ende des Strings festlegen
        if(size>3) // Da bei der for-Schlaufe sonst eine Minuszahl überprüft wird.
        {
          size=size-2; // der neue String ist um zwei Zeichen kürzer. wichtig, damit die for Schlaufe funktioniert
        }
        strcpy(w,new); // in Originalstring kopieren
      }
    }
  }
}
void sch_to_C(char *w) // Hier wird das "sch" durch ein C ersetzt
{
  int size=strlen(w);
  char new[size];
  char sch[]="C"; // sch-Symbol

  for (size_t i = 0; i < size-2; i++) { // size-2, da danach kein sch mehr kommen kann
    if(w[i]=='s' && w[i+1]=='c' && w[i+2]=='h') // überprüfung, ob ab Stelle i ein s, dann ein c und h folgt
    {
      for (size_t j = 0; j < i; j++) { // neuer string bis zum gefundenen "sch" gleich setzen (bis und ohne s)
        new[j]=w[j];
      }
      new[i]=sch[0]; // sch-Symbol an stelle des "s" einsetzen
      for (size_t k = i+1; k < size-2; k++) { // size-2, da der temporäre String k+2 vom Originalen bekommt
        new[k]=w[k+2]; // temporärer String neu Aufbauen, aber um zwei zurückverschoben
      }
      new[size-2]='\0'; // Ende des Strings festlegen
      if(size>3) // Da bei der for-Schlaufe sonst eine Minuszahl überprüft wird.
      {
        size=size-2; // der neue String ist um zwei Zeichen kürzer. wichtig, damit die for Schlaufe funktioniert
      }
      strcpy(w,new); // in Originalstring kopieren
    }
  }
}
void ch_to_0(char *w) // ch durch ch-Symbol ersetzen
{
  int size=strlen(w);
  char new[size];
  char ch[]="0"; // ch-Symbol

  for (size_t i = 0; i < size-1; i++) { // size-1, da danach kein ch mehr kommen kann
    if(w[i]=='c' && w[i+1]=='h') // überprüfung, ob ab Stelle i ein c und dann ein h folgt
    {
      for (size_t j = 0; j < i; j++) { // neuer string bis zum gefundenen "ch" gleich setzen (bis und ohne c)
        new[j]=w[j];
      }
      new[i]=ch[0]; // ch-Symbol an stelle des "c" einsetzen
      for (size_t k = i+1; k < size-1; k++) { // size-1, da der temporäre String k+1 vom Originalen bekommt
        new[k]=w[k+1]; // temporärer String neu Aufbauen, aber um zwei zurückverschoben
      }
      new[size-1]='\0'; // Ende des Strings festlegen
      if(size>2) // Da bei der for-Schlaufe sonst eine Minuszahl überprüft wird.
      {
        size=size-1; // der neue String ist um ein Zeichen kürzer. wichtig, damit die for Schlaufe funktioniert
      }
      strcpy(w,new); // in Originalstring kopieren
    }
  }
}
void db(char *w) // Doppelbuchstaben mit Doppelbuchstaben-Symbol darstellen
{
  int size=strlen(w);
  char db[]="1"; // db-Symbol
  char cons[]="bcdfghjklmnpqrstvwxyz&0CH$"; // Zu überprüfende Zeichen
  int size_cons=strlen(cons); // Anzahl der zu überprüfenden Zeichen

  for (size_t i = 0; i < size-1; i++) { // size-1 da danach kein doppelbuchstabe mehr kommen kann
    if(w[i]==w[i+1]) // überprüfung, ob ab Stelle i ein doppelbuchstaben kommt (i+1 ist gleich i)
    {
      if(wh_over_db==1 && w[i]==w[i+1] && w[i]==w[i+2]) // falls wh_over_db ein wird dreifachbuchstaben nicht ersetzt. mmm wird nicht zu m*m, auch wenn db zuerst käme (einstellung im header)
      {
        continue;
      }
      if(w[i]==w[i+1] && (w[i+2]=='7' || w[i+2]=='8' || w[i+2]=='9' || w[i+2]=='Q' || w[i+2]=='Z')) // Situation: mm7o(...mmom...); falls wh zuerst kam, kann dies vorkommen. dann darf dieser doppelbuchstabe nicht mehr ersetzt werden.
      {
        continue;
      }
      for (size_t j = 0; j < size_cons; j++) {
        if(w[i]==cons[j]) // Überprüfung mit cons-String erforderlich, dass nur Konsonanten ersetzt werden
        {
          w[i+1]=db[0]; // zweiter Buchstabe mit db Zeichen ersetzen
        }
      }
    }
  }
}
void wh(char *w) // Zeichenwiederholung mit entsprechendem Zeichen darstellen
{
  int size=strlen(w);
  char wh[]="Q"; // wh-Symbol
  char cons[]="bcdfghjklmnpqrstvwxyz&0CH$"; // Zu überprüfende Zeichen
  int size_cons=strlen(cons); // Anzahl der zu überprüfenden Zeichen

  for (size_t i = 0; i < size-2; i++) { // size-2 da danach kein wiederholbuchstaben mehr kommen kann
    if(w[i]==w[i+2] && w[i+1]!=' ' && w[i+1]!='\n') // überprüfung, ob ab Stelle i ein wiederholbuchstaben kommt (i+2 ist gleich i), aber nicht über ein leerzeichen hinweg
    {
      if(wh_over_db==0 && w[i]==w[i+1] && w[i]==w[i+2]) //Nicht ausführen, falls dreifachbuchstabe kommt (mmm) und wh_over_db aus und ist und wh zuerst kommt (wh_first=1)
      {
        continue;
      }
      if((wh_over_db==0 && wh_first==0) && (w[i+1]=='1' || w[i+1]=='2' || w[i+1]=='3' || w[i+1]=='4' || w[i+1]=='5' || w[i+1]=='6') && w[i]==w[i+2]) //Nicht ausführen, falls dreifachbuchstabe kommt (m1m)
      {
        continue;
      }
      if(wh_first==0 && w[i]==w[i+2] && (w[i+3]=='1' || w[i+3]=='2' || w[i+3]=='3' || w[i+3]=='4' || w[i+3]=='5' || w[i+3]=='6')) // falls db zuerst, bei allfälligen db buchstaben wh nicht ausführen; zb. bei mom1
      {
        continue;
      }
      for (size_t j = 0; j < size_cons; j++) {
        if(w[i]==cons[j]) // Überprüfung mit cons-String erforderlich, dass nur Konsonanten ersetzt werden
        {
          w[i+2]=w[i+1]; // der mittlere Buchstabe eins nach hinten verschieben
          w[i+1]=wh[0]; // wh Zeichen einfügen
        }
      }
    }
  }
}
void vowel_first_letter(char *w) // Vokalausfüllzeichen bei erstem Buchstabe
{
  int size=strlen(w);
  char new[2*size]; //temporärer String wird unbestimmt grösser (max 2mal)
  char vowel[]="Y"; // Ausfüllzeichen
  //
  char all_signs[]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9','0','A','E','I','O','U','S','C','H','B','N','M','Y','G','Q','Z',-23,-32,-28,-10,-4,'%','+','&','$','\0'};
  int size_all_signs=strlen(all_signs);

  int check_all_signs=0;

  if(w[0]=='a' || w[0]=='e' || w[0]=='i' || w[0]=='o' || w[0]=='u') // Erster Buchstabe des Strings überprüfen und ersetzen
  {
    new[0]=vowel[0];
    for (size_t i = 0; i < size; i++) {
      new[i+1]=w[i]; // temporärer String neu Aufbauen, aber um eins vorverschoben
    }
    new[size+1]='\0'; // Ende des Strings festlegen
    strcpy(w,new);
    size++; // String wurde um 1 grösser. damit die for-Schlaufe funktioniert, muss dies berücksichtigt werden
  }

  for (size_t j = 1; j < size; j++) { // Restliche Zeichen überprüfen
    check_all_signs=0; // Falls das vorherige Zeichen eines der Zeichen in all_signs ist, muss kein Vokalausfüllzeichen benutzt werden
    for (size_t k = 0; k < size_all_signs; k++) {
      if((w[j-1]==all_signs[k]))
      {
        check_all_signs++;
      }
    }
    if(check_all_signs<1 && (w[j]=='a' || w[j]=='e' || w[j]=='i' || w[j]=='o' || w[j]=='u')) // Falls das vorherige Zeichen keines der Zeichen in all_signs ist, wird ein Vokalausfüllzeichen benutzt
    {
      for (size_t z = 0; z < j; z++) { // neuer string bis zum gefundenen Vokal gleich setzen
        new[z]=w[z];
      }
      new[j]=vowel[0]; // Vokal mit Ausfüllzeichen ersetzen
      for (size_t k = j; k < size; k++) {
        new[k+1]=w[k]; // temporärer String neu Aufbauen, aber um eins vorverschoben
      }
      new[size+1]='\0'; // Ende des Strings festlegen
      strcpy(w,new);
      size++; // String wurde um 1 grösser. damit die for-Schlaufe funktioniert, muss dies berücksichtigt werden
    }
  }
}
void dv(char *w) //doppelvokal-Zeichen
{
  int size=strlen(w);
  char new[2*size]; //temporärer String wird unbestimmt grösser (max 1.5 mal)
  char vowel[]="B"; // db-Zeichen

  // Normal; zwei Vokale hintereinander: mein--> meBin
  for (size_t i = 0; i < size-1; i++) {
    if((w[i]=='a' || w[i]=='e' || w[i]=='i' || w[i]=='o' || w[i]=='u' || w[i]=='f' || w[i]=='&' || w[i]==(-28) || w[i]==(-10) || w[i]==(-4)) && (w[i+1]=='a' || w[i+1]=='e' || w[i+1]=='i' || w[i+1]=='o' || w[i+1]=='u')) // Falls zwei Vokale oder f/Vokal oder &/Vokal hintereinander, dann...
    {
      for (size_t j = 0; j <= i; j++) {
        new[j]=w[j]; // neuer string bis und MIT dem gefundenen (ersten) Vokal gleich setzen
      }
      new[i+1]=vowel[0]; // Vokalausfüllzeichen mit zweitem Vokal ersetzen
      for (size_t k = i+2; k <= size; k++) {
        new[k]=w[k-1]; // temporärer String neu Aufbauen, aber um eins vorverschoben
      }
      new[size+1]='\0'; // Ende des Strings festlegen
      strcpy(w,new);
      size++; // String wurde um 1 grösser. damit die for-Schlaufe funktioniert, muss dies berücksichtigt werden
      }
    }

  //Bei db f und &
  for (size_t i = 0; i < size-2; i++) {
    if((w[i]=='f' || w[i]=='&') && (w[i+1]=='1') && (w[i+2]=='a' || w[i+2]=='e' || w[i+2]=='i' || w[i+2]=='o' || w[i+2]=='u'))
    {
      for (size_t j = 0; j <= i+1; j++) {
        new[j]=w[j]; // neuer string bis und MIT dem gefundenen (ersten) Vokal gleich setzen
      }
      new[i+2]=vowel[0]; // Vokalausfüllzeichen mit zweitem Vokal ersetzen
      for (size_t k = i+3; k <= size; k++) {
        new[k]=w[k-1]; // temporärer String neu Aufbauen, aber um eins vorverschoben
      }
      new[size+1]='\0'; // Ende des Strings festlegen
      strcpy(w,new);
      size++; // String wurde um 1 grösser. damit die for-Schlaufe funktioniert, muss dies berücksichtigt werden
    }
  }

  //Bei wh
  for (size_t i = 0; i < size-2; i++) {
    if((w[i]=='f' || w[i]=='&') && (w[i+1]=='Q') && (w[i+2]=='a' || w[i+2]=='e' || w[i+2]=='i' || w[i+2]=='o' || w[i+2]=='u'))
    {
      for (size_t j = 0; j <= i+1; j++) {
        new[j]=w[j]; // neuer string bis und MIT dem gefundenen (ersten) Vokal gleich setzen
      }
      new[i+2]=vowel[0]; // Vokalausfüllzeichen mit zweitem Vokal ersetzen
      for (size_t k = i+3; k <= size; k++) {
        new[k]=w[k-1]; // temporärer String neu Aufbauen, aber um eins vorverschoben
      }
      new[size+1]='\0'; // Ende des Strings festlegen
      strcpy(w,new);
      size++; // String wurde um 1 grösser. damit die for-Schlaufe funktioniert, muss dies berücksichtigt werden
    }
  }
}
void design_g_G(char *w) // g zu G bei db, wh oder nachfolgendem Vokal
{
  int size=strlen(w);

  for (size_t i = 0; i < size; i++) { // Stringwert, der überprüft wird
    if(w[i]=='g' && (w[i+1]=='1' || w[i+1]=='Q' || w[i+1]=='a' || w[i+1]=='e' || w[i+1]=='i' || w[i+1]=='o' || w[i+1]=='u'))
    {
      w[i]='G';
    }
  }
}
void design_sch(char *w) // Breite des sch-Zeichen je nach nachfolgendem Zeichen ändern
{
  int size=strlen(w);

  for (size_t i = 0; i < size; i++) { // Stringwert, der überprüft wird
    if(w[i]=='C' && (w[i+1]=='b' || w[i+1]=='h' || w[i+1]=='p' || w[i+1]=='z' || w[i+1]=='r'))
    {
      w[i]='H';
    }
  }
}
void design_db(char *w) // db-Zeichen an zu verdoppelndes Zeichen anpassen
{
  int size=strlen(w);

  for (size_t i = 0; i < size; i++) { // Stringwert, der überprüft wird
    if(w[i]=='1' && (w[i-1]=='f' || w[i-1]=='&'))
    {
      w[i]='2';
    }
    if(w[i]=='1' && (w[i-1]=='G' || w[i-1]=='k' || w[i-1]=='s'))
    {
      w[i]='3';
    }
    if(w[i]=='1' && (w[i-1]=='j' || w[i-1]=='n' || w[i-1]=='q' || w[i-1]=='x' || w[i-1]=='y' || w[i-1]=='z' || w[i-1]=='S' || w[i-1]=='C'))
    {
      w[i]='4';
    }
    if(w[i]=='1' && (w[i-1]=='l' || w[i-1]=='r'))
    {
      w[i]='5';
    }
    if(w[i]=='1' && (w[i-1]=='t'))
    {
      w[i]='6';
    }
  }
}
void design_wh(char *w) // wh-Zeichen an zu verdoppelndes Zeichen anpassen
{
  int size=strlen(w);

  for (size_t i = 0; i < size; i++) { // Stringwert, der überprüft wird
    if(w[i]=='Q' && (w[i-1]=='t'))
    {
      w[i]='7';
    }
    if(w[i]=='Q' && (w[i-1]=='l' || w[i-1]=='r'))
    {
      w[i]='8';
    }
    if(w[i]=='Q' && (w[i-1]=='h' || w[i-1]=='f' || w[i-1]=='k' || w[i-1]=='m' || w[i-1]=='n' || w[i-1]=='s' || w[i-1]=='x' || w[i-1]=='0' || w[i-1]=='C' || w[i-1]=='&'))
    {
      w[i]='9';
    }
    if(w[i]=='Q' && (w[i-1]=='z' || (w[i-1]=='f' && w[i-3]=='M')))
    {
      w[i]='Z';
    }
  }
}
void design_uml(char *w) //uml-Zeichen anpassen, je nach nachfolgendem Zeichen
{
  int size=strlen(w);

  for (size_t i = 0; i < size; i++) { // Stringwert, der überprüft wird
    if(w[i]=='M' && ((w[i+2]=='s' && w[i+3]=='3') || (w[i+2]=='z' && w[i+3]=='4') || (w[i+2]=='G' && w[i+3]=='Q') || (w[i+2]=='s' && w[i+3]=='9'))) // Falls die db oder wh-Zeichen ins uml-Zeichen kommen...
    {
      w[i]='N';
      if(w[i+1]==(-28)) // ä
      {
        w[i+1]='a';
      }
      if(w[i+1]==(-10)) // ö
      {
        w[i+1]='o';
      }
      if(w[i+1]==(-4)) // ü
      {
        w[i+1]='u';
      }
    }
  }
}
void design_va(char *w) // Variante des Vokals "a"
{
  int size=strlen(w);

  for (size_t i = 0; i < size; i++) { // Stringwert, der überprüft wird
    //direkt nach Buchstabe:
    if(w[i]=='a' && (w[i-1]=='0' || w[i-1]=='c' || w[i-1]=='d' || w[i-1]=='h' || w[i-1]=='m' || w[i-1]=='p' || w[i-1]=='H'))
    {
      w[i]='A';
    }
    if(w[i]=='a' && (w[i-1]=='j' || w[i-1]=='t' || w[i-1]=='v' || w[i-1]=='w'))
    {
      w[i]=(-28); //ä
    }
    if(w[i]=='a' && (w[i-1]=='n' || w[i-1]=='s' || w[i-1]=='x' || w[i-1]=='y'))
    {
      w[i]=(-32); //à
    }

    //Nach db:
    if(w[i]=='a' && (w[i-1]=='1' || w[i-1]=='2' || w[i-1]=='3' || w[i-1]=='4' || w[i-1]=='5' || w[i-1]=='6'))
    {
      if(w[i-2]=='0' || w[i-2]=='c' || w[i-2]=='d' || w[i-2]=='h' || w[i-2]=='m' || w[i-2]=='p' || w[i-2]=='H')
      {
        w[i]='A';
      }
      if(w[i-2]=='j' || w[i-2]=='t' || w[i-2]=='v' || w[i-2]=='w')
      {
        w[i]=(-28); //ä
      }
      if(w[i-2]=='n' || w[i-2]=='s' || w[i-2]=='x' || w[i-2]=='y')
      {
        w[i]=(-32); //à
      }
    }

    //Nach wh:
    if(w[i]=='a' && (w[i-1]=='7' || w[i-1]=='8' || w[i-1]=='9' || w[i-1]=='Q' || w[i-1]=='Z'))
    {
      if(w[i-2]=='0' || w[i-2]=='c' || w[i-2]=='d' || w[i-2]=='h' || w[i-2]=='m' || w[i-2]=='p' || w[i-2]=='H')
      {
        w[i]='A';
      }
      if(w[i-2]=='j' || w[i-2]=='t' || w[i-2]=='v' || w[i-2]=='w')
      {
        w[i]=(-28); //ä
      }
      if(w[i-2]=='n' || w[i-2]=='s' || w[i-2]=='x' || w[i-2]=='y')
      {
        w[i]=(-32); //à
      }
    }

    //Spezialfall "L"
    if(w[i]=='a' && w[i-1]=='l' && (w[i-2]=='w' || w[i-2]=='d' || w[i-2]=='g' || w[i-2]=='G' || w[i-2]=='j' || w[i-2]=='k' || w[i-2]=='v' || w[i-2]=='B' || w[i-2]=='&' || w[i-2]=='#'))
    {
      w[i]=(-32);
    }
    //Spezialfall "L" nach db
    if(w[i]=='a' && w[i-1]=='l' && (w[i-2]=='1' || w[i-2]=='2' || w[i-2]=='3' || w[i-2]=='4' || w[i-2]=='5' || w[i-2]=='6') && (w[i-3]=='w' || w[i-3]=='d' || w[i-3]=='g' || w[i-3]=='G' || w[i-3]=='j' || w[i-3]=='k' || w[i-3]=='v' || w[i-3]=='B' || w[i-3]=='&' || w[i-3]=='#'))
    {
      w[i]=(-32);
    }
    //Spezialfall "L" nach wh
    if(w[i]=='a' && w[i-1]=='l' && (w[i-3]=='7' || w[i-3]=='8' || w[i-3]=='9' || w[i-3]=='Q' || w[i-3]=='Z') && (w[i-4]=='w' || w[i-4]=='d' || w[i-4]=='g' || w[i-4]=='G' || w[i-4]=='j' || w[i-4]=='k' || w[i-4]=='v' || w[i-4]=='B' || w[i-4]=='&' || w[i-4]=='#'))
    {
      w[i]=(-32);
    }
  }
}
void design_ve(char *w) // Variante des Vokals "e"
{
  int size=strlen(w);

  for (size_t i = 0; i < size; i++) { // Stringwert, der überprüft wird
    if(w[i]=='e' && (w[i-1]=='0' || w[i-1]=='c' || w[i-1]=='d' || w[i-1]=='j' || w[i-1]=='m' || w[i-1]=='p' || w[i-1]=='t' || w[i-1]=='v' || w[i-1]=='w' || w[i-1]=='H'))
    {
      w[i]='E';
    }
    if(w[i]=='e' && (w[i-1]=='n' || w[i-1]=='s' || w[i-1]=='x' || w[i-1]=='y' || w[i-1]=='z'))
    {
      w[i]=(-23); //é
    }

    //Nach db:
    if(w[i]=='e' && (w[i-1]=='1' || w[i-1]=='2' || w[i-1]=='3' || w[i-1]=='4' || w[i-1]=='5' || w[i-1]=='6'))
    {
      if(w[i-2]=='0' || w[i-2]=='c' || w[i-2]=='d' || w[i-2]=='j' || w[i-2]=='m' || w[i-2]=='p' || w[i-2]=='t' || w[i-2]=='v' || w[i-2]=='w' || w[i-2]=='H')
      {
        w[i]='E';
      }
      if(w[i-2]=='n' || w[i-2]=='s' || w[i-2]=='x' || w[i-2]=='y' || w[i-2]=='z')
      {
        w[i]=(-23); //é
      }
    }

    //Nach wh:
    if(w[i]=='e' && (w[i-1]=='7' || w[i-1]=='8' || w[i-1]=='9' || w[i-1]=='Q' || w[i-1]=='Z'))
    {
      if(w[i-2]=='0' || w[i-2]=='c' || w[i-2]=='d' || w[i-2]=='j' || w[i-2]=='m' || w[i-2]=='p' || w[i-2]=='t' || w[i-2]=='v' || w[i-2]=='w' || w[i-2]=='H')
      {
        w[i]='E';
      }
      if(w[i-2]=='n' || w[i-2]=='s' || w[i-2]=='x' || w[i-2]=='y' || w[i-2]=='z')
      {
        w[i]=(-23); //é
      }
    }

    //Spezialfall "L"
    if(w[i]=='e' && w[i-1]=='l' && (w[i-2]=='w' || w[i-2]=='d' || w[i-2]=='g' || w[i-2]=='G' || w[i-2]=='j' || w[i-2]=='k' || w[i-2]=='v' || w[i-2]=='B' || w[i-2]=='&' || w[i-2]=='#'))
    {
      w[i]=(-23);
    }
    //Spezialfall "L" nach db
    if(w[i]=='e' && w[i-1]=='l' && (w[i-2]=='1' || w[i-2]=='2' || w[i-2]=='3' || w[i-2]=='4' || w[i-2]=='5' || w[i-2]=='6') && (w[i-3]=='w' || w[i-3]=='d' || w[i-3]=='g' || w[i-3]=='G' || w[i-3]=='j' || w[i-3]=='k' || w[i-3]=='v' || w[i-3]=='B' || w[i-3]=='&' || w[i-3]=='#'))
    {
      w[i]=(-23);
    }
    //Spezialfall "L" nach wh
    if(w[i]=='e' && w[i-1]=='l' && (w[i-3]=='7' || w[i-3]=='8' || w[i-3]=='9' || w[i-3]=='Q' || w[i-3]=='Z') && (w[i-4]=='w' || w[i-4]=='d' || w[i-4]=='g' || w[i-4]=='G' || w[i-4]=='j' || w[i-4]=='k' || w[i-4]=='v' || w[i-4]=='B' || w[i-4]=='&' || w[i-4]=='#'))
    {
      w[i]=(-23);
    }
  }
}
void design_vi(char *w) // Variante des Vokals "i"
{
  int size=strlen(w);

  for (size_t i = 0; i < size; i++) { // Stringwert, der überprüft wird
    if(w[i]=='i' && (w[i-1]=='0' || w[i-1]=='H' || w[i-1]=='d' || w[i-1]=='m' || w[i-1]=='p' || w[i-1]=='t' || w[i-1]=='v' || w[i-1]=='w'))
    {
      w[i]='I';
    }
    if(w[i]=='i' && (w[i-1]=='n' || w[i-1]=='s' || w[i-1]=='x' || w[i-1]=='y' || w[i-1]=='z'))
    {
      w[i]='%';
    }

    //Nach db:
    if(w[i]=='i' && (w[i-1]=='1' || w[i-1]=='2' || w[i-1]=='3' || w[i-1]=='4' || w[i-1]=='5' || w[i-1]=='6'))
    {
      if(w[i-2]=='0' || w[i-2]=='H' || w[i-2]=='d' || w[i-2]=='m' || w[i-2]=='p' || w[i-2]=='t' || w[i-2]=='v' || w[i-2]=='w')
      {
        w[i]='I';
      }
      if(w[i-2]=='n' || w[i-2]=='s' || w[i-2]=='x' || w[i-2]=='y' || w[i-2]=='z')
      {
        w[i]='%';
      }
    }

    //Nach wh:
    if(w[i]=='i' && (w[i-1]=='7' || w[i-1]=='8' || w[i-1]=='9' || w[i-1]=='Q' || w[i-1]=='Z'))
    {
      if(w[i-2]=='0' || w[i-2]=='H' || w[i-2]=='d' || w[i-2]=='m' || w[i-2]=='p' || w[i-2]=='t' || w[i-2]=='v' || w[i-2]=='w')
      {
        w[i]='I';
      }
      if(w[i-2]=='n' || w[i-2]=='s' || w[i-2]=='x' || w[i-2]=='y' || w[i-2]=='z')
      {
        w[i]='%';
      }
    }
  }
}
void design_vo(char *w) // Variante des Vokals "o"
{
  int size=strlen(w);

  for (size_t i = 0; i < size; i++) { // Stringwert, der überprüft wird
    if(w[i]=='o' && (w[i-1]=='b' || w[i-1]=='c' || w[i-1]=='G' || w[i-1]=='h' || w[i-1]=='m' || w[i-1]=='H'))
    {
      w[i]='O';
    }
    if(w[i]=='o' && (w[i-1]=='0' || w[i-1]=='d' || w[i-1]=='j' || w[i-1]=='p' || w[i-1]=='t' || w[i-1]=='v' || w[i-1]=='w'))
    {
      w[i]=(-10);
    }
    if(w[i]=='o' && (w[i-1]=='s' || w[i-1]=='n'))
    {
      w[i]='+';
    }

    //Nach db:
    if(w[i]=='o' && (w[i-1]=='1' || w[i-1]=='2' || w[i-1]=='3' || w[i-1]=='4' || w[i-1]=='5' || w[i-1]=='6'))
    {
      if(w[i-1]=='b' || w[i-2]=='c' || w[i-2]=='G' || w[i-2]=='h' || w[i-2]=='m' || w[i-2]=='H')
      {
        w[i]='O';
      }
      if(w[i-2]=='0' || w[i-2]=='d' || w[i-2]=='j' || w[i-2]=='p' || w[i-2]=='t' || w[i-2]=='v' || w[i-2]=='w')
      {
        w[i]=(-10);
      }
      if(w[i]=='o' && w[i-2]=='s')
      {
        w[i]='+';
      }
    }

    //Nach wh:
    if(w[i]=='o' && (w[i-1]=='7' || w[i-1]=='8' || w[i-1]=='9' || w[i-1]=='Q' || w[i-1]=='Z'))
    {
      if(w[i-1]=='b' || w[i-2]=='c' || w[i-2]=='G' || w[i-2]=='h' || w[i-2]=='m' || w[i-2]=='H')
      {
        w[i]='O';
      }
      if(w[i-2]=='0' || w[i-2]=='d' || w[i-2]=='j' || w[i-2]=='p' || w[i-2]=='t' || w[i-2]=='v' || w[i-2]=='w')
      {
        w[i]=(-10);
      }
      if(w[i]=='o' && w[i-2]=='s')
      {
        w[i]='+';
      }
    }
  }
}
void design_vu(char *w) // Variante des Vokals "u"
{
  int size=strlen(w);

  for (size_t i = 0; i < size; i++) { // Stringwert, der überprüft wird
    if(w[i]=='u' && (w[i-1]=='b' || w[i-1]=='c' || w[i-1]=='G' || w[i-1]=='h' || w[i-1]=='k' || w[i-1]=='m' || w[i-1]=='H'))
    {
      w[i]='U';
    }
    if(w[i]=='u' && (w[i-1]=='0' || w[i-1]=='d' || w[i-1]=='j' || w[i-1]=='p' || w[i-1]=='t' || w[i-1]=='v' || w[i-1]=='w'))
    {
      w[i]=(-4);
    }

    //Nach db:
    if(w[i]=='u' && (w[i-1]=='1' || w[i-1]=='2' || w[i-1]=='3' || w[i-1]=='4' || w[i-1]=='5' || w[i-1]=='6'))
    {
      if(w[i-1]=='b' || w[i-2]=='c' || w[i-2]=='G' || w[i-2]=='h' || w[i-2]=='k' || w[i-2]=='m' || w[i-2]=='H')
      {
        w[i]='U';
      }
      if(w[i-2]=='0' || w[i-2]=='d' || w[i-2]=='j' || w[i-2]=='p' || w[i-2]=='t' || w[i-2]=='v' || w[i-2]=='w')
      {
        w[i]=(-4);
      }
    }

    //Nach wh:
    if(w[i]=='u' && (w[i-1]=='7' || w[i-1]=='8' || w[i-1]=='9' || w[i-1]=='Q' || w[i-1]=='Z'))
    {
      if(w[i-1]=='b' || w[i-2]=='c' || w[i-2]=='G' || w[i-2]=='h' || w[i-2]=='k' || w[i-2]=='m' || w[i-2]=='H')
      {
        w[i]='U';
      }
      if(w[i-2]=='0' || w[i-2]=='d' || w[i-2]=='j' || w[i-2]=='p' || w[i-2]=='t' || w[i-2]=='v' || w[i-2]=='w')
      {
        w[i]=(-4);
      }
    }
  }
}
void design_space_r(char *w) // Breiterer Leerschlag, falls das r an erster Stelle eines Worts kommt, da das r etwas weiter nach links ausschlägt
{
  int size=strlen(w);

  for (size_t i = 1; i < size; i++) {
    if(w[i]=='r' && (w[i-1]==' '))
    {
      w[i-1]='*';
    }
  }
}
void design_und(char *w) // und-Zeichen-Variante
{
  int size=strlen(w);

  for (size_t i = 0; i < size; i++) {
    if(w[i]=='&' && ((w[i+1]=='N' || w[i+1]=='B' || w[i+1]=='g')))
    {
      w[i]='#';
    }
    if(w[i]=='&' && ((w[i+1]=='l' || w[i+1]=='r') && (w[i+2]=='a' || w[i+2]=='e' || w[i+2]=='i' || w[i+2]=='o' || w[i+2]=='u')))
    {
      w[i]='#';
    }
    if(w[i]=='&' && (w[i+1]=='1' || w[i+1]=='2' || w[i+1]=='3' || w[i+1]=='4' || w[i+1]=='5' || w[i+1]=='6' || w[i+1]=='7' || w[i+1]=='8' || w[i+1]=='9' || w[i+1]=='Q' || w[i+1]=='Z') && (w[i+2]=='N' || w[i+2]=='B' || w[i+2]=='g'))
    {
      w[i]='#';
    }
    if(w[i]=='&' && (w[i+1]=='1' || w[i+1]=='2' || w[i+1]=='3' || w[i+1]=='4' || w[i+1]=='5' || w[i+1]=='6' || w[i+1]=='7' || w[i+1]=='8' || w[i+1]=='9' || w[i+1]=='Q' || w[i+1]=='Z') && ((w[i+2]=='l' || w[i+2]=='r') && (w[i+3]=='a' || w[i+3]=='e' || w[i+3]=='i' || w[i+3]=='o' || w[i+3]=='u')))
    {
      w[i]='#';
    }
  }
}

void translation(char *w) // Ablauf der Übersetzungsfunktionen
{
  cap_to_small(w);
  specialsigns(w);
  uml_r(w);
  und_and_to_sign(w);
  sch_to_C(w);
  ch_to_0(w);
  if(wh_first) // je nach Einstellung im Header
  {
    wh(w);
    db(w);
  }
  else
  {
    db(w);
    wh(w);
  }
  vowel_first_letter(w);
  dv(w);

  design_g_G(w);
  design_sch(w);
  design_db(w);
  design_wh(w);
  design_uml(w);
  design_va(w);
  design_ve(w);
  design_vi(w);
  design_vo(w);
  design_vu(w);
  design_space_r(w);
  design_und(w);
}

int main(void)
{
  system("CLS"); // Kommandozeile leeren, falls von dort aus gestartet
  FILE *ptr_file_r; // Pointer zur input-Datei
  char string_read[MAXCHAR]; // der eingelesene String (Zeile, da Zeilenweise eingelesen und übersetzt wird)
  char *pointer_line=string_read; // Pointer zum eingelsesnen String
  char string_total[MAXCHAR]; // Hier werden die übersetzten Zeilen zusammengefügt
  char *pointer_total=string_total; // Zeiger zum zusammengefügten String
  char *filename_r="input.txt"; // Pointer zum Dateiname
  long int size_r=0; // Anzahl Zeichen in eingelesener Zeile und später Anzahl der Zeile nach Übersetzung
  int total_size_r=0; // Insgesamte Anzahl der eingelesenen Zeichen
  int total_size_w=0; // Insgesamte Anzahl der übersetzten (und letztendlich geschriebenen) Zeichen
  int maxchar_reached=0; // Überprüfungsvariable, damit später die clock-Funktion allenfalls übersprungen werden kann

  clock_t begin=clock(); // timer
  clock_t end;

  if(german_and) {
    printf("EASY TENGWAR TRANSLATOR | Version 1.0 | German\n\n");
  }
  else if(english_and) {
    printf("EASY TENGWAR TRANSLATOR | Version 1.0 | English\n\n");
  }
  printf("Translating...\n\n");

  ptr_file_r=fopen(filename_r,"r"); // input.txt öffnen und auf entsprechenden Pointer setzen
  if(ptr_file_r==NULL) // Falls etwas schief läuft, z.B. kein input.txt file existiert
  {
    printf("Could not open/find %s. Please name your inputfile \"input.txt\".\n\n",filename_r);
    system("PAUSE");
    return 1;
  }
  else
  {
    int line_number=1; // Zähler für Zeilennummer
    while(fgets(pointer_line,MAXCHAR,ptr_file_r) != NULL) // Zeilenweises Auslesen des files
    {
      size_r=strlen(pointer_line); //Grösse der Zeile

      int max_linesize=10000; //Max 42168 möglich, nach übersetzung (keine Ahnung warum genau diese Zahl)

      if(size_r>max_linesize) // Falls Zeile zu lang ist...
      {
        end=clock();
        maxchar_reached=1; // Überprüfungsvariable, damit später die clock-Funktion nicht neu ausgelesen wird
        printf("WARNING:\n%d character(s) too much (before translation) in line %d.\nMaximum amount of characters allowed for one line: 10'000 (before translation)\n\n\n",size_r-max_linesize,line_number);
        system("PAUSE");
        system("CLS");
        printf("Translating...\n\n");
        printf("WARNING:\n%d character(s) too much (before translation) in line %d.\nMaximum amount of characters allowed for one line: 10'000 (before translation)\n\n\n",size_r-max_linesize,line_number);
        printf("Translated till line %d:\n\n",line_number-1);
        break;
      }
      if(size_r<2) // Da einige Funktionen nicht funktionieren bei nur einem Zeichen, macht es nur die Funktionen, die nötig sind
      {
        cap_to_small(pointer_line);
        specialsigns(pointer_line);
        uml_r(pointer_line);
        vowel_first_letter(pointer_line);
      }
      else //Falls nicht nur ein Zeichen in der Zeile ist, wird übersetzt...
      {
        translation(pointer_line);
      }

      total_size_r+=size_r; // Zeichenanzahl der eingelesenen (unübersetzten) Zeichen zur Gesamtanzahl der eingelesenen Zeichen addieren
      int maxchar_read=(MAXCHAR-total_size_w); // Anzahl an Zeichen, die nun noch eingelesen werden dürfen
      size_r=strlen(pointer_line); // Anzahl Zeichen der übersetzten Zeile
      if(size_r>maxchar_read) // Falls die übersetzte Zeile grösser als die noch erlaubte Anzahl ist
      {
        end=clock();
        maxchar_reached=1; // Überprüfungsvariable, damit später die clock-Funktion nicht neu ausgelesen wird
        int overflow=size_r-maxchar_read; // Anuahl der Zeichen, die zu viel sind
        printf("WARNING:\nMaximum amount of total characters reached on line %d:\n%d character(s) too much (after translation) in line %d.\n\nMaximum amount of characters allowed for this line: %d (after translation)\n\n\n",line_number,overflow,line_number,maxchar_read);
        system("PAUSE");
        system("CLS");
        printf("Translating...\n\n");
        printf("WARNING:\nMaximum amount of total characters reached on line %d:\n%d character(s) too much (after translation) in line %d.\n\nMaximum amount of characters allowed for this line: %d (after translation)\n\n\n",line_number,overflow,line_number,maxchar_read);
        printf("Translated till line %d:\n\n",line_number-1);
        break;
      }

      strcat(pointer_total,pointer_line); // Übersetzte Zeile zum String aller Zeilen hinzufügen
      total_size_w=strlen(pointer_total); // Anzahl der bis jetzt übersetzten Zeichen
      line_number++; // Zähler hochzählen, da nun die nächste Zeile eingelesen wird
    }
    printf("Translating %d character(s) from %s was successful.\n",total_size_r,filename_r);
  }
  fclose(ptr_file_r); // input.txt schliessen

  total_size_w=strlen(pointer_total); // insgesamte Anzahl der schlussendlich übersetzten Zeichen

  FILE *ptr_file_w; // Pointer zur output-Datei
  char *filename_w="output.txt"; // Pointer zum Dateiname
  ptr_file_w=fopen(filename_w,"w"); // output.txt öffnen/erstellen und auf entsprechenden Pointer setzen

  if(ptr_file_w==NULL) // Falls etwas schief ging
  {
    printf("Could not write \"%s\"\n\n",filename_w);
    system("PAUSE");
    return 1;
  }
  else
  {
    fprintf(ptr_file_w,"%s",pointer_total); // String mit allen übersetzten Zeilen in output.txt schreiben
    printf("Writing %d character(s) to %s was successful.\n\n",total_size_w,filename_w);
  }
  fclose(ptr_file_w); // output.txt schliessen

  if (!maxchar_reached){
    end=clock();
  }

  printf("Length of old string: %d\n",total_size_r);
  printf("Length of new string: %d\n",total_size_w);
  printf("Amount of new character(s): %d\n\n",total_size_w-total_size_r);

  double time_spent=(double)(end - begin) / CLOCKS_PER_SEC; // Zeitmessung in Sekunden umwandeln
	printf("Translated in %.3f seconds.\n\n\n",time_spent);

  system("PAUSE");
  return 0;
}
