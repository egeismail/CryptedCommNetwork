#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <VirtualWire.h>
#include <SoftwareSerial.h>
#define ITEMCOUNT 5
#define RECV_PIN 11
#define TRANS_PIN 12
#define SIG_LEN 4
#define BUFFER 64
#define SECBUFFER 64
#define MENUDELAY 1200
#define WORKBAUD 57600
char hexaKeys[4][3] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
 };
byte rowPins[4] = {8, 7, 6, 5}; 
byte colPins[3] = {4, 3, 2}; 
SoftwareSerial KeySerial(0, 1);
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
String Items[ITEMCOUNT] = {"Driver Modu","Anahtar Esle","Proje Hakk.","EEPROM Reset","Uyku Modu"};
int EEPROM_SIZE = EEPROM.length();
byte GSIG[4] = {243,25,210,185};
byte SSIG[4] = {129,82,222,189};
byte CID[4];
byte SepyKey[16];
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
    byte sepy[16] = {33,48,120,59,82,110,98,126,83,45,59,69,125,61,48,86};
    /*for(int i = 0;i < 16;i++){
      sepy[i] = random(32,127);
    }*/
    delay(1200);
    lcd.clear();
    CenteredWrite("Olusturuldu",0);
    for(int i = 0;i < SIG_LEN;i++){
      EEPROM.write(i,SSIG[i]);
    }
    for(int i = 4;i < 20;i++){
      EEPROM.write(i,sepy[i+4]);
    }
    for(int i = 0;i < 16;i++){
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
uint8_t rsdata[64];
bool RecvData(){
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  memset(rsdata,0,64);
  int packetsize=7;
  int writed=0;
  if(vw_wait_rx_max(1000)){
    int b=0;
    while(true){
        for(int t=0;t<64;b++){
          if(vw_wait_rx_max(2000)&&vw_get_message(buf,buflen)){
          
            for(int k=0;k<packetsize;k++){
                if((byte)buf[k] == 0){
                  break;
                  }
                rsdata[writed] = buf[k];
                writed++;
              }
          }else{
            return true;
          }
        }
        b++;
      }
    }else{
      return false;
    }
}
void SendData(byte *data,int sizea){
  int packetsize = 7;
  byte bucket[packetsize];
  for(int i=0;i<((sizea/packetsize))+1;i++){
      for(int j=0;j<packetsize;j++){
        if(sizea==i*packetsize+j){
          break;
          }
        bucket[j] = data[i*packetsize+j];
        }
      vw_send((uint8_t *)bucket, packetsize);
      vw_wait_tx(); 
      if(i*packetsize+strlen(bucket) >= sizea){
        break;
        }
      memset(bucket,0,packetsize);
     }
  }
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
  if(SelectedIndex < ITEMCOUNT){
    CenteredWrite(Items[SelectedIndex+1],1);
  }else{
    CenteredWrite(Items[0],1);
  }
}
bool TurnState = true;
bool DriverMode = false;
bool Mode = false;
void loop()
{
  if(!DriverMode){
  char customKey = customKeypad.getKey();
  if (customKey){
    if(TurnState){
    if(customKey == '5'){
      if(SelectedIndex == 2){
        lcd.clear();
        for(int i = 1;i < 4;i++){
          waitClear();
          CenteredWrite(TOBegin[i][0],0);
          CenteredWrite(TOBegin[i][1],1);
        }
        lcd.clear();
        RefreshMenu();
      }else if(SelectedIndex == 3){
        lcd.clear();
        EEPROMClear();
        CenteredWrite("Cihazi Yeniden",0);
        CenteredWrite("Baslatin",1);
        delay(100000);
      }else if(SelectedIndex == 4){
        lcd.clear();
        lcd.noBacklight();
        lcd.noDisplay();
        TurnState = false;
      }else if(SelectedIndex == 0){
        lcd.clear();
        CenteredWrite("RX",0);
        CenteredWrite("ID:"+CIDS,1);
        DriverMode = true;
        Serial.begin(WORKBAUD);
        return;
        }else if(SelectedIndex == 1){
        lcd.clear();
        CenteredWrite("Anahtar Esleme",0);
        CenteredWrite("Modu",1);
        waitClear();
        KeySerial.begin(4800);
        CenteredWrite("Mod Secin",0);
        CenteredWrite("1>RX 2>TX",1);
        bool mode = true;
        while(true){
          char customKey = customKeypad.getKey();
          if(customKey){
            if(customKey == '1'){
              mode=true;
              break;
            }else if(customKey == '2'){
              mode=false;
              break;
              }
            }
          delay(100);
        }
        waitClear();
        if(mode){
          CenteredWrite("RX",0);
          }else{
          CenteredWrite("TX",0);
            }
          CenteredWrite("Aktif",1);
          
        if(mode){
          for(int i=4;i<20;i++){
            SepyKey[i-4] = EEPROM.read(i);
            }
          
          KeySerial.write(0x2);
          KeySerial.write(SepyKey,16);
          KeySerial.write(0x3);
          }else{
            byte key[16];
            if(0x2 == KeySerial.read()){
              for(int i=0;i<16;i++){
                byte t = KeySerial.read();
                if(t == 0x3){
                  CenteredWrite("Hatali Iletisim",1);
                  break;
                  }else{
                    key[i] = t;
                    }
                }
                byte t = KeySerial.read();
                if(t==0x3){
                  CenteredWrite("Basarili Iletisim",0);
                  CenteredWrite("EEPROMA Yaziliyor",1);
                  for(int i=4;i<20;i++){
                    EEPROM.write(i,key[i-4]);
                    }
                  CenteredWrite("Yazildi.",1);
                  }else{
                    CenteredWrite("Hatali Iletisim",1);
                    }
              }
            
          }
          waitClear();
          KeySerial.end();
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
    char customKey = customKeypad.getKey();
          if(customKey){
            if(customKey == '#'){
              DriverMode = false;
              Serial.end();
            }else if(customKey='*'){
              if(Mode){
                Mode=false;
                CenteredWrite("RX",0);
              }else{
                Mode=true;
                CenteredWrite("TX",0);
               }
              }
          }
 
              
            
    if (Serial.available() > 0) {
       byte data[BUFFER];
       if(!GHLM){
            lcd.setCursor(0,1);
            lcd.print("*>");
            Serial.readBytes(data,BUFFER); 
            String datas = String((char*)data);
            if(datas=="SEPYGCID"){
              Serial.print("OKGCID");
              Serial.println(CIDS);
              memset(data, 0, sizeof(data));
              Serial.readBytes(data ,BUFFER);       
              datas = String((char*)data);
              if(datas == "SEPYKEY"){
                delay(100);
                for(int i=4;i < 20;i++){
                  Serial.write(EEPROM.read(i));
                  }
                Serial.println("SEPYPASS");
                memset(data, 0, sizeof(data));
                Serial.readBytes(data ,BUFFER);
                datas = String((char*)data);
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
          if(Mode){
            lcd.setCursor(15,0);
            lcd.print("*");
            Serial.readBytes(data,BUFFER);
            SendData(data,strlen(data));
            }else{
              if(RecvData()){
                lcd.setCursor(1,0);
                lcd.print("*");
                Serial.write(rsdata,BUFFER);
              }
            }
          /*
          Serial.readBytes(data,BUFFER); 
          vw_send((uint8_t *)data, BUFFER);
          vw_wait_tx();
          if(vw_wait_rx_max(2000)&&vw_get_message(rdata,sizeof(rdata))){
            Serial.write(0x2);
            Serial.write(rdata,sizeof(rdata));
            lcd.setCursor(0,0);
            lcd.print("*"+String((char*)rdata[0]));
          }else{
            Serial.write(0x1);
            lcd.setCursor(0,0);
            lcd.print("!");
          }*/
          /*
         if((char*)data[SECBUFFER]==0x0E){
            lcd.setCursor(14,0);
            lcd.print("^");
            vw_send((uint8_t *)data, BUFFER);
            vw_wait_tx();
            Serial.write(0x3);
            lcd.setCursor(14,0);
            lcd.print("*");
          }else if((char*)data[SECBUFFER]==0x0F){
            lcd.setCursor(15,0);
            lcd.print("!");
            if(vw_wait_rx_max(200)&&vw_get_message(rdata, (uint8_t)BUFFER)){
              lcd.setCursor(15,0);
              lcd.print("^");
              Serial.write(rdata,BUFFER);
            }else{
                for(int i;i< BUFFER-1;i++)
                  Serial.write(0x0);
                }
                Serial.write(0x2);
                lcd.setCursor(15,0);
                lcd.print("*");
            }else{
              lcd.setCursor(1,0);
              lcd.print("^");
              lcd.setCursor(15,1);
              lcd.print((char*)data[SECBUFFER]);
              }
            lcd.setCursor(16,1);
            lcd.print("*");*/
          
       }
       memset(data, 0, sizeof(data));
       memset(rsdata, 0, sizeof(rsdata));
       }else{
        if(!Mode && GHLM){
                if(RecvData()){
                 lcd.setCursor(1,0);
                 lcd.print("*");
                 Serial.write(rsdata,BUFFER);
                }
            }
        lcd.setCursor(0,1);
        lcd.print("!>");
        }
    }
}
