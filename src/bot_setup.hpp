#ifndef BOT_SETUP_HPP
#define BOT_SETUP_HPP

#include <config.hpp>
#include <logging.hpp>
#include <handlers.hpp>
#include <keyboard_utils.hpp>

#include <signal.h>
#include <iostream>

#include <tgbot/Bot.h>
#include <tgbot/net/TgLongPoll.h>

void setup_bot()
{
    try
    {
        config::init();

        signal(
            SIGINT, 
            [](int) 
            {
                logging::log_info("Bot was successfully shut down!");
                exit(0);
            });

        TgBot::Bot bot(config::bot_token);

        keyboard::markup::construct_markups();
        handlers::register_handlers(bot);

        bot.getApi().deleteWebhook();

        TgBot::TgLongPoll long_poll(bot);

        logging::log_info("Bot was successfully started!");

        while (true) 
        {
            try
            {
                long_poll.start();
            }
            catch(const std::exception& e)
            {
                logging::log_error(e.what());
            }
        }
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
        logging::log_info("Bot was unexpectedly shut down due to an error!");
    }
}

#endif