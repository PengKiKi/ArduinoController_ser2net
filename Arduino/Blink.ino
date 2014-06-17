
int maximumspeed = 220;//400*(7.4/13.5)

class DualVNH5019MotorShield
{
public:
	// CONSTRUCTORS
	DualVNH5019MotorShield(); // Default pin selection.
	DualVNH5019MotorShield(unsigned char INA1, unsigned char INB1, unsigned char PWM1, unsigned char EN1DIAG1, unsigned char CS1,
		unsigned char INA2, unsigned char INB2, unsigned char PWM2, unsigned char EN2DIAG2, unsigned char CS2); // User-defined pin selection. 

	// PUBLIC METHODS
	void init(); // Initialize TIMER 1, set the PWM to 20kHZ. 
	void setM1Speed(int speed); // Set speed for M1.
	void setM2Speed(int speed); // Set speed for M2.
	void setSpeeds(int m1Speed, int m2Speed); // Set speed for both M1 and M2.
	void setM1Brake(int brake); // Brake M1. 
	void setM2Brake(int brake); // Brake M2.
	void setBrakes(int m1Brake, int m2Brake); // Brake both M1 and M2.
	unsigned int getM1CurrentMilliamps(); // Get current reading for M1. 
	unsigned int getM2CurrentMilliamps(); // Get current reading for M2.
	unsigned char getM1Fault(); // Get fault reading from M1.
	unsigned char getM2Fault(); // Get fault reading from M2.

private:
	unsigned char _INA1;
	unsigned char _INB1;
	unsigned char _PWM1;
	unsigned char _EN1DIAG1;
	unsigned char _CS1;
	unsigned char _INA2;
	unsigned char _INB2;
	unsigned char _PWM2;
	unsigned char _EN2DIAG2;
	unsigned char _CS2;
};
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
DualVNH5019MotorShield::DualVNH5019MotorShield()
{
	//Pin on arduino	//Pin on motorshield
	_INA1 = 2;			//PIN 2
	_INB1 = 4;			//PIN 4
	_EN1DIAG1 = 6;		//PIN 6
	_CS1 = A0;			//PIN A0
	_INA2 = 7;			//PIN 7
	_INB2 = 8;			//PIN 8
	_EN2DIAG2 = 12;		//PIN 12
	_CS2 = A1;			//PIN A1
	_PWM1 = 9;			//PIN 9
	_PWM2 = 10;			//PIN 10
}

DualVNH5019MotorShield::DualVNH5019MotorShield(unsigned char INA1, unsigned char INB1, unsigned char PWM1, unsigned char EN1DIAG1, unsigned char CS1,
	unsigned char INA2, unsigned char INB2, unsigned char PWM2, unsigned char EN2DIAG2, unsigned char CS2)
{
	//Pin map
	//PWM1 and PWM2 cannot be remapped because the library assumes PWM is on timer1
	_INA1 = INA1;
	_INB1 = INB1;
	_EN1DIAG1 = EN1DIAG1;
	_CS1 = CS1;
	_INA2 = INA2;
	_INB2 = INB2;
	_EN2DIAG2 = EN2DIAG2;
	_CS2 = CS2;
	_PWM1 = PWM1;
	_PWM2 = PWM2;
}

// Public Methods //////////////////////////////////////////////////////////////
void DualVNH5019MotorShield::init()
{
	// Define pinMode for the pins and set the frequency for timer1.

	pinMode(_INA1, OUTPUT);
	pinMode(_INB1, OUTPUT);
	pinMode(_PWM1, OUTPUT);
	pinMode(_EN1DIAG1, INPUT);
	pinMode(_CS1, INPUT);
	pinMode(_INA2, OUTPUT);
	pinMode(_INB2, OUTPUT);
	pinMode(_PWM2, OUTPUT);
	pinMode(_EN2DIAG2, INPUT);
	pinMode(_CS2, INPUT);
#if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32U4__)
	// Timer 1 configuration
	// prescaler: clockI/O / 1
	// outputs enabled
	// phase-correct PWM
	// top of 400
	//
	// PWM frequency calculation
	// 16MHz / 1 (prescaler) / 2 (phase-correct) / 400 (top) = 20kHz

	TCCR1A = 0b10100000;
	TCCR1B = 0b00010001;
	ICR1 = 400;
#endif
}
// Set speed for motor 1, speed is a number betwenn -400 and 400
void DualVNH5019MotorShield::setM1Speed(int speed)
{
	unsigned char reverse = 0;

	if (speed < 0)
	{
		speed = -speed;  // Make speed a positive quantity
		reverse = 1;  // Preserve the direction
	}
	if (speed > /*400*/maximumspeed)  // Max PWM dutycycle
		speed = /*400*/maximumspeed;
#if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32U4__)
	OCR1A = speed;
#else
	analogWrite(_PWM1, speed * 51 / 80); // default to using analogWrite, mapping 400 to 255
#endif
	if (speed == 0)
	{
		digitalWrite(_INA1, LOW);   // Make the motor coast no
		digitalWrite(_INB1, LOW);   // matter which direction it is spinning.
	}
	else if (reverse)
	{
		digitalWrite(_INA1, LOW);
		digitalWrite(_INB1, HIGH);
	}
	else
	{
		digitalWrite(_INA1, HIGH);
		digitalWrite(_INB1, LOW);
	}
}

// Set speed for motor 2, speed is a number betwenn -400 and 400
void DualVNH5019MotorShield::setM2Speed(int speed)
{
	unsigned char reverse = 0;

	if (speed < 0)
	{
		speed = -speed;  // make speed a positive quantity
		reverse = 1;  // preserve the direction
	}
	if (speed > /*400*/maximumspeed)  // Max 
		speed = /*400*/maximumspeed;
#if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32U4__)
	OCR1B = speed;
#else
	analogWrite(_PWM2, speed * 51 / 80); // default to using analogWrite, mapping 400 to 255
#endif 
	if (speed == 0)
	{
		digitalWrite(_INA2, LOW);   // Make the motor coast no
		digitalWrite(_INB2, LOW);   // matter which direction it is spinning.
	}
	else if (reverse)
	{
		digitalWrite(_INA2, LOW);
		digitalWrite(_INB2, HIGH);
	}
	else
	{
		digitalWrite(_INA2, HIGH);
		digitalWrite(_INB2, LOW);
	}
}

// Set speed for motor 1 and 2
void DualVNH5019MotorShield::setSpeeds(int m1Speed, int m2Speed)
{
	setM1Speed(m1Speed);
	setM2Speed(m2Speed);
}

// Brake motor 1, brake is a number between 0 and 400
void DualVNH5019MotorShield::setM1Brake(int brake)
{
	// normalize brake
	if (brake < 0)
	{
		brake = -brake;
	}
	if (brake > /*400*/maximumspeed)  // Max brake
		brake = /*400*/maximumspeed;
	digitalWrite(_INA1, LOW);
	digitalWrite(_INB1, LOW);
#if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32U4__)
	OCR1A = brake;
#else
	analogWrite(_PWM1, brake * 51 / 80); // default to using analogWrite, mapping 400 to 255
#endif
}

// Brake motor 2, brake is a number between 0 and 400
void DualVNH5019MotorShield::setM2Brake(int brake)
{
	// normalize brake
	if (brake < 0)
	{
		brake = -brake;
	}
	if (brake > /*400*/maximumspeed)  // Max brake
		brake = /*400*/maximumspeed;
	digitalWrite(_INA2, LOW);
	digitalWrite(_INB2, LOW);
#if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32U4__)
	OCR1B = brake;
#else
	analogWrite(_PWM2, brake * 51 / 80); // default to using analogWrite, mapping 400 to 255
#endif
}

// Brake motor 1 and 2, brake is a number between 0 and /*400*/maximumspeed
void DualVNH5019MotorShield::setBrakes(int m1Brake, int m2Brake)
{
	setM1Brake(m1Brake);
	setM2Brake(m2Brake);
}

// Return motor 1 current value in milliamps.
unsigned int DualVNH5019MotorShield::getM1CurrentMilliamps()
{
	// 5V / 1024 ADC counts / 144 mV per A = 34 mA per count
	return analogRead(_CS1) * 34;
}

// Return motor 2 current value in milliamps.
unsigned int DualVNH5019MotorShield::getM2CurrentMilliamps()
{
	// 5V / 1024 ADC counts / 144 mV per A = 34 mA per count
	return analogRead(_CS2) * 34;
}

// Return error status for motor 1 
unsigned char DualVNH5019MotorShield::getM1Fault()
{
	return !digitalRead(_EN1DIAG1);
}

// Return error status for motor 2 
unsigned char DualVNH5019MotorShield::getM2Fault()
{
	return !digitalRead(_EN2DIAG2);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
struct Packet
{
	//#define __PACKET__DEBUG__
	static const char starter = 29;
	static const char terminator = 30;

	
	static const char get_milliamps = 1;
	static const char get_fault = 2;
	static const char set_brakes = 3;
	static const char set_speed = 4;
	static const char set_brakes_A = 5;
	static const char set_brakes_B = 6;
	static const char set_speed_A = 7;
	static const char set_speed_B = 8;
	static const char echo_data = 9;
	static const char set_max_speed = 10;
	static const char Arduino_Armed = 11;
	static const char Arduino_Unarmed = 12;
	static const char Arduino_Unarm_Error = 13;
	static const char TEST = 14;
	static const char GET_TEMP = 15;
	static const char GET_BATTERY_VOLTAGE = 16;
	static const char unlock = 17;
	static const char get_fault_1 = 18;
	static const char get_fault_2 = 19;
	static const char UN_ARM = 33;
	static const char arm_motorshield = 54;
	static const char CONNECTION_OK = 69;
	static const char I_AM_ALIVE = 111;

#if defined WIN32
#define ARDUINO_INTEGER short
#if defined __PACKET__DEBUG__
	typedef unsigned char uint8_t;
	class _Serial
	{
	public:
		size_t write(const uint8_t* data, size_t size)
		{
			std::cout << "DATA HEX: ";
			for (size_t i = 0; i < (size); ++i)
			{
				std::cout << "[" << std::hex << (int)data[i] << "]";
			}
			std::cout << "\r\n" << std::flush;
			std::cout << "DATA DEC: ";
			for (size_t i = 0; i < (size); ++i)
			{
				std::cout << "[" << std::dec << (int)data[i] << "]";
			}
			std::cout << "\r\n" << std::flush;
			return size;
		}
	} Serial;
	size_t n_send_socket(int socketid, char* data, size_t size)
	{
		std::cout << "DATA HEX: ";
		for (size_t i = 0; i < (size); ++i)
		{
			std::cout << "[" << std::hex << (int)data[i] << "]";
		}
		std::cout << "\r\n" << std::flush;
		std::cout << "DATA DEC: ";
		for (size_t i = 0; i < (size); ++i)
		{
			std::cout << "[" << std::dec << (int)data[i] << "]";
		}
		std::cout << "\r\n" << std::flush;
		return size;
	}
#endif
#else
#define ARDUINO_INTEGER int
#endif
private:
	char data[7];
public:
	Packet()
	{
		data[0] = starter;
		data[6] = terminator;
	}
	Packet(const char action, const ARDUINO_INTEGER value_one, const ARDUINO_INTEGER value_two, bool smallmode = false)
	{
		set(action, value_one, value_two, smallmode);
	}
	Packet(const char action, const char byte_a, const char byte_b, const char byte_c, const char byte_d)
	{
		set(action, byte_a, byte_b, byte_c, byte_d);
	}
	void set(const char action, const ARDUINO_INTEGER value_one, const ARDUINO_INTEGER value_two, bool smallmode = false)
	{
		data[0] = starter;
		data[1] = action;
		if (smallmode)
		{
			data[2] = (char)(floor(value_one / 100));
			data[3] = (char)((value_one - (data[2] * 100)));
			data[4] = (char)(floor(value_two / 100));
			data[5] = (char)((value_two - (data[4] * 100)));
		}
		else
		{
			data[2] = (char)(floor(value_one / 1000));
			data[3] = (char)((value_one - (data[2] * 1000)) / 8);
			data[4] = (char)(floor(value_two / 1000));
			data[5] = (char)((value_two - (data[4] * 1000)) / 8);
		}
		data[6] = terminator;
	}
	void set(const char action, const char byte_a, const char byte_b, const char byte_c, const char byte_d)
	{
		data[0] = starter;
		data[1] = action;
		data[2] = byte_a;
		data[3] = byte_b;
		data[4] = byte_c;
		data[5] = byte_d;
		data[6] = terminator;
	}
	void set(char * _data, size_t datasize = 7)
	{
		if (datasize > 7)
			datasize = 7;
		for (size_t i = 0; i < datasize; ++i)
		{
			data[i] = _data[i];
		}
	}
	const ARDUINO_INTEGER GetValueOne(bool smallmode = false)
	{
		if (smallmode)
		{
			return ((data[2] * 100) + (data[3]));
		}
		else
		{
			return ((data[2] * 1000) + (data[3] * 8));
		}
	}
	const ARDUINO_INTEGER GetValueTwo(bool smallmode = false)
	{
		if (smallmode)
		{
			return ((data[4] * 100) + (data[5]));
		}
		else
		{
			return ((data[4] * 1000) + (data[5] * 8));
		}
	}
	const char GetAction()
	{
		return data[1];
	}
	char GetB1()
	{
		return data[2];
	}
	char GetB2()
	{
		return data[3];
	}
	char GetB3()
	{
		return data[4];
	}
	char GetB4()
	{
		return data[5];
	}
	bool valid()
	{
		return (data[0] == starter && data[6] == terminator);
	}
	char * get()
	{
		return &data[0];
	}
	size_t size()
	{
		return 7;
	}
#if defined WIN32
	bool Send(int socketid)
	{
		if (n_send_socket(socketid, data, 7))
		{
			return true;
		}
		return false;
	}
	bool Send(int socketid, const char action, const char byte_a, const char byte_b, const char byte_c, const char byte_d)
	{
		set(action, byte_a, byte_b, byte_c, byte_d);
		if (n_send_socket(socketid, data, 7))
		{
			return true;
		}
		return false;
	}
	bool Send(int socketid, const char action, const ARDUINO_INTEGER value_one, const ARDUINO_INTEGER value_two, bool smallmode = false)
	{
		set(action, value_one, value_two, smallmode);
		if (n_send_socket(socketid, data, 7))
		{
			return true;
		}
		return false;
	}
#else
	void Send()
	{
		Serial.write((uint8_t*)data, 7);
	}
	void Send(const char action, const char byte_a, const char byte_b, const char byte_c, const char byte_d)
	{
		set(action, byte_a, byte_b, byte_c, byte_d);
		Serial.write((uint8_t*)data, 7);
	}
	void Send(const char action, const ARDUINO_INTEGER value_one, const ARDUINO_INTEGER value_two, bool smallmode = false)
	{
		set(action, value_one, value_two, smallmode);
		Serial.write((uint8_t*)data, 7);
	}
#endif
#undef ARDUINO_INTEGER
};

DualVNH5019MotorShield motors;

char LED1 = 5;
char LED2 = 3;

void setup()
{
	Serial.begin(115200);
	motors.init();
	
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);

	//temperature
	pinMode(A2, INPUT);
	//battery voltage
	pinMode(A3, INPUT);

	digitalWrite(LED1, HIGH);
	digitalWrite(LED2, LOW);

	motors.setSpeeds(0, 0);
}

uint8_t ReceiveBuffer[64];
int AR_STATE = 0;

class AliveTime
{
	unsigned long LastAliveTick;
	unsigned long ConnectionOk;
public:
	AliveTime()
	{
		LastAliveTick = millis();
		ConnectionOk = LastAliveTick;
	}
	void IAmAlive()
	{
		LastAliveTick = millis();
	}
	unsigned long GetElapsed()
	{
		return millis() - LastAliveTick;
	}
	void ConnectionCheck()
	{
		ConnectionOk = millis();
	}
	unsigned long ConnectionOkElapsed()
	{
		return millis() - ConnectionOk;
	}
} AliveTimer;
//starter action data1 data2 data3 data4 terminator|7

char ACTION_TO_DO;
int Value_One;
int Value_Two;

Packet ConnectionOkay(Packet::CONNECTION_OK, 0, 0);
Packet universal;

class CurrentProtection
{
	unsigned long LastCuttentSenseStart[2];
	int hasovercurrent[2];
	static const unsigned short OverCurrentProtectionKickIn = 13500;
	static const unsigned short OverCurrentProtectionKickInTime = 150;//ms
public:
	CurrentProtection()
	{
		hasovercurrent[0] = 0;
		hasovercurrent[1] = 1;
	}
	unsigned long GetElapsed(int index)
	{
		return millis() - LastCuttentSenseStart[index];
	}
	void Check()
	{
		if (hasovercurrent[0])
		{
			if (GetElapsed(0) > OverCurrentProtectionKickInTime)
			{
				if (motors.getM1CurrentMilliamps() > OverCurrentProtectionKickIn)
				{
					motors.setM1Speed(0);
				}
				else
				{
					hasovercurrent[0] = 0;
				}
			}
		}
		else
		{
			if (motors.getM1CurrentMilliamps() > OverCurrentProtectionKickIn)
			{
				hasovercurrent[0] = 1;
				LastCuttentSenseStart[0] = millis();
			}
		}
		if (hasovercurrent[1])
		{
			if (GetElapsed(1) > OverCurrentProtectionKickInTime)
			{
				if (motors.getM2CurrentMilliamps() > OverCurrentProtectionKickIn)
				{
					motors.setM2Speed(0);
				}
				else
				{
					hasovercurrent[1] = 0;
				}
			}
		}
		else
		{
			if (motors.getM2CurrentMilliamps() > OverCurrentProtectionKickIn)
			{
				hasovercurrent[1] = 1;
				LastCuttentSenseStart[1] = millis();
			}
		}
	}
} OverCurrentProtection;

void SafetyChecks()
{
	OverCurrentProtection.Check();
	if (AliveTimer.GetElapsed() > 500)
	{
		motors.setBrakes(400, 400);
	}
	if (AliveTimer.ConnectionOkElapsed() > 1000)
	{
		AliveTimer.ConnectionCheck();
		ConnectionOkay.Send();
	}
}

void Tick()
{
	while (Serial.available() >= 7 && AR_STATE)
	{
		if (Serial.read() == Packet::starter)
		{
			ReceiveBuffer[0] = Packet::starter;
			Serial.readBytes((char*)&ReceiveBuffer[1], 6);
			universal.set((char*)ReceiveBuffer);
			if (universal.valid())
			{
				ACTION_TO_DO = universal.GetAction();
				if(ACTION_TO_DO == Packet::get_milliamps)
				{
					unsigned int I1 = motors.getM1CurrentMilliamps();
					unsigned int I2 = motors.getM2CurrentMilliamps();
					universal.Send(ACTION_TO_DO, I1, I2);
				}
				else if (ACTION_TO_DO == Packet::get_fault)
				{
					if (!motors.getM1Fault() && !motors.getM2Fault())
						universal.Send(ACTION_TO_DO, 0, 0, 0, 0);
					else if (motors.getM1Fault() && motors.getM2Fault())
						universal.Send(ACTION_TO_DO, 1, 1, 0, 0);
					else if (!motors.getM1Fault() && motors.getM2Fault())
						universal.Send(ACTION_TO_DO, 0, 1, 0, 0);
					else universal.Send(ACTION_TO_DO, 1, 0, 0, 0);
				}
				else if (ACTION_TO_DO == Packet::get_fault_1)
				{
					universal.Send(ACTION_TO_DO, motors.getM1Fault(), 0, true);
				}
				else if (ACTION_TO_DO == Packet::get_fault_2)
				{
					universal.Send(ACTION_TO_DO, motors.getM2Fault(), 0, true);
				}
				else if (ACTION_TO_DO == Packet::set_brakes)
				{			
					motors.setBrakes(universal.GetValueOne(true), universal.GetValueTwo(true));
				}
				else if (ACTION_TO_DO == Packet::set_speed)
				{
					motors.setSpeeds(universal.GetValueOne(true), universal.GetValueTwo(true));
				}
				else if (ACTION_TO_DO == Packet::set_brakes_A)
				{
					motors.setM1Brake(universal.GetValueOne(true));
				}
				else if (ACTION_TO_DO == Packet::set_brakes_B)
				{
					motors.setM2Brake(universal.GetValueOne(true));
				}
				else if (ACTION_TO_DO == Packet::set_speed_A)
				{
					motors.setM1Speed(universal.GetValueOne(true));
				}
				else if (ACTION_TO_DO == Packet::set_speed_B)
				{
					motors.setM2Speed(universal.GetValueOne(true));
				}
				else if (ACTION_TO_DO == Packet::echo_data)
				{
					ReceiveBuffer[1] = ReceiveBuffer[0];
					ReceiveBuffer[2] = ReceiveBuffer[1];
					ReceiveBuffer[3] = ReceiveBuffer[2];
					ReceiveBuffer[4] = ReceiveBuffer[3];
					ReceiveBuffer[5] = ReceiveBuffer[4];
					ReceiveBuffer[0] = Packet::starter;
					ReceiveBuffer[6] = Packet::terminator;
					Serial.write(ReceiveBuffer, 7);
				}
				else if (ACTION_TO_DO == Packet::I_AM_ALIVE)
				{
					AliveTimer.IAmAlive();
				}
				else if (ACTION_TO_DO == Packet::arm_motorshield)
				{
					universal.Send(Packet::Arduino_Armed, Packet::Arduino_Armed, Packet::Arduino_Armed, Packet::Arduino_Armed, Packet::Arduino_Armed);
				}
				else if (ACTION_TO_DO == Packet::UN_ARM)
				{
					if (universal.GetValueOne(true) == 5000 && universal.GetValueTwo(true) == 3333)
					{
						digitalWrite(LED1, HIGH);
						digitalWrite(LED2, LOW);
						universal.Send(Packet::Arduino_Unarm_Error, Packet::Arduino_Unarm_Error, Packet::Arduino_Unarm_Error, Packet::Arduino_Unarm_Error, Packet::Arduino_Unarm_Error);
						motors.setSpeeds(0, 0);
						AR_STATE = 0;
					}
				}
				else if (ACTION_TO_DO == Packet::set_max_speed)
				{
					Value_Two = abs(universal.GetValueOne(true));
					if (Value_Two > 400)
					{
						Value_Two = 400;
					}
					maximumspeed = Value_Two;
				}
				else if (ACTION_TO_DO == Packet::TEST)
				{
					motors.setSpeeds(80, 80);
				}
				else if (ACTION_TO_DO == Packet::GET_BATTERY_VOLTAGE)
				{
					universal.Send(ACTION_TO_DO, analogRead(A3), 0, true);
				}
				else if (ACTION_TO_DO == Packet::GET_TEMP)
				{
					universal.Send(ACTION_TO_DO, analogRead(A2), 0, true);
				}
			}
		}
		SafetyChecks();
	}
	SafetyChecks();
}

static const int __debug__ = 0;
int __debug__once = 0;
void loop()
{
	if (__debug__ == 1)
	{
		if (__debug__once == 0)
		{
			motors.setSpeeds(80,80);

			__debug__once = 1;
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			motors.setSpeeds(0,0);
			delay(500);
			motors.setSpeeds(-80, -80);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			delay(100);
			Serial.print("M1 current: ");
			Serial.println(motors.getM2CurrentMilliamps());
			motors.setSpeeds(0, 0);
			Serial.println("DONE");
		}
	}
	else
	{
		if (AR_STATE)
		{
			Tick();
		}
		else
		{
			while (Serial.available() >= 7 && AR_STATE == 0)
			{
				if (Serial.read() == Packet::starter && AR_STATE == 0)
				{
					char tempbuffer[6];
					Serial.readBytes(&tempbuffer[0], 6);
					if (tempbuffer[0] == Packet::arm_motorshield && (tempbuffer[1] == Packet::unlock || tempbuffer[2] == Packet::unlock || tempbuffer[3] == Packet::unlock || tempbuffer[4] == Packet::unlock) && tempbuffer[5] == Packet::terminator)
					{
						AR_STATE = 1;
						universal.Send(Packet::Arduino_Armed, Packet::Arduino_Armed, Packet::Arduino_Armed, Packet::Arduino_Armed, Packet::Arduino_Armed);
						digitalWrite(LED1, LOW);
						digitalWrite(LED2, HIGH);
					}
					else
					{
						universal.Send(Packet::Arduino_Unarm_Error, Packet::Arduino_Unarm_Error, Packet::Arduino_Unarm_Error, Packet::Arduino_Unarm_Error, Packet::Arduino_Unarm_Error);
						digitalWrite(LED1, LOW);
						delay(50);
						digitalWrite(LED1, HIGH);
					}
				}
				else
				{
					universal.Send(Packet::Arduino_Unarm_Error, Packet::Arduino_Unarm_Error, Packet::Arduino_Unarm_Error, Packet::Arduino_Unarm_Error, Packet::Arduino_Unarm_Error);
				}
			}
		}
	}
}
