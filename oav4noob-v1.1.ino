// Branch v1.1
// Working version without servo sweep
// Visual Studio Code Integrated with Arduino

// Wish list
// do a servo sweep when obstacle is detected
// do a random decision to turn left or right
// add lcd monitor
// add rgb led light

#include <Servo.h>

Servo myServo;   // create Servo object
int pos = 10;    // initialise Servo position

//**********************************************************
//use these part if digital pins are used for the sensor
//#include <NewPing.h>	// import Sonar sensor library
//#define trigPin 12	// define the trigpin of HC-SR04
//#define echoPin 13	// define the echopin of HC-SR04
//**********************************************************

#include <AFMotor.h>	// import your Motor shield library
AF_DCMotor motor1(1, MOTOR12_64KHZ);	// set up left motor
AF_DCMotor motor2(3, MOTOR12_64KHZ);	// set up right motor
 
void setup() {
	Serial.begin(9600);   	// begin serial communitication  
  	Serial.println("Motor test!");
  	pinMode(A0, OUTPUT);  	// set the trigpin to output (send sound waves)
  	pinMode(A1, INPUT);		// set the echopin to input (recieve sound waves)
  	motor1.setSpeed(100);	// set the speed of the left motors, between 0-255
  	motor2.setSpeed (100);	// set the speed of the right motors, between 0-255
  	myServo.write(pos);		// set initial position of the Servo 
	}
 
void loop() {

	long duration, distance; // start the scan
  	digitalWrite(A0, LOW);  
  	delayMicroseconds(2);    // delays are required for a succesful sensor operation.
  	digitalWrite(A0, HIGH);

  	delayMicroseconds(10);   //this delay is required as well!
  	digitalWrite(A0, LOW);
  	duration = pulseIn(A1, HIGH);
  	distance = (duration/2) / 29.1; // convert the distance to centimeters.
  
  		if (distance < 25)/*if there's an obstacle 25 centimers, ahead, do the following: */ {   
  			Serial.println ("Close Obstacle detected!" );
  			Serial.println ("Obstacle Details:");
  			Serial.print ("Distance From Robot is " );
  			Serial.print ( distance);
  			Serial.print ( " CM!");   // print out the distance in centimeters.

  			Serial.println (" The obstacle is declared a threat due to close distance. ");
  			Serial.println (" Turning !");
    		motor1.run(FORWARD);  // Turn as long as there's an obstacle ahead.
    		motor2.run (BACKWARD);

		}
  		else {
   			Serial.println ("No obstacle detected. going forward");
   			delay (15);
   			motor1.run(FORWARD);  //if there's no obstacle ahead, Go Forward! 
    		motor2.run(FORWARD);  
  	}  

}	// end of program