#include <keyboard_utils.hpp>

namespace keyboard
{
    namespace markup
    {
        namespace reply
        {
            static void construct_main();
        }
    }

    namespace markup
    {
        namespace inline_
        {
            static void construct_system_info();

            static void construct_return_to_system_info();

            static void construct_return_to_pm2_logs();

            static void construct_process_workload();

            static void construct_return_to_process_workload();

            static void construct_system_controls();

            static void construct_return_to_system_controls();

            static void construct_pm2_tools();

            static void construct_return_to_pm2_tools();

            static void construct_reboot_system_verification();

            static void construct_shutdown_system_verification();
        }
    }
}

static void keyboard::markup::reply::construct_main()
{
    button::reply::system_info->text = "System info";
    button::reply::system_controls->text = "System controls";

    markup::reply::main->keyboard = { {button::reply::system_info, button::reply::system_controls} };
    markup::reply::main->resizeKeyboard = true;
}

static void keyboard::markup::inline_::construct_system_info()
{
    button::inline_::pm2_status->text = "PM2 status";
    button::inline_::pm2_status->callbackData = "get_pm2_status";

    button::inline_::pm2_logs->text = "PM2 logs";
    button::inline_::pm2_logs->callbackData = "choose_pm2_logs_process";

    button::inline_::system_metrics->text = "System metrics";
    button::inline_::system_metrics->callbackData = "get_system_metrics";

    button::inline_::process_workload->text = "Process workload";
    button::inline_::process_workload->callbackData = "get_process_workload";

    button::inline_::reboot_required->text = "Is reboot required";
    button::inline_::reboot_required->callbackData = "button::inline_::reboot_required";

    button::inline_::updates_available->text = "Are updates available";
    button::inline_::updates_available->callbackData = "button::inline_::updates_available";

    markup::inline_::system_info->inlineKeyboard =
    {
        {button::inline_::pm2_status, button::inline_::pm2_logs},
        {button::inline_::system_metrics, button::inline_::process_workload},
        {button::inline_::reboot_required, button::inline_::updates_available}
    };
}

static void keyboard::markup::inline_::construct_return_to_system_info()
{
    button::inline_::return_to_system_info->text = "Go back";
    button::inline_::return_to_system_info->callbackData = "button::inline_::return_to_system_info";

    markup::inline_::return_to_system_info->inlineKeyboard =
    {
        {button::inline_::return_to_system_info}
    };
}

static void keyboard::markup::inline_::construct_return_to_pm2_logs()
{
    button::inline_::return_to_pm2_logs->text = "Go back";
    button::inline_::return_to_pm2_logs->callbackData = "choose_pm2_logs_process";

    markup::inline_::return_to_pm2_logs->inlineKeyboard =
    {
        {button::inline_::return_to_pm2_logs}
    };
}

static void keyboard::markup::inline_::construct_process_workload()
{
    button::inline_::process_workload_by_cpu->text = "CPU";
    button::inline_::process_workload_by_cpu->callbackData = "process_workload_by_cpu";

    button::inline_::process_workload_by_ram->text = "RAM";
    button::inline_::process_workload_by_ram->callbackData = "process_workload_by_ram";

    markup::inline_::process_workload->inlineKeyboard =
    {
        {button::inline_::process_workload_by_cpu, button::inline_::process_workload_by_ram},
        {button::inline_::return_to_system_info}
    };
}

static void keyboard::markup::inline_::construct_return_to_process_workload()
{
    button::inline_::return_to_process_workload->text = "Go back";
    button::inline_::return_to_process_workload->callbackData = button::inline_::process_workload->callbackData;

    markup::inline_::return_to_process_workload->inlineKeyboard =
    {
        {button::inline_::return_to_process_workload}
    };
}

static void keyboard::markup::inline_::construct_system_controls()
{
    button::inline_::update_packages->text = "Update packages";
    button::inline_::update_packages->callbackData = "update_packages";

    button::inline_::pm2_tools->text = "PM2 tools";
    button::inline_::pm2_tools->callbackData = "pm2_tools";

    button::inline_::reboot_system->text = "Reboot system";
    button::inline_::reboot_system->callbackData = "reboot_system";

    button::inline_::shutdown_system->text = "Shut down system";
    button::inline_::shutdown_system->callbackData = "shutdown_system";

    button::inline_::cancel_system_controls_command->text = "No";
    button::inline_::cancel_system_controls_command->callbackData = "return_to_system_controls";

    markup::inline_::system_controls->inlineKeyboard =
    {
        {button::inline_::update_packages, button::inline_::pm2_tools},
        {button::inline_::reboot_system, button::inline_::shutdown_system}
    };
}

static void keyboard::markup::inline_::construct_return_to_system_controls()
{
    button::inline_::return_to_system_controls->text = "Go back";
    button::inline_::return_to_system_controls->callbackData = "return_to_system_controls";

    markup::inline_::return_to_system_controls->inlineKeyboard =
    {
        {button::inline_::return_to_system_controls}
    };
}

static void keyboard::markup::inline_::construct_pm2_tools()
{
    button::inline_::start_pm2_process->text = "Start process";
    button::inline_::start_pm2_process->callbackData = "start_pm2_process";

    button::inline_::stop_pm2_process->text = "Stop process";
    button::inline_::stop_pm2_process->callbackData = "stop_pm2_process";

    button::inline_::restart_pm2_process->text = "Restart process";
    button::inline_::restart_pm2_process->callbackData = "restart_pm2_process";

    markup::inline_::pm2_tools->inlineKeyboard =
    {
        {button::inline_::start_pm2_process, button::inline_::stop_pm2_process},
        {button::inline_::restart_pm2_process},
        {button::inline_::return_to_system_controls}
    };
}

static void keyboard::markup::inline_::construct_return_to_pm2_tools()
{
    button::inline_::return_to_pm2_tools->text = "Go back";
    button::inline_::return_to_pm2_tools->callbackData = button::inline_::pm2_tools->callbackData;

    markup::inline_::return_to_pm2_tools->inlineKeyboard =
    {
        {button::inline_::return_to_pm2_tools}
    };
}

static void keyboard::markup::inline_::construct_reboot_system_verification()
{
    button::inline_::verify_system_reboot->text = "Yes";
    button::inline_::verify_system_reboot->callbackData = "verify_system_reboot";

    markup::inline_::reboot_system_verification->inlineKeyboard =
    {
        {button::inline_::verify_system_reboot},
        {button::inline_::cancel_system_controls_command}
    };
}

static void keyboard::markup::inline_::construct_shutdown_system_verification()
{
    button::inline_::verify_system_shutdown->text = "Yes";
    button::inline_::verify_system_shutdown->callbackData = "verify_system_shutdown";

    markup::inline_::shutdown_system_verification->inlineKeyboard =
    {
        {button::inline_::verify_system_shutdown},
        {button::inline_::cancel_system_controls_command}
    };
}

TgBot::InlineKeyboardMarkup::Ptr keyboard::markup::inline_::construct_pm2_logs_processes(
    const std::vector<std::pair<size_t, std::string>>& processes_data)
{
    auto pm2_processes_inline_kb_markup = std::make_shared<TgBot::InlineKeyboardMarkup>();

    for (size_t i = 0; i < processes_data.size(); ++i)
    {
        auto pm2_process_btn = std::make_shared<TgBot::InlineKeyboardButton>();
        pm2_process_btn->text = processes_data[i].second;
        pm2_process_btn->callbackData = "get_pm2_logs_" + std::to_string(processes_data[i].first);

        if (i % 2 == 0)
        {
            pm2_processes_inline_kb_markup->inlineKeyboard.emplace_back(
                std::vector<TgBot::InlineKeyboardButton::Ptr>{pm2_process_btn});
        }
        else
        {
            pm2_processes_inline_kb_markup->inlineKeyboard.back().emplace_back(pm2_process_btn);
        }
    }

    pm2_processes_inline_kb_markup->inlineKeyboard.emplace_back(
        std::vector<TgBot::InlineKeyboardButton::Ptr>{button::inline_::return_to_system_info});

    return pm2_processes_inline_kb_markup;
}

TgBot::InlineKeyboardMarkup::Ptr keyboard::markup::inline_::construct_pm2_tools_processes(
    const std::vector<std::pair<size_t, std::string>>& processes_data,
    const std::string& callback_data_starts_with)
{
    auto pm2_processes_inline_kb_markup = std::make_shared<TgBot::InlineKeyboardMarkup>();

    if (processes_data.size() > 1)
    {
        auto pm2_process_btn = std::make_shared<TgBot::InlineKeyboardButton>();
        pm2_process_btn->text = "All";
        pm2_process_btn->callbackData = callback_data_starts_with + "all";

        pm2_processes_inline_kb_markup->inlineKeyboard.emplace_back(
            std::vector<TgBot::InlineKeyboardButton::Ptr>{pm2_process_btn});
    }

    for (size_t i = 0; i < processes_data.size(); ++i)
    {
        auto pm2_process_btn = std::make_shared<TgBot::InlineKeyboardButton>();
        pm2_process_btn->text = processes_data[i].second;
        pm2_process_btn->callbackData = callback_data_starts_with + std::to_string(processes_data[i].first);

        if (i % 2 == 0)
        {
            pm2_processes_inline_kb_markup->inlineKeyboard.emplace_back(
                std::vector<TgBot::InlineKeyboardButton::Ptr>{pm2_process_btn});
        }
        else
        {
            pm2_processes_inline_kb_markup->inlineKeyboard.back().emplace_back(pm2_process_btn);
        }
    }

    pm2_processes_inline_kb_markup->inlineKeyboard.emplace_back(
        std::vector<TgBot::InlineKeyboardButton::Ptr>{button::inline_::return_to_pm2_tools});

    return pm2_processes_inline_kb_markup;
}

void keyboard::markup::construct_markups()
{
    markup::reply::construct_main();
    markup::inline_::construct_system_info();
    markup::inline_::construct_return_to_system_info();
    markup::inline_::construct_return_to_pm2_logs();
    markup::inline_::construct_process_workload();
    markup::inline_::construct_return_to_process_workload();
    markup::inline_::construct_system_controls();
    markup::inline_::construct_return_to_system_controls();
    markup::inline_::construct_pm2_tools();
    markup::inline_::construct_return_to_pm2_tools();
    markup::inline_::construct_reboot_system_verification();
    markup::inline_::construct_shutdown_system_verification();
}