#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H
#include <iostream>
#include <sstream>
#include <iomanip>
#include <date/date.h>

class dateManager{
public:
    date::year_month_day convert(std::string dateString);
    void incDays(date::year_month_day& date_, int days_);
    bool isBigger(date::year_month_day date1, date::year_month_day date2);
    std::string getString(date::year_month_day date_);
};






#endif //DATA_MANAGER_H