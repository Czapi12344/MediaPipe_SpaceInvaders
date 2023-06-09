// Copyright 2019 The MediaPipe Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#import "MPPInputSource.h"

/// A source that outputs frames from a video, played in real time.
/// Not meant for batch processing of video.
@interface MPPPlayerInputSource : MPPInputSource

/// Initializes the video source with optional audio processing.
///
/// @param video The video asset to play.
/// @param audioProcessingEnabled If set, indicates that the (first) audio track
///        should be processed if it exists, and the corresponding methods for
///        audio will be invoked on the @c delegate.
- (instancetype)initWithAVAsset:(AVAsset*)video audioProcessingEnabled:(BOOL)audioProcessingEnabled;

/// Initializes the video source to process @c video with audio processing disabled.
- (instancetype)initWithAVAsset:(AVAsset*)video;

/// Skip into video @c time from beginning (time 0), within error of +/- tolerance to closest time.
- (void)seekToTime:(CMTime)time tolerance:(CMTime)tolerance;

/// Set time into video at which to end playback.
- (void)setPlaybackEndTime:(CMTime)time;

/// Returns the current video's timestamp.
- (CMTime)currentPlayerTime;

@end
