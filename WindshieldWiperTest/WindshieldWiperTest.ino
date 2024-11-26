#include <ArduinoBLE.h>


 // Motor B connections (Connected to the L298N)
int enB = 3;
int in3 = 5;
int in4 = 4;

BLEService MotorService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy Motor Service

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

void setup() {
  // Lets me see print to a console log
  Serial.begin(9600);
  while(!Serial);

	// Set all the motor control pins to outputs
	pinMode(enB, OUTPUT);
	pinMode(in3, OUTPUT);
	pinMode(in4, OUTPUT);
	
	// Turn off motors - Initial state
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);

  if(!BLE.begin()){
    Serial.println("Starting Bluetooth Low Energy Module Failed!");

    while(1);
  }

  BLE.setLocalName("Room 315 Door Opener");
  BLE.setAdvertisedService(MotorService); 

  MotorService.addCharacteristic(switchCharacteristic);

  BLE.addService(MotorService);

  switchCharacteristic.writeValue(0);
  
  // Starts showing as an available device?
  BLE.advertise(); 

  Serial.println("BLE Motor Peripheral");
}

void loop() {
  BLEDevice central = BLE.central();

  if(central){ 
    Serial.print("Connected to central ");
    Serial.println(central.address());

    while(central.connected()){
      if(switchCharacteristic.written()){
        if (switchCharacteristic.value()){
          directionControl();
        } else{
          Serial.println(F("Motor off"));
        }
      }
    }
  }

   // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
	// directionControl();
	// delay(1000);
	// speedControl();
	// delay(1000);
}

// This function lets you control spinning direction of motors
void directionControl() {
	// Set motors to maximum speed
	// For PWM maximum possible values are 0 to 255
	analogWrite(enB, 255);

	// Turn on motor B
	digitalWrite(in3, HIGH);
	digitalWrite(in4, LOW);
	delay(2000);
	
	// Now change motor directions
	digitalWrite(in3, LOW);
	digitalWrite(in4, HIGH);
	delay(2000);
	
	// Turn off motors
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);
}

// This function lets you control speed of the motors
void speedControl() {
	// Turn on motors
	digitalWrite(in3, LOW);
	digitalWrite(in4, HIGH);
	
	// Accelerate from zero to maximum speed
	for (int i = 0; i < 256; i++) {
		analogWrite(enB, i);
		delay(20);
	}
	
	// Decelerate from maximum speed to zero
	for (int i = 255; i >= 0; --i) {
		analogWrite(enB, i);
		delay(20);
	}
	
	// Now turn off motors
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);
}