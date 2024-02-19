#include "Config.h"
#include <fstream>
#include <filesystem>

#include "main/PluginState.h"

using nlohmann::json;

bool ConfigManager::Load()
{
	this->entries.clear();
	
	this->path = GetProjectDirectory(CFG_FILENAME);

	if (std::filesystem::exists(this->path))
	{
		const auto fileSize = static_cast<size_t>(std::filesystem::file_size(this->path));

		if (std::ifstream file(this->path, std::ios::out | std::ios::binary); file.is_open())
		{
			void* buff = malloc(fileSize + 1);
			static_cast<char*>(buff)[fileSize] = 0;

			file.read(static_cast<char*>(buff), fileSize);

			this->json = json::parse(static_cast<const char*>(buff));

			free(buff);

			file.close();

			return true;
		}
	}

	return false;
}
void ConfigManager::Save()
{
	for (ConfigEntryBase* entry : entries)
	{
		entry->Refresh();
	}
	
	std::ofstream file(this->path);

	if (file.is_open())
	{
		const std::string dump = this->json.dump(1, '\t');
		file.write(dump.c_str(), dump.size());
	}
	else
	{
		throw std::exception("File did not open");
	}

	if (file.is_open())
	{
		file.close();
	}
}

json& ConfigManager::Get()
{
	return this->json;
}

ConfigManager* ConfigManager::GetInstance()
{
	if (m_pInstance == nullptr)
		m_pInstance = new ConfigManager();
	return m_pInstance;
}

void ConfigManager::RegisterEntry(ConfigEntryBase* entry)
{
	if (entry != nullptr)
		entries.emplace_back(entry);
}

void ConfigManager::UnregisterEntry(ConfigEntryBase* entry)
{
	entries.remove(entry);
}

ConfigManager::ConfigManager()
{
	m_pInstance = this;
}

ConfigManager* ConfigManager::m_pInstance = nullptr;