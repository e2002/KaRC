#ifndef parser_h
#define parser_h
#include <ctype.h>
#define BUFLEN  140

char title[BUFLEN];
char song[BUFLEN];
char nameset[BUFLEN];
bool playing = false;
int volume = 0;
struct tm *dt;
byte listSize=0;

char* ucase(char *line) {
  bool next=false;
  for (char *iter = line; *iter != '\0'; ++iter)
  {
    byte rus=(byte)*iter;
    if(next){
      if(rus>=128 && rus<=143) *iter=(char)(rus+32);
      if(rus>=176 && rus<=191) *iter=(char)(rus-32);
      next=false;
    }
    if(rus==208) next=true;
    if(rus==209) {
      *iter=(char)208;
      next=true;
    }
    *iter = toupper(*iter);
  }
  return line;
}

char *trimwhitespace(char *str)
{
  char *end;
  // Trim leading space
  while (isspace((unsigned char)*str)) str++;
  if (*str == 0) // All spaces?
    return str;
  // Trim trailing space
  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end)) end--;
  // Write new null terminator character
  end[1] = '\0';
  return str;
}

int volRoundUp(int numToRound, int multiple)
{
  return ((numToRound + multiple - 1) / multiple) * multiple;
}

void parse(HXScreen *screen, const char* line) {
  char* ici;

  // ************* NAMESET ************************
  if ((ici = strstr(line, "MESET#: ")) != NULL)
  {
    sscanf(line, "%*s%d %[^\r\n]", &screen->nsNum, &nameset);
    screen->setName(ucase(nameset));
    strcpy(title, "[connecting]");
    strcpy(song, "");
    screen->setTitle(title);
    screen->setSong(song);
    dbg("dd.nameset=");
    dbgln(nameset);
  }

  // ************* META ************************
  // ##CLI.META#: Ooze - Meeting With Strange Species (Karl-Axel Bissler Not Too Loud Rmx)

  if ((ici = strstr(line, "META#: ")) != NULL || (ici = strstr(line, "ICY4#: ")) != NULL)
  {
    if (sscanf(line, "%[^\r\n-]%[^\r\n]", &title, &song) != 2) {
      strcpy(title, ici + 7);
      strcpy(song, "");
      screen->setTitle(ucase(title));
      screen->setSong(ucase(song));
    } else {
      strcpy(title, strstr(title, "META#: ") + 7);
      strcpy(song, strstr(song, "-") + 1);
      strcpy(song, trimwhitespace(song));
      screen->setTitle(ucase(title));
      screen->setSong(ucase(song));
    }
    dbg("title=");
    dbgln(title);
    dbg("song=");
    dbgln(song);
  }

  // ************* PLAYING ************************
  if ((ici = strstr(line, "PLAYING#")) != NULL)
  {
    playing = true;
  }

  // ************* STOPPED ************************
  if ((ici = strstr(line, "STOPPED#")) != NULL)
  {
    playing = false;
    //startDeepSleepTimer();
    strcpy(title, "[stopped]");
    if ((ici = strstr(line, "Invalid")) != NULL) strcpy(title, "[invalid address]");
    if ((ici = strstr(line, "failed")) != NULL) strcpy(title, "[decoder failed]");
    if ((ici = strstr(line, "cli stop")) != NULL) strcpy(title, "[stopped]");
    if ((ici = strstr(line, "from State")) != NULL) strcpy(title, "[stopped]");
    if ((ici = strstr(line, "from addon")) != NULL) strcpy(title, "[stopped]");
    strcpy(song, "");
    screen->setTitle(title);
    screen->setSong(song);
  }

  // ************* VOL ************************
  if ((ici = strstr(line, "VOL#:")) != NULL)
  {
    volume = volRoundUp(atoi(ici + 6) * 100 / 254, 5) ;
    screen->setVolLevel(volume);
  }

  // ************* DATE ************************
  if ((ici = strstr(line, "SYS.DATE#:")) != NULL)
  {
    char lstr[30];
    if (*(ici + 11) != '2') //// invalid date. try again later
    {
      return;
    }
    strcpy(lstr, ici + 11);
    time_t timestamp;
    dt = gmtime(&timestamp);
    int year, month, day, hour, minute, second;
    sscanf(lstr, "%04d-%02d-%02dT%02d:%02d:%02d", &(year), &(month), &(day), &(hour), &(minute), &(second));
    dt->tm_year = year; dt->tm_mon = month - 1; dt->tm_mday = day;
    dt->tm_hour = hour; dt->tm_min = minute; dt->tm_sec = second;
    dt->tm_year -= 1900;
    //dd.timestamp = mktime(dt);
    screen->setTime(mktime(dt));
  }

  // ************* PLAYLIST ************************
  if ((ici = strstr(line, "CLI.LISTNUM#: ")) != NULL)
  {
    byte listN;
    char listT[20];
    sscanf(line, "%*s%d: %[^,]", &listN, &listT);
    if (strlen(listT) != 0) {
      //strcpy(playList[listN],listT);
      screen->plPush(listN,ucase(listT));
      screen->playlistSizePlus();
    }
  }
}

#endif
