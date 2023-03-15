#include "dateManager.h"

date::year_month_day dateManager::convert(std::string dateString){
    std::istringstream in{dateString};
    date::year_month_day tempDate;
    in >> date::parse("%d-%m-%Y", tempDate);
    return tempDate;
}

date::year_month_day dateManager::inc_days(date::year_month_day date_, int days_){
    return date::sys_days(date_) + date::days{days_};
}



std::string dateManager::get_string(date::year_month_day date_){
    std::ostringstream out;
    out.fill('0');
    out << std::setw(2) << unsigned{date_.day()} << "-"
    << std::setw(2) << unsigned{date_.month()}<< "-"
    << date_.year();
    return out.str();
}

bool dateManager::valid_format(std::string dateString){
    std::istringstream in{dateString};
    date::year_month_day tempDate;
    in >> date::parse("%d-%m-%Y", tempDate);
    return in.fail() != true;
}
