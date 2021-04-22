#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <regex.h>

#define ARR_SIZE 64
#define FILE_NAME "diary.bin"
#define FILE_BACKUP "diary.bak"

#define REG_TIME "^([0-9]|0[0-9]|1[0-9]|2[0-3]):[0-5][0-9]$"

typedef struct Date Date;
typedef struct Meeting Meeting;
typedef struct Node Node;
typedef union Search Search;

typedef struct Date
{
    int day;
    int month;
    int year;
} Date;

typedef struct Meeting
{
    Date   date;
    char   day[ARR_SIZE];
    char   begHour[ARR_SIZE];
    char   endHour[ARR_SIZE];
    char   subject[ARR_SIZE];
    int      repeat;
    int      repeatType;
} Meeting;

typedef struct Node
{
    Meeting     data;
    Node*       next;
    Node*       prev;
} Node;

typedef union Search
{
    char *buffer;
    Date dt;
} Search;

void PrintDesignedHeader(const char* header)
{
    FILE *fptr;
    char c = ' ';

    fptr = fopen(header, "r");
    if (fptr == NULL)
    {
        printf("Cannot open file \n");
        return;
    }

    c = fgetc(fptr);
    while (c != EOF)
    {
        printf ("%c", c);
        c = fgetc(fptr);
    }

    fclose(fptr);
}

void PrintSimpleHeader(const char* header, FILE* fp)
{
    int i = 0;

    for (; i < 32; ++i)
    {
        if (16 == i)
            fprintf(fp, "%s", header);
        else
            fprintf(fp, "-");
    }
    fprintf(fp, "\n");
}

int Menu(const char* menu[], int numOfObjects, const char* header)
{
     int keyPressed = 0;

     PrintDesignedHeader(header);
     printf("Enter the number of operation you would like to use: \n");
     for (int i = 1; i <= numOfObjects; ++i)
     {
        printf("%s \n", menu[i - 1]); /* Print all options in menu */
     }

     scanf("%d", &keyPressed); /* Get option from user */

     return keyPressed;
 }

int MainMenu()
{
    const char *MainMenu[] = { "| 1 |Add new Record",
                                               "| 2 |Save to file",
                                               "| 3 |Revert",
                                               "| 4 |Search for meetings (Console)",
                                               "| 5 |Search for meeting (TXT)",
                                               "| 6 |Delete single meeting",
                                               "| 7 |Delete all matching meetings by date",
                                               "| 8 |Delete all matching meetings by subject",
                                               "| 9 |Print whole meeting list (Console)",
                                               "| 10|Print whole meeting list (TXT)",
                                               "| 11|Print weekly meetings (Console)",
                                               "| 12|Print weekly meetings (TXT)",
                                               "| 13|Save and Exit",
                                               "| 14|Quit" };

    return Menu(MainMenu, 14, "MainMenu.txt"); /*Return Option from user to main */
}

struct tm *ConvertToTime_T(Date *date)
{
    time_t      curTime, check;
    struct tm   *timeinfo = { 0 };

    time(&curTime);
    timeinfo = localtime(&curTime);         /* Initialize other member of structure */

    timeinfo->tm_year = date->year - 1900; /* Year since 1900 */
    timeinfo->tm_mon =date->month - 1;     /* Month starts from 0 */
    timeinfo->tm_mday =date->day;
    timeinfo->tm_isdst = -1;

    check = mktime(timeinfo);

    if (-1 == check) /* Mktime got error convert */
    {
        printf("Error while converting \n");
        return NULL;
    }

    return timeinfo;
}

int CompareDates(Date dateMeet, Date dateDiary)
{
    /* Compare two years */
    if (dateDiary.year < dateMeet.year) return -1;
    else if (dateDiary.year > dateMeet.year) return 1;
    /* Years equal, compare months */
    if (dateDiary.month < dateMeet.month) return -1;
    else if (dateDiary.month > dateMeet.month) return 1;
    /* Years AND months equal, compare days */
    if (dateDiary.day < dateMeet.day) return -1;
    else if (dateDiary.day > dateMeet.day) return 1;
    /* All three are equal */
    return 0;
}

int DatePassed(Date *meet)
{
       Date today = {0};
       time_t rawtime;
       struct tm *info;
       time( &rawtime );
       info = localtime( &rawtime ); /* Get today's date */

       today.day = info->tm_mday;
       today.month = info->tm_mon + 1;
       today.year = info->tm_year + 1900;

       if (1 == CompareDates(*meet, today)) /* Compare today's date and user entry */
       {
           printf("Date passed already ! \n");
           return 0;
       }

       return 1;
}

int IsDateValid(Date *date)
{
    int   vDay = 0, vMonth = 0, vYear = 0;
    vYear = date->year - 1900;
    vMonth = date->month - 1;
    vDay = date->day;

    struct tm *timeinfo = ConvertToTime_T(date);

    /* Check if dates are same after converting, if they are not the date does not exists */
    if (vDay != timeinfo->tm_mday || vMonth != timeinfo->tm_mon || vYear != timeinfo->tm_year)
    {
        printf("Date does not exists \n");
        return 0;
    }

    return 1;
}

int GetWeekDay(Meeting *meet)
{
    struct tm *timeinfo = ConvertToTime_T(&meet->date);

    if (6 == timeinfo->tm_wday)
    {
        printf("Saturday ! \n");
        return 0;
    }

    strftime(meet->day, ARR_SIZE, "%A", timeinfo); /* Get day from Sunday to Friday */
    return 1;
}

void CheckYear(int *year)
{
    int count = 0;
    int temp = *year;

    while (temp)
    {
        ++count;
        temp /= 10;
    }
    if (count < 4)
        *year += 2000; /* Fix year format */
}

void TokenizeDate(Date *date, char *str) /* Convert date from char array to date structure */
{
    const char *delim = "-./\n";
    char *ptr = NULL;
    int *temp = &date->day;     /* Points to first member at Date */

    ptr = strtok(str, delim);   /* Tokenize day from string */

    while (NULL != ptr)
    {
        sscanf(ptr, "%d", temp);  /* Initialize to Date structure */
        temp++;                   /* Increment pointer */
        ptr = strtok(NULL, delim);/* Tokenize again */
    }

    CheckYear(&date->year);      /* Fix year format to 4 digits */
}

char *EnterAString(char *buffer, const char *data)
{
    char *check = NULL;

    printf("Enter a %s: \n", data);
    setbuf(stdin, NULL);                      /* Clean buffer */
    check = fgets(buffer, ARR_SIZE, stdin);  /* Get string */

    return check;
}

int DateEntry(Date *date, const char *data)
{
    char buffer[ARR_SIZE], *check = NULL;

    check = EnterAString(buffer, data);       /* Get date from user as a string */
    TokenizeDate(date, buffer);

    if (NULL == check || !IsDateValid(date))  /* Check date validity and string entry */
        return 0;

    return 1;
}

void RepeatMenu(Meeting *meet)
{
    const char *menu[] = { "| 1 |Daily", "| 2 |Weekly", "| 3 |Monthly" };

    printf("How many times your meeting repeats ? \n");
    scanf("%d", &meet->repeat);

    meet->repeatType = Menu(menu, 3, "RepeatMenu.txt"); /* Initialize repeatType by user choose */
}

void Repeat(Meeting *meet)
{
    char choose = ' ';
    printf("Does your meeting repeats ? (y/n) \n");
    do
    {
        scanf("%c", &choose);
        switch (tolower(choose))
        {
        case 'y': RepeatMenu(meet); break;                           /* Opens repeat menu */
        case 'n': meet->repeat = 1; meet->repeatType = 0; break;    /* Initialize "default" */
        default: printf("Invalid option choosed \n"); break;          /* Try again */
        }
    } while (choose != 'y' && choose != 'n');
}

float ConvertStringToFloat(char *time)
{
    char *ptr = NULL;
    float timeFL = 0;

    if (NULL == (ptr = strchr(time, ':')))
        return 0;
    *ptr = '.';                   /* Used for atof */
    strtok(time, "\n");          /* Erase the new line character */

    timeFL = (float)atof(time);
    *ptr = ':';                   /* Get back the colons to char array */

    return timeFL;
}

int SequentialHours(char *startTime, char *endTime)
{
    float startTimeFL = ConvertStringToFloat(startTime);
    float endTimeFL = ConvertStringToFloat(endTime);

    if (startTimeFL >= endTimeFL) /* Check if begin time is earlier than end time */
    {
        printf("Error ! time inserted is not Sequential \n ");
        return 0;
    }

    return 1;
}

void ProperTimeFormat(char *time) /* Add leading zero's to the time */
{
    int hour = 0, minute = 0;
    char *token = NULL;

    token = strtok(time, ":");
    hour = atoi(token);

    token = strtok(NULL, ":");
    minute = atoi(token);

    sprintf(time, "%02d:%02d", hour, minute);
}

int IsTimeValid(const char *time)
{
    regex_t    regex;
    int status = 1;

    if (regcomp(&regex, REG_TIME, REG_EXTENDED))
    {
        printf("Error ! \n");
        return 0;
    }

    status = regexec(&regex, time,  0, NULL, 0);
    regfree(&regex);
    if (status != 0)
    {
        return 0;
    }
    return 1;
}

int TimeEntry(char *time, const char *data)
{
    char *check = NULL;
    check = EnterAString(time, data);       /* Get time string from user */
    time[strlen(time) - 1] = '\0';

    if (NULL == check || !IsTimeValid(time)) /* check time validity and string entry */
    {
        printf("Invalid time initialize \n");
        return 0;
    }

    ProperTimeFormat(time);                  /* Add leading zero's to time */

    return 1;
}

int CompareTime(Meeting *timeMeet, Meeting *timeDiary)
{
    float timeDiaryBeg =  ConvertStringToFloat(timeDiary->begHour);
    float timeDiaryEnd = ConvertStringToFloat(timeDiary->endHour);

    float timeMeetBeg  =  ConvertStringToFloat(timeMeet->begHour);
    float timeMeetEnd = ConvertStringToFloat(timeMeet->endHour);

    if (timeDiaryBeg >= timeMeetBeg && timeDiaryBeg <= timeMeetEnd) /* Means overlap */
        return 0;

    if (timeDiaryEnd >=  timeMeetBeg && timeDiaryEnd <= timeMeetEnd) /* Means overlap */
        return 0;

    if (timeMeetBeg >= timeDiaryBeg && timeMeetBeg <= timeDiaryEnd) /* Means overlap */
        return 0;

    if (timeMeetEnd >=  timeDiaryBeg && timeMeetEnd <= timeDiaryEnd) /* Means overlap */
        return 0;

    return (timeMeetBeg > timeDiaryBeg) ? 1 : -1;
}

int AddDaysToDate(Meeting *meet, int days)
{
    struct tm *timeinfo = ConvertToTime_T(&meet->date);

    switch (meet->repeatType)
    {
    case 1: timeinfo->tm_mday += 1; break; /* Add one day */
    case 2: timeinfo->tm_mday += 7; break; /* Add one week */
    case 3: timeinfo->tm_mon += 1; break;  /* Add one month */
    default: timeinfo->tm_mday += days;   /* Add days sends to func parameter */
    }
    mktime(timeinfo);

    /* New date after adding */
    meet->date.day = timeinfo->tm_mday;
    meet->date.month = timeinfo->tm_mon + 1;
    meet->date.year = timeinfo->tm_year + 1900;

    if (0 == GetWeekDay(meet))
        return 0;

    return 1;
}

void DestroyList(Node** head)
{
  Node* curr = *head;
  Node* next = NULL;
  while (curr != NULL)
  {
    next = curr->next;
    free(curr);
    curr = next;
  }
  *head = NULL;
}

Node *MakeNewNode(Meeting *meet)
{
    Node *newMeeting = malloc(sizeof(Node));
    if (! newMeeting)
        printf("Error while allocating memory for node !\n");

    newMeeting->data = *meet;
    newMeeting->next = NULL;
    newMeeting->prev = NULL;

    return newMeeting;
}

void PrintMeeting(Meeting data, int numOfMeet, FILE *fp)
{
        fprintf(fp, "%d -> ", numOfMeet);
        fprintf(fp, "%4d/%d/%-6d   ", data.date.day, data.date.month, data.date.year);
        fprintf(fp,"%-12s %-4s - %-8s %s\n", data.day, data.begHour, data.endHour, data.subject);
}

void PrintWholeList(Node *diary, FILE *fp)
{
    int i = 1;

    PrintSimpleHeader("Meeting List", fp);
    for (; diary; ++i, diary = diary->next)
    {
        PrintMeeting(diary->data, i, fp);
    }
    printf("Meeting print ended ! \n");
}

Node* RepeatedList(Node *meet, int numOfMeet)
{
    Node *head = NULL;
    Node* newNode = NULL;
    int i = 1;

    for(; i <= numOfMeet; ++i)
    {
        if (! head)
            head = newNode = MakeNewNode(&meet->data);
        else
        {
            if (! AddDaysToDate(&meet->data, 0))
            {
                DestroyList(&head);
                break;
            }
            newNode->next = MakeNewNode(&meet->data);
            newNode->next->prev = newNode;
            newNode = newNode->next;
        }
    }

    return head;
}

int CheckOverLap(Meeting *meet, Node *diary)
{
    int cmpTime = 0, cmpDate = 0;

    for (; diary; diary = diary->next)
    {
        cmpDate = CompareDates(meet->date, diary->data.date); /* Dates equal */
        cmpTime = CompareTime(meet, &diary->data);          /* Over lap with time */
        if (!cmpDate)
        {
            if (!cmpTime)
            {
                return 0; /* Means over lap */
            }
        }
    }

    return 1;
}

Node *MergeTwoLists(Node *meet, Node *diary)
{
    int cmpDate = 0;
    int cmpTime = 0;

    if (! meet)
        return diary;
    if(! diary)
        return meet;

    cmpDate = CompareDates(meet->data.date, diary->data.date);
    cmpTime = CompareTime(&meet->data, &diary->data);
    if ((1 == cmpDate) || (!cmpDate && -1 == cmpTime))
    {
        meet->next = MergeTwoLists(meet->next, diary);
        meet->next->prev = meet;
        meet->prev = NULL;
        return meet;
    }
    else
    {
        diary->next = MergeTwoLists(meet, diary->next);
        diary->next->prev = diary;
        diary->prev = NULL;
        return diary;
    }
}

Node* AddRecord(Node *diary)
{
    int entry = 1;
    Meeting temp = {};
    Node *meet = NULL;
    meet = MakeNewNode(&temp);
    Node *head = NULL;

    do
    {
        if (!DateEntry(&meet->data.date, "Date"))                           /* Enter a date and check if correct */
            continue;

        if (!DatePassed(&meet->data.date))
            continue;

        if (!GetWeekDay(&meet->data))                                         /* Get day of week entered */
            continue;

        if (!TimeEntry(meet->data.begHour, "Start time (8:00 - 20:00)"))    /* Enter start time on work day range */
            continue;

        if (!TimeEntry(meet->data.endHour, "End time (8:00 - 20:00)"))      /* Enter end time on work day range */
            continue;

        if (!SequentialHours(meet->data.begHour, meet->data.endHour) )          /*Check if start time earlier than end time */
            continue;

        if (! EnterAString(meet->data.subject, "Subject"))
            continue;

        entry--;                                                        /* all data entered correctlly */
    } while (entry);

    Repeat(&meet->data);                                                      /* Repeat choose */

    head = meet = RepeatedList(meet, meet->data.repeat);

    while (meet)
    {
        if (0 == CheckOverLap(&meet->data, diary))                               /* Over lap with whole diary check */
        {
            printf("There is overlap with another meeting ! \n");
            DestroyList(&meet);
            return diary;
        }
        meet = meet->next;
    }

    return MergeTwoLists(head, diary);
}

int *AllocateArray()
{
    int *arr = calloc(ARR_SIZE, sizeof(int));
    if (NULL == arr)
    {
        printf("Error ! could not allocate memory for the array \n");
    }

    return arr;
}

Search SearchBySubject(Node *diary, FILE *fp)
{
    int i = 1;
    Search bySub = {0};
    bySub.buffer =malloc(sizeof(ARR_SIZE));

    EnterAString(bySub.buffer, "subject of the meeting");   /* Get subject to search from user */

    PrintSimpleHeader("Meetings", fp);               /* Meetings header print */
    for (; diary; ++i ,diary = diary->next)
    {
        if (!strcmp(bySub.buffer, diary->data.subject))  /* Compare subject entered with diary subject */
        {
            PrintMeeting(diary->data, i,  fp); /* Print meeting found */
        }
    }
    return bySub;
}

Search SearchByDate(Node *diary, FILE *fp)
{
    int i = 1;
    Search date = { 0 };                              /* Date to search */
    DateEntry(&date.dt, "Enter a date to search");

    PrintSimpleHeader("Meetings", fp);               /* Print Meetings header */
    for (; diary; ++i, diary = diary->next)
    {
        if (!CompareDates(date.dt, diary->data.date))
        {
            PrintMeeting(diary->data, i,  fp); /* Print meeting found */
        }
    }
    return date;
}

Search SearchMeetingMenu(Node *diary, FILE *fp, const char *prefix)
{
    const char *menu[] = {"| 1 |Subject", "| 2 |Date"};

    switch (Menu(menu, 2, prefix)) /* Display menu to user and get input from user */
    {
        case 1:  return SearchBySubject(diary, fp);
        default: return SearchByDate(diary, fp);
    }
}

void FreeNode(Node **diary, Node *del)
{
    if (*diary == NULL || del == NULL)
        return;

    if (*diary == del)
        *diary = del->next;

    if (del->next != NULL)
        del->next->prev = del->prev;

    if (del->prev != NULL)
        del->prev->next = del->next;

    free(del);
}

Node *DeleteNodeBySubject(Node *diary, const char* sub)
{
    if(! diary)
        return diary;

     if (!strcmp(sub, diary->data.subject))
    {
        FreeNode(&diary, diary);
        return DeleteNodeBySubject(diary, sub);
    }
    else
    {
        diary->next = DeleteNodeBySubject(diary->next, sub);
        return diary;
    }
}

Node *DeleteNodeByDate(Node *diary, Date dt)
{
    if(! diary)
        return diary;

    if (!CompareDates(dt, diary->data.date))
    {
        FreeNode(&diary, diary);
        return DeleteNodeByDate(diary, dt);
    }
    else
    {
        diary->next = DeleteNodeByDate(diary->next, dt);
        return diary;
    }
}

Node* DeleteSingleNodeSub(Node *diary, char* sub, int index)
{
    if (! diary)
    {
        printf("No such meeting ! \n");
        return diary;
    }

    if (1 == index && (! strcmp(sub, diary->data.subject)))
    {
        if (diary->next)
			diary->next -> prev = diary->prev;
		if (diary->prev)
			diary->prev -> next = diary->next;
		FreeNode(&diary, diary);
		return diary;
    }
    diary->next = DeleteSingleNodeSub(diary->next, sub, index - 1);
    return diary;
}

Node* DeleteSingleNodeDate(Node* diary, Date dt, int index)
{
    if (! diary)
    {
        printf("No such meeting ! \n");
        return diary;
    }

    if (1 == index && (!  CompareDates(dt, diary->data.date)))
    {
        if (diary->next)
			diary->next -> prev = diary->prev;
		if (diary->prev)
			diary->prev -> next = diary->next;
		FreeNode(&diary, diary);
		return diary;
    }
    diary->next = DeleteSingleNodeDate(diary->next, dt, index - 1);
    return diary;
}

Node* DeleteMeeting(Node *diary)
{
    const char* search[] = {"| 1 | Search by subject",
                                         "| 2 | Search by date"};
    Search ret = {0};
    int index = 0;

    switch (Menu(search, 2, "SearchMenu.txt"))
    {
        case 1:
            ret = SearchBySubject(diary, stdout);
            printf("Enter appointment number: ");
            scanf("%d", &index);
            diary = DeleteSingleNodeSub(diary, ret.buffer, index);
            free(ret.buffer);
            break;
        case 2:
            ret = SearchByDate(diary, stdout);
            printf("Enter appointment number: ");
            scanf("%d", &index);
            return DeleteSingleNodeDate(diary, ret.dt, index);
    }
    return diary;
}

FILE *OpenTxtFile()
{
    char buffer[ARR_SIZE] = "", *check = NULL;
    FILE *fp = NULL;

    check = EnterAString(buffer, "File name");              /* Get file name from user */

    buffer[strcspn(buffer, "\n")] = '\0';                   /* "Cut" new line from the file name */
    strcat(buffer, ".txt");                                 /* Add .txt to file name */

    if (NULL == check || NULL == (fp = fopen(buffer, "w"))) /* Means failed to open txt file or get string */
    {
        printf("Failed open a txt file \n");
        return NULL;
    }

    printf("File %s created successfully \n", buffer);
    return fp;
}

FILE *OpenBinFile()
{
    FILE *fp = NULL;

    fp = fopen(FILE_NAME, "rb+");           /* Open for reading */

    if (NULL == fp)
    {
        fp = fopen(FILE_NAME, "wb+");       /* Case error or file not found, open new one */
        if (NULL == fp)
        {
            printf("Error open new file!"); /* Could not open new file */
            return NULL;
        }
        printf("New file %s created \n", FILE_NAME);
    }

    return fp;
}

Node *LoadFromFile(const char *prefix)
{
    Node *head = NULL;
    Node *newNode = NULL;
    FILE *fp = NULL;
    Meeting meet = {};

    if (NULL == (fp = OpenBinFile()))
        return NULL;

    printf("%s \n", prefix);
    while (fread(&meet, sizeof(Meeting), 1, fp))
    {
        if (! head)
            head = newNode = MakeNewNode(&meet);
        else
        {
            newNode->next = MakeNewNode(&meet);
            newNode->next->prev = newNode;
            newNode = newNode->next;
        }
    }

    printf("Data loaded to DIARY from %s \n", FILE_NAME);
    fclose(fp);

    return head;
}

void SaveToFile(Node *diary, const char *name)
{
    FILE *fp = fopen(name, "wb+");

    if (NULL == fp)
    {
        printf("Error ! could not open file for writing !\n");
        return;
    }

    for (;  diary; diary = diary->next)
    {
        fwrite(&diary->data, sizeof(Meeting), 1, fp); /* Write meeting line by line */
    }

    printf("Data saved to %s \n", name);
    fclose(fp);
}

void FreeArray(char ***array, int row, int col)
{
    int i = 0, j = 0;
    for (i = 0; i < row; i++)
    {
        for (j = 0; j < col; j++)
        {
            free(array[i][j]);
        }
        free(array[i]);
    }
    free(array);
}

char ***Alocate3DArray(int row, int col)
{
    int i = 0, j = 0;
    char ***weeklyString = (char***)calloc(row, sizeof(char*)); /* row allocation */
    if (NULL == weeklyString)
        return NULL;

    for (i = 0; i < row; ++i)
    {
        if (NULL == (weeklyString[i] = (char**)calloc(8, sizeof(char*)))) /* Column allocation */
            return NULL;
    }

    for (i = 0; i < row; ++i)
    {
        for (j = 0; j < col; ++j)
        {
            if (NULL == (weeklyString[i][j] = (char*)calloc(32, sizeof(char)))) /* Strings length allocation */
                return NULL;
        }
    }

    return weeklyString;
}

Node *GetWholeWeek(Node *weekArray)
{
    int subDays = 0;
    struct tm *timeinfo = ConvertToTime_T(&weekArray->data.date);
    subDays = -(timeinfo->tm_wday); /* Subtraction to get start of week */
    AddDaysToDate(&weekArray->data, subDays); /* Get whole week dates and week days */

    weekArray->data.repeatType = 1;
    weekArray = RepeatedList(weekArray, weekArray->data.repeat);

    return weekArray;
}

Node *InitializeSearchArray()
{
    Node *weekArray = malloc(sizeof(Node)); /* Allocation for every day at week */

    if(! weekArray)
    {
        printf("Error while allocating weekly array \n");
        return weekArray;
    }

    if (!DateEntry(&weekArray->data.date, "Some date of the week")) /* Date entry by the user */
        return NULL;

    if (!GetWeekDay(&weekArray->data)) /* Get week day by the date */
        return NULL;

    strcpy(weekArray->data.begHour, "00:00"); /* First hours to search in diary initialize */
    strcpy(weekArray->data.endHour, "02:00");

    weekArray->data.repeatType = 0;
    weekArray->data.repeat = 6;

    return GetWholeWeek(weekArray);
}

void NextHours(Node *weekArray)
{
    float begHour = 0, endHour = 0;

    for (; weekArray; weekArray = weekArray->next)
    {
        begHour = ConvertStringToFloat(weekArray->data.begHour);
        endHour = ConvertStringToFloat(weekArray->data.endHour);

        begHour += 2.0; /* Next hours to search */
        endHour += 2.0;

        sprintf(weekArray->data.begHour, "%0.f:00", begHour);
        sprintf(weekArray->data.endHour, "%0.f:00", endHour);
    }
}

void PrintWeeklySchedule_Console(char ***weeklyString, int row, int col)
{
    int i = 0, j = 0;

    PrintDesignedHeader("Weekly.txt");
    for (i = 0; i < row - 1; ++i)
    {
        for (j = 0; j < col; ++j)
        {
                printf("%16s",  weeklyString[i][j]);
        }
        printf("\n");
    }

    FreeArray(weeklyString, row, col);
}

void PrintWeeklySchedule_TXT(char ***weeklyString, int row, int col, FILE *fp)
{
    int i = 0, j = 0, space = 0;

    PrintSimpleHeader("Weekly Schedule:", fp);
    for (i = 0; i < row - 1; ++i)
    {
        /* Print aligned to text file */
        if      (i == 0) space = 19;
        else if (i == 1) space = 20;
        else             space = 20;

        for (j = 0; j < col; ++j)
        {
                if (0 == strcmp(weeklyString[i][j], "  Empty"))
                        fprintf(fp, " ");

                fprintf(fp, "%*s", space,  weeklyString[i][j]);
        }
        fprintf(fp, "\n");
    }

    printf("Weekly schedule printed to file successfully \n");
    FreeArray(weeklyString, row, col);
}

void PrintWeeklySchedule_Data(Node *weekArray, char ***weeklyString,int row, int col, Node *diary, FILE *fp)
{
    int i = 2, j = 0;
    Node *temp = weekArray;

    for (i = 2; i < row; ++i, j = 0)
    {
        weekArray = temp;
        strcpy(weeklyString[i][j], temp->data.begHour); /* Insert time to left column */
        ProperTimeFormat(weeklyString[i][j]);
        ++i;
        strcpy(weeklyString[i][j], "  -");
        for (j = 1; j < col; ++j, weekArray = weekArray->next)
        {
            if(0 == CheckOverLap(&weekArray->data, diary))  /* Means there is meeting */
                strcpy(weeklyString[i][j], "Meeting");
            else
                strcpy(weeklyString[i][j], "  Empty");
        }
        NextHours(temp);                                     /* Advance time to search */
    }

    if (stdout == fp)
       PrintWeeklySchedule_Console(weeklyString, row, col);      /* Print to stdout i,e. console */
    else
       PrintWeeklySchedule_TXT(weeklyString, row, col, fp);     /* Print to given txt file */

}

void PrintWeeklySchedule_Header(Node *diary, FILE *fp)
{
    int i = 1;
    Node *weekArray = InitializeSearchArray();
    Node *temp = weekArray;
    int row = 26, col = 7;
    char ***weeklyString = Alocate3DArray(row, col);

    for (; temp; ++i, temp = temp->next)
    {
        /* Initialize first row (date) */
        sprintf(weeklyString[0][i], "%d/%d/%d",
        temp->data.date.day, temp->data.date.month, temp->data.date.year);

        /* Initialize second row (week day) */
        strcpy(weeklyString[1][i], temp->data.day);
    }

    PrintWeeklySchedule_Data(weekArray, weeklyString, row, col, diary, fp);

    DestroyList(&weekArray);
}

int main()
{
    int op = 0;
    char buffer[ARR_SIZE] = {};
    Date dt = {0};
    Node* diary = NULL;
    FILE* fp = NULL;

    diary = LoadFromFile("Loading...");

    do
    {
        op = MainMenu();
        fp = (op == 5 || op == 10 || op == 12) ? OpenTxtFile() : NULL;
        switch (op)
        {
        case 1:  diary = AddRecord(diary); break;
        case 2:  SaveToFile(diary, FILE_BACKUP); break;
        case 3:  diary = LoadFromFile("Reverting diary..."); break;
        case 4:  SearchMeetingMenu(diary, stdout, "SearchMenu.txt");  break;
        case 5:  SearchMeetingMenu(diary, fp, "SearchMenu.txt"); fclose(fp); break;
        case 6:  diary = DeleteMeeting(diary); break;
        case 7:  DateEntry(&dt, "Enter a date"); diary = DeleteNodeByDate(diary, dt); break;
        case 8:  EnterAString(buffer, "Enter a subject: "); diary = DeleteNodeBySubject(diary, buffer); break;
        case 9:  PrintWholeList(diary, stdout); break;
        case 10: PrintWholeList(diary, fp); fclose(fp); break;
        case 11: PrintWeeklySchedule_Header(diary, stdout); break;
        case 12: PrintWeeklySchedule_Header(diary, fp); fclose(fp); break;
        case 13: SaveToFile(diary, FILE_NAME); break;
        case 14: break;
        default: break;
        }
        printf("Press [Enter] key to continue.\n");
        while(getchar()!='\n');
        getchar();
    } while (op != 13 && op != 14);

    printf("Automatic save is ON ! \n");
    SaveToFile(diary, FILE_NAME);
    DestroyList(&diary);

    return 0;
}
