#include <stdio.h>
#include "happyTime.h"

int main() {
    TgBot::Bot bot(BOT_TOKEN);

        bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
            bot.getApi().sendMessage(message->chat->id, "Привіт, цей бот створений для людей, які забувають про день народження! Щоб почати використовувати цього бота, вам потрібно натиснути меню біля чату!");
    });



    bot.getEvents().onCommand("writeinformation", [&bot](TgBot::Message::Ptr message) {
        std::istringstream iss(message->text);
        std::string command, month1, day1, name1;
        iss >> command >> month1 >> day1 >> name1;

        int64_t id2 = message->from->id;
        std::cout << "Extracted ID: " << id2 << std::endl;

        if (month1.empty() || day1.empty() || name1.empty()) {
            bot.getApi().sendMessage(message->chat->id, "Ви написали неправильну команду. Приклад: (/writeinformation <місяць> <день> <назва>). приклад: /writeinformation 7 27 Діма");
        } else {
            try {
                int month = std::stoi(month1);
                int day = std::stoi(day1);
                bool changenumber = false; 

                if (equalplus(month, 1) && equalminus(month, 12)) {
                    changenumber = true;
                } else {
                    changenumber = false;
                    bot.getApi().sendMessage(message->chat->id, "Введіть місяць від 1 до 12");                    
                }           

                if (changenumber && equalplus(day, 1) && equalminus(day, 31)) {
                    WriteInformation(month, day, name1, id2);
                    bot.getApi().sendMessage(message->chat->id, "Інформація написана успішно.");
                    changenumber = false;
                } else if (changenumber) {
                    bot.getApi().sendMessage(message->chat->id, "Тип дня з 1 по 31.");
                }

            } catch (const std::invalid_argument &e) {
                bot.getApi().sendMessage(message->chat->id, "Недійсний формат числа. Переконайтеся, що ви правильно ввели цифри.");
            } catch (const std::out_of_range &e) {
                bot.getApi().sendMessage(message->chat->id, "Номер поза діапазоном. Будь ласка, введіть дійсні числа.");
            }
        }
    });
    
    bot.getEvents().onCommand("information", [&bot](TgBot::Message::Ptr message) {
        try {
            pqxx::connection C(connector());
            time_t now = time(0);
            tm *current_time = localtime(&now);
            int64_t id2 = message->from->id;
            bool boolnumber1 = false;



            std::string information = "SELECT * FROM users WHERE tg_user_id = " + std::to_string(id2) + ";";
            pqxx::nontransaction N(C);
            pqxx::result R(N.exec(information));

            std::tm current_date = {};


            if (R.empty()) {
                bot.getApi().sendMessage(message->chat->id, "Для цього користувача не знайдено інформації.");
            } else {
                for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c) {
                    int month = c[1].as<int>() - 1;
                    int day = c[2].as<int>();

                    if (month < current_time->tm_mon || day < current_time->tm_mday){
                        boolnumber1 = true;
                    }else{
                        boolnumber1 = false;
                    }

                    std::tm current_date = {};
                        current_date.tm_year = current_time->tm_year;
                        current_date.tm_mon = current_time->tm_mon;
                        current_date.tm_mday = current_time->tm_mday;
    
                    std::tm birthday_date = {};
                        birthday_date.tm_year = current_time->tm_year + boolnumber1;
                        birthday_date.tm_mon = month;
                        birthday_date.tm_mday = day;

                    int days_between = calculateDaysBetween(current_date, birthday_date);
                    string happybirthday;

                    if(equalplus(days_between,1) && equalminus(days_between,365)){
                        happybirthday = std::to_string(days_between);
                    } else{
                        happybirthday = "Happy Birthday  🎉🎉🎉";
                    }        
                                   
                    bot.getApi().sendMessage(message->chat->id, "ІМ'Я = " + c[3].as<std::string>());
                    bot.getApi().sendMessage(message->chat->id, "МІСЯЦЬ = " + std::to_string(c[1].as<int>()));
                    bot.getApi().sendMessage(message->chat->id, "ДЕНЬ = " + std::to_string(c[2].as<int>()));
                    bot.getApi().sendMessage(message->chat->id, "ЗАЛИШИТЬСЯ = " + happybirthday);
                    bot.getApi().sendMessage(message->chat->id, "­");

                }
            }
            C.disconnect();
        } catch (const std::exception &e) {
            bot.getApi().sendMessage(message->chat->id, "Error: " + std::string(e.what()));
            }
        });



    bot.getEvents().onCommand("delete", [&bot](TgBot::Message::Ptr message) {
        pqxx::connection C(connector());
        std::istringstream iss(message->text);
        std::string command, name;
        iss >> command >> name;
        int64_t id2 = message->from->id;
        pqxx::work W(C);

        if (name.empty()) {
            bot.getApi().sendMessage(message->chat->id, "Ви не ввели ім'я");
        } else {
            std::string info = "DELETE FROM users WHERE tg_user_id = " + std::to_string(id2) + " AND name = '" + name + "';";
            W.exec(info);
            W.commit();
            bot.getApi().sendMessage(message->chat->id, "Користувач з іменем " + name + " видалено.");
        }
        C.disconnect();
    });



    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }
    return 0;
}