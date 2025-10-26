Update TOS 1.6 fuer ST/STE/68010:

Folgende Aenderungen wurden durchgefuehrt:

1.      Alle Patches aus TOSFIX.PRG (von Atari) wurden durchgefuehrt
2.      _shell_p ($4F6) wird bei reset auf null gesetzt
3.      befindet sich auf Diskette kein DESKTOP.INF, so wird alternativ
        nach DESKTOP.INC fuer Farbe, sowie nach DESKTOP.INM fuer monochrom
        gesucht. Fehlen diese, wird eine Version aus dem ROM verwendet,
        welche standartmaessig die Textdarstellung verwendet.
        Achtung - bei 'Arbeit sichern' wird immer DESKTOP.INF verwendet.
4.      Wird ein ausfuehrbarer Bootsektor geladen, so wird gefragt, ob
        dieser ausgefuehrt werden soll. Bei einigen Programmen muss man
        sich allerdings beeilen, da diese davon ausgehen, dass das Laufwerk
        noch selektiert ist.
5.      Zusaetzlich wurden noch folgende Tastaturbefehle implimentiert:
        CTRL ALT 1: Mouse Speeder aus
         "    "  2:   "     "     geringe Wirkung
         "    "  3:   "     "     grosse Wirkung
         "    "  5: 50 Hz
         "    "  6: 60 Hz
         "    "  P: parkt alle Atari-Harddisks (Target 0-7, Unit 0&1)
         "    "  K: Tastaturklick aus/an
6.      Bei gedrueckter ALTERNATE-Taste, koennen ASCII-Codes in dezimal
        eingegeben werden. (Nur mit dem numerischen Tastenfeld)
7.      Werden waehrend eines Reset Pin 20 und Pin 22 der RS232 kurz-
        geschlossen, so wird ein echter RESET durchgefuehrt (unabhaengig
        von $426)...

    
Folgende Dateien gehoeren dazu:

        FC0000.IMG      ROM-Version ST
        E00000.IMG      ROM-Version STE
        3D0000.IMG      RAM-Version 4MB fuer ST/STE
        1D0000.IMG      RAM-Version 2MB fuer ST/STE
        0D0000.IMG      RAM-Version 1MB fuer ST/STE

