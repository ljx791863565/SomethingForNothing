#ifndef DATE_H
#define DATE_H

#include "copyable.h"
#include "types.h"

struct tm;

namespace muduo {

class Date : public muduo::copyable {
public:
	static const int kDaysPerWeek = 7;
	static const int kJulianDayNumber_1970_01_01;

	Date()
		: julianDayNumber_(0)
	{}

	explicit Date(int julianDayNumber)
		: julianDayNumber_(julianDayNumber)
	{}

	explicit Date(const struct tm &t);

	Date(int year, int month, int day);

	void swap(Date &d)
	{
		std::swap(julianDayNumber_, d.julianDayNumber_);
	}

	bool valid() const { return julianDayNumber_ > 0; }

	int julianDayNumber() const { return julianDayNumber_; }

	int weekDay() const
	{
		return (julianDayNumber_ + 1) % kDaysPerWeek;
	}

	string toString() const;

	struct YMD {
		int year;
		int month;
		int day;
	};

	YMD ymd() const;

	int year() const { return ymd().year; }
	int month() const { return ymd().month; }
	int day() const { return ymd().day; }

private:
	int julianDayNumber_;
};

inline bool operator<(const Date &lhs, const Date &rhs)
{
	return lhs.julianDayNumber() < rhs.julianDayNumber();
}

inline bool operator==(const Date &lhs, const Date &rhs)
{
	return lhs.julianDayNumber() < rhs.julianDayNumber();
}

} /* namespace muduo */

#endif /* DATE_H */
