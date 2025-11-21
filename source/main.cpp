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
const ui8 	 MaxTags = 5;

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
	string tags[MaxTags];

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
			printf("ERROR: Invalid command\n\n");
			goto usage_msg;
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
				printf("ERROR: Not enough arguments supplied\n\n");
				goto usage_msg;
			}
		}
		else if(IsDate(arg) == true || (arg.length == 1 && arg[0] == '.') || (arg.length >= 2 && arg.length <= 4 && arg[0] == '+')) { // Date
      if(arg[0] == '.') {
				if(targetDate.length == 0)
					targetDate = DateToString(GetDate(0));
				else {
					printf("ERROR: Target date already supplied: %s\n\n", arg);
					goto usage_msg;
				}
			}
			else if(arg[0] == '+') {
				string daysString = arg.chars + 1;
				
				// Determine validity and whether work days have been specified
				bool isValid = true;
				bool workDays = false;
				{
					const ui8 MaxDigits = 3;
					
					const char* workDaysSub = FindSubstring("w", daysString);
					if(workDaysSub != Null) {
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
				
				if(targetDate.length == 0) {
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
				
					targetDate = GetDate(calendarDays);
				}
				else if(reschedulePeriod.length == 0)
					reschedulePeriod = arg.chars + 1;
				else {
					printf("ERROR: Reschedule period already supplied\n\n", arg);
					goto usage_msg;
				}						
			}
			else {
				if(targetDate.length == 0)
					targetDate = GetDate(arg);
				else {
					printf("ERROR: Target date already supplied\n\n", arg);
					goto usage_msg;
				}
			}
		}
		else if(arg.length == 1) { // Flag
			if(arg[0] == '!' || arg[0] == '?' || arg[0] == '@')
				flag = arg;
			else {
				printf("ERROR: Invalid flag supplied: %s\n\n", arg);
				goto usage_msg;
			}
		}
		else if(arg == "-t") { // Tags
			it += 1;
			
			if(it == argsCount) {
				printf("ERROR: No tags specified\n\n");
				goto usage_msg;
			}
			
			// Everything following this switch is considered to be a tag
			do {
				arg = args[it];
				
				bool added = false;
				ForAll(MaxTags) {
					if(tags[it].length == 0) {
						tags[it] = arg;
						added = true;
						break;
					}
				}
				
				if(added == false) {
					printf("ERROR: Number of tags exceeded, max 5\n\n");
					goto program_exit;
				}
				
				it += 1;
			}
			while(it < argsCount);
		}
		else { // Invalid argument
			printf("ERROR: Invalid argument: %s\n\n", arg);
			goto usage_msg;
		}
	}		
	
	// Check minimum required arguments have been supplied
	if(taskString.length == 0) {
		Log(logFile, "ERROR - No task string specified.\n");
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
		
		PrintDetailedTask(Null, taskString, dateAdded, targetDate, reschedulePeriod, flag, tags);
		
		// @TODO - Store in calendar.txt
		
		goto program_exit;
	}
	else if(command == "list") {
		#if 0
	  // @TODO
		// By string, ID, flags & tags, - means not
		// <60 days & >60 days
		// Automatically list preempt tasks with a date <30 days when listing priority tasks?
		
		// @TODO - Parse calendar entries and display
		// id(8 bit unsigned) task_text(string) date_added(dd/mm/yyyy) date_due_by(dd/mm/yyyy | -) reschedule_data(days | -) flag(! | ? | @) tags(#id1 #id2 ... #id5)
		
		// @TODO - search / list filters - view only task, id, all tasks from #group, etc
		// 			 	- E.g. view only the tags of tasks due by x date
		
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
		const char* tags[MaxTags] = { Null };
		
		const char* toStore = Null; // Temp string
		bool        scanningTaskString = false;
		ForAll(calendar.size) {
			char c = data[it];
			
			if(c == '\n') { // Reset task data
				if(printDetailed == true)
					PrintDetailedTask(id, task, dateAdded, dateDue, reschedulePeriod, flag, tags);
				else
					PrintTaskWide(id, task, dateAdded, dateDue, reschedulePeriod, flag, tags);
				
				id = Null;
				task = Null;
				dateAdded = Null;
				dateDue = Null;
				reschedulePeriod = Null;
				flag = Null;
				
				ForAll(MaxTags)
					tags[it] = Null;
					
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
					ForAll(MaxTags) {
						if(tags[it] == Null) {
							tags[it] = toStore;
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