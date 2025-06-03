#include <iostream>
#include <string>
#include <functional>
#include <unordered_map>
#include <utility>

#include "Network/URL.hpp"
#include "Network/HTTP.hpp"
#include "Network/HTTPServer.hpp"
#include "Network/HTTPRequests.hpp"
#include "Network/TCPServer.hpp"
#include "Network/TCPClient.hpp"
#include "Network/ServerSessionData.hpp"
#include "Network/Timer.hpp"
#include <windows.h>


int main()
{
    enum ServerStates
    {
        CHOOSE_STATE = 0,
        INIT,
        EXIT,
        PROCESS,
        PAUSE,
    } state = ServerStates::CHOOSE_STATE;

    std::vector<std::pair<ServerStates, std::string>> menu_selection_variants = {
        {ServerStates::INIT, "Start"},
        {ServerStates::EXIT, "Exit"}
    };


    //std::cout << "|" << net::Requests::get("https://www.google.com/search/?q=dljvdh&sca_esv=fe962eec8177d20a&sxsrf=ADLYWIIzmsDArvbyJOybBdlj9nE37ajGag%3A1734774333054&source=hp&ei=PY5mZ50c14Xszw_X56ywCg&iflsig=AL9hbdgAAAAAZ2acTaSnMpQN7uENuiCKPZlcsMrdpdqG&ved=0ahUKEwjdron_ybiKAxXXAvsDHdczC6YQ4dUDCBc&uact=5&oq=dljvdh&gs_lp=Egdnd3Mtd2l6IgZkbGp2ZGgyCBAAGIAEGKIEMggQABiABBiiBDIIEAAYgAQYogRIsgZQnwJY3QRwAXgAkAEAmAGeAaAB0waqAQMwLja4AQPIAQD4AQGYAgegAv4GqAIKwgIHECMYJxjqAsICERAuGIAEGLEDGNEDGIMBGMcBwgILEAAYgAQYsQMYgwHCAggQABiABBixA8ICBRAAGIAEwgIOEC4YgAQYsQMYgwEY1ALCAgsQABiABBgBGAoYKsICDxAuGIAEGAEY0QMYxwEYCsICCRAuGIAEGAEYCsICCRAAGIAEGAEYCsICCRAAGIAEGAoYKsICBxAAGIAEGArCAgQQABgewgIIEAAYBRgKGB7CAgYQABgFGB7CAgcQLhiABBgNwgIHEAAYgAQYDZgDE_EFDZfLSqLdvJWSBwMxLjagB70v&sclient=gws-wiz") << "|" << std::endl;
    //std::cout << "|" << net::Requests::get("https://www.google.com") << "|" << std::endl;

    net::HTTPServer server;
    server.addHandler("/", [](net::HTTPRequest request) -> net::HTTPResponse
    {
        net::HTTPResponse response;
        net::URI uri(request.start_line[1]);
        std::cout << request.toString() << std::endl;

        for (auto& [key, value] : uri.getParamsPtr())
            response.body += "<p>" + key + ": " + value + "</p>";
        response.body += "<p>success</p>";

        response.start_line[0] = "HTTP/1.1";
        response.start_line[1] = "200";
        response.start_line[2] = "OK";

        response.headers["Version"] = "HTTP/1.1";
        response.headers["Content-Type"] = "text/html; charset=utf-8";
        response.headers["Content-Length"] = std::to_string(response.body.length());

        return response;
    });

    server.addHandler("/test", [](net::HTTPRequest request) -> net::HTTPResponse
    {
        net::HTTPResponse response;
        net::URI uri(request.start_line[1]);
        std::cout << request.toString() << std::endl;

        response.body = "answer!!!!!!!!!!!!!!!!!!!";

        response.start_line[0] = "HTTP/1.1";
        response.start_line[1] = "200";
        response.start_line[2] = "OK";

        response.headers["Version"] = "HTTP/1.1";
        response.headers["Content-Type"] = "text/html; charset=utf-8";
        response.headers["Content-Length"] = std::to_string(response.body.length());

        return response;
    });

    Timer timer;
    while (state != ServerStates::EXIT)
    {
        switch(state)
        {
            case ServerStates::CHOOSE_STATE:
            {
                std::cout << "Choise action:\n";
                for (unsigned int i = 0; i < menu_selection_variants.size(); i++)
                    std::cout << "\t" << i + 1 << ". " + menu_selection_variants[i].second + ";\n";
                std::cout << "Enter: ";

                std::string action_str;
                std::cin >> action_str;
                std::cout << std::endl;

                try
                {
                    int action = stoi(action_str) - 1;
                    if ((action >= 0) && (action < menu_selection_variants.size()))
                        state = menu_selection_variants[action].first;
                    else
                        std::cout << "Incorrect value!\n";
                }
                catch(...)
                {
                    std::cout << "Incorrect value!\n";
                }
            } break;

            case ServerStates::INIT:
            {
                std::string localhost;
                std::cout << "Start on localhost (\"1\" for accept): ";
                std::cin >> localhost;

                server.init(80, localhost == "1");
                server.start(1);

                std::cout << "Server address: " << server.getSelfAddress().toString() << std::endl;

                state = ServerStates::PROCESS;
            } break;

            case ServerStates::EXIT:
            {
                std::cout << "exit\n";
            } break;

            case ServerStates::PROCESS:
            {
                if (server.hasNewSessionData())
                {
                    net::ServerSessionData session = server.getNextSessionData();
                    std::cout << "Request index: " << session.getId() << "\n"
                    << "REQUEST:\n"
                    << session.getRequest() << "\n"
                    << "RESPONSE:\n"
                    << session.getResponse() << "\n"
                    << "============================\n"
                    << "Press Space for stop...\n\n";
                }

                //std::cout << "a\n";
                if (GetAsyncKeyState(VK_SPACE) < 0)
                    state = ServerStates::PAUSE;
                timer.sleep(16);
            } break;

            case ServerStates::PAUSE:
            {
                server.stop();
                std::string command;
                std::cout << "For continue enter \"1\", для выхода любое другое значение: ";
                std::cin >> command;

                if (command == "1")
                {
                    server.start(1);
                    state = ServerStates::PROCESS;
                }
                else
                    state = ServerStates::EXIT;
            } break;

            default:
            {
                state = ServerStates::CHOOSE_STATE;
            } break;
        }
    }

	system("pause");
	return 0;
}