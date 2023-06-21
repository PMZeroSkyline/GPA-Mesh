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
vector<vector<string>> ParseCSV(const string &path)
{
    vector<vector<string>> csv;
    ifstream f(path);
    string line;
    while (getline(f, line))
    {
        csv.push_back(move(Split(line, ',')));
    }
    return csv;
}
struct vec4
{
    float x = 0.f, y = 0.f, z = 0.f, w = 0.f;
};
vector<vec4> GetVBVColumn(const vector<vector<string>>& csv, const string& columnName)
{
    vector<vec4> out;
    int it = -1;
    for (int i = 0; i < csv[0].size(); i++)
    {
        if (csv[0][i] == columnName)
        {
            it = i;
        }
    }
    if (it == -1)
    {
        return out;
    }
    int lastLineID = 0;
    int offset = 0;
    vec4 v;
    for (int i = 1; i < csv.size(); i++)
    {
        int lineID = stoi(csv[i][0]);
        if (lineID != lastLineID)
        {
            out.push_back(v);
            lastLineID = lineID;
            offset = 0;
        }
        if (offset < 4)
        {
            string strValue = csv[i][it];
            if (strValue.size())
            {
                *(((float*)&v) + offset) = stof(csv[i][it]);
            }
        }
        offset++;
    }
    out.push_back(v);
    return out;
}
void EnsureParentFolder(string targetPath)
{
    fs::path path = targetPath;
    bool isExists = fs::exists(path.parent_path()) && fs::is_directory(path.parent_path());
    if (!isExists)
    {
        fs::create_directories(path.parent_path());
    }
}
void InsertTexcoordExportOBJ(const string& geometryPath, const string& vbvPath, const string& columnName, const string& outputPath)
{
    // read .obj file to lines
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

    // insert uv
    vector<vector<string>> csv = move(ParseCSV(vbvPath));
    vector<vec4> uvColumn = GetVBVColumn(csv, columnName);
    vector<string> newLines;
    for (int i = 0; i < lines.size(); i++)
    {
        if (lines[i][0] == 'f')
        {
            break;
        }
        newLines.push_back(lines[i]);
    }
    for (int i = 0; i < uvColumn.size(); i++)
    {
        vec4& v = uvColumn[i];
        string newLine = "vt " + to_string(v.x) + " " + to_string(v.y);
        newLines.push_back(newLine);
    }
    for (int i = 0; i < lines.size(); i++)
    {
        if (lines[i][0] != 'f')
        {
            continue;
        }
        newLines.push_back(lines[i]);
    }
    lines = move(newLines);

    // fill .obj file 'f' line
    for (int i = 0; i != lines.size(); i++)
    {
        if (lines[i][0] != 'f')
        {
            continue;
        }
        vector<string> words = move(Split(lines[i], ' '));
        string newLine = "f ";
        for (int j = 1; j != words.size(); j++)
        {
            vector<string> ids = move(Split(words[j], '/'));
            newLine += ' ' + ids[0] + '/' + ids[0] + '/' + ids[0];
        }
        lines[i] = newLine;
    }

    // output
    EnsureParentFolder(outputPath);
    ofstream outputFile(outputPath);
    for (const string& l : lines)
    {
        outputFile << l << endl;
    }

    cout << "we done. output path : " << outputPath << endl;
}
void GetFolderFiles(const string& path, vector<fs::path> &result, bool isRecursive = true)
{
    for (const auto& entry : fs::directory_iterator(path)) 
	{
        if (fs::is_regular_file(entry)) 
		{
			result.push_back(entry.path());
        } 
		if (isRecursive)
		{
			if (fs::is_directory(entry)) 
			{
        	    GetFolderFiles(entry.path().string(), result, isRecursive);
        	}
		}
    }
}
struct RequiredInputs
{
    string geometryPath = "";
    string vbvPath = "";
    string columnName = "";
    string outputPath = "";
    bool Check(bool isPrintLog = true)
    {
        if (geometryPath == "")
        {
            if (isPrintLog)
            {
                if (vbvPath == "")
                {
                    cout << "Read input failed : geometryPath is null ?" << endl;
                }
                else
                {
                    cout << "Read input failed : geometryPath is null?, vbvPath : " << vbvPath << endl;
                }
            }
            return false;
        }
        if (geometryPath.find(".obj") == string::npos)
        {
            if (isPrintLog)
            {
                cout << "Read input failed : geometryPath need is .obj file path! (" << geometryPath << ")" << endl;
            }
            return false;
        }
        if (vbvPath == "")
        {
            if (isPrintLog)
            {
                cout << "Read input failed : vbvPath is null ? geometryPath : " << geometryPath << endl;
            }
            return false;
        }
        if (vbvPath.find(".csv") == string::npos)
        {
            if (isPrintLog)
            {
                cout << "Read input failed : vbvPath need is .csv file path! (" << vbvPath << ")" << endl;
            }
            return false;
        }
        if (columnName == "")
        {
            if (isPrintLog)
            {
                cout << "Read input failed : columnName is null ?" << endl;
            }
            return false;
        }
        if (outputPath == "")
        {
            if (isPrintLog)
            {
                cout << "Read input failed : outputPath is null ? geometryPath " << geometryPath << ", vbvPath : " << vbvPath << endl;
            }
            return false;
        }
        if (outputPath.find(".obj") == string::npos)
        {
            if (isPrintLog)
            {
                cout << "Read input failed : outputPath need is .obj file path! (" << outputPath << ")" << endl;
            }
            return false;
        }
        if (isPrintLog)
        {
            cout << "\nRead input success";
            cout << ": geometryPath: " << geometryPath;
            cout << "; vbvPath: " << vbvPath;
            cout << "; columnName: " << columnName;
            cout << "; outputPath: " << outputPath << "\n" << endl;
        }
        return true;
    }
};
void PrintHelp()
{
    cout << "\n\n## 1. Convert a single model" << endl;
    cout << "\nTo insert UV data to a single model and export it, need to enter the model path + UV Buffer file path + the column name that represents UV in the UV Buffer + path to the exported model" << endl;
    cout << "\ncommand :-s [geometry.obj path] [vbv.csv path] [texcoord column name] [output path]" << endl;
    cout << "\n- geometry.obj path\n" << endl;
    cout << "It should be the obj model exported from GPA" << endl;
    cout << "\n- vbv.csv path" << endl;
    cout << "It should be the VBV file (.csv) exported from GPA, usually there will be more than one VBV in one DrawCall of GPA, please pay attention to which VBV file the UV information exists, we need to use the VBV with UV information." << endl;
    cout << "\n- texcoord column name" << endl;
    cout << "Usually there is no specific name for UV in VBV, please select a column of data in VBV as UV information, usually this column will be called TEXCOORD / TEXCOORD0 / TEXCOORD* / float2, the value of this column is characterized by only x and y values, and are between 0 and 1, cut are floating point numbers, need to screen the name of this UV column yourself What is then passed in, the name can be seen in the first line of the data displayed in GPA by clicking VBV, the general layout format is index columnName1 columnName2 ..." << endl;
    cout << "\n- output path" << endl;
    cout << "Path to export model" << endl;
    cout << "\nExample (in cmd):" << endl;
    cout << "```" << endl;
    cout << "gmesh D:/my_mesh.obj D:/my_buffer.csv TEXCOORD D:/my_new_mesh.obj" << endl;
    cout << "```" << endl;
    cout << "\n\n## 2. Convert multiple models" << endl;
    cout << "\nTo batch insert UV datas to models and export, need to enter the path to store all of the model and UVBuffer files folder path + the column name that represents UV in the UV Buffer, note: the model file and UV Buffer file name must be the same, e.g. head.obj & head.csv" << endl;
    cout << "\ncommand : -m [geometry and vbv files folder] [texcoord column name]" << endl;
    cout << "\n- geometry and vbv files folder" << endl;
    cout << "store all of the model and UVBuffer files folder path. note: the model file and UV Buffer file name must be the same, e.g. head.obj & head.csv" << endl;
    cout << "\n- texcoord column name" << endl;
    cout << "Usually there is no specific name for UV in VBV, please select a column of data in VBV as UV information, usually this column will be called TEXCOORD / TEXCOORD0 / TEXCOORD* / float2, the value of this column is characterized by only x and y values, and are between 0 and 1, cut are floating point numbers, need to screen the name of this UV column yourself What is then passed in, the name can be seen in the first line of the data displayed in GPA by clicking VBV, the general layout format is index columnName1 columnName2 ..." << endl;
    cout << "\nExample (in cmd):" << endl;
    cout << "```" << endl;
    cout << "gmesh D:/my_gpa_files TEXCOORD" << endl;
    cout << "```" << endl;

    
}
int main(int argc, char** argv)
{
    if (argc < 2)
    {
        PrintHelp();
        return -1;
    }
    string fc = argv[1];
    if (fc == "-s" && argc == 6)
    {
        // Single obj convert
        RequiredInputs i;
        i.geometryPath = argv[2];
        i.vbvPath = argv[3];
        i.columnName = argv[4];
        i.outputPath = argv[5];
        if (!i.Check())
        {
            return -1;
        }
        InsertTexcoordExportOBJ(i.geometryPath, i.vbvPath, i.columnName, i.outputPath);
    }
    else if (fc == "-m" && argc == 4)
    {
        // Multiple obj convert
        string folderDir = argv[2];
        string columnName = argv[3];

        vector<fs::path> paths;
        GetFolderFiles(argv[2], paths, false);

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
                InsertTexcoordExportOBJ(i.geometryPath, i.vbvPath, i.columnName, i.outputPath);
            }
        }
    }
    else
    {
        PrintHelp();
        return -1;
    }
    return 0;
}