#include "libraries.h"
#include "read.h"
//=====================================================================
//STRINGS
//=====================================================================
inline char ctoupper(char c)
{
	if (c >= 'a' && c <= 'z')
		return (c - 'a' + 'A');

	return c;
}

bool QuickCompare(const char *p, const char *q)
{
	for (unsigned int i = 0; i < strlen(p) && *p != '\0'; i++)
	{
		if (ctoupper(*(p + i)) != ctoupper(*(q + i)))
			return false;
	}

	return true;
}

void stoupper(char *s)
{
	bool bString = false;

	while (*s != '\0')
	{
		if (*s == '\"')
			bString = (bString) ? false : true;

		if (!bString && *s >= 'a' && *s <= 'z')
			*s -= 32;
		s++;
	}
}

int CopyTo(char *Dest, const char *src, int ln)
{
	int i = 0;

	for (; i < ln; i++)
	{
		if (*src == ' ' || *src == '\0')
			break;

		*Dest++ = *src++;
	}

	*Dest = '\0';

	return i + 1;
}

int CopyTo(char *Dest, const char *src, bool(*cfunc)(char), unsigned int ln)
{
	unsigned int i = 0;

	for (; i < ln; i++)
	{
		if (cfunc(*src) || *src == '\0')
			break;

		*Dest++ = *src++;
	}

	*Dest = '\0';

	return i + 1;
}


//=====================================================================
//COMPARISONS
//=====================================================================
bool Compare(const char *s1, const char *s2, int len)
{
	if (s1 == s2)
		return false;

	int i = 0;

	while (i < len && *s1 == *s2)
	{
		if (*s1 == '\0' || *s2 == '\0')
			break;

		s1++;
		s2++;
		i++;
	}

	if (i == len)
		return true;

	return false;
}

bool Compare(const char *s1, const char *s2)
{
	if (s1 == NULL || s2 == NULL)
		return false;

	while (*s1++ == *s2++)
	{
		if (*s1 == '\0')
			return true;
	}

	return false;
}

bool CompareReverse(const char *s1, const char *s2, unsigned int dist)
{
	int i = 0;

	if (s1 == NULL || s2 == NULL)
		return false;

	for (i = 0; i < (int)dist; i++)
	{
		if (!iswascii(*(s1 - i)))
			return false;

		if (*s2 == *(s1 - i)) //compare chars
			break;
	}

	return Compare(s1 - i, s2, strlen(s2));
}

char *FindChar(char *str, char c, unsigned int dist)
{
	for (unsigned int i = 0; (*(str + i) != '\0' || c != '\0') && i < dist; i++)
	{
		if (*(str + i) == c)
			return str + i;
	}

	return str;
}

char *ReadNumber(char *str, char input[], unsigned int dist)
{
	for (unsigned int j = 0; j < dist && (IsNumber(*str) || *str == '.') && *str != '\0'; j++, str++)
		input[j] = *str;

	return str;
}

bool IsLetter(char c)
{
	if (c >= 'A' && c <= 'Z')
		return true;

	return false;
}

bool IsNumber(char c)
{
	if (c >= '0' && c <= '9')
		return true;

	return false;
}

bool IsOperator(char c)
{
	if (c == '+' || c == '-' || c == '*' || c == '/')
		return true;

	return false;
}

bool IsLogical(const char *str)
{
	if ((*str == '&' && *(str + 1) == '&') || (*str == '|' && *(str + 1) == '|'))
		return true;

	return false;
}

bool IsConditioner(const char *str)
{
	if (((*str == '=' || *str == '!' || *str == '>' || *str == '<') && (*(str + 1) == '=')) || (*str == '>' || *str == '<' && (*(str + 1) == ' ')))
		return true;

	return false;
}

bool SearchChar(const char *field, int dist, char c)
{
	for (int i = 0; i < dist && *field != '\0'; i++)
		if (*field++ == c)
			return true;

	return false;
}

bool IsValidName(char c)
{
	if (c >= 65 && c <= 90)
		return true;

	return false;
}