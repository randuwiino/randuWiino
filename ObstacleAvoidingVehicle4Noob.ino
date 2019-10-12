// This is a Master version
// Initial version without servo sweep

#include <Servo.h>

Servo myServo;  //create servo object
int pos = 10;    //Initialise servo position

#include <AFMotor.h> //import your motor shield library
//#define trigPin 12 // define the pins of your sensor
//#define echoPin 13 

AF_DCMotor motor1(1, MOTOR12_64KHZ); // set up motors.
AF_DCMotor motor2(3, MOTOR12_64KHZ);
 
void setup() {
  Serial.begin(9600);   // begin serial communitication  
  Serial.println("Motor test!");
  pinMode(A0, OUTPUT);  // set the trig pin to output (Send sound waves)
  pinMode(A1, INPUT);   // set the echo pin to input (recieve sound waves)
  motor1.setSpeed(100); //set the speed of the motors, between 0-255
  motor2.setSpeed (100);
  myServo.write(pos) 
}
 
void loop() {

   long duration, distance; // start the scan
  digitalWrite(A0, LOW);  
  delayMicroseconds(2);     // delays are required for a succesful sensor operation.
  digitalWrite(A0, HIGH);

  delayMicroseconds(10);    //this delay is required as well!
  digitalWrite(A0, LOW);
  duration = pulseIn(A1, HIGH);
  distance = (duration/2) / 29.1;// convert the distance to centimeters.
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
   motor1.run(FORWARD);   //if there's no obstacle ahead, Go Forward! 
    motor2.run(FORWARD);  
  }  
 
}
