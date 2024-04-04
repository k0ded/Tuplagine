#pragma once
#include <imgui.h>
#include <vector>
#include <string>
#include <memory>

enum class PrintStructure
{
	Normal,
	Compact,
	Log,
	Warning,
	Error,
	Assert
};
enum class MessageType
{
	Log,
	Warning,
	Error,
	Assert
};
struct Message
{
	std::string message;
	MessageType type;
	float time;
};

class Console
{
public:
	Console();
	~Console();

	void Log(const char* message);
	void Warning(const char* message);
	void Error(const char* message);
	void Assert(bool condition, const char* message);

private:
	PrintStructure myPrintStructure = PrintStructure::Normal;

	static inline std::vector<std::shared_ptr<Message>> myMessages;
	std::vector<std::shared_ptr<Message>> myLogs;
	std::vector<std::shared_ptr<Message>> myWarnings;
	std::vector<std::shared_ptr<Message>> myErrors;
	std::vector<std::shared_ptr<Message>> myAsserts;
};


// Can see logs warnings and errors.
// Can assert to act like breakpoints.