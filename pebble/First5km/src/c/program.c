#include "include.h"

#define R ((short) (1 << 11))
#define W ((short) (1 << 12))
#define T ((short) (1 << 13))
#define TR ((short) (R | T))
#define TW ((short) (W | T))
#define TIME_MASK ((short) (R - 1))

// r = run, R = run back
// w = walk, W = walk back
static short _program[] = {
	// Debug - only enable this during development :-)
	//4, (short) (R|10), (short) (W|10), (short) (TR|10), (short) (TW|10),

	// week 1
	6, (short) (R|150), (short) (W|150), (short) (R|90), (short) (TR|90), (short) (TW|150), (short) (TR|150),
	6, (short) (R|150), (short) (W|150), (short) (R|90), (short) (TR|90), (short) (TW|150), (short) (TR|150),
	6, (short) (R|270), (short) (W|150), (short) (R|150), (short) (TR|150), (short) (TW|150), (short) (TR|270),

	// Week 2
	6, (short) (R|180), (short) (W|120), (short) (R|90), (short) (TR|90), (short) (TW|120), (short) (TR|180),
	6, (short) (R|300), (short) (W|120), (short) (R|150), (short) (TR|150), (short) (TW|120), (short) (TR|300),
	6, (short) (R|270), (short) (W|120), (short) (R|180), (short) (TR|180), (short) (TW|120), (short) (TR|270),

	// Week 3
	6, (short) (R|270), (short) (W|120), (short) (R|180), (short) (TR|180), (short) (TW|120), (short) (TR|270),
	8, (short) (R|180), (short) (W|120), (short) (R|180), (short) (W|60), (short) (TW|60), (short) (TR|180), (short) (TW|120), (short) (TR|180),
	6, (short) (R|390), (short) (W|45), (short) (R|45), (short) (TR|45), (short) (TW|45), (short) (TR|390),

	// Week 4
	6, (short) (R|300), (short) (W|120), (short) (R|150), (short) (TR|150), (short) (TW|120), (short) (TR|300),
	8, (short) (R|180), (short) (W|120), (short) (R|180), (short) (W|60), (short) (TW|60), (short) (TR|180), (short) (TW|120), (short) (TR|180),
	8, (short) (R|300), (short) (W|120), (short) (R|300), (short) (W|60), (short) (TW|60), (short) (TR|300), (short) (TW|120), (short) (TR|300),

	// Week 5
	4, (short) (R|420), (short) (W|90), (short) (TW|90), (short) (TR|420),
	8, (short) (R|300), (short) (W|120), (short) (R|300), (short) (W|60), (short) (TW|60), (short) (TR|300), (short) (TW|120), (short) (TR|300),
	4, (short) (R|600), (short) (W|120), (short) (TW|120), (short) (TR|600),

	// Week 6
	4, (short) (R|600), (short) (W|90), (short) (TW|90), (short) (TR|600),
	8, (short) (R|300), (short) (W|120), (short) (R|300), (short) (W|60), (short) (TW|60), (short) (TR|300), (short) (TW|120), (short) (TR|300),
	4, (short) (R|600), (short) (W|90), (short) (TW|90), (short) (TR|600),

	// Week 7
	3, (short) (R|720), (short) (TW|120), (short) (TR|480),
	8, (short) (R|300), (short) (W|120), (short) (R|300), (short) (W|60), (short) (TW|60), (short) (TR|300), (short) (TW|120), (short) (TR|300),
	4, (short) (R|600), (short) (W|90), (short) (TW|90), (short) (TR|600),

	// Week 8
	3, (short) (R|720), (short) (TW|180), (short) (TR|480),
	8, (short) (R|300), (short) (W|120), (short) (R|300), (short) (W|60), (short) (TW|60), (short) (TR|300), (short) (TW|120), (short) (TR|300),
	4, (short) (R|720), (short) (TR|180), (short) (TW|180), (short) (TR|300),

	// Week 9
	4, (short) (R|600), (short) (TR|420), (short) (TW|60), (short) (TR|180),
	6, (short) (R|420), (short) (W|120), (short) (R|210), (short) (TR|210), (short) (TW|120), (short) (TR|420),
	3, (short) (R|900), (short) (TW|120), (short) (TR|300),

	// Week 10
	4, (short) (R|600), (short) (TR|300), (short) (TW|120), (short) (TR|300),
	6, (short) (R|420), (short) (W|120), (short) (R|210), (short) (TR|210), (short) (TW|120), (short) (TR|420),
	4, (short) (R|600), (short) (W|60), (short) (TW|60), (short) (TR|600),

	// Week 11
	3, (short) (R|900), (short) (TW|120), (short) (TR|600),
	2, (short) (R|600), (short) (TR|600),
	2, (short) (R|600), (short) (TR|600),

	// Week 12
	2, (short) (R|600), (short) (TR|600),
	2, (short) (R|600), (short) (TR|600),
	2, (short) (R|600), (short) (TR|600),

	// Week 13
	2, (short) (R|780), (short) (TR|720),
	2, (short) (R|480), (short) (TR|420),
	2, (short) (R|600), (short) (TR|600),

	// Week 14
	2, (short) (R|600), (short) (TR|600),
	2, (short) (R|780), (short) (TR|720),
	2, (short) (R|690), (short) (TR|690),

	// Week 15
	2, (short) (R|780), (short) (TR|720),
	2, (short) (R|900), (short) (TR|900),
	2, (short) (R|840), (short) (TR|840),

	// Week 16
	2, (short) (R|780), (short) (TR|720),
	2, (short) (R|900), (short) (TR|900),
	2, (short) (R|900), (short) (TR|900),

	// If we ever go too far
	0
};
static short _day_index = 0;
static short _run_index;
static short _activities;

void program_select_day(short day)
{
	short index = 0;

	for (; day > 0; day--) {
		index += _program[index] + 1;
	}
	
	_day_index = index;
}

void program_get_times(short *total_time, short *run_time)
{
	short index = _day_index, cnt;

	*total_time = 0;
	*run_time = 0;
	cnt = _program[index++];
	for (; cnt > 0; cnt--) {
		if ((_program[index] & R) == R) {
			*total_time += _program[index] & TIME_MASK;
			*run_time += _program[index] & TIME_MASK;
		} else if ((_program[index] & W) == W) {
			*total_time += _program[index] & TIME_MASK;
		}
		index++;
	}
}

void program_start_run()
{
	_run_index = 0;
	_activities = _program[_day_index];
}

short program_next_activity()
{
	_run_index++;
	if (_run_index <= _activities) {
		return _program[_day_index + _run_index];
	}

	return -1;
}

void program_get_internals(short *activities, short *run_index, short *day_index)
{
	*activities = _activities;
	*run_index = _run_index;
	*day_index = _day_index;
}

short program_time_from_activity(short activity)
{
	return (short) (activity & TIME_MASK);
}

bool program_is_run(short activity)
{
	return (activity & R) == R;
}

bool program_is_walk(short activity)
{
	return (activity & W) == W;
}

bool program_is_turned(short activity)
{
	return (activity & T) == T;
}

bool program_is_done(short activity)
{
	return activity == -1;
}
