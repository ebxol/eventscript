//READ.H
bool GetLine(FILE *fp, char *scriptline);
int read_main(char *fn, char *eventline);
void stoupper(char *s);
bool QuickCompare(const char *p, const char *q);
inline char ctoupper(char c);

//STR
bool Compare(const char *s1, const char *s2);
bool Compare(const char *s1, const char *s2, int len);
bool CompareReverse(const char *s1, const char *s2, unsigned int dist);
void UpperCase(const char *c);
int CopyTo(char *Dest, const char *src, int ln = 32);
int CopyTo(char *Dest, const char *src, bool(*cfunc)(char), unsigned int ln = 32);
char *FindChar(char *str, char c, unsigned int dist);
char *ReadNumber(char *str, char input[], unsigned int dist);
bool IsValidName(char c);
bool IsLetter(char c);
bool IsNumber(char c);
bool IsOperator(char c);
bool IsConditioner(const char *str);
bool IsLogical(const char *str);
bool SearchChar(const char *field, int dist, char c);