bool IsValidChar(char c) {
  return IsLetter(c) == true || IsNumber(c) == true || c == '\"' || c == '/' || c == '-' || c == '?' || c == '!' || c == '#';
}

// @EXPORT_API
void ConvertStringToLowerCase(const char* s) {
	auto length = GetStringLength(s, false);
	ForAll(length) {
    if(s[it] >= 'A' && s[it] <= 'Z')
			s[it] += 'a' - 'A';
	}
}

// @EXPORT_API
date GetDate(const char* s) {
	string dateString = s;
	
	ConvertStringToLowerCase(dateString);
	if(dateString == "mon" || dateString == "tue" || dateString == "wed" || dateString == "thu" || 
		 dateString == "fri" || dateString == "sat" || dateString == "sun")
	{
		ui8 argDay = 0; // 1 -> 7 to match the date struct
		{
			const char* days[] = { "mon", "tue", "wed", "thu", "fri", "sat", "sun" };
			ForAll(7) {
				if(arg == days[it]) {
					argDay = it + 1;
					break;
				}
			}
		}
		
		si8 offset = argDay - GetDate(0).dayOfTheWeek;
		if(offset < 0)
			offset += 7;
		
		return GetDate(offset);
	}
	else {
		ui8 day = 0;
		ui8 month = 0;
		ui16 year = 0;
		
		// Day
		{
			dateString[2] = '\0';
			auto d = StringToInt(dateString);
			AssertRetType(d >= 1 && d <= 31, date());
			day = d;
		}
		
		// Month
		{
			if(dateString.length >= DateFormatMedium.length)
				dateString[5] = '\0';
			
			auto m = StringToInt(dateString.chars + 3);
			Assert(m >= 1 && m <= 12)
			month = m;
		}
		
		// Year
		if(dateString.length >= DateFormatMedium.length) {
			auto y = StringToInt(dateString.chars + 6);
			
			auto currentYear = GetDate(0).year;
			AssertRetType(y >= (currentYear - 2000) && y <= 99 || y >= currentYear && y <= 2099, date());
			
			if(y < 100)
				y += 2000;
			year = y;
		}
		
		// @TODO - Create a tm struct, fill in day, month & year, convert to time_t with mktime(), then convert back.
		// tm->wday should have been filled out.
		
	}
	
	Assert(false);
	
	return date();
}

// @EXPORT_API
bool IsDate(const char* s) {
	// string DateFormatShort  = "dd/mm";
	// string DateFormatMedium = "dd/mm/yy";
	// string DateFormatLong   = "dd/mm/yyyy";
	
	string date = s;
	
	ConvertStringToLowerCase(date);
	if(date == "mon" || date == "tue" || date == "wed" || date == "thu" || 
		 date == "fri" || date == "sat" || date == "sun")
		 return true;
	
	if(date.length != DateFormatShort.length && date.length != DateFormatMedium.length && date.length != DateFormatLong.length)
		return false;
	
	// Check day
	{
		if(date[2] != '/')
			return false;
		
		date[2] = '\0';
		auto day = StringToInt(date);
		if(day < 0 || day > 31)
			return false;
	}
	
	// Check month
	{
		if(date.length >= DateFormatMedium.length) {
			if(date[5] != '/')
				return false;
			date[5] = '\0';
		}
		
		auto month = StringToInt(date.chars + 3);
		if(month < 0 || month > 12)
			return false;
	}
	
	// Check year
	if(date.length >= DateFormatMedium.length) {
		auto year = StringToInt(date.chars + 6);
		
		if(date.length == DateFormatMedium.length && (year < 0 || year > 99))
			return false;
		if(date.length == DateFormatLong.length && (year < 0 || year > 2099))
			return false;
	}
	
	// @TODO - Check date viability e.g. 31st feb
	
	return true;
}

void PrintDetailedTask(const char* id, const char* task, const char* dateAdded, const char* dateDue, const char* reschedulePeriod, const char* flag, string* groups) {
  // @TODO - Add assertions once program takes shape
	// AssertRet(id != Null);
	// AssertRet(task != Null);
	// AssertRet(dateAdded != Null);
	// AssertRet(dateDue != Null);
	// AssertRet(reschedulePeriod	!= Null);
	// AssertRet(flag != Null);
	// AssertRet(groups != Null);
	
	id = "-"; // @TODO - Update once IDs are implemented
	if(reschedulePeriod == Null)
		reschedulePeriod = "-";
	if(flag == Null)
		flag = "-";
	
	printf("ID:         %s\n", id);
	printf("String:     %s\n", task);
	printf("Date added: %s\n", dateAdded);
	printf("Date due:   %s\n", dateDue);
	printf("Reschedule: %s\n", reschedulePeriod);
	printf("Flag:       %s\n", flag);
	printf("Groups:     ");
	if(groups[0] == Null) // If no groups supplied
		printf("-\n");
	else { 
		printf("%s\n", groups[0] + 1);
		FromTo(1, MaxGroups) {
			if(groups[it] != Null)
				printf("            %s\n", groups[it] + 1); // +1 to skip the #
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
		
	  lengths[it] = Max(headerLength, contentLength) + 2;
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