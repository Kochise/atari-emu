Steem SSE 4.2.0

Atari ST emulator

https://sourceforge.net/projects/steemsse/

R0
Initial release, full of bugs
20 July 2025

R1
Fix bug in archive handling
25 July 2025

R2
Fix blitter read Lines per Bit-Block register
Fix debugger IO labels
10 August 2025

R3
Various bugfixes
24 August 2025

R4
On starting, Steem SSE looks first in the application folder for your current
steem.ini file, then in Documents/steem then in the registry and lastly in the
roaming "AppData/steem" user folder.
If it can't find it, it asks you where it is or it should be created. All your
settings will be recorded in that file.
Steem also needs a temp folder for archive handling and such. First it checks
in the steem.ini file if the path is specified ([Main]TempPath). If not it
tries to use "AppData/steem", if not writable the system temp folder
(adding /steem), if not writable the place where steem.ini is, if not writable
the folder where the executable was started.
21 September 2025
