/*
 Copyright (c) 2013 Alun Bestor and contributors. All rights reserved.
 This source file is released under the GNU General Public License 2.0. A full copy of this license
 can be found in this XCode project at Resources/English.lproj/BoxerHelp/pages/legalese.html, or read
 online at [http://www.gnu.org/licenses/gpl-2.0.txt].
 */


#import <QuartzCore/QuartzCore.h>
#import "BXFrameRenderingView.h"

@class BXVideoFrame;
@class BXBasicRenderer;

/// \c BXRenderingLayer is the OpenGL layer used by BXLayerBackedRenderingView for rendering its content.
@interface BXRenderingLayer : CAOpenGLLayer

@property (nonatomic) BXRenderingStyle renderingStyle;
@property (strong, readonly, nonatomic) BXVideoFrame *currentFrame;

- (void) updateWithFrame: (BXVideoFrame *)currentFrame;
- (BOOL) supportsRenderingStyle: (BXRenderingStyle)style;
- (NSSize) maxFrameSize;

@end
