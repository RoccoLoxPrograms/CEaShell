# Change Log

All notable changes to CEaShell will be documented in this file.

## [2.0.1] - 2024-09-08

### Added
- Pressing <kbd>vars</kbd> in the file info menu opens the editor.

### Changed
- Modify behavior for moving the cursor down in the file info menu.
- Holding down <kbd>alpha</kbd> will no longer repeat when changing input mode in CEaShell text input contexts.
- Repeat delay for <kbd>on</kbd> hotkey selector is now longer.
- Update VAT sorting code to Mateo's latest version.

### Fixed
- Theta characters no longer render as `[` when collapsing menus.
- Transition speed setting is more stable.
- Issue with editing TI-BASIC programs when Hexaedit is on the calculator after opening the edit options menu once.
- File cursor behavior when hiding both apps and AppVars folder.
- File names now properly render during collapse animation after deleting through the file info menu.
- Cursor behavior for <kbd>↑</kbd> and <kbd>↓</kbd> keys in text input contexts no longer goes out of bounds.
- Using `expr(` or storing a string variable with an error to an equation no longer crashes after going to the error.
- Block running and editing of system variables in obscure contexts.
- Editing an archived program when a scrolling Ans value is freshly displayed on the homescreen.
- Conflict with GetCSC hook and apps that use ti.appData like ProbSym

## [2.0.0] - 2024-07-08

### Added
- Scrollbar in main file menu.
- Four different GUI scales in the main file menu.
- Option to customize normal and hidden file name text colors.
- Fast alpha scrolling feature in the TI-OS editor.
- Integration with [HexaEdit](https://github.com/captain-calc/HexaEdit-CE) for as an alternative file editor.
- More detailed comments in header files and consistent naming / formatting schemes in all source files.
- Added support for app icons in CEaShell.
- App icons and descriptions in the TI-OS app menu.
- Option to hide the "Programming" menu in Python versions of TI-OS and jump directly to TI-BASIC.
- File searching in the main file view.
- Show file size next to the currently selected file in TI-OS apps and programs menus.
- CEaShell installer has a French translation, which is selected when TI-OS has its language set to French.
- Quickly edit programs using <kbd>vars</kbd> when in CEaShell.
- <kbd>on</kbd> + <kbd>0</kbd> - <kbd>9</kbd> customizable shortcut to launch programs / apps.

### Changed
- 95% of code has been re-written since [1.1.5](#115---2023-03-25).
- Major restructuring and optimization.
- French translation is now on the same branch.
- Status bar now covers the whole top of the screen.
- Calculate rounded corners of windows to make them fit inside each other.
- You can turn off the calculator in any menu of CEaShell now.
- More consistent / easy to use interfaces.
- Hidden files use a different color for names, rather than the fake transparent dither effect.
- Menus are no longer hardcoded.
- Transitions use SPI commands to disable and enable LCD updates, as opposed to getting a sprite of large areas of the screen like previously.
- Move options between settings and customization menus to make more sense.
- Improved text-wrap algorithm.
- Use static allocation for things that were previously dynamically allocated.
- Change file properties format, you can now open the info menu while selecting folders as well.
- Proper text-input routine with timer-based key debouncing.
- Restrcuture GetCSC hooks to work better with each other.
- Exit homescreen hook in a safer way and no longer force context.
- Show the number of files in a folder when viewing it in the info menu.
- Replaced welcome screen with a more minimalist design.
- Shorten descriptions in the OS with "..." if they would exceed the space available for displaying.

### Fixed
- Fix pixel offsets for menu options.
- Fix issues with strange edge-cases when exiting TI-BASIC execution.
- Properly chain hooks.
- Correctly go to errors in TI-BASIC subprograms.
- Correctly go to errors in TI-BASIC temporary parsers.
- Safely handle Garbage Collection while using CEaShell.
- Issue with using recall in the TI-BASIC editor.

## [1.1.5] - 2023-03-25

### Added
- Add lowercase naming for AppVars.

### Changed
- VAT only sorts when pressing the program button, not typing the letter C or opening the draw menu.

### Fixed
- Fix bug with icon offset.
- Reinstall icon hook when program errors out.

## [1.0.6] - 2023-01-25

### Fixed
- Calculator crashes when running empty BASIC programs from the homescreen.
- App change hook is not preserved when editing programs through CEaShell.
- Correct a bug with the editor.

### Removed
- Unnecessary xLibc palette sprite has been removed.

## [1.0.0] - 2022-12-27

### First official release! 🎉

## [0.84.1] - 2022-12-26

### Added
- Version detection for CEaShell save AppVar.

### Fixed
- Fix some small bugs and inconsistencies.

## [0.83.3] - 2022-12-22

### Fixed
- "File count" and "Show hidden programs" work better in the other menu.
- Fix a memory leak when selecting "Goto" on programs erroring when ran in the shell.
- Reinstall the GetCSC hook when you select "Goto" running a program from the OS with the homerun hook.
- Fix issue with inverting shell.

## [0.82.8] - 2022-12-21

### Changed
- Speed up opening and closing of menus.
- Exit app more cleanly.
- Move app_tools to a submodule.
- Make some code a little nicer.
- Debounce keys before running apps as suggested by [LogicalJoe](https://github.com/LogicalJoe).

### Fixed
- Do bools in assembly correctly.
- Properly handle hiding hidden programs in the shell when the option is selected.
- Correct a memory leak.
- Fixed issues with running basic programs, along with a memory leak.
- Fix an issue with hiding/unhiding CEaShell.

## [0.81.2] - 2022-12-18

### Changed
- Optimize scrolling by caching VAT pointers so we don't search through every program every time.
- Speed up alpha searching on some keys.
- Handle garbage collecting when archiving the AppVar.

### Fixed
- Fix displaying of theta character in program names.
- Fix an issue with alpha searching.

## [0.80.0] - 2022-12-06

### Added
- Pressing [on] when in the shell will power off the calculator, and will return to the shell when turned back on.

### Fixed
- Some other small fixes

## [0.79.3] - 2022-12-03

### Fixed
- Properly handle garbage collecting, both in and outside of the shell!
- Remove Pascal from the list of languages, because ti84pceg.inc is not Pascal.

## [0.78.1] - 2022-11-23

### Added
- Support for running apps, plus an apps folder.
- Option in the settings menu to show/hide folders.

### Changed
- Corners are no longer different sizes in the settings and graphics menus.

## [0.77.3] - 2022-11-11

### Added
- Added a special type of "Celtic appvar" with the ":CEL" header, which is editable.

### Changed
- Using some code written by Jacobly, you can press the same button twice now for alpha searching.

## [0.76.1] - 2022-11-05

### Added
- You can copy programs and appvars.
- You can create new programs.
- Beta testers are included in the credits.

## [0.75.5] - 2022-11-01

### Added
- Help screen when the save data AppVar does not exist.
- Error for lack of memory.

### Changed
- Update screenshots.

### Fixed
- Fixed a bug where editing a program too many times caused a memory leak.
- Fixed two bugs when exiting the program editor.
- Fixed a bug running programs.
- Correct an issue with the cursor.

## [0.74.4] - 2022-10-25

### Added
- About screen with credits.
- Makefile now contains the version number for the about menu.
- Added customizable APD in the shell.
- Create header for programs to be hidden in CEaShell.

### Fixed
- Return to the OS if the program being ran by the homescreen hook doesn't exist.

## [0.73.2] - 2022-10-20

### Added
- Added scrolling in the settings menu.
- Added an option to turn on lowercase.
- Added an option to disable the busy indicator.

### Changed
- Disable/re-enable GetCSC hook when running programs.

### Fixed
- Fixed a bug with exiting CEaShell.

## [0.72.0] - 2022-10-17

### Added
- Users can display a count of files in a directory.

## [0.71.0] - 2022-10-17

### Added
- The user can create their own custom color themes.

## [0.70.7] - 2022-10-16

### Added
- Handle running programs from the OS ourselves so we can do fancy things.
- Dark mode hook, APD hook, and shell launching hook.
- Cesium's label jumping menu.
- Allow the user to edit locked programs from "Goto" in the error handler, even if the program is run from the OS. This only works when the "Edit locked programs" option is toggled on.

### Changed
- Made so the icon hook and the [on] shortcuts are individually toggleable.
- Enlarged the cursor on the delete confirmation box slightly.
- Made some other minor changes to the settings menu.

### Fixed
- Correct an issue where editing archived programs from the OS caused file corruption.
- Fixed a bug in the OS program editor shortcuts menu where hitting "Execute program" could cause a RAM clear.
- Fix some strange issues and memory leaks.

## [0.68.5] - 2022-10-06

### Added
- Add an option to show or hide programs in the CEaShell program list.
- Add another obscure format of icon / description.
- When restarting the app from a running or editing a program, restore which file was currently selected.

### Fixed
- Fix jumping to error when selecting "Goto" on BASIC errors (Thanks Mateo).
- Allow "Goto" on locked BASIC programs when the user has "Edit locked programs" enabled.

## [0.67.0] - 2022-10-04

### Added
- Add an option to the CEaShell settings to allow the user to edit locked programs in the shell.
- Added license heading to unlockBasic.asm.

### Fixed
- Fix some bugs in the OS edit menu caused by our hooks.

## [0.66.1] - 2022-10-04

### Added
- Add editing support to the shell, using Cesium's code (slightly modified to work).
- Allow editing of archived programs from the OS [prgm] edit menu, if the user wishes.

### Fixed
- Editing programs too much caused a RAM clear - perfect to ensure that BASIC projects stay small!

## [0.65.1] - 2022-10-02

### Added
- Icon hook now has an option in the settings menu.

### Changed
- Improve GetCSC hook installation code.

## [0.64.4] - 2022-10-01

### Added
- Added descriptions for programs in the program menu.

### Changed
- Move the icons a bit, as suggested by [mateoconlechuga](https://github.com/mateoconlechuga/).
- Make the cursor slightly bigger, so it's easier to see.

### Fixed
- **Re-re-re**-fixed the Stop hook.
- Properly cut off descriptions.
- Properly handle empty program menus.

## [0.63.4] - 2022-09-29

### Added
- Support program icons in the edit menu as well.

### Changed
- Use a flag to optimize program icon drawing even more.

### Fixed
- Fix Stop hook **again**.
- **Actually** fix Stop hook **AGAIN** again.

## [0.62.3] - 2022-09-28

### Added
- Added program icons in the program menu.

### Changed
- For the icon hook, only sort the vat when the [PRGM] key is pressed now, to speed up the process.

### Fixed
- Uninstall TI-BASIC Stop token hook properly.
- Small memory leak running archived basic programs before exiting the shell.

## [0.61.1] - 2022-09-25

### Added
- Create assembly functions to run programs, since the toolchain's don't work with apps.

### Changed
- Re-work things to make CEaShell an app.

### Fixed
- Take care of an issue with detecting free RAM and ROM, and remove a warning.

## [0.60.2] - 2022-09-13

### Added
- All menu options have info blurbs describing the option in greater detail.
- Info box explaining how to exit the theme picker.

### Changed
- Move "Slow" text in transition speed option to the left quite a bit.

## [0.59.0] - 2022-09-13

### Changed
- Switch the locations of the theme picker and options thing.

## [0.58.9] - 2022-09-13

### Added
- All options in the customization menu can be changed now.
- Support for descriptions with monochrome icons.

### Changed
- Update a variable type since warnings are BAD.
- Move some options in the customizing menu a few pixels.

### Fixed
- Fix a bug where deleting from the homescreen deleted the program behind the cursor if "Hide CEaShell" was on.

## [0.57.0] - 2022-09-06

### Changed
- Updated VAT sorting code.

## [0.56.0] - 2022-09-06

### Added
- Create an option to hide/unhide CEaShell on the main screen.
- Implement ASM routine to get correct size.
- Add another option to a non-working customizing menu.

### Fixed
- Fix a small bug with modifying file types.

## [0.55.0] - 2022-09-04

### Added
- Created a function to properly detect file sizes.

## [0.54.0] - 2022-09-03

### Added
- Added some dummy options to the customization menu.

### Fixed
- Correct a bug when toggling file attributes.

## [0.53.5] - 2022-09-02

### Added
- Hook to detect Stop token in TI-BASIC program execution.
- Implement basic alpha searching.
- Create a README file.

### Changed
- Improve scrolling to off-screen locations with alpha searching.
- Appvar type shows up properly in the info menu.
- Declare a lot of variables as constant.

### Fixed
- Invert switch looks a bit better now when inverted and will work with user-made themes.

## [0.52.9] - 2022-08-28

### Added
- Wonky renaming thing.
- Delete confirmation menu.
- Delete files from the homescreen.

### Changed
- Added numbers to renaming input box.

### Fixed
- Fixed a bug where pressing clear on the customization menu didn't exit the shell.
- Display sprites correctly.
- Issue with rounding corners after delete prompt has been corrected.
- Fixed a graphical bug when exiting renaming prompt without renaming.

## [0.51.1] - 2022-08-25

### Added
- Support running programs (but not fully, BASIC programs still need to handle the Stop token)!

### Fixed
- Fixed a bunch of weird stuff with the cursor messing up.
- Debounce after exiting programs.
- Close AppVar after writing to it to prevent memory leak.

## [0.50.2] - 2022-08-24

### Added
- Created an AppVars folder.
- Properly handle AppVar file operations.

## [0.49.0] - 2022-08-23

### Added
- Support Cesium format TI-BASIC icons.

## [0.48.0] - 2022-08-23

### Added
- Added support for 16x16 color, 16x16 monochrome, and 8x8 monochrome icons in BASIC programs.

## [0.47.1] - 2022-08-21

### Added
- Support TI-BASIC descriptions.

### Fixed
- Properly detect the end of the file in TI-BASIC programs.

## [0.46.0] - 2022-08-20

### Added
- Display free RAM and ROM instead of description for files that lack one.

## [0.45.3] - 2022-08-20

### Fixed
- Properly display descriptions for ASM, C, and ICE programs.
- Correct typo.
- Fixed an issue where files without icons or descriptions would crash the calculator.

## [0.44.0] - 2022-08-19

### Added
- Began work on a function to get description information from files.

## [0.40.0] - 2022-08-18

### Added
- More comments in source code.
- Basic support for assembly type icons.
- Redraws on garbage collect.

### Changed
- Sped up hidden program graphics.
- Battery charging updates more frequently.
- Minor assembly optimizations.
- Re-worked the round corners function so that it actually works better.

### Fixed
- Small graphical bugs in the info menu with hidden programs.
- Fixed MORE file operation bugs.

## [0.39.0] - 2022-08-15

### Added
- Hide and unhide programs.
- Lock and unlock programs.

### Fixed
- Fixed deleting hidden programs.

## [0.38.0] - 2022-08-15

### Added
- Check box routine.
- Info menu options (only some are usable).

## [0.37.0] - 2022-08-13

### Added
- Program icon for the shell.
- Buttons in info menu.

### Changed
- Scrolling in theme picker has different behavior.

### Fixed
- Broken round corners function now works properly.

## [0.36.0] - 2022-08-12

### Changed
- Re-organized some code.
- Made the info menu look a bit better.

### Removed
- Unnecessary function for unfilled round rectangle.

## [0.35.0] - 2022-08-11

### Added
- Added hidden program indicator ("transparent" effect on file icon).
- Began work on info menu.

## [0.34.0] - 2022-08-11

### Changed
- Properly display file type.

## [0.33.0] - 2022-08-11

### Changed
- Pressing down at the end of the top row returns to previous column.
- Update to toolchain 10.2.

## [0.32.0] - 2022-08-10

### Added
- ASM routine which figures out the different program types (C, ICE, etc.).

### Changed
- Finish updating to nightly toolchain.
- Description now reflects the current version.

### Removed
- Unneeded variable for keeping track of the number of files drawn.

## [0.31.0] - 2022-08-09

### Changed
- Begin updating to nightly toolchain.

### Fixed
- Battery charging status is displayed properly.

## [0.30.0] - 2022-08-08

### Fixed
- All known scrolling issues have been corrected.
- An issue with displaying names.

## [0.29.0] - 2022-08-08

### Added
- Display program names in status bar.

## [0.28.0] - 2022-08-08

### Changed
- File icon corner looks better.

## [0.27.0] - 2022-08-07

### Fixed
- Issue with flashing status bar.

## [0.26.0] - 2022-08-07

### Added
- Begun work on displaying selected file's name in status bar (It's still a bit broken).
- Implemented partial redraw for better optimization.

### Changed
- Cursor is more responsive.

## [0.25.0] - 2022-08-07

### Fixed
- Cursor behavior has been corrected.

## [0.24.0] - 2022-08-07

### Added
- Up and down cursor movement in the file manager.

### Fixed
- Left and right scrolling issues.

## [0.23.0] - 2022-08-07

### Added
- Left and right file scrolling.

## [0.22.0] - 2022-08-07

### Changed
- Use a better method for detecting keypresses, with timer-based debouncing.

## [0.21.0] - 2022-08-07

### Added
- Account for hidden program names and correct the first character when displaying.

## [0.20.0] - 2022-08-07

### Fixed
- Programs now display in the proper order.
- Starting location of files.
- Other minor issues

## [0.19.0] - 2022-08-07

### Fixed
- Battery colors have been corrected.

## [0.18.0] - 2022-08-06

### Fixed
- Issue with redrawing the background during transition animations.

## [0.17.0] - 2022-08-06

### Added
- Begin drawing files.

## [0.16.0] - 2022-08-06

### Added
- Refresh clock.
- VAT sorting algorithm from [here](https://github.com/mateoconlechuga/cesium/blob/b7ac7118f7f63755557d9cea316d0d398010332d/src/sort.asm)

## [0.15.0] - 2022-08-06

### Added
- Implement alternate icon colors.
- Default file icons.
- More functions for various utilities.

### Changed
- Improved theme menu.

## [0.14.0] - 2022-07-13

### Added
- Created light versions of various icons.

### Fixed
- Correctly account for 24-Hour time.

## [0.13.0] - 2022-07-12

### Changed
- Cleaned up and organized the code.
- Slight optimizations.

## [0.12.0] - 2022-07-11

### Changed
- Invert palette code has now been written in assembly.

## [0.11.0] - 2022-07-11

### Changed
- Finish fixing formatting.

## [0.10.0] - 2022-07-11

### Changed
- Begin fixing formatting.

## [0.9.0] - 2022-07-11

### Added
- Finish implementing theme customization.

## [0.8.0] - 2022-07-10

### Added
- Implemented cursor in the theme picker menu.

## [0.7.0] - 2022-07-08

### Added
- Created a system for menus.
- Began work on customization menu.

## [0.6.0] - 2022-07-08

### Added
- Transition animations between menus.

### Changed
- Tidied up word-wrap function for descriptions.

## [0.5.0] - 2022-07-05

### Added
- Created a pill at the bottom of the screen displaying info of the currently selected file.
- Word-wrap function for file descriptions.

## [0.4.0] - 2022-07-04

### Added
- Round rectangle function.
- Created various icons for use in CEaShell.

### Changed
- Optimizations to various UI elements.

## [0.3.0] - 2022-07-03

### Added
- Added status bar with clock, battery, and current file / menu indicators.

## [0.2.0] - 2022-07-02

### Added
- Fill the screen with a background color.

## [0.1.0] - 2022-07-02

### Added
- Initial commit.
