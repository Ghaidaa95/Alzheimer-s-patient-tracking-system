// include the needed library
#include <SoftwareSerial.h>
#include <TinyGPS.h>

TinyGPS GPS;
const int GPSRX=4;
const int GPSTX=5;
const int GSMRX=10;
const int GSMTX=11;
SoftwareSerial GPSLocatin(GPSRX,GPSTX);// open the serial port for GPS
SoftwareSerial Sim800l(GSMRX,GSMTX);// open the serial port for Sim800L
const unsigned long GPSDelay= 1000;
float lat,Lang;
unsigned long age;
int year;
byte month,day,hour,minute,second,hundredths;
String messageType="media";

void setup() {
  // open the serial monitar 
  Serial.begin(115200); 
  GPSLocatin.begin(9600);
  Sim800l.begin(19200);
  Serial.println("ALZHEIMER'S PATIENT TRACKING SYSTEM USING ARDUINO.");
  Serial.println("Team: Ghaidaa Fahad Alorini\nSara Khalid Alghamdi\nFutoon Hesham Gammash\nFutoon Faisal Alosaimi\nAhad Ahmad Habib");
  Serial.println("PROJECT SUPERVISOR: Dr. Mohsin Bilal");
  // 
  Sim800l.println("AT+CMGR=1"); // read SMS messages 
  delay(500);
  Sim800l.println ("AT+CNMI=1,2,0,0,0"); // new message indicator
  delay(500);
  Sim800l.println("AT+CMGF=1"); // select message format to a text mode
  delay(500);
  GetGPS();
  Serial.println("patient is in range");
}

void loop() {
  while(Sim800l.available()){ // check if there is a signal for the Sim800L
    if(Sim800l.find("Track")){ // checks if the message which been received is the predefined message
      // it true then it will call the GetGPS function to get the location then the sendMessage function to send the location to the caretaker.
      Serial.println("received the predefined message");
      GetGPS();
      delay(500); 
      sendMessage();
      delay(500);
      Sim800l.println("AT+CMGD=1"); // delete the message to free the buffer space
      Serial.println("Delete the message");
      delay(500); 
    }
    else {
      Serial.println("received a diffrent message ");
      Sim800l.println("AT+CMGD=1"); // delete the message to free the buffer space
      Serial.println("Delete the message");
    }
  }
  range();
}

void sendMessage(){
    Sim800l.println("AT+CMGF=1"); // select message format to a text mode
    delay(100);
    Sim800l.println("AT+CMGS=\"+966XXXXXXXXX\"");  // send SMS message, it takes the caretaker number to send the message to.
    delay(200);
    Sim800l.println("The patient Current Location is:");
    delay(100);
    // checks if the caretaker want the location either in text format or media (URL) format.
    if(messageType=="Text"){
    Sim800l.print("Latitude:");
    Sim800l.println(lat);
    delay(100);
    Sim800l.println("Longitude:");
    Sim800l.println(Lang);
    delay(100);
    }
    else {
    Sim800l.print("https://www.google.com.sa/maps/search/");
    Sim800l.print(lat);
    Sim800l.print(",");
    Sim800l.print(Lang);
    }
    Sim800l.println("Please take some action soon..\nThankyou");
    Sim800l.print((char)26); // the ASCII code of the ctrl+z is 26 (required according to the datasheet)
    Sim800l.println("");
    delay(100); 
    Serial.println("Text Sent.");
    Sim800l.println("");
    delay(2000);  
}

void rangeAlerat(){
    Sim800l.println("AT+CMGF=1"); // select message format to a text mode
    delay(100);
    Sim800l.println("AT+CMGS=\"+966XXXXXXXXX\"");  // send SMS message, it takes the caretaker number to send the message to.
    delay(200);
    Sim800l.println("The patient is out of range:");
    delay(100);
    // checks if the caretaker want the location either in text format or media (URL) format.
    if(messageType=="Text"){
    Sim800l.print("Latitude:");
    Sim800l.println(lat);
    delay(100);
    Sim800l.println("Longitude:");
    Sim800l.println(Lang);
    delay(100);
    }
    else {
    Sim800l.print("https://www.google.com.sa/maps/search/");
    Sim800l.print(lat);
    Sim800l.print(",");
    Sim800l.print(Lang);
    }
    Sim800l.println("Please take some action soon..\nThankyou");
    Sim800l.print((char)26); // the ASCII code of the ctrl+z is 26 (required according to the datasheet)
    Sim800l.println("");
    delay(100); 
    Serial.println("Text Sent.");
    Sim800l.println("");
    delay(2000);  
}

void range(){
  GetGPS();
  if (lat != 0.00 && Lang != 0.00){
    float home_lat = 00.000000;// put ypur coordinate here
    float home_lon = 00.000000; // put ypur coordinate here
    float range = 0.000200; // the range change
    int newlat , newlon;
    if ((home_lon+range) > Lang || (home_lon-range) > Lang) {
      newlon=1;
    }
    if ( (home_lat+range) > lat || (home_lat-range) < lat) {
      newlat=1;
    }

    if (newlat == 1 && newlon == 1 ){
      Serial.println("patient is out of range");
      rangeAlerat();
    }
    else {
    Serial.println("patient is in range");
    }
  }
}

void GetGPS(){
    bool newGPSData = false;
  for (unsigned long start = millis();millis()-start<GPSDelay;){
    while(GPSLocatin.available()) //checks if any data available in the reciver
    {
      char Location = GPSLocatin.read();// read the available data
      if (GPS.encode(Location)) // check if a new valid sentence came into the reciver
      newGPSData = true; // change the newGPSData into true
    }
  }
  if(newGPSData){ // if true then get the coordinates
    GPS.f_get_position(&lat,&Lang,&age); // Store the value in the latitude and longitude variable 
    GPS.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths); // Store the value of the time into thier variables
    // print the Location data in the serial moniter.
    Serial.println("The patient location is:");
    Serial.println("Latitude \t Longitude\tDtae \t   Time");
    Serial.print(lat,6);
    Serial.print("\t");
    Serial.print(Lang,6);
    Serial.print("\t");
    Serial.print(day);
    Serial.print("/");
    Serial.print(month);
    Serial.print("/");
    Serial.print(year);
    Serial.print("\t");
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(":");
    Serial.print(second);
    Serial.print("\n");
  }
}
     
