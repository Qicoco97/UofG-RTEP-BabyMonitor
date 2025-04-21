// INIReader.h - Simple and easy-to-use INI file reader
// Project homepage: https://github.com/benhoyt/inih
// License: New BSD license

#ifndef __INIREADER_H__
#define __INIREADER_H__

#include <map>
#include <string>

// Reads an INI file into a map of section/name to value.
// Simple and lightweight with good performance.
class INIReader
{
public:
    // Constructs the reader and parses the given INI file.
    explicit INIReader(const std::string& filename);

    // Returns 0 if parsing succeeded, or the line number of the first error,
    // or -1 if the file could not be opened.
    int ParseError() const;

    // Gets a string value from the INI file, or returns default_value if not found.
    std::string Get(const std::string& section,
                    const std::string& name,
                    const std::string& default_value) const;

    // Gets an integer (supports decimal and hex formats), or returns default_value if not found or invalid.
    long GetInteger(const std::string& section,
                    const std::string& name,
                    long default_value) const;

    // Gets a floating-point number, or returns default_value if not found or invalid.
    double GetReal(const std::string& section,
                   const std::string& name,
                   double default_value) const;

    // Gets a boolean value (true/false, yes/no, on/off, 1/0), case-insensitive.
    // Returns default_value if not found or invalid.
    bool GetBoolean(const std::string& section,
                    const std::string& name,
                    bool default_value) const;

private:
    int _error;
    std::map<std::string, std::string> _values;

    // Creates a lower-case key from section and name for case-insensitive lookup.
    static std::string MakeKey(const std::string& section, const std::string& name);

    // Callback used by ini_parse to store values into the map.
    static int ValueHandler(void* user,
                            const char* section,
                            const char* name,
                            const char* value);
};

#endif // __INIREADER_H__
