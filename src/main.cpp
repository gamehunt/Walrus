#include "roll_engine.hpp"
#include "dice.hpp"
#include <dpp/dpp.h>
#include <dpp/misc-enum.h>
#include <nlohmann/json.hpp>
#include <argparse/argparse.hpp>

using json = nlohmann::json;

void register_commands(dpp::cluster& bot) {
    bot.log(dpp::ll_info, "Registering global commands this run...");
    if (dpp::run_once<struct register_bot_commands>()) {
        dpp::slashcommand ping("ping", "Pong!", bot.me.id);
        bot.global_bulk_command_create({
            ping
        }); 
    }
}
 
int main(int argc, const char** argv) {
    argparse::ArgumentParser program("Walrus");

    program.add_argument("-c","--config")
        .default_value("config.json");

    program.add_argument("--register-commands")
        .default_value(false)
        .implicit_value(true);

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
      std::cerr << err.what() << std::endl;
      std::cerr << program;
      return 1;
    }

    std::ifstream f(program.get<std::string>("config"));
    if(!f.is_open()) {
        std::cout << "Failed to open config.json!" << std::endl;
        exit(1);
    }

    std::srand(std::time({}));

    json data = json::parse(f);
    dpp::cluster bot(data["token"], dpp::i_default_intents | dpp::i_message_content);
 
    bot.on_log(dpp::utility::cout_logger());
 
    bot.on_slashcommand([](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "ping") {
            event.reply("Pong!");
        }
    });

    bot.on_message_create([&bot](const dpp::message_create_t& event) {
        if(event.msg.author.is_bot()) {
            return;
        }
        try {
            roll_engine::result res = roll_engine::eval(event.msg.content);
            if(res.valid) {
                event.reply(res.value);
            }
        } catch(std::exception& e) {;}
    });
 
    bot.on_ready([&bot, &program](const dpp::ready_t& event) {
        if(program.get<bool>("--register-commands")) {
            register_commands(bot);
        }
        bot.log(dpp::ll_info, "We are ready...");
    });
 
    bot.start(dpp::st_wait);
}
