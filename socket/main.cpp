#include "main.h"
#include <Windows.h>
#include <iostream>
#include <intrin.h>

CSocket* g_pSocket;
std::queue<remoteConnect> onRemoteConnect;
std::queue<remoteDisconnect> onRemoteDisconnect;
std::queue<receiveData> onSocketReceiveData;
std::queue<socketAnswer> onSocketAnswer;
std::queue<socketClose> onSocketClose;
std::queue<socketUDP> onUDPReceiveData;

#define MAX_SOCKETS 10
#define INVALID_SOCKET (-1)
#define NO_IP_RETURN "0.0.0.0"

enum pType {
	TCP = 1,
	UDP = 2
};
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

#define THIS_IS_DLL

#if defined THIS_IS_DLL 

BOOL WINAPI DllMain(
	_In_  HINSTANCE hinstDLL,
	_In_  DWORD fdwReason,
	_In_  LPVOID lpvReserved
	)
{
	return true;
}

char host[16] = "192.168.2.222";
Packet packetinfo;
Packet alive(Packet::I_AM_ALIVE, Packet::I_AM_ALIVE, Packet::I_AM_ALIVE, Packet::I_AM_ALIVE, Packet::I_AM_ALIVE);
ULONGLONG time_last_alive_sent = 0;
ULONGLONG timenow;

int SocketID;
int port = 1001;

extern "C" __declspec(dllexport) void __cdecl Init();
extern "C" __declspec(dllexport) bool __cdecl Connect(const char * _host = host);
extern "C" __declspec(dllexport) bool __cdecl Connected();
extern "C" __declspec(dllexport) void __cdecl cleanup();
extern "C" __declspec(dllexport) bool __cdecl IsArmed();
extern "C" __declspec(dllexport) bool __cdecl Reconnect();
extern "C" __declspec(dllexport) double __cdecl GetVoltage();
extern "C" __declspec(dllexport) double __cdecl GetTemperature();
extern "C" __declspec(dllexport) bool __cdecl GetMotor1Fault();
extern "C" __declspec(dllexport) bool __cdecl GetMotor2Fault();
extern "C" __declspec(dllexport) unsigned short __cdecl GetMotor1Current();
extern "C" __declspec(dllexport) unsigned short __cdecl GetMotor2Current();
extern "C" __declspec(dllexport) ULONGLONG __cdecl GetLastReportedTimeAlive();
extern "C" __declspec(dllexport) bool __cdecl UnArm();
extern "C" __declspec(dllexport) bool __cdecl Arm();
extern "C" __declspec(dllexport) void __cdecl Run();
extern "C" __declspec(dllexport) bool __cdecl SetSpeed(short speed_m1, short speed_m2);
extern "C" __declspec(dllexport) bool __cdecl SetBrake(short brake_m1, short brake_m2);

struct SRobotInfo
{
	bool Armed;
	bool Connected;
	ULONGLONG last_time_reported_alive;
	double Voltage;
	double Temperature;
	bool AliveSendError;
	unsigned short MotorCurrent[2];
	bool MotorFault[2];
	SRobotInfo()
	{
		Armed = false;
		Connected = false;
		last_time_reported_alive = 0;
		Voltage = 0.0;
		Temperature = 0.0;
		AliveSendError = false;
		MotorCurrent[0] = 0;
		MotorCurrent[1] = 0;
		MotorFault[0] = false;
		MotorFault[1] = false;
	}

} * RobotInfo;

int currentcycle = 0;
ULONGLONG lastcycletime = 0;

__declspec(dllexport) void __cdecl Run()
{
	if (RobotInfo->Connected)
	{
		timenow = GetTickCount64();
		if (RobotInfo->Armed)
		{
			bool did_update = false;
			if ((timenow - time_last_alive_sent) > 200)
			{
				time_last_alive_sent = timenow;
				if (!alive.Send(SocketID))
				{
					RobotInfo->AliveSendError = true;
				}
				else
				{
					RobotInfo->AliveSendError = false;
				}
				did_update = true;
			}
			if (did_update == false && RobotInfo->AliveSendError == false && ((timenow - lastcycletime) > 50))
			{
				lastcycletime = timenow;
				++currentcycle;
				switch (currentcycle)
				{
					case 1:
					{
						packetinfo.Send(SocketID, Packet::GET_BATTERY_VOLTAGE, 0, 0, 0, 0);
						break;
					}
					case 2:
					{
						packetinfo.Send(SocketID, Packet::get_fault_1, 0, 0, 0, 0);
						break;
					}
					case 3:
					{
						packetinfo.Send(SocketID, Packet::get_fault_2, 0, 0, 0, 0);
						break;
					}
					case 4:
					{
						packetinfo.Send(SocketID, Packet::get_milliamps, 0, 0, 0, 0);
						break;
					}
					case 5:
					{
						packetinfo.Send(SocketID, Packet::GET_TEMP, 0, 0, 0, 0);
						currentcycle = 0;
						break;
					}
					default:
					{
						currentcycle = 0;
						break;
					}
				}
			}
		}
		if (!onSocketAnswer.empty()) 
		{
			socketAnswer tempData = onSocketAnswer.front();
			RobotInfo->last_time_reported_alive = timenow;
			{
				packetinfo.set(tempData.data, 7);
				switch (packetinfo.GetAction())
				{
					case Packet::get_milliamps:
					{
						RobotInfo->MotorCurrent[0] = (unsigned short)packetinfo.GetValueOne();
						RobotInfo->MotorCurrent[1] = (unsigned short)packetinfo.GetValueTwo();
						break;
					}
					case Packet::get_fault:
					{
						RobotInfo->MotorFault[0] = (packetinfo.GetB1() != 0);
						RobotInfo->MotorFault[1] = (packetinfo.GetB2() != 0);
						break;
					}
					case Packet::get_fault_1:
					{
						RobotInfo->MotorFault[0] = (packetinfo.GetValueOne(true) != 0);
						break;
					}
					case Packet::get_fault_2:
					{
						RobotInfo->MotorFault[1] = (packetinfo.GetValueOne(true) != 0);
						break;
					}
					case Packet::GET_BATTERY_VOLTAGE:
					{
						double Voltage2 = ((double)packetinfo.GetValueOne(true) / 72.013093289689034369885433715221);
						RobotInfo->Voltage = Voltage2;
						break;
					}
					case Packet::GET_TEMP:
					{
						double mVout = (((double)packetinfo.GetValueOne(true)) / 1023.0) * 5000.0;
						double Temp = (mVout - 400.0) / 19.5; //Ta = (Vout-400mV)/19.5mV //Original
						RobotInfo->Temperature = Temp;
						break;
					}
					case Packet::Arduino_Armed:
					{
						RobotInfo->Armed = true;
						break;
					}
					case Packet::Arduino_Unarmed:
					{
						RobotInfo->Armed = false;
						break;
					}
					case Packet::Arduino_Unarm_Error:
					{
						RobotInfo->Armed = false;
						break;
					}
				}
			}
			free(tempData.data);
			onSocketAnswer.pop();
		}
		if (!onSocketClose.empty()) 
		{
			socketClose tempData = onSocketClose.front();
			RobotInfo->Connected = false;
			onSocketClose.pop();
			SocketID = n_create_socket(pType::TCP);
		}
	}
	else
	{
		if (n_connect_socket(SocketID, host, port))
		{
			RobotInfo->Connected = true;
		}
		else
		{
			RobotInfo->Connected = false;
		}
	}
}


__declspec(dllexport) void __cdecl Init()
{
	g_pSocket = new CSocket();
	time_last_alive_sent = 0;
	timenow = GetTickCount64();
	SocketID = 0;
	port = 1001;
	RobotInfo = new SRobotInfo();
	currentcycle = 0;
	lastcycletime = 0;
	RobotInfo->Armed = false;
	RobotInfo->Connected = false;
	RobotInfo->last_time_reported_alive = 0;
	RobotInfo->Voltage = 0.0;
	RobotInfo->Temperature = 0.0;
	RobotInfo->AliveSendError = false;
	RobotInfo->MotorCurrent[0] = 0;
	RobotInfo->MotorCurrent[1] = 0;
	RobotInfo->MotorFault[0] = false;
	RobotInfo->MotorFault[1] = false;
	alive.set(Packet::I_AM_ALIVE, Packet::I_AM_ALIVE, Packet::I_AM_ALIVE, Packet::I_AM_ALIVE, Packet::I_AM_ALIVE);
	memcpy(host, "192.168.2.222", 14);
	SocketID = n_create_socket(pType::TCP);
}

__declspec(dllexport) bool __cdecl Connect(char * _host)
{
	memcpy(host, _host, strlen(_host));
	if (n_connect_socket(SocketID, host, port))
	{
		RobotInfo->Connected = true;
		return true;
	}
	else
	{
		RobotInfo->Connected = false;
		return false;
	}
}

__declspec(dllexport) bool __cdecl Connected()
{
	return RobotInfo->Connected;
}

__declspec(dllexport) void __cdecl cleanup()
{
	RobotInfo->Connected = false;
	delete RobotInfo;
	delete g_pSocket;
}

__declspec(dllexport) bool __cdecl IsArmed()
{
	return RobotInfo->Armed;
}

__declspec(dllexport) bool __cdecl Reconnect()
{
	n_destroy_socket(SocketID);
	SocketID = n_create_socket(pType::TCP);

	if (n_connect_socket(SocketID, host, port))
	{
		RobotInfo->Connected = true;
	}
	else
	{
		RobotInfo->Connected = false;
	}
	return Connected();
}

__declspec(dllexport) double __cdecl GetVoltage()
{
	return RobotInfo->Voltage;
}

__declspec(dllexport) double __cdecl GetTemperature()
{
	return RobotInfo->Temperature;
}

__declspec(dllexport) bool __cdecl GetMotor1Fault()
{
	return RobotInfo->MotorFault[0];
}

__declspec(dllexport) bool __cdecl GetMotor2Fault()
{
	return RobotInfo->MotorFault[1];
}

__declspec(dllexport) unsigned short __cdecl GetMotor1Current()
{
	return RobotInfo->MotorCurrent[0];
}

__declspec(dllexport) unsigned short __cdecl GetMotor2Current()
{
	return RobotInfo->MotorCurrent[1];
}

__declspec(dllexport) ULONGLONG __cdecl GetLastReportedTimeAlive()
{
	return RobotInfo->last_time_reported_alive;
}

__declspec(dllexport) bool __cdecl UnArm()
{
	return packetinfo.Send(SocketID, Packet::UN_ARM, 5000, 3333, true);
}

__declspec(dllexport) bool __cdecl Arm()
{
	return packetinfo.Send(SocketID, Packet::arm_motorshield, Packet::unlock, Packet::unlock, Packet::unlock, Packet::unlock);
}

__declspec(dllexport) bool __cdecl SetSpeed(short speed_m1, short speed_m2)
{
	return packetinfo.Send(SocketID, Packet::set_speed, speed_m1, speed_m2, true);
}

__declspec(dllexport) bool __cdecl SetBrake(short brake_m1, short brake_m2)
{
	return packetinfo.Send(SocketID, Packet::set_brakes, brake_m1, brake_m2, true);
}

#else

//-71,5 K = 22*C
//-7,0 K = 100 *C
//(2216960-ReadVal()*2165)/ReadVal()=R
//T=3278.1786619/ (log/*natural*/(((2216960-(packetinfo.GetValueOne(true)*2165))/packetinfo.GetValueOne(true))/1.0671406)

//1 Val = 0,01439620644493177387914230019493 V
int main()
{
	Packet packetinfo;
	Packet alive(Packet::I_AM_ALIVE, Packet::I_AM_ALIVE, Packet::I_AM_ALIVE, Packet::I_AM_ALIVE, Packet::I_AM_ALIVE);
	ULONGLONG time_last_alive_sent = 0;
	ULONGLONG timenow;

	bool armed = false;
	bool connected = false;
	g_pSocket = new CSocket();
	int SocketID = n_create_socket(pType::TCP);
	std::string host("192.168.2.222");
	int port = 1001;
	if (n_connect_socket(SocketID, host, port))
	{
		connected = true;
		std::cout << "CONNECTED\r\n" << std::flush;
	}
	else
	{
		std::cout << "Cannot connect, will retry\r\n" << std::flush;
	}
	std::cout << "Use [L_CTRL + Q] to exit\r\n" << std::flush;
	while ((GetAsyncKeyState(VK_LCONTROL) && GetAsyncKeyState('Q')) == 0)
	{
		if (connected)
		{
			timenow = GetTickCount64();
			if (armed && (timenow - time_last_alive_sent) > 200)
			{
				time_last_alive_sent = timenow;
				if (!alive.Send(SocketID))
				{
					std::cout << "!!!ALIVE!!! Send error\r\n" << std::flush;
				}
			}
			if (!onRemoteDisconnect.empty()) {
				remoteDisconnect tempData = onRemoteDisconnect.front();
				{
					//someone disconnected
					std::cout << "Someone disconnected closed\r\n" << std::flush;
				}
				onRemoteDisconnect.pop();
			}
			if (!onSocketAnswer.empty()) {
				socketAnswer tempData = onSocketAnswer.front();
				{
					packetinfo.set(tempData.data, 7);
					switch (packetinfo.GetAction())
					{
					case Packet::get_milliamps:
						{
							std::cout << "Current stats:";

							std::cout << "\r\n\tMotor 1: " << std::dec <</*_byteswap_ushort*/((unsigned short)packetinfo.GetValueOne()) << " mA";
							std::cout << "\r\n\tMotor 2: " << std::dec <</*_byteswap_ushort*/((unsigned short)packetinfo.GetValueTwo()) << " mA";

							std::cout << "\r\n" << std::flush;
							break;
						}
					case Packet::get_fault:
						{
							std::cout << "Fault stats:";

							std::cout << "\r\n\tMotor 1: " << (packetinfo.GetB1() != 0);
							std::cout << "\r\n\tMotor 2: " << (packetinfo.GetB2() != 0);

							std::cout << "\r\n" << std::flush;
							break;
						}
					case Packet::get_fault_1:
					{
						std::cout << "M1 Fault: " << (packetinfo.GetValueOne(true) != 0) << "\r\n" << std::flush;
						break;
					}
					case Packet::get_fault_2:
					{
						std::cout << "M2 Fault: " << (packetinfo.GetValueOne(true) != 0) << "\r\n" << std::flush;
						break;
					}
					case Packet::CONNECTION_OK:
						{
							std::cout << "Connection is still alive!!\r\n" << std::flush;
							break;
						}
					case Packet::GET_BATTERY_VOLTAGE:
						{
							double Voltage = (packetinfo.GetValueOne(true) / 72.013093289689034369885433715221);
							std::cout << std::dec << "Voltage: " << Voltage << " V\r\n" << std::flush;
							break;
						}
					case Packet::GET_TEMP:
						{
							double mVout = (((double)packetinfo.GetValueOne(true)) / 1023.0) * 5000.0;
							double Temp = (mVout - 400.0) / 19.5; //Ta = (Vout-400mV)/19.5mV //Original
							std::cout << std::dec << "Temperature: " << Temp << " *C (" << packetinfo.GetValueOne(true) << ")\r\n" << std::flush;
							break;
						}
					case Packet::echo_data:
						{
							std::cout << "ECHO:";

							std::cout << "\r\n\tValue_One: " << /*_byteswap_ushort*/(packetinfo.GetValueOne());
							std::cout << "\r\n\tValue_Two: " << /*_byteswap_ushort*/(packetinfo.GetValueTwo());

							std::cout << "\r\n\tS_ENABLED Value_One: " << /*_byteswap_ushort*/(packetinfo.GetValueOne(true));
							std::cout << "\r\n\tS_ENABLED Value_Two: " << /*_byteswap_ushort*/(packetinfo.GetValueTwo(true));

							std::cout << "\r\n\ta: " << /*_byteswap_ushort*/(int)(packetinfo.GetB1());
							std::cout << "\r\n\tb: " << /*_byteswap_ushort*/(int)(packetinfo.GetB2());
							std::cout << "\r\n\tc: " << /*_byteswap_ushort*/(int)(packetinfo.GetB3());
							std::cout << "\r\n\td: " << /*_byteswap_ushort*/(int)(packetinfo.GetB4());

							std::cout << "\r\n" << std::flush;
							break;
						}
					case Packet::Arduino_Armed:
						{
							armed = true;
							break;
						}
					case Packet::Arduino_Unarmed:
						{
							armed = false;
							break;
						}
						default:
						{
							std::cout << "Unknown packet received\r\n" << std::flush;
							std::cout << "DATA: [" << std::hex << (int)packetinfo.get()[0] << "|"
								<< std::hex << (int)packetinfo.get()[1] << "|"
								<< std::hex << (int)packetinfo.get()[2] << "|"
								<< std::hex << (int)packetinfo.get()[3] << "|"
								<< std::hex << (int)packetinfo.get()[4] << "|"
								<< std::hex << (int)packetinfo.get()[5] << "|"
								<< std::hex << (int)packetinfo.get()[6] << "]" << "\r\n" << std::flush;
							break;
						}
					}
					//connection answered
					/*std::cout << "Answered: \r\n";
					std::cout << "DATA: [" << std::hex << (UINT8)tempData.data[0] << "|"
						<< std::hex << (UINT8)tempData.data[1] << "|"
						<< std::hex << (UINT8)tempData.data[2] << "|"
						<< std::hex << (UINT8)tempData.data[3] << "|"
						<< std::hex << (UINT8)tempData.data[4] << "|"
						<< std::hex << (UINT8)tempData.data[5] << "|"
						<< std::hex << (UINT8)tempData.data[6] << "]" << "\r\n" << std::flush;*/
				}
				free(tempData.data);
				onSocketAnswer.pop();
			}
			if (!onSocketClose.empty()) {
				socketClose tempData = onSocketClose.front();
				connected = false;
				std::cout << "Socket closed\r\n" << std::flush;
				onSocketClose.pop();
				SocketID = n_create_socket(pType::TCP);
				continue;
			}
			{
				static int speeds = 20;
				static bool back = false;
				if (GetAsyncKeyState('W') & 1)
				{
					back = false;
					if (packetinfo.Send(SocketID, Packet::set_speed, speeds, speeds, true))
					{
						std::cout << "Send OK\r\n" << std::flush;
					}
				}
				if (GetAsyncKeyState('V') & 1)
				{
					if (packetinfo.Send(SocketID, Packet::GET_BATTERY_VOLTAGE, 0, 0))
					{
						std::cout << "Send OK\r\n" << std::flush;
					}
				}
				if (GetAsyncKeyState('T') & 1)
				{
					if (packetinfo.Send(SocketID, Packet::GET_TEMP, 0, 0))
					{
						std::cout << "Send OK\r\n" << std::flush;
					}
				}
				if (GetAsyncKeyState('S') & 1)
				{
					speeds = 1;
					if (packetinfo.Send(SocketID, Packet::set_brakes, 400, 400, true))
					{
						std::cout << "Send OK\r\n" << std::flush;
					}
				}
				if (GetAsyncKeyState('Z') & 1)
				{
					back = true;
					if (packetinfo.Send(SocketID, Packet::set_speed, -speeds, -speeds, true))
					{
						std::cout << "Send OK\r\n" << std::flush;
					}
				}
				if (GetAsyncKeyState('A') & 1)
				{
					if (packetinfo.Send(SocketID, Packet::set_speed, -speeds, speeds, true))
					{
						std::cout << "Send OK\r\n" << std::flush;
					}
				}
				if (GetAsyncKeyState('D') & 1)
				{
					if (packetinfo.Send(SocketID, Packet::set_speed, speeds, -speeds, true))
					{
						std::cout << "Send OK\r\n" << std::flush;
					}
				}
				if (GetAsyncKeyState('R') & 1)
				{
					if (packetinfo.Send(SocketID, Packet::set_speed, 0, 0, 0, 0))
					{
						std::cout << "Send OK\r\n" << std::flush;
					}
				}
				if (GetAsyncKeyState('C') & 1)
				{
					if (packetinfo.Send(SocketID, Packet::get_milliamps, 0, 0, 0, 0))
					{
						std::cout << "Send OK\r\n" << std::flush;
					}
				}
				if (GetAsyncKeyState('F') & 1)
				{
					packetinfo.Send(SocketID, Packet::get_fault_1, 0, 0, 0, 0);
					if (packetinfo.Send(SocketID, Packet::get_fault_2, 0, 0, 0, 0))
					{
						std::cout << "Send OK\r\n" << std::flush;
					}
				}
				if (GetAsyncKeyState('U') & 1)
				{
					if (packetinfo.Send(SocketID, Packet::UN_ARM, 5000, 3333, true))
					{
						std::cout << "Send OK\r\n" << std::flush;
					}
				}
				if (GetAsyncKeyState('Y') & 1)
				{
					packetinfo.set(Packet::arm_motorshield, Packet::unlock, Packet::unlock, Packet::unlock, Packet::unlock);
					if (packetinfo.Send(SocketID))
					{
						std::cout << "Send OK\r\n" << std::flush;
					}
				}
				if (GetAsyncKeyState('H') & 1)
				{
					if (packetinfo.Send(SocketID, Packet::echo_data, 1, 2, 3, 4))
					{
						std::cout << "Send OK\r\n" << std::flush;
					}
				}
				if (GetAsyncKeyState('B') & 1)
				{
					if (packetinfo.Send(SocketID, Packet::TEST, 0, 0, 0, 0))
					{
						std::cout << "Send OK\r\n" << std::flush;
					}
				}
				if (GetAsyncKeyState('E') & 1)
				{
					speeds += 10;
					if (back)
					{
						if (packetinfo.Send(SocketID, Packet::set_speed, -speeds, -speeds, true))
						{
							std::cout << "Send OK\r\n" << std::flush;
						}
					}
					else
					{
						if (packetinfo.Send(SocketID, Packet::set_speed, speeds, speeds, true))
						{
							std::cout << "Send OK\r\n" << std::flush;
						}
					}
				}
				if (GetAsyncKeyState('X') & 1)
				{
					speeds -= 10;
					if (back)
					{
						if (packetinfo.Send(SocketID, Packet::set_speed, -speeds, -speeds, true))
						{
							std::cout << "Send OK\r\n" << std::flush;
						}
					}
					else
					{
						if (packetinfo.Send(SocketID, Packet::set_speed, speeds, speeds, true))
						{
							std::cout << "Send OK\r\n" << std::flush;
						}
					}
				}
			}
		}
		else
		{
			if (n_connect_socket(SocketID, host, port))
			{
				connected = true;
				std::cout << "CONNECTED\r\n" << std::flush;
			}
			else
			{
				connected = false;
			}
		}
		Sleep(1);
	}
	delete g_pSocket;
	return 0;
}
#endif