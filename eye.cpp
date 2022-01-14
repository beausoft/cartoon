#include "eye.h"

AnimationSchedule::AnimationSchedule(long duration) {
	mDuration = duration;
	mStartTime = system_clock::now();
	mRunning = false;
	mTrack = new LPANIMATION_TRACK[10];
	mTrackCount = 0;
}

bool AnimationSchedule::start() {
	if (!mRunning) {
		mStartTime = system_clock::now();
		mRunning = true;
		return true;
	}
	return false;
}

bool AnimationSchedule::stop() {
	if (mRunning) {
		mRunning = false;
		for (int i = 0; i < mTrackCount; i++) {
			mTrack[i]->callback(0.0f, mRunning);
			mTrack[i]->finished = false;
		}
		return true;
	}
	return false;
}

void AnimationSchedule::addTrack(long start, long end, AnimationCallback callback) {
	mTrack[mTrackCount++] = new ANIMATION_TRACK{ start, end, false, callback };
}

bool AnimationSchedule::update() {
	if (mRunning) {
		auto endTime = system_clock::now();
		auto duration = duration_cast<milliseconds>(endTime - mStartTime);
		long t = long(duration.count());

		if (t > mDuration) {
			t = mDuration;
		}
		for (int i = 0; i < mTrackCount; i++) {
			if (mTrack[i]->start <= t && mTrack[i]->end > t) {
				float alpha = (float(t) - mTrack[i]->start) / (mTrack[i]->end - mTrack[i]->start);
				mTrack[i]->callback(alpha, mRunning);
			}
			else if (mTrack[i]->end <= t && !mTrack[i]->finished) {
				mTrack[i]->callback(1.0f, mRunning);
				mTrack[i]->finished = true;
			}
		}
		if (t == mDuration) {
			stop();
		}
		return true;
	}
	return false;
}

AnimationSchedule::~AnimationSchedule() {
	for (int i = 0; i < mTrackCount; i++) {
		delete mTrack[i];
	}
	delete mTrack;
}
