#include <iostream>
#include <ctime>
#include <string>
#include <pqxx/pqxx>
#include <tgbot/tgbot.h>
#include <sstream>
#include <stdexcept>
#include <chrono>

using namespace std;
using namespace pqxx;






string BOT_TOKEN = "Insert token";

struct Birthday {
    int day;
    int month;
};

struct happyTime{
    int day;
    int month;
};

string connector(){
  return "Insert postgresql database";

bool equalplus(int x, int y){
  return x > y || x == y;
}

int equalminus(int z, int c){
  return z < c || z == c;
}

bool isHappybirthDay (Birthday birthday){
  time_t now = time(0);
  tm *current_time = localtime(&now);
  return birthday.month == current_time->tm_mon + 1 && birthday.day == current_time->tm_mday;
}

int calculateDaysBetween(const std::tm& start_date, const std::tm& end_date) {
    std::chrono::system_clock::time_point start = std::chrono::system_clock::from_time_t(std::mktime(const_cast<std::tm*>(&start_date)));
    std::chrono::system_clock::time_point end = std::chrono::system_clock::from_time_t(std::mktime(const_cast<std::tm*>(&end_date)));

    auto duration = std::chrono::duration_cast<std::chrono::hours>(end - start).count();
    return duration / 24;
};


void WriteInformation(const int MONTH, const int DAY, const std::string &NAME, int64_t TGID) {
    try {
        pqxx::connection C(connector());
        pqxx::work W(C);

        std::string sql = "INSERT INTO users (month, day, name, tg_user_id) VALUES ($1, $2, $3, $4)";
        W.exec_params(sql, MONTH, DAY, NAME, TGID);

        W.commit();
        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}


void surprise(Birthday birthday){
  if (isHappybirthDay(birthday)) std::cout<<"Happy Birtday!"<<endl;
}

