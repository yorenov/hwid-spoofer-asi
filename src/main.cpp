#include <RakHook/rakhook.hpp>

#include "main/PluginState.h"
#include <kthook/kthook.hpp>

#include "defines.h"
#include "utils/settings/Config.h"

#include <openssl/sha.h>

using namespace std::chrono_literals;


struct SpooferConfig
{
    std::string hwid{};
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SpooferConfig, hwid)
};

std::string sha1(const std::string& input) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), hash);

    std::stringstream result;
    for (unsigned char i : hash) {
        result << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(i);
    }

    return result.str();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved)
{
    switch (dwReasonForCall)
    {
        case (DLL_PROCESS_ATTACH):
        {
            setlocale(LC_ALL, ".UTF8");  // NOLINT(cert-err33-c)
#ifdef _DEBUG
            AllocConsole();
            freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
            freopen_s(reinterpret_cast<FILE**>(stderr), "CONOUT$", "w", stderr);
#endif

            DisableThreadLibraryCalls(hModule);

            static kthook::kthook_signal<void(__cdecl*)()> gameProcessHook;
            gameProcessHook.set_dest(0x53BEE0);
            gameProcessHook.after.connect(
                    [](const auto& pHook) {
                        static bool initCompleted = false;

                        if (initCompleted)
                            return std::nullopt;

                        if (!rakhook::initialize())
                            return std::nullopt;

                        initCompleted = true;

                        namespace fs = std::filesystem;

                        if (const auto path = GetProjectDirectory(); !fs::exists(path))
                            fs::create_directories(path);

                        const auto pConfig = ConfigManager::GetInstance();
                        if (!pConfig->Load()) {
                            pConfig->Save();

                            bool loaded = pConfig->Load();
                            assert(loaded);
                        }

                        static ConfigEntry<SpooferConfig> configEntry{"spoofer", {}};
                        configEntry.Register(pConfig);
                        pConfig->Save(); // or null in json

                        rakhook::on_send_packet += [] (RakNet::BitStream *bitStream, PacketPriority &priority, PacketReliability &reliability, char &ord_channel) {
                            uint8_t id;
                            bitStream->ResetReadPointer();
                            bitStream->Read(id);

                            if (id != 215)
                                return true;

                            debugWL("Its 215!");

                            bitStream->ResetReadPointer();
                            bitStream->IgnoreBits(3 * 8);

                            int8_t subId;
                            bitStream->Read(subId);

                            if (subId != 51)
                                return true;

                            debugWL("Its auth packet!");

                            const auto& config = configEntry.Get();
                            static unsigned char bytes[] = { 1, 0, 51, 0, 0, 0, 0 };

                            RakNet::BitStream newBitStream;
                            newBitStream.Reset();

                            newBitStream.Write(static_cast<uint8_t>(215));
                            newBitStream.Write(reinterpret_cast<char*>(&bytes), 7);

                            newBitStream.Write(static_cast<int16_t>(config.hwid.length()));
                            newBitStream.Write(static_cast<int16_t>(0));
                            newBitStream.Write(config.hwid.c_str(), static_cast<int>(config.hwid.length()));

                            std::string hashedHWID = sha1(config.hwid + "71QNzN7t8v");

                            newBitStream.Write(static_cast<int16_t>(40));
                            newBitStream.Write(static_cast<int16_t>(0));
                            newBitStream.Write(hashedHWID.c_str(), 40);

                            rakhook::send(&newBitStream, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0);

                            return false;
                        };

                        return std::nullopt;
                    });
            gameProcessHook.install();

            break;
        }
        case DLL_PROCESS_DETACH:
        {
#ifdef _DEBUG
            FreeConsole();
#endif
            break;
        }
        default:
            break;
    }
    return TRUE;
}

