// SPDX-License-Identifier: Apache License 2.0

#include "server/Endpoints.h"

#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <storage/Storage.h>
#include <uWebSocket/src/HttpResponse.h>

namespace tpunkt
{

    class TestScope
    {
      public:
        TestScope(const std::string& name) : name(name)
        {
            std::cout << "TestScope " << name << " constructed.\n";
        }

        TestScope(const TestScope& scope) : name(scope.name)
        {
            std::cout << "TestScope " << name << " copied.\n";
        }

        ~TestScope()
        {
            std::cout << "TestScope " << name << " destroyed.\n";
            name = "Deleted";
        }

        void print(const std::string& message) const
        {
            std::cout << "TestScope " << name << ": " << message << "\n";
        }

      private:
        std::string name;
    };


    void UploadEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
    {
    //TODO make allocator to avoid dynamic memory here (fragmentation)
        auto transaction = std::make_shared<TestScope>("Local Scope");

        std::string filePath = "./uploaded_file.txt";
        auto file = std::make_shared<std::ofstream>(filePath, std::ios::binary);

        res->onData(
            [ file, transaction, res ](std::string_view chunk, bool isLast) mutable
            {
                transaction.get()->print("onData callback called.");
                if(file->is_open())
                {
                    file->write(chunk.data(), chunk.size());
                }

                if(isLast)
                {
                    printf("Is last true\n");
                    std::thread thread{[ & ]()
                                       {
                                           std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                                           printf("Ending now\n");
                                           res->end("File uploaded successfully!");
                                       }};
                    thread.detach();
                    file->close();
                }
            });

        res->onAborted(
            [ file, transaction ]() mutable
            {
                transaction.get()->print("onAborted callback called.");
                if(file->is_open())
                {
                    file->close();
                    std::remove("./uploaded_file.txt");
                }
                std::cerr << "Upload aborted" << std::endl;
            });

        transaction.get()->print("handle method end.");
    }

} // namespace tpunkt