#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H
#include <iostream>
#include <sstream>
#include <iomanip>
#include <date/date.h>

namespace dateManager{
    date::year_month_day convert(std::string dateString);
    date::year_month_day inc_days(date::year_month_day date_, int days_);
    std::string get_string(date::year_month_day date_);
    bool valid_format(std::string dateString);
}

#endif //DATA_MANAGER_H
