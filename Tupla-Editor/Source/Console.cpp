#include "Console.h"
#include <iostream>
#include "CommonUtilities/Time/Time.h"

Console::Console()
{
	ImGui::Begin("Console");

	Log("Hello World");
	 
	// Print Logs
	switch (myPrintStructure)
	{
	case PrintStructure::Normal:
	{
		// Print each message
		for (int i = 0; i < myMessages.size(); i++)
		{
			std::string print = myMessages[i]->message;

			switch (myMessages[i]->type)
			{
			case MessageType::Log:
			{
				std::string type = "(Log) ";
				print = type + print;
			}
				break;
			case MessageType::Warning:
			{
				std::string type = "(Warning) ";
				print = type + print;
			}
				break;
			case MessageType::Error:
			{
				std::string type = "(Error) ";
				print = type + print;
			}
				break;
			case MessageType::Assert:
			{
				std::string type = "(Assert) ";
				print = type + print;
			}
				break;
			default:
				break;
			}


			int hour = {};
			int minute = {};
			int second = {};
			int millisecond = {};
			float time = myMessages[i]->time;
			while (time > 0)
			{
				if (time > 3600)
				{
					hour++;
					time -= 3600;
				}
				else if (time > 60)
				{
					minute++;
					time -= 60;
				}
				else if (time > 1)
				{
					second++;
					time -= 1;
				}
				else
				{
					millisecond = time * 1000;
					time = 0;
				}
			}

			std::string printTime;
			printTime += " (" + std::to_string(hour) + ":" + std::to_string(minute) + ":" + std::to_string(second) + "." + std::to_string(millisecond) + " ms)";

			print += printTime;

			ImGui::Text(print.c_str());
		}
	}
	break;
	case PrintStructure::Compact:
	{
		// Print all messages but if they are a douplet of another add a counter instead
	}
	break;
	case PrintStructure::Log:
	{
		// Print only Logs
	}
	break;
	case PrintStructure::Warning:
	{
		// Print only Warnings
	}
	break;
	case PrintStructure::Error:
	{
		// Print only Errors
	}
	break;
	case PrintStructure::Assert:
	{
		// Print only Asserts
	}
	break;
	default:
		break;
	};
}

Console::~Console()
{
	ImGui::End();
}

void Console::Log(const char* message)
{
	std::shared_ptr<Message> msg;
	msg = std::make_shared<Message>();
	msg->message = message;
	msg->type = MessageType::Log;
	msg->time = Time::TotalTime();

	myLogs.push_back(msg);
	myMessages.push_back(msg);
}

void Console::Warning(const char* message)
{
	std::shared_ptr<Message> msg;
	msg = std::make_shared<Message>();
	msg->message = message;
	msg->type = MessageType::Warning;
	msg->time = Time::TotalTime();

	myWarnings.push_back(msg);
	myMessages.push_back(msg);
}

void Console::Error(const char* message)
{
	std::shared_ptr<Message> msg;
	msg = std::make_shared<Message>();
	msg->message = message;
	msg->type = MessageType::Error;
	msg->time = Time::TotalTime();

	myErrors.push_back(msg);
	myMessages.push_back(msg);
}

void Console::Assert(bool condition, const char* message)
{
	if (!condition)
	{
		std::shared_ptr<Message> msg;
		msg = std::make_shared<Message>();
		msg->message = message;
		msg->type = MessageType::Assert;
		msg->time = Time::TotalTime();

		myAsserts.push_back(msg);
		myMessages.push_back(msg);
	}
}
