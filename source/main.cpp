#include "apad_array.h"
#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_file.h"
#include "apad_string.h"
#include "apad_win32.h"

#define DEBUG_ON

const ui8 MaxGroups = 5;

// @TODO - Import from API once implemented
bool IsLetter(char c) {
	return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';
}

// @TODO - Import from API once implemented
bool IsNumber(char c) {
	return c >= '0' && c <= '9';
}

bool IsValidChar(char c) {
  return IsLetter(c) == true || IsNumber(c) == true || c == '\"' || c == '/' || c == '-' || c == '?' || c == '!' || c == '#';
}

// Display dates as days left or >60 days
// Storage format
// id(8 bit unsigned) task_text(string) date_added(dd/mm/yyyy) date_due_by(dd/mm/yyyy | -) reschedule_data(days | -) flag(! | ? | @ | -) groups(#id1 #id2 ... #id5)\r\n

#define Log(_string) printf("%s\n", _string)
#define LogError(_string) printf("ERROR - %s\n", _string)

#include <stdio.h>
ConsoleAppEntryPoint(args, argsCount) {
	Log("\n"); // Insert blank line for clarity
	
	#ifdef DEBUG_ON
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
	#ifndef DEBUG_ON
	if(argsCount < 2) {
		LogError("No commands supplied.\n");
		Log("Usage: calendar [add] [list] [delete | del] [modify | mod] [reschedule | res] [undo] [redo]\n");
		goto program_exit;
	}
	#else
		args[1] = "list";
	#endif
		
	// @TODO - Log file for the day
	// 			   - Timestamp, ID, list of changes
	
	
		
	const char* command = args[1];
	if(StringsAreEqual(command, "add") == true) { // @TODO
	  
		// @TODO - Print message "task added", followed by task data
		
	}
	else if(StringsAreEqual(command, "list") == true) {
	  // @TODO
		// By string, ID, flags & groups, - means not
		// <60 days & >60 days
		// Automatically list preempt tasks with a date <30 days when listing priority tasks?
		
		// @TODO - Parse calendar entries and display
		// id(8 bit unsigned) task_text(string) date_added(dd/mm/yyyy) date_due_by(dd/mm/yyyy | -) reschedule_data(days | -) flag(! | ? | @) groups(#id1 #id2 ... #id5)
		
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
				printf("ID: %s\n", id);
				printf("Task: %s\n", task);
				printf("Data added: %s\n", dateAdded);
				printf("Data due: %s\n", dateDue);
				printf("Reschedule: %s\n", reschedulePeriod);
				printf("Flag: %s\n", flag);
				// printf("Groups: %s\n", flag);
				
				id = Null;
				task = Null;
				dateAdded = Null;
				dateDue = Null;
				reschedulePeriod = Null;
				flag = Null;
				
				ForAll(MaxGroups)
					groups[it] = Null;
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
	
	program_exit:
	if(IsValid(calendar) == true)
		FreeFile(calendar);
	
	return 0;
}