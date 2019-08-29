#include "CoffeeControl.h"

void setup(){
	
	//initialize pins, Serial Display and motors
	mySerial.begin(9600);
	myServo.attach(8);
	pinMode(9, INPUT);
	pinMode(5, OUTPUT);
	digitalWrite(5, HIGH);
	myServo.write(95);
	myServo.detach();
	mySerial.write(12);
	delay(100);
}

void loop(){
	if(running){
		
	//-------------- Temperature calculation from sensor data ----------------
	
		// reading byte data from sensor and converting to degrees
		int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;
		byte i;
		byte present = 0;
		byte data[12];
		byte addr[8];
		
		if(!ds.search(addr)){
			ds.reset_search();
			return;
		}
		
		ds.reset();
		ds.select(addr);
		ds.write(0x44, 1);
		delay(1000);
		
		present = ds.reset();
		ds.select(addr);
		ds.write(0xBE);
		for(i = 0; i < 9; i++){
			data[i] = ds.read();
		}
		
		//seperating whole and decimal portions of temperature reading
		
		LowByte = data[0];
		HighByte = data[1];
		TReading = (HighByte<<8) + LowByte;
		SignBit = TReading & 0x8000;
		if(SignBit){
			TReading = (TReading^0xffff) + 1;
			
		}
		Tc_100 = (6*TReading) + TReading/4
		
		Whole = Tc_100/100;
		Fract = Tc_100%100;
		
		//printing out whole and decimal portions of the temperature reading to Serial display
		
		mySerial.write(12);
		delay(100);
		
		if(SignBit){
			mySerial.print("-");
		}
		mySerial.print(Whole);
		mySerial.print(".");
		if(fract < 10){
			mySerial.print("0");
		}
		mySerial.print(Fract);
		mySerial.print(" degrees celsius");
		
	//---------------------- Mug servo motor control --------------------------
		
		//if drink in mug is too hot, i.e. above 100 degrees celsius
		if(Whole >= 100){
			
			//open lid if closed
			if(closed){
				myServo.attach(8);
				for(int i = 70; i < 96; i++){
					myServo.write(i);
					delay(15);
				}
				myServo.detach();
				closed = false;
			}
			
			//if user picks up mug warn that it is too hot
			if(digitalRead(9) == HIGH){
				tone(6, 3000, 1000);
				mySerial.write(12);
				delay(100);
				mySerial.print("DO NOT PICK UP DRINK IS TOO HOT");
				while(digitalRead(9) == HIGH){
					delay(5);
				}
			}
		}
		
		// if drink is at regular temperature
		if(Whole < 100){
			
			// if lid is open close it
			if(!closed){
				myServo.attach(8);
				for(int i = 70; i > 94; i--){
					myServo.write(i);
					delay(15);
				}
				myServo.detach();
				closed = true;
			}
			
			// if user picks up mug
			if(digitalRead(9) == HIGH){
				
				//open lid
				myServo.attach(8);
				for(int i = 70; i < 96; i++){
					myServo.write(i);
					delay(15);
				}
				myServo.detach();
				closed = false;
				
				//wait till mug is put down
				while(digitalRead(9) = HIGH){
					delay(5);
				}
				
				//close lid
				myServo.attach(8);
				for(int i = 70; i > 94; i--){
					myServo.write(i);
					delay(15);
				}
				myServo.detach();
				closed = true;
				
			}
		}
		
		//if off button is pressed
		if(digitalRead(7) == HIGH){
			
			//print out off message
			delay(100);
			mySerial.write(12);
			delay(100);
			mySerial.print("Turning Off");
			running = false;
			
			//if lid is open close it
			if(closed){
				myServo.attach(8);
				for(int i = 70; i < 96; i++){
					myServo.write(i);
					delay(15);
				}
				myServo.detach();
				closed = false;
			}
		}
				
	}else{
		
		//if on button is not pressed
		if(digitalRead(7) == LOW){
			
			//print out that the mug is off
			mySerial.write(12);
			delay(15);
			mySerial.print("off");
		}
		
		//wait till on button is pressed
		while(digitalRead(7) == LOW){
			delay(5);
		}
		
		//print out that mug is turning on
		running = true;
		delay(15);
		mySerial.write(12);
		mySerial.print("Turning On");
		delay(500);
	}
}