#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <ItemAssistantCore/ItemAssistantCore.h>
#include <map>
#include <fstream>
#include <Shared/Singleton.h>
#include <Shared/UnicodeSupport.h>


namespace aoia {

    class ITEMASSISTANTCORE_API SettingsManager
    {
        SINGLETON(SettingsManager)
    public:
        ~SettingsManager();

        /// Gets the value associated with the specified key. Returns empty string if unknown key.
        std::tstring getValue(std::tstring const& key) const;

        /// Assigns a value to the specified key.
        void setValue(std::tstring const& key, std::tstring const& value);

        /// Reads in settings from a specified file.
        void readSettings(std::tstring const& filename);

        /// Writes current settings to the specified file.
        void writeSettings(std::tstring const& filename);

    private:
        std::map<std::tstring, std::tstring> m_values;
    };
}

#endif // SETTINGSMANAGER_H
