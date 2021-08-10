/*
    Author: PearlZeal
    Hardware: ESP8266 Wi-Fi MCU.
    Description:  Arduino coding part.
    Libraries used: SPI, ESP8266WiFi, ESP8266HTTPClient, MFRC522
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
 
//define pins used
#define SS_PIN D8
#define RST_PIN D3

//define each RFID tag with a name for convinience (can add more usesrs also)
#define userA "20 4A 4A 2F" 
#define userB "39 8E A4 B2"
#define userC "09 A6 AB B2"
#define userD "19 B5 B2 B2"

boolean door_status=0;  //door status is either 1 or 0 (login/logout)
#define login   1
#define logout  0

char cmd, data_in;  //characters for input from serial monitor
boolean locker_state  = logout; //locker_state is previous state of the door
#define open_pressed    2
#define close_pressed   3
#define exit_pressed    4

int state=0;
String state_value;
String userID = " ";


/*if login --> read cmd
      if O--> open 
      if C-->close 
      if E-->logout
 * if logout --> logout 
 rfid with no access --> not authorised message
 */

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
 

const char* ssid = "********"; //replace ******** with the name of the WIFI you like to connect
const char* password = "******"; //replace ****** with the password of that WIFI source

String api_url = ""; //URL link to the api 

void setup() 
{
//uncomment the below two lines and replace with pin numbers used for motor which controls the gate/door 
  //pinMode(5,OUTPUT);  
  //pinMode(6,OUTPUT);
  
  Serial.begin(9600);
  //trying to connect to the wifi source
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
      Serial.print(".");
      delay(200);
  }  
  Serial.println("Wifi Connected");

  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
} //end of setup

/*******************************************************************************************************************************/
void loop()
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  // here the UID of the card/cards that you want to give access are compared with the scanned card
  if (content.substring(1) == userA || content.substring(1) == userB || content.substring(1) == userC || content.substring(1) == userD) //if they match... go into this loop
  {
    Serial.print("Authorized access to: ");
    if(content.substring(1) == userA)
    {
      Serial.println("A");
      userID = userA;
    }
    else if(content.substring(1) == userB)
    {
      Serial.println("B");
      userID = userB;
    }
    else if(content.substring(1) == userC)
    {
      Serial.println("C");
      userID = userC;

    }
    else if(content.substring(1) == userD)
    {
      Serial.println("D");
      userID = userD;
    }
  locker_state = login;
    Serial.println();
  }  
 else  //if the user is not authorized... deny 'em the access
 {
    Serial.println(" Warning!! Access denied");
    locker_state = logout;
      Serial.println("Approximate your card to the reader..."); // wait for new card to be scanned!
      Serial.println();

    delay(250);
  }

  while(locker_state) //if the card is authorized... then ask for input!
  {
      //Serial.println("Enter input");
      input_data();
      cmd = ' ';
      door();
      delay(250);
  }
} //end of loop
/***********************************************************************************************************************************************/
//function to upload data using HTTP Client
void upload_data()
{
     HTTPClient http; //Declare an object of class HTTPClient
     api_url.replace("\n", "");
     api_url.replace("\r", "");
     api_url.replace(" ", "%20");
     http.begin(api_url);
    // Serial.println(api_url);
     int httpCode = http.GET(); //Send the request
     
     if (httpCode > 0)
     {
          String server_response = http.getString(); //Get the request response payload

          //if(server_response.indexOf("") >= 0)
          {
              Serial.println(server_response);
          }
         
          delay(100);
     }
     http.end(); //Close connection
} //end of upload_data

/*****************************************************************************************************************************************************/
//creats an api url to update the status of door into database
void create_api_url()
{
  if(state==2)
  {
    state_value="Opening_Door";
  }
  else if(state==3)
  {
    state_value="Closing_Door";
  }

  api_url = "http://thantrajna.com/2021_march/newfolder/attendance_api.php?user=";
  api_url += userID;
  api_url += "&status=";
  api_url += state_value;
  api_url.replace(" ","%20");

  upload_data();

  Serial.println(api_url);
} //end of create_api_url

/*****************************************************************************************************************************************************/
//takes data from serial monitor
void input_data()
{
  state=0;
  //Serial.println("Enter input for door O->OPEN, C->CLOSE, E->EXIT");
    if(Serial.available())
    {
     
        data_in = Serial.read();

        if(data_in >= 'A' && data_in <= 'Z')
        {
          cmd = data_in;
        }
         
        Serial.println(cmd);
        //Serial.println(state);
    if(cmd=='O') //open door
    {
      state=open_pressed;
     
    }
    else if(cmd=='C')  //close door
    {
      state=close_pressed;
    }
    else if(cmd=='E')  //logout
    {
      state=exit_pressed;
    }
   
    }

    delay(250);
  } //end of input_data
  /*****************************************************************************************************************************************************/

//based on the previous state of the door you can either open or close the door
void door()
{
  if(state==open_pressed && door_status==0)
  {
   
    Serial.println("\ndoor opening");
    create_api_url();
    open_door();
    door_status=1;
   
  }
  if(state==close_pressed && door_status==1)
  {
    Serial.println("\ndoor closed");
    create_api_url();
    close_door();
    door_status=0;
   
  }
  if(state==exit_pressed)
  {
    Serial.println("status=logged_out");
    locker_state = logout;
      Serial.println("Approximate your card to the reader...");
      Serial.println();

  }

} //end of door
/*****************************************************************************************************************************************************/

//for the motors... customise based on the pins connected

void open_door()
{
  digitalWrite(5,1);
  digitalWrite(6,0);
  delay(400);
  digitalWrite(5,0);
  digitalWrite(6,0);
}

void close_door()
{
  digitalWrite(5,0);
  digitalWrite(6,1);
  delay(400);
  digitalWrite(5,0);
  digitalWrite(6,0);
}
