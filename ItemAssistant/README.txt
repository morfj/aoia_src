Installation:
=============
- Unzip and run from your location of choice.

When upgrading from a previous version I recommend making a copy in a safe
location of your old ItemAssistant.db file just to be on the safe side.
If the automated migration to the new version creates wierd results please
send a detailed description of the problem and attach BOTH the old and the 
newly converted ItemAssistant.db file.

Support may be found at the forum: http://apps.sourceforge.net/phpbb/aoia/

Usage:
======
Online help system is available at http://ia-help.frellu.net

Aknowledgements:
================
This application is in no way only due to my own efforts. Some people who 
have contributed either directly or indirectly include:
    * kennethx1     - Moral support and code.
    * auno          - aoppa source was a big help.
    * morb, gnarf   - clicksaver rocks and showed me the technique that made 
                      it all possible.
    * RobotJunk     - Multi-thread patch for the hook dll and Client-to-Server 
                      message hooking as well as all the item-transfer message 
                      decoding.
    * Gremlin       - For being a guinea pig.
    * Everyone else that has provided feedback and done testing.

Change Log:
===========
Version 0.9.3										Release Date: ??/?/2009
	* Added support for NPC trade-in quest rewards in the item tracking.
    * Now parsing the 18.0.1 database correctly.
	* Bug Fix: Identify view will now sort (right hand side list).

Version 0.9.2                                       Release Date: 20/2/2009
    * Enhanced the player-shop monitor to refresh the view when something 
      changes on disc. Cleaned up file system monitoring.
    * Added more logging to make it easier to submit bug reports.
    * Added hooking of messages sent from the client. Up until now only 
      messages from the server have been intercepted.
    * Inventory viewer now tracks changes to the players inventory and 
      backpacks as they are being changed by the user. This should reduce the 
      number of full reparsings you would need to do.
    * Added "Identify" view that lets you quickly find any bio clumps or 
      compounds from Alappa based on what they will turn into once 
      tradeskilled.
    * Experiemental: Added support for recording a toons dimension ID.
    * Fixed a bug with recording playershop IDs.

Version 0.9.1                                       Release Date: 25/10/2008
    * Added experimental tracking of player-shop contents.
    * Updated the help menu a bit.
    * Reduced lag by making the AO hook multi-threaded and cache relevant 
      information for a longer period of time.
    * Fixed pause button and tray popup in the Player Shop Monitor.
    * Fixed problem with some Inamorata Rifles showing an empty name.

Version 0.9.0                                       Release Date: 09/10/2008
    * Now works on both WindowsXP as well as Vista (32 and 64 bit) without the
      need to be launched in compatibility mode.
    * Added "-log" commandline option.
    * Playershop monitor added.
    * The application will now parse the original AO database to create its 
      own item database.
    * GUI cleanup.
    * Some context menu commands now work with multiple selection of items.
    * Copying item references to AO script format now also writes the result
      to a script called 'aoia' to the scripts folder of Anarchy Online.
    * Added CSV export to inventory context menu.
    * Specifying minimum and maximum QL level when looking for an item is now 
      possible.
    * Items are now shown with the correct interpolated name.
    * Fixed an issue that would cause the character ID to be shown instead of 
      the character name.
    * The Pattern Matcher now has an option to exclude already assembled ABCD 
      patterns from the availability calculation.

Version 0.8.4                                       Release Date: 31/03/2008
    * Updated DB scheme to fix issue with 32 bit unsigned character IDs.
    * Added code to update database file from previous version to the current
      version.
    * The ItemAssistant.db is created during startup if it is missing.
    * Added sorting of toons and named backpacks in the inventory tree-view.
    * Updated URL to aomarket.
    * Fixed a crashbug.
    * General code cleanup.
    * Updated project to Visual Studio 2008.

Version 0.8.3                                       Release Date: 20/03/2008
    * Fixed crash bug with zoning and login after 17.8.1.
    * Inventory tree-view now has a node for the "Social" tab.
    * Added context menu to items. You are now able to show itemstats using 
      either auno.org or aomainframe.net. You can also copy the item as a 
      HTML link or VBB link.
    * Updated item database to version 17.8.1.

Version 0.8.2                                       Release Date: 16/01/2007
    * Automatic detection of toon names. Removed possibility to rename a toon 
      name manualy. Names are detected on zone.
    * Pressing the applications minimize or close button now sends the 
      application to the system tray instead. To actually exit the application,
      select "Exit" from the tray menu or the file menu.
    * Updated item database to version 17.0.3.

Version 0.8.1                                       Release Date: 24/11/2006
    * Fixed a bug in the Inventory view that caused the last character of all 
      itemnames to disappear.

Version 0.8.0                                       Release Date: 7/11/2006
    * Updated the item DB to match v16.4.1 of Anarchy Online.
    * Upgraded to use latest version of SQLite (v3.3.8). You should still be 
      able to use your old DB file.
    * Converted to using UNICODE wherever I can. This means support for the 
      UTF-8 encoded XML files that AO is using, is in place. I have only 
      tested with a Norwegian characterset, but I suspect Russian and 
      whatnot should work as well. The downside is that the app will probably 
      NOT work on Windows 95/98... but who cares?
    * The about box now actually contains some information. :P
    * Removed the "Bot Export" view. It didn't do squat anyway.
    * The "Message View" is now only available in the debug version like 
      originally intended.
    * Now supports a new command line argument called "-db". It specifies 
      what database file to use.
      Example: ItemAssistant.exe -db C:\my_rk2_items.db
    * Re-thought the whole pattern-matcher concept with some new ideas and 
      valuable input by "Senlee".
      (You want YOUR ideas heard? Use the feedback form!)
    * Redesigned the Pattern Matcher GUI. Now have a listview containing the 
      name and availability of all the pocket bosses.
    * Selecting a pocket boss in the list will bring up a report outlining 
      where you have pattern pieces for this pockect boss stored. (Both toon 
      names and backpack names and locations.)
    * The application is now multithreaded. A seperate thread is spawned to 
      calculate the availability column.

Version 0.6.2                                       Release Date: 27/04/2006
    * Pattern Matcher and Inventory list-view now shows proper container names
      for location of pattern pieces.

Version 0.6.1                                       Release Date: 16/04/2006
    * Pattern Matcher has had some work done on it. It no longer does a query 
      on startup, so starup speed has improved. Also the pattern and toon 
      combo-boxes work.
    * Zoning now removes items in the "unknown" container of the current 
      charater.

Version 0.6                                         Release Date: 10/12/2005
    * Backpack parsing to match new packet format in AO 16.1.
    * First version of the info panel (CTRL+I). Still lots of work left on it.

Version 0.5                                         Release Date: 20/09/2005
    * Release build.
    * AO Item database updated to version 16.0.2
    * Info panel (CTRL-I) that shows details about selected item in the 
      Inventory view.
    * Zoning now auto-deletes items in the "unknown" tree node.
    * Deleting the items in the "unknown" tree node should no longer delete 
      all items.

Version 0.4 (and earlier)                           Release Date: BC
    * Initial test builds.
    * Debug builds.
