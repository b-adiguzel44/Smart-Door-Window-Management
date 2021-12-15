// Smart Door/Window Management
// An IoT project for interaction with doors/windows via NodeMCU ESP8266 using Firebase Real-Time Database
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <Servo.h>
// Definitions of Firebase and WiFi connections
#define FIREBASE_HOST "YOUR_FIREBASE_HOST"
#define FIREBASE_AUTH "YOUR_FIREBASE_AUTH"
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define MIN_DISTANCE 11
#define ANGULAR_DEGREE 90

// Definitions of Pins 
// (D1 = Servo Pin, D3 = Echo, D4 = Trigger)
#define echoPin D3
#define trigPin D4
// Servo S90 motor bağlantısı
#define servoPin D1

// Servo and FirebaseData objects
Servo sg90;
FirebaseData Data;

// Variables
int prev_distance;      // the value that previously received  coming from database is stored in this variable
String canOpen;         // if true(1) indicates interaction with the door/window could occur, if false(0) indicates interaction with the door/window COULDN'T occur
String isOpen;          // if true(1) indicates the door/window is currently open, if false(0) indicates the door/window is currently closed
long duration;
int distance;
int degree = ANGULAR_DEGREE;       // the degree of servo motor you'd like you to begin with


void setup() {
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  // Pin definitions
  pinMode(trigPin, OUTPUT);         // D4 trigger pin
  pinMode(echoPin, INPUT);          // D3 echo pin
  // Servo pin definitions
  sg90.attach(servoPin);
  sg90.write(ANGULAR_DEGREE);
  pinMode(LED_BUILTIN, OUTPUT);     // NodeMCU ESP8266 Built-in LED
  digitalWrite(LED_BUILTIN, LOW);   // Flickering the Built-in LED
 
} 

void loop() {

  // Receiving data from Firebase Real-Time Database
  getData();
  // Measuring the distance in front of the door/window
  MeasureDistance();
  // Deciding whether the door/window can be opened or not according to the incoming and measured data
  RotateMotor();
}

void MeasureDistance() {
  String temp_state;
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
   
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // listening to ECHO pin
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  
  // if Measured distance is less and equal to 11, there is an obstacle in front of the door/window
  if (distance <= MIN_DISTANCE) {
    
    // Lights Built-in LED in your NodeMCU
    digitalWrite(LED_BUILTIN, LOW);
    
    // Set CanOpen to false (INTERACTION WITH THE DOOR/WINDOW IS UNAVAILABLE) (If the current value is different from the database, it saves the value.)
    if(Firebase.getString(Data, "/door_status/CanOpen", temp_state))
    {
      if(canOpen == temp_state)
        Firebase.setString(Data, "/door_status/CanOpen", "0");  
    }
      
    // Save the measured value to the database if the measured value in the sensor is less than or greater than the value registered in the database
    if(distance < prev_distance || distance > prev_distance) {
        // If the process is failed, it will display an error message on Serial port.
        if(!Firebase.setInt(Data, "/front_door/distance", distance))
        {
          delay(500); 
          Serial.println("FAILED");
          Serial.println("REASON: " + Data.errorReason());
          Serial.println("------------------------------------");
          Serial.println();
        } 
    }
  }
  
  // Gets Built-in LED to the previous state(Flickering) (This means that there is no obstacle(s) therefore indicates the door/window can be closed)
  else {
    digitalWrite(LED_BUILTIN, HIGH);

    // Set CanOpen to true (INTERACTION WITH THE DOOR/WINDOW IS AVAILABLE) (If the current value is different from the database, it saves the value.)
    if(Firebase.getString(Data, "/door_status/CanOpen", temp_state))
    {
      if(canOpen == temp_state)
        Firebase.setString(Data, "/door_status/CanOpen", "1");  
    }
    
    // If storing process for the database fails, it will display an error message on Serial port.
    if(!Firebase.setInt(Data, "/front_door/distance", MIN_DISTANCE))
    {
      delay(500); 
      Serial.println("FAILED");
      Serial.println("REASON: " + Data.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }   
  }
  Serial.print("distance : " );
  Serial.print(distance);
  Serial.println(" cm");  
  delay(200);
}

// Turning the SG90 Servo motor on and off according to the status of the door/window
void RotateMotor() {
  String temp_state;
  // If the door/window can be opened/closed
  if(canOpen != "0") {
    Serial.println("The door/window can be opened/closed.");

    // If the door/window is closed
    if(isOpen != "1") {
      // Rotate the Servo motor to 90 degrees
      if(degree != 90) {
        
        int pos;
        for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 90 degrees
                                            // in steps of 1 degree
          sg90.write(pos);                  // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
        
        degree = 90;
      }
      // Set to true(1) if the value in the database is different from than your local variable
      if(Firebase.getString(Data, "/door_status/IsOpen", temp_state))
      {
        if(isOpen != temp_state)
          Firebase.setString(Data, "/door_status/IsOpen", "1");
      }
     
      Serial.println("The door/window is closed");
      delay(200);  
    }
    // If the door/window is open
    else {
      // Rotate the Servo motor to 0 degrees
      if(degree != 0) {
 
        int pos;
        for (pos = 90; pos >= 0; pos -= 1) { // goes from 90 degrees to 0 degrees
          sg90.write(pos);                  // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }

        degree = 0;
      }
      
      // Set to false(0) if the value in the database is different from than your local variable
      if(Firebase.getString(Data, "/door_status/IsOpen", temp_state))
      {
        if(isOpen != temp_state)
          Firebase.setString(Data, "/door_status/IsOpen", "0");
      }
      Serial.println("The door/window is open");
      delay(200);   
   }
  }
  // If the door/window can NOT be opened/closed
  else {
    Serial.println("There is an obstacle in front of the door/window. The door/window cannot be opened/closed");
  }
}

// Pull the data from Firebase Real-Time Database
void getData() {

  // The status of whether the door/window can be interacted or not
  if(!Firebase.getString(Data, "/door_status/CanOpen", canOpen))
  {
      delay(500); 
      Serial.println("FAILED");
      Serial.println("REASON: " + Data.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
  }
  // The current status of the door/window (OPEN or CLOSED)
  if(!Firebase.getString(Data, "/door_status/IsOpen", isOpen))
  {
      delay(500); 
      Serial.println("FAILED");
      Serial.println("REASON: " + Data.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
  }
  // Fetch the previously saved distance data in database
  if(!Firebase.getInt(Data, "/front_door/distance", prev_distance))
  {
      delay(500); 
      Serial.println("FAILED");
      Serial.println("REASON: " + Data.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
  }
}
