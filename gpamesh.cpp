/**
*   zeroskyline 2021/10/30
*/
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>

std::string VBVFilePath, IBVFilePath, meshFilePath;
std::vector<std::string> splitVBVFilePath;
int indexCount = 0;
int startIndexLocation = 0;
int lineIter = 0;
int lastVertIndex = 0;
int lastTriangleIndex = 0;
int attrIter = 0;
int positionLineIter = -1;
int normalLineIter = -1;
int tangentLineIter= -1;
int texcoordLineIter = -1;
std::string lineBuffer;
std::vector<std::string> splitLineBuffer;
std::array<float, 2> texcoordContainer;
std::array<float, 3> positionContainer;
std::array<float, 3> normalContainer;
std::array<float, 4> tangentContainer;
std::array<int, 3> triangleContainer;
std::vector<std::array<float, 3>> POSITION;
std::vector<std::array<float, 3>> NORMAL;
std::vector<std::array<float, 4>> TANGENT;
std::vector<std::array<float, 2>> TEXCOORD;
std::vector<std::array<int,3>> TRIANGLE;

// utility functions
void SaveMesh(std::string saveMeshFilePath)
{
    std::ofstream saveMeshFile(saveMeshFilePath);
    for(int i = 0; i != POSITION.size(); i++)
    {
        saveMeshFile << "v";
        for(int j = 0; j != 3; j++)
        {
            saveMeshFile << " " << POSITION[i][j];
        }
        saveMeshFile << std::endl;
    }
    for(int i = 0; i != NORMAL.size(); i++)
    {
        saveMeshFile << "vn";
        for(int j = 0; j != 3; j++)
        {
            saveMeshFile << " " << NORMAL[i][j];
        }
        saveMeshFile << std::endl;
    }
    for(int i = 0; i != TEXCOORD.size(); i++)
    {
        saveMeshFile << "vt";
        for(int j = 0; j != 2; j++)
        {
            saveMeshFile << " " << TEXCOORD[i][j];
        }
        saveMeshFile << std::endl;
    }
    for(int i = 0; i != TRIANGLE.size(); i++)
    {
        saveMeshFile << "f";
        for(int j = 0; j !=3; j++)
        {
            int vertID = TRIANGLE[i][j];
            saveMeshFile << " " << vertID << "/" << vertID << "/" << vertID;
        }
        saveMeshFile << std::endl;
    }
    saveMeshFile.close();
    system("cls");
    std::cout << R"(               __)" << std::endl;
    std::cout << R"(    ..=====.. |==|)" << std::endl;
    std::cout << R"(    ||     || |= |)" << std::endl;
    std::cout << R"( _  ||     || |^*| _)" << std::endl;
    std::cout << R"(|=| o=,===,=o |__||=|)" << std::endl;
    std::cout << R"(|_|  _______)~`)  |_|)" << std::endl;
    std::cout << R"(    [=======]  ())" << std::endl;
    std::cout << "\n\n\n\nwe done!";
    std::cout << "\n\n\n\nThe file are saved in : \n\n        " << saveMeshFilePath << "\n\n\n\n";
    system("PAUSE");
    system("gpamesh");
}
void LogError(std::string message)
{
    system("color 74");
    system("cls");
    std::cout << "\n\n\n\n" << message << "\n\n\n\n" << std::endl;
    system("PAUSE");
    system("gpamesh");
}
std::vector<std::string> SplitLine(std::string lineBuffer, char tag){
    std::vector<std::string> nameList;
    std::string name;
    for(auto a : lineBuffer){
        if(a == tag){
            nameList.push_back(name);
            name = "";
        }
        else{
            name += a;
        }
    }
    nameList.push_back(name);
    return nameList;
}
std::string ReplaceAll(std::string targetStr, std::string searchStr, std::string replaceStr)
{
    int pos = targetStr.find(searchStr);
    while (pos != -1)
    {
        targetStr.replace(pos, searchStr.length(), replaceStr);
        pos = targetStr.find(searchStr);
    }
    return targetStr;
}
std::string FindDirectoryPath(std::string filePath)
{
    std::string path = ReplaceAll(filePath, "/", R"(\)");
    int pos = path.find_last_of(R"(\)");
    if(pos == -1)
    {
        LogError("File directory not legal : " + path);
        return "";
    }
    std::string dir(path.begin(), path.begin() + pos + 1);
    return dir;
}

/**
*   __main__ 
*/
void main(int argc, char *argv[]){



    // get vbv data

    system("cls");
    system("color F2");
    std::cout << R"(  __________________  _____                          .__     )" << std::endl;
    std::cout << R"( /  _____/\______   \/  _  \     _____   ____   _____|  |__  )" << std::endl;
    std::cout << R"(/   \  ___ |     ___/  /_\  \   /     \_/ __ \ /  ___/  |  \ )" << std::endl;
    std::cout << R"(\    \_\  \|    |  /    |    \ |  Y Y  \  ___/ \___ \|   Y  \)" << std::endl;
    std::cout << R"( \______  /|____|  \____|__  / |__|_|  /\___  >____  >___|  /)" << std::endl;
    std::cout << R"(        \/                 \/        \/     \/     \/     \/ )" << std::endl;
    std::cout << "\n\n\n\nDrag and drop VBV in and press Enter (Multiple files separated by ` , `)" << std::endl;
    std::cout << "\n\n\n\nGPA VBV (.csv) : ";
    std::cin >> VBVFilePath;

    splitVBVFilePath = SplitLine(VBVFilePath, ',');
    for(int fileIter = 0; fileIter != splitVBVFilePath.size(); fileIter++)
    {
        std::ifstream VBVFile(splitVBVFilePath[fileIter].c_str());
        if(!VBVFile)
            LogError("load VBV file failed : " + VBVFilePath);

        lineIter = 0;
        lastVertIndex = 0;
        lastTriangleIndex = 0;
        attrIter = 0;
        positionLineIter = -1;
        normalLineIter = -1;
        tangentLineIter= -1;
        texcoordLineIter = -1;

        while(getline(VBVFile, lineBuffer))
        {        
            splitLineBuffer = SplitLine(lineBuffer, ',');
            if(lineIter == 0)
            {
                for(int i = 0; i != splitLineBuffer.size(); i++)
                {
                    if (splitLineBuffer[i] == "POSITION")
                        positionLineIter = i;
                    if (splitLineBuffer[i] == "NORMAL")
                        normalLineIter = i;
                    if (splitLineBuffer[i] == "TANGENT")
                        tangentLineIter = i;
                    if (splitLineBuffer[i] == "TEXCOORD")
                        texcoordLineIter = i;
                }
            }
            else
            {
                int vertIndex = std::stoi(splitLineBuffer[0]);
                if (lastVertIndex != vertIndex)
                {
                    lastVertIndex = vertIndex;
                    attrIter = 0;
                    if (positionLineIter != -1)
                        POSITION.push_back(positionContainer);
                    if (normalLineIter != -1)
                        NORMAL.push_back(normalContainer);
                    if (tangentLineIter != -1)
                        TANGENT.push_back(tangentContainer);
                    if (texcoordLineIter != -1)
                        TEXCOORD.push_back(texcoordContainer);
                }
                if (positionLineIter != -1 && attrIter < 3)
                    positionContainer[attrIter] = std::atof(splitLineBuffer[positionLineIter].c_str());
                if (normalLineIter != -1 && attrIter < 3)
                    normalContainer[attrIter] = std::atof(splitLineBuffer[normalLineIter].c_str());
                if (tangentLineIter != -1 && attrIter < 4)
                    tangentContainer[attrIter] = std::atof(splitLineBuffer[tangentLineIter].c_str());
                if (texcoordLineIter != -1 && attrIter < 2)
                    texcoordContainer[attrIter] = std::atof(splitLineBuffer[texcoordLineIter].c_str());
                attrIter++;
            }
            lineIter++;
        }
        if (positionLineIter != -1)
            POSITION.push_back(positionContainer);
        if (normalLineIter != -1)
            NORMAL.push_back(normalContainer);
        if (tangentLineIter != -1)
            TANGENT.push_back(tangentContainer);
        if (texcoordLineIter != -1)
            TEXCOORD.push_back(texcoordContainer);
        VBVFile.close();
    }

    // get IBV / Obj data
    
    system("cls");
    std::cout << R"(       __)" << std::endl;
    std::cout << R"(   _  |@@|)" << std::endl;
    std::cout << R"(  / \ \--/ __)" << std::endl;
    std::cout << R"(  ) O|----|  |   __)" << std::endl;
    std::cout << R"( / / \ }{ /\ )_ / _\)" << std::endl;
    std::cout << R"( )/  /\__/\ \__O (__)" << std::endl;
    std::cout << R"(|/  (--/\--)    \__/)" << std::endl;
    std::cout << R"(/   _)(  )(_)" << std::endl;
    std::cout << R"(   `---''---`)" << std::endl;
    std::cout << "\n\n\n\nDrag and drop Geometry or IBV in and press Enter" << std::endl;
    std::cout << "\n\n\n\nGPA Geometry(.obj) / IBV (.csv) : ";
    std::cin >> lineBuffer;
    splitLineBuffer = SplitLine(lineBuffer, '.');

    if(splitLineBuffer[splitLineBuffer.size()-1] == "csv")
    {
        IBVFilePath = lineBuffer;

        std::ifstream IBVFile(IBVFilePath.c_str());
        if(!IBVFile)
            LogError("load IBV file failed : " + IBVFilePath);

        system("cls");
        std::cout << R"(       __)" << std::endl;
        std::cout << R"(   _  |@@|)" << std::endl;
        std::cout << R"(  / \ \--/ __)" << std::endl;
        std::cout << R"(  ) O|----|  |   __)" << std::endl;
        std::cout << R"( / / \ }{ /\ )_ / _\)" << std::endl;
        std::cout << R"( )/  /\__/\ \__O (__)" << std::endl;
        std::cout << R"(|/  (--/\--)    \__/)" << std::endl;
        std::cout << R"(/   _)(  )(_)" << std::endl;
        std::cout << R"(   `---''---`)" << std::endl;
        std::cout << "\n\n\n\nGPA Execution->Arguments DrawIndexed IndexCount" << std::endl;
        std::cout << "\n\n\n\nIndexCount: ";
        std::cin >> indexCount;

        system("cls");
        std::cout << R"(       __)" << std::endl;
        std::cout << R"(   _  |@@|)" << std::endl;
        std::cout << R"(  / \ \--/ __)" << std::endl;
        std::cout << R"(  ) O|----|  |   __)" << std::endl;
        std::cout << R"( / / \ }{ /\ )_ / _\)" << std::endl;
        std::cout << R"( )/  /\__/\ \__O (__)" << std::endl;
        std::cout << R"(|/  (--/\--)    \__/)" << std::endl;
        std::cout << R"(/   _)(  )(_)" << std::endl;
        std::cout << R"(   `---''---`)" << std::endl;
        std::cout << "\n\n\n\nGPA Execution->Arguments DrawIndexed StartIndexLocation" << std::endl;
        std::cout << "\n\n\n\nStartIndexLocation: ";
        std::cin >> startIndexLocation;

        lineIter = 0;
        int count = 0;
        while(getline(IBVFile, lineBuffer))
        {        
            splitLineBuffer = SplitLine(lineBuffer, ',');
            if(lineIter > startIndexLocation && count != indexCount)
            {
                int vertIndex = std::stoi(splitLineBuffer[0]);
                int triangleIndex = vertIndex / 3;
                int triangleVertIndex = vertIndex % 3;
                if(lastTriangleIndex != triangleIndex)
                {
                    lastTriangleIndex = triangleIndex;
                    TRIANGLE.push_back(triangleContainer);
                }
                triangleContainer[triangleVertIndex] = stoi(splitLineBuffer[1]) + 1;
                count++;
            }
            lineIter++;
        }
        TRIANGLE.push_back(triangleContainer);
        IBVFile.close();

        std::string saveMeshFileDir = FindDirectoryPath(splitVBVFilePath[0]);
        std::string saveMeshFileName;
        std::string saveMeshFilePath;

        system("cls");
        std::cout << R"(               __)" << std::endl;
        std::cout << R"(    ..=====.. |==|)" << std::endl;
        std::cout << R"(    ||     || |= |)" << std::endl;
        std::cout << R"( _  ||     || |^*| _)" << std::endl;
        std::cout << R"(|=| o=,===,=o |__||=|)" << std::endl;
        std::cout << R"(|_|  _______)~`)  |_|)" << std::endl;
        std::cout << R"(    [=======]  ())" << std::endl;
        std::cout << "\n\n\n\nSave mesh name : ";
        std::cin >> saveMeshFileName;

        saveMeshFilePath = saveMeshFileDir + saveMeshFileName + ".obj";
        SaveMesh(saveMeshFilePath);
    }
    else if(splitLineBuffer[splitLineBuffer.size()-1] == "obj")
    {
        meshFilePath = lineBuffer;
        std::ifstream MeshFile(meshFilePath.c_str());
        if(!MeshFile)
            LogError("load Obj file failed : " + meshFilePath);
        
        while(getline(MeshFile, lineBuffer))
        {        
            splitLineBuffer = SplitLine(lineBuffer, ' ');
            if(splitLineBuffer[0] == "f")
            {
                for(int i = 0; i != 3; i++)
                {
                    std::vector<std::string> splitTriangle = SplitLine(splitLineBuffer[i + 1], '/');
                    triangleContainer[i] = std::stoi(splitTriangle[0]);
                }
                TRIANGLE.push_back(triangleContainer);
            }
        }
        MeshFile.close();
        SaveMesh(meshFilePath);
    }
    else
    {
        LogError("load Mesh / IBV file failed : " + lineBuffer);
    }

}