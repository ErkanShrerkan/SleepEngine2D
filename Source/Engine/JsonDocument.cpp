#include "pch.h"
#include "JsonDocument.h"
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/writer.h>
#include <rapidjson/pointer.h>
#include <rapidjson/prettywriter.h>
#include <filesystem>

JsonDocument::JsonDocument()
{
}

bool JsonDocument::ParseFile(const std::string& aJsonFile)
{
    //std::filesystem::path dataPath(std::filesystem::current_path() / L"Data");
    //std::filesystem::create_directory(dataPath);
    std::string jsonPath(/*dataPath.string() + "/" +*/ aJsonFile);
    std::ifstream t(jsonPath);
    std::stringstream buffer;
    buffer << t.rdbuf();
    t.close();

    std::string content = buffer.str();
    rapidjson::StringStream stream(content.c_str());
    bool error = myDocument.ParseStream(stream).HasParseError();

    printf("<%s> Parse error at offset %u: \"%s\", when loading file\n",
        "JsonDocument::JsonDocument",
        (unsigned)myDocument.GetErrorOffset(),
        rapidjson::GetParseError_En(myDocument.GetParseError())
    );

    if (error)
    {
        myDocument.SetObject();
    }

    return error;
}

void JsonDocument::SaveToFile(const std::string& aJsonFile, bool aSavePretty)
{
    rapidjson::StringBuffer buffer;
    if (aSavePretty)
    {
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        myDocument.Accept(writer);
    }
    else
    {
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        myDocument.Accept(writer);
    }
   
    std::ofstream file{ std::string(aJsonFile) };
    file << buffer.GetString();
    file.close();
}

void JsonDocument::SetUInt(const std::string& aSource, uint aValue)
{
    rapidjson::Pointer(aSource.c_str()).Set(myDocument, aValue);
}

uint JsonDocument::GetUInt(const std::string& aSource)
{
    auto jint = rapidjson::Pointer(aSource.c_str()).Get(myDocument);
    return jint ? jint->GetUint() : UINT_MAX;
}

void JsonDocument::SetFloat(const std::string& aSource, float aValue)
{
    rapidjson::Pointer(aSource.c_str()).Set(myDocument, aValue);
}

float JsonDocument::GetFloat(const std::string& aSource, float aDefaultValue)
{
    auto jfloat = rapidjson::Pointer(aSource.c_str()).Get(myDocument);
    return jfloat ? jfloat->GetFloat() : aDefaultValue;
}

void JsonDocument::SetBool(const std::string& aSource, bool aValue)
{
    rapidjson::Pointer(aSource.c_str()).Set(myDocument, aValue);
}

bool JsonDocument::GetBool(const std::string& aSource, bool aDefaultValue)
{
    auto jbool = rapidjson::Pointer(aSource.c_str()).Get(myDocument);
    return jbool ? jbool->GetBool() : aDefaultValue;
}
