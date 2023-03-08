#include "dateManager.h"

date::year_month_day dateManager::convert(std::string dateString){
    std::istringstream in{dateString};
    date::year_month_day tempDate;
    in >> date::parse("%d-%m-%Y", tempDate);
    return tempDate;
}

void dateManager::inc_day(date::year_month_day& date_, int days_){
    date_ = date::sys_days(date_) + date::days{days_};
}

bool dateManager::isBigger(date::year_month_day date1, date::year_month_day date2){
    return date::sys_days(date1) > date::sys_days(date2);
}

std::string dateManager::getString(date::year_month_day date_){
    std::ostringstream out;
    out << std::setw(2) << unsigned{date_.day()} << "-"
    << std::setw(2) << unsigned{date_.month()}<< "-"
    << date_.year();
    return out.str();
}