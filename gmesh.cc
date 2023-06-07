#include <iostream>
#include <string>
#include <fstream>
#include <vector>
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

int main(int argc, char** argv)
{
    // if (argc != 5)
    // {
    //     cout << "command : [geometry.obj path] [vbv.csv path] [column name] [output path]" << endl;
    // }
    // string geometryPath = argv[1];
    // string vbvPath = argv[2];
    // string columnName = argv[3];
    // string outputPath = argv[4];

    // grd.obj grd.csv TEXCOORD ground.obj
    string geometryPath = "grd.obj";
    string vbvPath = "grd.csv";
    string columnName = "TEXCOORD";
    string outputPath = "ground.obj";


    // read .obj file to lines
    ifstream geoFile(geometryPath);
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
    ofstream outputFile(outputPath);
    for (const string& l : lines)
    {
        outputFile << l << endl;
    }

    cout << "we done. output path : " << outputPath << endl;
}