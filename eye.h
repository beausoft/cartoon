#pragma once
#include "vector2.h"
#include <chrono>
using namespace std;
using namespace chrono;

#define EYE_AP0 Vector2(2., 181.).Div_(Vector2(323., 323.))
#define EYE_AP1 Vector2(8., 0.).Div_(Vector2(323., 323.))
#define EYE_AP2 Vector2(140., 29.).Div_(Vector2(323., 323.))
#define EYE_AP3 Vector2(159., 189.).Div_(Vector2(323., 323.))

#define EYE_BP0 Vector2(159., 189.).Div_(Vector2(323., 323.))
#define EYE_BP1 Vector2(167., 291.).Div_(Vector2(323., 323.))
#define EYE_BP2 Vector2(0., 323.).Div_(Vector2(323., 323.))
#define EYE_BP3 Vector2(2., 181.).Div_(Vector2(323., 323.))

#define EYE_KERNEL Vector2(130., 205.).Div_(Vector2(323., 323.))

#define EYE_AP1_TO Vector2(10., 79.).Div_(Vector2(323., 323.))
#define EYE_AP2_TO Vector2(159., 88.).Div_(Vector2(323., 323.))

#define EYE_BP1_TO Vector2(160., 292.).Div_(Vector2(323., 323.))
#define EYE_BP2_TO Vector2(2., 310.).Div_(Vector2(323., 323.))

typedef void (*AnimationCallback)(float, bool);

typedef struct tagTrack
{
	long start;
	long end;
	bool finished;
	AnimationCallback callback;
}ANIMATION_TRACK, *LPANIMATION_TRACK;

class AnimationSchedule {
public:
	AnimationSchedule(long duration);
	~AnimationSchedule();
	bool start();
	bool stop();
	void addTrack(long start, long end, AnimationCallback callback);
	bool update();

private:
	long mDuration;
	system_clock::time_point mStartTime;
	bool mRunning;
	LPANIMATION_TRACK* mTrack;
	int mTrackCount;
};
