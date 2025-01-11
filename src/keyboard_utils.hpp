#ifndef KEYBOARD_UTILS_HPP
#define KEYBOARD_UTILS_HPP

#include <tgbot/types/ReplyKeyboardMarkup.h>
#include <tgbot/types/InlineKeyboardMarkup.h>

namespace keyboard
{
    namespace markup
    {
        namespace reply
        {
            inline auto main = std::make_shared<TgBot::ReplyKeyboardMarkup>();
        }

        namespace inline_
        {
            inline auto system_info = std::make_shared<TgBot::InlineKeyboardMarkup>();
            inline auto return_to_system_info = std::make_shared<TgBot::InlineKeyboardMarkup>();
            inline auto return_to_pm2_logs = std::make_shared<TgBot::InlineKeyboardMarkup>();
            inline auto return_to_process_workload = std::make_shared<TgBot::InlineKeyboardMarkup>();
            inline auto process_workload = std::make_shared<TgBot::InlineKeyboardMarkup>();
            inline auto system_controls = std::make_shared<TgBot::InlineKeyboardMarkup>();
            inline auto return_to_system_controls = std::make_shared<TgBot::InlineKeyboardMarkup>();
            inline auto return_to_pm2_tools = std::make_shared<TgBot::InlineKeyboardMarkup>();
            inline auto pm2_tools = std::make_shared<TgBot::InlineKeyboardMarkup>();
            inline auto reboot_system_verification = std::make_shared<TgBot::InlineKeyboardMarkup>();
            inline auto shutdown_system_verification = std::make_shared<TgBot::InlineKeyboardMarkup>();

            TgBot::InlineKeyboardMarkup::Ptr construct_pm2_logs_processes(
                const std::vector<std::pair<size_t, std::string>>& processes_data);

            TgBot::InlineKeyboardMarkup::Ptr construct_pm2_tools_processes(
                const std::vector<std::pair<size_t, std::string>>& processes_data,
                const std::string& callback_data_starts_with);
        }

        void construct_markups();
    }

    namespace button
    {
        namespace reply
        {
            inline auto system_info = std::make_shared<TgBot::KeyboardButton>();
            inline auto system_controls = std::make_shared<TgBot::KeyboardButton>();
        }

        namespace inline_
        {
            inline auto pm2_status = std::make_shared<TgBot::InlineKeyboardButton>();
            inline auto pm2_logs = std::make_shared<TgBot::InlineKeyboardButton>();
            inline auto system_metrics = std::make_shared<TgBot::InlineKeyboardButton>();
            inline auto process_workload = std::make_shared<TgBot::InlineKeyboardButton>();
            inline auto reboot_required = std::make_shared<TgBot::InlineKeyboardButton>();
            inline auto updates_available = std::make_shared<TgBot::InlineKeyboardButton>();

            inline auto return_to_system_info = std::make_shared<TgBot::InlineKeyboardButton>();

            inline auto return_to_pm2_logs = std::make_shared<TgBot::InlineKeyboardButton>();

            inline auto return_to_process_workload = std::make_shared<TgBot::InlineKeyboardButton>();

            inline auto process_workload_by_cpu = std::make_shared<TgBot::InlineKeyboardButton>();
            inline auto process_workload_by_ram = std::make_shared<TgBot::InlineKeyboardButton>();

            inline auto update_packages = std::make_shared<TgBot::InlineKeyboardButton>();
            inline auto pm2_tools = std::make_shared<TgBot::InlineKeyboardButton>();
            inline auto reboot_system = std::make_shared<TgBot::InlineKeyboardButton>();
            inline auto shutdown_system = std::make_shared<TgBot::InlineKeyboardButton>();
            inline auto cancel_system_controls_command = std::make_shared<TgBot::InlineKeyboardButton>();

            inline auto return_to_system_controls = std::make_shared<TgBot::InlineKeyboardButton>();

            inline auto return_to_pm2_tools = std::make_shared<TgBot::InlineKeyboardButton>();

            inline auto start_pm2_process = std::make_shared<TgBot::InlineKeyboardButton>();
            inline auto stop_pm2_process = std::make_shared<TgBot::InlineKeyboardButton>();
            inline auto restart_pm2_process = std::make_shared<TgBot::InlineKeyboardButton>();

            inline auto verify_system_reboot = std::make_shared<TgBot::InlineKeyboardButton>();

            inline auto verify_system_shutdown = std::make_shared<TgBot::InlineKeyboardButton>();
        }
    }
}

#endif