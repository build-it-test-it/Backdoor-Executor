/*
 * Minimal JSON header for GeneralAssistantModel.mm compilation
 */
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace Json {
    // Forward declarations of Json classes
    class Value;
    class Reader;
    class Writer;
    
    // Basic Json Value class
    class Value {
    public:
        enum ValueType {
            nullValue = 0,
            intValue,
            uintValue,
            realValue,
            stringValue,
            booleanValue,
            arrayValue,
            objectValue
        };
        
        Value() {}
        Value(const std::string& value) {}
        Value(int value) {}
        Value(double value) {}
        Value(bool value) {}
        
        bool isNull() const { return true; }
        bool isBool() const { return false; }
        bool isInt() const { return false; }
        bool isUInt() const { return false; }
        bool isDouble() const { return false; }
        bool isString() const { return false; }
        bool isArray() const { return false; }
        bool isObject() const { return false; }
        
        int asInt() const { return 0; }
        unsigned int asUInt() const { return 0; }
        double asDouble() const { return 0.0; }
        bool asBool() const { return false; }
        std::string asString() const { return ""; }
        
        Value& operator[](const std::string& key) { return *this; }
        Value& operator[](int index) { return *this; }
        
        // Object operations
        bool isMember(const std::string& key) const { return false; }
        Value get(const std::string& key, const Value& defaultValue) const { return defaultValue; }
        
        // Array operations
        Value get(int index, const Value& defaultValue) const { return defaultValue; }
        unsigned int size() const { return 0; }
        
        // Member iteration
        std::vector<std::string> getMemberNames() const { return {}; }
    };
    
    // Basic Json Reader class
    class Reader {
    public:
        bool parse(const std::string& json, Value& root, bool collectComments = true) { return true; }
    };
    
    // FastWriter for quick string conversion
    class FastWriter {
    public:
        std::string write(const Value& root) { return "{}"; }
    };
    
    // StyledWriter for pretty printing
    class StyledWriter {
    public:
        std::string write(const Value& root) { return "{}"; }
    };
}
