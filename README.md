# Personal-Diary
Personal Diary mini project in C Language

## Welcome to my personal diary mini program, some rules for the usage:
    • The separation of the date (day, month and year) could hyphen, dot or backslash.
    • The time entry should be with colons i.e. 08:00.
    • Time range is from 8 AM to 8 PM, you can enter new meeting only in this range.
    • Subject is not requirement, could be empty.
    • Meeting on Saturday is not available and checked by the date.
    • Have fun !

## What I was learned from the project ?
First and in my opinion the most important I learn how to take something "big" and  split it into small parts or sections, in this way I worked step by step and I were more neat and organized. 
In the practical part I learned a lot more new topics and "tools" I can use in the future in the C language for example: memory allocation for simple pointers or 3 dimensional pointers, structures and accessing them with pointers,  files (binary or txt) and reading and writing from them, time.h library and here usage, converting from one to another data types by me needs.
Another thing I  learn about the C language is some of her undefined behavior or some bugs that could happen and how to handle with them and avoid them.

## Functions:
- void PrintDesignedHeader(const char *_header)
    - Print a designed header to console, usually of menu's of the program. 
    - _header is the name of the text file that the designed header stored in.
   
- void PrintSimpleHeader(const char *_header,   FILE *_fp)
    - Print a simple header to console and text files.
    - _header is the the header to print.
    - _fp is the name of the file. (could be stdout too).
    
-int Menu(const char *_menu[], int _numOfObjects, const char *_header)
    - Function to print menu's in the program and get user input.
    - _menu is the options to choose.
    - _numOfObjects is counter of options in the menu.
    - _header is the menu header.
    - Returns key pressed (number) by the user.

  int MainMenu()
    • Function that stores the main menu array of strings, call Menu and return the user input to main.
 struct tm *ConvertToTime_T(Date *_date)
    • Function that take as parameter Date structure and initialize/convert it to struct tm type. And returns the result after mktime convert structure to time_t.
    • Returns pointer to date after being converted to time_t structure.
   int DatePassed(Date *_meet)
    • Functions to check whether the user entered a future date.
    • Returns 0 if them passed either 1.
   int IsDateValid(Date *_date)
    • Function that takes Date structure and check whether the date is exists. For example: 30/2/20 does'nt exists.
    • Returns 0 if day is not valid either returns 1.
 int GetWeekDay(Meeting *_meet)
    • Functions that gets the week day name i.e. "Sunday" and check whether the inserted date is not at Saturday (not possible).
    • _meet is out current meeting.
    • Returns 0 if the week day is Saturday either returns 1.
 void CheckYear(int *_year)
    • Function to correct the year inserted format. i.e. if the user entered 20 as year the function will change it to 2020.
    • _year is Date structure member.
 void TokenizeDate(Date *_date, char *_str)
    • Function to "tokenize" the string entered by the user and insert it to Date structure properly. User can enter it with backslash/dot/hyphen and the function will knew to do the work.
    • _date is the Date structure to insert to.
    • _str is the user input string.
char *EnterAString(char *_buffer, const char *_data)
    • Function used to get string from the user, cleans the buffer before entry to avoid errors.
    • _buffer is the string to enter.
    • _date is used to print instruction to the user.
    • Returns char pointer to confirm string is inserted.
int DateEntry(Date *_date, const char *_data)
    • Funtion that contains whole date function collectively. i.e. get user input, tokenize and check validity. 
    • _date is the Date structure to insert.
    • _date is the instruction for the user.
    • Returns 0 if date acceptance is not valid either returns 1.
 void RepeatMenu(Meeting *_meet)
    • Function that display repeat type menu and ask the user times to repeat.
void Repeat(Meeting *_meet)
    • Function that lets the user choose if his meeting repeats. If it is that RepeatMenu activated else continue.
 float ConvertStringToFloat(char *_time)
    • Function that gets string that contains some time and convert it to float type. I.e. "17:50" convert to 17.5. 
    • Return time string after being converted to float.
 int SequentialHours(char *_startTime, char *_endTime)
    • Function that check if the start and end time that taken from the user are sequential. (Start is earlier than end).
    • Return 0 if time is not sequential either returns 1.
 void ProperTimeFormat(char *_time)
    • Function that take time string and propers it, adding leading zeroes for example if user entered 9:45 the function change it to 09:45.
  int IsTimeValid(char *_time)
    • Function to check if the time entered by the user is correct and also in the work day range, for example if entered 14:62 the function does'nt allow this or user entered time before 8 AM or after 8 PM.
    • Returns 0 if time acceptance is not valid either returns 1.
   int TimeEntry(char *_time, const char *_data)
    • Funtion that contains whole time function collectively. i.e. get user input, check time validality and proper the time format.
int CompareTime(Meeting *_timeMeet, Meeting *_timeDiary)
    • Function that gets two meetings and check if their time is overlaps either which one is earlier by TIME only. 
    • Return 0 if there is time overlap with the meetings, returns 1 if Diary meeting is earlier or -1 if new meeting is earlier.
  int CompareDates(Date _dateMeet, Date _dateDiary)
    • Function that gets two dates (of meetings) and check which one is earlier either their on same day and returns that result.
    • Retruns 0 if dates are equal, 1 if new meeting is earlier or -1 if diary date is earlier.
 int AddDaysToDate(Meeting *_meet, int _days)
    • Function that gets the Meeting structure and add day, week or month to the given date, another option is to add _days that passed as parameter for another uses as get start of some week.
    • Returns 0 if after days add the new week day is Saturday eiher returns 1.
 int CheckOverLap(Meeting *_meet, Diary *_diary)
    • Function that gets some meeting (could be repeated also), and the user's diary and check if the new meeting is not overlap with another meeting that already in the diary.
    • Returns 0 if there is overlap with another meeting in the diary either returns 1.
void InsertMeeting(Meeting *_meet, Diary *_diary, int _index)
    • Function that insert the new meeting (could be repeated also) to the diary on the right place. _index is the place to insert (Gets from CheckIndexToInsert function). The function uses memmove function to move the whole diary.
  void CheckIndexToInsert(Meeting *_meet, Diary *_diary)
    • Function that check where to insert the new meeting (could be repeated also) to the diary on the right place, I.e. the diary always sorted.
    void AddRecord(Meeting *_meet, Diary *_diary)
    • Funtion that contains whole adding new record functions collectively. i.e. get user input of date, start and end time of meeting, get week day by the date, subject and repeat (type and times). After getting all the input from user this function activates another functions that check overlap with the diary and if there is no overlap it activates function that adding the new meeting/s to the diary on the right place.
 void PrintMeeting(Meeting _meet, int _numOfMeet, FILE *_fp)
    • Print a single meeting to the console or to file by the parameter in a single line.
    void PrintWholeList(Diary *_diary, FILE *_fp)
    • Print whole diary as a sorted list with meeting number.
   int *AllocateArray()
    • Allocating a memory to array of integers.
    • Returns address of new array after memory allocation.
   int *SearchBySubject(Diary _diary, FILE *_fp)
    • Function that get a string from user (subject) and search for specific meeting/s at the diary.
    • Return pointer to array of meetings found in the search (they're index in the diary), if not found return NULL.
 int *SearchByDate(Diary _diary, FILE *_fp)
    • Function that get a date from user, and search for a specific meeting that   match the date entered.
    • Return pointer to array of meetings found in the search (they're index in the diary), if not found return NULL.
  int *SearchMeetingMenu(Diary _diary, FILE *_fp, const char *_prefix)
    • Function that display the "Search menu" (date or subject) and get option choose from the user and activate the  required function.
    • Return pointer to array of meetings found in the search (they're index in the diary), if not found return NULL.
  int ConfirmMeeting (Diary *_diary, int *_index, int  _numOfMeet)
    • Function that used to confirm that the number of meeting the user entered is found on the list of meeting. (the list that displayed after searching a specific date by subject or date).
    • Returns 1 if meeting successfully deleted either returns 0.
  void DeleteMeetingFromDiary(Diary *_diary)
    • Function that get from user number of meeting to delete, after search and display it on console, there is confirmation function that being activated to check if the 
number entered by the user is fit with meeting on the list displayed.
    
   FILE *OpenBinFile()
    • Function that open the binary file that contains the diary, if does not succeed it opens and new binary file.
    • Return file pointer to existent file either return file pointer to new one (binary).
  void LoadFromFile(Diary *_diary, const char *_prefix)
    • After binary file is opened, the function read number of meeting on the diary at the current time and then  read diary from the file lin by line and insert it to Diary structure.
 void SaveToFile(Diary *_diary, const char *_name)
    • Function that saving the number of meetings and the diary to a binary file, DIARY.BAK (backup file) or DIARY.BIN (the original diary) by the parameter.
FILE *OpenTxtFile()
    • Function that lets the user to choose a name for the new txt file and then opens a file.
    • Returns file pointer to new txt file, name entry by the user.
  void FreeArray(char ***_array, int _row, int _col)
    • Function that freeing the 3D array that used for the weekly display of meetings.
 char ***Alocate3DArray(int _row, int _col)
    • Function that allocated a memory for the 3D array to the weekly display of meetings.
    • Returns pointer of 3 dimensional array for the weekly display.
Meeting *GetWholeWeek(Meeting *_weekArray)
    • Function that initialize the whole array to dates and days of week by the date that the user choose.
    • Return pointer to "search array" after  being initialized.
   Meeting *InitializeSearchArray()
    • Function that let the user choose a date of week he would like to display and then initialize the initial data that used for search meeting on the diary, for example we start to search from 8 AM that we initialize whole array to 8 AM "start time".
    • Return pointer to "search array" after  being initialized. (GetWholeWeek call).
   void NextHours(Meeting *_weekArray)
    • Function that advanced that start and end time of meeting by 2 hours for keep searching. i.e. if the start and end time of some meeting is 8 AM and 10 AM they will be 10 AM and 12 PM.
    void PrintWeeklySchedule_Console(char ***_weeklyString, int _row, int _col)
    • Function that prints the weekly display to the console (stdout).
   void PrintWeeklySchedule_TXT(char ***_weeklyString, int _row, int _col, FILE *_fp)
    • Function that prints the weekly display to txt file given.
    void PrintWeeklySchedule_Data(Meeting *_weekArray, char ***_weeklyString,int _row, int _col, Diary *_diary, FILE *_fp)
    • Function that initialize the weekly 2D array of string with time of meeting at the left and shows if we got a meeting in that time by display "Empty", i.e. no meeting either "Meeting", under and right date and week day and beside the right time.
   void PrintWeeklySchedule_Header(Diary *_diary, FILE *_fp)
    • Function that initialize the header of the weekly display, put the right dates and week days at the first and the second row.
