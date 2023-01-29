#include <SoftwareSerial.h>            //SoftwareSerial kütüphanimizi ekliyoruz.
#include<Servo.h>
#include<stdlib.h>
#include<String.h>


#define yagmur A0
Servo motor;

int su_miktari ;
int uyari = 0 ;                                         
String agAdi = "Redmi Note 7";                 //Ağımızın adını buraya yazıyoruz.     //
String agSifresi = "derssifresi";           //Ağımızın şifresini buraya yazıyoruz. //DEGISECEK

int rxPin = 10;                                               //ESP8266 RX pini
int txPin = 11;                                               //ESP8266 TX pini


String ip = "184.106.153.149";                                //Thingspeak ip adresi //DEGISECEKK


SoftwareSerial esp(rxPin, txPin);                             //Seri haberleşme pin ayarlarını yapıyoruz.

void setup() {  
  
  pinMode(yagmur,INPUT);
  motor.attach(9);
  Serial.begin(9600);  //Seri port ile haberleşmemizi başlatıyoruz.
  Serial.println("Started");
  esp.begin(115200);                                          //ESP8266 ile seri haberleşmeyi başlatıyoruz.
  esp.println("AT");                                          //AT komutu ile modül kontrolünü yapıyoruz.
  Serial.println("AT Yollandı");
  while(!esp.find("OK")){                                     //Modül hazır olana kadar bekliyoruz.
    esp.println("AT");
    Serial.println("ESP8266 Bulunamadı.");
  }
  Serial.println("OK Komutu Alındı");
  esp.println("AT+CWMODE=1");                                 //ESP8266 modülünü client olarak ayarlıyoruz.
  while(!esp.find("OK")){                                     //Ayar yapılana kadar bekliyoruz.
    esp.println("AT+CWMODE=1");
    Serial.println("Ayar Yapılıyor....");
  }
  Serial.println("Client olarak ayarlandı");
  Serial.println("Aga Baglaniliyor...");
  esp.println("AT+CWJAP=\""+agAdi+"\",\""+agSifresi+"\"");    //Ağımıza bağlanıyoruz.
  while(!esp.find("OK"));                                     //Ağa bağlanana kadar bekliyoruz.
  Serial.println("Aga Baglandi.");
  delay(1000);
}
void loop() {
  esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");           //Thingspeak'e bağlanıyoruz.
  if(esp.find("Error")){                                      //Bağlantı hatası kontrolü yapıyoruz.
    Serial.println("AT+CIPSTART Error");
  }


  su_miktari = analogRead(yagmur);
  delay(200);
  Serial.print("su miktarı : ");
  Serial.println(su_miktari);
 
  if(su_miktari < 550){
   
    delay(500);
    uyari = uyari+1;
   if(uyari = 3){
     
      motor.write(90);
      
    }
  }
  else{
    
    uyari = 0;
    motor.write(0);
  }


  
  String veri = "GET https://api.thingspeak.com/update?api_key=X0Y1J30JG53RMGI9";   //Thingspeak komutu. Key kısmına kendi api keyimizi yazıyoruz. 
  
  veri += "&field1=";

  veri += String(su_miktari);   
  

 
  
 
  esp.print("AT+CIPSEND=");                                   //ESP'ye göndereceğimiz veri uzunluğunu veriyoruz.
  esp.println(veri.length()+2);
  delay(2000);
  if(esp.find(">")){                                          //ESP8266 hazır olduğunda içindeki komutlar çalışıyor.
    esp.print(veri);                                          //Veriyi gönderiyoruz.
    Serial.println(veri);
    Serial.println("Veri gonderildi.");
    delay(1000);
  }
  Serial.println("Baglantı Kapatildi.");
  esp.println("AT+CIPCLOSE");                                //Bağlantıyı kapatıyoruz
  delay(1000);                                               //Yeni veri gönderimi için 1 dakika bekliyoruz.
}
