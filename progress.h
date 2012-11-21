#ifndef _PROGRESS_H_
#define _PROGRESS_H_

struct progress
{
	void (*progress_callback)(double fraction);
};

#endif
