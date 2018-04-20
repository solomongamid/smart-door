#include <ESP8266WiFi.h>
#include <Servo.h> 

#define led_pin 13
#define servo_pin 5
#define DEBUG

ADC_MODE(ADC_VCC);

int flag = 0;
const char* ssid     = "";        // Nom du réseau
const char* password = "";        // clé 

WiFiServer server(80);
Servo myservo; 
int pos = 0; 
String readString;
void setup()
{
  #ifdef DEBUG
    Serial.begin(115200);
    Serial.println();
    Serial.printf("Connecting to %s ", ssid);
  #endif
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    #ifdef DEBUG
      Serial.print(".");
    #endif
  }
  #ifdef DEBUG
    Serial.println(" connected");
  #endif
  myservo.attach(servo_pin);
  pinMode(led_pin, OUTPUT); 
  server.begin();
  #ifdef DEBUG
    Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());
  #endif
}


void loop()
{
  WiFiClient client = server.available();
  // wait for a client (web browser) to connect
  if (client)
  {

    #ifdef DEBUG
      Serial.println("\n[Client connected]");
    #endif
    while (client.connected())
    {
       
      // read line by line what the client (web browser) is requesting
      if (client.available())
      {
        char c = client.read();
        
        if (readString.length() < 100) {
          //store characters to string
          readString += c;
         }
          
        // wait for end of client's request, that is marked with an empty line
        if (c == '\n')
        {
          int valeur = analogRead(A0);
          float tension = round(((valeur * (3.3 / 1024.0))/100 + 2.88) * 20)   ;
          #ifdef DEBUG
        //    Serial.println(readString); //print to serial monitor for debuging
          Serial.println(valeur);
          flag++;
          #endif
           client.println("HTTP/1.1 200 OK"); //send new page
           client.println("Content-Type: text/html");
           client.println();     
           client.println("<HTML>");
           client.println("<HEAD>");
           client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
           client.println("<meta http-equiv="content-type" content="text/html; charset=UTF-8" />");
           client.println("<meta http-equiv="content-language" content="fr-FR" />");
           client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
           client.println("<link rel='stylesheet' type='text/css' href='http://randomnerdtutorials.com/ethernetcss.css' />");
           client.println("<TITLE>Smart Door</TITLE>");
           client.println("</HEAD>");
           client.println("<BODY>");
           client.println("<H1>Smart Door</H1>");
           client.println("<hr />");
           client.println("<p>capacité de la batterie");
           client.println( tension);
           client.println( "% </p>");
           client.println("<br />");  
           client.println("<H2>Arduino with Wifi</H2>");
           client.println("<br />");     
           client.println("<br />"); 
           client.println("<a href=\"/?button2on\"\">Open Door</a>");
           client.println("</BODY>");
           client.println("</HTML>");

            delay(1); // give the web browser time to receive the data
        
            // close the connection:
            client.stop();
            
     //controls the Arduino if you press the buttons
           if (readString.indexOf("?button2on") > 0){
               #ifdef DEBUG
                 Serial.println("Servo ON");
               #endif
                for(pos; pos < 360; pos += 3)  // goes from 0 degrees to 180 degrees 
                {                                  // in steps of 1 degree 
                  myservo.write(pos);              // tell servo to go to position in variable 'pos' 
                  digitalWrite(led_pin, HIGH);
                  delay(15);                       // waits 15ms for the servo to reach the position 
                }
                 delay(1360);
               #ifdef DEBUG
                 Serial.println("Servo OFF");
               #endif
                 for(pos; pos > 0; pos-=3) 
                 {                            
                  myservo.write(pos);              // tell servo to go to position in variable 'pos' 
                  digitalWrite(led_pin, LOW);
                  delay(15);                       // waits 15ms for the servo to reach the position 
                }
           }
          //clearing string for next read
            readString="";     
         }
        }
      }
   }
 }
