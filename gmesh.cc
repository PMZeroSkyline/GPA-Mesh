#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <filesystem>
namespace fs = std::filesystem;
using namespace std;
vector<string> Split(const string &src, char tag)
{
    vector<string> out;
    size_t it = 0;
    size_t pos;
    while ((pos = src.find(tag, it)) != string::npos)
    {
        out.push_back(src.substr(it, pos-it));
        it = pos+1;
    }
    out.push_back(src.substr(it, src.size()-it));
    return out;
}
vector<vector<string>> ReadCSV(const string &path)
{
    vector<vector<string>> csv;
    ifstream f(path);
    string line;
    while (getline(f, line))
    {
        csv.push_back(move(Split(line, ',')));
    }
    f.close();
    return csv;
}
struct vec4
{
    float x = 0.f, y = 0.f, z = 0.f, w = 0.f;
};
vector<vec4> GetCSVColumn(const vector<vector<string>>& csv, const string& columnName)
{
    vector<vec4> out;
    int columnId = -1;
    for (int i = 0; i < csv[0].size(); i++)
    {
        if (csv[0][i] == columnName)
        {
            columnId = i;
        }
    }
    if (columnId == -1)
    {
        return out;
    }
    int vertId = 0;
    int offset = 0;
    vec4 v;
    for (int i = 1; i < csv.size(); i++)
    {
        int id = stoi(csv[i][0]);
        if (id != vertId)
        {
            out.push_back(v);
            vertId = id;
            offset = 0;
        }
        if (offset < 4)
        {
            string strValue = csv[i][columnId];
            if (strValue.size())
            {
                *(((float*)&v) + offset) = stof(csv[i][columnId]);
            }
        }
        offset++;
    }
    out.push_back(v);
    return out;
}
void ExportWithTexcoord(const string& geometryPath, const string& vbvPath, const string& columnName, const string& outputPath)
{
    ifstream geoFile(geometryPath);
    if (!geoFile.is_open())
    {
        cout << "Read geometry file failed : " << geometryPath << endl;
        return;
    }
    vector<string> lines;
    string line;
    while (getline(geoFile, line))
    {
        lines.push_back(line);
    }
    geoFile.close();

    vector<vector<string>> csv = ReadCSV(vbvPath);
    vector<vec4> uvColumn = GetCSVColumn(csv, columnName);
    if (uvColumn.size() == 0)
    {
        cout << "Failed to find columnName '" << columnName << "' in '" << vbvPath << "' file" << endl;
        return;
    }
    vector<string> outLines;
    for (int i = 0; i < lines.size(); i++)
    {
        if (lines[i][0] == 'f')
        {
            break;
        }
        outLines.push_back(lines[i]);
    }
    for (int i = 0; i < uvColumn.size(); i++)
    {
        vec4& v = uvColumn[i];
        string newLine = "vt " + to_string(v.x) + " " + to_string(v.y);
        outLines.push_back(newLine);
    }
    for (int i = 0; i < lines.size(); i++)
    {
        if (lines[i][0] != 'f')
        {
            continue;
        }
        outLines.push_back(lines[i]);
    }
    lines = move(outLines);
    for (int i = 0; i != outLines.size(); i++)
    {
        if (outLines[i][0] != 'f')
        {
            continue;
        }
        vector<string> words = Split(outLines[i], ' ');
        string updateFaceLine = "f ";
        for (int j = 1; j != words.size(); j++)
        {
            vector<string> ids = Split(words[j], '/');
            updateFaceLine += ' ' + ids[0] + '/' + ids[0] + '/' + ids[0];
        }
        outLines[i] = updateFaceLine;
    }
    if (fs::path(outputPath).parent_path().string() != "" && !fs::exists(fs::path(outputPath).parent_path()))
    {
        fs::create_directories(fs::path(outputPath).parent_path());
    }
    ofstream outputFile(outputPath);
    if (!outputFile.is_open())
    {
        cout << "Write output file failed : " << outputPath << endl;
        return;
    }
    for (const string& l : outLines)
    {
        outputFile << l << endl;
    }
    outputFile.close();
    cout << "Write finished. output path : " << outputPath << endl;
}
struct RequiredInputs
{
    string geometryPath = "";
    string vbvPath = "";
    string columnName = "";
    string outputPath = "";
    bool Check()
    {
        if (geometryPath == "")
        {
            cout << "Read input failed : geometryPath is null ? " << vbvPath << endl;
            return false;
        }
        if (geometryPath.find(".obj") == string::npos)
        {
            cout << "Read input failed : geometryPath need is .obj file path! (" << geometryPath << ")" << endl;
            return false;
        }
        if (vbvPath == "")
        {
            cout << "Read input failed : vbvPath is null ? " << geometryPath << endl;
            return false;
        }
        if (vbvPath.find(".csv") == string::npos)
        {
            cout << "Read input failed : vbvPath need is .csv file path! (" << vbvPath << ")" << endl;
            return false;
        }
        if (columnName == "")
        {
            cout << "Read input failed : columnName is null ?" << endl;
            return false;
        }
        if (outputPath == "")
        {
            cout << "Read input failed : outputPath is null ? geometryPath " << geometryPath << ", vbvPath : " << vbvPath << endl;
            return false;
        }
        if (outputPath.find(".obj") == string::npos)
        {
            cout << "Read input failed : outputPath need is .obj file path! (" << outputPath << ")" << endl;
            return false;
        }
        cout << "Read input success";
        cout << ": geometryPath: " << geometryPath;
        cout << "; vbvPath: " << vbvPath;
        cout << "; columnName: " << columnName;
        cout << "; outputPath: " << outputPath << endl;
        return true;
    }
};
void PrintHelp()
{
    cout << "## 1. Extract a single model" << endl;
    cout << "\tcommand \t: gmesh -s TEXCOORD1 D:/my_buffer.csv D:/my_mesh.obj D:/out_mesh.obj" << endl;
    cout << "\tdescription \t: -s [texcoord column name] [vbv.csv path] [mesh.obj path] [output path]" << endl;
    cout << "\t\t\t- mesh.obj path (It should be the mesh file '.obj' exported from GPA)" << endl;
    cout << "\t\t\t- vbv.csv path (It should be the vertex buffer vector file '.csv' exported from GPA)" << endl;
    cout << "\t\t\t- texcoord column name (specific name for UV in VBV file)" << endl;
    cout << "## 2. Extract multiple models" << endl;
    cout << "\tcommand \t: gmesh -m TEXCOORD1 D:/mesh_and_vbv" << endl;
    cout << "\tdescription \t: -m [texcoord column name] [mesh and vbv files folder]" << endl;
    cout << "\t\t\t- mesh.obj and vbv.csv files folder (the mesh file and vbv file name must be the same, e.g. ground.obj & ground.csv)" << endl;
}
void PrintParameterNotMatch()
{
    cout << "\nThe input parameters do not match, enter 'gmesh -help' to see the parameters information." << endl;
}
int main(int argc, char** argv)
{
    if (argc < 2)
    {
        PrintParameterNotMatch();
        return -1;
    }
    string fc = argv[1];
    if (fc == "-help" && argc == 2)
    {
        PrintHelp();
    }
    else if (fc == "-s" && argc == 6) // Single obj convert
    {
        RequiredInputs i;
        i.columnName = argv[2];
        i.vbvPath = argv[3];
        i.geometryPath = argv[4];
        i.outputPath = argv[5];
        if (!i.Check())
        {
            return -1;
        }
        ExportWithTexcoord(i.geometryPath, i.vbvPath, i.columnName, i.outputPath);
    }
    else if (fc == "-m" && argc == 4) // Multiple obj convert
    {
        string columnName = argv[2];
        string folderDir = argv[3];
        vector<fs::path> paths;
        for (const auto& entry : fs::directory_iterator(folderDir)) 
	    {
            if (fs::is_regular_file(entry)) 
	    	{
	    		paths.push_back(entry.path());
            } 
        }
        map<string, RequiredInputs> executions;
        for (auto& path : paths)
        {
            if (path.extension() == ".obj")
            {
                executions[path.stem().string()].geometryPath = path.string();
                executions[path.stem().string()].outputPath = path.parent_path().string() + "/output/" + path.filename().string();
            }
            if (path.extension() == ".csv")
            {
                executions[path.stem().string()].vbvPath = path.string();
                executions[path.stem().string()].columnName = columnName;
            }
        }
        for (auto& exec : executions)
        {
            RequiredInputs& i = exec.second;
            if (i.Check())
            {
                ExportWithTexcoord(i.geometryPath, i.vbvPath, i.columnName, i.outputPath);
            }
        }
    }
    else
    {
        PrintParameterNotMatch();
        return -1;
    }
    return 0;
}