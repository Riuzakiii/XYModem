/*
 * Usage : ./xmodem.exe -f "filepath" -port COM1 
 * -h : get help 
 * -port : the serial port of your device. If you don't add a port after this, it will display the available ports
 * -f "filepath1": send send file (filepath should be absolute, don't
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
    SerialHandler serialHandler (serialDevice);

    XModem xmodem (serialHandler, logType (), 2);
    ghc::filesystem::path file;
    bool withHex = false;
    std::string lastParam;

    cliParser.setCommands({
        {"-h", [](std::string_view){
           spdlog::info("This program allows you to send one file "
                         "using the XModem file transfer protocol.\n Options "
                         ":\n    -h : get help \n    -f \"filepath1\"  -id "
                         "\"VID/PID of the device\""
                         "\"filepath2\" ... : send one or more files \n");}},
        {"-f", [&file](std::string_view value){
                spdlog::info (value);
                file=value.data();
        }},
        {"--hex", [&withHex](std::string_view){withHex = true;}},
        {"-port", [&serialDevice](std::string_view val){
              const auto availablePorts = serial::list_ports ();
              const auto serialPort =
                  std::find_if (availablePorts.begin (),
                                availablePorts.end (),
                                [&val] (auto& device) {
                                    if(val.empty())
                                    {
                                        spdlog::info ("Available devices");
                                        spdlog::info (device.description);
                                        spdlog::info (device.port);
                                        spdlog::info (device.hardware_id);
                                    }
                                    return device.port == val;
                                });
              if (serialPort != availablePorts.end ())
              {
                  spdlog::info (serialPort->port);
                  serialDevice.setPort (serialPort->port);
                  serialDevice.open ();
              }
              else
              {
                  spdlog::error ("Unable to find the requested device");
              }
        }}
    });

    try
    {
        // parse command line arguments and launch related commands
        cliParser.parse (argc, argv);
        if (!file.empty ())
        {
            xmodem.transmit (
                file, [] (float) {}, [] () { return false; }, withHex);
        }
    }
    catch (serial::IOException& e)
    {
        spdlog::warn (e.what ());
        assert (false);
    }
    catch (XYModemExceptions::Timeout const& e)
    {
        spdlog::warn (e.what ());
    }
    catch (XYModemExceptions::CouldNotOpenFile const& e)
    {
        spdlog::warn (e.what ());
    }
    catch (XYModemExceptions::TransmissionAborted const& e)
    {
        spdlog::warn (e.what ());
    }
    catch (std::exception const& e)
    {
        spdlog::warn (e.what ());
        assert (false);
    }
}
