#define APAD_DEBUG

#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include "apad_array.h"
#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_file.h"
#include "apad_intrinsics.h"
#include "apad_string.h"
#include "apad_win32.h"
#include "helpers.cpp"

// Storage format
// id(8 bit unsigned) task_text(string) date_added(dd/mm/yyyy) date_due_by(dd/mm/yyyy | -) reschedule_data(days | -) flag(! | ? | @ | -) groups(#id1 #id2 ... #id5)\r\n
ConsoleAppEntryPoint(args, argsCount) {
	Log("\n"); // Insert blank line for clarity
	
	#ifdef APAD_DEBUG
		#if 0
		args[1] = "add";
		args[2] = "new task";
		args[3] = "+3";
		// args[4] = "+2";
		argsCount = 4;	
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
	if(StringsAreEqual(command, "add") == true) {
	  // @TODO - If error, goto usage_msg
		
		const char* task = Null;
		      char  dateAdded[] = "dd/mm/yyyy\0";
					char  dateDue[GetArrayLength(dateAdded)] = "-\0";
		const char* reschedulePeriod = Null;
		const char* flag = Null;
		const char* groups[MaxGroups] = { Null };
		
		// Date added 
		{
			auto date = GetDate(Null);
			auto string = DateToString(date);
			auto length = GetStringLength(dateAdded, false);
			ForAll(length)
				dateAdded[it] = string.string[it];
		}
		
		// Scan through remaining arguments
		FromTo(2, argsCount) {
			const char* arg = args[it];
			
			if(arg[0] == '+') { // Day offset from today
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
					LogError("Invalid day offset (max length allowed is 3)\n");
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
				
				if(dateDue[0] != '-') { // Reschedule period @TODO - Allow work days
					// si32 				i = StringToInt(number);
					reschedulePeriod = daysString;
				}
				else { // Date due
				  if(calendarDays > 60)
						CopyString(">60", -1, dateDue, GetStringLength(dateDue, true), true);
					else {
						auto date = GetDate(calendarDays);
						auto string = DateToString(date);
						CopyString(string.string, -1, dateDue, sizeof(dateDue), true);
					}
				}
			}
			else if( // Date due
			        IsNumber(arg[0]) == true || arg[0] == '.' ||
			        StringsAreEqual(arg, "mon") || StringsAreEqual(arg, "tue") || StringsAreEqual(arg, "wed") || StringsAreEqual(arg, "thu") || 
							StringsAreEqual(arg, "fri") || StringsAreEqual(arg, "sat") || StringsAreEqual(arg, "sun")) 
			{
				if(arg[0] == '.') { // Today
				  auto date = GetDate(0);
					auto string = DateToString(date);
					
					CopyString(string.string, -1, dateDue, GetArrayLength(dateDue), false);
				}
				else if(IsNumber(arg[0]) == true) { // Allowed formats are dd//mm, dd/mm/yy and dd/mm/yyyy
					auto argLength = GetStringLength(arg, false);
											
				  // Initial format check
					// Accepted formats : dd/mm (year assumed to be current), dd/mm/yy or dd/mm/yyyy
					if(!(argLength == GetStringLength("dd/mm", false) && arg[2] == '/' || (argLength == GetStringLength("dd/mm/yy", false) || argLength == GetStringLength("dd/mm/yyyy", false)) && arg[5] == '/')) {
						LogError("Wrong date due format\n");
						goto usage_msg; // @TODO - More specific message?
					}
					
					// If these both are false, the format is dd/mm
					bool hasShortYear = argLength == GetStringLength("dd/mm/yy", false);
					bool hasLongYear = argLength == GetStringLength("dd/mm/yyyy", false);
					
					// Copy into temp string omitting the forward slashes
					char tempString[10] = { '\0' }; // Length of 10 to accomodate dd/mm/yyyy
					ForAll(argLength) {
						if(arg[it] != '/')
							tempString[it] = arg[it];
					}
					
					auto day = StringToInt(tempString);
					auto month = StringToInt(tempString + 3);
					auto year = Null;
					if(hasShortYear == true)
						year = 2000 + StringToInt(arg + 6);
					else if(hasLongYear == true)
						year = StringToInt(arg + 6);
					else 
						year = GetDate(0).year;
					
					// Sanity check
					// @TODO - Check whether the date is real, e.g. 30th of feb
					if(day < 0 || day > 31 || month < 0 || month > 12 || year < GetDate(0).year) {
						LogError("Invalid due date\n");
						goto usage_msg; // @TODO - More specific message?
					}
					
					// Create dateDue
					{
						dateDue[0] = arg[0];
						dateDue[1] = arg[1];
						dateDue[2] = '/';
						dateDue[3] = arg[3];
						dateDue[4] = arg[4];
						dateDue[5] = '/';
						auto string = ToString(year);
						ForAll(4)
							dateDue[it + 6] = string.string[it];
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
			goto usage_msg; // @TODO - More specific message?
		}
		
	  Log("Task added\n");
		
		PrintDetailedTask(Null, task, dateAdded, dateDue, reschedulePeriod, flag, groups);
		
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
	
	goto program_exit;
	
	// @TODO - Parse file contents into a table / list of tasks and deadlines
	
	// @TODO - Save to data/calendar.txt
	// @TODO - Daily local backup at the start of the day
	// 			   - Limit to 10 max previous logs
	//				 - Store with current date
	//

  usage_msg:
	Log("Usage: calendar [add] [list] [delete | del] [modify | mod] [reschedule | res] [undo] [redo]\n");
	// @TODO - Add specific messages for individual commands? E.g. how does the user know the right format for the dates?
	// @TODO - A git-like set of help messages for individual commands?
			
	program_exit:
	if(IsValid(calendar) == true)
		FreeFile(calendar);
	
	return 0;
}