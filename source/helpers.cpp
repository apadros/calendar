bool IsValidChar(char c) {
  return IsLetter(c) == true || IsNumber(c) == true || c == '\"' || c == '/' || c == '-' || c == '?' || c == '!' || c == '#';
}

// @EXPORT_API
bool IsDate(const char* s) {
	// string DateFormatShort  = "dd/mm";
	// string DateFormatMedium = "dd/mm/yy";
	// string DateFormatLong   = "dd/mm/yyyy";
	
	string date = s;
	
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