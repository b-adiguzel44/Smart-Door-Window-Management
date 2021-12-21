# Smart-Door-Window-Management
An IoT project for interaction with doors/windows via NodeMCU ESP8266 using Firebase Real-Time Database

Difficulty : Beginner

Prerequisites :
- Basic knowledge of Programming and Algorithm

What Hardwares I've used are shown below :
- NodeMCU ESP8266 V3 
- HC SR04 Ultrasonic Distance Sensor
- Tower Pro SG90 RC Mini 9gr Servo Motor


To monitor and control NodeMCU ESP8266 I've used Kodular, a software that provides several "modules" (services) to create Android apps without coding, Here below there are screenshots of the status of our system. 

## Example Screenshots

![Screenshot_20211215_165753_io makeroid companion](https://user-images.githubusercontent.com/56561641/146356790-e88375df-2037-4fad-a977-224723e87f02.jpg)
- The "Closed Door" status along with a button to toggle (Opening the door) the status

![Screenshot_20211215_165800_io makeroid companion](https://user-images.githubusercontent.com/56561641/146357270-2929bc14-8fc6-43b2-8a3f-040f08bf86dd.jpg)
- The "Open Door" status, notifying the user about the process ("Door is opening")


![Screenshot_20211215_165814_io makeroid companion](https://user-images.githubusercontent.com/56561641/146357503-579394c0-0e3b-4275-b56f-6a067fd08adc.jpg)
- The "Blocked" status, notifying the user about an obstacle in front of the door while displaying the distance between them therefore interaction with the door is unavailable as long as said obstacle stays in the same spot.
