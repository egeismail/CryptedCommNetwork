#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <EEPROM.h>
#define ITEMCOUNT 5
#define RECV_PIN 12
#define SIG_LEN 4
char hexaKeys[4][3] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
 };
byte rowPins[4] = {8, 7, 6, 5}; 
byte colPins[3] = {4, 3, 2}; 


LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, 4, 3);
const int LCDSize[2] = {16,2};
const String TOBegin[5][2] = {{"Arayuz","Baslatiliyor"},{"Kriptolu","Iletisim Agi"},{"Yazilim & Devre","Ege I. KOSEDAG"},{"Yetkili Ogrt.","Ayse COBANOGLU"},{"Kriptolu","Iletisim Agi"}};

int CenterSpace(String string){
  return LCDSize[0]/2-string.length()/2;
  }
void CenteredWrite(String WRS,int row){
  lcd.setCursor(CenterSpace(WRS),row);
  lcd.print(WRS);
  }
int SelectedIndex = 0;
String Items[ITEMCOUNT] = {"Cihazlari Ara","Driver Modu","Anahtar Esle","Proje Hakk.","Uyku Modu"};
int EEPROM_SIZE = EEPROM.length();
byte GSIG[4] = {243,25,210,185};
byte SSIG[4] = {129,82,222,189};
byte CID[4];
byte SepyKey[64];
String CIDS;
void EEPROMClear(){
  for(int i=0;i < EEPROM_SIZE;i++){
    EEPROM.write(i,0);
    }
  }
void e2reader(){
  char buffer[16];
  char valuePrint[4];
  byte value;
  unsigned int address;
  uint8_t trailingSpace = 2;
 
  Serial.print("Dumping "); Serial.print(E2END + 1);
  Serial.println(" bytes from EEPROM.");
  Serial.print("baseAddr ");
  for(int x = 0; x < 2; x++){
    Serial.print(" ");
    for(int y = 0; y < 25; y++)
      Serial.print("=");
  }
 
  // E2END is a macro defined as the last EEPROM address
  // (1023 for ATMEGA328P)
  for(address = 0; address <= E2END; address++){
    // read a byte from the current address of the EEPROM
    value = EEPROM.read(address);
 
    // add space between two sets of 8 bytes
    if(address % 8 == 0)
      Serial.print("  ");
 
    // newline and address for every 16 bytes
    if(address % 16 == 0){
      //print the buffer
      if(address > 0 && address % 16 == 0)
        printASCII(buffer);
 
      sprintf(buffer, "\n 0x%05X: ", address);
      Serial.print(buffer);
 
      //clear the buffer for the next data block
      memset (buffer, 32, 16);
    }
 
    // save the value in temporary storage
    buffer[address%16] = value;
 
    // print the formatted value
    sprintf(valuePrint, " %02X", value);
    Serial.print(valuePrint);
  }
 
  if(address % 16 > 0){
    if(address % 16 < 9)
      trailingSpace += 2;
 
    trailingSpace += (16 - address % 16) * 3;
  }
 
  for(int i = trailingSpace; i > 0; i--)
    Serial.print(" ");
 
  //last line of data and a new line
  printASCII(buffer);
  Serial.println();
}
 
void printASCII(char * buffer){
  for(int i = 0; i < 16; i++){
    if(i == 8)
      Serial.print(" ");
 
    if(buffer[i] > 31 and buffer[i] < 127){
      Serial.print(buffer[i]);
    }else{
      Serial.print(".");
    }
  }
}
void SetupKey(){
  //Check ID
  lcd.clear();
  CenteredWrite("SKEY Kontrol",0);
  CenteredWrite("Ediliyor",1);
  byte signature[4];
  for(int i = 0;i < SIG_LEN;i++){
    signature[i] = EEPROM.read(i);
  }
  bool SignatureCheck = true;
  for(int i = 0;i < 4;i++){
    if(signature[i]!=SSIG[i]){
      SignatureCheck=false;
      break;
      }
    }
  if(!SignatureCheck){
    delay(1200);
    lcd.clear();
    CenteredWrite("SKEY",0);
    CenteredWrite("Olusturuluyor",1);
    //Create ID
    byte sepy[64];
    for(int i = 0;i < 64;i++){
      sepy[i] = random(0,255);
    }
    delay(1200);
    lcd.clear();
    CenteredWrite("Olusturuldu",0);
    for(int i = 0;i < SIG_LEN;i++){
      EEPROM.write(i,SSIG[i]);
    }
    for(int i = 4;i < 68;i++){
      EEPROM.write(i,sepy[i+4]);
    }
    for(int i = 0;i < 64;i++){
      SepyKey[i] = sepy[i];
      }
  }else{
    /*delay(500);
    lcd.clear();
    for(int i = 4;i < 68;i++){
      SepyKey[i-4]=EEPROM.read(i);
    }
    CenteredWrite("SKey Onbellege",0);
    CenteredWrite("Alindi",1);
    }
    delay(1800);
    lcd.clear();*/
    }
  }
void SetupID(){
  //Check ID
  lcd.clear();
  CenteredWrite("CID Kontrol",0);
  CenteredWrite("Ediliyor",1);
  byte signature[4];
  for(int i = EEPROM_SIZE-(SIG_LEN+4);i < EEPROM_SIZE-SIG_LEN;i++){
    signature[i-(EEPROM_SIZE-(SIG_LEN+4))] = EEPROM.read(i);
  }
  bool SignatureCheck = true;
  for(int i = 0;i < 4;i++){
    if(signature[i]!=GSIG[i]){
      SignatureCheck=false;
      break;
      }
    }
  if(!SignatureCheck){
    delay(1200);
    lcd.clear();
    CenteredWrite("CID",0);
    CenteredWrite("Olusturuluyor",1);
    delay(1200);
    lcd.clear();
    //Create ID
    byte CID[SIG_LEN];
    String CIDS;
    for(int i = 0;i < SIG_LEN;i++){
      CID[i] = random(0,255);
      CIDS = CIDS + String(CID[i],HEX);
      }
    //String CID = String(random(268435456,4294967295),HEX);
    CenteredWrite("Yeni Imza",0);
    CenteredWrite("CID:"+CIDS,1);
    for(int i = EEPROM_SIZE-(SIG_LEN+4);i < EEPROM_SIZE-SIG_LEN;i++){
      EEPROM.write(i,GSIG[i-(EEPROM_SIZE-(SIG_LEN+4))]);
    }
    for(int i = EEPROM_SIZE-(SIG_LEN);i < EEPROM_SIZE;i++){
      EEPROM.write(i,CID[i-(EEPROM_SIZE-(SIG_LEN))]);
    }
  }else{
    for(int i = EEPROM_SIZE-(SIG_LEN);i < EEPROM_SIZE;i++){
      CID[i-(EEPROM_SIZE-(SIG_LEN))]=EEPROM.read(i);
      //Serial.println(String(CID[i-(EEPROM_SIZE-(SIG_LEN))],HEX));
      CIDS = CIDS+String(CID[i-(EEPROM_SIZE-(SIG_LEN))],HEX);
    }
    delay(500);
    lcd.clear();
    CenteredWrite("CID",0);
    CenteredWrite(CIDS,1);
    }
    delay(1800);
    lcd.clear();
  }
bool GHLM = false;
void setup()
{
  randomSeed((analogRead(0)*analogRead(1)*analogRead(2)*analogRead(3)));
  lcd.begin(LCDSize[0],LCDSize[1]);
  SetupID();
  SetupKey();
  Serial.begin(57600);
  e2reader();
  Serial.end();
  /*Begin of ui*/
  /*CenteredWrite(TOBegin[0][0],0);
  CenteredWrite(TOBegin[0][1],1);
  for(int i = 0;i < 5;i++){ 
    lcd.clear();
    CenteredWrite(TOBegin[i][0],0);
    CenteredWrite(TOBegin[i][1],1);
    //delay(1200);
  }
  delay(2000);*/
  lcd.clear();
  //Begin UI
  lcd.setCursor(0,0);
  lcd.print(">");
  CenteredWrite(Items[SelectedIndex],0);
    if(SelectedIndex < ITEMCOUNT){
      CenteredWrite(Items[SelectedIndex+1],1);
    }
}
void RefreshMenu(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(">");
  CenteredWrite(Items[SelectedIndex],0);
  if(SelectedIndex < ITEMCOUNT-1){
    CenteredWrite(Items[SelectedIndex+1],1);
  }else{
    CenteredWrite(Items[0],1);
  }
}
String ByteToString(byte *data){
  String string;
  for(int i=0;i < sizeof(data);i++){
    string = string + (String)char(i);
    }
  return string;
  }
bool TurnState = true;
bool DriverMode = false;
void loop()
{
  if(!DriverMode){
  char customKey = customKeypad.getKey();
  if (customKey){
    if(TurnState){
    if(customKey == '5'){
      if(SelectedIndex == 3){
        lcd.clear();
        for(int i = 1;i < 5;i++){
          lcd.clear();
          CenteredWrite(TOBegin[i][0],0);
          CenteredWrite(TOBegin[i][1],1);
          delay(1500);
        }
        lcd.clear();
        RefreshMenu();
      }else if(SelectedIndex == 4){
        lcd.clear();
        lcd.noBacklight();
        lcd.noDisplay();
        TurnState = false;
      }else if(SelectedIndex == 1){
        lcd.clear();
        CenteredWrite("Driver Modu",0);
        CenteredWrite("Bilgisayar Bekleniyor",1);
        delay(800);
        lcd.clear();
        CenteredWrite("Driver Modu",0);
        CenteredWrite("ID:"+CIDS,1);
        DriverMode = true;
        Serial.begin(115200);
        return;
        }
    }
    if(customKey == '#'){
      lcd.clear();
      CenteredWrite("SIndex : "+String(SelectedIndex),0);
      delay(2000);
      RefreshMenu();
      return;
     }
    if(customKey == '2' && SelectedIndex > 0){
       SelectedIndex--;
    }else if(customKey == '8' && SelectedIndex < ITEMCOUNT-1){
       SelectedIndex++;
    }else if(SelectedIndex == ITEMCOUNT-1){
        SelectedIndex = 0;
    }else if(SelectedIndex == 0){
      SelectedIndex = ITEMCOUNT-1;
     }
    RefreshMenu();
    }else{
      if(customKey == '*'){
        TurnState=true;
        lcd.backlight();
        lcd.display();
        RefreshMenu();
        }
    }
  }
  }else{
    char data[64];
    if (Serial.available() > 0) {
       lcd.setCursor(0,1);
       lcd.print("*>");
       Serial.readBytes(data,64);
       if(!GHLM){
            String datas = String(data);
            if(datas=="SEPYGCID"){
              Serial.print("OKGCID");
              Serial.println(CIDS);
              memset(data, 0, sizeof(data));
              Serial.readBytes(data ,64);       
              datas = String(data);
              if(datas == "SEPYKEY"){
                delay(100);
                for(int i=0;i < 64;i++){
                  Serial.write(EEPROM.read(i+4));
                  }
                Serial.println("SEPYPASS");
                lcd.setCursor(0,0);
                lcd.print("*");
                //GHLM = !GHLM;
                }
              }
        }else{
          
          }
       memset(data, 0, sizeof(data));
      }else{
        lcd.setCursor(0,1);
        lcd.print("!>");
        }
    char customKey = customKeypad.getKey();
    if(customKey){
      if(customKey == '#'){
        DriverMode = false;
        Serial.end();
        }
      }
    }
}
