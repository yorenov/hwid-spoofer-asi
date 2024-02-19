#pragma once

#include "utils/other/json.hpp"
#include <list>

constexpr char CFG_FILENAME[] = PROJECT_NAME_STRICT ".json";

template<class T>
nlohmann::json& GetOrDefault(nlohmann::json& json, const char* key, T def)
{
	if (!json.contains(key))
	{
		json[key] = def;
	}

	return json[key];
}

template<class T>
std::optional<T> GetOrDefault(nlohmann::json& json, const char* key, std::optional<T> def)
{
	if (!json.contains(key))
	{
		return def;
	}

	return json[key].get<T>();
}


class ConfigEntryBase // Only for refresh function!
{
public:	
	virtual void Refresh() {}
};

class ConfigManager
{
	nlohmann::json json;
	std::filesystem::path path;
	std::list<ConfigEntryBase*> entries;

	static ConfigManager* m_pInstance;	
public:
	ConfigManager();

	bool Load();
	void Save();

	nlohmann::json& Get();

	static ConfigManager* GetInstance();
	
	void RegisterEntry(ConfigEntryBase* entry);
	void UnregisterEntry(ConfigEntryBase* entry);
};

template<class Ty>
class ConfigEntry : public ConfigEntryBase
{
	ConfigManager* manager;
	std::string id;
	Ty value;
	
public:
	ConfigEntry() = delete;
	ConfigEntry(const std::string& entry, Ty defaultValue)
	{
        id = entry;
        value = defaultValue;
	}
	
	~ConfigEntry()
	{
		if (manager != nullptr)
			manager->UnregisterEntry(this);
	}

	void Register(ConfigManager* config)
	{
        manager = config;
		manager->RegisterEntry(this);

		if (!manager->Get().contains(id))
			this->Refresh();
		else
            value = manager->Get().at(id).get<Ty>();
	}

	Ty& Get()
	{
		return value;
	}
	void Refresh() override
	{
        manager->Get()[id] = value;
	}
	
	Ty& operator*()
	{
		return this->Get();
	}
	Ty& operator=(Ty second)
	{
        value = second;
		return this->Get();
	}
	
	ConfigEntry& operator=(ConfigEntry& second) = delete; // Class replacing is not allowed. Define -> register -> use;
};
