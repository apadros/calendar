#include "apad_base_types.h"

const ui8 MaxGroups = 5;

#define Log(_string) printf("%s\n", _string)
#define LogError(_string) printf("ERROR - %s\n", _string)

#define GetMax(_a, _b) ((_a) > (_b) ? (_a) : (_b))

// @EXPORT_API apad_string.cpp
void CopyString(const char* source, si16 srcLength, char* destination, ui16 destLength, bool copyEOS) {
	auto realSourceLength = srcLength == -1 ? GetStringLength(source, true) : srcLength;
	AssertRet(realSourceLength <= destLength);
	ForAll(realSourceLength)
	  destination[it] = source[it];
}

// @EXPORT_API apad_string.cpp
bool IsLetter(char c) {
	return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';
}

// @EXPORT_API apad_string.cpp
bool IsNumber(char c) {
	return c >= '0' && c <= '9';
}

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
date GetDate(si32 offsetDays) {
	time_t timeNowSecs = time(NULL) + offsetDays * 24 * 60 * 60;
	auto*  timeNow = localtime(&timeNowSecs); // UTC time
	
	date ret = {};
	ret.dayOfTheWeek = timeNow->tm_wday == 0 ? 7 : timeNow->tm_wday;
	ret.year = 1900 + timeNow->tm_year;
	ret.month = 1 + timeNow->tm_mon;
	ret.day = timeNow->tm_mday;
	return ret;
}

// @EXPORT_API
short_string DateToString(date d) {
	const char* string = "dd/mm/yyyy";
	short_string ret;
	CopyString(string, -1, ret.string, GetStringLength(string, true), true);
	
	auto temp = ToString(d.day);
	if(d.day <= 9) {
		ret.string[0] = '0';
		ret.string[1] = temp.string[0];
	}
	else {
		ret.string[0] = temp.string[0];
		ret.string[1] = temp.string[1];
	}
	
	temp = ToString(d.month);
	if(d.month <= 9) {
		ret.string[3] = '0';
		ret.string[4] = temp.string[0];
	}
	else {
		ret.string[3] = temp.string[0];
		ret.string[4] = temp.string[1];
	}
	
	temp = ToString(d.year);
	ret.string[6] = temp.string[0];
	ret.string[7] = temp.string[1];
	ret.string[8] = temp.string[2];
	ret.string[9] = temp.string[3];
	
	return ret;
}

void PrintDetailedTask(const char* id, const char* task, const char* dateAdded, const char* dateDue, const char* reschedulePeriod, const char* flag, const char** groups) {
  // @TODO - Add assertions once program takes shape
	// AssertRet(id != Null);
	// AssertRet(task != Null);
	// AssertRet(dateAdded != Null);
	// AssertRet(dateDue != Null);
	// AssertRet(reschedulePeriod	!= Null);
	// AssertRet(flag != Null);
	// AssertRet(groups != Null);
	
	id = "-"; // @TODO - Update once IDs are implemented
	
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

// @TODO - PrintTaskWide() - add support for a batch to to ascertain each colum length
void PrintTaskWide(const char* id, const char* task, const char* dateAdded, const char* dateDue, const char* reschedulePeriod, const char* flag, const char** groups) {
	// @TODO - Add assertions once program takes shape
	// AssertRet(id != Null);
	// AssertRet(task != Null);
	// AssertRet(dateAdded != Null);
	// AssertRet(dateDue != Null);
	// AssertRet(reschedulePeriod	!= Null);
	// AssertRet(flag != Null);
	// AssertRet(groups != Null);
	
	// @TODO - Make it so dates which coincide with current year are displayed in dd/mm format
	
	id = Null; // @TODO - Update once IDs are implemented
	
	const char* headers[] = { "ID", "String", "Added", "Due", "Reschedule", "Flag" }; // Groups implemented separately
	const ui8   headersCount = GetArrayLength(headers);
	const char* contents[] = { id, task, dateAdded, dateDue, reschedulePeriod, flag };
	ui16        lengths[headersCount] = { };
	ForAll(headersCount) {
		const char* header = headers[it];
		const char* content = "-";
		if(contents[it] != Null)
			content = contents[it];
		
		auto headerLength = GetStringLength(header, false);
	  auto contentLength = GetStringLength(content, false);
		
	  lengths[it] = GetMax(headerLength, contentLength) + 2;
	}
	
	// Print headers
	ui16 totalHeadersLength = 0;
	ForAll(headersCount) {
		const char* header = headers[it];
		ui16 finalLength = lengths[it];
		
		printf(" %s ", header);	
		ui16 headerLength = GetStringLength(header, false);
		totalHeadersLength += 1 + headerLength + 1;
		si16 printLength = finalLength - (headerLength + 2);
		if(printLength > 0) {
			ForAll(printLength) {
				printf(" ");
				totalHeadersLength += 1;
			}
		}
		printf("|");
		totalHeadersLength += 1;
	}
	
	// Groups
	{
		const char* string = " Groups ";
	  printf(string);
	  totalHeadersLength += GetStringLength(string, false);
	}
	printf("\n");
	
	// Print separator
	ForAll(totalHeadersLength)
	  printf("=");
	printf("\n");

	// Print content
	ForAll(headersCount) {
		const char* content = "-";
		if(contents[it] != Null)
			content = contents[it];
		ui16 finalLength = lengths[it];
		
		printf(" %s ", content);
		si16 printLength = finalLength - (GetStringLength(content, false) + 2);
		if(printLength > 0) {
			ForAll(printLength)
				printf(" ");
		}
		printf("|");
	}
	
	// Groups
	ForAll(MaxGroups) {
		const char* group = groups[it];
		if(group == Null)
			continue;
		if(it > 0)
			printf(",");
		printf(" %s", groups[it]);
	}
}