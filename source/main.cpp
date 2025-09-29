#define APAD_DEBUG

#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include "apad_array.h"
#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_file.h"
#include "apad_string.h"
#include "apad_win32.h"

const ui8 MaxGroups = 5;

#define Log(_string) printf("%s\n", _string)
#define LogError(_string) printf("ERROR - %s\n", _string)

// @EXPORT_API apad_string.cpp
bool IsLetter(char c) {
	return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';
}

// @EXPORT_API apad_string.cpp
bool IsNumber(char c) {
	return c >= '0' && c <= '9';
}

// @EXPORT_API apad_string.cpp
bool IsValidChar(char c) {
  return IsLetter(c) == true || IsNumber(c) == true || c == '\"' || c == '/' || c == '-' || c == '?' || c == '!' || c == '#';
}

// @EXPORT_API apad_string.cpp
#include <stdlib.h>
si32 StringToInt(const char* string) {
  AssertRet(string, Null);
  return atoi(string);
}

// @EXPORT_API
struct date {
  ui8  dayOfTheWeek; // 1 -> 7
  ui8  day; 				 // 1 -> 31
  ui8  month; 			 // 1 -> 12
  ui16 year;
};

// @EXPORT_API
#include <time.h>
date GetCurrentDate(si32 offsetDays) {
	time_t timeNowSecs = time(NULL) + offsetDays * 24 * 60 * 60;
	auto*  timeNow = localtime(&timeNowSecs); // UTC time
	
	date ret = {};
	ret.dayOfTheWeek = timeNow->tm_wday == 0 ? 7 : timeNow->tm_wday;
	ret.year = 1900 + timeNow->tm_year;
	ret.month = 1 + timeNow->tm_mon;
	ret.day = timeNow->tm_mday;
	return ret;
}

void PrintSingleTask(const char* id, const char* task, const char* dateAdded, const char* dateDue, const char* reschedulePeriod, const char* flag, const char** groups) {
  // @TODO - Add assertions once program takes shape
	// AssertRet(id != Null);
	// AssertRet(task != Null);
	// AssertRet(dateAdded != Null);
	// AssertRet(dateDue != Null);
	// AssertRet(reschedulePeriod	!= Null);
	// AssertRet(flag != Null);
	// AssertRet(groups != Null);
	
	printf("ID:         %s\n", id);
	printf("String:     %s\n", task);
	printf("Date added: %s\n", dateAdded);
	printf("Date due:   %s\n", dateDue);
	printf("Reschedule: %s\n", reschedulePeriod);
	printf("Flag:       %s\n", flag);
	if(groups[0] != Null) { // Check if any groups present
		printf("Groups:     %s\n", groups[0]);
		FromTo(1, MaxGroups) {
			if(groups[it] != Null)
				printf("            %s\n", groups[it]);
		}
	}
}

// Storage format
// id(8 bit unsigned) task_text(string) date_added(dd/mm/yyyy) date_due_by(dd/mm/yyyy | -) reschedule_data(days | -) flag(! | ? | @ | -) groups(#id1 #id2 ... #id5)\r\n

ConsoleAppEntryPoint(args, argsCount) {
	Log("\n"); // Insert blank line for clarity
	
	#ifdef APAD_DEBUG
		#if 1
		args[1] = "add";
		args[2] = "hello";
		args[3] = "+61";
		args[4] = "+2";
		argsCount = 5;	
		#endif
	#endif
	
	#ifdef APAD_DEBUG
	const char* dataPath = "../../data/calendar.txt";
	#else
	const char* dataPath = "data/calendar.txt";
	#endif 
	
	// Check existance of calendar.txt
	if(FileExists(dataPath) == false) {
		LogError("Couldn't find data/calendar.txt\n");
		goto program_exit;
	}
	
	// Load calendar.txt
	auto calendar = LoadFile(dataPath);
	if(ErrorIsSet() == true) {
		LogError("Couldn't load data/calendar.txt");
		goto program_exit;
	}

	// Parse command line arguments
	if(argsCount < 2) {
		LogError("No commands supplied.\n");
		goto usage_msg;
	}
		
	// @TODO - Log file for the day
	// 			   - Timestamp, ID, list of changes	
		
	const char* command = args[1];
	if(StringsAreEqual(command, "add") == true) { // @TODO
	  // @TODO - If error, goto usage_msg
		
		const char* id = "0"; // @TODO - Generate
		const char* task = Null;
		const char* dateAdded = Null; // @TODO - Add, import from API when available
					char  dateDue[] = "dd/mm/yyyy\0";
		const char* reschedulePeriod = Null;
		const char* flag = Null;
		const char* groups[MaxGroups] = { Null };
		
		// Scan through remaining arguments
		FromTo(2, argsCount) {
			const char* arg = args[it];
			
			if(arg[0] == '+' && dateDue[0] != 'd') { // Reschedule period
				const char* number = arg + 1;
				si32 				i = StringToInt(number);
				reschedulePeriod = number;
			}
			else if( // Date due
			        IsNumber(arg[0]) == true || arg[0] == '.' || arg[0] == '+' || 
			        StringsAreEqual(arg, "mon") || StringsAreEqual(arg, "tue") || StringsAreEqual(arg, "wed") || StringsAreEqual(arg, "thu") || 
							StringsAreEqual(arg, "fri") || StringsAreEqual(arg, "sat") || StringsAreEqual(arg, "sun")) 
			{
				if(IsNumber(arg[0]) == true) { // Assumed dd/mm/yy or dd/mm/yyyy format. Convert to dd/mm/yyyy if needed
				}
				else if(arg[0] == '+') { // Offset from today
				  const char* number = arg + 1;
					si32 				i = StringToInt(number);
					
					if(i > 60) {
						dateDue[0] = '>';
						dateDue[1] = '6';
						dateDue[2] = '0';
						dateDue[3] = '\0';
					}
					else {
						auto date = GetCurrentDate(i);
						
						auto temp = ToString(date.day);
						if(date.day <= 9) {
							dateDue[0] = '0';
							dateDue[1] = temp.string[0];
						}
						else {
							dateDue[0] = temp.string[0];
							dateDue[1] = temp.string[1];
						}
						
						temp = ToString(date.month);
						if(date.month <= 9) {
							dateDue[3] = '0';
							dateDue[4] = temp.string[0];
						}
						else {
							dateDue[3] = temp.string[0];
							dateDue[4] = temp.string[1];
						}
						
						temp = ToString(date.year);
						dateDue[6] = temp.string[0];
						dateDue[7] = temp.string[1];
						dateDue[8] = temp.string[2];
						dateDue[9] = temp.string[3];
					}
				}
				else { // Assumed to be a day of the week @TODO
				}
				
			}
			else if(arg[0] == '!' || arg[0] == '?' || arg[0] == '@') { // Flag
				flag = arg;
			}
			else if(arg[0] == '#') { // Group
				ForAll(MaxGroups) {
					if(groups[it] == Null) {
						groups[it] = arg;
						break;
					}
				}
			}
			else // If none of the above it is considered to be an entry string. The " is not carried as part of the argument
				task = arg;
		}
		
		if(task == Null) {
			LogError("No string specified.\n");
			goto usage_msg;
		}
		
	  Log("Task added\n");
		
		PrintSingleTask(id, task, dateAdded, dateDue, reschedulePeriod, flag, groups);
		
		// @TODO - Store in calendar.txt
		
		goto program_exit;
	}
	else if(StringsAreEqual(command, "list") == true) {
	  // @TODO
		// By string, ID, flags & groups, - means not
		// <60 days & >60 days
		// Automatically list preempt tasks with a date <30 days when listing priority tasks?
		
		// @TODO - Parse calendar entries and display
		// id(8 bit unsigned) task_text(string) date_added(dd/mm/yyyy) date_due_by(dd/mm/yyyy | -) reschedule_data(days | -) flag(! | ? | @) groups(#id1 #id2 ... #id5)
		
		// @TODO - search / list filters - view only task, id, all tasks from #group, etc
		// 			 	- E.g. view only the groups of tasks due by x date
		
		char*       data = (char*)calendar.memory;
		const char* id = Null;
		const char* task = Null;
		const char* dateAdded = Null;
		const char* dateDue = Null;
		const char* reschedulePeriod = Null;
		const char* flag = Null;
		const char* groups[MaxGroups] = { Null };
		
		const char* toStore = Null; // Temp string
		bool        scanningTaskString = false;
		ForAll(calendar.size) {
			char c = data[it];
			
			if(c == '\n') { // Reset task data
			  // @TODO - Finish task printing
				PrintSingleTask(id, task, dateAdded, dateDue, reschedulePeriod, flag, groups);
				
				id = Null;
				task = Null;
				dateAdded = Null;
				dateDue = Null;
				reschedulePeriod = Null;
				flag = Null;
				
				ForAll(MaxGroups)
					groups[it] = Null;
					
				Log("\n");
			}
			else if(toStore == Null && IsValidChar(c) == true) {
				if(c == '\"') { // Beginning of the task string
					toStore = data + it + 1;
					scanningTaskString = true;
				}
				else
					toStore = data + it;
			}
			else if(scanningTaskString == true && c == '\"') { // Finish scanning task string
			  data[it] = '\0';
				task = toStore;
				toStore = Null;
				scanningTaskString = false;
			}
			else if(scanningTaskString == false && toStore != Null && IsValidChar(c) == false) { // Finish scanning other data
				data[it] = '\0';
				
				if(id == Null)
					id = toStore;
				else if(dateAdded == Null)
					dateAdded = toStore;
				else if(dateDue == Null)
					dateDue = toStore;
				else if(reschedulePeriod == Null)
					reschedulePeriod = toStore;
				else if(flag == Null)
					flag = toStore;
				else if(task != Null) {
					ForAll(MaxGroups) {
						if(groups[it] == Null) {
							groups[it] = toStore;
							break;
						}
					}
				}
				
				toStore = Null;
			}
		}
	}
	else if(StringsAreEqual(command, "delete") == true) { // @TODO
	}
	else if(StringsAreEqual(command, "modify") == true) { // @TODO
		// @TODO
		// Print the details which have been updated, followed by the entire task data
		// - E.g. Task ID / flag / group updated
		// - Keep it simple
		// When making any changes to an entry, display the updated portion of info 
		// before and after, then display the updated entry will all info

	}
	else if(StringsAreEqual(command, "reschedule") == true) { // @TODO
	}
	else if(StringsAreEqual(command, "undo") == true) { // @TODO
	}
	else if(StringsAreEqual(command, "redo") == true) { // @TODO
	}
	
	// @TODO - Parse file contents into a table / list of tasks and deadlines
	
	// @TODO - Save to data/calendar.txt
	// @TODO - Daily local backup at the start of the day
	// 			   - Limit to 10 max previous logs
	//				 - Store with current date
	//

  usage_msg:
	Log("Usage: calendar [add] [list] [delete | del] [modify | mod] [reschedule | res] [undo] [redo]\n");
			
	program_exit:
	if(IsValid(calendar) == true)
		FreeFile(calendar);
	
	return 0;
}