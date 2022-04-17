/*
 * Usage : ./ymodem.exe -f "filepath" -port COM1 
 * -h : get help 
 * -port : the serial port of your device. If you don't add a port after this, it will display the available ports
 * -f "filepath1": send file (filepath should be absolute, don't
 * know what happens if the path is relative) -hex : activate the hexadecimal
 * output of the messages send to the device
 */
#include "XYModem.h"
#include "Devices/SerialHandler.h"
#include "CLIParser.h"

using namespace xymodem;

int main (int argc, [[maybe_unused]] char* argv[])
{
    auto serialDevice = serial::Serial ();
    CLIParser cliParser;
    serialDevice.setBaudrate (115200);
    const uint32_t interByteTimeout = 100000;
    const uint32_t readTimeoutConstant = 20;
    const uint32_t readTimeoutMultiplier = 0;
    const uint32_t writeTimeoutConstant = 20;
    const uint32_t writeTimeoutMultiplier = 0;
    serialDevice.setTimeout (interByteTimeout,
                             readTimeoutConstant,
                             readTimeoutMultiplier,
                             writeTimeoutConstant,
                             writeTimeoutMultiplier);

    auto serialHandler = std::make_shared<SerialHandler>(serialDevice);
    auto logger = std::make_shared<Spdlogger>();
    YModemSender ymodem (serialHandler, logger);

    std::vector<std::shared_ptr<File>> files;
    bool withHex = false;
    std::string lastParam;

    cliParser.setCommands({
        {"-h", [&logger](std::string_view){
           logger->info("This program allows you to send one or more files "
                         "using the YModem file transfer protocol.\n Options "
                         ":\n    -h : get help \n    -f \"filepath1\"  -port "
                         "\"serial port of the device\""
                         "\"filepath2\" ... : send one or more files \n");}},
        {"-f", [&files, &logger](std::string_view value){
                logger->info (value);
                files.emplace_back (std::make_shared<DesktopFile>(value.data()));
        }},
        {"--hex", [&withHex](std::string_view){withHex = true;}},
        {"-port", [&serialDevice, &logger](std::string_view val){
              logger->info ("Available devices");
              const auto availablePorts = serial::list_ports ();
              const auto serialPort =
                  std::find_if (availablePorts.begin (),
                                availablePorts.end (),
                                [&val, &logger] (auto& device) {
                                    logger->info (device.description);
                                    logger->info (device.port);
                                    logger->info (device.hardware_id);
                                    return device.port == val;
                                });

              if (serialPort != availablePorts.end ())
              {
                  serialDevice.setPort (serialPort->port);
                  serialDevice.open ();
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
        if (!files.empty ())
        {
            ymodem.transmit (
                files, [] (float) {}, [] () { return false; }, withHex);
        }
    }
    catch (serial::IOException& e)
    {
        logger->warn (e.what ());
        assert (false);
    }
    catch (XYModemExceptions::Timeout const& e)
    {
        logger->warn (e.what ());
    }
    catch (XYModemExceptions::CouldNotOpenFile const& e)
    {
        logger->warn (e.what ());
    }
    catch (XYModemExceptions::TransmissionAborted const& e)
    {
        logger->warn (e.what ());
    }
    catch (std::exception const& e)
    {
        logger->warn (e.what ());
        assert (false);
    }
}
