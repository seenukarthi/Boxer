/* 
 Copyright (c) 2013 Alun Bestor and contributors. All rights reserved.
 This source file is released under the GNU General Public License 2.0. A full copy of this license
 can be found in this XCode project at Resources/English.lproj/BoxerHelp/pages/legalese.html, or read
 online at [http://www.gnu.org/licenses/gpl-2.0.txt].
 */


//BXImportSession is a BXSession document subclass which manages the importing of a new game
//from start to finish.
//Besides handling the emulator session that runs the game installer, BXImportSession also
//prepares a new gamebox and manages the drag-drop wizard which bookends (or in many cases
//comprises) the import process.

#import "BXSession.h"
#import "ADBOperation.h"
#import "ADBOperationDelegate.h"

#pragma mark -
#pragma mark Class constants

//Constants returned by importStage
typedef NS_ENUM(NSInteger, BXImportStage) {
	BXImportSessionWaitingForSource,
	BXImportSessionLoadingSource,
	BXImportSessionWaitingForInstaller,
	BXImportSessionReadyToLaunchInstaller,
	BXImportSessionRunningInstaller,
	BXImportSessionReadyToFinalize,
	BXImportSessionImportingSourceFiles,
	BXImportSessionCancellingSourceFileImport,
	BXImportSessionCleaningGamebox,
	BXImportSessionFinished
};

//Constants returned by sourceFileImportType
typedef NS_ENUM(NSInteger, BXSourceFileImportType) {
	BXImportTypeUnknown,                //!< We haven't decided yet what we're importing from
	BXImportFromPreInstalledGame,		//!< Bundling a pre-installed game folder into a gamebox
	BXImportFromCDVolume,				//!< Coping from a real CD
	BXImportFromFloppyVolume,			//!< Copying from a real floppy disk
	BXImportFromCDImage,				//!< Copying from a CD image
	BXImportFromFloppyImage,			//!< Copying from a floppy-disk image
	BXImportFromHardDiskImage,			//!< Copying from a hard-disk image (currently unused)
	BXImportFromFolderToCD,				//!< Converting a folder of game files into a fake CD
	BXImportFromFolderToFloppy,			//!< Converting a folder of game files into a fake floppy
	BXImportFromFolderToHardDisk,		//!< Converting a folder of game files into a secondary hard disk
};


@class BXImportWindowController;
@class BXEmulatorConfiguration;
@protocol BXDriveImport;

/// \c BXImportSession is a BXSession document subclass which manages the importing of a new game
/// from start to finish.
///
/// Besides handling the emulator session that runs the game installer, \c BXImportSession also
/// prepares a new gamebox and manages the drag-drop wizard which bookends (or in many cases
/// comprises) the import process.
@interface BXImportSession : BXSession <ADBOperationDelegate>
{
	NSURL *_sourceURL;
	BXImportWindowController *_importWindowController;
	NSArray *_installerURLs;
    
    NSURL *_bundledConfigurationURL;
    BXEmulatorConfiguration *_configurationToImport;
	
	NSURL *_rootDriveURL;
	
	BXImportStage _importStage;
	ADBOperationProgress _stageProgress;
	BOOL _stageProgressIndeterminate;
	
	BOOL _hasAutoGeneratedIcon;
	BOOL _didMountSourceVolume;
	
	ADBOperation *_sourceFileImportOperation;
	BXSourceFileImportType _sourceFileImportType;
	BOOL _sourceFileImportRequired;
}

#pragma mark -
#pragma mark Properties

/// The window controller which manages the import window, as distinct from the DOS session window.
@property (retain, nonatomic) BXImportWindowController *importWindowController;


/// The source path from which we are installing the game.
@property (copy, nonatomic) NSURL *sourceURL;

/// The path of the DOSBox configuration file we will use for guiding the import.
/// Populated when a suitable configuration file is found in the source path.
@property (copy, nonatomic) NSURL *bundledConfigurationURL;
@property (retain, nonatomic) BXEmulatorConfiguration *configurationToImport;

/// The range of possible DOS installers to choose from, ordered by preference.
@property (retain, nonatomic) NSArray *installerURLs;

/// The display filename of the gamebox, minus extension.
/// Changing this will rename the gamebox file itself.
@property (retain, nonatomic) NSString *gameboxName;


/// What stage of the import process we are up to (as a BXImportStage constant.)
@property (readonly, nonatomic) BXImportStage importStage;

/// How far through the current stage we have progressed.
///
/// Only relevant during the \c BXImportSessionLoadingSourcePath and \c BXImportSessionImportingSourceFiles stages.
@property (readonly, nonatomic) ADBOperationProgress stageProgress;

///Whether our progress cannot be meaningfully determined currently.
///
///Only relevant during the \c BXImportSessionLoadingSourcePath and \c BXImportSessionImportingSourceFiles stages.
@property (readonly, nonatomic) BOOL stageProgressIndeterminate;

/// The final import/file transfer operation being performed to import the game's source files into the gamebox.
/// Only relevant during the BXImportSessionImportingSourceFiles stage.
@property (readonly, retain, nonatomic) ADBOperation *sourceFileImportOperation;

/// The specific way we are importing the game's source files into the gamebox.
/// This affects descriptions in the progress UI and confirmation prompts.
@property (readonly, nonatomic) BXSourceFileImportType sourceFileImportType;

/// Whether it is necessary to import the source files into the gamebox after installation.
/// If YES, then the user will not be able to skip the final importing step.
@property (assign, nonatomic) BOOL sourceFileImportRequired;


#pragma mark -
#pragma mark Import helper methods

/// The UTIs of filetypes we can accept for import.
+ (NSSet<NSString*> *)acceptedSourceTypes;

/// Whether the specified source path is a folder, volume or image type we can import.
+ (BOOL) canImportFromSourceURL: (NSURL *)sourceURL;

/// Whether the game has installed any files into the gamebox's C drive.
///
/// Only relevant after the game's installer has been run.
@property (readonly) BOOL gameDidInstall;

/// Whether the DOS session is currently running an installer program.
///
/// Used for toggling the display of the installation help bar versus the this-program-is-default bar.
@property (readonly,getter=isRunningInstaller) BOOL runningInstaller;


/// Generate a new bootleg cover-art icon and add it to the gamebox.
///
/// This icon will be based on the gamebox's name and the size and age of the files being imported.
- (void) generateBootlegIcon;


#pragma mark -
#pragma mark Import steps

/// Selects the specified source URL, detects the installers available in the source URL,
/// and continues to the next step of importing.
- (void) importFromSourceURL: (NSURL *)URL;

/// Cancels an in-progress installer scan started from importFromSourcePath:.
- (void) cancelInstallerScan;

/// Clears any previously-specified source URL and returns to the source selection step.
- (void) cancelSourceSelection;

/// Launches the specified DOS installer program to continue importing.
- (void) launchInstallerAtURL: (NSURL *)URL;

/// Skips the installer selection process and continues to the next step of importing.
- (void) skipInstaller;

/// Closes the DOS installer process and continues to the next step of importing.
- (void) finishInstaller;

/// Copy the source files into the gamebox.
- (void) importSourceFiles;

/// Cancel an in-progress source-file import.
- (void) cancelSourceFileImport;

/// Clean up the gamebox and finish the operation.
- (void) finalizeGamebox;


#pragma mark -
#pragma mark Notifications

/// Custom progress callbacks for sourceFileImportOperation.
- (void) sourceFileImportInProgress: (NSNotification *)notification;
- (void) sourceFileImportDidFinish: (NSNotification *)notification;

/// Custom progress callbacks for our installer scanner.
- (void) installerScanDidFinish: (NSNotification *)notification;

@end
