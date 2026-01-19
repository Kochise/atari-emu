/*
// Demonstriert verschiedene AES ERROR, die von WINX gemeldet werden
//
// Fuegt man die folgende Zeile in WINX.INF ein
//   WINXERR = 8- 
// dann wird das "AES ERROR"-Alert fuer WINXERR unterdrueckt.
*/

#include <aes.h>

void main( void)
{
	int win;
	
	appl_init();
	
	wind_set( 123, WF_KIND, 0, 0, 0, 0);
		/* AES ERROR, da Subfunktion WF_KIND gueltiges Fenster braucht */
	wind_set( 123, 456, 0, 0, 0, 0);
		/* Keine Fehlermeldung, da Subfunktion unbekannt.
		   Fehler wird als Funktionswert 0 gemeldet */

	win = wind_create( 0, 100, 100, 100, 100);
	if (win > 0) {
		wind_open( win, 100, 100, 100, 100);
		wind_open( win, 100, 100, 100, 100);
			/* AES ERROR, da Fenster bereits offen */
		wind_close( win);
		wind_close( win);
			/* AES ERROR, da Fenster bereits geschlossen */
		wind_set( win, WF_TOP, 0, 0, 0, 0);
			/* AES ERROR, da Toppen nur fuer offene Fenster erlaubt */
	}
	wind_delete( win);
	
	wind_open( win, 100, 100, 100, 100);
		/* AES ERROR, da Fenster bereits freigegeben */

	wind_update( END_UPDATE);
		/* AES ERROR, da mehr END_UPDATEs als BEG_UPDATEs */
		
	appl_exit();
} /* main */

