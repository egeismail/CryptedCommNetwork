#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <VirtualWire.h>
#define ITEMCOUNT 5
#define RECV_PIN 12
#define TRANS_PIN 11
#define SIG_LEN 4
#define BUFFER 129
#define SECBUFFER 128
#define MENUDELAY 1200
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
const String TOBegin[5][2] = {{"Arayuz","Baslatiliyor"},{"Kriptolu","Iletisim Agi"},{"Yazilim & Devre","Ege I. KOSEDAG"},{"Yetkili Ogrt.","Ayse COBANOGLU"}};
const String Words[2] = {"Driver","Modu"};
int CenterSpace(String string){
  return 16/2-string.length()/2;
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
  }
  }
void waitClear(){
  delay(MENUDELAY);
  lcd.clear();
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
    waitClear();
    CenteredWrite("CID",0);
    CenteredWrite("Olusturuluyor",1);
    waitClear();
    //Create ID
    byte CID[SIG_LEN];
    String CIDS;
    for(int i = 0;i < SIG_LEN;i++){
      CID[i] = random(0,255);
      CIDS = CIDS + String(CID[i],HEX);
      }
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
    waitClear();
    CenteredWrite("CID",0);
    CenteredWrite(CIDS,1);
    }
    waitClear();
  }
bool GHLM = false;
void setup()
{
  randomSeed((analogRead(0)*analogRead(1)*analogRead(2)*analogRead(3)));
  lcd.begin(16,2);
  SetupID();
  SetupKey();
  lcd.clear();
  /*Begin of ui*/
  vw_set_tx_pin(TRANS_PIN);
  vw_set_rx_pin(RECV_PIN);
  vw_setup(2000);
  vw_wait_rx_max(100);
  vw_rx_start();
  CenteredWrite(TOBegin[0][0],0);
  CenteredWrite(TOBegin[0][1],1);
  for(int i = 0;i < 5;i++){ 
    lcd.clear();
    CenteredWrite(TOBegin[i][0],0);
    CenteredWrite(TOBegin[i][1],1);
    //delay(1200);
  }
  waitClear();
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
        for(int i = 1;i < 4;i++){
          waitClear();
          CenteredWrite(TOBegin[i][0],0);
          CenteredWrite(TOBegin[i][1],1);
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
        CenteredWrite(Items[1],0);
        CenteredWrite("Bilgisayar Bekleniyor",1);
        waitClear();
        CenteredWrite(Items[1],0);
        CenteredWrite("ID:"+CIDS,1);
        DriverMode = true;
        Serial.begin(115200);
        return;
        }
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
    char data[BUFFER];
    uint8_t rdata[BUFFER];
    if (Serial.available() > 0) {
       lcd.setCursor(0,1);
       lcd.print("*>");
       Serial.readBytes(data,BUFFER);
       if(!GHLM){
            String datas = String(data);
            if(datas=="SEPYGCID"){
              Serial.print("OKGCID");
              Serial.println(CIDS);
              memset(data, 0, sizeof(data));
              Serial.readBytes(data ,BUFFER);       
              datas = String(data);
              if(datas == "SEPYKEY"){
                delay(100);
                for(int i=4;i < 68;i++){
                  Serial.write(EEPROM.read(i));
                  }
                Serial.println("SEPYPASS");
                memset(data, 0, sizeof(data));
                Serial.readBytes(data ,BUFFER);
                datas = String(data);
                if(datas=="COMPLETED"){
                  lcd.setCursor(0,0);
                  lcd.print("*");
                  GHLM = !GHLM;
                  }else{
                    lcd.setCursor(0,0);
                    lcd.print("!");
                  }
                }
              }
        }else{
          if(data[SECBUFFER]=>0xA0 && data[SECBUFFER]<0xA9){
            vw_send((uint8_t *)data, BUFFER);
            vw_wait_tx();
            Serial.write(0x3);
          }else if(data[SECBUFFER]=>0xB0 && data[SECBUFFER]<0xB9){
            if(vw_get_message(rdata, BUFFER)){
                Serial.write(rdata,BUFFER);
              }else{
                for(int i;i<BUFFER-1;i++)
                  Serial.write(0x0);
                Serial.write(0x2);
                }
            }
          }
       memset(data, 0, sizeof(data));
       memset(rdata, 0, sizeof(rdata));
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
