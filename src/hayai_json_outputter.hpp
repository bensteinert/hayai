#include "hayai_outputter.hpp"
#include "hayai_console.hpp"
#include <vector>
#include <string>
#include <cstddef>
#include <sstream>
#include <Python/Python.h>


namespace hayai
{
#define WRITE_PROPERTY(stream, key, value, indent) {       \
    stream << std::setw(indent*4) << "\"" << key << "\": \"" << value << "\"," << std::endl;      \
}

#define WRITE_PROPERTY_NO_COMMA(stream, key, value, indent) {                                \
    stream << std::setw(indent*4) << "\"" << key << "\": \"" << value << "\"" << std::endl;      \
}

#define WRITE_DELIMITER(stream, sign, indent) {                                \
    stream << std::setw(indent*4) << sign << std::endl;      \
}

    class JsonOutputter
        :   public Outputter {

    private:
        bool initialWrite;
        FILE *jsonFile;

    public:
        virtual void Begin(const std::size_t& enabledCount, const std::size_t& disabledCount) {
            initialWrite = true;
            jsonFile = fopen("./result.json", "w");
            if (!jsonFile) {
                return;
            }
            fprintf(jsonFile, "[\n");
        }


        virtual void End(const std::size_t& enabledCount, const std::size_t& disabledCount) {
            if (!jsonFile) {
                return;
            }
            fprintf(jsonFile, "]\n");
            fclose(jsonFile);
        }


        virtual void BeginTest(const std::string& fixtureName,
                const std::string& testName,
                const std::string& parameters,
                const std::size_t& runsCount,
                const std::size_t& iterationsCount) {
            if (!jsonFile) {
                return;
            }

            std::stringstream stream("");

            if (initialWrite) {
                initialWrite = false;
            } else {
                WRITE_DELIMITER(stream, ",", 1);
            }

            WRITE_DELIMITER(stream, "{", 1);

            WRITE_PROPERTY(stream, "group", fixtureName, 2);
            WRITE_PROPERTY(stream, "name", testName, 2);
            WRITE_PROPERTY(stream, "benchmarkParameters", parameters, 2);
            WRITE_PROPERTY(stream, "numberOfRuns", runsCount, 2);
            WRITE_PROPERTY(stream, "numberOfIterationsPerRun", iterationsCount, 2);


            const std::string &jsonBegin = stream.str();
            fwrite(jsonBegin.c_str(), 1, jsonBegin.length(), jsonFile);
        }


        virtual void EndTest(const std::string& fixtureName,
                const std::string& testName,
                const std::string& parameters,
                const TestResult& result) {
            if (!jsonFile) {
                return;
            }

            std::stringstream stream("");

            WRITE_PROPERTY(stream, "totalTime", result.TimeTotal(), 2);
            WRITE_PROPERTY(stream, "averagePerRun", result.RunTimeAverage(), 2);
            WRITE_PROPERTY(stream, "fastestRun", result.RunTimeMinimum(), 2);
            WRITE_PROPERTY_NO_COMMA(stream, "slowestRun", result.RunTimeMaximum(), 2);

            WRITE_DELIMITER(stream, "}", 1);

            const std::string &listEntry = stream.str();

            fwrite(listEntry.c_str(), 1, listEntry.length(), jsonFile);
        }

        virtual void SkipDisabledTest(const std::string& fixtureName,
                const std::string& testName,
                const std::string& parameters,
                const std::size_t& runsCount,
                const std::size_t& iterationsCount) {

        };
    };
}
