#include "eye.h"

AnimationSchedule::AnimationSchedule(long duration) {
	mDuration = duration;
	mStartTime = 0;
	mRunning = false;
	mTrack = new ANIMATION_TRACK[10];
	mTrackCount = 0;
}

void AnimationSchedule::start() {

}

void AnimationSchedule::stop() {

}

void AnimationSchedule::addTrack(long start, long end, AnimationCallback callback) {
	// mTrack[mTrackCount++] = new ANIMATION_TRACK{ start, end, false, callback };
}

void AnimationSchedule::update() {

}

AnimationSchedule::~AnimationSchedule() {
	
}

