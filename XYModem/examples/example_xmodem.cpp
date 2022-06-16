// Copyright 2022 Riuzakiii

/*
 * Usage : ./xmodem.exe -f "filepath" -port COM1
 * -h : get help
 * -port : the serial port of your device. If you don't add a port after this,
 * it will display the available ports -f "filepath1": send send file (filepath
 * should be absolute, don't know what happens if the path is relative) -hex :
 * activate the hexadecimal output of the messages send to the device
 */
#include "CLIParser.h"
#include "Devices/SerialHandler.h"
#include "XYModem.h"

using namespace xymodem;

int main (int argc, [[maybe_unused]] char* argv[])
{
    auto serialDevice = serial::Serial();
    CLIParser cliParser;
    serialDevice.setBaudrate (115200);
    const uint32_t interByteTimeout = 100000;
    const uint32_t readTimeoutConstant = 20;
    const uint32_t readTimeoutMultiplier = 0;
    const uint32_t writeTimeoutConstant = 20;
    const uint32_t writeTimeoutMultiplier = 0;
    serialDevice.setTimeout (interByteTimeout, readTimeoutConstant, readTimeoutMultiplier, writeTimeoutConstant, writeTimeoutMultiplier);

    auto serialHandler = std::make_shared<SerialHandler> (serialDevice);
    auto logger = std::make_shared<Spdlogger>();
    XModemSender<xymodem::payloadSize1K> xmodem (serialHandler, logger);

    std::string filePath;
    bool withHex = false;

    cliParser.setCommands ({
        {"-h",
         [&logger] (std::string_view)
         {
             logger->info ("This program allows you to send one file "
                           "using the XModem file transfer protocol.\n Options "
                           ":\n    -h : get help \n    -f \"filepath1\"  -port "
                           "\"serial port of the device\""
                           "\"filepath2\" ... : send one or more files \n");
         }                                                   },
        {       "-f",
         [&filePath,                                &logger] (std::string_view value)
         {
             logger->info (value);
             filePath = value.data();
         }},
        {                                      "--hex", [&withHex] (std::string_view) { withHex = true; }},
        { "-port",
         [&serialDevice,                                         &logger] (std::string_view val)
         {
             const auto availablePorts = serial::list_ports();
             const auto serialPort = std::find_if (availablePorts.begin(),
                                                   availablePorts.end(),
                                                   [&val, &logger] (auto& device)
                                                   {
                                                       if (val.empty())
                                                       {
                                                           logger->info ("Available devices");
                                                           logger->info (device.description);
                                                           logger->info (device.port);
                                                           logger->info (device.hardware_id);
                                                       }
                                                       return device.port == val;
                                                   });
             if (serialPort != availablePorts.end())
             {
                 logger->info (serialPort->port);
                 serialDevice.setPort (serialPort->port);
                 serialDevice.open();
             }
             else
             {
                 logger->error ("Unable to find the requested device");
             }
         }}
    });

    try
    {
        // parse command line arguments and launch related commands
        cliParser.parse (argc, argv);
        if (!filePath.empty())
        {
            xmodem.transmit (
                std::make_shared<xymodem::DesktopFile> (filePath),
                [] (float) {},
                []() { return false; },
                withHex);
        }
    }
    catch (serial::IOException& e)
    {
        logger->warn (e.what());
        assert (false);
    }
    catch (XYModemExceptions::Timeout const& e)
    {
        logger->warn (e.what());
    }
    catch (XYModemExceptions::CouldNotOpenFile const& e)
    {
        logger->warn (e.what());
    }
    catch (XYModemExceptions::TransmissionAborted const& e)
    {
        logger->warn (e.what());
    }
    catch (std::exception const& e)
    {
        logger->warn (e.what());
        assert (false);
    }
}
