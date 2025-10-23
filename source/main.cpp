#define APAD_DEBUG

#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include "apad_array.h"
#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_file.h"
#include "apad_intrinsics.h"
#include "apad_logging.h"
#include "apad_maths.h"
#include "apad_string.h"
#include "apad_time.h"
#include "apad_win32.h"

const char* DateFormatShort = "dd/mm";
const char* DateFormatMedium = "dd/mm/yy";
const char* DateFormatLong = "dd/mm/yyyy";
const ui8 	MaxDateLength = 11; // DateFormatLong length + \0
const ui8 	MaxGroups = 5;

#include "helpers.cpp"

// Storage format
// id(8 bit unsigned) task_text(string) date_added(dd/mm/yyyy) date_due_by(dd/mm/yyyy | -) reschedule_data(days | -) flag(! | ? | @ | -) groups(#id1 #id2 ... #id5)\r\n
ConsoleAppEntryPoint(args, argsCount) {
	auto logFile = OpenLogFile();
	
	Log(logFile, "\n"); // Insert blank line for clarity
	
	#ifdef APAD_DEBUG
		#if 0
		args[1] = "add";
		args[2] = "new task";
		args[3] = "mond";
		// args[4] = "+2";
		argsCount = 4;	
		#endif
	#endif
	
	#ifdef APAD_DEBUG
	const char* dataPath = "../../data/calendar.txt";
	#else
	const char* dataPath = "data/calendar.txt";
	#endif 
	
	if(FileExists(dataPath) == false) {
		Log(logFile, "ERROR - Couldn't find data/calendar.txt\n");
		goto program_exit;
	}
	
	auto calendar = LoadFile(dataPath);
	if(ErrorIsSet() == true) {
		Log(logFile, "ERROR - Couldn't load data/calendar.txt");
		goto program_exit;
	}

	if(argsCount < 2) {
		Log(logFile, "ERROR - No commands supplied.\n");
		goto usage_msg;
	}
	
	// Parse arguments and check their validities
	const char* command = args[1];
	const char* taskString = Null;
	const char* reschedulePeriod = Null;
	const char* flag = Null;
	const char* groups[MaxGroups] = { Null };
				char  targetDate[MaxDateLength] = "-\0";
	
	FromTo(2, argsCount) {
		const char* arg = args[it];
					auto  argLength = GetStringLength(arg, false);
		
		if(arg[0] == '.') { // Set target date to today
		  auto today = GetDate(0);
			auto string = DateToString(today);
			CopyString(string, -1, targetDate, MaxDateLength, false);
		}
		else if(arg[0] == '+') { // Day offset from today
		  const char* daysString = arg + 1;
			
		  // Determine validity and whether work days have been specified
			bool isValid = true;
			bool workDays = false;
			{
				const ui8 MaxDigits = 3;
				
				char* workDaysSub = (char*)FindSubstring("w", daysString);
				if(workDaysSub != Null) {
					workDays = true;
					*workDaysSub = '\0';
				}
				
				auto length = GetStringLength(daysString, false);
				if(length == 0 || length > MaxDigits)
					isValid = false;
				
				ForAll(length) {
					if(IsNumber(daysString[it]) == false)
						isValid = false;
				}
			}
		  if(isValid == false) {
				Log(logFile, "ERROR - Invalid day offset (max length allowed is 3)\n");
			  goto program_exit;
			}
			
			ui16 calendarDays = 0;
			{
				si32 days = StringToInt(daysString);
				if(workDays == true) {
					ForAll(days) {
						calendarDays += 1;
						while(GetDate(calendarDays).dayOfTheWeek >= 6) // Weekend
							calendarDays += 1;
					}
				}
				else
					calendarDays = days;
			}				
			
			if(targetDate[0] != '-') // Reschedule period @TODO - Allow work days
				reschedulePeriod = daysString;
			else { // Date due
			  if(calendarDays > 60)
					CopyString(">60", -1, targetDate, MaxDateLength, true);
				else {
					auto string = DateToString(GetDate(calendarDays));
					CopyString(string, -1, targetDate, MaxDateLength, true);
				}
			}
		} 
		else if( // Specific next day of the week
						StringsAreEqual(arg, "mon") == true || StringsAreEqual(arg, "tue") == true || StringsAreEqual(arg, "wed") == true || StringsAreEqual(arg, "thu") == true || 
						StringsAreEqual(arg, "fri") == true || StringsAreEqual(arg, "sat") == true || StringsAreEqual(arg, "sun") == true)
		{
			ui8 argDay = 0; // 1 -> 7 to match the date struct
			{
				const char* days[] = { "mon", "tue", "wed", "thu", "fri", "sat", "sun" };
				ForAll(7) {
					if(StringsAreEqual(arg, days[it]) == true) {
						argDay = it + 1;
						break;
					}
				}
			}
			
			si8 offset = argDay - GetDate(0).dayOfTheWeek;
			if(offset < 0)
				offset += 7;
			
			auto string = DateToString(GetDate(offset));
			CopyString(string, -1, targetDate, MaxDateLength, false);
		}
		else if( // Target date specified in short, medium or long format
						IsNumber(arg[0]) == true && (
						(argLength == GetStringLength(DateFormatShort, false) && arg[2] == '/') || 
						(argLength == GetStringLength(DateFormatMedium, false) && arg[2] == '/' && arg[5] == '/') || 
						((argLength + 1) == MaxDateLength && arg[2] == '/' && arg[5] == '/') ))
		{
			auto argLength = GetStringLength(arg, false);
			
			ui16 year = Null;
			{
				// If these both are false, the format is dd/mm
				bool hasShortYear = argLength == GetStringLength(DateFormatMedium, false);
				bool hasLongYear = argLength == GetStringLength(DateFormatLong, false);
				
				// Copy into temp string omitting the forward slashes
				char tempString[10] = { '\0' }; // Length of 10 to accomodate dd/mm/yyyy
				ForAll(argLength) {
					if(arg[it] != '/')
						tempString[it] = arg[it];
				}
				
				ui8 day = StringToInt(tempString);
				ui8 month = StringToInt(tempString + 3);
				
				if(hasShortYear == true)
					year = 2000 + StringToInt(arg + 6);
				else if(hasLongYear == true)
					year = StringToInt(arg + 6);
				else 
					year = GetDate(0).year;
				
				// Sanity check
				// @TODO - Check whether the date is real, e.g. 30th of feb
				if(day < 0 || day > 31 || month < 0 || month > 12 || year < GetDate(0).year) {
					Log(logFile, "ERROR - Invalid target date\n");
					goto usage_msg; // @TODO - More specific message?
				}
			}
			
			// Create target date
			{
				targetDate[0] = arg[0];
				targetDate[1] = arg[1];
				targetDate[2] = '/';
				targetDate[3] = arg[3];
				targetDate[4] = arg[4];
				targetDate[5] = '/';
				auto string = ToString(year);
				ForAll(4)
					targetDate[it + 6] = string[it];
			}
		}
		else if(argLength == 1 && (arg[0] == '!' || arg[0] == '?' || arg[0] == '@')) // Flag
			flag = arg;
		else if(arg[0] == '#') { // Group
			ForAll(MaxGroups) {
				if(groups[it] == Null) {
					groups[it] = arg;
					break;
				}
			}
		}
		else if(taskString == Null) // If none of the above it is considered to be an entry string only if Null. The " is not carried as part of the argument.
			taskString = arg;
		else { // Invalid argument
			printf("ERROR: Invalid argument: %s\n\n", arg);
			goto usage_msg;
		}
	}		
	
	// Check minimum required arguments have been supplied
	// Command validity checked further down
	if(taskString == Null) {
		Log(logFile, "ERROR - No task string specified.\n");
		goto usage_msg;
	}
	else if(targetDate[0] == '-'){
		Log(logFile, "ERROR - No target date specified.\n");
		goto usage_msg;
	}
		
	// @TODO - Log file for the day
	// 			   - Timestamp, ID, list of changes	
	
	// Parse command, output error message if invalid
	if(StringsAreEqual(command, "add") == true) {
	  char dateAdded[] = "dd/mm/yyyy\0";
		{
			auto date = GetDate(Null);
			auto string = DateToString(date);
			auto length = GetStringLength(dateAdded, false);
			ForAll(length)
				dateAdded[it] = string[it];
		}
		
		Log(logFile, "Task added\n");
		
		PrintDetailedTask(Null, taskString, dateAdded, targetDate, reschedulePeriod, flag, groups);
		
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
		
		// @TODO - Specify which info columns are wanted - -da (date added) -dd (date due) etc ?
		
		// Scan through remaining arguments
		bool printDetailed = false;
		FromTo(2, argsCount) {
			const char* arg = args[it];
			if(StringsAreEqual(arg, "-d") == true)
				printDetailed = true;
		}
		
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
				if(printDetailed == true)
					PrintDetailedTask(id, task, dateAdded, dateDue, reschedulePeriod, flag, groups);
				else
					PrintTaskWide(id, task, dateAdded, dateDue, reschedulePeriod, flag, groups);
				
				id = Null;
				task = Null;
				dateAdded = Null;
				dateDue = Null;
				reschedulePeriod = Null;
				flag = Null;
				
				ForAll(MaxGroups)
					groups[it] = Null;
					
				Log(logFile, "\n");
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
				
				if(dateAdded == Null)
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
	else if(StringsAreEqual(command, "del") == true) { // Delete - @TODO
	}
	else if(StringsAreEqual(command, "mod") == true) { // Modify - @TODO
		// @TODO
		// Print the details which have been updated, followed by the entire task data
		// - E.g. Task ID / flag / group updated
		// - Keep it simple
		// When making any changes to an entry, display the updated portion of info 
		// before and after, then display the updated entry will all info

	}
	else if(StringsAreEqual(command, "resc") == true) { // Reschedule @TODO
	}
	else if(StringsAreEqual(command, "undo") == true) { // @TODO
	}
	else if(StringsAreEqual(command, "redo") == true) { // @TODO
	}
	else {
		Log(logFile, "ERROR - Invalid command supplied.\n");
		goto usage_msg;
	}
	
	goto program_exit;
	
	// @TODO - Parse file contents into a table / list of tasks and deadlines
	
	// @TODO - Save to data/calendar.txt
	// @TODO - Daily local backup at the start of the day
	// 			   - Limit to 10 max previous logs
	//				 - Store with current date
	//

  usage_msg:
	Log(logFile, "Usage: calendar [add] [list] [delete | del] [modify | mod] [reschedule | res] [undo] [redo]\n");
	// @TODO - Add specific messages for individual commands? E.g. how does the user know the right format for the dates?
	// @TODO - A git-like set of help messages for individual commands?
			
	program_exit:
	if(IsValid(calendar) == true)
		FreeFile(calendar);
	
	printf("%s\n", (const char*)logFile.memory);
	CloseLogFile(logFile);

	
	return 0;
}