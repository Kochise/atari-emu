NB. The TOS.IMG for TOS 4.92 supplied in this archive has *already* been
    modified as described below.

>> How to load a new TOS from hard disk

>> Copy the file BOOT3.PRG to where you want it on your hard disk. Put the
>> TOS.IMG file for the TOS you want to load in the root directory of your
>> partition C.

>> Prepare a disk containing in the AUTO folder:
>>   - Your hard disk driver (eg. AHDI.PRG or ICDBOOT.PRG)
>>   - The program CAUTO modified by my treatment, under the name CAUTO_M.PRG.
>>   - The modification consists of forcing the boot drive to 2 (drive C) with
>>   - a move.w #2,$446

>> Take a sector editor and look in the TOS.IMG file for the following sequence
>> 42780446 (clr.w _bootdev.w) and replace the first two bytes with 6002
>> (BRA PC+2) This modification will make your TOS not search the AUTO and ACC
>> folders any more, at each reboot or resolution change, but remain on the last
>> boot drive.

>> Boot normally from hard disk with the special disk in the drive. TOS
>> then loads itself from the hard disk. Once loaded, it searches on the disk
>> for the hard disk driver (which allows it to default to the hard drive) and 
>> the CAUTO_M program which:
>>  - loads the auto programs from disk C
>>  - forces the boot drive to C
>> The accessories are automatically loaded normally from partition C.

Original french text by Marc Abramson    
Translation by Ivan Dimbleby (ied2@recursiv.demon.co.uk)
