#include "include.h"

static char *_translations[][2] = {
	{ "Day", "Dag" },		// LOCALE_DAY
	{ "Language", "Sprog" },	// LOCALE_LANGUAGE
	{ "English", "Engelsk" },	// LOCALE_ENGLISH
	{ "Danish", "Dansk" },		// LOCALE_DANISH
	{ "Completed", "Gennemført" },	// LOCALE_COMPLETED
	{ "No", "Nej" },		// LOCALE_NO
	{ "Yes", "Ja" },		// LOCALE_YES
	{ "Multiuser", "Flerbruger" },	// LOCALE_MULTIUSER
	{ "Select user", "Vælg bruger" },	// LOCALE_SELECTUSER
	{ "Run", "Løb" }		// LOCALE_RUN
};

char *locale_get(int key)
{
	return _translations[key][user_profiles[selected_user].language];
}
