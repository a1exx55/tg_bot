#include <handlers.hpp>

#include <config.hpp>
#include <logging.hpp>
#include <keyboard_utils.hpp>
#include <tg_bot_utils.hpp>

#include <format>
#include <iostream>

#include <boost/process.hpp>

namespace handlers
{
    const size_t max_message_size = 4096;

    std::string execute_terminal_command(const std::string& command, bool requires_sudo = false, double timeout = 0.0)
    {
        boost::process::opstream in_stream;
        boost::process::ipstream out_stream;
        boost::process::child process(
            "/bin/bash",
            "-c",
            std::format(
                "{}timeout {}s /bin/bash", 
                requires_sudo ? "sudo -Skp \"\" " : "", 
                timeout),
            boost::process::std_in < in_stream,
            (boost::process::std_out & boost::process::std_err) > out_stream);
        
        if (requires_sudo)
        {
            in_stream << config::sudo_password << std::endl;
        }

        in_stream << command << std::endl;

        in_stream.pipe().close();

        std::string output_data;
        char buffer[max_message_size];
        
        while (out_stream && out_stream.read(buffer, max_message_size).gcount())
        {
            output_data.append(buffer, out_stream.gcount());
        }
        
        process.wait();
        
        return output_data;
    }
    
    void on_start_command(const TgBot::Api& bot_api, const TgBot::Message::Ptr message)
    try
    {
        bot_api.sendMessage(
            message->from->id, 
            "Features:\n"
            "— *_System info_* provides different system data to obtain information about its status\\.\n"
            "— *_System controls_* provides different abilities to manage system and control its status\\.", 
            nullptr, 
            nullptr, 
            keyboard::markup::reply::main,
            "MarkdownV2");
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void send_system_info(const TgBot::Api& bot_api, const TgBot::Message::Ptr message)
    try
    {
        bot_api.sendMessage(
            message->from->id, 
            "System info commands:", 
            nullptr, 
            nullptr, 
            keyboard::markup::inline_::system_info);
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void return_to_system_info(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        bot_api.editMessageText(
            "System info commands:", 
            callback->from->id, 
            callback->message->messageId, 
            "", 
            "",
            nullptr,
            keyboard::markup::inline_::system_info);
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void send_system_controls(const TgBot::Api& bot_api, const TgBot::Message::Ptr message)
    try
    {
        bot_api.sendMessage(
            message->from->id,
            "System controls' commands:", 
            nullptr, 
            nullptr, 
            keyboard::markup::inline_::system_controls);
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void return_to_system_controls(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        bot_api.editMessageText(
            "System controls' commands:", 
            callback->from->id, 
            callback->message->messageId, 
            "", 
            "",
            nullptr,
            keyboard::markup::inline_::system_controls);
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void get_pm2_status(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        std::string message;

        std::string pm2_list_data = execute_terminal_command("pm2 jlist");
        
        json::array pm2_list_json = json::parse(pm2_list_data).as_array();

        if (pm2_list_json.empty())
        {
            message = "No PM2 processes available";
        }
        else
        {
            size_t process_number = 1;

            for (const auto& pm2_process : pm2_list_json)
            {
                message += std::format(
                    "*_{}\\. {}_*\n",
                    process_number++,
                    tg_bot_utils::escape_markdown_message(pm2_process.as_object().at("name").as_string().c_str()));

                std::string pm2_process_status = pm2_process.as_object().at("pm2_env").as_object().at("status").as_string().c_str();
                
                if (pm2_process_status == "online")
                {
                    std::string pm2_process_uptime_str;
                    auto pm2_process_uptime = (std::chrono::system_clock::now() - std::chrono::milliseconds{pm2_process.as_object().at("pm2_env").as_object().at("pm_uptime").as_int64()}).time_since_epoch();
                    size_t pm2_process_uptime_count;

                    if ((pm2_process_uptime_count = std::chrono::duration_cast<std::chrono::seconds>(pm2_process_uptime).count()) < 60)
                    {
                        pm2_process_uptime_str = std::to_string(pm2_process_uptime_count) + " seconds";
                    }

                    else if ((pm2_process_uptime_count = std::chrono::duration_cast<std::chrono::minutes>(pm2_process_uptime).count()) < 60)
                    {
                        pm2_process_uptime_str = std::to_string(pm2_process_uptime_count) + " minutes";
                    }
                    else if ((pm2_process_uptime_count = std::chrono::duration_cast<std::chrono::hours>(pm2_process_uptime).count()) < 24)
                    {
                        pm2_process_uptime_str = std::to_string(pm2_process_uptime_count) + " hours";
                    }
                    else
                    {
                        pm2_process_uptime_str = std::to_string(std::chrono::duration_cast<std::chrono::days>(pm2_process_uptime).count()) + " days";
                    }

                    message += std::format(
                        "— Status: _online_\n— CPU: _{}%_\n— RAM: _{}MB_\n— Uptime: _{}_",
                            std::round(
                                pm2_process.as_object().at("monit").as_object().at("cpu").to_number<double>()),
                            std::round(
                                pm2_process.as_object().at("monit").as_object().at("memory").to_number<double>() / 1024 / 1024),
                        pm2_process_uptime_str);
                }
                else
                {
                    message += std::format("— Status: _{}_", pm2_process_status);
                }

                message += "\n\n";
            }
        }

        bot_api.editMessageText(
            message, 
            callback->from->id, 
            callback->message->messageId, 
            "", 
            "MarkdownV2",
            nullptr,
            keyboard::markup::inline_::return_to_system_info);
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void choose_pm2_logs_process(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        std::string pm2_list_data = execute_terminal_command("pm2 jlist");
        
        json::array pm2_list_json = json::parse(pm2_list_data).as_array();

        if (pm2_list_json.empty())
        {
            bot_api.editMessageText(
                "No PM2 processes available", 
                callback->from->id, 
                callback->message->messageId,
                "",
                "",
                nullptr,
                keyboard::markup::inline_::return_to_system_info);
        }
        else
        {
            std::vector<std::pair<size_t, std::string>> pm2_processes_data;

            for (const auto& pm2_process : pm2_list_json)
            {
                pm2_processes_data.emplace_back(
                    pm2_process.as_object().at("pm_id").to_number<size_t>(), 
                    pm2_process.as_object().at("name").as_string().c_str());
            }

            bot_api.editMessageText(
                "Choose PM2 process to inspect:", 
                callback->from->id, 
                callback->message->messageId,
                "",
                "",
                nullptr,
                keyboard::markup::inline_::construct_pm2_logs_processes(pm2_processes_data));
        }
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }
    

    void get_pm2_logs(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        std::string pm2_process_id = callback->data.substr(callback->data.rfind('_') + 1);

        bot_api.editMessageText(
            execute_terminal_command(std::format("pm2 logs {} --err --lines 10", pm2_process_id), false, 0.5), 
            callback->from->id, 
            callback->message->messageId, 
            "", 
            "",
            nullptr,
            keyboard::markup::inline_::return_to_pm2_logs);
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void get_system_metrics(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        static const std::string cpu_temp_command =
            R"(sensors | awk '/Core/ {sum+=$3; count++} END {printf "%.2f°C", sum/count}')";

        std::string message = "*CPU temperature:* " + tg_bot_utils::escape_markdown_message(
            execute_terminal_command(cpu_temp_command));

        static const std::string cpu_usage_command =
            R"(cat /proc/stat | grep cpu | tail -1 | awk '{print ($5*100)/($2+$3+$4+$5+$6+$7+$8+$9+$10)}'| awk '{printf "%.2f%%",100-$1}')";

        message += "\n\n*CPU usage:* " + tg_bot_utils::escape_markdown_message(
            execute_terminal_command(cpu_usage_command));

        static const std::string ram_usage_command =
            R"(free -h --si | awk '/Mem/ {printf "%s/%s",$3,$2}')";

        message += "\n\n*RAM usage:* " + tg_bot_utils::escape_markdown_message(
            execute_terminal_command(ram_usage_command));

        static const std::string fs_usage_command =
            R"(df -h / | tail -1 | awk '{printf "%s/%s",$3,$2}')";

        message += "\n\n*Filesystem usage:* " + tg_bot_utils::escape_markdown_message(
            execute_terminal_command(fs_usage_command));

        bot_api.editMessageText(
            message, 
            callback->from->id, 
            callback->message->messageId, 
            "", 
            "MarkdownV2",
            nullptr,
            keyboard::markup::inline_::return_to_system_info);
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void choose_process_workload_option(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        bot_api.editMessageText(
            "Choose what to sort the process workload by:",
            callback->from->id,
            callback->message->messageId,
            "",
            "",
            nullptr,
            keyboard::markup::inline_::process_workload);
    }
    catch (const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void get_process_workload_by_cpu(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        static const std::string process_workload_by_cpu_command =
            R"(ps -eo pid,comm,%cpu,%mem --sort=-%cpu | grep -v "ps" | head -n 11 | awk 'NR==1 {printf "%-8s %-15s %6s %7s\n", "PID", "Process name", "CPU", "RAM"} NR>1 {printf "%-8s %-15s %6.1f%% %6.1f%%\n", $1, $2, $3, $4}')";

        bot_api.editMessageText(
            std::format(
                "```\n{}\n```",
                tg_bot_utils::escape_markdown_message(
                    execute_terminal_command(process_workload_by_cpu_command))),
            callback->from->id,
            callback->message->messageId,
            "",
            "MarkdownV2",
            nullptr,
            keyboard::markup::inline_::return_to_process_workload);
    }
    catch (const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void get_process_workload_by_ram(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
        try
    {
        static const std::string process_workload_by_ram_command =
            R"(ps -eo pid,comm,%cpu,%mem --sort=-%mem | grep -v "ps" | head -n 11 | awk 'NR==1 {printf "%-8s %-15s %6s %7s\n", "PID", "Process name", "CPU", "RAM"} NR>1 {printf "%-8s %-15s %6.1f%% %6.1f%%\n", $1, $2, $3, $4}')";

        bot_api.editMessageText(
            std::format(
            "```\n{}\n```",
            tg_bot_utils::escape_markdown_message(
            execute_terminal_command(process_workload_by_ram_command))),
            callback->from->id,
            callback->message->messageId,
            "",
            "MarkdownV2",
            nullptr,
            keyboard::markup::inline_::return_to_process_workload);
    }
    catch (const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void check_if_reboot_required(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        static const std::string reboot_required_command =
        R"(
            FILE=/var/run/reboot-required
            if [ -f "$FILE" ]; then
                echo "System reboot is required"
            else
                echo "System reboot is not required"
            fi
        )";

        bot_api.editMessageText(
            execute_terminal_command(reboot_required_command), 
            callback->from->id, 
            callback->message->messageId, 
            "", 
            "MarkdownV2",
            nullptr,
            keyboard::markup::inline_::return_to_system_info);
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void check_if_updates_available(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        bot_api.editMessageText(
            "Checking for available updates. Wait...", 
            callback->from->id,
            callback->message->messageId);
        
        static const std::string available_updates =
            "apt update &> /dev/null && apt list --upgradable 2> /dev/null | wc -l";

        bot_api.editMessageText(
            std::format(
                "\n\n{} packages can be updated",
                std::stoull(execute_terminal_command(available_updates, true)) - 1), 
            callback->from->id, 
            callback->message->messageId, 
            "", 
            "MarkdownV2",
            nullptr,
            keyboard::markup::inline_::return_to_system_info);
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void update_packages(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        bot_api.editMessageText(
            "Updating packages. Wait...", 
            callback->from->id, 
            callback->message->messageId);
        
        static const std::string update_command =
            R"(apt update &> /dev/null && apt upgrade -y 2> /dev/null | grep -Po "\d+ (?:not )?upgraded" | awk '{print $1, "packages were", ($2 == "upgraded" ? "upgraded" : "not upgraded")}')";

        bot_api.editMessageText(
            execute_terminal_command(update_command, true), 
            callback->from->id, 
            callback->message->messageId, 
            "", 
            "",
            nullptr,
            keyboard::markup::inline_::return_to_system_controls);
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void get_pm2_tools(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        bot_api.editMessageText(
            "Choose PM2 tool to use:", 
            callback->from->id, 
            callback->message->messageId, 
            "", 
            "",
            nullptr,
            keyboard::markup::inline_::pm2_tools);
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void choose_pm2_process_to_start(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        std::string pm2_list_data = execute_terminal_command("pm2 jlist");
        
        json::array pm2_list_json = json::parse(pm2_list_data).as_array();

        if (pm2_list_json.empty())
        {
            bot_api.editMessageText(
                "No PM2 processes available", 
                callback->from->id, 
                callback->message->messageId,
                "",
                "",
                nullptr,
                keyboard::markup::inline_::return_to_pm2_tools);
        }
        else
        {
            std::vector<std::pair<size_t, std::string>> pm2_processes_to_start_data;

            for (const auto& pm2_process : pm2_list_json)
            {
                if (pm2_process.as_object().at("pm2_env").as_object().at("status").as_string() != "online")
                {
                    pm2_processes_to_start_data.emplace_back(
                        pm2_process.as_object().at("pm_id").to_number<size_t>(), 
                        pm2_process.as_object().at("name").as_string().c_str());
                }
            }

            if (pm2_processes_to_start_data.empty())
            {
                bot_api.editMessageText(
                    "No PM2 processes available to start", 
                    callback->from->id, 
                    callback->message->messageId,
                    "",
                    "",
                    nullptr,
                    keyboard::markup::inline_::return_to_pm2_tools);
            }
            else
            {
                bot_api.editMessageText(
                    "Choose PM2 process to start:", 
                    callback->from->id, 
                    callback->message->messageId,
                    "",
                    "",
                    nullptr,
                    keyboard::markup::inline_::construct_pm2_tools_processes(
                        pm2_processes_to_start_data, 
                        "start_pm2_process_")); 
            }
        }
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void start_pm2_process(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        std::string pm2_process_id = callback->data.substr(callback->data.rfind('_') + 1);
        
        bot_api.editMessageText(
            pm2_process_id == "all" ? "All PM2 processes are started!" : "PM2 process is started!", 
            callback->from->id, 
            callback->message->messageId, 
            "", 
            "",
            nullptr,
            keyboard::markup::inline_::return_to_pm2_tools);

        execute_terminal_command(std::format("pm2 start {}", pm2_process_id));
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void choose_pm2_process_to_stop(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        std::string pm2_list_data = execute_terminal_command("pm2 jlist");
        
        json::array pm2_list_json = json::parse(pm2_list_data).as_array();

        if (pm2_list_json.empty())
        {
            bot_api.editMessageText(
                "No PM2 processes available", 
                callback->from->id, 
                callback->message->messageId,
                "",
                "",
                nullptr,
                keyboard::markup::inline_::return_to_pm2_tools);
        }
        else
        {
            std::vector<std::pair<size_t, std::string>> pm2_processes_to_stop_data;

            for (const auto& pm2_process : pm2_list_json)
            {
                if (pm2_process.as_object().at("pm2_env").as_object().at("status").as_string() != "stopped")
                {
                    pm2_processes_to_stop_data.emplace_back(
                        pm2_process.as_object().at("pm_id").to_number<size_t>(), 
                        pm2_process.as_object().at("name").as_string().c_str());
                }
            }

            if (pm2_processes_to_stop_data.empty())
            {
                bot_api.editMessageText(
                    "No PM2 processes available to stop", 
                    callback->from->id, 
                    callback->message->messageId,
                    "",
                    "",
                    nullptr,
                    keyboard::markup::inline_::return_to_pm2_tools);
            }
            else
            {
                bot_api.editMessageText(
                    "Choose PM2 process to stop:", 
                    callback->from->id, 
                    callback->message->messageId,
                    "",
                    "",
                    nullptr,
                    keyboard::markup::inline_::construct_pm2_tools_processes(
                        pm2_processes_to_stop_data, 
                        "stop_pm2_process_")); 
            }
        }
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void stop_pm2_process(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        std::string pm2_process_id = callback->data.substr(callback->data.rfind('_') + 1);
        
        bot_api.editMessageText(
            pm2_process_id == "all" ? "All PM2 processes are stopped!" : "PM2 process is stopped!", 
            callback->from->id, 
            callback->message->messageId, 
            "", 
            "",
            nullptr,
            keyboard::markup::inline_::return_to_pm2_tools);

        execute_terminal_command(std::format("pm2 stop {}", pm2_process_id));
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void choose_pm2_process_to_restart(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        std::string pm2_list_data = execute_terminal_command("pm2 jlist");
        
        json::array pm2_list_json = json::parse(pm2_list_data).as_array();

        if (pm2_list_json.empty())
        {
            bot_api.editMessageText(
                "No PM2 processes available", 
                callback->from->id, 
                callback->message->messageId,
                "",
                "",
                nullptr,
                keyboard::markup::inline_::return_to_pm2_tools);
        }
        else
        {
            std::vector<std::pair<size_t, std::string>> pm2_processes_to_restart_data;

            for (const auto& pm2_process : pm2_list_json)
            {
                if (pm2_process.as_object().at("pm2_env").as_object().at("status").as_string() != "stopped")
                {
                    pm2_processes_to_restart_data.emplace_back(
                        pm2_process.as_object().at("pm_id").to_number<size_t>(), 
                        pm2_process.as_object().at("name").as_string().c_str());
                }
            }

            if (pm2_processes_to_restart_data.empty())
            {
                bot_api.editMessageText(
                    "No PM2 processes available to restart", 
                    callback->from->id, 
                    callback->message->messageId,
                    "",
                    "",
                    nullptr,
                    keyboard::markup::inline_::return_to_pm2_tools);
            }
            else
            {
                bot_api.editMessageText(
                    "Choose PM2 process to restart:", 
                    callback->from->id, 
                    callback->message->messageId,
                    "",
                    "",
                    nullptr,
                    keyboard::markup::inline_::construct_pm2_tools_processes(
                        pm2_processes_to_restart_data, 
                        "restart_pm2_process_")); 
            }
        }
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void restart_pm2_process(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        std::string pm2_process_id = callback->data.substr(callback->data.rfind('_') + 1);
        
        bot_api.editMessageText(
            pm2_process_id == "all" ? "All PM2 processes are restarted!" : "PM2 process is restarted!", 
            callback->from->id, 
            callback->message->messageId, 
            "", 
            "",
            nullptr,
            keyboard::markup::inline_::return_to_pm2_tools);

        execute_terminal_command(std::format("pm2 restart {}", pm2_process_id));
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void verify_system_reboot(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        bot_api.editMessageText(
            "Do you really want to reboot system?", 
            callback->from->id, 
            callback->message->messageId, 
            "", 
            "", 
            nullptr, 
            keyboard::markup::inline_::reboot_system_verification);
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void reboot_system(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        bot_api.editMessageText(
            "System is rebooting. Wait...", 
            callback->from->id,
            callback->message->messageId);

        bot_api.sendSticker(
            callback->from->id, 
            "CAACAgIAAxkBAAEH5a5mxlrq5o4MrcqTWpdw8tTiI9n2lQACyhMAAnscoUgi5zjt6JynqzUE");

        execute_terminal_command("reboot", true);
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void verify_system_shutdown(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        bot_api.editMessageText(
            "Do you really want to shut down system?", 
            callback->from->id, 
            callback->message->messageId, 
            "", 
            "", 
            nullptr, 
            keyboard::markup::inline_::shutdown_system_verification);
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    void shutdown_system(const TgBot::Api& bot_api, const TgBot::CallbackQuery::Ptr callback)
    try
    {
        bot_api.editMessageText(
            "System is shutting down. Bye(", 
            callback->from->id, 
            callback->message->messageId);

        bot_api.sendSticker(
            callback->from->id, 
            "CAACAgIAAxkBAAEH6V9mxy-ooVmoYsoHoRfYc1ufd-k1JAACGxsAAp1hoEh1dVqj6-ZQZDUE");

        execute_terminal_command("shutdown -h now", true);
    }
    catch(const std::exception& e)
    {
        logging::log_error(e.what());
    }

    bool validate_user_by_message(const TgBot::Message::Ptr message)
    {
        return config::allowed_user_ids.contains(message->from->id);
    }

    bool validate_user_by_callback(const TgBot::CallbackQuery::Ptr callback)
    {
        return config::allowed_user_ids.contains(callback->from->id);
    }

    void register_handlers(TgBot::Bot& bot)
    {
        tg_bot_utils::register_command_handlers(
            bot,
            {
                {"start", on_start_command}
            },
            validate_user_by_message);

        tg_bot_utils::register_message_handlers(
            bot,
            {
                {keyboard::button::reply::system_info->text, send_system_info},
                {keyboard::button::reply::system_controls->text, send_system_controls}
            },
            validate_user_by_message);

        tg_bot_utils::register_callback_handlers(
            bot,
            {
                {keyboard::button::inline_::return_to_system_info->callbackData, return_to_system_info},
                {keyboard::button::inline_::return_to_system_controls->callbackData, return_to_system_controls},
                {keyboard::button::inline_::pm2_status->callbackData, get_pm2_status},
                {keyboard::button::inline_::pm2_logs->callbackData, choose_pm2_logs_process},
                {keyboard::button::inline_::system_metrics->callbackData, get_system_metrics},
                {keyboard::button::inline_::process_workload->callbackData, choose_process_workload_option},
                {keyboard::button::inline_::process_workload_by_cpu->callbackData, get_process_workload_by_cpu},
                {keyboard::button::inline_::process_workload_by_ram->callbackData, get_process_workload_by_ram},
                {keyboard::button::inline_::reboot_required->callbackData, check_if_reboot_required},
                {keyboard::button::inline_::updates_available->callbackData, check_if_updates_available},
                {keyboard::button::inline_::update_packages->callbackData, update_packages},
                {keyboard::button::inline_::pm2_tools->callbackData, get_pm2_tools},
                {keyboard::button::inline_::start_pm2_process->callbackData, choose_pm2_process_to_start},
                {keyboard::button::inline_::stop_pm2_process->callbackData, choose_pm2_process_to_stop},
                {keyboard::button::inline_::restart_pm2_process->callbackData, choose_pm2_process_to_restart},
                {keyboard::button::inline_::reboot_system->callbackData, verify_system_reboot},
                {keyboard::button::inline_::verify_system_reboot->callbackData, reboot_system},
                {keyboard::button::inline_::cancel_system_controls_command->callbackData, return_to_system_controls},
                {keyboard::button::inline_::shutdown_system->callbackData, verify_system_shutdown},
                {keyboard::button::inline_::verify_system_shutdown->callbackData, shutdown_system},
            },
            tg_bot_utils::text_comparison_policy::equal,
            validate_user_by_callback);

        tg_bot_utils::register_callback_handlers(
            bot,
            {
                {"get_pm2_logs_", get_pm2_logs},
                {"start_pm2_process_", start_pm2_process},
                {"stop_pm2_process_", stop_pm2_process},
                {"restart_pm2_process_", restart_pm2_process}
            },
            tg_bot_utils::text_comparison_policy::starts_with,
            validate_user_by_callback);
    }
}