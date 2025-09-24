#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_file.h"
#include "apad_string.h"
#include "apad_win32.h"

// Task data
// ID - int
// String
// Flags - priority, awaiting response / feedback, preempt
// Groups
// - Preceded by #
// Upper limit of 5?
// Date added
// Date due by
// - - means none
// - Store as dd/mm/yyyy, display as days left or >60 days
// Reschedule info
// - - if none

// Storage format
// id(8 bit unsigned) task_text(string) date_added(dd/mm/yyyy) date_due_by(dd/mm/yyyy | -) reschedule_data(days | -) flag(! | ? | @) groups(#id1 #id2 ... #id5)

#define Log(_string) printf("%s\n", _string)
#define LogError(_string) printf("ERROR - %s\n", _string)

#include <stdio.h>
ConsoleAppEntryPoint(args, argsCount) {
	Log("\n"); // Insert blank line for clarity
	
	const char* dataPath = "data/calendar.txt";
	
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
		Log("Usage: calendar [add] [list] [delete | del] [modify | mod] [reschedule | res] [undo] [redo]\n");
		goto program_exit;
	}
		
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
		
		const char* start = (const char*)calendar.memory;
		
		char id[3] = { '\0' };
		
		// @TODO - Loop until first space is found, copy chars, ToString(), continue from there
		
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
	
	goto program_exit;
}