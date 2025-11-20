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

			string DateFormatShort  = "dd/mm";
			string DateFormatMedium = "dd/mm/yy";
			string DateFormatLong   = "dd/mm/yyyy";
const ui8 	 MaxGroups = 5;

#include "helpers.cpp"

string 		ValidCommands[] = 	{ "add", "list", "del", "resc", "mod", "undo", "redo" };
BeginEnum(ValidCommandsIndex) { Add, List, Delete, Reschedule, Modify, Undo, Redo, Length } EndEnum(ValidCommandsIndex);

// Storage format
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
	string dataPath = "../../data/calendar.txt";
	#else
	string dataPath = "data/calendar.txt";
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
	
	// Data to be parsed from arguments
	string command;
	string taskString;
	string targetDate;
	string reschedulePeriod;
	string flag;
	string groups[MaxGroups];

	// Parse and check command
	command = args[1];
	{
		bool found = false;
		ForAll(ValidCommandsIndex::Length) {
			if(command == ValidCommands[it]) {
				found = true;
				break;
			}
		}
		
		if(found == false) {
			goto usage_msg;
			// @TODO - More specific error message
		}
	}
	
	// Parse remaining arguments
	FromTo(2, argsCount) {
		string arg = args[it];
		
		if(arg == "-s") { // Task string
			if(it + 1 < argsCount) {
				it += 1;
				taskString = args[it];
				continue;
			}
			else {
				printf("ERROR: Not enough arguments supplied\n\n", arg);
				goto usage_msg;
			}
		}
		else if(IsDate(arg) == true || (arg.length == 1 && arg[0] == '.') || (arg.length >= 2 && arg.length <= 4 && arg[0] == '+')) { // Date
      date d;
			
			if(arg[0] == '.')
				d = GetDate(0);
			else if(arg[0] == '+') {
				string daysString = arg + 1;
				
				// Determine validity and whether work days have been specified
				bool isValid = true;
				bool workDays = false;
				{
					const ui8 MaxDigits = 3;
					
					string workDaysSub = FindSubstring("w", daysString);
					if(workDaysSub.chars != Null) {
						workDays = true;
						workDaysSub = '\0';
					}
					
					if(daysString.length == 0 || daysString.length > MaxDigits)
						isValid = false;
					
					ForAll(daysString.length) {
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
				
				d = GetDate(calendarDays);
			}
			else { // Convert date to string
				
				
				
			}
		
			
			if(targetDate.length == 0)
				targetDate = arg;
			else {
				if(reschedulePeriod.length != 0) {
					printf("ERROR: Reschedule period already supplied\n\n", arg);
					goto usage_msg;
				}
				reschedulePeriod = arg;
			}
		}
		else if(arg.length == 1) {
			char c = arg[0];
			if(c == '.') {
				if(targetDate.length == 0)
					targetDate = DateToString(GetDate(0));
			  else {
					printf("ERROR: Target date already supplied\n\n", arg);
					goto usage_msg;
				}
			}
			else if(c == '!') {
			}
			else if(c == '?') {
			}
			else if(c == '@') {
			}
			else {
				printf("ERROR: Invalid flag supplied\n\n", arg);
				goto usage_msg;
			}
		}
		
		if(arg[0] == '.') { // Set target date to today
		  auto today = GetDate(0);
			auto string = DateToString(today);
			CopyString(string, -1, targetDate, targetDate.length, false);
		}
		else if(arg[0] == '+') { // Day offset from today
		  
		  				
			
			{ // Date due
			  
			}
		} 
		else if( // Specific next day of the week
						arg == "mon" || arg == "tue" || arg == "wed" || arg == "thu" || 
						arg == "fri" || arg == "sat" || arg == "sun")
		{
			
		}
		else if( // Target date specified in short, medium or long format
						IsNumber(arg[0]) == true && (
						(arg.length == DateFormatShort.length && arg[2] == '/') || 
						(arg.length == DateFormatMedium.length && arg[2] == '/' && arg[5] == '/') || 
						((arg.length + 1) == DateFormatLong.length && arg[2] == '/' && arg[5] == '/') ))
		{
			ui16 year = Null;
			{
				// If these both are false, the format is dd/mm
				bool hasShortYear = arg.length == DateFormatMedium.length;
				bool hasLongYear = arg.length == DateFormatLong.length;
				
				// Copy into temp string omitting the forward slashes
				string tempString = DateFormatLong;
				ForAll(arg.length) {
					if(arg[it] != '/')
						tempString[it] = arg[it];
					else
						tempString[it] = '\0';
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
		else if(arg.length == 1 && (arg[0] == '!' || arg[0] == '?' || arg[0] == '@')) // Flag
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
	
	// Parse command, output error message if invalid
	if(command == "add") {
	  string dateAdded = DateFormatLong;
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
	else if(command == "list") {
		#if 0
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
			if(arg == "-d")
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
		#endif
	}
	else if(command == "del") { // Delete - @TODO
	}
	else if(command == "mod") { // Modify - @TODO
		// @TODO
		// Print the details which have been updated, followed by the entire task data
		// - E.g. Task ID / flag / group updated
		// - Keep it simple
		// When making any changes to an entry, display the updated portion of info 
		// before and after, then display the updated entry will all info

	}
	else if(command == "resc") { // Reschedule @TODO
	}
	else if(command == "undo") { // @TODO
	}
	else if(command == "redo") { // @TODO
	}
	else {
		Log(logFile, "ERROR - Invalid command supplied.\n");
		goto usage_msg;
	}
	
	goto program_exit;
	
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